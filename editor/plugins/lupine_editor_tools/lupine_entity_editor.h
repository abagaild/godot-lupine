#ifndef LUPINE_ENTITY_EDITOR_H
#define LUPINE_ENTITY_EDITOR_H

#include "lupine_editor_plugin.h"
#include "scene/gui/control.h"
#include "scene/gui/split_container.h"
#include "scene/gui/tab_container.h"
#include "scene/gui/tree.h"
#include "scene/gui/item_list.h"
#include "scene/gui/line_edit.h"
#include "scene/gui/spin_box.h"
#include "scene/gui/option_button.h"
#include "scene/gui/check_box.h"
#include "scene/gui/text_edit.h"
#include "scene/gui/button.h"
#include "scene/gui/label.h"
#include "scene/gui/separator.h"
#include "scene/gui/scroll_container.h"
#include "scene/gui/panel_container.h"

// Entity Editor - RPG Maker style entity management
class LupineEntityEditor : public LupineEditorTool {
	GDCLASS(LupineEntityEditor, LupineEditorTool);

private:
	// Main UI components
	HSplitContainer *main_split = nullptr;
	VBoxContainer *left_panel = nullptr;
	TabContainer *right_panel = nullptr;

	// Entity list and management
	VBoxContainer *entity_list_container = nullptr;
	ItemList *entity_list = nullptr;
	Button *new_entity_btn = nullptr;
	Button *duplicate_entity_btn = nullptr;
	Button *delete_entity_btn = nullptr;
	Button *import_entity_btn = nullptr;
	Button *export_entity_btn = nullptr;

	// Entity editing tabs
	Control *basic_tab = nullptr;
	Control *visuals_tab = nullptr;
	Control *behavior_tab = nullptr;
	Control *stats_tab = nullptr;
	Control *equipment_tab = nullptr;
	Control *ai_tab = nullptr;

	// Basic properties
	LineEdit *entity_name_edit = nullptr;
	OptionButton *entity_type_option = nullptr;
	TextEdit *entity_description_edit = nullptr;
	CheckBox *entity_enabled_check = nullptr;

	// Visual properties
	LineEdit *sprite_path_edit = nullptr;
	Button *sprite_browse_btn = nullptr;
	SpinBox *sprite_scale_spin = nullptr;
	OptionButton *animation_set_option = nullptr;
	CheckBox *flip_h_check = nullptr;
	CheckBox *flip_v_check = nullptr;

	// Behavior properties
	OptionButton *movement_type_option = nullptr;
	SpinBox *movement_speed_spin = nullptr;
	CheckBox *can_interact_check = nullptr;
	LineEdit *interaction_script_edit = nullptr;
	CheckBox *blocks_movement_check = nullptr;

	// Stats properties (for RPG entities)
	SpinBox *health_spin = nullptr;
	SpinBox *mana_spin = nullptr;
	SpinBox *attack_spin = nullptr;
	SpinBox *defense_spin = nullptr;
	SpinBox *speed_spin = nullptr;
	SpinBox *level_spin = nullptr;
	SpinBox *experience_spin = nullptr;

	// Current entity data
	Dictionary current_entity_data;
	int current_entity_index = -1;
	Vector<Dictionary> entity_database;

	void _create_ui();
	void _create_entity_list();
	void _create_basic_tab();
	void _create_visuals_tab();
	void _create_behavior_tab();
	void _create_stats_tab();
	void _create_equipment_tab();
	void _create_ai_tab();

	// Event handlers
	void _on_entity_selected(int p_index);
	void _on_new_entity_pressed();
	void _on_duplicate_entity_pressed();
	void _on_delete_entity_pressed();
	void _on_import_entity_pressed();
	void _on_export_entity_pressed();
	void _on_sprite_browse_pressed();
	void _on_entity_property_changed();

	// Data management
	void _load_entity_data(int p_index);
	void _save_current_entity();
	void _update_entity_list();
	void _create_new_entity();
	Dictionary _create_default_entity_data();
	void _load_entity_database();
	void _save_entity_database();

protected:
	static void _bind_methods();

public:
	LupineEntityEditor();
	virtual ~LupineEntityEditor();

	// LupineEditorTool interface
	virtual void initialize() override;
	virtual void cleanup() override;
	virtual String get_tool_name() const override { return "Entity Editor"; }
	virtual String get_tool_description() const override { return "RPG Maker style entity management for players, enemies, and NPCs"; }
	virtual String get_tool_category() const override { return "Core Tools"; }

	// Entity management
	void add_entity(const Dictionary &p_entity_data);
	void remove_entity(int p_index);
	Dictionary get_entity_data(int p_index) const;
	void set_entity_data(int p_index, const Dictionary &p_data);
	int get_entity_count() const;

	// Import/Export
	void import_entities_from_file(const String &p_path);
	void export_entities_to_file(const String &p_path);
	void export_entity_to_file(int p_index, const String &p_path);
};

#endif // LUPINE_ENTITY_EDITOR_H
