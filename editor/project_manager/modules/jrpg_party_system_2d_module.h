#ifndef JRPG_PARTY_SYSTEM_2D_MODULE_H
#define JRPG_PARTY_SYSTEM_2D_MODULE_H

#include "lupine_module_base.h"

class JRPGPartySystem2DModule : public LupineModuleBase {
public:
	void generate_script(Ref<FileAccess> p_file) override;
	void generate_scene(Ref<FileAccess> p_file, const String &p_scene_name) override;
	
	String get_module_name() const override { return "JRPG Party System 2D"; }
	String get_module_description() const override { return "Complete JRPG party management system for 2D games with followers, party member stats, equipment, and overworld movement with 4/8-directional animations"; }
	String get_module_category() const override { return "Party Systems"; }
	Vector<String> get_files_to_create() const override {
		Vector<String> files;
		files.push_back("globals/PartyManager.gd");
		files.push_back("scripts/party/PartyMember.gd");
		files.push_back("scripts/party/PartyFollower2D.gd");
		files.push_back("scripts/party/PartyStats.gd");
		files.push_back("scripts/party/PartyEquipment.gd");
		files.push_back("scripts/party/PartyFormation.gd");
		files.push_back("scripts/ui/PartyMenuUI.gd");
		files.push_back("scripts/ui/PartyMemberUI.gd");
		files.push_back("scenes/party/PartyFollower2D.tscn");
		files.push_back("scenes/ui/PartyMenuUI.tscn");
		files.push_back("scenes/ui/PartyMemberUI.tscn");
		return files;
	}

	bool is_global_script() const override { return true; }
	String get_autoload_name() const override { return "PartyManager"; }
};

#endif // JRPG_PARTY_SYSTEM_2D_MODULE_H
