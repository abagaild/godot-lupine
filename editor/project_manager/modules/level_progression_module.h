#ifndef LEVEL_PROGRESSION_MODULE_H
#define LEVEL_PROGRESSION_MODULE_H

#include "lupine_module_base.h"

class LevelProgressionModule : public LupineModuleBase {
public:
	void generate_script(Ref<FileAccess> p_file) override;
	void generate_scene(Ref<FileAccess> p_file, const String &p_scene_name) override;

	String get_module_name() const override { return "Level Progression System"; }
	String get_module_description() const override { return "Level progression with save/load, checkpoints, area transitions, and game state management"; }
	String get_module_category() const override { return "RPG Systems"; }
	Vector<String> get_files_to_create() const override {
		Vector<String> files;
		files.push_back("globals/LevelManager.gd");
		files.push_back("globals/SaveSystem.gd");
		files.push_back("scripts/world/Checkpoint.gd");
		files.push_back("scripts/world/AreaTransition.gd");
		files.push_back("scripts/world/LevelExit.gd");
		files.push_back("scenes/world/Checkpoint.tscn");
		files.push_back("scenes/world/AreaTransition.tscn");
		files.push_back("scenes/world/LevelExit.tscn");
		return files;
	}

	bool is_global_script() const override { return true; }
	String get_autoload_name() const override { return "LevelManager"; }
};

#endif // LEVEL_PROGRESSION_MODULE_H
