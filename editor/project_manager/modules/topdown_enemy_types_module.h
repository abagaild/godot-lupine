#ifndef TOPDOWN_ENEMY_TYPES_MODULE_H
#define TOPDOWN_ENEMY_TYPES_MODULE_H

#include "lupine_module_base.h"

class TopdownEnemyTypesModule : public LupineModuleBase {
public:
	void generate_script(Ref<FileAccess> p_file) override;
	void generate_scene(Ref<FileAccess> p_file, const String &p_scene_name) override;
	
	String get_module_name() const override { return "Top-down Enemy Types"; }
	String get_module_description() const override { return "Specialized enemy types for top-down RPG: patrol guards, ranged attackers, chasers, and boss enemies"; }
	String get_module_category() const override { return "Enemies"; }
	Vector<String> get_files_to_create() const override {
		Vector<String> files;
		files.push_back("scripts/enemies/EnemyPatrolGuard.gd");
		files.push_back("scripts/enemies/EnemyRangedAttacker.gd");
		files.push_back("scripts/enemies/EnemyChaser.gd");
		files.push_back("scripts/enemies/EnemyBoss.gd");
		files.push_back("scripts/enemies/EnemySpawner.gd");
		files.push_back("scenes/enemies/EnemyPatrolGuard.tscn");
		files.push_back("scenes/enemies/EnemyRangedAttacker.tscn");
		files.push_back("scenes/enemies/EnemyChaser.tscn");
		files.push_back("scenes/enemies/EnemyBoss.tscn");
		files.push_back("scenes/enemies/EnemySpawner.tscn");
		return files;
	}

	bool is_global_script() const override { return false; }
};

#endif // TOPDOWN_ENEMY_TYPES_MODULE_H
