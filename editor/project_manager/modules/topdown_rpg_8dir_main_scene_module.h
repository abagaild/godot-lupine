#ifndef TOPDOWN_RPG_8DIR_MAIN_SCENE_MODULE_H
#define TOPDOWN_RPG_8DIR_MAIN_SCENE_MODULE_H

#include "lupine_module_base.h"

class TopdownRPG8DirMainSceneModule : public LupineModuleBase {
public:
	void generate_script(Ref<FileAccess> p_file) override;
	void generate_scene(Ref<FileAccess> p_file, const String &p_scene_name) override;

	String get_module_name() const override { return "Top-down RPG Main Scene (8-Direction)"; }
	String get_module_description() const override { return "Complete main scene setup for 8-directional top-down RPG with example content"; }
	String get_module_category() const override { return "Scene Management"; }
	Vector<String> get_files_to_create() const override {
		Vector<String> files;
		files.push_back("scenes/Main.tscn");
		files.push_back("scripts/GameManager.gd");
		files.push_back("scripts/WorldManager.gd");
		files.push_back("scripts/InputManager.gd");
		files.push_back("scenes/ui/GameUI.tscn");
		files.push_back("scripts/ui/GameUI.gd");
		files.push_back("scripts/characters/BasicNPC.gd");
		files.push_back("scripts/world/TreasureChest.gd");
		return files;
	}

	bool is_global_script() const override { return false; }
};

#endif // TOPDOWN_RPG_8DIR_MAIN_SCENE_MODULE_H
