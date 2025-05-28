#include "lupine_editor_plugin.h"
#include "lupine_world_builder.h"
#include "lupine_entity_editor.h"
#include "lupine_dialogue_editor.h"
#include "lupine_asset_manager.h"
#include "lupine_quest_designer.h"
#include "lupine_combat_designer.h"
#include "lupine_topdown_rpg_editor.h"
#include "editor/editor_node.h"
#include "editor/editor_interface.h"
#include "scene/gui/separator.h"
#include "scene/gui/button.h"
#include "scene/gui/label.h"
#include "scene/gui/tab_container.h"

// LupineEditorTool Implementation

void LupineEditorTool::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_tool_name"), &LupineEditorTool::get_tool_name);
	ClassDB::bind_method(D_METHOD("get_tool_description"), &LupineEditorTool::get_tool_description);
	ClassDB::bind_method(D_METHOD("get_tool_category"), &LupineEditorTool::get_tool_category);
	ClassDB::bind_method(D_METHOD("get_is_active"), &LupineEditorTool::get_is_active);
	ClassDB::bind_method(D_METHOD("activate"), &LupineEditorTool::activate);
	ClassDB::bind_method(D_METHOD("deactivate"), &LupineEditorTool::deactivate);
}

LupineEditorTool::LupineEditorTool() {
	tool_name = "Base Tool";
	tool_description = "Base Lupine Editor Tool";
	tool_category = "General";
	is_active = false;
}

LupineEditorTool::~LupineEditorTool() {
	if (tool_panel && tool_panel->get_parent()) {
		tool_panel->get_parent()->remove_child(tool_panel);
	}
	if (tool_panel) {
		memdelete(tool_panel);
		tool_panel = nullptr;
	}
}

// LupineEditorPlugin Implementation

void LupineEditorPlugin::_bind_methods() {
	// No methods need binding for the simplified tab-based approach
}

LupineEditorPlugin::LupineEditorPlugin() {
	print_line("LupineEditorPlugin: Constructor called!");

	// Initialize member variables
	tool_manager = nullptr;
	world_builder = nullptr;
	entity_editor = nullptr;
	dialogue_editor = nullptr;
	asset_manager = nullptr;
	quest_designer = nullptr;
	combat_designer = nullptr;

	// Initialize the plugin immediately in constructor (like other built-in plugins)
	print_line("LupineEditorPlugin: Initializing plugin...");

	// Create tool manager
	tool_manager = memnew(LupineToolManager(this));
	print_line("LupineEditorPlugin: Tool manager created");

	// Setup tool instances
	_setup_tool_instances();

	// Add tool tabs to the editor
	_add_tool_tabs();

	print_line("LupineEditorPlugin: Plugin initialized successfully");
}

LupineEditorPlugin::~LupineEditorPlugin() {
	if (world_builder) {
		memdelete(world_builder);
	}
	if (entity_editor) {
		memdelete(entity_editor);
	}
	if (dialogue_editor) {
		memdelete(dialogue_editor);
	}
	if (asset_manager) {
		memdelete(asset_manager);
	}
	if (quest_designer) {
		memdelete(quest_designer);
	}
	if (combat_designer) {
		memdelete(combat_designer);
	}
	if (tool_manager) {
		memdelete(tool_manager);
	}
}

void LupineEditorPlugin::enable_plugin() {
	// For built-in plugins, initialization is done in constructor
	// This method is kept for compatibility but does nothing
	print_line("LupineEditorPlugin: enable_plugin() called (no-op for built-in plugins)");
}

void LupineEditorPlugin::disable_plugin() {
	// Remove all tool tabs from the bottom panel
	if (world_builder) {
		Control *panel = world_builder->get_tool_panel();
		if (panel) {
			remove_control_from_bottom_panel(panel);
		}
	}
	if (entity_editor) {
		Control *panel = entity_editor->get_tool_panel();
		if (panel) {
			remove_control_from_bottom_panel(panel);
		}
	}
	if (dialogue_editor) {
		Control *panel = dialogue_editor->get_tool_panel();
		if (panel) {
			remove_control_from_bottom_panel(panel);
		}
	}
	if (asset_manager) {
		Control *panel = asset_manager->get_tool_panel();
		if (panel) {
			remove_control_from_bottom_panel(panel);
		}
	}
	if (quest_designer) {
		Control *panel = quest_designer->get_tool_panel();
		if (panel) {
			remove_control_from_bottom_panel(panel);
		}
	}
	if (combat_designer) {
		Control *panel = combat_designer->get_tool_panel();
		if (panel) {
			remove_control_from_bottom_panel(panel);
		}
	}
}

bool LupineEditorPlugin::handles(Object *p_object) const {
	// Handle scene files and nodes that can be edited with Lupine tools
	return Object::cast_to<Node>(p_object) != nullptr;
}

void LupineEditorPlugin::edit(Object *p_object) {
	Node *node = Object::cast_to<Node>(p_object);
	if (node && tool_manager) {
		tool_manager->on_node_selected(node);
	}
}

void LupineEditorPlugin::make_visible(bool p_visible) {
	// Tab visibility is now managed by the editor itself
	// No action needed here
}

String LupineEditorPlugin::get_plugin_name() const {
	return "Lupine Editor Tools";
}

void LupineEditorPlugin::_add_tool_tabs() {
	print_line("LupineEditorPlugin: Adding tool tabs to editor...");

	// Add each tool as a separate tab in the bottom panel
	if (world_builder) {
		Control *panel = world_builder->get_tool_panel();
		if (panel) {
			add_control_to_bottom_panel(panel, "World Builder");
			print_line("LupineEditorPlugin: World Builder tab added");
		}
	}

	if (entity_editor) {
		Control *panel = entity_editor->get_tool_panel();
		if (panel) {
			add_control_to_bottom_panel(panel, "Entity Editor");
			print_line("LupineEditorPlugin: Entity Editor tab added");
		}
	}

	if (dialogue_editor) {
		Control *panel = dialogue_editor->get_tool_panel();
		if (panel) {
			add_control_to_bottom_panel(panel, "Dialogue Editor");
			print_line("LupineEditorPlugin: Dialogue Editor tab added");
		}
	}

	if (asset_manager) {
		Control *panel = asset_manager->get_tool_panel();
		if (panel) {
			add_control_to_bottom_panel(panel, "Asset Manager");
			print_line("LupineEditorPlugin: Asset Manager tab added");
		}
	}

	if (quest_designer) {
		Control *panel = quest_designer->get_tool_panel();
		if (panel) {
			add_control_to_bottom_panel(panel, "Quest Designer");
			print_line("LupineEditorPlugin: Quest Designer tab added");
		}
	}

	if (combat_designer) {
		Control *panel = combat_designer->get_tool_panel();
		if (panel) {
			add_control_to_bottom_panel(panel, "Combat Designer");
			print_line("LupineEditorPlugin: Combat Designer tab added");
		}
	}

	print_line("LupineEditorPlugin: All tool tabs added successfully");
}

void LupineEditorPlugin::_setup_tool_instances() {
	print_line("LupineEditorPlugin: Setting up tool instances...");

	// Create and register World Builder
	print_line("LupineEditorPlugin: Creating World Builder...");
	world_builder = memnew(LupineWorldBuilder);
	world_builder->initialize();
	register_tool(world_builder);
	print_line("LupineEditorPlugin: World Builder registered");

	// Create and register Entity Editor
	print_line("LupineEditorPlugin: Creating Entity Editor...");
	entity_editor = memnew(LupineEntityEditor);
	entity_editor->initialize();
	register_tool(entity_editor);
	print_line("LupineEditorPlugin: Entity Editor registered");

	// Create and register Dialogue Editor
	print_line("LupineEditorPlugin: Creating Dialogue Editor...");
	dialogue_editor = memnew(LupineDialogueEditor);
	dialogue_editor->initialize();
	register_tool(dialogue_editor);
	print_line("LupineEditorPlugin: Dialogue Editor registered");

	// Create and register Asset Manager
	print_line("LupineEditorPlugin: Creating Asset Manager...");
	asset_manager = memnew(LupineAssetManager);
	asset_manager->initialize();
	register_tool(asset_manager);
	print_line("LupineEditorPlugin: Asset Manager registered");

	// Create and register Quest Designer
	print_line("LupineEditorPlugin: Creating Quest Designer...");
	quest_designer = memnew(LupineQuestDesigner);
	quest_designer->initialize();
	register_tool(quest_designer);
	print_line("LupineEditorPlugin: Quest Designer registered");

	// Create and register Combat Designer
	print_line("LupineEditorPlugin: Creating Combat Designer...");
	combat_designer = memnew(LupineCombatDesigner);
	combat_designer->initialize();
	register_tool(combat_designer);
	print_line("LupineEditorPlugin: Combat Designer registered");

	// Create and register Topdown RPG Editor
	print_line("LupineEditorPlugin: Creating Topdown RPG Editor...");
	topdown_rpg_editor = memnew(LupineTopdownRPGEditor);
	topdown_rpg_editor->initialize();
	register_tool(topdown_rpg_editor);
	print_line("LupineEditorPlugin: Topdown RPG Editor registered");
}



void LupineEditorPlugin::register_tool(LupineEditorTool *p_tool) {
	if (!p_tool || !tool_manager) {
		return;
	}

	tool_manager->register_tool(p_tool->get_tool_name(), p_tool);
}

void LupineEditorPlugin::unregister_tool(LupineEditorTool *p_tool) {
	if (!p_tool || !tool_manager) {
		return;
	}

	tool_manager->unregister_tool(p_tool->get_tool_name());
}

LupineEditorTool *LupineEditorPlugin::get_tool(const String &p_name) {
	if (!tool_manager) {
		return nullptr;
	}
	return tool_manager->get_tool(p_name);
}

void LupineEditorPlugin::activate_tool(const String &p_name) {
	if (!tool_manager) {
		return;
	}
	tool_manager->activate_tool(p_name);
}

void LupineEditorPlugin::deactivate_all_tools() {
	if (!tool_manager) {
		return;
	}
	tool_manager->deactivate_current_tool();
}



// LupineToolManager Implementation

void LupineToolManager::_bind_methods() {
	ClassDB::bind_method(D_METHOD("register_tool", "name", "tool"), &LupineToolManager::register_tool);
	ClassDB::bind_method(D_METHOD("unregister_tool", "name"), &LupineToolManager::unregister_tool);
	ClassDB::bind_method(D_METHOD("get_tool", "name"), &LupineToolManager::get_tool);
	ClassDB::bind_method(D_METHOD("activate_tool", "name"), &LupineToolManager::activate_tool);
	ClassDB::bind_method(D_METHOD("deactivate_current_tool"), &LupineToolManager::deactivate_current_tool);
	ClassDB::bind_method(D_METHOD("get_active_tool"), &LupineToolManager::get_active_tool);
}

LupineToolManager::LupineToolManager(LupineEditorPlugin *p_plugin) {
	editor_plugin = p_plugin;
	active_tool = nullptr;
}

LupineToolManager::~LupineToolManager() {
	// Clean up registered tools
	for (HashMap<String, LupineEditorTool*>::Iterator it = registered_tools.begin(); it != registered_tools.end(); ++it) {
		if (it->value) {
			it->value->cleanup();
		}
	}
	registered_tools.clear();
	active_tool = nullptr;
}

void LupineToolManager::register_tool(const String &p_name, LupineEditorTool *p_tool) {
	if (!p_tool) {
		return;
	}

	if (registered_tools.has(p_name)) {
		ERR_PRINT("Tool with name '" + p_name + "' is already registered.");
		return;
	}

	registered_tools[p_name] = p_tool;
	p_tool->initialize();
}

void LupineToolManager::unregister_tool(const String &p_name) {
	if (!registered_tools.has(p_name)) {
		return;
	}

	LupineEditorTool *tool = registered_tools[p_name];
	if (tool == active_tool) {
		deactivate_current_tool();
	}

	if (tool) {
		tool->cleanup();
	}

	registered_tools.erase(p_name);
}

LupineEditorTool *LupineToolManager::get_tool(const String &p_name) {
	if (!registered_tools.has(p_name)) {
		return nullptr;
	}
	return registered_tools[p_name];
}

Vector<String> LupineToolManager::get_tool_names() const {
	Vector<String> names;
	for (HashMap<String, LupineEditorTool*>::ConstIterator it = registered_tools.begin(); it != registered_tools.end(); ++it) {
		names.push_back(it->key);
	}
	return names;
}

Vector<LupineEditorTool*> LupineToolManager::get_tools_by_category(const String &p_category) const {
	Vector<LupineEditorTool*> tools;
	for (HashMap<String, LupineEditorTool*>::ConstIterator it = registered_tools.begin(); it != registered_tools.end(); ++it) {
		if (it->value && it->value->get_tool_category() == p_category) {
			tools.push_back(it->value);
		}
	}
	return tools;
}

void LupineToolManager::activate_tool(const String &p_name) {
	LupineEditorTool *tool = get_tool(p_name);
	if (!tool) {
		ERR_PRINT("Tool '" + p_name + "' not found.");
		return;
	}

	// Deactivate current tool
	if (active_tool && active_tool != tool) {
		active_tool->deactivate();
	}

	// Activate new tool
	active_tool = tool;
	active_tool->activate();
}

void LupineToolManager::deactivate_current_tool() {
	if (active_tool) {
		active_tool->deactivate();
		active_tool = nullptr;
	}
}

void LupineToolManager::on_scene_changed(Node *p_scene) {
	for (HashMap<String, LupineEditorTool*>::Iterator it = registered_tools.begin(); it != registered_tools.end(); ++it) {
		if (it->value) {
			it->value->on_scene_changed(p_scene);
		}
	}
}

void LupineToolManager::on_node_selected(Node *p_node) {
	for (HashMap<String, LupineEditorTool*>::Iterator it = registered_tools.begin(); it != registered_tools.end(); ++it) {
		if (it->value) {
			it->value->on_node_selected(p_node);
		}
	}
}

void LupineToolManager::on_project_settings_changed() {
	for (HashMap<String, LupineEditorTool*>::Iterator it = registered_tools.begin(); it != registered_tools.end(); ++it) {
		if (it->value) {
			it->value->on_project_settings_changed();
		}
	}
}

void LupineToolManager::save_all_tool_data() {
	for (HashMap<String, LupineEditorTool*>::Iterator it = registered_tools.begin(); it != registered_tools.end(); ++it) {
		if (it->value) {
			it->value->save_tool_data();
		}
	}
}

void LupineToolManager::load_all_tool_data() {
	for (HashMap<String, LupineEditorTool*>::Iterator it = registered_tools.begin(); it != registered_tools.end(); ++it) {
		if (it->value) {
			it->value->load_tool_data();
		}
	}
}

void LupineToolManager::reset_all_tool_data() {
	for (HashMap<String, LupineEditorTool*>::Iterator it = registered_tools.begin(); it != registered_tools.end(); ++it) {
		if (it->value) {
			it->value->reset_tool_data();
		}
	}
}
