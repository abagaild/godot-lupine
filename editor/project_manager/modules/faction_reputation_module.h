#ifndef FACTION_REPUTATION_MODULE_H
#define FACTION_REPUTATION_MODULE_H

#include "lupine_module_base.h"

class FactionReputationModule : public LupineModuleBase {
public:
	void generate_script(Ref<FileAccess> p_file) override;
	void generate_scene(Ref<FileAccess> p_file, const String &p_scene_name) override;
	
	String get_module_name() const override { return "Faction Reputation System"; }
	String get_module_description() const override { return "Complete faction reputation system with individual character relationships affecting faction standing, shop discounts, quest availability, and political consequences"; }
	String get_module_category() const override { return "Social Systems"; }
	Vector<String> get_files_to_create() const override {
		Vector<String> files;
		files.push_back("globals/FactionReputationManager.gd");
		files.push_back("scripts/faction/Faction.gd");
		files.push_back("scripts/faction/FactionMember.gd");
		files.push_back("scripts/faction/ReputationEffect.gd");
		files.push_back("scripts/faction/PoliticalEvent.gd");
		files.push_back("scripts/ui/FactionStatusUI.gd");
		files.push_back("scripts/ui/ReputationOverviewUI.gd");
		files.push_back("scripts/ui/PoliticalMapUI.gd");
		files.push_back("scenes/ui/FactionStatusUI.tscn");
		files.push_back("scenes/ui/ReputationOverviewUI.tscn");
		files.push_back("scenes/ui/PoliticalMapUI.tscn");
		files.push_back("data/factions/factions.json");
		files.push_back("data/factions/faction_members.json");
		files.push_back("data/factions/reputation_effects.json");
		files.push_back("data/factions/political_events.json");
		return files;
	}

	bool is_global_script() const override { return true; }
	String get_autoload_name() const override { return "FactionReputationManager"; }
};

#endif // FACTION_REPUTATION_MODULE_H
