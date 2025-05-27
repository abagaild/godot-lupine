#ifndef CAMERA_TOPDOWN_RPG_MODULE_H
#define CAMERA_TOPDOWN_RPG_MODULE_H

#include "lupine_module_base.h"

class CameraTopdownRPGModule : public LupineModuleBase {
public:
	void generate_script(Ref<FileAccess> p_file) override;
	void generate_scene(Ref<FileAccess> p_file, const String &p_scene_name) override;
	
	String get_module_name() const override { return "Top-down RPG Camera Controller"; }
	String get_module_description() const override { return "Top-down camera with player following, zoom controls, and smooth movement"; }
	String get_module_category() const override { return "Camera Controllers"; }
	Vector<String> get_files_to_create() const override {
		Vector<String> files;
		files.push_back("scripts/CameraTopdownRPG.gd");
		files.push_back("scenes/CameraTopdownRPG.tscn");
		return files;
	}
};

#endif // CAMERA_TOPDOWN_RPG_MODULE_H
