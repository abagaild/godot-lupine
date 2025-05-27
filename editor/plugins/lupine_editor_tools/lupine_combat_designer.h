#ifndef LUPINE_COMBAT_DESIGNER_H
#define LUPINE_COMBAT_DESIGNER_H

#include "lupine_editor_plugin.h"
#include "scene/gui/tab_container.h"
#include "scene/gui/tree.h"
#include "scene/gui/item_list.h"
#include "scene/gui/text_edit.h"
#include "scene/gui/spin_box.h"
#include "scene/gui/option_button.h"
#include "scene/gui/check_box.h"
#include "scene/gui/slider.h"
#include "scene/gui/progress_bar.h"
#include "scene/gui/split_container.h"
#include "scene/gui/box_container.h"
#include "scene/gui/line_edit.h"
#include "scene/gui/button.h"
#include "scene/gui/text_edit.h"
#include "scene/gui/check_box.h"
#include "scene/gui/graph_edit.h"
#include "scene/gui/graph_node.h"
#include "scene/gui/dialogs.h"
#include "scene/gui/slider.h"
#include "scene/gui/progress_bar.h"
#include "core/io/json.h"

// Combat system types
enum LupineCombatType {
	COMBAT_TYPE_TURN_BASED,
	COMBAT_TYPE_REAL_TIME,
	COMBAT_TYPE_TACTICAL_GRID,
	COMBAT_TYPE_ACTION_RPG
};

// Damage types
enum LupineDamageType {
	DAMAGE_TYPE_PHYSICAL,
	DAMAGE_TYPE_MAGICAL,
	DAMAGE_TYPE_FIRE,
	DAMAGE_TYPE_ICE,
	DAMAGE_TYPE_LIGHTNING,
	DAMAGE_TYPE_POISON,
	DAMAGE_TYPE_HEALING
};

// Status effect data
struct LupineStatusEffect {
	String id;
	String name;
	String description;
	String icon_path;
	int duration; // -1 for permanent
	bool is_beneficial;
	Dictionary stat_modifiers; // stat_name -> modifier_value
	String script_path; // Custom effect script

	LupineStatusEffect() : duration(1), is_beneficial(false) {}
};

// Skill/Ability data
struct LupineSkill {
	String id;
	String name;
	String description;
	String icon_path;
	String animation_path;
	LupineDamageType damage_type;
	int base_damage;
	int mp_cost;
	int cooldown;
	String target_type; // "single", "all_enemies", "all_allies", "self", "area"
	Vector<String> status_effects; // IDs of status effects to apply
	Dictionary custom_properties;
	String script_path; // Custom skill script

	LupineSkill() : damage_type(DAMAGE_TYPE_PHYSICAL), base_damage(0), mp_cost(0), cooldown(0) {}
};

// Character class/job data
struct LupineCharacterClass {
	String id;
	String name;
	String description;
	String icon_path;
	Dictionary base_stats; // stat_name -> base_value
	Dictionary stat_growth; // stat_name -> growth_per_level
	Vector<String> learnable_skills; // Skill IDs
	Dictionary skill_requirements; // skill_id -> level_requirement
	Vector<String> equipment_types; // Types of equipment this class can use

	LupineCharacterClass() {}
};

// Combat balance data
struct LupineCombatBalance {
	float damage_multiplier;
	float healing_multiplier;
	float status_duration_multiplier;
	int base_hit_chance;
	int base_critical_chance;
	float level_scaling_factor;
	Dictionary type_effectiveness; // damage_type -> effectiveness_multiplier

	LupineCombatBalance() :
		damage_multiplier(1.0f), healing_multiplier(1.0f), status_duration_multiplier(1.0f),
		base_hit_chance(85), base_critical_chance(5), level_scaling_factor(1.1f) {}
};

// Combat Designer tool
class LupineCombatDesigner : public LupineEditorTool {
	GDCLASS(LupineCombatDesigner, LupineEditorTool);

private:
	// UI Components
	TabContainer *main_tabs = nullptr;

	// Skills Tab
	VBoxContainer *skills_panel = nullptr;
	HSplitContainer *skills_split = nullptr;
	ItemList *skills_list = nullptr;
	VBoxContainer *skill_editor = nullptr;
	LineEdit *skill_name_edit = nullptr;
	TextEdit *skill_description_edit = nullptr;
	OptionButton *skill_damage_type = nullptr;
	SpinBox *skill_damage_spin = nullptr;
	SpinBox *skill_mp_cost_spin = nullptr;
	SpinBox *skill_cooldown_spin = nullptr;
	OptionButton *skill_target_type = nullptr;

	// Status Effects Tab
	VBoxContainer *effects_panel = nullptr;
	HSplitContainer *effects_split = nullptr;
	ItemList *effects_list = nullptr;
	VBoxContainer *effect_editor = nullptr;
	LineEdit *effect_name_edit = nullptr;
	TextEdit *effect_description_edit = nullptr;
	SpinBox *effect_duration_spin = nullptr;
	CheckBox *effect_beneficial_check = nullptr;
	Tree *effect_modifiers_tree = nullptr;

	// Classes Tab
	VBoxContainer *classes_panel = nullptr;
	HSplitContainer *classes_split = nullptr;
	ItemList *classes_list = nullptr;
	VBoxContainer *class_editor = nullptr;
	LineEdit *class_name_edit = nullptr;
	TextEdit *class_description_edit = nullptr;
	Tree *class_stats_tree = nullptr;
	Tree *class_skills_tree = nullptr;

	// Balance Tab
	VBoxContainer *balance_panel = nullptr;
	VBoxContainer *balance_editor = nullptr;
	Slider *damage_multiplier_slider = nullptr;
	Slider *healing_multiplier_slider = nullptr;
	Slider *status_duration_slider = nullptr;
	SpinBox *hit_chance_spin = nullptr;
	SpinBox *critical_chance_spin = nullptr;
	Slider *level_scaling_slider = nullptr;
	Tree *type_effectiveness_tree = nullptr;

	// Combat Flow Tab
	VBoxContainer *flow_panel = nullptr;
	GraphEdit *combat_flow_graph = nullptr;
	Button *add_flow_node_button = nullptr;

	// Preview Tab
	VBoxContainer *preview_panel = nullptr;
	VBoxContainer *combat_preview = nullptr;
	Button *simulate_combat_button = nullptr;
	TextEdit *simulation_log = nullptr;
	ProgressBar *simulation_progress = nullptr;

	// Data
	Vector<LupineSkill> skills_database;
	HashMap<String, LupineSkill*> skills_map;
	Vector<LupineStatusEffect> effects_database;
	HashMap<String, LupineStatusEffect*> effects_map;
	Vector<LupineCharacterClass> classes_database;
	HashMap<String, LupineCharacterClass*> classes_map;
	LupineCombatBalance balance_settings;

	String current_skill_id;
	String current_effect_id;
	String current_class_id;

	// Methods
	void _create_ui();
	void _create_skills_tab();
	void _create_effects_tab();
	void _create_classes_tab();
	void _create_balance_tab();
	void _create_flow_tab();
	void _create_preview_tab();

	void _populate_skills_list();
	void _populate_effects_list();
	void _populate_classes_list();
	void _update_skill_editor();
	void _update_effect_editor();
	void _update_class_editor();
	void _update_balance_editor();

	// Event handlers
	void _on_skill_selected(int p_index);
	void _on_effect_selected(int p_index);
	void _on_class_selected(int p_index);
	void _on_skill_property_changed();
	void _on_effect_property_changed();
	void _on_class_property_changed();
	void _on_balance_property_changed();
	void _on_simulate_combat_pressed();
	void _on_add_skill_pressed();
	void _on_add_effect_pressed();
	void _on_add_class_pressed();
	void _on_remove_skill_pressed();
	void _on_remove_effect_pressed();
	void _on_remove_class_pressed();

	// Data management
	LupineSkill *create_new_skill();
	LupineStatusEffect *create_new_effect();
	LupineCharacterClass *create_new_class();
	void save_combat_database();
	void load_combat_database();

	// Combat simulation
	void _simulate_combat();
	void _log_simulation_event(const String &p_event);

	// Balance testing
	void _test_damage_balance();
	void _test_skill_balance();
	void _analyze_combat_flow();

protected:
	static void _bind_methods();

public:
	LupineCombatDesigner();
	virtual ~LupineCombatDesigner();

	// LupineEditorTool interface
	virtual void initialize() override;
	virtual void cleanup() override;
	virtual Control *get_tool_panel() override { return tool_panel; }
	virtual String get_tool_name() const override { return "Combat Designer"; }
	virtual String get_tool_description() const override { return "Battle system configuration and balance tools"; }
	virtual String get_tool_category() const override { return "Combat Design"; }

	// Database access
	LupineSkill *get_skill(const String &p_skill_id);
	LupineStatusEffect *get_effect(const String &p_effect_id);
	LupineCharacterClass *get_class(const String &p_class_id);
	Vector<LupineSkill> get_all_skills() const { return skills_database; }
	Vector<LupineStatusEffect> get_all_effects() const { return effects_database; }
	Vector<LupineCharacterClass> get_all_classes() const { return classes_database; }

	// Export functionality
	void export_combat_data(const String &p_path);
	void import_combat_data(const String &p_path);
	Dictionary skill_to_dictionary(const LupineSkill &p_skill);
	Dictionary effect_to_dictionary(const LupineStatusEffect &p_effect);
	Dictionary class_to_dictionary(const LupineCharacterClass &p_class);
	LupineSkill skill_from_dictionary(const Dictionary &p_dict);
	LupineStatusEffect effect_from_dictionary(const Dictionary &p_dict);
	LupineCharacterClass class_from_dictionary(const Dictionary &p_dict);
};

// Combat Simulation Dialog
class LupineCombatSimulationDialog : public AcceptDialog {
	GDCLASS(LupineCombatSimulationDialog, AcceptDialog);

private:
	LupineCombatDesigner *combat_designer = nullptr;
	OptionButton *combat_type_option = nullptr;
	SpinBox *party_size_spin = nullptr;
	SpinBox *enemy_count_spin = nullptr;
	SpinBox *simulation_rounds_spin = nullptr;
	CheckBox *detailed_log_check = nullptr;
	TextEdit *results_display = nullptr;
	ProgressBar *simulation_progress = nullptr;

	void _on_simulate_pressed();
	void _run_simulation();

protected:
	static void _bind_methods();

public:
	LupineCombatSimulationDialog(LupineCombatDesigner *p_designer);
	virtual ~LupineCombatSimulationDialog();

	void show_simulation_dialog();
};

#endif // LUPINE_COMBAT_DESIGNER_H
