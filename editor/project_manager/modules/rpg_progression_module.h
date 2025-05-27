#ifndef RPG_PROGRESSION_MODULE_H
#define RPG_PROGRESSION_MODULE_H

#include "lupine_module_base.h"

class RPGProgressionModule : public LupineModuleBase {
public:
	void generate_script(Ref<FileAccess> p_file) override;
	void generate_scene(Ref<FileAccess> p_file, const String &p_scene_name) override;
	
	String get_module_name() const override { return "RPG Progression System"; }
	String get_module_description() const override { return "Character progression with leveling, skill trees, equipment upgrades, and stat bonuses"; }
	String get_module_category() const override { return "RPG Systems"; }
	Vector<String> get_files_to_create() const override {
		Vector<String> files;
		files.push_back("globals/ProgressionSystem.gd");
		files.push_back("scripts/progression/SkillTree.gd");
		files.push_back("scripts/progression/Skill.gd");
		files.push_back("scripts/progression/EquipmentUpgrade.gd");
		files.push_back("scripts/progression/StatBonus.gd");
		files.push_back("scripts/ui/SkillTreeUI.gd");
		files.push_back("scripts/ui/CharacterSheetUI.gd");
		files.push_back("scenes/ui/SkillTreeUI.tscn");
		files.push_back("scenes/ui/CharacterSheetUI.tscn");
		return files;
	}

	bool is_global_script() const override { return true; }
	String get_autoload_name() const override { return "ProgressionSystem"; }
};

#endif // RPG_PROGRESSION_MODULE_H
