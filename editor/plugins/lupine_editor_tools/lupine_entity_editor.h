#ifndef LUPINE_ENTITY_EDITOR_H
#define LUPINE_ENTITY_EDITOR_H

#include "lupine_editor_plugin.h"
#include "scene/gui/tab_container.h"
#include "scene/gui/tree.h"
#include "scene/gui/item_list.h"
#include "scene/gui/grid_container.h"
#include "scene/gui/box_container.h"

// Entity Editor tool for configuring characters, NPCs, enemies, and items
class LupineEntityEditor : public LupineEditorTool {
	GDCLASS(LupineEntityEditor, LupineEditorTool);

private:
	TabContainer *main_tabs = nullptr;

	// Character/NPC tab
	VBoxContainer *character_panel = nullptr;
	Tree *character_stats_tree = nullptr;

	// Enemy tab
	VBoxContainer *enemy_panel = nullptr;
	Tree *enemy_stats_tree = nullptr;

	// Item tab
	VBoxContainer *item_panel = nullptr;
	Tree *item_properties_tree = nullptr;

	Node *current_target = nullptr;

	void _create_ui();
	void _create_character_tab();
	void _create_enemy_tab();
	void _create_item_tab();
	void _update_properties_for_node(Node *p_node);

protected:
	static void _bind_methods();

public:
	LupineEntityEditor();
	virtual ~LupineEntityEditor();

	// LupineEditorTool interface
	virtual void initialize() override;
	virtual void cleanup() override;
	virtual Control *get_tool_panel() override { return tool_panel; }
	virtual String get_tool_name() const override { return "Entity Editor"; }
	virtual String get_tool_description() const override { return "Configure characters, NPCs, enemies, and items"; }
	virtual String get_tool_category() const override { return "Entity Design"; }

	// Scene integration
	virtual void on_node_selected(Node *p_node) override;
};

#endif // LUPINE_ENTITY_EDITOR_H
