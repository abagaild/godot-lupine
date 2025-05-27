#ifndef PLATFORMER_ELEMENTS_MODULE_H
#define PLATFORMER_ELEMENTS_MODULE_H

#include "lupine_module_base.h"

class PlatformerElementsModule : public LupineModuleBase {
public:
	void generate_script(Ref<FileAccess> p_file) override;
	void generate_scene(Ref<FileAccess> p_file, const String &p_scene_name) override;

	String get_module_name() const override { return "Platformer Elements"; }
	String get_module_description() const override { return "Ground tiles, hazards, interactables, climbable surfaces, and platformer mechanics"; }
	String get_module_category() const override { return "Platformer Systems"; }
	Vector<String> get_files_to_create() const override {
		Vector<String> files;
		files.push_back("scripts/platformer/PlatformerTile.gd");
		files.push_back("scripts/platformer/SpikeTile.gd");
		files.push_back("scripts/platformer/BouncePlate.gd");
		files.push_back("scripts/platformer/FlameThrower.gd");
		files.push_back("scripts/platformer/ClimbableSurface.gd");
		files.push_back("scripts/platformer/Door.gd");
		files.push_back("scripts/platformer/Teleporter.gd");
		files.push_back("scripts/platformer/Switch.gd");
		files.push_back("scripts/platformer/Lever.gd");
		files.push_back("scenes/platformer/SpikeTile.tscn");
		files.push_back("scenes/platformer/BouncePlate.tscn");
		files.push_back("scenes/platformer/FlameThrower.tscn");
		files.push_back("scenes/platformer/ClimbableSurface.tscn");
		files.push_back("scenes/platformer/Door.tscn");
		files.push_back("scenes/platformer/Teleporter.tscn");
		files.push_back("scenes/platformer/Switch.tscn");
		files.push_back("scenes/platformer/Lever.tscn");
		return files;
	}

	bool is_global_script() const override { return false; }
};

#endif // PLATFORMER_ELEMENTS_MODULE_H
