#ifndef JRPG_PARTY_SYSTEM_3D_MODULE_H
#define JRPG_PARTY_SYSTEM_3D_MODULE_H

#include "lupine_module_base.h"

class JRPGPartySystem3DModule : public LupineModuleBase {
public:
	void generate_script(Ref<FileAccess> p_file) override;
	void generate_scene(Ref<FileAccess> p_file, const String &p_scene_name) override;
	
	String get_module_name() const override { return "JRPG Party System 3D"; }
	String get_module_description() const override { return "Complete JRPG party management system for 3D games with followers, party member stats, equipment, and 3D overworld movement"; }
	String get_module_category() const override { return "Party Systems"; }
	Vector<String> get_files_to_create() const override {
		Vector<String> files;
		files.push_back("scripts/party/PartyFollower3D.gd");
		files.push_back("scripts/party/PartyFormation3D.gd");
		files.push_back("scripts/party/PartyManager3D.gd");
		files.push_back("scenes/party/PartyFollower3D.tscn");
		return files;
	}

	bool is_global_script() const override { return false; }
};

#endif // JRPG_PARTY_SYSTEM_3D_MODULE_H
