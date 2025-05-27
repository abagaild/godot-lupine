#ifndef LUPINE_QUEST_DESIGNER_H
#define LUPINE_QUEST_DESIGNER_H

#include "lupine_editor_plugin.h"
#include "scene/gui/graph_edit.h"
#include "scene/gui/graph_node.h"
#include "scene/gui/tree.h"
#include "scene/gui/item_list.h"
#include "scene/gui/text_edit.h"
#include "scene/gui/spin_box.h"
#include "scene/gui/option_button.h"
#include "scene/gui/check_box.h"
#include "scene/gui/split_container.h"
#include "scene/gui/box_container.h"
#include "scene/gui/tab_container.h"
#include "scene/gui/line_edit.h"
#include "scene/gui/button.h"
#include "scene/gui/dialogs.h"
#include "scene/resources/packed_scene.h"
#include "core/io/json.h"

// Quest objective types
enum LupineQuestObjectiveType {
	QUEST_OBJECTIVE_TALK,
	QUEST_OBJECTIVE_KILL,
	QUEST_OBJECTIVE_COLLECT,
	QUEST_OBJECTIVE_DELIVER,
	QUEST_OBJECTIVE_ESCORT,
	QUEST_OBJECTIVE_REACH_LOCATION,
	QUEST_OBJECTIVE_INTERACT,
	QUEST_OBJECTIVE_CUSTOM
};

// Quest status
enum LupineQuestStatus {
	QUEST_STATUS_INACTIVE,
	QUEST_STATUS_AVAILABLE,
	QUEST_STATUS_ACTIVE,
	QUEST_STATUS_COMPLETED,
	QUEST_STATUS_FAILED
};

// Quest objective data
struct LupineQuestObjective {
	String id;
	String title;
	String description;
	LupineQuestObjectiveType type;
	Dictionary parameters; // Type-specific parameters
	bool is_optional;
	bool is_completed;
	Vector<String> prerequisites; // Other objective IDs that must be completed first

	LupineQuestObjective() : type(QUEST_OBJECTIVE_TALK), is_optional(false), is_completed(false) {}
};

// Quest reward data
struct LupineQuestReward {
	String type; // "item", "gold", "experience", "custom"
	String item_id;
	int quantity;
	Dictionary custom_data;

	LupineQuestReward() : quantity(1) {}
};

// Complete quest data
struct LupineQuest {
	String id;
	String title;
	String description;
	String category;
	int level_requirement;
	LupineQuestStatus status;
	Vector<LupineQuestObjective> objectives;
	Vector<LupineQuestReward> rewards;
	Vector<String> prerequisites; // Other quest IDs
	Dictionary metadata;

	LupineQuest() : level_requirement(1), status(QUEST_STATUS_INACTIVE) {}
};

// Visual Quest Designer tool
class LupineQuestDesigner : public LupineEditorTool {
	GDCLASS(LupineQuestDesigner, LupineEditorTool);

private:
	// UI Components
	HSplitContainer *main_split = nullptr;
	VBoxContainer *left_panel = nullptr;
	TabContainer *main_tabs = nullptr;

	// Quest List Tab
	VBoxContainer *quest_list_panel = nullptr;
	ItemList *quest_list = nullptr;
	Button *new_quest_button = nullptr;
	Button *delete_quest_button = nullptr;
	Button *duplicate_quest_button = nullptr;

	// Quest Editor Tab
	VBoxContainer *quest_editor_panel = nullptr;
	LineEdit *quest_title_edit = nullptr;
	TextEdit *quest_description_edit = nullptr;
	LineEdit *quest_category_edit = nullptr;
	SpinBox *quest_level_spin = nullptr;
	OptionButton *quest_status_option = nullptr;

	// Objectives Tab
	VBoxContainer *objectives_panel = nullptr;
	Tree *objectives_tree = nullptr;
	Button *add_objective_button = nullptr;
	Button *remove_objective_button = nullptr;
	VBoxContainer *objective_details_panel = nullptr;

	// Rewards Tab
	VBoxContainer *rewards_panel = nullptr;
	Tree *rewards_tree = nullptr;
	Button *add_reward_button = nullptr;
	Button *remove_reward_button = nullptr;
	VBoxContainer *reward_details_panel = nullptr;

	// Flow Designer Tab
	GraphEdit *quest_flow_graph = nullptr;
	Button *add_node_button = nullptr;

	// Quest data
	Vector<LupineQuest> quest_database;
	HashMap<String, LupineQuest*> quest_map;
	String current_quest_id;
	String current_objective_id;
	String current_reward_id;

	// Methods
	void _create_ui();
	void _create_quest_list_tab();
	void _create_quest_editor_tab();
	void _create_objectives_tab();
	void _create_rewards_tab();
	void _create_flow_designer_tab();

	void _populate_quest_list();
	void _update_quest_editor();
	void _update_objectives_tree();
	void _update_rewards_tree();
	void _update_objective_details();
	void _update_reward_details();

	// Event handlers
	void _on_quest_selected(int p_index);
	void _on_new_quest_pressed();
	void _on_delete_quest_pressed();
	void _perform_delete_quest();
	void _on_duplicate_quest_pressed();
	void _on_quest_property_changed();
	void _on_objective_selected();
	void _on_add_objective_pressed();
	void _on_remove_objective_pressed();
	void _on_objective_property_changed();
	void _on_reward_selected();
	void _on_add_reward_pressed();
	void _on_remove_reward_pressed();
	void _on_reward_property_changed();
	void _on_add_flow_node_pressed();

	// Quest management
	LupineQuest *create_new_quest();
	void delete_quest(const String &p_quest_id);
	LupineQuest *duplicate_quest(const String &p_quest_id);
	void save_quest_database();
	void load_quest_database();

	// Objective management
	LupineQuestObjective *add_objective_to_quest(const String &p_quest_id);
	void remove_objective_from_quest(const String &p_quest_id, const String &p_objective_id);

	// Reward management
	LupineQuestReward *add_reward_to_quest(const String &p_quest_id);
	void remove_reward_from_quest(const String &p_quest_id, const String &p_reward_id);

	// Flow graph
	void _create_quest_flow_node(const String &p_quest_id, Vector2 p_position);
	void _update_quest_flow_graph();

protected:
	static void _bind_methods();

public:
	LupineQuestDesigner();
	virtual ~LupineQuestDesigner();

	// LupineEditorTool interface
	virtual void initialize() override;
	virtual void cleanup() override;
	virtual Control *get_tool_panel() override { return tool_panel; }
	virtual String get_tool_name() const override { return "Quest Designer"; }
	virtual String get_tool_description() const override { return "Visual quest creation and management tools"; }
	virtual String get_tool_category() const override { return "Narrative Design"; }

	// Quest database access
	LupineQuest *get_quest(const String &p_quest_id);
	Vector<LupineQuest> get_all_quests() const { return quest_database; }
	Vector<LupineQuest> get_quests_by_category(const String &p_category);

	// Export functionality
	void export_quest_data(const String &p_path);
	void import_quest_data(const String &p_path);
	Dictionary quest_to_dictionary(const LupineQuest &p_quest);
	LupineQuest quest_from_dictionary(const Dictionary &p_dict);
};

// Quest Objective Editor Dialog
class LupineQuestObjectiveDialog : public AcceptDialog {
	GDCLASS(LupineQuestObjectiveDialog, AcceptDialog);

private:
	LupineQuestDesigner *quest_designer = nullptr;
	LupineQuestObjective *current_objective = nullptr;

	LineEdit *title_edit = nullptr;
	TextEdit *description_edit = nullptr;
	OptionButton *type_option = nullptr;
	CheckBox *optional_check = nullptr;
	VBoxContainer *parameters_container = nullptr;

	void _on_type_changed(int p_index);
	void _update_parameters_ui();
	void _on_confirmed();

protected:
	static void _bind_methods();

public:
	LupineQuestObjectiveDialog(LupineQuestDesigner *p_designer);
	virtual ~LupineQuestObjectiveDialog();

	void edit_objective(LupineQuestObjective *p_objective);
};

// Quest Reward Editor Dialog
class LupineQuestRewardDialog : public AcceptDialog {
	GDCLASS(LupineQuestRewardDialog, AcceptDialog);

private:
	LupineQuestDesigner *quest_designer = nullptr;
	LupineQuestReward *current_reward = nullptr;

	OptionButton *type_option = nullptr;
	LineEdit *item_id_edit = nullptr;
	SpinBox *quantity_spin = nullptr;
	VBoxContainer *custom_container = nullptr;

	void _on_type_changed(int p_index);
	void _update_ui();
	void _on_confirmed();

protected:
	static void _bind_methods();

public:
	LupineQuestRewardDialog(LupineQuestDesigner *p_designer);
	virtual ~LupineQuestRewardDialog();

	void edit_reward(LupineQuestReward *p_reward);
};

#endif // LUPINE_QUEST_DESIGNER_H
