#ifndef PLAYER_CONTROLLER_2D_TOPDOWN_8DIR_MODULE_H
#define PLAYER_CONTROLLER_2D_TOPDOWN_8DIR_MODULE_H

#include "lupine_module_base.h"

class PlayerController2DTopdown8DirModule : public LupineModuleBase {
public:
	void generate_script(Ref<FileAccess> p_file) override;
	void generate_scene(Ref<FileAccess> p_file, const String &p_scene_name) override;

	String get_module_name() const override { return "2D Top-down Player Controller (8-Direction)"; }
	String get_module_description() const override { return "8-directional movement controller with health, stamina, combat, dash, and interaction systems"; }
	String get_module_category() const override { return "Player Controllers"; }
	Vector<String> get_files_to_create() const override {
		Vector<String> files;
		files.push_back("scripts/PlayerController2DTopdown8Dir.gd");
		files.push_back("scenes/Player2DTopdown8Dir.tscn");
		return files;
	}

	bool is_global_script() const override { return false; }
};

#endif // PLAYER_CONTROLLER_2D_TOPDOWN_8DIR_MODULE_H
