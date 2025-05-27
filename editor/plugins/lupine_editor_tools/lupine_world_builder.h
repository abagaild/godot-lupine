#ifndef LUPINE_WORLD_BUILDER_H
#define LUPINE_WORLD_BUILDER_H

#include "lupine_editor_plugin.h"
#include "scene/2d/node_2d.h"
#include "scene/3d/node_3d.h"
#include "scene/gui/tree.h"
#include "scene/gui/item_list.h"
#include "scene/gui/grid_container.h"
#include "scene/gui/flow_container.h"
#include "scene/gui/texture_rect.h"
#include "scene/gui/rich_text_label.h"
#include "scene/gui/box_container.h"
#include "scene/gui/split_container.h"
#include "scene/gui/option_button.h"
#include "scene/gui/line_edit.h"
#include "scene/gui/label.h"
#include "scene/gui/scroll_container.h"
#include "scene/resources/texture.h"
#include "scene/resources/packed_scene.h"
#include "core/io/resource_loader.h"

class LupineEntityPalette;
class LupineSceneCanvas;
class LupinePropertyPanel;

// Represents a placeable entity in the world builder
struct LupineEntity {
	String id;
	String name;
	String description;
	String category;
	String scene_path;
	Ref<Texture2D> icon;
	Ref<PackedScene> scene_template;
	Dictionary default_properties;
	Vector2 grid_size = Vector2(1, 1); // Size in grid units
	bool is_3d = false;

	LupineEntity() {}
	LupineEntity(const String &p_id, const String &p_name, const String &p_category) :
		id(p_id), name(p_name), category(p_category) {}
};

// Main World Builder tool
class LupineWorldBuilder : public LupineEditorTool {
	GDCLASS(LupineWorldBuilder, LupineEditorTool);

private:
	// UI Components
	HSplitContainer *main_split = nullptr;
	VBoxContainer *left_panel = nullptr;
	VBoxContainer *right_panel = nullptr;

	// Entity palette
	LupineEntityPalette *entity_palette = nullptr;

	// Scene canvas
	LupineSceneCanvas *scene_canvas = nullptr;

	// Property panel
	LupinePropertyPanel *property_panel = nullptr;

	// Tool state
	Node *current_scene = nullptr;
	Node *selected_node = nullptr;
	LupineEntity *selected_entity_type = nullptr;
	bool is_placing_mode = false;
	bool snap_to_grid = true;
	int grid_size = 32;

	// Entity library
	Vector<LupineEntity> entity_library;
	HashMap<String, LupineEntity*> entity_map;

	// Methods
	void _create_ui();
	void _setup_entity_library();
	void _load_entity_definitions();
	void _on_entity_selected(const String &p_entity_id);
	void _on_canvas_clicked(Vector2 p_position);
	void _on_node_selected_in_canvas(Node *p_node);
	void _place_entity_at_position(Vector2 p_position);
	void _update_property_panel();
	void _on_property_changed(const String &p_property, const Variant &p_value);

	// Grid and snapping
	Vector2 _snap_to_grid(Vector2 p_position);
	void _draw_grid();

protected:
	static void _bind_methods();

public:
	LupineWorldBuilder();
	virtual ~LupineWorldBuilder();

	// LupineEditorTool interface
	virtual void initialize() override;
	virtual void cleanup() override;
	virtual Control *get_tool_panel() override { return tool_panel; }
	virtual void activate() override;
	virtual void deactivate() override;
	virtual String get_tool_name() const override { return "World Builder"; }
	virtual String get_tool_description() const override { return "Visual scene composition with drag-and-drop entity placement"; }
	virtual String get_tool_category() const override { return "World Design"; }

	// Scene integration
	virtual void on_scene_changed(Node *p_scene) override;
	virtual void on_node_selected(Node *p_node) override;

	// Entity management
	void add_entity_type(const LupineEntity &p_entity);
	void remove_entity_type(const String &p_id);
	LupineEntity *get_entity_type(const String &p_id);
	Vector<LupineEntity> get_entities_by_category(const String &p_category);
	const Vector<LupineEntity> &get_entity_library() const { return entity_library; }

	// Tool settings
	void set_snap_to_grid(bool p_enabled) { snap_to_grid = p_enabled; }
	bool get_snap_to_grid() const { return snap_to_grid; }
	void set_grid_size(int p_size) { grid_size = p_size; }
	int get_grid_size() const { return grid_size; }
};

// Entity Palette - shows available entities to place
class LupineEntityPalette : public VBoxContainer {
	GDCLASS(LupineEntityPalette, VBoxContainer);

private:
	LupineWorldBuilder *world_builder = nullptr;
	OptionButton *category_filter = nullptr;
	LineEdit *search_filter = nullptr;
	ItemList *entity_list = nullptr;
	RichTextLabel *entity_description = nullptr;

	Vector<LupineEntity> filtered_entities;

	void _on_category_changed(int p_index);
	void _on_search_changed(const String &p_text);
	void _on_entity_selected(int p_index);
	void _update_entity_list();
	void _update_description();

protected:
	static void _bind_methods();

public:
	LupineEntityPalette(LupineWorldBuilder *p_builder);
	virtual ~LupineEntityPalette();

	void refresh_entities();
	void set_selected_entity(const LupineEntity *p_entity);
};

// Scene Canvas - visual representation of the scene with placement tools
class LupineSceneCanvas : public Control {
	GDCLASS(LupineSceneCanvas, Control);

private:
	LupineWorldBuilder *world_builder = nullptr;
	Node *scene_root = nullptr;

	// Visual state
	bool show_grid = true;
	bool show_gizmos = true;
	Color grid_color = Color(0.3, 0.3, 0.3, 0.5);
	Color selection_color = Color(1.0, 0.5, 0.0, 0.8);

	// Interaction state
	Node *hovered_node = nullptr;
	Vector2 last_mouse_position;
	bool is_dragging = false;

	void _draw_grid();
	void _draw_gizmos();
	void _draw_node_gizmos(Node *p_node);
	void _draw_selection();
	Node *_get_node_at_position(Vector2 p_position);
	Node *_find_node_at_position(Node *p_node, Vector2 p_position);
	Vector2 _world_to_canvas(Vector2 p_world_pos);
	Vector2 _canvas_to_world(Vector2 p_canvas_pos);

protected:
	static void _bind_methods();
	virtual void _notification(int p_what);
	virtual void _gui_input(const Ref<InputEvent> &p_event);

public:
	LupineSceneCanvas(LupineWorldBuilder *p_builder);
	virtual ~LupineSceneCanvas();

	void set_scene_root(Node *p_scene);
	Node *get_scene_root() const { return scene_root; }

	// Visual settings
	void set_show_grid(bool p_show) { show_grid = p_show; queue_redraw(); }
	bool get_show_grid() const { return show_grid; }
	void set_show_gizmos(bool p_show) { show_gizmos = p_show; queue_redraw(); }
	bool get_show_gizmos() const { return show_gizmos; }

	// Signals
	void emit_canvas_clicked(Vector2 p_position);
	void emit_node_selected(Node *p_node);
};

// Property Panel - shows and edits properties of selected objects
class LupinePropertyPanel : public VBoxContainer {
	GDCLASS(LupinePropertyPanel, VBoxContainer);

private:
	LupineWorldBuilder *world_builder = nullptr;
	Label *title_label = nullptr;
	ScrollContainer *properties_scroll = nullptr;
	VBoxContainer *properties_container = nullptr;

	Node *current_node = nullptr;
	HashMap<String, Control*> property_controls;

	void _create_property_control(const String &p_name, const Variant &p_value);
	void _update_property_control(const String &p_name, const Variant &p_value);
	void _clear_properties();
	void _on_property_control_changed(const String &p_name, const Variant &p_value);

protected:
	static void _bind_methods();

public:
	LupinePropertyPanel(LupineWorldBuilder *p_builder);
	virtual ~LupinePropertyPanel();

	void set_target_node(Node *p_node);
	Node *get_target_node() const { return current_node; }
	void refresh_properties();
};

#endif // LUPINE_WORLD_BUILDER_H
