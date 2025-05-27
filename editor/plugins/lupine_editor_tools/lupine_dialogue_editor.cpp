#include "lupine_dialogue_editor.h"
#include "scene/gui/label.h"
#include "scene/gui/button.h"
#include "scene/gui/line_edit.h"
#include "scene/gui/separator.h"
#include "scene/gui/box_container.h"

void LupineDialogueEditor::_bind_methods() {
	ClassDB::bind_method(D_METHOD("_on_add_node_pressed"), &LupineDialogueEditor::_on_add_node_pressed);
	ClassDB::bind_method(D_METHOD("_on_connection_request"), &LupineDialogueEditor::_on_connection_request);
	ClassDB::bind_method(D_METHOD("_on_disconnection_request"), &LupineDialogueEditor::_on_disconnection_request);
}

LupineDialogueEditor::LupineDialogueEditor() {
	tool_name = "Dialogue Editor";
	tool_description = "Node-based visual dialogue and conversation design";
	tool_category = "Narrative Design";

	main_split = nullptr;
	left_panel = nullptr;
	dialogue_graph = nullptr;
	next_node_id = 1;
}

LupineDialogueEditor::~LupineDialogueEditor() {
	cleanup();
}

void LupineDialogueEditor::initialize() {
	if (tool_panel) {
		return; // Already initialized
	}

	_create_ui();
}

void LupineDialogueEditor::cleanup() {
	if (tool_panel && tool_panel->get_parent()) {
		tool_panel->get_parent()->remove_child(tool_panel);
	}

	if (tool_panel) {
		memdelete(tool_panel);
		tool_panel = nullptr;
	}

	main_split = nullptr;
	left_panel = nullptr;
	dialogue_graph = nullptr;
	dialogue_nodes.clear();
}

void LupineDialogueEditor::_create_ui() {
	// Create main panel
	tool_panel = memnew(Control);
	tool_panel->set_name("DialogueEditor");
	tool_panel->set_anchors_and_offsets_preset(Control::PRESET_FULL_RECT);

	// Create main split
	main_split = memnew(HSplitContainer);
	main_split->set_anchors_and_offsets_preset(Control::PRESET_FULL_RECT);
	tool_panel->add_child(main_split);

	// Create left panel for tools
	left_panel = memnew(VBoxContainer);
	left_panel->set_custom_minimum_size(Size2(200, 0));
	main_split->add_child(left_panel);

	// Add title
	Label *title = memnew(Label);
	title->set_text("Dialogue Tools");
	left_panel->add_child(title);

	HSeparator *separator = memnew(HSeparator);
	left_panel->add_child(separator);

	// Add node creation button
	Button *add_node_btn = memnew(Button);
	add_node_btn->set_text("Add Dialogue Node");
	add_node_btn->connect("pressed", callable_mp(this, &LupineDialogueEditor::_on_add_node_pressed));
	left_panel->add_child(add_node_btn);

	// Create graph edit
	dialogue_graph = memnew(GraphEdit);
	dialogue_graph->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	dialogue_graph->set_v_size_flags(Control::SIZE_EXPAND_FILL);
	dialogue_graph->set_right_disconnects(true);
	dialogue_graph->connect("connection_request", callable_mp(this, &LupineDialogueEditor::_on_connection_request));
	dialogue_graph->connect("disconnection_request", callable_mp(this, &LupineDialogueEditor::_on_disconnection_request));
	main_split->add_child(dialogue_graph);
}

void LupineDialogueEditor::_create_dialogue_node(Vector2 p_position) {
	GraphNode *node = memnew(GraphNode);
	node->set_name("DialogueNode_" + itos(next_node_id));
	node->set_title("Dialogue " + itos(next_node_id));
	node->set_position_offset(p_position);
	node->set_resizable(true);
	node->set_custom_minimum_size(Size2(200, 150));

	// Create content container
	VBoxContainer *content = memnew(VBoxContainer);
	node->add_child(content);

	// Add character name field
	Label *char_label = memnew(Label);
	char_label->set_text("Character:");
	content->add_child(char_label);

	LineEdit *char_name = memnew(LineEdit);
	char_name->set_placeholder("Character Name");
	content->add_child(char_name);

	// Add dialogue text field
	Label *text_label = memnew(Label);
	text_label->set_text("Dialogue:");
	content->add_child(text_label);

	TextEdit *dialogue_text = memnew(TextEdit);
	dialogue_text->set_custom_minimum_size(Size2(180, 80));
	dialogue_text->set_placeholder("Enter dialogue text...");
	content->add_child(dialogue_text);

	// Set up ports
	node->set_slot(0, false, 0, Color(1, 1, 1), true, 0, Color(1, 1, 1));

	dialogue_graph->add_child(node);
	dialogue_nodes[next_node_id] = node;
	next_node_id++;
}

void LupineDialogueEditor::_on_add_node_pressed() {
	Vector2 center = dialogue_graph->get_size() / 2;
	Vector2 scroll_offset = dialogue_graph->get_scroll_offset();
	Vector2 position = center + scroll_offset;

	_create_dialogue_node(position);
}

void LupineDialogueEditor::_on_connection_request(const String &p_from, int p_from_port, const String &p_to, int p_to_port) {
	dialogue_graph->connect_node(p_from, p_from_port, p_to, p_to_port);
}

void LupineDialogueEditor::_on_disconnection_request(const String &p_from, int p_from_port, const String &p_to, int p_to_port) {
	dialogue_graph->disconnect_node(p_from, p_from_port, p_to, p_to_port);
}
