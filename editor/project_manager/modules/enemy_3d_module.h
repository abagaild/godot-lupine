#ifndef ENEMY_3D_MODULE_H
#define ENEMY_3D_MODULE_H

#include "lupine_module_base.h"

class Enemy3DModule : public LupineModuleBase {
public:
	void generate_script(Ref<FileAccess> p_file) override;
	void generate_scene(Ref<FileAccess> p_file, const String &p_scene_name) override;

	String get_module_name() const override { return "3D Enemy System"; }
	String get_module_description() const override { return "Advanced 3D enemy AI with patrol, chase, attack patterns, and boss mechanics"; }
	String get_module_category() const override { return "Enemy Systems"; }
	Vector<String> get_files_to_create() const override {
		Vector<String> files;
		files.push_back("scripts/enemies/Enemy3D.gd");
		files.push_back("scripts/enemies/EnemyAI3D.gd");
		files.push_back("scripts/enemies/Boss3D.gd");
		files.push_back("scripts/enemies/Goomba3D.gd");
		files.push_back("scripts/enemies/Koopa3D.gd");
		files.push_back("scenes/enemies/Goomba3D.tscn");
		files.push_back("scenes/enemies/Koopa3D.tscn");
		files.push_back("scenes/enemies/Boss3D.tscn");
		return files;
	}

	bool is_global_script() const override { return false; }
};

#endif // ENEMY_3D_MODULE_H
