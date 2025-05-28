#include "lupine_topdown_rpg_editor.h"
#include "editor/editor_node.h"
#include "editor/editor_interface.h"
#include "scene/gui/separator.h"
#include "scene/gui/margin_container.h"
#include "scene/2d/tile_map.h"
#include "core/io/json.h"
#include "core/os/time.h"
#include "core/input/input_event.h"

// LupineTopdownRPGEditor Implementation

void LupineTopdownRPGEditor::_bind_methods() {
	// No methods to bind for now
}

LupineTopdownRPGEditor::LupineTopdownRPGEditor() {
	print_line("LupineTopdownRPGEditor: Constructor called");
}

LupineTopdownRPGEditor::~LupineTopdownRPGEditor() {
	if (tool_panel) {
		memdelete(tool_panel);
	}
}

void LupineTopdownRPGEditor::initialize() {
	if (tool_panel) {
		return; // Already initialized
	}

	print_line("LupineTopdownRPGEditor: Initializing...");
	_create_ui();
	_setup_tabs();
	print_line("LupineTopdownRPGEditor: Initialization complete");
}

void LupineTopdownRPGEditor::cleanup() {
	if (tool_panel) {
		tool_panel->queue_free();
		tool_panel = nullptr;
	}

	// Clean up sub-editors
	if (map_editor) {
		memdelete(map_editor);
		map_editor = nullptr;
	}
	if (entity_placer) {
		memdelete(entity_placer);
		entity_placer = nullptr;
	}
}

void LupineTopdownRPGEditor::_create_ui() {
	// Create main panel
	tool_panel = memnew(Control);
	tool_panel->set_name("TopdownRPGEditor");
	tool_panel->set_anchors_and_offsets_preset(Control::PRESET_FULL_RECT);

	// Create main tab container
	main_tabs = memnew(TabContainer);
	main_tabs->set_anchors_and_offsets_preset(Control::PRESET_FULL_RECT);
	tool_panel->add_child(main_tabs);

	print_line("LupineTopdownRPGEditor: UI created");
}

void LupineTopdownRPGEditor::_setup_tabs() {
	// Create Map Editor tab
	map_editor = memnew(LupineTopdownMapEditor(this));
	map_editor->set_name("Map Editor");
	main_tabs->add_child(map_editor);

	// Create Entity Placer tab
	entity_placer = memnew(LupineTopdownEntityPlacer(this));
	entity_placer->set_name("Entity Placer");
	main_tabs->add_child(entity_placer);

	// Create Event Editor tab (placeholder for now)
	Control *event_tab = memnew(Control);
	event_tab->set_name("Event Editor");
	Label *event_label = memnew(Label);
	event_label->set_text("Event Editor - Coming Soon");
	event_label->set_anchors_and_offsets_preset(Control::PRESET_CENTER);
	event_tab->add_child(event_label);
	main_tabs->add_child(event_tab);

	// Create Transition Editor tab (placeholder for now)
	Control *transition_tab = memnew(Control);
	transition_tab->set_name("Scene Transitions");
	Label *transition_label = memnew(Label);
	transition_label->set_text("Scene Transition Editor - Coming Soon");
	transition_label->set_anchors_and_offsets_preset(Control::PRESET_CENTER);
	transition_tab->add_child(transition_label);
	main_tabs->add_child(transition_tab);

	print_line("LupineTopdownRPGEditor: Tabs setup complete");
}

void LupineTopdownRPGEditor::on_scene_changed(Node *p_scene) {
	current_scene = Object::cast_to<Node2D>(p_scene);
	if (current_scene) {
		current_scene_path = current_scene->get_scene_file_path();
		_load_scene_data();

		// Update sub-editors
		if (map_editor) {
			TileMap *tilemap = Object::cast_to<TileMap>(current_scene->get_node(NodePath("TileMap")));
			if (!tilemap) {
				// Look for TileMap in children
				for (int i = 0; i < current_scene->get_child_count(); i++) {
					tilemap = Object::cast_to<TileMap>(current_scene->get_child(i));
					if (tilemap) break;
				}
			}
			map_editor->set_tilemap(tilemap);
		}

		if (entity_placer) {
			entity_placer->refresh_placed_entities();
		}
	}
}

void LupineTopdownRPGEditor::on_node_selected(Node *p_node) {
	// Handle node selection for property editing
	if (entity_placer && p_node) {
		// Check if selected node is a placed entity
		for (int i = 0; i < placed_entities.size(); i++) {
			if (placed_entities[i].entity_id == p_node->get_name()) {
				// Update entity placer to show this entity's properties
				break;
			}
		}
	}
}

void LupineTopdownRPGEditor::add_entity(const TopdownEntityData &p_entity) {
	placed_entities.push_back(p_entity);
	_save_scene_data();
}

void LupineTopdownRPGEditor::remove_entity(const String &p_entity_id) {
	for (int i = 0; i < placed_entities.size(); i++) {
		if (placed_entities[i].entity_id == p_entity_id) {
			placed_entities.remove_at(i);
			break;
		}
	}
	_save_scene_data();
}

void LupineTopdownRPGEditor::update_entity(const String &p_entity_id, const TopdownEntityData &p_entity) {
	for (int i = 0; i < placed_entities.size(); i++) {
		if (placed_entities[i].entity_id == p_entity_id) {
			placed_entities.write[i] = p_entity;
			break;
		}
	}
	_save_scene_data();
}

void LupineTopdownRPGEditor::_save_scene_data() {
	if (current_scene_path.is_empty()) {
		return;
	}

	// Save entity data to scene metadata or separate file
	Dictionary scene_data;

	Array entities_array;
	for (const TopdownEntityData &entity : placed_entities) {
		Dictionary entity_dict;
		entity_dict["id"] = entity.entity_id;
		entity_dict["name"] = entity.entity_name;
		entity_dict["type"] = entity.entity_type;
		entity_dict["position"] = entity.position;
		entity_dict["properties"] = entity.properties;
		entity_dict["script_path"] = entity.script_path;
		entity_dict["scene_path"] = entity.scene_path;
		entities_array.push_back(entity_dict);
	}
	scene_data["entities"] = entities_array;

	Array triggers_array;
	for (const TopdownEventTrigger &trigger : event_triggers) {
		Dictionary trigger_dict;
		trigger_dict["id"] = trigger.trigger_id;
		trigger_dict["name"] = trigger.trigger_name;
		trigger_dict["type"] = trigger.trigger_type;
		trigger_dict["position"] = trigger.position;
		trigger_dict["size"] = trigger.size;
		trigger_dict["conditions"] = trigger.conditions;
		trigger_dict["actions"] = trigger.actions;
		trigger_dict["repeatable"] = trigger.is_repeatable;
		triggers_array.push_back(trigger_dict);
	}
	scene_data["triggers"] = triggers_array;

	// Save to file
	String data_path = current_scene_path.get_basename() + "_lupine_data.json";
	Ref<FileAccess> file = FileAccess::open(data_path, FileAccess::WRITE);
	if (file.is_valid()) {
		Ref<JSON> json;
		json.instantiate();
		String json_string = json->stringify(scene_data);
		file->store_string(json_string);
		file->close();
	}
}

void LupineTopdownRPGEditor::_load_scene_data() {
	if (current_scene_path.is_empty()) {
		return;
	}

	// Clear existing data
	placed_entities.clear();
	event_triggers.clear();
	scene_transitions.clear();

	// Load from file
	String data_path = current_scene_path.get_basename() + "_lupine_data.json";
	Ref<FileAccess> file = FileAccess::open(data_path, FileAccess::READ);
	if (file.is_valid()) {
		String json_text = file->get_as_text();
		file->close();

		Ref<JSON> json;
		json.instantiate();
		Error parse_result = json->parse(json_text);
		if (parse_result == OK) {
			Dictionary scene_data = json->get_data();

			// Load entities
			if (scene_data.has("entities")) {
				Array entities_array = scene_data["entities"];
				for (int i = 0; i < entities_array.size(); i++) {
					Dictionary entity_dict = entities_array[i];
					TopdownEntityData entity;
					entity.entity_id = entity_dict.get("id", "");
					entity.entity_name = entity_dict.get("name", "");
					entity.entity_type = entity_dict.get("type", "");
					entity.position = entity_dict.get("position", Vector2());
					entity.properties = entity_dict.get("properties", Dictionary());
					entity.script_path = entity_dict.get("script_path", "");
					entity.scene_path = entity_dict.get("scene_path", "");
					placed_entities.push_back(entity);
				}
			}

			// Load triggers
			if (scene_data.has("triggers")) {
				Array triggers_array = scene_data["triggers"];
				for (int i = 0; i < triggers_array.size(); i++) {
					Dictionary trigger_dict = triggers_array[i];
					TopdownEventTrigger trigger;
					trigger.trigger_id = trigger_dict.get("id", "");
					trigger.trigger_name = trigger_dict.get("name", "");
					trigger.trigger_type = trigger_dict.get("type", "");
					trigger.position = trigger_dict.get("position", Vector2());
					trigger.size = trigger_dict.get("size", Vector2());
					trigger.conditions = trigger_dict.get("conditions", Vector<String>());
					trigger.actions = trigger_dict.get("actions", Vector<String>());
					trigger.is_repeatable = trigger_dict.get("repeatable", true);
					event_triggers.push_back(trigger);
				}
			}
		}
	}
}

// LupineTopdownMapEditor Implementation

void LupineTopdownMapEditor::_bind_methods() {
	ClassDB::bind_method(D_METHOD("_on_tile_selected"), &LupineTopdownMapEditor::_on_tile_selected);
	ClassDB::bind_method(D_METHOD("_on_canvas_input"), &LupineTopdownMapEditor::_on_canvas_input);
}

LupineTopdownMapEditor::LupineTopdownMapEditor(LupineTopdownRPGEditor *p_parent) {
	parent_editor = p_parent;
	_create_ui();
}

LupineTopdownMapEditor::~LupineTopdownMapEditor() {
}

void LupineTopdownMapEditor::_create_ui() {
	set_anchors_and_offsets_preset(Control::PRESET_FULL_RECT);

	// Create main split container
	main_split = memnew(HSplitContainer);
	main_split->set_anchors_and_offsets_preset(Control::PRESET_FULL_RECT);
	add_child(main_split);

	// Create tile palette on the left
	tile_palette = memnew(VBoxContainer);
	tile_palette->set_custom_minimum_size(Size2(250, 0));
	main_split->add_child(tile_palette);

	Label *palette_label = memnew(Label);
	palette_label->set_text("Tile Palette");
	tile_palette->add_child(palette_label);

	tile_list = memnew(ItemList);
	tile_list->set_v_size_flags(Control::SIZE_EXPAND_FILL);
	tile_list->connect("item_selected", callable_mp(this, &LupineTopdownMapEditor::_on_tile_selected));
	tile_palette->add_child(tile_list);

	// Create map canvas on the right
	map_canvas = memnew(Control);
	map_canvas->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	map_canvas->connect("gui_input", callable_mp(this, &LupineTopdownMapEditor::_on_canvas_input));
	main_split->add_child(map_canvas);

	_setup_tile_palette();
}

void LupineTopdownMapEditor::_setup_tile_palette() {
	// Add default tiles
	TopdownMapTile grass_tile;
	grass_tile.tile_id = "grass";
	grass_tile.tile_name = "Grass";
	grass_tile.category = "terrain";
	grass_tile.is_collision = false;
	available_tiles.push_back(grass_tile);

	TopdownMapTile stone_tile;
	stone_tile.tile_id = "stone";
	stone_tile.tile_name = "Stone Wall";
	stone_tile.category = "walls";
	stone_tile.is_collision = true;
	available_tiles.push_back(stone_tile);

	TopdownMapTile water_tile;
	water_tile.tile_id = "water";
	water_tile.tile_name = "Water";
	water_tile.category = "terrain";
	water_tile.is_collision = true;
	available_tiles.push_back(water_tile);

	refresh_tile_palette();
}

void LupineTopdownMapEditor::refresh_tile_palette() {
	tile_list->clear();
	for (const TopdownMapTile &tile : available_tiles) {
		String display_name = tile.tile_name;
		if (tile.is_collision) {
			display_name += " (Collision)";
		}
		tile_list->add_item(display_name);
	}
}

void LupineTopdownMapEditor::_on_tile_selected(int p_index) {
	if (p_index >= 0 && p_index < available_tiles.size()) {
		selected_tile_id = available_tiles[p_index].tile_id;
	}
}

void LupineTopdownMapEditor::_on_canvas_input(const Ref<InputEvent> &p_event) {
	Ref<InputEventMouseButton> mb = p_event;
	if (mb.is_valid() && mb->is_pressed()) {
		if (current_tilemap && !selected_tile_id.is_empty()) {
			Vector2 local_pos = map_canvas->get_local_mouse_position();
			Vector2i tile_coords = current_tilemap->local_to_map(local_pos);

			if (mb->get_button_index() == MouseButton::LEFT) {
				_paint_tile(tile_coords);
			} else if (mb->get_button_index() == MouseButton::RIGHT) {
				_erase_tile(tile_coords);
			}
		}
	}
}

void LupineTopdownMapEditor::_paint_tile(Vector2i p_coords) {
	if (!current_tilemap) return;

	// Find the selected tile data
	for (const TopdownMapTile &tile : available_tiles) {
		if (tile.tile_id == selected_tile_id) {
			// Set tile in tilemap (simplified - would need proper tile source setup)
			current_tilemap->set_cell(current_layer, p_coords, 0, tile.atlas_coords);
			break;
		}
	}
}

void LupineTopdownMapEditor::_erase_tile(Vector2i p_coords) {
	if (!current_tilemap) return;
	current_tilemap->erase_cell(current_layer, p_coords);
}

void LupineTopdownMapEditor::set_tilemap(TileMap *p_tilemap) {
	current_tilemap = p_tilemap;
}

// LupineTopdownEntityPlacer Implementation

void LupineTopdownEntityPlacer::_bind_methods() {
	ClassDB::bind_method(D_METHOD("_on_entity_selected"), &LupineTopdownEntityPlacer::_on_entity_selected);
	ClassDB::bind_method(D_METHOD("_on_canvas_input"), &LupineTopdownEntityPlacer::_on_canvas_input);
}

LupineTopdownEntityPlacer::LupineTopdownEntityPlacer(LupineTopdownRPGEditor *p_parent) {
	parent_editor = p_parent;
	_create_ui();
}

LupineTopdownEntityPlacer::~LupineTopdownEntityPlacer() {
}

void LupineTopdownEntityPlacer::_create_ui() {
	set_anchors_and_offsets_preset(Control::PRESET_FULL_RECT);

	// Create main split container
	main_split = memnew(HSplitContainer);
	main_split->set_anchors_and_offsets_preset(Control::PRESET_FULL_RECT);
	add_child(main_split);

	// Create entity palette on the left
	entity_palette = memnew(VBoxContainer);
	entity_palette->set_custom_minimum_size(Size2(250, 0));
	main_split->add_child(entity_palette);

	Label *palette_label = memnew(Label);
	palette_label->set_text("Entity Palette");
	entity_palette->add_child(palette_label);

	entity_tree = memnew(Tree);
	entity_tree->set_v_size_flags(Control::SIZE_EXPAND_FILL);
	entity_tree->connect("item_selected", callable_mp(this, &LupineTopdownEntityPlacer::_on_entity_selected));
	entity_palette->add_child(entity_tree);

	// Create placement canvas on the right
	placement_canvas = memnew(Control);
	placement_canvas->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	placement_canvas->connect("gui_input", callable_mp(this, &LupineTopdownEntityPlacer::_on_canvas_input));
	main_split->add_child(placement_canvas);

	_setup_entity_palette();
}

void LupineTopdownEntityPlacer::_setup_entity_palette() {
	entity_tree->clear();
	TreeItem *root = entity_tree->create_item();
	root->set_text(0, "Entities");

	// NPCs category
	TreeItem *npcs = entity_tree->create_item(root);
	npcs->set_text(0, "NPCs");

	TreeItem *basic_npc = entity_tree->create_item(npcs);
	basic_npc->set_text(0, "Basic NPC");
	basic_npc->set_metadata(0, "npc_basic");

	TreeItem *merchant = entity_tree->create_item(npcs);
	merchant->set_text(0, "Merchant");
	merchant->set_metadata(0, "npc_merchant");

	// Enemies category
	TreeItem *enemies = entity_tree->create_item(root);
	enemies->set_text(0, "Enemies");

	TreeItem *guard = entity_tree->create_item(enemies);
	guard->set_text(0, "Guard");
	guard->set_metadata(0, "enemy_guard");

	TreeItem *slime = entity_tree->create_item(enemies);
	slime->set_text(0, "Slime");
	slime->set_metadata(0, "enemy_slime");

	// Interactive Objects category
	TreeItem *objects = entity_tree->create_item(root);
	objects->set_text(0, "Interactive Objects");

	TreeItem *chest = entity_tree->create_item(objects);
	chest->set_text(0, "Treasure Chest");
	chest->set_metadata(0, "object_chest");

	TreeItem *door = entity_tree->create_item(objects);
	door->set_text(0, "Door");
	door->set_metadata(0, "object_door");

	// Triggers category
	TreeItem *triggers = entity_tree->create_item(root);
	triggers->set_text(0, "Triggers");

	TreeItem *area_trigger = entity_tree->create_item(triggers);
	area_trigger->set_text(0, "Area Trigger");
	area_trigger->set_metadata(0, "trigger_area");
}

void LupineTopdownEntityPlacer::_on_entity_selected() {
	TreeItem *selected = entity_tree->get_selected();
	if (selected && selected->get_metadata(0) != Variant()) {
		selected_entity_type = selected->get_metadata(0);
	}
}

void LupineTopdownEntityPlacer::_on_canvas_input(const Ref<InputEvent> &p_event) {
	Ref<InputEventMouseButton> mb = p_event;
	if (mb.is_valid() && mb->is_pressed()) {
		Vector2 local_pos = placement_canvas->get_local_mouse_position();

		if (mb->get_button_index() == MouseButton::LEFT) {
			if (!selected_entity_type.is_empty()) {
				_place_entity(local_pos);
			} else {
				_select_entity(local_pos);
			}
		}
	}
}

void LupineTopdownEntityPlacer::_place_entity(Vector2 p_position) {
	if (selected_entity_type.is_empty() || !parent_editor) {
		return;
	}

	// Create new entity data
	TopdownEntityData new_entity;
	new_entity.entity_id = "entity_" + String::num_int64(Time::get_singleton()->get_ticks_msec());
	new_entity.entity_type = selected_entity_type;
	new_entity.position = p_position;

	// Set default properties based on entity type
	if (selected_entity_type.begins_with("npc_")) {
		new_entity.entity_name = "NPC";
		new_entity.scene_path = "res://scenes/characters/NPC.tscn";
		new_entity.script_path = "res://scripts/characters/NPC.gd";
		new_entity.properties["dialogue_id"] = "";
		new_entity.properties["can_move"] = false;
	} else if (selected_entity_type.begins_with("enemy_")) {
		new_entity.entity_name = "Enemy";
		new_entity.scene_path = "res://scenes/enemies/" + selected_entity_type.substr(6).capitalize() + ".tscn";
		new_entity.script_path = "res://scripts/enemies/" + selected_entity_type.substr(6).capitalize() + ".gd";
		new_entity.properties["health"] = 100;
		new_entity.properties["damage"] = 25;
		new_entity.properties["patrol_range"] = 100;
	} else if (selected_entity_type.begins_with("object_")) {
		new_entity.entity_name = "Interactive Object";
		new_entity.scene_path = "res://scenes/objects/" + selected_entity_type.substr(7).capitalize() + ".tscn";
		new_entity.script_path = "res://scripts/objects/" + selected_entity_type.substr(7).capitalize() + ".gd";
		if (selected_entity_type == "object_chest") {
			new_entity.properties["loot_table"] = "";
			new_entity.properties["is_locked"] = false;
		} else if (selected_entity_type == "object_door") {
			new_entity.properties["target_scene"] = "";
			new_entity.properties["spawn_point"] = "";
			new_entity.properties["requires_key"] = false;
		}
	} else if (selected_entity_type.begins_with("trigger_")) {
		new_entity.entity_name = "Trigger";
		new_entity.scene_path = "res://scenes/triggers/AreaTrigger.tscn";
		new_entity.script_path = "res://scripts/triggers/AreaTrigger.gd";
		new_entity.properties["trigger_once"] = false;
		new_entity.properties["event_id"] = "";
	}

	// Add to parent editor
	parent_editor->add_entity(new_entity);

	// Refresh display
	refresh_placed_entities();
}

void LupineTopdownEntityPlacer::_select_entity(Vector2 p_position) {
	// Find entity at position
	Vector<TopdownEntityData> entities = parent_editor->get_placed_entities();
	for (int i = 0; i < entities.size(); i++) {
		Vector2 entity_pos = entities[i].position;
		float distance = p_position.distance_to(entity_pos);
		if (distance < 32.0f) { // 32 pixel selection radius
			selected_entity = const_cast<TopdownEntityData*>(&entities[i]);
			_update_properties_panel();
			break;
		}
	}
}

void LupineTopdownEntityPlacer::_update_properties_panel() {
	// This would update a properties panel to show the selected entity's properties
	// Implementation would depend on having a properties panel UI
}

void LupineTopdownEntityPlacer::refresh_entity_palette() {
	_setup_entity_palette();
}

void LupineTopdownEntityPlacer::refresh_placed_entities() {
	// This would refresh the visual representation of placed entities
	// Implementation would involve updating visual indicators on the canvas
}