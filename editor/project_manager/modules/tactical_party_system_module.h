#ifndef TACTICAL_PARTY_SYSTEM_MODULE_H
#define TACTICAL_PARTY_SYSTEM_MODULE_H

#include "lupine_module_base.h"

class TacticalPartySystemModule : public LupineModuleBase {
public:
	void generate_script(Ref<FileAccess> p_file) override;
	void generate_scene(Ref<FileAccess> p_file, const String &p_scene_name) override;
	
	String get_module_name() const override { return "Tactical Party System"; }
	String get_module_description() const override { return "Complete tactical RPG party management with classes, recruitment, dismissal, and commander-style gameplay for Fire Emblem-style games"; }
	String get_module_category() const override { return "Tactical Systems"; }
	Vector<String> get_files_to_create() const override {
		Vector<String> files;
		files.push_back("globals/TacticalPartyManager.gd");
		files.push_back("scripts/tactical/TacticalUnit.gd");
		files.push_back("scripts/tactical/UnitClass.gd");
		files.push_back("scripts/tactical/ClassSystem.gd");
		files.push_back("scripts/tactical/RecruitmentSystem.gd");
		files.push_back("scripts/tactical/CommanderController.gd");
		files.push_back("scripts/ui/TacticalPartyUI.gd");
		files.push_back("scripts/ui/RecruitmentUI.gd");
		files.push_back("scripts/ui/ClassChangeUI.gd");
		files.push_back("scenes/ui/TacticalPartyUI.tscn");
		files.push_back("scenes/ui/RecruitmentUI.tscn");
		files.push_back("scenes/ui/ClassChangeUI.tscn");
		files.push_back("data/classes/default_classes.json");
		files.push_back("data/units/recruitable_units.json");
		return files;
	}

	bool is_global_script() const override { return true; }
	String get_autoload_name() const override { return "TacticalPartyManager"; }
};

#endif // TACTICAL_PARTY_SYSTEM_MODULE_H
