#ifndef CAMERA_3D_MODULE_H
#define CAMERA_3D_MODULE_H

#include "lupine_module_base.h"

class Camera3DModule : public LupineModuleBase {
public:
	void generate_script(Ref<FileAccess> p_file) override;
	void generate_scene(Ref<FileAccess> p_file, const String &p_scene_name) override;

	String get_module_name() const override { return "3D Camera System"; }
	String get_module_description() const override { return "Advanced 3D camera with Mario 64-style controls, automatic transitions, and cinematic modes"; }
	String get_module_category() const override { return "Camera Controllers"; }
	Vector<String> get_files_to_create() const override {
		Vector<String> files;
		files.push_back("scripts/camera/Camera3DController.gd");
		files.push_back("scripts/camera/CameraState3D.gd");
		files.push_back("scripts/camera/CinematicCamera3D.gd");
		files.push_back("scenes/camera/Camera3DController.tscn");
		return files;
	}

	bool is_global_script() const override { return false; }
};

#endif // CAMERA_3D_MODULE_H
