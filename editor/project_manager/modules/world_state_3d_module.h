#ifndef WORLD_STATE_3D_MODULE_H
#define WORLD_STATE_3D_MODULE_H

#include "lupine_module_base.h"

class WorldState3DModule : public LupineModuleBase {
public:
	void generate_script(Ref<FileAccess> p_file) override;
	void generate_scene(Ref<FileAccess> p_file, const String &p_scene_name) override;

	String get_module_name() const override { return "3D World State System"; }
	String get_module_description() const override { return "Advanced world state management with persistent variables, triggers, and complex puzzle mechanics"; }
	String get_module_category() const override { return "World Systems"; }
	Vector<String> get_files_to_create() const override {
		Vector<String> files;
		files.push_back("globals/WorldState3D.gd");
		files.push_back("scripts/world/WorldTrigger3D.gd");
		files.push_back("scripts/world/PuzzleManager3D.gd");
		files.push_back("scripts/world/LocationManager3D.gd");
		files.push_back("scripts/puzzles/PuzzleBase3D.gd");
		files.push_back("scripts/puzzles/SwitchPuzzle3D.gd");
		files.push_back("scripts/puzzles/MovingPlatform3D.gd");
		files.push_back("scripts/puzzles/PressurePlate3D.gd");
		files.push_back("scenes/world/WorldTrigger3D.tscn");
		files.push_back("scenes/puzzles/SwitchPuzzle3D.tscn");
		files.push_back("scenes/puzzles/MovingPlatform3D.tscn");
		files.push_back("scenes/puzzles/PressurePlate3D.tscn");
		return files;
	}

	bool is_global_script() const override { return true; }
	String get_autoload_name() const override { return "WorldState3D"; }
};

#endif // WORLD_STATE_3D_MODULE_H
