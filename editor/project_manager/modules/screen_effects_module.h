#ifndef SCREEN_EFFECTS_MODULE_H
#define SCREEN_EFFECTS_MODULE_H

#include "lupine_module_base.h"

class ScreenEffectsModule : public LupineModuleBase {
public:
	void generate_script(Ref<FileAccess> p_file) override;
	void generate_scene(Ref<FileAccess> p_file, const String &p_scene_name) override;

	String get_module_name() const override { return "Screen Effects"; }
	String get_module_description() const override { return "Screen effects system with damage flash, fade transitions, CRT filter, and visual effects"; }
	String get_module_category() const override { return "Visual Effects"; }
	Vector<String> get_files_to_create() const override {
		Vector<String> files;
		files.push_back("globals/ScreenEffects.gd");
		files.push_back("scripts/effects/ScreenOverlay.gd");
		files.push_back("scenes/effects/ScreenOverlay.tscn");
		return files;
	}

	bool is_global_script() const override { return true; }
	String get_autoload_name() const override { return "ScreenEffects"; }
};

#endif // SCREEN_EFFECTS_MODULE_H
