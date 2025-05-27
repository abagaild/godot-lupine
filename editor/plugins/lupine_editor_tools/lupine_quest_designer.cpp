#include "lupine_quest_designer.h"
#include "editor/editor_node.h"
#include "scene/gui/label.h"
#include "scene/gui/separator.h"
#include "scene/gui/button.h"
#include "scene/gui/grid_container.h"
#include "core/io/file_access.h"
#include "core/string/ustring.h"
#include "core/os/os.h"

// LupineQuestDesigner Implementation

void LupineQuestDesigner::_bind_methods() {
	ClassDB::bind_method(D_METHOD("_on_quest_selected", "index"), &LupineQuestDesigner::_on_quest_selected);
	ClassDB::bind_method(D_METHOD("_on_new_quest_pressed"), &LupineQuestDesigner::_on_new_quest_pressed);
	ClassDB::bind_method(D_METHOD("_on_delete_quest_pressed"), &LupineQuestDesigner::_on_delete_quest_pressed);
	ClassDB::bind_method(D_METHOD("_on_duplicate_quest_pressed"), &LupineQuestDesigner::_on_duplicate_quest_pressed);
	ClassDB::bind_method(D_METHOD("_on_quest_property_changed"), &LupineQuestDesigner::_on_quest_property_changed);
	ClassDB::bind_method(D_METHOD("_on_objective_selected"), &LupineQuestDesigner::_on_objective_selected);
	ClassDB::bind_method(D_METHOD("_on_add_objective_pressed"), &LupineQuestDesigner::_on_add_objective_pressed);
	ClassDB::bind_method(D_METHOD("_on_remove_objective_pressed"), &LupineQuestDesigner::_on_remove_objective_pressed);
	ClassDB::bind_method(D_METHOD("_on_objective_property_changed"), &LupineQuestDesigner::_on_objective_property_changed);
	ClassDB::bind_method(D_METHOD("_on_reward_selected"), &LupineQuestDesigner::_on_reward_selected);
	ClassDB::bind_method(D_METHOD("_on_add_reward_pressed"), &LupineQuestDesigner::_on_add_reward_pressed);
	ClassDB::bind_method(D_METHOD("_on_remove_reward_pressed"), &LupineQuestDesigner::_on_remove_reward_pressed);
	ClassDB::bind_method(D_METHOD("_on_reward_property_changed"), &LupineQuestDesigner::_on_reward_property_changed);
	ClassDB::bind_method(D_METHOD("_on_add_flow_node_pressed"), &LupineQuestDesigner::_on_add_flow_node_pressed);
}

LupineQuestDesigner::LupineQuestDesigner() {
	tool_name = "Quest Designer";
	tool_description = "Visual quest creation and management tools";
	tool_category = "Narrative Design";

	main_split = nullptr;
	left_panel = nullptr;
	main_tabs = nullptr;
	quest_list_panel = nullptr;
	quest_list = nullptr;
	new_quest_button = nullptr;
	delete_quest_button = nullptr;
	duplicate_quest_button = nullptr;
	quest_editor_panel = nullptr;
	quest_title_edit = nullptr;
	quest_description_edit = nullptr;
	quest_category_edit = nullptr;
	quest_level_spin = nullptr;
	quest_status_option = nullptr;
	objectives_panel = nullptr;
	objectives_tree = nullptr;
	add_objective_button = nullptr;
	remove_objective_button = nullptr;
	objective_details_panel = nullptr;
	rewards_panel = nullptr;
	rewards_tree = nullptr;
	add_reward_button = nullptr;
	remove_reward_button = nullptr;
	reward_details_panel = nullptr;
	quest_flow_graph = nullptr;
	add_node_button = nullptr;

	current_quest_id = "";
	current_objective_id = "";
	current_reward_id = "";
}

LupineQuestDesigner::~LupineQuestDesigner() {
	cleanup();
}

void LupineQuestDesigner::initialize() {
	if (tool_panel) {
		return; // Already initialized
	}

	_create_ui();
	load_quest_database();
	_populate_quest_list();
}

void LupineQuestDesigner::cleanup() {
	if (tool_panel && tool_panel->get_parent()) {
		tool_panel->get_parent()->remove_child(tool_panel);
	}

	if (tool_panel) {
		memdelete(tool_panel);
		tool_panel = nullptr;
	}

	// Clear references
	main_split = nullptr;
	left_panel = nullptr;
	main_tabs = nullptr;
	quest_list_panel = nullptr;
	quest_list = nullptr;
	new_quest_button = nullptr;
	delete_quest_button = nullptr;
	duplicate_quest_button = nullptr;
	quest_editor_panel = nullptr;
	quest_title_edit = nullptr;
	quest_description_edit = nullptr;
	quest_category_edit = nullptr;
	quest_level_spin = nullptr;
	quest_status_option = nullptr;
	objectives_panel = nullptr;
	objectives_tree = nullptr;
	add_objective_button = nullptr;
	remove_objective_button = nullptr;
	objective_details_panel = nullptr;
	rewards_panel = nullptr;
	rewards_tree = nullptr;
	add_reward_button = nullptr;
	remove_reward_button = nullptr;
	reward_details_panel = nullptr;
	quest_flow_graph = nullptr;
	add_node_button = nullptr;
}

void LupineQuestDesigner::_create_ui() {
	// Create main panel
	tool_panel = memnew(Control);
	tool_panel->set_name("QuestDesigner");
	tool_panel->set_anchors_and_offsets_preset(Control::PRESET_FULL_RECT);

	// Create main split container
	main_split = memnew(HSplitContainer);
	main_split->set_anchors_and_offsets_preset(Control::PRESET_FULL_RECT);
	tool_panel->add_child(main_split);

	// Create left panel (quest list)
	left_panel = memnew(VBoxContainer);
	left_panel->set_custom_minimum_size(Size2(250, 0));
	main_split->add_child(left_panel);

	// Quest list header
	Label *quest_list_label = memnew(Label);
	quest_list_label->set_text("Quest Database");
	left_panel->add_child(quest_list_label);

	// Quest management buttons
	HBoxContainer *quest_buttons = memnew(HBoxContainer);
	left_panel->add_child(quest_buttons);

	new_quest_button = memnew(Button);
	new_quest_button->set_text("New");
	new_quest_button->connect("pressed", callable_mp(this, &LupineQuestDesigner::_on_new_quest_pressed));
	quest_buttons->add_child(new_quest_button);

	delete_quest_button = memnew(Button);
	delete_quest_button->set_text("Delete");
	delete_quest_button->set_disabled(true);
	delete_quest_button->connect("pressed", callable_mp(this, &LupineQuestDesigner::_on_delete_quest_pressed));
	quest_buttons->add_child(delete_quest_button);

	duplicate_quest_button = memnew(Button);
	duplicate_quest_button->set_text("Duplicate");
	duplicate_quest_button->set_disabled(true);
	duplicate_quest_button->connect("pressed", callable_mp(this, &LupineQuestDesigner::_on_duplicate_quest_pressed));
	quest_buttons->add_child(duplicate_quest_button);

	// Quest list
	quest_list = memnew(ItemList);
	quest_list->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	quest_list->set_v_size_flags(Control::SIZE_EXPAND_FILL);
	quest_list->connect("item_selected", callable_mp(this, &LupineQuestDesigner::_on_quest_selected));
	left_panel->add_child(quest_list);

	// Create main tabs
	main_tabs = memnew(TabContainer);
	main_tabs->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	main_tabs->set_v_size_flags(Control::SIZE_EXPAND_FILL);
	main_split->add_child(main_tabs);

	_create_quest_editor_tab();
	_create_objectives_tab();
	_create_rewards_tab();
	_create_flow_designer_tab();
}

void LupineQuestDesigner::_create_quest_editor_tab() {
	quest_editor_panel = memnew(VBoxContainer);
	quest_editor_panel->set_name("Quest Details");
	main_tabs->add_child(quest_editor_panel);

	// Quest title
	Label *title_label = memnew(Label);
	title_label->set_text("Quest Title:");
	quest_editor_panel->add_child(title_label);

	quest_title_edit = memnew(LineEdit);
	quest_title_edit->set_placeholder("Enter quest title...");
	quest_title_edit->connect("text_changed", callable_mp(this, &LupineQuestDesigner::_on_quest_property_changed));
	quest_editor_panel->add_child(quest_title_edit);

	// Quest description
	Label *desc_label = memnew(Label);
	desc_label->set_text("Description:");
	quest_editor_panel->add_child(desc_label);

	quest_description_edit = memnew(TextEdit);
	quest_description_edit->set_custom_minimum_size(Size2(0, 100));
	quest_description_edit->set_placeholder("Enter quest description...");
	quest_description_edit->connect("text_changed", callable_mp(this, &LupineQuestDesigner::_on_quest_property_changed));
	quest_editor_panel->add_child(quest_description_edit);

	// Quest properties grid
	GridContainer *properties_grid = memnew(GridContainer);
	properties_grid->set_columns(2);
	quest_editor_panel->add_child(properties_grid);

	// Category
	Label *category_label = memnew(Label);
	category_label->set_text("Category:");
	properties_grid->add_child(category_label);

	quest_category_edit = memnew(LineEdit);
	quest_category_edit->set_placeholder("Main, Side, Daily, etc.");
	quest_category_edit->connect("text_changed", callable_mp(this, &LupineQuestDesigner::_on_quest_property_changed));
	properties_grid->add_child(quest_category_edit);

	// Level requirement
	Label *level_label = memnew(Label);
	level_label->set_text("Level Requirement:");
	properties_grid->add_child(level_label);

	quest_level_spin = memnew(SpinBox);
	quest_level_spin->set_min(1);
	quest_level_spin->set_max(100);
	quest_level_spin->set_value(1);
	quest_level_spin->connect("value_changed", callable_mp(this, &LupineQuestDesigner::_on_quest_property_changed));
	properties_grid->add_child(quest_level_spin);

	// Status
	Label *status_label = memnew(Label);
	status_label->set_text("Status:");
	properties_grid->add_child(status_label);

	quest_status_option = memnew(OptionButton);
	quest_status_option->add_item("Inactive");
	quest_status_option->add_item("Available");
	quest_status_option->add_item("Active");
	quest_status_option->add_item("Completed");
	quest_status_option->add_item("Failed");
	quest_status_option->connect("item_selected", callable_mp(this, &LupineQuestDesigner::_on_quest_property_changed));
	properties_grid->add_child(quest_status_option);
}

void LupineQuestDesigner::_create_objectives_tab() {
	objectives_panel = memnew(VBoxContainer);
	objectives_panel->set_name("Objectives");
	main_tabs->add_child(objectives_panel);

	// Objectives header
	HBoxContainer *objectives_header = memnew(HBoxContainer);
	objectives_panel->add_child(objectives_header);

	Label *objectives_label = memnew(Label);
	objectives_label->set_text("Quest Objectives");
	objectives_header->add_child(objectives_label);

	add_objective_button = memnew(Button);
	add_objective_button->set_text("Add Objective");
	add_objective_button->connect("pressed", callable_mp(this, &LupineQuestDesigner::_on_add_objective_pressed));
	objectives_header->add_child(add_objective_button);

	remove_objective_button = memnew(Button);
	remove_objective_button->set_text("Remove");
	remove_objective_button->set_disabled(true);
	remove_objective_button->connect("pressed", callable_mp(this, &LupineQuestDesigner::_on_remove_objective_pressed));
	objectives_header->add_child(remove_objective_button);

	// Objectives tree
	objectives_tree = memnew(Tree);
	objectives_tree->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	objectives_tree->set_v_size_flags(Control::SIZE_EXPAND_FILL);
	objectives_tree->set_hide_root(true);
	objectives_tree->connect("item_selected", callable_mp(this, &LupineQuestDesigner::_on_objective_selected));
	objectives_panel->add_child(objectives_tree);

	// Objective details panel
	objective_details_panel = memnew(VBoxContainer);
	objective_details_panel->set_custom_minimum_size(Size2(0, 200));
	objectives_panel->add_child(objective_details_panel);

	Label *details_label = memnew(Label);
	details_label->set_text("Objective Details");
	objective_details_panel->add_child(details_label);

	HSeparator *separator = memnew(HSeparator);
	objective_details_panel->add_child(separator);
}

void LupineQuestDesigner::_create_rewards_tab() {
	rewards_panel = memnew(VBoxContainer);
	rewards_panel->set_name("Rewards");
	main_tabs->add_child(rewards_panel);

	// Rewards header
	HBoxContainer *rewards_header = memnew(HBoxContainer);
	rewards_panel->add_child(rewards_header);

	Label *rewards_label = memnew(Label);
	rewards_label->set_text("Quest Rewards");
	rewards_header->add_child(rewards_label);

	add_reward_button = memnew(Button);
	add_reward_button->set_text("Add Reward");
	add_reward_button->connect("pressed", callable_mp(this, &LupineQuestDesigner::_on_add_reward_pressed));
	rewards_header->add_child(add_reward_button);

	remove_reward_button = memnew(Button);
	remove_reward_button->set_text("Remove");
	remove_reward_button->set_disabled(true);
	remove_reward_button->connect("pressed", callable_mp(this, &LupineQuestDesigner::_on_remove_reward_pressed));
	rewards_header->add_child(remove_reward_button);

	// Rewards tree
	rewards_tree = memnew(Tree);
	rewards_tree->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	rewards_tree->set_v_size_flags(Control::SIZE_EXPAND_FILL);
	rewards_tree->set_hide_root(true);
	rewards_tree->connect("item_selected", callable_mp(this, &LupineQuestDesigner::_on_reward_selected));
	rewards_panel->add_child(rewards_tree);

	// Reward details panel
	reward_details_panel = memnew(VBoxContainer);
	reward_details_panel->set_custom_minimum_size(Size2(0, 200));
	rewards_panel->add_child(reward_details_panel);

	Label *details_label = memnew(Label);
	details_label->set_text("Reward Details");
	reward_details_panel->add_child(details_label);

	HSeparator *separator = memnew(HSeparator);
	reward_details_panel->add_child(separator);
}

void LupineQuestDesigner::_create_flow_designer_tab() {
	VBoxContainer *flow_panel = memnew(VBoxContainer);
	flow_panel->set_name("Quest Flow");
	main_tabs->add_child(flow_panel);

	// Flow header
	HBoxContainer *flow_header = memnew(HBoxContainer);
	flow_panel->add_child(flow_header);

	Label *flow_label = memnew(Label);
	flow_label->set_text("Quest Flow Designer");
	flow_header->add_child(flow_label);

	add_node_button = memnew(Button);
	add_node_button->set_text("Add Quest Node");
	add_node_button->connect("pressed", callable_mp(this, &LupineQuestDesigner::_on_add_flow_node_pressed));
	flow_header->add_child(add_node_button);

	// Quest flow graph
	quest_flow_graph = memnew(GraphEdit);
	quest_flow_graph->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	quest_flow_graph->set_v_size_flags(Control::SIZE_EXPAND_FILL);
	quest_flow_graph->set_right_disconnects(true);
	flow_panel->add_child(quest_flow_graph);
}

void LupineQuestDesigner::_populate_quest_list() {
	if (!quest_list) {
		return;
	}

	quest_list->clear();

	for (const LupineQuest &quest : quest_database) {
		int item_index = quest_list->add_item(quest.title.is_empty() ? quest.id : quest.title);
		quest_list->set_item_metadata(item_index, quest.id);

		// Set icon based on quest status
		String icon_name;
		switch (quest.status) {
			case QUEST_STATUS_INACTIVE:
				icon_name = "StatusWarning";
				break;
			case QUEST_STATUS_AVAILABLE:
				icon_name = "StatusSuccess";
				break;
			case QUEST_STATUS_ACTIVE:
				icon_name = "StatusError";
				break;
			case QUEST_STATUS_COMPLETED:
				icon_name = "StatusSuccess";
				break;
			case QUEST_STATUS_FAILED:
				icon_name = "StatusError";
				break;
		}

		Ref<Texture2D> icon = EditorNode::get_singleton()->get_gui_base()->get_theme_icon(icon_name, "EditorIcons");
		if (icon.is_valid()) {
			quest_list->set_item_icon(item_index, icon);
		}
	}
}

void LupineQuestDesigner::_update_quest_editor() {
	if (current_quest_id.is_empty()) {
		// Clear all fields
		if (quest_title_edit) quest_title_edit->set_text("");
		if (quest_description_edit) quest_description_edit->set_text("");
		if (quest_category_edit) quest_category_edit->set_text("");
		if (quest_level_spin) quest_level_spin->set_value(1);
		if (quest_status_option) quest_status_option->select(0);
		return;
	}

	LupineQuest *quest = get_quest(current_quest_id);
	if (!quest) {
		return;
	}

	// Update UI with quest data
	if (quest_title_edit) quest_title_edit->set_text(quest->title);
	if (quest_description_edit) quest_description_edit->set_text(quest->description);
	if (quest_category_edit) quest_category_edit->set_text(quest->category);
	if (quest_level_spin) quest_level_spin->set_value(quest->level_requirement);
	if (quest_status_option) quest_status_option->select(quest->status);

	_update_objectives_tree();
	_update_rewards_tree();
}

void LupineQuestDesigner::_update_objectives_tree() {
	if (!objectives_tree) {
		return;
	}

	objectives_tree->clear();

	if (current_quest_id.is_empty()) {
		return;
	}

	LupineQuest *quest = get_quest(current_quest_id);
	if (!quest) {
		return;
	}

	TreeItem *root = objectives_tree->create_item();

	for (int i = 0; i < quest->objectives.size(); i++) {
		const LupineQuestObjective &objective = quest->objectives[i];
		TreeItem *item = objectives_tree->create_item(root);

		String display_text = objective.title.is_empty() ? objective.id : objective.title;
		if (objective.is_optional) {
			display_text += " (Optional)";
		}
		if (objective.is_completed) {
			display_text += " ✓";
		}

		item->set_text(0, display_text);
		item->set_metadata(0, objective.id);

		// Set icon based on objective type
		String icon_name;
		switch (objective.type) {
			case QUEST_OBJECTIVE_TALK:
				icon_name = "AudioStreamPlayer";
				break;
			case QUEST_OBJECTIVE_KILL:
				icon_name = "RigidBody2D";
				break;
			case QUEST_OBJECTIVE_COLLECT:
				icon_name = "PackedScene";
				break;
			case QUEST_OBJECTIVE_DELIVER:
				icon_name = "ArrowRight";
				break;
			case QUEST_OBJECTIVE_ESCORT:
				icon_name = "CharacterBody2D";
				break;
			case QUEST_OBJECTIVE_REACH_LOCATION:
				icon_name = "Position2D";
				break;
			case QUEST_OBJECTIVE_INTERACT:
				icon_name = "Control";
				break;
			default:
				icon_name = "Script";
				break;
		}

		Ref<Texture2D> icon = EditorNode::get_singleton()->get_gui_base()->get_theme_icon(icon_name, "EditorIcons");
		if (icon.is_valid()) {
			item->set_icon(0, icon);
		}
	}
}

void LupineQuestDesigner::_update_rewards_tree() {
	if (!rewards_tree) {
		return;
	}

	rewards_tree->clear();

	if (current_quest_id.is_empty()) {
		return;
	}

	LupineQuest *quest = get_quest(current_quest_id);
	if (!quest) {
		return;
	}

	TreeItem *root = rewards_tree->create_item();

	for (int i = 0; i < quest->rewards.size(); i++) {
		const LupineQuestReward &reward = quest->rewards[i];
		TreeItem *item = rewards_tree->create_item(root);

		String display_text = reward.type;
		if (reward.type == "item" && !reward.item_id.is_empty()) {
			display_text += ": " + reward.item_id;
		}
		if (reward.quantity > 1) {
			display_text += " x" + itos(reward.quantity);
		}

		item->set_text(0, display_text);
		item->set_metadata(0, i); // Store index as metadata

		// Set icon based on reward type
		String icon_name;
		if (reward.type == "item") {
			icon_name = "PackedScene";
		} else if (reward.type == "gold") {
			icon_name = "CurrencySign";
		} else if (reward.type == "experience") {
			icon_name = "TrendUp";
		} else {
			icon_name = "Script";
		}

		Ref<Texture2D> icon = EditorNode::get_singleton()->get_gui_base()->get_theme_icon(icon_name, "EditorIcons");
		if (icon.is_valid()) {
			item->set_icon(0, icon);
		}
	}
}

void LupineQuestDesigner::_on_quest_selected(int p_index) {
	if (p_index < 0 || p_index >= quest_list->get_item_count()) {
		current_quest_id = "";
		delete_quest_button->set_disabled(true);
		duplicate_quest_button->set_disabled(true);
		_update_quest_editor();
		return;
	}

	current_quest_id = quest_list->get_item_metadata(p_index);
	delete_quest_button->set_disabled(false);
	duplicate_quest_button->set_disabled(false);
	_update_quest_editor();
}

void LupineQuestDesigner::_on_new_quest_pressed() {
	LupineQuest *new_quest = create_new_quest();
	if (new_quest) {
		_populate_quest_list();

		// Select the new quest
		for (int i = 0; i < quest_list->get_item_count(); i++) {
			if (quest_list->get_item_metadata(i) == new_quest->id) {
				quest_list->select(i);
				_on_quest_selected(i);
				break;
			}
		}
	}
}

void LupineQuestDesigner::_on_delete_quest_pressed() {
	if (current_quest_id.is_empty()) {
		return;
	}

	// Show confirmation dialog
	ConfirmationDialog *confirm = memnew(ConfirmationDialog);
	confirm->set_text("Are you sure you want to delete this quest?");
	confirm->connect("confirmed", callable_mp(this, &LupineQuestDesigner::_perform_delete_quest));

	EditorNode::get_singleton()->get_gui_base()->add_child(confirm);
	confirm->popup_centered();
}

void LupineQuestDesigner::_perform_delete_quest() {
	if (!current_quest_id.is_empty()) {
		delete_quest(current_quest_id);
		current_quest_id = "";
		_populate_quest_list();
		_update_quest_editor();
	}
}

void LupineQuestDesigner::_on_duplicate_quest_pressed() {
	if (current_quest_id.is_empty()) {
		return;
	}

	LupineQuest *duplicated = duplicate_quest(current_quest_id);
	if (duplicated) {
		_populate_quest_list();

		// Select the duplicated quest
		for (int i = 0; i < quest_list->get_item_count(); i++) {
			if (quest_list->get_item_metadata(i) == duplicated->id) {
				quest_list->select(i);
				_on_quest_selected(i);
				break;
			}
		}
	}
}

void LupineQuestDesigner::_on_quest_property_changed() {
	if (current_quest_id.is_empty()) {
		return;
	}

	LupineQuest *quest = get_quest(current_quest_id);
	if (!quest) {
		return;
	}

	// Update quest properties from UI
	if (quest_title_edit) quest->title = quest_title_edit->get_text();
	if (quest_description_edit) quest->description = quest_description_edit->get_text();
	if (quest_category_edit) quest->category = quest_category_edit->get_text();
	if (quest_level_spin) quest->level_requirement = quest_level_spin->get_value();
	if (quest_status_option) quest->status = (LupineQuestStatus)quest_status_option->get_selected();

	// Update quest list display
	_populate_quest_list();

	// Auto-save
	save_quest_database();
}

// Missing method implementations
void LupineQuestDesigner::delete_quest(const String &p_quest_id) {
	quest_map.erase(p_quest_id);

	for (int i = 0; i < quest_database.size(); i++) {
		if (quest_database[i].id == p_quest_id) {
			quest_database.remove_at(i);
			break;
		}
	}
}





LupineQuest *LupineQuestDesigner::duplicate_quest(const String &p_quest_id) {
	LupineQuest *original = get_quest(p_quest_id);
	if (!original) {
		return nullptr;
	}

	LupineQuest duplicated = *original;
	duplicated.id = "quest_" + String::num_uint64(OS::get_singleton()->get_unix_time());
	duplicated.title = original->title + " (Copy)";

	quest_database.push_back(duplicated);
	quest_map[duplicated.id] = const_cast<LupineQuest*>(&quest_database[quest_database.size() - 1]);

	return const_cast<LupineQuest*>(&quest_database[quest_database.size() - 1]);
}

LupineQuest *LupineQuestDesigner::get_quest(const String &p_quest_id) {
	if (quest_map.has(p_quest_id)) {
		return quest_map[p_quest_id];
	}
	return nullptr;
}

void LupineQuestDesigner::save_quest_database() {
	// Placeholder implementation
	print_line("Saving quest database...");
}

void LupineQuestDesigner::load_quest_database() {
	// Placeholder implementation
	print_line("Loading quest database...");
}

// Missing method implementations
LupineQuest *LupineQuestDesigner::create_new_quest() {
	LupineQuest new_quest;
	new_quest.id = "quest_" + String::num_uint64(OS::get_singleton()->get_unix_time());
	new_quest.title = "New Quest";
	new_quest.description = "Enter quest description here...";
	new_quest.category = "Main";
	new_quest.level_requirement = 1;
	new_quest.status = QUEST_STATUS_INACTIVE;

	quest_database.push_back(new_quest);
	quest_map[new_quest.id] = const_cast<LupineQuest*>(&quest_database[quest_database.size() - 1]);

	return const_cast<LupineQuest*>(&quest_database[quest_database.size() - 1]);
}

void LupineQuestDesigner::_on_objective_selected() {
	// Handle objective selection
	print_line("Objective selected");
}

void LupineQuestDesigner::_on_add_objective_pressed() {
	// Add new objective to current quest
	print_line("Add objective pressed");
}

void LupineQuestDesigner::_on_remove_objective_pressed() {
	// Remove selected objective
	print_line("Remove objective pressed");
}

void LupineQuestDesigner::_on_objective_property_changed() {
	// Handle objective property changes
	print_line("Objective property changed");
}

void LupineQuestDesigner::_on_reward_selected() {
	// Handle reward selection
	print_line("Reward selected");
}

void LupineQuestDesigner::_on_add_reward_pressed() {
	// Add new reward to current quest
	print_line("Add reward pressed");
}

void LupineQuestDesigner::_on_remove_reward_pressed() {
	// Remove selected reward
	print_line("Remove reward pressed");
}

void LupineQuestDesigner::_on_reward_property_changed() {
	// Handle reward property changes
	print_line("Reward property changed");
}

void LupineQuestDesigner::_on_add_flow_node_pressed() {
	// Add new flow node to quest flow graph
	print_line("Add flow node pressed");
}
