#ifndef QUEST_SYSTEM_MODULE_H
#define QUEST_SYSTEM_MODULE_H

#include "lupine_module_base.h"

class QuestSystemModule : public LupineModuleBase {
public:
	void generate_script(Ref<FileAccess> p_file) override;
	void generate_scene(Ref<FileAccess> p_file, const String &p_scene_name) override;

	String get_module_name() const override { return "Quest System"; }
	String get_module_description() const override { return "Comprehensive quest system with fetch, kill-count, escort, and multi-stage quests with auto-tracking and journal UI"; }
	String get_module_category() const override { return "RPG Systems"; }
	Vector<String> get_files_to_create() const override {
		Vector<String> files;
		files.push_back("globals/QuestSystem.gd");
		files.push_back("scripts/quests/Quest.gd");
		files.push_back("scripts/quests/QuestObjective.gd");
		files.push_back("scripts/ui/QuestJournal.gd");
		files.push_back("scripts/ui/QuestTracker.gd");
		files.push_back("scenes/ui/QuestJournal.tscn");
		files.push_back("scenes/ui/QuestTracker.tscn");
		return files;
	}

	bool is_global_script() const override { return true; }
	String get_autoload_name() const override { return "QuestSystem"; }
};

#endif // QUEST_SYSTEM_MODULE_H
