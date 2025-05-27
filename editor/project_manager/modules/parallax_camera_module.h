#ifndef PARALLAX_CAMERA_MODULE_H
#define PARALLAX_CAMERA_MODULE_H

#include "lupine_module_base.h"

class ParallaxCameraModule : public LupineModuleBase {
public:
	void generate_script(Ref<FileAccess> p_file) override;
	void generate_scene(Ref<FileAccess> p_file, const String &p_scene_name) override;

	String get_module_name() const override { return "Parallax Sidescroller Camera"; }
	String get_module_description() const override { return "Enhanced sidescroller camera with parallax background layers and smooth following"; }
	String get_module_category() const override { return "Camera Controllers"; }
	Vector<String> get_files_to_create() const override {
		Vector<String> files;
		files.push_back("scripts/camera/ParallaxCamera.gd");
		files.push_back("scripts/camera/ParallaxLayer.gd");
		files.push_back("scenes/camera/ParallaxCamera.tscn");
		return files;
	}

	bool is_global_script() const override { return false; }
};

#endif // PARALLAX_CAMERA_MODULE_H
