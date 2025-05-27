#include "lupine_world_builder.h"
#include "editor/editor_node.h"
#include "editor/editor_data.h"
#include "scene/gui/separator.h"
#include "scene/gui/check_box.h"
#include "scene/gui/spin_box.h"
#include "scene/gui/box_container.h"
#include "core/io/json.h"
#include "core/io/file_access.h"
#include "core/math/math_funcs.h"

// LupineWorldBuilder Implementation

void LupineWorldBuilder::_bind_methods() {
	ClassDB::bind_method(D_METHOD("_on_entity_selected"), &LupineWorldBuilder::_on_entity_selected);
	ClassDB::bind_method(D_METHOD("_on_canvas_clicked"), &LupineWorldBuilder::_on_canvas_clicked);
	ClassDB::bind_method(D_METHOD("_on_node_selected_in_canvas"), &LupineWorldBuilder::_on_node_selected_in_canvas);
	ClassDB::bind_method(D_METHOD("_on_property_changed"), &LupineWorldBuilder::_on_property_changed);

	ADD_SIGNAL(MethodInfo("entity_selected", PropertyInfo(Variant::DICTIONARY, "entity")));
	ADD_SIGNAL(MethodInfo("canvas_clicked", PropertyInfo(Variant::VECTOR2, "position")));
	ADD_SIGNAL(MethodInfo("node_selected", PropertyInfo(Variant::OBJECT, "node")));
	ADD_SIGNAL(MethodInfo("property_changed", PropertyInfo(Variant::STRING, "property"), PropertyInfo(Variant::NIL, "value")));
}

LupineWorldBuilder::LupineWorldBuilder() {
	tool_name = "World Builder";
	tool_description = "Visual scene composition with drag-and-drop entity placement";
	tool_category = "World Design";

	main_split = nullptr;
	left_panel = nullptr;
	right_panel = nullptr;
	entity_palette = nullptr;
	scene_canvas = nullptr;
	property_panel = nullptr;
	current_scene = nullptr;
	selected_node = nullptr;
	selected_entity_type = nullptr;
	is_placing_mode = false;
	snap_to_grid = true;
	grid_size = 32;
}

LupineWorldBuilder::~LupineWorldBuilder() {
	cleanup();
}

void LupineWorldBuilder::initialize() {
	if (tool_panel) {
		return; // Already initialized
	}

	_create_ui();
	_setup_entity_library();

	// Connect signals
	connect("entity_selected", callable_mp(this, &LupineWorldBuilder::_on_entity_selected));
	connect("canvas_clicked", callable_mp(this, &LupineWorldBuilder::_on_canvas_clicked));
	connect("node_selected", callable_mp(this, &LupineWorldBuilder::_on_node_selected_in_canvas));
	connect("property_changed", callable_mp(this, &LupineWorldBuilder::_on_property_changed));
}

void LupineWorldBuilder::cleanup() {
	if (tool_panel && tool_panel->get_parent()) {
		tool_panel->get_parent()->remove_child(tool_panel);
	}

	if (tool_panel) {
		memdelete(tool_panel);
		tool_panel = nullptr;
	}

	main_split = nullptr;
	left_panel = nullptr;
	right_panel = nullptr;
	entity_palette = nullptr;
	scene_canvas = nullptr;
	property_panel = nullptr;
}

void LupineWorldBuilder::activate() {
	LupineEditorTool::activate();

	if (scene_canvas) {
		scene_canvas->set_show_grid(snap_to_grid);
		scene_canvas->set_show_gizmos(true);
	}
}

void LupineWorldBuilder::deactivate() {
	LupineEditorTool::deactivate();
	is_placing_mode = false;
	selected_entity_type = nullptr;
}

void LupineWorldBuilder::_create_ui() {
	// Create main panel
	tool_panel = memnew(Control);
	tool_panel->set_name("WorldBuilder");
	tool_panel->set_anchors_and_offsets_preset(Control::PRESET_FULL_RECT);

	// Create main split container
	main_split = memnew(HSplitContainer);
	main_split->set_anchors_and_offsets_preset(Control::PRESET_FULL_RECT);
	tool_panel->add_child(main_split);

	// Create left panel (entity palette)
	left_panel = memnew(VBoxContainer);
	left_panel->set_custom_minimum_size(Size2(300, 0));
	main_split->add_child(left_panel);

	// Add title for entity palette
	Label *palette_title = memnew(Label);
	palette_title->set_text("Entity Palette");
	palette_title->add_theme_style_override("normal", EditorNode::get_singleton()->get_gui_base()->get_theme_stylebox("panel", "Panel"));
	left_panel->add_child(palette_title);

	// Create entity palette
	entity_palette = memnew(LupineEntityPalette(this));
	left_panel->add_child(entity_palette);

	// Create center container for canvas and right panel
	HSplitContainer *center_split = memnew(HSplitContainer);
	center_split->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	main_split->add_child(center_split);

	// Create scene canvas container
	VBoxContainer *canvas_container = memnew(VBoxContainer);
	canvas_container->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	center_split->add_child(canvas_container);

	// Add canvas title and controls
	HBoxContainer *canvas_header = memnew(HBoxContainer);
	canvas_container->add_child(canvas_header);

	Label *canvas_title = memnew(Label);
	canvas_title->set_text("Scene Canvas");
	canvas_header->add_child(canvas_title);

	// Add grid toggle
	CheckBox *grid_toggle = memnew(CheckBox);
	grid_toggle->set_text("Show Grid");
	grid_toggle->set_pressed(snap_to_grid);
	grid_toggle->connect("toggled", callable_mp(this, &LupineWorldBuilder::set_snap_to_grid));
	canvas_header->add_child(grid_toggle);

	// Add grid size control
	Label *grid_size_label = memnew(Label);
	grid_size_label->set_text("Grid Size:");
	canvas_header->add_child(grid_size_label);

	SpinBox *grid_size_spin = memnew(SpinBox);
	grid_size_spin->set_min(8);
	grid_size_spin->set_max(128);
	grid_size_spin->set_step(8);
	grid_size_spin->set_value(grid_size);
	grid_size_spin->connect("value_changed", callable_mp(this, &LupineWorldBuilder::set_grid_size));
	canvas_header->add_child(grid_size_spin);

	// Create scene canvas
	scene_canvas = memnew(LupineSceneCanvas(this));
	scene_canvas->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	scene_canvas->set_v_size_flags(Control::SIZE_EXPAND_FILL);
	canvas_container->add_child(scene_canvas);

	// Create right panel (properties)
	right_panel = memnew(VBoxContainer);
	right_panel->set_custom_minimum_size(Size2(250, 0));
	center_split->add_child(right_panel);

	// Add title for property panel
	Label *property_title = memnew(Label);
	property_title->set_text("Properties");
	property_title->add_theme_style_override("normal", EditorNode::get_singleton()->get_gui_base()->get_theme_stylebox("panel", "Panel"));
	right_panel->add_child(property_title);

	// Create property panel
	property_panel = memnew(LupinePropertyPanel(this));
	right_panel->add_child(property_panel);
}

void LupineWorldBuilder::_setup_entity_library() {
	// Load entity definitions from project or built-in defaults
	_load_entity_definitions();

	// Refresh the entity palette
	if (entity_palette) {
		entity_palette->refresh_entities();
	}
}

void LupineWorldBuilder::_load_entity_definitions() {
	// Clear existing entities
	entity_library.clear();
	entity_map.clear();

	// Add default entities for common game objects

	// Characters and NPCs
	LupineEntity npc_entity("npc_basic", "Basic NPC", "Characters");
	npc_entity.description = "A basic non-player character with dialogue capabilities";
	npc_entity.scene_path = "res://scenes/characters/BasicNPC.tscn";
	npc_entity.default_properties["dialogue_file"] = "";
	npc_entity.default_properties["character_name"] = "NPC";
	add_entity_type(npc_entity);

	LupineEntity enemy_entity("enemy_basic", "Basic Enemy", "Characters");
	enemy_entity.description = "A basic enemy with AI and combat capabilities";
	enemy_entity.scene_path = "res://scenes/enemies/BasicEnemy.tscn";
	enemy_entity.default_properties["health"] = 100;
	enemy_entity.default_properties["damage"] = 10;
	enemy_entity.default_properties["ai_type"] = "patrol";
	add_entity_type(enemy_entity);

	// Interactive Objects
	LupineEntity chest_entity("chest", "Treasure Chest", "Interactive");
	chest_entity.description = "A chest that can contain items and gold";
	chest_entity.scene_path = "res://scenes/objects/Chest.tscn";
	chest_entity.default_properties["items"] = Array();
	chest_entity.default_properties["gold"] = 0;
	chest_entity.default_properties["is_locked"] = false;
	add_entity_type(chest_entity);

	LupineEntity door_entity("door", "Door", "Interactive");
	door_entity.description = "A door that can be opened, locked, or require keys";
	door_entity.scene_path = "res://scenes/objects/Door.tscn";
	door_entity.default_properties["is_locked"] = false;
	door_entity.default_properties["required_key"] = "";
	door_entity.default_properties["target_scene"] = "";
	add_entity_type(door_entity);

	// Environment
	LupineEntity tree_entity("tree", "Tree", "Environment");
	tree_entity.description = "Decorative tree for outdoor environments";
	tree_entity.scene_path = "res://scenes/environment/Tree.tscn";
	tree_entity.default_properties["tree_type"] = "oak";
	tree_entity.default_properties["has_collision"] = true;
	add_entity_type(tree_entity);

	LupineEntity rock_entity("rock", "Rock", "Environment");
	rock_entity.description = "Decorative rock formation";
	rock_entity.scene_path = "res://scenes/environment/Rock.tscn";
	rock_entity.default_properties["rock_size"] = "medium";
	rock_entity.default_properties["has_collision"] = true;
	add_entity_type(rock_entity);

	// Triggers and Logic
	LupineEntity trigger_entity("trigger", "Trigger Zone", "Logic");
	trigger_entity.description = "Invisible area that triggers events when entered";
	trigger_entity.scene_path = "res://scenes/logic/TriggerZone.tscn";
	trigger_entity.default_properties["trigger_type"] = "enter";
	trigger_entity.default_properties["event_name"] = "";
	trigger_entity.default_properties["one_shot"] = false;
	add_entity_type(trigger_entity);

	LupineEntity spawn_entity("spawn_point", "Spawn Point", "Logic");
	spawn_entity.description = "Player or entity spawn location";
	spawn_entity.scene_path = "res://scenes/logic/SpawnPoint.tscn";
	spawn_entity.default_properties["spawn_type"] = "player";
	spawn_entity.default_properties["facing_direction"] = "down";
	add_entity_type(spawn_entity);
}

void LupineWorldBuilder::on_scene_changed(Node *p_scene) {
	current_scene = p_scene;
	selected_node = nullptr;

	if (scene_canvas) {
		scene_canvas->set_scene_root(p_scene);
	}

	if (property_panel) {
		property_panel->set_target_node(nullptr);
	}
}

void LupineWorldBuilder::on_node_selected(Node *p_node) {
	selected_node = p_node;

	if (property_panel) {
		property_panel->set_target_node(p_node);
	}
}

void LupineWorldBuilder::add_entity_type(const LupineEntity &p_entity) {
	entity_library.push_back(p_entity);
	entity_map[p_entity.id] = const_cast<LupineEntity*>(&entity_library[entity_library.size() - 1]);
}

void LupineWorldBuilder::remove_entity_type(const String &p_id) {
	entity_map.erase(p_id);

	for (int i = 0; i < entity_library.size(); i++) {
		if (entity_library[i].id == p_id) {
			entity_library.remove_at(i);
			break;
		}
	}
}

LupineEntity *LupineWorldBuilder::get_entity_type(const String &p_id) {
	if (entity_map.has(p_id)) {
		return entity_map[p_id];
	}
	return nullptr;
}

Vector<LupineEntity> LupineWorldBuilder::get_entities_by_category(const String &p_category) {
	Vector<LupineEntity> result;
	for (const LupineEntity &entity : entity_library) {
		if (entity.category == p_category) {
			result.push_back(entity);
		}
	}
	return result;
}

Vector2 LupineWorldBuilder::_snap_to_grid(Vector2 p_position) {
	if (!snap_to_grid) {
		return p_position;
	}

	int x = Math::round(p_position.x / grid_size) * grid_size;
	int y = Math::round(p_position.y / grid_size) * grid_size;
	return Vector2(x, y);
}

void LupineWorldBuilder::_on_entity_selected(const String &p_entity_id) {
	selected_entity_type = get_entity_type(p_entity_id);
	is_placing_mode = true;
}

void LupineWorldBuilder::_on_canvas_clicked(Vector2 p_position) {
	if (is_placing_mode && selected_entity_type) {
		_place_entity_at_position(p_position);
	}
}

void LupineWorldBuilder::_on_node_selected_in_canvas(Node *p_node) {
	on_node_selected(p_node);
}

void LupineWorldBuilder::_place_entity_at_position(Vector2 p_position) {
	if (!selected_entity_type || !current_scene) {
		return;
	}

	Vector2 snapped_pos = _snap_to_grid(p_position);

	// Load the entity scene template
	Ref<PackedScene> scene_template = ResourceLoader::load(selected_entity_type->scene_path);
	if (!scene_template.is_valid()) {
		ERR_PRINT("Failed to load entity scene: " + selected_entity_type->scene_path);
		return;
	}

	// Instantiate the entity
	Node *entity_instance = scene_template->instantiate();
	if (!entity_instance) {
		ERR_PRINT("Failed to instantiate entity: " + selected_entity_type->name);
		return;
	}

	// Set position based on node type
	Node2D *node_2d = Object::cast_to<Node2D>(entity_instance);
	Node3D *node_3d = Object::cast_to<Node3D>(entity_instance);

	if (node_2d) {
		node_2d->set_position(snapped_pos);
	} else if (node_3d) {
		node_3d->set_position(Vector3(snapped_pos.x, 0, snapped_pos.y));
	}

	// Set default properties
	Array keys = selected_entity_type->default_properties.keys();
	for (int i = 0; i < keys.size(); i++) {
		String key = keys[i];
		Variant value = selected_entity_type->default_properties[key];
		if (entity_instance->has_method("set_" + key)) {
			entity_instance->call("set_" + key, value);
		} else if (entity_instance->has_meta(key)) {
			entity_instance->set_meta(key, value);
		}
	}

	// Add to scene
	current_scene->add_child(entity_instance);
	entity_instance->set_owner(current_scene);

	// Select the new entity
	on_node_selected(entity_instance);

	// Mark scene as modified
	EditorNode::get_singleton()->get_edited_scene()->set_edited(true);
}

void LupineWorldBuilder::_on_property_changed(const String &p_property, const Variant &p_value) {
	if (!selected_node) {
		return;
	}

	// Apply property change to selected node
	if (selected_node->has_method("set_" + p_property)) {
		selected_node->call("set_" + p_property, p_value);
	} else if (selected_node->has_meta(p_property)) {
		selected_node->set_meta(p_property, p_value);
	}

	// Mark scene as modified
	EditorNode::get_singleton()->get_edited_scene()->set_edited(true);
}

// LupineEntityPalette Implementation

void LupineEntityPalette::_bind_methods() {
	ClassDB::bind_method(D_METHOD("_on_category_changed", "index"), &LupineEntityPalette::_on_category_changed);
	ClassDB::bind_method(D_METHOD("_on_search_changed", "text"), &LupineEntityPalette::_on_search_changed);
	ClassDB::bind_method(D_METHOD("_on_entity_selected", "index"), &LupineEntityPalette::_on_entity_selected);
}

LupineEntityPalette::LupineEntityPalette(LupineWorldBuilder *p_builder) {
	world_builder = p_builder;

	set_h_size_flags(Control::SIZE_EXPAND_FILL);
	set_v_size_flags(Control::SIZE_EXPAND_FILL);

	// Create category filter
	Label *category_label = memnew(Label);
	category_label->set_text("Category:");
	add_child(category_label);

	category_filter = memnew(OptionButton);
	category_filter->add_item("All Categories");
	category_filter->add_item("Characters");
	category_filter->add_item("Interactive");
	category_filter->add_item("Environment");
	category_filter->add_item("Logic");
	category_filter->connect("item_selected", callable_mp(this, &LupineEntityPalette::_on_category_changed));
	add_child(category_filter);

	// Create search filter
	Label *search_label = memnew(Label);
	search_label->set_text("Search:");
	add_child(search_label);

	search_filter = memnew(LineEdit);
	search_filter->set_placeholder("Search entities...");
	search_filter->connect("text_changed", callable_mp(this, &LupineEntityPalette::_on_search_changed));
	add_child(search_filter);

	// Create entity list
	entity_list = memnew(ItemList);
	entity_list->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	entity_list->set_v_size_flags(Control::SIZE_EXPAND_FILL);
	entity_list->connect("item_selected", callable_mp(this, &LupineEntityPalette::_on_entity_selected));
	add_child(entity_list);

	// Create description area
	Label *desc_label = memnew(Label);
	desc_label->set_text("Description:");
	add_child(desc_label);

	entity_description = memnew(RichTextLabel);
	entity_description->set_custom_minimum_size(Size2(0, 80));
	entity_description->set_fit_content(true);
	entity_description->set_scroll_active(false);
	add_child(entity_description);
}

LupineEntityPalette::~LupineEntityPalette() {
}

void LupineEntityPalette::refresh_entities() {
	_update_entity_list();
}

void LupineEntityPalette::set_selected_entity(const LupineEntity *p_entity) {
	if (!p_entity || !entity_list) {
		return;
	}

	// Find and select the entity in the list
	for (int i = 0; i < filtered_entities.size(); i++) {
		if (filtered_entities[i].id == p_entity->id) {
			entity_list->select(i);
			_update_description();
			break;
		}
	}
}

void LupineEntityPalette::_on_category_changed(int p_index) {
	_update_entity_list();
}

void LupineEntityPalette::_on_search_changed(const String &p_text) {
	_update_entity_list();
}

void LupineEntityPalette::_on_entity_selected(int p_index) {
	if (p_index < 0 || p_index >= filtered_entities.size()) {
		return;
	}

	const LupineEntity &entity = filtered_entities[p_index];
	_update_description();

	// Notify world builder
	if (world_builder) {
		world_builder->emit_signal("entity_selected", entity.id);
	}
}

void LupineEntityPalette::_update_entity_list() {
	if (!entity_list || !world_builder) {
		return;
	}

	entity_list->clear();
	filtered_entities.clear();

	String selected_category = "";
	if (category_filter && category_filter->get_selected() > 0) {
		selected_category = category_filter->get_item_text(category_filter->get_selected());
	}

	String search_text = "";
	if (search_filter) {
		search_text = search_filter->get_text().to_lower();
	}

	// Filter entities
	for (const LupineEntity &entity : world_builder->get_entity_library()) {
		bool matches_category = selected_category.is_empty() || selected_category == "All Categories" || entity.category == selected_category;
		bool matches_search = search_text.is_empty() || entity.name.to_lower().contains(search_text) || entity.description.to_lower().contains(search_text);

		if (matches_category && matches_search) {
			filtered_entities.push_back(entity);
			entity_list->add_item(entity.name);

			// Set icon if available
			if (entity.icon.is_valid()) {
				entity_list->set_item_icon(entity_list->get_item_count() - 1, entity.icon);
			}
		}
	}

	_update_description();
}

void LupineEntityPalette::_update_description() {
	if (!entity_description) {
		return;
	}

	int selected = entity_list ? entity_list->get_selected_items().size() > 0 ? entity_list->get_selected_items()[0] : -1 : -1;

	if (selected >= 0 && selected < filtered_entities.size()) {
		const LupineEntity &entity = filtered_entities[selected];
		String desc_text = "[b]" + entity.name + "[/b]\n";
		desc_text += "[i]Category: " + entity.category + "[/i]\n\n";
		desc_text += entity.description;
		entity_description->set_text(desc_text);
	} else {
		entity_description->set_text("Select an entity to see its description.");
	}
}

// LupineSceneCanvas Implementation

void LupineSceneCanvas::_bind_methods() {
	ClassDB::bind_method(D_METHOD("emit_canvas_clicked", "position"), &LupineSceneCanvas::emit_canvas_clicked);
	ClassDB::bind_method(D_METHOD("emit_node_selected", "node"), &LupineSceneCanvas::emit_node_selected);

	ADD_SIGNAL(MethodInfo("canvas_clicked", PropertyInfo(Variant::VECTOR2, "position")));
	ADD_SIGNAL(MethodInfo("node_selected", PropertyInfo(Variant::OBJECT, "node")));
}

LupineSceneCanvas::LupineSceneCanvas(LupineWorldBuilder *p_builder) {
	world_builder = p_builder;
	scene_root = nullptr;
	show_grid = true;
	show_gizmos = true;
	grid_color = Color(0.3, 0.3, 0.3, 0.5);
	selection_color = Color(1.0, 0.5, 0.0, 0.8);
	hovered_node = nullptr;
	is_dragging = false;

	set_focus_mode(Control::FOCUS_ALL);
	set_clip_contents(true);
}

LupineSceneCanvas::~LupineSceneCanvas() {
}

void LupineSceneCanvas::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_DRAW: {
			if (show_grid) {
				_draw_grid();
			}
			if (show_gizmos) {
				_draw_gizmos();
			}
			_draw_selection();
		} break;
	}
}

void LupineSceneCanvas::_gui_input(const Ref<InputEvent> &p_event) {
	Ref<InputEventMouseButton> mb = p_event;
	Ref<InputEventMouseMotion> mm = p_event;

	if (mb.is_valid()) {
		if (mb->get_button_index() == MouseButton::LEFT && mb->is_pressed()) {
			Vector2 click_pos = _canvas_to_world(mb->get_position());
			Node *clicked_node = _get_node_at_position(click_pos);

			if (clicked_node) {
				emit_node_selected(clicked_node);
			} else {
				emit_canvas_clicked(click_pos);
			}
		}
	}

	if (mm.is_valid()) {
		Vector2 mouse_pos = _canvas_to_world(mm->get_position());
		Node *node_under_mouse = _get_node_at_position(mouse_pos);

		if (node_under_mouse != hovered_node) {
			hovered_node = node_under_mouse;
			queue_redraw();
		}

		last_mouse_position = mm->get_position();
	}
}

void LupineSceneCanvas::set_scene_root(Node *p_scene) {
	scene_root = p_scene;
	queue_redraw();
}

void LupineSceneCanvas::emit_canvas_clicked(Vector2 p_position) {
	emit_signal("canvas_clicked", p_position);
	if (world_builder) {
		world_builder->emit_signal("canvas_clicked", p_position);
	}
}

void LupineSceneCanvas::emit_node_selected(Node *p_node) {
	emit_signal("node_selected", p_node);
	if (world_builder) {
		world_builder->emit_signal("node_selected", p_node);
	}
}

void LupineSceneCanvas::_draw_grid() {
	if (!world_builder) {
		return;
	}

	int grid_size = world_builder->get_grid_size();
	Size2 canvas_size = get_size();

	// Draw vertical lines
	for (int x = 0; x < canvas_size.width; x += grid_size) {
		draw_line(Vector2(x, 0), Vector2(x, canvas_size.height), grid_color, 1.0);
	}

	// Draw horizontal lines
	for (int y = 0; y < canvas_size.height; y += grid_size) {
		draw_line(Vector2(0, y), Vector2(canvas_size.width, y), grid_color, 1.0);
	}
}

void LupineSceneCanvas::_draw_gizmos() {
	if (!scene_root) {
		return;
	}

	// Draw gizmos for all child nodes
	_draw_node_gizmos(scene_root);
}

void LupineSceneCanvas::_draw_node_gizmos(Node *p_node) {
	if (!p_node) {
		return;
	}

	// Draw gizmo for 2D nodes
	Node2D *node_2d = Object::cast_to<Node2D>(p_node);
	if (node_2d) {
		Vector2 world_pos = node_2d->get_global_position();
		Vector2 canvas_pos = _world_to_canvas(world_pos);

		// Draw position indicator
		Color gizmo_color = (p_node == hovered_node) ? selection_color : Color(1, 1, 1);
		draw_circle(canvas_pos, 4, gizmo_color);
		draw_circle(canvas_pos, 4, Color(0, 0, 0), false, 1.0);

		// Draw name label
		String name = p_node->get_name();
		Vector2 text_pos = canvas_pos + Vector2(8, -8);
		draw_string(get_theme_default_font(), text_pos, name, HORIZONTAL_ALIGNMENT_LEFT, -1, 12, gizmo_color);
	}

	// Recursively draw gizmos for children
	for (int i = 0; i < p_node->get_child_count(); i++) {
		_draw_node_gizmos(p_node->get_child(i));
	}
}

void LupineSceneCanvas::_draw_selection() {
	// Draw selection indicators for selected nodes
	// This would be implemented based on the current selection state
}

Node *LupineSceneCanvas::_get_node_at_position(Vector2 p_position) {
	if (!scene_root) {
		return nullptr;
	}

	return _find_node_at_position(scene_root, p_position);
}

Node *LupineSceneCanvas::_find_node_at_position(Node *p_node, Vector2 p_position) {
	if (!p_node) {
		return nullptr;
	}

	// Check 2D nodes
	Node2D *node_2d = Object::cast_to<Node2D>(p_node);
	if (node_2d) {
		Vector2 node_pos = node_2d->get_global_position();
		float distance = node_pos.distance_to(p_position);

		// Check if position is within a reasonable selection radius
		if (distance < 16.0) {
			return p_node;
		}
	}

	// Check children
	for (int i = 0; i < p_node->get_child_count(); i++) {
		Node *found = _find_node_at_position(p_node->get_child(i), p_position);
		if (found) {
			return found;
		}
	}

	return nullptr;
}

Vector2 LupineSceneCanvas::_world_to_canvas(Vector2 p_world_pos) {
	// Simple 1:1 mapping for now
	// In a more advanced implementation, this would handle zoom and pan
	return p_world_pos;
}

Vector2 LupineSceneCanvas::_canvas_to_world(Vector2 p_canvas_pos) {
	// Simple 1:1 mapping for now
	// In a more advanced implementation, this would handle zoom and pan
	return p_canvas_pos;
}

// LupinePropertyPanel Implementation

void LupinePropertyPanel::_bind_methods() {
	ClassDB::bind_method(D_METHOD("_on_property_control_changed"), &LupinePropertyPanel::_on_property_control_changed);
}

LupinePropertyPanel::LupinePropertyPanel(LupineWorldBuilder *p_builder) {
	world_builder = p_builder;
	current_node = nullptr;

	set_h_size_flags(Control::SIZE_EXPAND_FILL);
	set_v_size_flags(Control::SIZE_EXPAND_FILL);

	// Create title label
	title_label = memnew(Label);
	title_label->set_text("No Selection");
	title_label->add_theme_style_override("normal", EditorNode::get_singleton()->get_gui_base()->get_theme_stylebox("panel", "Panel"));
	add_child(title_label);

	// Create scroll container for properties
	properties_scroll = memnew(ScrollContainer);
	properties_scroll->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	properties_scroll->set_v_size_flags(Control::SIZE_EXPAND_FILL);
	add_child(properties_scroll);

	// Create properties container
	properties_container = memnew(VBoxContainer);
	properties_container->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	properties_scroll->add_child(properties_container);
}

LupinePropertyPanel::~LupinePropertyPanel() {
}

void LupinePropertyPanel::set_target_node(Node *p_node) {
	current_node = p_node;
	refresh_properties();
}

void LupinePropertyPanel::refresh_properties() {
	_clear_properties();

	if (!current_node) {
		title_label->set_text("No Selection");
		return;
	}

	title_label->set_text("Properties: " + current_node->get_name());

	// Add basic node properties
	_create_property_control("Name", current_node->get_name());
	_create_property_control("Class", current_node->get_class());

	// Add position for 2D nodes
	Node2D *node_2d = Object::cast_to<Node2D>(current_node);
	if (node_2d) {
		_create_property_control("Position", node_2d->get_position());
		_create_property_control("Rotation", node_2d->get_rotation());
		_create_property_control("Scale", node_2d->get_scale());
	}

	// Add position for 3D nodes
	Node3D *node_3d = Object::cast_to<Node3D>(current_node);
	if (node_3d) {
		_create_property_control("Position", node_3d->get_position());
		_create_property_control("Rotation", node_3d->get_rotation());
		_create_property_control("Scale", node_3d->get_scale());
	}

	// Add custom properties from metadata
	List<StringName> meta_list;
	current_node->get_meta_list(&meta_list);

	for (const StringName &meta_name : meta_list) {
		Variant meta_value = current_node->get_meta(meta_name);
		_create_property_control(meta_name, meta_value);
	}
}

void LupinePropertyPanel::_create_property_control(const String &p_name, const Variant &p_value) {
	HBoxContainer *property_row = memnew(HBoxContainer);
	properties_container->add_child(property_row);

	// Property name label
	Label *name_label = memnew(Label);
	name_label->set_text(p_name + ":");
	name_label->set_custom_minimum_size(Size2(80, 0));
	property_row->add_child(name_label);

	// Property value control based on type
	Control *value_control = nullptr;

	switch (p_value.get_type()) {
		case Variant::STRING: {
			LineEdit *line_edit = memnew(LineEdit);
			line_edit->set_text(p_value);
			line_edit->set_h_size_flags(Control::SIZE_EXPAND_FILL);
			line_edit->connect("text_changed", callable_mp(this, &LupinePropertyPanel::_on_property_control_changed).bind(p_name));
			value_control = line_edit;
		} break;

		case Variant::INT: {
			SpinBox *spin_box = memnew(SpinBox);
			spin_box->set_value(p_value);
			spin_box->set_h_size_flags(Control::SIZE_EXPAND_FILL);
			spin_box->connect("value_changed", callable_mp(this, &LupinePropertyPanel::_on_property_control_changed).bind(p_name));
			value_control = spin_box;
		} break;

		case Variant::FLOAT: {
			SpinBox *spin_box = memnew(SpinBox);
			spin_box->set_value(p_value);
			spin_box->set_step(0.01);
			spin_box->set_h_size_flags(Control::SIZE_EXPAND_FILL);
			spin_box->connect("value_changed", callable_mp(this, &LupinePropertyPanel::_on_property_control_changed).bind(p_name));
			value_control = spin_box;
		} break;

		case Variant::BOOL: {
			CheckBox *check_box = memnew(CheckBox);
			check_box->set_pressed(p_value);
			check_box->connect("toggled", callable_mp(this, &LupinePropertyPanel::_on_property_control_changed).bind(p_name));
			value_control = check_box;
		} break;

		case Variant::VECTOR2: {
			HBoxContainer *vector_container = memnew(HBoxContainer);
			Vector2 vec = p_value;

			SpinBox *x_spin = memnew(SpinBox);
			x_spin->set_value(vec.x);
			x_spin->set_step(0.01);
			x_spin->set_h_size_flags(Control::SIZE_EXPAND_FILL);
			vector_container->add_child(x_spin);

			SpinBox *y_spin = memnew(SpinBox);
			y_spin->set_value(vec.y);
			y_spin->set_step(0.01);
			y_spin->set_h_size_flags(Control::SIZE_EXPAND_FILL);
			vector_container->add_child(y_spin);

			// Note: Vector2 property changes would need special handling
			value_control = vector_container;
		} break;

		default: {
			// Fallback to string representation
			LineEdit *line_edit = memnew(LineEdit);
			line_edit->set_text(p_value.stringify());
			line_edit->set_h_size_flags(Control::SIZE_EXPAND_FILL);
			line_edit->set_editable(false); // Read-only for unsupported types
			value_control = line_edit;
		} break;
	}

	if (value_control) {
		property_row->add_child(value_control);
		property_controls[p_name] = value_control;
	}
}

void LupinePropertyPanel::_update_property_control(const String &p_name, const Variant &p_value) {
	if (!property_controls.has(p_name)) {
		return;
	}

	Control *control = property_controls[p_name];

	// Update control value based on type
	LineEdit *line_edit = Object::cast_to<LineEdit>(control);
	if (line_edit) {
		line_edit->set_text(p_value.stringify());
		return;
	}

	SpinBox *spin_box = Object::cast_to<SpinBox>(control);
	if (spin_box) {
		spin_box->set_value(p_value);
		return;
	}

	CheckBox *check_box = Object::cast_to<CheckBox>(control);
	if (check_box) {
		check_box->set_pressed(p_value);
		return;
	}
}

void LupinePropertyPanel::_clear_properties() {
	property_controls.clear();

	// Remove all children from properties container
	for (int i = properties_container->get_child_count() - 1; i >= 0; i--) {
		Node *child = properties_container->get_child(i);
		properties_container->remove_child(child);
		memdelete(child);
	}
}

void LupinePropertyPanel::_on_property_control_changed(const String &p_name, const Variant &p_value) {
	if (!current_node || !world_builder) {
		return;
	}

	// Apply the property change
	if (p_name == "Name") {
		current_node->set_name(p_value);
	} else if (p_name == "Position") {
		Node2D *node_2d = Object::cast_to<Node2D>(current_node);
		Node3D *node_3d = Object::cast_to<Node3D>(current_node);

		if (node_2d) {
			node_2d->set_position(p_value);
		} else if (node_3d) {
			node_3d->set_position(p_value);
		}
	} else {
		// Set as metadata
		current_node->set_meta(p_name, p_value);
	}

	// Notify world builder of the change
	world_builder->emit_signal("property_changed", p_name, p_value);
}