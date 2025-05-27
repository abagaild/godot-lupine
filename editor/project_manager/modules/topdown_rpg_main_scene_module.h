#ifndef TOPDOWN_RPG_MAIN_SCENE_MODULE_H
#define TOPDOWN_RPG_MAIN_SCENE_MODULE_H

#include "lupine_module_base.h"

class TopdownRPGMainSceneModule : public LupineModuleBase {
public:
	void generate_script(Ref<FileAccess> p_file) override;
	void generate_scene(Ref<FileAccess> p_file, const String &p_scene_name) override;
	
	String get_module_name() const override { return "Top-down RPG Main Scene"; }
	String get_module_description() const override { return "Complete main scene setup for top-down RPG with player, camera, UI, world manager, and game systems"; }
	String get_module_category() const override { return "Main Scenes"; }
	Vector<String> get_files_to_create() const override {
		Vector<String> files;
		files.push_back("scenes/Main.tscn");
		files.push_back("scripts/GameManager.gd");
		files.push_back("scripts/WorldManager.gd");
		files.push_back("scripts/InputManager.gd");
		files.push_back("scenes/ui/GameUI.tscn");
		files.push_back("scripts/ui/GameUI.gd");
		return files;
	}

	bool is_global_script() const override { return true; }
	String get_autoload_name() const override { return "GameManager"; }
};

#endif // TOPDOWN_RPG_MAIN_SCENE_MODULE_H
