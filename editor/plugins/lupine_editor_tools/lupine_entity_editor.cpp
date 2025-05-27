#include "lupine_entity_editor.h"
#include "scene/gui/label.h"
#include "scene/gui/separator.h"
#include "scene/gui/box_container.h"

void LupineEntityEditor::_bind_methods() {
	// Bind methods for signals and callbacks
}

LupineEntityEditor::LupineEntityEditor() {
	tool_name = "Entity Editor";
	tool_description = "Configure characters, NPCs, enemies, and items";
	tool_category = "Entity Design";

	main_tabs = nullptr;
	character_panel = nullptr;
	character_stats_tree = nullptr;
	enemy_panel = nullptr;
	enemy_stats_tree = nullptr;
	item_panel = nullptr;
	item_properties_tree = nullptr;
	current_target = nullptr;
}

LupineEntityEditor::~LupineEntityEditor() {
	cleanup();
}

void LupineEntityEditor::initialize() {
	if (tool_panel) {
		return; // Already initialized
	}

	_create_ui();
}

void LupineEntityEditor::cleanup() {
	if (tool_panel && tool_panel->get_parent()) {
		tool_panel->get_parent()->remove_child(tool_panel);
	}

	if (tool_panel) {
		memdelete(tool_panel);
		tool_panel = nullptr;
	}

	main_tabs = nullptr;
	character_panel = nullptr;
	character_stats_tree = nullptr;
	enemy_panel = nullptr;
	enemy_stats_tree = nullptr;
	item_panel = nullptr;
	item_properties_tree = nullptr;
}

void LupineEntityEditor::_create_ui() {
	// Create main panel
	tool_panel = memnew(Control);
	tool_panel->set_name("EntityEditor");
	tool_panel->set_anchors_and_offsets_preset(Control::PRESET_FULL_RECT);

	// Create main tabs
	main_tabs = memnew(TabContainer);
	main_tabs->set_anchors_and_offsets_preset(Control::PRESET_FULL_RECT);
	tool_panel->add_child(main_tabs);

	_create_character_tab();
	_create_enemy_tab();
	_create_item_tab();
}

void LupineEntityEditor::_create_character_tab() {
	character_panel = memnew(VBoxContainer);
	character_panel->set_name("Characters");
	main_tabs->add_child(character_panel);

	Label *title = memnew(Label);
	title->set_text("Character & NPC Configuration");
	character_panel->add_child(title);

	character_stats_tree = memnew(Tree);
	character_stats_tree->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	character_stats_tree->set_v_size_flags(Control::SIZE_EXPAND_FILL);
	character_panel->add_child(character_stats_tree);
}

void LupineEntityEditor::_create_enemy_tab() {
	enemy_panel = memnew(VBoxContainer);
	enemy_panel->set_name("Enemies");
	main_tabs->add_child(enemy_panel);

	Label *title = memnew(Label);
	title->set_text("Enemy Configuration");
	enemy_panel->add_child(title);

	enemy_stats_tree = memnew(Tree);
	enemy_stats_tree->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	enemy_stats_tree->set_v_size_flags(Control::SIZE_EXPAND_FILL);
	enemy_panel->add_child(enemy_stats_tree);
}

void LupineEntityEditor::_create_item_tab() {
	item_panel = memnew(VBoxContainer);
	item_panel->set_name("Items");
	main_tabs->add_child(item_panel);

	Label *title = memnew(Label);
	title->set_text("Item & Equipment Configuration");
	item_panel->add_child(title);

	item_properties_tree = memnew(Tree);
	item_properties_tree->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	item_properties_tree->set_v_size_flags(Control::SIZE_EXPAND_FILL);
	item_panel->add_child(item_properties_tree);
}

void LupineEntityEditor::on_node_selected(Node *p_node) {
	current_target = p_node;
	_update_properties_for_node(p_node);
}

void LupineEntityEditor::_update_properties_for_node(Node *p_node) {
	// Clear all trees
	if (character_stats_tree) {
		character_stats_tree->clear();
	}
	if (enemy_stats_tree) {
		enemy_stats_tree->clear();
	}
	if (item_properties_tree) {
		item_properties_tree->clear();
	}

	if (!p_node) {
		return;
	}

	// Determine node type and populate appropriate tree
	String node_name = String(p_node->get_name()).to_lower();
	String node_class = p_node->get_class();

	if (node_name.contains("npc") || node_name.contains("character")) {
		// Populate character tree
		TreeItem *root = character_stats_tree->create_item();
		root->set_text(0, "Character Properties");

		TreeItem *name_item = character_stats_tree->create_item(root);
		name_item->set_text(0, "Name");
		name_item->set_text(1, p_node->get_name());
		name_item->set_editable(1, true);

		// Add more character-specific properties as needed
	} else if (node_name.contains("enemy") || node_name.contains("monster")) {
		// Populate enemy tree
		TreeItem *root = enemy_stats_tree->create_item();
		root->set_text(0, "Enemy Properties");

		TreeItem *name_item = enemy_stats_tree->create_item(root);
		name_item->set_text(0, "Name");
		name_item->set_text(1, p_node->get_name());
		name_item->set_editable(1, true);

		// Add more enemy-specific properties as needed
	} else if (node_name.contains("item") || node_name.contains("chest")) {
		// Populate item tree
		TreeItem *root = item_properties_tree->create_item();
		root->set_text(0, "Item Properties");

		TreeItem *name_item = item_properties_tree->create_item(root);
		name_item->set_text(0, "Name");
		name_item->set_text(1, p_node->get_name());
		name_item->set_editable(1, true);

		// Add more item-specific properties as needed
	}
}
