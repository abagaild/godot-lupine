#ifndef TOPDOWN_COMBAT_MODULE_H
#define TOPDOWN_COMBAT_MODULE_H

#include "lupine_module_base.h"

class TopdownCombatModule : public LupineModuleBase {
public:
	void generate_script(Ref<FileAccess> p_file) override;
	void generate_scene(Ref<FileAccess> p_file, const String &p_scene_name) override;

	String get_module_name() const override { return "Top-down Combat System"; }
	String get_module_description() const override { return "Enhanced combat system with projectiles, area attacks, status effects, and visual feedback"; }
	String get_module_category() const override { return "Combat"; }
	Vector<String> get_files_to_create() const override {
		Vector<String> files;
		files.push_back("scripts/combat/Projectile.gd");
		files.push_back("scripts/combat/AreaAttack.gd");
		files.push_back("scripts/combat/StatusEffect.gd");
		files.push_back("scripts/combat/TopdownCombatManager.gd");
		files.push_back("scripts/combat/DamageNumber.gd");
		files.push_back("scripts/combat/HitEffect.gd");
		files.push_back("scenes/combat/Projectile.tscn");
		files.push_back("scenes/combat/AreaAttack.tscn");
		files.push_back("scenes/combat/DamageNumber.tscn");
		files.push_back("scenes/combat/HitEffect.tscn");
		return files;
	}

	bool is_global_script() const override { return true; }
	String get_autoload_name() const override { return "TopdownCombatManager"; }
};

#endif // TOPDOWN_COMBAT_MODULE_H
