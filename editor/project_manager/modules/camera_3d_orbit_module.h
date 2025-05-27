#ifndef CAMERA_3D_ORBIT_MODULE_H
#define CAMERA_3D_ORBIT_MODULE_H

#include "lupine_module_base.h"

class Camera3DOrbitModule : public LupineModuleBase {
public:
	void generate_script(Ref<FileAccess> p_file) override;
	void generate_scene(Ref<FileAccess> p_file, const String &p_scene_name) override;
	
	String get_module_name() const override { return "3D Third-Person Orbit Camera"; }
	String get_module_description() const override { return "3D third-person camera with orbit controls, zoom, and smooth following"; }
	String get_module_category() const override { return "Camera Controllers"; }
	Vector<String> get_files_to_create() const override {
		Vector<String> files;
		files.push_back("scripts/Camera3DOrbit.gd");
		files.push_back("scenes/Camera3DOrbit.tscn");
		return files;
	}
};

#endif // CAMERA_3D_ORBIT_MODULE_H
