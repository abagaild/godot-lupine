#ifndef TACTICAL_COMBAT_2D_MODULE_H
#define TACTICAL_COMBAT_2D_MODULE_H

#include "lupine_module_base.h"

class TacticalCombat2DModule : public LupineModuleBase {
public:
	void generate_script(Ref<FileAccess> p_file) override;
	void generate_scene(Ref<FileAccess> p_file, const String &p_scene_name) override;
	
	String get_module_name() const override { return "Tactical Combat 2D"; }
	String get_module_description() const override { return "Complete 2D tactical combat system with grid-based movement, turn management, AI enemies, and Fire Emblem-style gameplay"; }
	String get_module_category() const override { return "Combat Systems"; }
	Vector<String> get_files_to_create() const override {
		Vector<String> files;
		files.push_back("globals/TacticalCombatManager.gd");
		files.push_back("scripts/tactical/TacticalGrid2D.gd");
		files.push_back("scripts/tactical/TacticalUnit2D.gd");
		files.push_back("scripts/tactical/TacticalAI.gd");
		files.push_back("scripts/tactical/EnemyArmy.gd");
		files.push_back("scripts/tactical/TacticalAction.gd");
		files.push_back("scripts/tactical/MovementCalculator.gd");
		files.push_back("scripts/tactical/AttackCalculator.gd");
		files.push_back("scripts/ui/TacticalUI.gd");
		files.push_back("scripts/ui/UnitInfoPanel.gd");
		files.push_back("scripts/ui/ActionMenu.gd");
		files.push_back("scenes/tactical/TacticalBattlefield2D.tscn");
		files.push_back("scenes/tactical/TacticalUnit2D.tscn");
		files.push_back("scenes/ui/TacticalUI.tscn");
		files.push_back("scenes/ui/UnitInfoPanel.tscn");
		files.push_back("data/enemies/enemy_armies.json");
		return files;
	}

	bool is_global_script() const override { return true; }
	String get_autoload_name() const override { return "TacticalCombatManager"; }
};

#endif // TACTICAL_COMBAT_2D_MODULE_H
