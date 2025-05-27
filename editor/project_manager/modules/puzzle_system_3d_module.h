#ifndef PUZZLE_SYSTEM_3D_MODULE_H
#define PUZZLE_SYSTEM_3D_MODULE_H

#include "lupine_module_base.h"

class PuzzleSystem3DModule : public LupineModuleBase {
public:
	void generate_script(Ref<FileAccess> p_file) override;
	void generate_scene(Ref<FileAccess> p_file, const String &p_scene_name) override;

	String get_module_name() const override { return "3D Puzzle System"; }
	String get_module_description() const override { return "Complex 3D puzzle mechanics with switches, pressure plates, moving platforms, and interconnected systems"; }
	String get_module_category() const override { return "Puzzle Systems"; }
	Vector<String> get_files_to_create() const override {
		Vector<String> files;
		files.push_back("scripts/puzzles/PuzzleBase3D.gd");
		files.push_back("scripts/puzzles/SwitchPuzzle3D.gd");
		files.push_back("scripts/puzzles/PressurePlate3D.gd");
		files.push_back("scripts/puzzles/MovingPlatform3D.gd");
		files.push_back("scripts/puzzles/RotatingPlatform3D.gd");
		files.push_back("scenes/puzzles/SwitchPuzzle3D.tscn");
		files.push_back("scenes/puzzles/PressurePlate3D.tscn");
		files.push_back("scenes/puzzles/MovingPlatform3D.tscn");
		files.push_back("scenes/puzzles/RotatingPlatform3D.tscn");
		return files;
	}

	bool is_global_script() const override { return false; }
};

#endif // PUZZLE_SYSTEM_3D_MODULE_H
