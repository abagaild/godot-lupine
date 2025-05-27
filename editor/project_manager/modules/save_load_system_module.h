#ifndef SAVE_LOAD_SYSTEM_MODULE_H
#define SAVE_LOAD_SYSTEM_MODULE_H

#include "lupine_module_base.h"

class SaveLoadSystemModule : public LupineModuleBase {
public:
	void generate_script(Ref<FileAccess> p_file) override;
	void generate_scene(Ref<FileAccess> p_file, const String &p_scene_name) override;
	void generate_file(Ref<FileAccess> p_file, const String &p_relative_path) override;

	String get_module_name() const override { return "Save/Load System"; }
	String get_module_description() const override { return "Game state persistence with multiple save slots, auto-save, and quick save/load functionality"; }
	String get_module_category() const override { return "Visual Novel"; }
	Vector<String> get_files_to_create() const override {
		Vector<String> files;
		files.push_back("globals/SaveLoadSystem.gd");
		files.push_back("scripts/ui/SaveLoadMenu.gd");
		files.push_back("scenes/ui/SaveLoadMenu.tscn");
		return files;
	}

	bool is_global_script() const override { return true; }
	String get_autoload_name() const override { return "SaveLoadSystem"; }
};

#endif // SAVE_LOAD_SYSTEM_MODULE_H
