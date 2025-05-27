#ifndef CAMERA_SIDESCROLLER_MODULE_H
#define CAMERA_SIDESCROLLER_MODULE_H

#include "lupine_module_base.h"

class CameraSidescrollerModule : public LupineModuleBase {
public:
	void generate_script(Ref<FileAccess> p_file) override;
	void generate_scene(Ref<FileAccess> p_file, const String &p_scene_name) override;
	
	String get_module_name() const override { return "Sidescroller Camera Controller"; }
	String get_module_description() const override { return "2D sidescroller camera with player following, zoom controls, and lookahead"; }
	String get_module_category() const override { return "Camera Controllers"; }
	Vector<String> get_files_to_create() const override {
		Vector<String> files;
		files.push_back("scripts/CameraSidescroller.gd");
		files.push_back("scenes/CameraSidescroller.tscn");
		return files;
	}
};

#endif // CAMERA_SIDESCROLLER_MODULE_H
