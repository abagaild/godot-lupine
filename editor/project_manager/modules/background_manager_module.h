#ifndef BACKGROUND_MANAGER_MODULE_H
#define BACKGROUND_MANAGER_MODULE_H

#include "lupine_module_base.h"

class BackgroundManagerModule : public LupineModuleBase {
public:
	void generate_script(Ref<FileAccess> p_file) override;
	void generate_scene(Ref<FileAccess> p_file, const String &p_scene_name) override;
	void generate_file(Ref<FileAccess> p_file, const String &p_relative_path) override;

	String get_module_name() const override { return "Background Manager"; }
	String get_module_description() const override { return "Background and scene management with transitions, key images, and automatic asset discovery"; }
	String get_module_category() const override { return "Visual Novel"; }
	Vector<String> get_files_to_create() const override {
		Vector<String> files;
		files.push_back("globals/BackgroundManager.gd");
		files.push_back("scenes/ui/BackgroundDisplay.tscn");
		files.push_back("scripts/ui/BackgroundDisplay.gd");
		return files;
	}

	bool is_global_script() const override { return true; }
	String get_autoload_name() const override { return "BackgroundManager"; }
};

#endif // BACKGROUND_MANAGER_MODULE_H
