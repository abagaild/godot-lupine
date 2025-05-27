#ifndef LUPINE_EDITOR_PLUGIN_H
#define LUPINE_EDITOR_PLUGIN_H

#include "editor/plugins/editor_plugin.h"
#include "scene/gui/control.h"
#include "scene/gui/container.h"
#include "scene/gui/button.h"
#include "scene/gui/label.h"
#include "scene/gui/tab_container.h"
#include "scene/gui/tree.h"
#include "scene/gui/item_list.h"
#include "scene/gui/line_edit.h"
#include "scene/gui/spin_box.h"
#include "scene/gui/option_button.h"
#include "scene/gui/check_box.h"
#include "scene/gui/text_edit.h"
#include "scene/gui/split_container.h"
#include "scene/gui/scroll_container.h"
#include "scene/gui/panel_container.h"
#include "scene/gui/margin_container.h"
#include "scene/gui/box_container.h"
#include "scene/resources/packed_scene.h"
#include "core/object/ref_counted.h"

class LupineToolManager;
class LupineWorldBuilder;
class LupineEntityEditor;
class LupineDialogueEditor;
class LupineAssetManager;
class LupineQuestDesigner;
class LupineCombatDesigner;

// Base class for all Lupine editor tools
class LupineEditorTool : public RefCounted {
	GDCLASS(LupineEditorTool, RefCounted);

protected:
	String tool_name;
	String tool_description;
	String tool_category;
	Control *tool_panel = nullptr;
	bool is_active = false;

	static void _bind_methods();

public:
	LupineEditorTool();
	virtual ~LupineEditorTool();

	// Tool interface
	virtual void initialize() {}
	virtual void cleanup() {}
	virtual Control *get_tool_panel() { return tool_panel; }
	virtual void activate() { is_active = true; }
	virtual void deactivate() { is_active = false; }
	virtual bool get_is_active() const { return is_active; }

	// Tool properties
	virtual String get_tool_name() const { return tool_name; }
	virtual String get_tool_description() const { return tool_description; }
	virtual String get_tool_category() const { return tool_category; }

	// Data management
	virtual void save_tool_data() {}
	virtual void load_tool_data() {}
	virtual void reset_tool_data() {}

	// Scene integration
	virtual void on_scene_changed(Node *p_scene) {}
	virtual void on_node_selected(Node *p_node) {}
	virtual void on_project_settings_changed() {}
};

// Main Lupine Editor Plugin
class LupineEditorPlugin : public EditorPlugin {
	GDCLASS(LupineEditorPlugin, EditorPlugin);

private:
	LupineToolManager *tool_manager = nullptr;
	Control *main_panel = nullptr;
	TabContainer *tool_tabs = nullptr;

	// Tool instances
	LupineWorldBuilder *world_builder = nullptr;
	LupineEntityEditor *entity_editor = nullptr;
	LupineDialogueEditor *dialogue_editor = nullptr;
	LupineAssetManager *asset_manager = nullptr;
	LupineQuestDesigner *quest_designer = nullptr;
	LupineCombatDesigner *combat_designer = nullptr;

	// UI components
	Button *toggle_button = nullptr;
	VBoxContainer *sidebar_container = nullptr;
	HSplitContainer *main_split = nullptr;

	bool is_lupine_mode_active = false;

	void _create_main_interface();
	void _create_tool_tabs();
	void _setup_tool_instances();
	void _toggle_lupine_mode();
	void _on_tool_tab_changed(int p_tab);
	void _update_tool_visibility();

protected:
	static void _bind_methods();

public:
	LupineEditorPlugin();
	~LupineEditorPlugin();

	// EditorPlugin interface
	void enable_plugin();
	void disable_plugin();
	virtual bool handles(Object *p_object) const override;
	virtual void edit(Object *p_object) override;
	virtual void make_visible(bool p_visible) override;
	virtual String get_plugin_name() const override;

	// Tool management
	void register_tool(LupineEditorTool *p_tool);
	void unregister_tool(LupineEditorTool *p_tool);
	LupineEditorTool *get_tool(const String &p_name);
	void activate_tool(const String &p_name);
	void deactivate_all_tools();

	// Getters
	LupineToolManager *get_tool_manager() const { return tool_manager; }
	bool is_active() const { return is_lupine_mode_active; }
};

// Tool Manager - handles registration and coordination of tools
class LupineToolManager : public RefCounted {
	GDCLASS(LupineToolManager, RefCounted);

private:
	HashMap<String, LupineEditorTool*> registered_tools;
	LupineEditorTool *active_tool = nullptr;
	LupineEditorPlugin *editor_plugin = nullptr;

protected:
	static void _bind_methods();

public:
	LupineToolManager(LupineEditorPlugin *p_plugin);
	~LupineToolManager();

	// Tool registration
	void register_tool(const String &p_name, LupineEditorTool *p_tool);
	void unregister_tool(const String &p_name);
	LupineEditorTool *get_tool(const String &p_name);
	Vector<String> get_tool_names() const;
	Vector<LupineEditorTool*> get_tools_by_category(const String &p_category) const;

	// Tool activation
	void activate_tool(const String &p_name);
	void deactivate_current_tool();
	LupineEditorTool *get_active_tool() const { return active_tool; }

	// Event handling
	void on_scene_changed(Node *p_scene);
	void on_node_selected(Node *p_node);
	void on_project_settings_changed();

	// Data management
	void save_all_tool_data();
	void load_all_tool_data();
	void reset_all_tool_data();
};

#endif // LUPINE_EDITOR_PLUGIN_H
