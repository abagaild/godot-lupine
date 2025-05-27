#ifndef LOCATION_MANAGER_3D_MODULE_H
#define LOCATION_MANAGER_3D_MODULE_H

#include "lupine_module_base.h"

class LocationManager3DModule : public LupineModuleBase {
public:
	void generate_script(Ref<FileAccess> p_file) override;
	void generate_scene(Ref<FileAccess> p_file, const String &p_scene_name) override;

	String get_module_name() const override { return "3D Location Manager"; }
	String get_module_description() const override { return "Advanced location and level management with seamless transitions, unique areas, and hub worlds"; }
	String get_module_category() const override { return "World Systems"; }
	Vector<String> get_files_to_create() const override {
		Vector<String> files;
		files.push_back("globals/LocationManager3D.gd");
		files.push_back("scripts/world/Location3D.gd");
		files.push_back("scripts/world/LevelTransition3D.gd");
		files.push_back("scripts/world/SpawnPoint3D.gd");
		files.push_back("scenes/world/LevelTransition3D.tscn");
		files.push_back("scenes/world/SpawnPoint3D.tscn");
		return files;
	}

	bool is_global_script() const override { return true; }
	String get_autoload_name() const override { return "LocationManager3D"; }
};

#endif // LOCATION_MANAGER_3D_MODULE_H
