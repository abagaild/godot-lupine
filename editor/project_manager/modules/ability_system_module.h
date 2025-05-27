#ifndef ABILITY_SYSTEM_MODULE_H
#define ABILITY_SYSTEM_MODULE_H

#include "lupine_module_base.h"

class AbilitySystemModule : public LupineModuleBase {
public:
	void generate_script(Ref<FileAccess> p_file) override;
	void generate_scene(Ref<FileAccess> p_file, const String &p_scene_name) override;

	String get_module_name() const override { return "Ability System"; }
	String get_module_description() const override { return "RPG ability system with spells, skills, cooldowns, and upgradeable abilities"; }
	String get_module_category() const override { return "RPG Systems"; }
	Vector<String> get_files_to_create() const override {
		Vector<String> files;
		files.push_back("globals/AbilitySystem.gd");
		files.push_back("scripts/abilities/Ability.gd");
		files.push_back("scripts/abilities/ProjectileAbility.gd");
		files.push_back("scripts/abilities/MeleeAbility.gd");
		files.push_back("scripts/abilities/BuffAbility.gd");
		files.push_back("scripts/projectiles/Projectile.gd");
		files.push_back("scenes/abilities/Fireball.tscn");
		files.push_back("scenes/abilities/Slash.tscn");
		files.push_back("scenes/projectiles/Projectile.tscn");
		return files;
	}

	bool is_global_script() const override { return true; }
	String get_autoload_name() const override { return "AbilitySystem"; }
};

#endif // ABILITY_SYSTEM_MODULE_H
