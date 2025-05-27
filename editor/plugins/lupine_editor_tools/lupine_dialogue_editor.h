#ifndef LUPINE_DIALOGUE_EDITOR_H
#define LUPINE_DIALOGUE_EDITOR_H

#include "lupine_editor_plugin.h"
#include "scene/gui/graph_edit.h"
#include "scene/gui/graph_node.h"
#include "scene/gui/text_edit.h"
#include "scene/gui/option_button.h"
#include "scene/gui/split_container.h"
#include "scene/gui/box_container.h"

// Node-based dialogue editor for visual conversation design
class LupineDialogueEditor : public LupineEditorTool {
	GDCLASS(LupineDialogueEditor, LupineEditorTool);

private:
	HSplitContainer *main_split = nullptr;
	VBoxContainer *left_panel = nullptr;
	GraphEdit *dialogue_graph = nullptr;

	// Dialogue node management
	int next_node_id = 1;
	HashMap<int, GraphNode*> dialogue_nodes;

	void _create_ui();
	void _create_dialogue_node(Vector2 p_position);
	void _on_add_node_pressed();
	void _on_connection_request(const String &p_from, int p_from_port, const String &p_to, int p_to_port);
	void _on_disconnection_request(const String &p_from, int p_from_port, const String &p_to, int p_to_port);

protected:
	static void _bind_methods();

public:
	LupineDialogueEditor();
	virtual ~LupineDialogueEditor();

	// LupineEditorTool interface
	virtual void initialize() override;
	virtual void cleanup() override;
	virtual Control *get_tool_panel() override { return tool_panel; }
	virtual String get_tool_name() const override { return "Dialogue Editor"; }
	virtual String get_tool_description() const override { return "Node-based visual dialogue and conversation design"; }
	virtual String get_tool_category() const override { return "Narrative Design"; }
};

#endif // LUPINE_DIALOGUE_EDITOR_H
