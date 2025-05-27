#ifndef PLATFORMER_3D_CONTROLLER_MODULE_H
#define PLATFORMER_3D_CONTROLLER_MODULE_H

#include "lupine_module_base.h"

class Platformer3DControllerModule : public LupineModuleBase {
public:
	void generate_script(Ref<FileAccess> p_file) override;
	void generate_scene(Ref<FileAccess> p_file, const String &p_scene_name) override;

	String get_module_name() const override { return "3D Platformer Controller"; }
	String get_module_description() const override { return "Advanced 3D platformer controller with Mario 64-style movement, wall jumping, swimming, and complex abilities"; }
	String get_module_category() const override { return "Player Controllers"; }
	Vector<String> get_files_to_create() const override {
		Vector<String> files;
		files.push_back("scripts/player/Player3D.gd");
		files.push_back("scripts/player/MovementState3D.gd");
		files.push_back("scripts/player/PlayerAbilities3D.gd");
		files.push_back("scripts/player/PlayerAnimator3D.gd");
		files.push_back("scenes/player/Player3D.tscn");
		return files;
	}

	bool is_global_script() const override { return false; }
};

#endif // PLATFORMER_3D_CONTROLLER_MODULE_H
