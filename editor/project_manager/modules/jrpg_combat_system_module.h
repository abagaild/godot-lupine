#ifndef JRPG_COMBAT_SYSTEM_MODULE_H
#define JRPG_COMBAT_SYSTEM_MODULE_H

#include "lupine_module_base.h"

class JRPGCombatSystemModule : public LupineModuleBase {
public:
	void generate_script(Ref<FileAccess> p_file) override;
	void generate_scene(Ref<FileAccess> p_file, const String &p_scene_name) override;
	
	String get_module_name() const override { return "JRPG Combat System"; }
	String get_module_description() const override { return "Complete JRPG-style turn-based combat system with party management, skills, items, and AI-controlled party members"; }
	String get_module_category() const override { return "Combat Systems"; }
	Vector<String> get_files_to_create() const override {
		Vector<String> files;
		files.push_back("globals/CombatManager.gd");
		files.push_back("scripts/combat/CombatEncounter.gd");
		files.push_back("scripts/combat/CombatAction.gd");
		files.push_back("scripts/combat/CombatAI.gd");
		files.push_back("scripts/combat/BattleUI.gd");
		files.push_back("scripts/combat/ActionMenu.gd");
		files.push_back("scripts/combat/TargetSelector.gd");
		files.push_back("scripts/combat/CombatAnimations.gd");
		files.push_back("scripts/skills/SkillDatabase.gd");
		files.push_back("scripts/skills/Skill.gd");
		files.push_back("scenes/combat/BattleScene.tscn");
		files.push_back("scenes/combat/BattleUI.tscn");
		files.push_back("scenes/combat/ActionMenu.tscn");
		files.push_back("scenes/combat/TargetSelector.tscn");
		files.push_back("data/skills/default_skills.json");
		return files;
	}

	bool is_global_script() const override { return true; }
	String get_autoload_name() const override { return "CombatManager"; }
};

#endif // JRPG_COMBAT_SYSTEM_MODULE_H
