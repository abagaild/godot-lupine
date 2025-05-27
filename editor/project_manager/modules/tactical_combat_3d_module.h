#ifndef TACTICAL_COMBAT_3D_MODULE_H
#define TACTICAL_COMBAT_3D_MODULE_H

#include "lupine_module_base.h"

class TacticalCombat3DModule : public LupineModuleBase {
public:
	void generate_script(Ref<FileAccess> p_file) override;
	void generate_scene(Ref<FileAccess> p_file, const String &p_scene_name) override;
	
	String get_module_name() const override { return "Tactical Combat 3D"; }
	String get_module_description() const override { return "Complete 3D tactical combat system with grid-based movement, height advantages, 3D positioning, and advanced tactical mechanics"; }
	String get_module_category() const override { return "Combat Systems"; }
	Vector<String> get_files_to_create() const override {
		Vector<String> files;
		files.push_back("scripts/tactical/TacticalGrid3D.gd");
		files.push_back("scripts/tactical/TacticalUnit3D.gd");
		files.push_back("scripts/tactical/HeightCalculator.gd");
		files.push_back("scripts/tactical/LineOfSight3D.gd");
		files.push_back("scripts/tactical/TacticalCamera3D.gd");
		files.push_back("scenes/tactical/TacticalBattlefield3D.tscn");
		files.push_back("scenes/tactical/TacticalUnit3D.tscn");
		files.push_back("scenes/tactical/TacticalCamera3D.tscn");
		return files;
	}

	bool is_global_script() const override { return false; }
};

#endif // TACTICAL_COMBAT_3D_MODULE_H
