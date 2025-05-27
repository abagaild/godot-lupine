#ifndef CAMERA_3D_FPS_MODULE_H
#define CAMERA_3D_FPS_MODULE_H

#include "lupine_module_base.h"

class Camera3DFPSModule : public LupineModuleBase {
public:
	void generate_script(Ref<FileAccess> p_file) override;
	void generate_scene(Ref<FileAccess> p_file, const String &p_scene_name) override;
	
	String get_module_name() const override { return "3D First-Person Camera"; }
	String get_module_description() const override { return "3D first-person camera with mouse look, zoom controls, and smooth movement"; }
	String get_module_category() const override { return "Camera Controllers"; }
	Vector<String> get_files_to_create() const override {
		Vector<String> files;
		files.push_back("scripts/Camera3DFPS.gd");
		files.push_back("scenes/Camera3DFPS.tscn");
		return files;
	}
};

#endif // CAMERA_3D_FPS_MODULE_H
