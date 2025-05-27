#include "lupine_editor_plugin.h"
#include "lupine_world_builder.h"
#include "lupine_entity_editor.h"
#include "lupine_dialogue_editor.h"
#include "lupine_asset_manager.h"
#include "lupine_quest_designer.h"
#include "lupine_combat_designer.h"
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
	ClassDB::bind_method(D_METHOD("_toggle_lupine_mode"), &LupineEditorPlugin::_toggle_lupine_mode);
	ClassDB::bind_method(D_METHOD("_on_tool_tab_changed", "tab"), &LupineEditorPlugin::_on_tool_tab_changed);
}

LupineEditorPlugin::LupineEditorPlugin() {
	print_line("LupineEditorPlugin: Constructor called!");

	// Initialize member variables
	tool_manager = nullptr;
	main_panel = nullptr;
	tool_tabs = nullptr;
	world_builder = nullptr;
	entity_editor = nullptr;
	dialogue_editor = nullptr;
	asset_manager = nullptr;
	quest_designer = nullptr;
	combat_designer = nullptr;
	toggle_button = nullptr;
	sidebar_container = nullptr;
	main_split = nullptr;
	is_lupine_mode_active = false;

	// Initialize the plugin immediately in constructor (like other built-in plugins)
	print_line("LupineEditorPlugin: Initializing plugin...");

	// Create the main Lupine Tools panel
	main_panel = memnew(VBoxContainer);
	main_panel->set_name("LupineEditorTools");
	main_panel->set_custom_minimum_size(Size2(250, 400));

	// Create header
	Label *header_label = memnew(Label);
	header_label->set_text("Lupine Editor Tools");
	header_label->set_horizontal_alignment(HORIZONTAL_ALIGNMENT_CENTER);
	header_label->add_theme_font_size_override("font_size", 14);
	main_panel->add_child(header_label);

	// Add separator
	HSeparator *separator1 = memnew(HSeparator);
	main_panel->add_child(separator1);

	// Core Tools Section
	Label *core_label = memnew(Label);
	core_label->set_text("Core Tools");
	core_label->add_theme_font_size_override("font_size", 12);
	main_panel->add_child(core_label);

	// Entity Editor - Main implementation
	Button *entity_editor_btn = memnew(Button);
	entity_editor_btn->set_text("Entity Editor");
	entity_editor_btn->set_tooltip_text("Manage player, enemy, and NPC entities with visual and logic controls");
	entity_editor_btn->connect("pressed", callable_mp(this, &LupineEditorPlugin::_open_entity_editor));
	main_panel->add_child(entity_editor_btn);

	// World Builder
	Button *world_builder_btn = memnew(Button);
	world_builder_btn->set_text("World Builder");
	world_builder_btn->set_tooltip_text("Visual world building and asset placement tool");
	world_builder_btn->connect("pressed", callable_mp(this, &LupineEditorPlugin::_open_world_builder));
	main_panel->add_child(world_builder_btn);

	// Dialogue Editor
	Button *dialogue_editor_btn = memnew(Button);
	dialogue_editor_btn->set_text("Dialogue Editor");
	dialogue_editor_btn->set_tooltip_text("Create and manage dialogue trees and conversations");
	dialogue_editor_btn->connect("pressed", callable_mp(this, &LupineEditorPlugin::_open_dialogue_editor));
	main_panel->add_child(dialogue_editor_btn);

	// Add separator
	HSeparator *separator2 = memnew(HSeparator);
	main_panel->add_child(separator2);

	// Scene Management Section
	Label *scene_label = memnew(Label);
	scene_label->set_text("Scene Management");
	scene_label->add_theme_font_size_override("font_size", 12);
	main_panel->add_child(scene_label);

	// Scene Viewer
	Button *scene_viewer_btn = memnew(Button);
	scene_viewer_btn->set_text("Scene Viewer");
	scene_viewer_btn->set_tooltip_text("Browse and manage all scenes in your project");
	scene_viewer_btn->connect("pressed", callable_mp(this, &LupineEditorPlugin::_open_scene_viewer));
	main_panel->add_child(scene_viewer_btn);

	// Overworld Editor
	Button *overworld_editor_btn = memnew(Button);
	overworld_editor_btn->set_text("Overworld Editor");
	overworld_editor_btn->set_tooltip_text("Manage map connections and world transitions");
	overworld_editor_btn->connect("pressed", callable_mp(this, &LupineEditorPlugin::_open_overworld_editor));
	main_panel->add_child(overworld_editor_btn);

	// Add to dock
	add_control_to_dock(DOCK_SLOT_LEFT_UL, main_panel);
	print_line("LupineEditorPlugin: Test panel with buttons added to dock");

	// Add toggle button to toolbar
	toggle_button = memnew(Button);
	toggle_button->set_text("Lupine Tools");
	toggle_button->set_tooltip_text("Toggle Lupine Engine visual editing tools");
	// Skip connecting the signal for now to avoid potential issues
	// toggle_button->connect("pressed", callable_mp(this, &LupineEditorPlugin::_toggle_lupine_mode));

	add_control_to_container(CONTAINER_TOOLBAR, toggle_button);
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
	if (toggle_button) {
		remove_control_from_container(CONTAINER_TOOLBAR, toggle_button);
		toggle_button = nullptr;
	}

	if (main_panel) {
		remove_control_from_docks(main_panel);
		memdelete(main_panel);
		main_panel = nullptr;
	}
}

bool LupineEditorPlugin::handles(Object *p_object) const {
	// Handle scene files and nodes that can be edited with Lupine tools
	return is_lupine_mode_active && (Object::cast_to<Node>(p_object) != nullptr);
}

void LupineEditorPlugin::edit(Object *p_object) {
	if (!is_lupine_mode_active) {
		return;
	}

	Node *node = Object::cast_to<Node>(p_object);
	if (node && tool_manager) {
		tool_manager->on_node_selected(node);
	}
}

void LupineEditorPlugin::make_visible(bool p_visible) {
	if (main_panel) {
		main_panel->set_visible(p_visible && is_lupine_mode_active);
	}
}

String LupineEditorPlugin::get_plugin_name() const {
	return "Lupine Editor Tools";
}

void LupineEditorPlugin::_create_main_interface() {
	print_line("LupineEditorPlugin: Creating main interface...");

	// Create main panel
	main_panel = memnew(Control);
	main_panel->set_name("LupineEditorTools");
	main_panel->set_anchors_and_offsets_preset(Control::PRESET_FULL_RECT);
	print_line("LupineEditorPlugin: Main panel created");

	// Create main split container
	main_split = memnew(HSplitContainer);
	main_split->set_anchors_and_offsets_preset(Control::PRESET_FULL_RECT);
	main_panel->add_child(main_split);
	print_line("LupineEditorPlugin: Split container created");

	// Create sidebar for tool selection
	sidebar_container = memnew(VBoxContainer);
	sidebar_container->set_custom_minimum_size(Size2(250, 0));
	main_split->add_child(sidebar_container);
	print_line("LupineEditorPlugin: Sidebar container created");

	// Add title label
	Label *title_label = memnew(Label);
	title_label->set_text("Lupine Editor Tools");
	// Skip theme styling for now to avoid potential initialization issues
	sidebar_container->add_child(title_label);
	print_line("LupineEditorPlugin: Title label added");

	// Add separator
	HSeparator *separator = memnew(HSeparator);
	sidebar_container->add_child(separator);
	print_line("LupineEditorPlugin: Separator added");

	// Create tool tabs container
	_create_tool_tabs();
	print_line("LupineEditorPlugin: Tool tabs created");
}

void LupineEditorPlugin::_create_tool_tabs() {
	tool_tabs = memnew(TabContainer);
	tool_tabs->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	tool_tabs->set_v_size_flags(Control::SIZE_EXPAND_FILL);
	tool_tabs->connect("tab_changed", callable_mp(this, &LupineEditorPlugin::_on_tool_tab_changed));
	main_split->add_child(tool_tabs);
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
}

void LupineEditorPlugin::_toggle_lupine_mode() {
	is_lupine_mode_active = !is_lupine_mode_active;

	if (toggle_button) {
		toggle_button->set_pressed(is_lupine_mode_active);
	}

	_update_tool_visibility();

	if (is_lupine_mode_active) {
		// Activate first tool by default
		if (tool_tabs && tool_tabs->get_tab_count() > 0) {
			activate_tool(world_builder->get_tool_name());
		}
	} else {
		deactivate_all_tools();
	}
}

void LupineEditorPlugin::_on_tool_tab_changed(int p_tab) {
	if (!tool_tabs || p_tab < 0 || p_tab >= tool_tabs->get_tab_count()) {
		return;
	}

	String tab_name = tool_tabs->get_tab_title(p_tab);
	activate_tool(tab_name);
}

void LupineEditorPlugin::_update_tool_visibility() {
	if (main_panel) {
		main_panel->set_visible(is_lupine_mode_active);
	}
}

void LupineEditorPlugin::register_tool(LupineEditorTool *p_tool) {
	if (!p_tool || !tool_manager || !tool_tabs) {
		return;
	}

	tool_manager->register_tool(p_tool->get_tool_name(), p_tool);

	// Add tool panel to tabs
	Control *tool_panel = p_tool->get_tool_panel();
	if (tool_panel) {
		tool_tabs->add_child(tool_panel);
		tool_tabs->set_tab_title(tool_tabs->get_tab_count() - 1, p_tool->get_tool_name());
	}
}

void LupineEditorPlugin::unregister_tool(LupineEditorTool *p_tool) {
	if (!p_tool || !tool_manager) {
		return;
	}

	tool_manager->unregister_tool(p_tool->get_tool_name());

	// Remove from tabs
	Control *tool_panel = p_tool->get_tool_panel();
	if (tool_panel && tool_tabs && tool_panel->get_parent() == tool_tabs) {
		tool_tabs->remove_child(tool_panel);
	}
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
