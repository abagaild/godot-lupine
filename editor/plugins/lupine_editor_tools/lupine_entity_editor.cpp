#include "lupine_entity_editor.h"
#include "scene/gui/label.h"
#include "scene/gui/separator.h"
#include "scene/gui/box_container.h"
#include "scene/gui/grid_container.h"
#include "core/io/file_access.h"
#include "core/io/json.h"

void LupineEntityEditor::_bind_methods() {
	ClassDB::bind_method(D_METHOD("_on_entity_selected"), &LupineEntityEditor::_on_entity_selected);
	ClassDB::bind_method(D_METHOD("_on_new_entity_pressed"), &LupineEntityEditor::_on_new_entity_pressed);
	ClassDB::bind_method(D_METHOD("_on_duplicate_entity_pressed"), &LupineEntityEditor::_on_duplicate_entity_pressed);
	ClassDB::bind_method(D_METHOD("_on_delete_entity_pressed"), &LupineEntityEditor::_on_delete_entity_pressed);
	ClassDB::bind_method(D_METHOD("_on_import_entity_pressed"), &LupineEntityEditor::_on_import_entity_pressed);
	ClassDB::bind_method(D_METHOD("_on_export_entity_pressed"), &LupineEntityEditor::_on_export_entity_pressed);
	ClassDB::bind_method(D_METHOD("_on_sprite_browse_pressed"), &LupineEntityEditor::_on_sprite_browse_pressed);
	ClassDB::bind_method(D_METHOD("_on_entity_property_changed"), &LupineEntityEditor::_on_entity_property_changed);
}

LupineEntityEditor::LupineEntityEditor() {
	tool_name = "Entity Editor";
	tool_description = "RPG Maker style entity management for players, enemies, and NPCs";
	tool_category = "Core Tools";

	// Initialize all pointers to nullptr
	main_split = nullptr;
	left_panel = nullptr;
	right_panel = nullptr;
	entity_list_container = nullptr;
	entity_list = nullptr;
	new_entity_btn = nullptr;
	duplicate_entity_btn = nullptr;
	delete_entity_btn = nullptr;
	import_entity_btn = nullptr;
	export_entity_btn = nullptr;

	basic_tab = nullptr;
	visuals_tab = nullptr;
	behavior_tab = nullptr;
	stats_tab = nullptr;
	equipment_tab = nullptr;
	ai_tab = nullptr;

	entity_name_edit = nullptr;
	entity_type_option = nullptr;
	entity_description_edit = nullptr;
	entity_enabled_check = nullptr;

	sprite_path_edit = nullptr;
	sprite_browse_btn = nullptr;
	sprite_scale_spin = nullptr;
	animation_set_option = nullptr;
	flip_h_check = nullptr;
	flip_v_check = nullptr;

	movement_type_option = nullptr;
	movement_speed_spin = nullptr;
	can_interact_check = nullptr;
	interaction_script_edit = nullptr;
	blocks_movement_check = nullptr;

	health_spin = nullptr;
	mana_spin = nullptr;
	attack_spin = nullptr;
	defense_spin = nullptr;
	speed_spin = nullptr;
	level_spin = nullptr;
	experience_spin = nullptr;

	current_entity_index = -1;
}

LupineEntityEditor::~LupineEntityEditor() {
	cleanup();
}

void LupineEntityEditor::initialize() {
	if (tool_panel) {
		return; // Already initialized
	}

	print_line("LupineEntityEditor: Initializing Entity Editor...");
	_create_ui();
	_load_entity_database();
	print_line("LupineEntityEditor: Entity Editor initialized successfully");
}

void LupineEntityEditor::cleanup() {
	print_line("LupineEntityEditor: Cleaning up Entity Editor...");

	// Save current entity data before cleanup
	if (current_entity_index >= 0) {
		_save_current_entity();
	}
	_save_entity_database();

	if (tool_panel && tool_panel->get_parent()) {
		tool_panel->get_parent()->remove_child(tool_panel);
	}

	if (tool_panel) {
		memdelete(tool_panel);
		tool_panel = nullptr;
	}

	// Reset all pointers
	main_split = nullptr;
	left_panel = nullptr;
	right_panel = nullptr;
	entity_list_container = nullptr;
	entity_list = nullptr;
	new_entity_btn = nullptr;
	duplicate_entity_btn = nullptr;
	delete_entity_btn = nullptr;
	import_entity_btn = nullptr;
	export_entity_btn = nullptr;

	basic_tab = nullptr;
	visuals_tab = nullptr;
	behavior_tab = nullptr;
	stats_tab = nullptr;
	equipment_tab = nullptr;
	ai_tab = nullptr;

	entity_name_edit = nullptr;
	entity_type_option = nullptr;
	entity_description_edit = nullptr;
	entity_enabled_check = nullptr;

	sprite_path_edit = nullptr;
	sprite_browse_btn = nullptr;
	sprite_scale_spin = nullptr;
	animation_set_option = nullptr;
	flip_h_check = nullptr;
	flip_v_check = nullptr;

	movement_type_option = nullptr;
	movement_speed_spin = nullptr;
	can_interact_check = nullptr;
	interaction_script_edit = nullptr;
	blocks_movement_check = nullptr;

	health_spin = nullptr;
	mana_spin = nullptr;
	attack_spin = nullptr;
	defense_spin = nullptr;
	speed_spin = nullptr;
	level_spin = nullptr;
	experience_spin = nullptr;
}

void LupineEntityEditor::_create_ui() {
	print_line("LupineEntityEditor: Creating UI...");

	// Create main panel
	tool_panel = memnew(Control);
	tool_panel->set_name("EntityEditor");
	tool_panel->set_anchors_and_offsets_preset(Control::PRESET_FULL_RECT);

	// Create main split container (entity list on left, editor on right)
	main_split = memnew(HSplitContainer);
	main_split->set_anchors_and_offsets_preset(Control::PRESET_FULL_RECT);
	tool_panel->add_child(main_split);

	// Create left panel for entity list
	_create_entity_list();

	// Create right panel for entity editing
	right_panel = memnew(TabContainer);
	right_panel->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	main_split->add_child(right_panel);

	// Create editing tabs
	_create_basic_tab();
	_create_visuals_tab();
	_create_behavior_tab();
	_create_stats_tab();
	_create_equipment_tab();
	_create_ai_tab();

	print_line("LupineEntityEditor: UI created successfully");
}

void LupineEntityEditor::_create_entity_list() {
	print_line("LupineEntityEditor: Creating entity list...");

	// Create left panel container
	left_panel = memnew(VBoxContainer);
	left_panel->set_custom_minimum_size(Size2(300, 0));
	main_split->add_child(left_panel);

	// Add title
	Label *title = memnew(Label);
	title->set_text("Entity Database");
	title->set_horizontal_alignment(HORIZONTAL_ALIGNMENT_CENTER);
	left_panel->add_child(title);

	// Add separator
	HSeparator *separator = memnew(HSeparator);
	left_panel->add_child(separator);

	// Create entity list
	entity_list = memnew(ItemList);
	entity_list->set_v_size_flags(Control::SIZE_EXPAND_FILL);
	entity_list->connect("item_selected", callable_mp(this, &LupineEntityEditor::_on_entity_selected));
	left_panel->add_child(entity_list);

	// Create button container
	HBoxContainer *button_container = memnew(HBoxContainer);
	left_panel->add_child(button_container);

	// Add entity management buttons
	new_entity_btn = memnew(Button);
	new_entity_btn->set_text("New");
	new_entity_btn->set_tooltip_text("Create a new entity");
	new_entity_btn->connect("pressed", callable_mp(this, &LupineEntityEditor::_on_new_entity_pressed));
	button_container->add_child(new_entity_btn);

	duplicate_entity_btn = memnew(Button);
	duplicate_entity_btn->set_text("Duplicate");
	duplicate_entity_btn->set_tooltip_text("Duplicate selected entity");
	duplicate_entity_btn->connect("pressed", callable_mp(this, &LupineEntityEditor::_on_duplicate_entity_pressed));
	button_container->add_child(duplicate_entity_btn);

	delete_entity_btn = memnew(Button);
	delete_entity_btn->set_text("Delete");
	delete_entity_btn->set_tooltip_text("Delete selected entity");
	delete_entity_btn->connect("pressed", callable_mp(this, &LupineEntityEditor::_on_delete_entity_pressed));
	button_container->add_child(delete_entity_btn);

	// Create second row of buttons
	HBoxContainer *button_container2 = memnew(HBoxContainer);
	left_panel->add_child(button_container2);

	import_entity_btn = memnew(Button);
	import_entity_btn->set_text("Import");
	import_entity_btn->set_tooltip_text("Import entities from file");
	import_entity_btn->connect("pressed", callable_mp(this, &LupineEntityEditor::_on_import_entity_pressed));
	button_container2->add_child(import_entity_btn);

	export_entity_btn = memnew(Button);
	export_entity_btn->set_text("Export");
	export_entity_btn->set_tooltip_text("Export selected entity to file");
	export_entity_btn->connect("pressed", callable_mp(this, &LupineEntityEditor::_on_export_entity_pressed));
	button_container2->add_child(export_entity_btn);
}

void LupineEntityEditor::_create_basic_tab() {
	print_line("LupineEntityEditor: Creating basic tab...");

	basic_tab = memnew(ScrollContainer);
	basic_tab->set_name("Basic");
	right_panel->add_child(basic_tab);

	VBoxContainer *container = memnew(VBoxContainer);
	basic_tab->add_child(container);

	// Entity name
	Label *name_label = memnew(Label);
	name_label->set_text("Entity Name:");
	container->add_child(name_label);

	entity_name_edit = memnew(LineEdit);
	entity_name_edit->set_placeholder("Enter entity name...");
	entity_name_edit->connect("text_changed", callable_mp(this, &LupineEntityEditor::_on_entity_property_changed));
	container->add_child(entity_name_edit);

	// Entity type
	Label *type_label = memnew(Label);
	type_label->set_text("Entity Type:");
	container->add_child(type_label);

	entity_type_option = memnew(OptionButton);
	entity_type_option->add_item("Player");
	entity_type_option->add_item("NPC");
	entity_type_option->add_item("Enemy");
	entity_type_option->add_item("Boss");
	entity_type_option->add_item("Item");
	entity_type_option->add_item("Interactable");
	entity_type_option->connect("item_selected", callable_mp(this, &LupineEntityEditor::_on_entity_property_changed));
	container->add_child(entity_type_option);

	// Entity description
	Label *desc_label = memnew(Label);
	desc_label->set_text("Description:");
	container->add_child(desc_label);

	entity_description_edit = memnew(TextEdit);
	entity_description_edit->set_custom_minimum_size(Size2(0, 100));
	entity_description_edit->set_placeholder("Enter entity description...");
	entity_description_edit->connect("text_changed", callable_mp(this, &LupineEntityEditor::_on_entity_property_changed));
	container->add_child(entity_description_edit);

	// Entity enabled
	entity_enabled_check = memnew(CheckBox);
	entity_enabled_check->set_text("Entity Enabled");
	entity_enabled_check->set_pressed(true);
	entity_enabled_check->connect("toggled", callable_mp(this, &LupineEntityEditor::_on_entity_property_changed));
	container->add_child(entity_enabled_check);
}

void LupineEntityEditor::_create_visuals_tab() {
	print_line("LupineEntityEditor: Creating visuals tab...");

	visuals_tab = memnew(ScrollContainer);
	visuals_tab->set_name("Visuals");
	right_panel->add_child(visuals_tab);

	VBoxContainer *container = memnew(VBoxContainer);
	visuals_tab->add_child(container);

	// Sprite path
	Label *sprite_label = memnew(Label);
	sprite_label->set_text("Sprite Path:");
	container->add_child(sprite_label);

	HBoxContainer *sprite_container = memnew(HBoxContainer);
	container->add_child(sprite_container);

	sprite_path_edit = memnew(LineEdit);
	sprite_path_edit->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	sprite_path_edit->set_placeholder("res://path/to/sprite.png");
	sprite_path_edit->connect("text_changed", callable_mp(this, &LupineEntityEditor::_on_entity_property_changed));
	sprite_container->add_child(sprite_path_edit);

	sprite_browse_btn = memnew(Button);
	sprite_browse_btn->set_text("Browse");
	sprite_browse_btn->connect("pressed", callable_mp(this, &LupineEntityEditor::_on_sprite_browse_pressed));
	sprite_container->add_child(sprite_browse_btn);

	// Sprite scale
	Label *scale_label = memnew(Label);
	scale_label->set_text("Sprite Scale:");
	container->add_child(scale_label);

	sprite_scale_spin = memnew(SpinBox);
	sprite_scale_spin->set_min(0.1);
	sprite_scale_spin->set_max(10.0);
	sprite_scale_spin->set_step(0.1);
	sprite_scale_spin->set_value(1.0);
	sprite_scale_spin->connect("value_changed", callable_mp(this, &LupineEntityEditor::_on_entity_property_changed));
	container->add_child(sprite_scale_spin);

	// Animation set
	Label *anim_label = memnew(Label);
	anim_label->set_text("Animation Set:");
	container->add_child(anim_label);

	animation_set_option = memnew(OptionButton);
	animation_set_option->add_item("None");
	animation_set_option->add_item("Basic 4-Direction");
	animation_set_option->add_item("Basic 8-Direction");
	animation_set_option->add_item("Combat Animations");
	animation_set_option->add_item("Custom");
	animation_set_option->connect("item_selected", callable_mp(this, &LupineEntityEditor::_on_entity_property_changed));
	container->add_child(animation_set_option);

	// Flip options
	flip_h_check = memnew(CheckBox);
	flip_h_check->set_text("Flip Horizontal");
	flip_h_check->connect("toggled", callable_mp(this, &LupineEntityEditor::_on_entity_property_changed));
	container->add_child(flip_h_check);

	flip_v_check = memnew(CheckBox);
	flip_v_check->set_text("Flip Vertical");
	flip_v_check->connect("toggled", callable_mp(this, &LupineEntityEditor::_on_entity_property_changed));
	container->add_child(flip_v_check);
}

void LupineEntityEditor::_create_behavior_tab() {
	print_line("LupineEntityEditor: Creating behavior tab...");

	behavior_tab = memnew(ScrollContainer);
	behavior_tab->set_name("Behavior");
	right_panel->add_child(behavior_tab);

	VBoxContainer *container = memnew(VBoxContainer);
	behavior_tab->add_child(container);

	// Movement type
	Label *movement_label = memnew(Label);
	movement_label->set_text("Movement Type:");
	container->add_child(movement_label);

	movement_type_option = memnew(OptionButton);
	movement_type_option->add_item("None");
	movement_type_option->add_item("Random");
	movement_type_option->add_item("Patrol");
	movement_type_option->add_item("Follow Player");
	movement_type_option->add_item("Custom Script");
	movement_type_option->connect("item_selected", callable_mp(this, &LupineEntityEditor::_on_entity_property_changed));
	container->add_child(movement_type_option);

	// Movement speed
	Label *speed_label = memnew(Label);
	speed_label->set_text("Movement Speed:");
	container->add_child(speed_label);

	movement_speed_spin = memnew(SpinBox);
	movement_speed_spin->set_min(0.0);
	movement_speed_spin->set_max(1000.0);
	movement_speed_spin->set_step(10.0);
	movement_speed_spin->set_value(100.0);
	movement_speed_spin->connect("value_changed", callable_mp(this, &LupineEntityEditor::_on_entity_property_changed));
	container->add_child(movement_speed_spin);

	// Interaction options
	can_interact_check = memnew(CheckBox);
	can_interact_check->set_text("Can Interact");
	can_interact_check->connect("toggled", callable_mp(this, &LupineEntityEditor::_on_entity_property_changed));
	container->add_child(can_interact_check);

	Label *script_label = memnew(Label);
	script_label->set_text("Interaction Script:");
	container->add_child(script_label);

	interaction_script_edit = memnew(LineEdit);
	interaction_script_edit->set_placeholder("res://scripts/interaction.gd");
	interaction_script_edit->connect("text_changed", callable_mp(this, &LupineEntityEditor::_on_entity_property_changed));
	container->add_child(interaction_script_edit);

	// Movement blocking
	blocks_movement_check = memnew(CheckBox);
	blocks_movement_check->set_text("Blocks Movement");
	blocks_movement_check->connect("toggled", callable_mp(this, &LupineEntityEditor::_on_entity_property_changed));
	container->add_child(blocks_movement_check);
}

void LupineEntityEditor::_create_stats_tab() {
	print_line("LupineEntityEditor: Creating stats tab...");

	stats_tab = memnew(ScrollContainer);
	stats_tab->set_name("Stats");
	right_panel->add_child(stats_tab);

	VBoxContainer *container = memnew(VBoxContainer);
	stats_tab->add_child(container);

	// Create a grid for stats
	GridContainer *stats_grid = memnew(GridContainer);
	stats_grid->set_columns(2);
	container->add_child(stats_grid);

	// Health
	Label *health_label = memnew(Label);
	health_label->set_text("Health:");
	stats_grid->add_child(health_label);

	health_spin = memnew(SpinBox);
	health_spin->set_min(1);
	health_spin->set_max(9999);
	health_spin->set_value(100);
	health_spin->connect("value_changed", callable_mp(this, &LupineEntityEditor::_on_entity_property_changed));
	stats_grid->add_child(health_spin);

	// Mana
	Label *mana_label = memnew(Label);
	mana_label->set_text("Mana:");
	stats_grid->add_child(mana_label);

	mana_spin = memnew(SpinBox);
	mana_spin->set_min(0);
	mana_spin->set_max(9999);
	mana_spin->set_value(50);
	mana_spin->connect("value_changed", callable_mp(this, &LupineEntityEditor::_on_entity_property_changed));
	stats_grid->add_child(mana_spin);

	// Attack
	Label *attack_label = memnew(Label);
	attack_label->set_text("Attack:");
	stats_grid->add_child(attack_label);

	attack_spin = memnew(SpinBox);
	attack_spin->set_min(0);
	attack_spin->set_max(999);
	attack_spin->set_value(10);
	attack_spin->connect("value_changed", callable_mp(this, &LupineEntityEditor::_on_entity_property_changed));
	stats_grid->add_child(attack_spin);

	// Defense
	Label *defense_label = memnew(Label);
	defense_label->set_text("Defense:");
	stats_grid->add_child(defense_label);

	defense_spin = memnew(SpinBox);
	defense_spin->set_min(0);
	defense_spin->set_max(999);
	defense_spin->set_value(5);
	defense_spin->connect("value_changed", callable_mp(this, &LupineEntityEditor::_on_entity_property_changed));
	stats_grid->add_child(defense_spin);

	// Speed
	Label *speed_label = memnew(Label);
	speed_label->set_text("Speed:");
	stats_grid->add_child(speed_label);

	speed_spin = memnew(SpinBox);
	speed_spin->set_min(0);
	speed_spin->set_max(999);
	speed_spin->set_value(10);
	speed_spin->connect("value_changed", callable_mp(this, &LupineEntityEditor::_on_entity_property_changed));
	stats_grid->add_child(speed_spin);

	// Level
	Label *level_label = memnew(Label);
	level_label->set_text("Level:");
	stats_grid->add_child(level_label);

	level_spin = memnew(SpinBox);
	level_spin->set_min(1);
	level_spin->set_max(999);
	level_spin->set_value(1);
	level_spin->connect("value_changed", callable_mp(this, &LupineEntityEditor::_on_entity_property_changed));
	stats_grid->add_child(level_spin);

	// Experience
	Label *exp_label = memnew(Label);
	exp_label->set_text("Experience:");
	stats_grid->add_child(exp_label);

	experience_spin = memnew(SpinBox);
	experience_spin->set_min(0);
	experience_spin->set_max(999999);
	experience_spin->set_value(0);
	experience_spin->connect("value_changed", callable_mp(this, &LupineEntityEditor::_on_entity_property_changed));
	stats_grid->add_child(experience_spin);
}

void LupineEntityEditor::_create_equipment_tab() {
	equipment_tab = memnew(ScrollContainer);
	equipment_tab->set_name("Equipment");
	right_panel->add_child(equipment_tab);

	VBoxContainer *container = memnew(VBoxContainer);
	equipment_tab->add_child(container);

	Label *placeholder = memnew(Label);
	placeholder->set_text("Equipment system - Coming soon!");
	container->add_child(placeholder);
}

void LupineEntityEditor::_create_ai_tab() {
	ai_tab = memnew(ScrollContainer);
	ai_tab->set_name("AI");
	right_panel->add_child(ai_tab);

	VBoxContainer *container = memnew(VBoxContainer);
	ai_tab->add_child(container);

	Label *placeholder = memnew(Label);
	placeholder->set_text("AI behavior system - Coming soon!");
	container->add_child(placeholder);
}

// Event handlers
void LupineEntityEditor::_on_entity_selected(int p_index) {
	print_line("LupineEntityEditor: Entity selected: " + itos(p_index));

	// Save current entity before switching
	if (current_entity_index >= 0) {
		_save_current_entity();
	}

	current_entity_index = p_index;
	_load_entity_data(p_index);
}

void LupineEntityEditor::_on_new_entity_pressed() {
	print_line("LupineEntityEditor: Creating new entity...");
	_create_new_entity();
}

void LupineEntityEditor::_on_duplicate_entity_pressed() {
	if (current_entity_index < 0 || current_entity_index >= entity_database.size()) {
		print_line("LupineEntityEditor: No entity selected for duplication");
		return;
	}

	Dictionary entity_data = entity_database[current_entity_index];
	entity_data["name"] = String(entity_data["name"]) + " (Copy)";
	entity_database.push_back(entity_data);
	_update_entity_list();

	// Select the new entity
	entity_list->select(entity_database.size() - 1);
	_on_entity_selected(entity_database.size() - 1);
}

void LupineEntityEditor::_on_delete_entity_pressed() {
	if (current_entity_index < 0 || current_entity_index >= entity_database.size()) {
		print_line("LupineEntityEditor: No entity selected for deletion");
		return;
	}

	entity_database.remove_at(current_entity_index);
	_update_entity_list();

	// Select previous entity or clear selection
	if (entity_database.size() > 0) {
		int new_index = MAX(0, current_entity_index - 1);
		entity_list->select(new_index);
		_on_entity_selected(new_index);
	} else {
		current_entity_index = -1;
	}
}

void LupineEntityEditor::_on_import_entity_pressed() {
	print_line("LupineEntityEditor: Import entity - TODO");
	// TODO: Implement file dialog for importing
}

void LupineEntityEditor::_on_export_entity_pressed() {
	print_line("LupineEntityEditor: Export entity - TODO");
	// TODO: Implement file dialog for exporting
}

void LupineEntityEditor::_on_sprite_browse_pressed() {
	print_line("LupineEntityEditor: Browse sprite - TODO");
	// TODO: Implement file dialog for sprite selection
}

void LupineEntityEditor::_on_entity_property_changed() {
	// Auto-save when properties change
	if (current_entity_index >= 0) {
		_save_current_entity();
	}
}

// Data management methods
Dictionary LupineEntityEditor::_create_default_entity_data() {
	Dictionary data;
	data["name"] = "New Entity";
	data["type"] = 0; // Player
	data["description"] = "";
	data["enabled"] = true;

	// Visual properties
	data["sprite_path"] = "";
	data["sprite_scale"] = 1.0;
	data["animation_set"] = 0;
	data["flip_h"] = false;
	data["flip_v"] = false;

	// Behavior properties
	data["movement_type"] = 0;
	data["movement_speed"] = 100.0;
	data["can_interact"] = false;
	data["interaction_script"] = "";
	data["blocks_movement"] = false;

	// Stats
	data["health"] = 100;
	data["mana"] = 50;
	data["attack"] = 10;
	data["defense"] = 5;
	data["speed"] = 10;
	data["level"] = 1;
	data["experience"] = 0;

	return data;
}

void LupineEntityEditor::_create_new_entity() {
	Dictionary new_entity = _create_default_entity_data();
	entity_database.push_back(new_entity);
	_update_entity_list();

	// Select the new entity
	entity_list->select(entity_database.size() - 1);
	_on_entity_selected(entity_database.size() - 1);
}

void LupineEntityEditor::_update_entity_list() {
	if (!entity_list) return;

	entity_list->clear();
	for (int i = 0; i < entity_database.size(); i++) {
		Dictionary entity = entity_database[i];
		String name = entity.get("name", "Unnamed Entity");
		String type_names[] = {"Player", "NPC", "Enemy", "Boss", "Item", "Interactable"};
		int type = entity.get("type", 0);
		String type_name = (type >= 0 && type < 6) ? type_names[type] : "Unknown";

		entity_list->add_item(name + " (" + type_name + ")");
	}
}

void LupineEntityEditor::_load_entity_data(int p_index) {
	if (p_index < 0 || p_index >= entity_database.size()) {
		return;
	}

	current_entity_data = entity_database[p_index];

	// Load basic properties
	if (entity_name_edit) entity_name_edit->set_text(current_entity_data.get("name", ""));
	if (entity_type_option) entity_type_option->select(current_entity_data.get("type", 0));
	if (entity_description_edit) entity_description_edit->set_text(current_entity_data.get("description", ""));
	if (entity_enabled_check) entity_enabled_check->set_pressed(current_entity_data.get("enabled", true));

	// Load visual properties
	if (sprite_path_edit) sprite_path_edit->set_text(current_entity_data.get("sprite_path", ""));
	if (sprite_scale_spin) sprite_scale_spin->set_value(current_entity_data.get("sprite_scale", 1.0));
	if (animation_set_option) animation_set_option->select(current_entity_data.get("animation_set", 0));
	if (flip_h_check) flip_h_check->set_pressed(current_entity_data.get("flip_h", false));
	if (flip_v_check) flip_v_check->set_pressed(current_entity_data.get("flip_v", false));

	// Load behavior properties
	if (movement_type_option) movement_type_option->select(current_entity_data.get("movement_type", 0));
	if (movement_speed_spin) movement_speed_spin->set_value(current_entity_data.get("movement_speed", 100.0));
	if (can_interact_check) can_interact_check->set_pressed(current_entity_data.get("can_interact", false));
	if (interaction_script_edit) interaction_script_edit->set_text(current_entity_data.get("interaction_script", ""));
	if (blocks_movement_check) blocks_movement_check->set_pressed(current_entity_data.get("blocks_movement", false));

	// Load stats
	if (health_spin) health_spin->set_value(current_entity_data.get("health", 100));
	if (mana_spin) mana_spin->set_value(current_entity_data.get("mana", 50));
	if (attack_spin) attack_spin->set_value(current_entity_data.get("attack", 10));
	if (defense_spin) defense_spin->set_value(current_entity_data.get("defense", 5));
	if (speed_spin) speed_spin->set_value(current_entity_data.get("speed", 10));
	if (level_spin) level_spin->set_value(current_entity_data.get("level", 1));
	if (experience_spin) experience_spin->set_value(current_entity_data.get("experience", 0));
}

void LupineEntityEditor::_save_current_entity() {
	if (current_entity_index < 0 || current_entity_index >= entity_database.size()) {
		return;
	}

	// Save basic properties
	if (entity_name_edit) current_entity_data["name"] = entity_name_edit->get_text();
	if (entity_type_option) current_entity_data["type"] = entity_type_option->get_selected();
	if (entity_description_edit) current_entity_data["description"] = entity_description_edit->get_text();
	if (entity_enabled_check) current_entity_data["enabled"] = entity_enabled_check->is_pressed();

	// Save visual properties
	if (sprite_path_edit) current_entity_data["sprite_path"] = sprite_path_edit->get_text();
	if (sprite_scale_spin) current_entity_data["sprite_scale"] = sprite_scale_spin->get_value();
	if (animation_set_option) current_entity_data["animation_set"] = animation_set_option->get_selected();
	if (flip_h_check) current_entity_data["flip_h"] = flip_h_check->is_pressed();
	if (flip_v_check) current_entity_data["flip_v"] = flip_v_check->is_pressed();

	// Save behavior properties
	if (movement_type_option) current_entity_data["movement_type"] = movement_type_option->get_selected();
	if (movement_speed_spin) current_entity_data["movement_speed"] = movement_speed_spin->get_value();
	if (can_interact_check) current_entity_data["can_interact"] = can_interact_check->is_pressed();
	if (interaction_script_edit) current_entity_data["interaction_script"] = interaction_script_edit->get_text();
	if (blocks_movement_check) current_entity_data["blocks_movement"] = blocks_movement_check->is_pressed();

	// Save stats
	if (health_spin) current_entity_data["health"] = health_spin->get_value();
	if (mana_spin) current_entity_data["mana"] = mana_spin->get_value();
	if (attack_spin) current_entity_data["attack"] = attack_spin->get_value();
	if (defense_spin) current_entity_data["defense"] = defense_spin->get_value();
	if (speed_spin) current_entity_data["speed"] = speed_spin->get_value();
	if (level_spin) current_entity_data["level"] = level_spin->get_value();
	if (experience_spin) current_entity_data["experience"] = experience_spin->get_value();

	// Update the database
	entity_database.set(current_entity_index, current_entity_data);

	// Update the list display
	_update_entity_list();
}

void LupineEntityEditor::_load_entity_database() {
	print_line("LupineEntityEditor: Loading entity database...");

	// For now, create some default entities
	entity_database.clear();

	// Create a default player entity
	Dictionary player_entity = _create_default_entity_data();
	player_entity["name"] = "Player";
	player_entity["type"] = 0; // Player
	player_entity["description"] = "The main character";
	entity_database.push_back(player_entity);

	// Create a default NPC entity
	Dictionary npc_entity = _create_default_entity_data();
	npc_entity["name"] = "Village NPC";
	npc_entity["type"] = 1; // NPC
	npc_entity["description"] = "A friendly villager";
	npc_entity["can_interact"] = true;
	entity_database.push_back(npc_entity);

	// Create a default enemy entity
	Dictionary enemy_entity = _create_default_entity_data();
	enemy_entity["name"] = "Goblin";
	enemy_entity["type"] = 2; // Enemy
	enemy_entity["description"] = "A small hostile creature";
	enemy_entity["health"] = 50;
	enemy_entity["attack"] = 15;
	enemy_entity["defense"] = 3;
	entity_database.push_back(enemy_entity);

	_update_entity_list();
	print_line("LupineEntityEditor: Entity database loaded with " + itos(entity_database.size()) + " entities");
}

void LupineEntityEditor::_save_entity_database() {
	print_line("LupineEntityEditor: Saving entity database...");
	// TODO: Implement saving to file
	// For now, just print that we're saving
	print_line("LupineEntityEditor: Entity database saved (" + itos(entity_database.size()) + " entities)");
}
