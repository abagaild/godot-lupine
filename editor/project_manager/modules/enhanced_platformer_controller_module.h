#ifndef ENHANCED_PLATFORMER_CONTROLLER_MODULE_H
#define ENHANCED_PLATFORMER_CONTROLLER_MODULE_H

#include "lupine_module_base.h"

class EnhancedPlatformerControllerModule : public LupineModuleBase {
public:
	void generate_script(Ref<FileAccess> p_file) override;
	void generate_scene(Ref<FileAccess> p_file, const String &p_scene_name) override;

	String get_module_name() const override { return "Enhanced 2D Platformer Controller"; }
	String get_module_description() const override { return "Advanced 2D platformer with wall jumping, climbing, status effects, and enhanced movement"; }
	String get_module_category() const override { return "Player Controllers"; }
	Vector<String> get_files_to_create() const override {
		Vector<String> files;
		files.push_back("scripts/player/EnhancedPlatformerController.gd");
		files.push_back("scripts/player/StatusEffectManager.gd");
		files.push_back("scenes/player/EnhancedPlatformerPlayer.tscn");
		return files;
	}

	bool is_global_script() const override { return false; }
};

#endif // ENHANCED_PLATFORMER_CONTROLLER_MODULE_H
