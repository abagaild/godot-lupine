#ifndef ENEMY_AI_MODULE_H
#define ENEMY_AI_MODULE_H

#include "lupine_module_base.h"

class EnemyAIModule : public LupineModuleBase {
public:
	void generate_script(Ref<FileAccess> p_file) override;
	void generate_scene(Ref<FileAccess> p_file, const String &p_scene_name) override;

	String get_module_name() const override { return "Enemy AI System"; }
	String get_module_description() const override { return "Modular enemy AI with patrol, chase, attack behaviors and easy customization for different enemy types"; }
	String get_module_category() const override { return "RPG Systems"; }
	Vector<String> get_files_to_create() const override {
		Vector<String> files;
		files.push_back("scripts/enemies/EnemyBase.gd");
		files.push_back("scripts/enemies/EnemyPatrol.gd");
		files.push_back("scripts/enemies/EnemyChaser.gd");
		files.push_back("scripts/enemies/EnemyFlying.gd");
		files.push_back("scripts/enemies/EnemyStats.gd");
		files.push_back("scenes/enemies/EnemyPatrol.tscn");
		files.push_back("scenes/enemies/EnemyChaser.tscn");
		files.push_back("scenes/enemies/EnemyFlying.tscn");
		return files;
	}

	bool is_global_script() const override { return false; }
};

#endif // ENEMY_AI_MODULE_H
