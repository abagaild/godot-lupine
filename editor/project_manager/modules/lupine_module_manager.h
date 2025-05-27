#ifndef LUPINE_MODULE_MANAGER_H
#define LUPINE_MODULE_MANAGER_H

#include "core/object/ref_counted.h"
#include "core/string/ustring.h"
#include "core/variant/variant.h"
#include "core/io/file_access.h"
#include "core/io/config_file.h"
#include "../lupine_project_types.h"

// Forward declarations
class LupineModuleBase;

class LupineModuleManager : public RefCounted {
	GDCLASS(LupineModuleManager, RefCounted);

private:
	Vector<ProjectModule> available_modules;
	HashMap<String, LupineModuleBase*> module_generators;

public:
	LupineModuleManager();
	~LupineModuleManager();

	// Initialize all modules
	void initialize_modules();

	// Get available modules
	Vector<ProjectModule> get_available_modules() const { return available_modules; }

	// Create module files
	void create_module_file(const String &p_file_path, const String &p_module_id, const String &p_relative_path);

	// Module registration
	void register_module(const String &p_id, LupineModuleBase* p_generator);

	// Autoload and main scene generation
	void register_autoloads(const String &p_project_path, const Vector<String> &p_selected_modules);
	void create_main_scene(const String &p_project_path, const Vector<String> &p_selected_modules);

private:
	void _register_player_controllers();
	void _register_camera_controllers();
	void _register_core_systems();
	void _register_rpg_systems();
	void _register_3d_platformer_modules();
	void _register_visual_novel_modules();
	void _register_ui_systems();
	void _register_monster_capture_modules();

	// Legacy module generators
	void _generate_legacy_script(Ref<FileAccess> p_file, const String &p_module_id, const String &p_filename);
	void _generate_legacy_scene(Ref<FileAccess> p_file, const String &p_module_id, const String &p_filename);

	// Legacy player controller generators
	void _create_2d_topdown_controller(Ref<FileAccess> p_file);
	void _create_2d_topdown_8dir_controller(Ref<FileAccess> p_file);
	void _create_2d_platformer_controller(Ref<FileAccess> p_file);
	void _create_3d_third_person_controller(Ref<FileAccess> p_file);
	void _create_3d_first_person_controller(Ref<FileAccess> p_file);
	void _create_2d_player_scene(Ref<FileAccess> p_file, const String &p_scene_name);
	void _create_3d_player_scene(Ref<FileAccess> p_file, const String &p_scene_name);

	// Main scene generators
	void _create_2d_main_scene(Ref<FileAccess> p_file, const String &p_player_scene, const String &p_camera_scene);
	void _create_3d_main_scene(Ref<FileAccess> p_file, const String &p_player_scene, const String &p_camera_scene);
};

#endif // LUPINE_MODULE_MANAGER_H
