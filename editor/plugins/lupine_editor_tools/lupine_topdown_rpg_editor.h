#ifndef LUPINE_TOPDOWN_RPG_EDITOR_H
#define LUPINE_TOPDOWN_RPG_EDITOR_H

#include "lupine_editor_plugin.h"
#include "scene/gui/control.h"
#include "scene/gui/tab_container.h"
#include "scene/gui/split_container.h"
#include "scene/gui/item_list.h"
#include "scene/gui/tree.h"
#include "scene/gui/button.h"
#include "scene/gui/line_edit.h"
#include "scene/gui/text_edit.h"
#include "scene/gui/spin_box.h"
#include "scene/gui/option_button.h"
#include "scene/gui/check_box.h"
#include "scene/gui/label.h"
#include "scene/gui/panel.h"
#include "scene/gui/scroll_container.h"
#include "scene/2d/node_2d.h"
#include "scene/2d/tile_map.h"

// Forward declarations
class LupineTopdownMapEditor;
class LupineTopdownEntityPlacer;

// Map tile data structure
struct TopdownMapTile {
	String tile_id;
	String tile_name;
	String category;
	Vector2i atlas_coords;
	bool is_collision;
	bool is_interactive;
	Dictionary metadata;

	TopdownMapTile() : is_collision(false), is_interactive(false) {}
};

// Entity placement data
struct TopdownEntityData {
	String entity_id;
	String entity_name;
	String entity_type; // "npc", "enemy", "item", "trigger", "decoration"
	Vector2 position;
	Dictionary properties;
	String script_path;
	String scene_path;

	TopdownEntityData() {}
};

// Event trigger data
struct TopdownEventTrigger {
	String trigger_id;
	String trigger_name;
	String trigger_type; // "area", "interaction", "auto", "conditional"
	Vector2 position;
	Vector2 size;
	Vector<String> conditions;
	Vector<String> actions;
	bool is_repeatable;

	TopdownEventTrigger() : is_repeatable(true) {}
};

// Scene transition data
struct TopdownSceneTransition {
	String transition_id;
	String target_scene;
	Vector2 door_position;
	Vector2 spawn_position;
	String transition_type; // "door", "teleport", "fade", "slide"
	Dictionary requirements; // Keys, items, etc.

	TopdownSceneTransition() {}
};

// Main Topdown RPG Editor Tool
class LupineTopdownRPGEditor : public LupineEditorTool {
	GDCLASS(LupineTopdownRPGEditor, LupineEditorTool);

private:
	// UI Components
	Control *tool_panel = nullptr;
	TabContainer *main_tabs = nullptr;

	// Sub-editors
	LupineTopdownMapEditor *map_editor = nullptr;
	LupineTopdownEntityPlacer *entity_placer = nullptr;

	// Current scene data
	Node2D *current_scene = nullptr;
	String current_scene_path;
	Vector<TopdownEntityData> placed_entities;
	Vector<TopdownEventTrigger> event_triggers;
	Vector<TopdownSceneTransition> scene_transitions;

	// Methods
	void _create_ui();
	void _setup_tabs();
	void _on_scene_changed(Node *p_scene);
	void _save_scene_data();
	void _load_scene_data();

protected:
	static void _bind_methods();

public:
	LupineTopdownRPGEditor();
	virtual ~LupineTopdownRPGEditor();

	// LupineEditorTool interface
	virtual void initialize() override;
	virtual void cleanup() override;
	virtual Control *get_tool_panel() override { return tool_panel; }
	virtual String get_tool_name() const override { return "Topdown RPG Editor"; }
	virtual String get_tool_description() const override { return "Complete RPG Maker-style editor for topdown RPG games"; }
	virtual String get_tool_category() const override { return "RPG Development"; }

	// Scene integration
	virtual void on_scene_changed(Node *p_scene) override;
	virtual void on_node_selected(Node *p_node) override;

	// Data access
	Vector<TopdownEntityData> get_placed_entities() const { return placed_entities; }
	Vector<TopdownEventTrigger> get_event_triggers() const { return event_triggers; }
	Vector<TopdownSceneTransition> get_scene_transitions() const { return scene_transitions; }

	// Entity management
	void add_entity(const TopdownEntityData &p_entity);
	void remove_entity(const String &p_entity_id);
	void update_entity(const String &p_entity_id, const TopdownEntityData &p_entity);

	// Event management
	void add_event_trigger(const TopdownEventTrigger &p_trigger);
	void remove_event_trigger(const String &p_trigger_id);
	void update_event_trigger(const String &p_trigger_id, const TopdownEventTrigger &p_trigger);

	// Transition management
	void add_scene_transition(const TopdownSceneTransition &p_transition);
	void remove_scene_transition(const String &p_transition_id);
	void update_scene_transition(const String &p_transition_id, const TopdownSceneTransition &p_transition);
};

// Map Editor Sub-component
class LupineTopdownMapEditor : public Control {
	GDCLASS(LupineTopdownMapEditor, Control);

private:
	LupineTopdownRPGEditor *parent_editor = nullptr;

	// UI Components
	HSplitContainer *main_split = nullptr;
	VBoxContainer *tile_palette = nullptr;
	Control *map_canvas = nullptr;
	ItemList *tile_list = nullptr;
	Tree *layer_tree = nullptr;

	// Map data
	TileMap *current_tilemap = nullptr;
	Vector<TopdownMapTile> available_tiles;
	String selected_tile_id;
	int current_layer = 0;

	// Tools
	enum EditMode {
		MODE_PAINT,
		MODE_ERASE,
		MODE_FILL,
		MODE_SELECT
	};
	EditMode current_mode = MODE_PAINT;

	// Methods
	void _create_ui();
	void _setup_tile_palette();
	void _setup_layer_management();
	void _on_tile_selected(int p_index);
	void _on_canvas_input(const Ref<InputEvent> &p_event);
	void _paint_tile(Vector2i p_coords);
	void _erase_tile(Vector2i p_coords);
	void _flood_fill(Vector2i p_coords);

protected:
	static void _bind_methods();

public:
	LupineTopdownMapEditor(LupineTopdownRPGEditor *p_parent);
	virtual ~LupineTopdownMapEditor();

	void set_tilemap(TileMap *p_tilemap);
	void refresh_tile_palette();
	void set_edit_mode(EditMode p_mode) { current_mode = p_mode; }
};

// Entity Placer Sub-component
class LupineTopdownEntityPlacer : public Control {
	GDCLASS(LupineTopdownEntityPlacer, Control);

private:
	LupineTopdownRPGEditor *parent_editor = nullptr;

	// UI Components
	HSplitContainer *main_split = nullptr;
	VBoxContainer *entity_palette = nullptr;
	Control *placement_canvas = nullptr;
	Tree *entity_tree = nullptr;
	VBoxContainer *properties_panel = nullptr;

	// Entity data
	Vector<TopdownEntityData> entity_templates;
	String selected_entity_type;
	TopdownEntityData *selected_entity = nullptr;

	// Methods
	void _create_ui();
	void _setup_entity_palette();
	void _setup_properties_panel();
	void _on_entity_selected();
	void _on_canvas_input(const Ref<InputEvent> &p_event);
	void _place_entity(Vector2 p_position);
	void _select_entity(Vector2 p_position);
	void _update_properties_panel();

protected:
	static void _bind_methods();

public:
	LupineTopdownEntityPlacer(LupineTopdownRPGEditor *p_parent);
	virtual ~LupineTopdownEntityPlacer();

	void refresh_entity_palette();
	void refresh_placed_entities();
};

#endif // LUPINE_TOPDOWN_RPG_EDITOR_H
