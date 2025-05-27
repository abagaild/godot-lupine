#include "lupine_module_manager.h"
#include "lupine_module_base.h"
#include "player_stats_module.h"
#include "camera_topdown_rpg_module.h"
#include "camera_sidescroller_module.h"
#include "camera_3d_orbit_module.h"
#include "camera_3d_fps_module.h"
#include "visual_novel_script_parser_module.h"
#include "character_portrait_system_module.h"
#include "background_manager_module.h"
#include "audio_manager_module.h"
#include "save_load_system_module.h"
#include "choice_system_module.h"
#include "visual_novel_main_scene_module.h"

LupineModuleManager::LupineModuleManager() {
	initialize_modules();
}

LupineModuleManager::~LupineModuleManager() {
	// Clean up module generators
	for (auto &pair : module_generators) {
		delete pair.value;
	}
	module_generators.clear();
}

void LupineModuleManager::initialize_modules() {
	available_modules.clear();

	// Register all module types
	_register_core_systems();
	_register_player_controllers();
	_register_camera_controllers();
	_register_visual_novel_modules();
}

void LupineModuleManager::_register_core_systems() {
	// Player Stats System
	PlayerStatsModule* stats_module = new PlayerStatsModule();
	register_module("player_stats", stats_module);

	ProjectModule stats_mod;
	stats_mod.id = "player_stats";
	stats_mod.name = stats_module->get_module_name();
	stats_mod.description = stats_module->get_module_description();
	stats_mod.category = stats_module->get_module_category();
	stats_mod.files_to_create = stats_module->get_files_to_create();
	available_modules.push_back(stats_mod);
}

void LupineModuleManager::_register_player_controllers() {
	// 2D Top-down 4-directional
	ProjectModule module_2d_topdown;
	module_2d_topdown.id = "player_controller_2d_topdown";
	module_2d_topdown.name = "2D Top-down Player Controller";
	module_2d_topdown.description = "4-directional movement controller with health, stamina, attack, dash, and interaction";
	module_2d_topdown.category = "Player Controllers";
	module_2d_topdown.files_to_create.push_back("scripts/PlayerController2DTopdown.gd");
	module_2d_topdown.files_to_create.push_back("scripts/PlayerStats.gd");
	module_2d_topdown.files_to_create.push_back("scenes/Player2DTopdown.tscn");
	available_modules.push_back(module_2d_topdown);

	// 2D Top-down 8-directional
	ProjectModule module_2d_topdown_8dir;
	module_2d_topdown_8dir.id = "player_controller_2d_topdown_8dir";
	module_2d_topdown_8dir.name = "2D Top-down Player Controller (8-Direction)";
	module_2d_topdown_8dir.description = "8-directional movement controller with health, stamina, attack, dash, and interaction";
	module_2d_topdown_8dir.category = "Player Controllers";
	module_2d_topdown_8dir.files_to_create.push_back("scripts/PlayerController2DTopdown8Dir.gd");
	module_2d_topdown_8dir.files_to_create.push_back("scripts/PlayerStats.gd");
	module_2d_topdown_8dir.files_to_create.push_back("scenes/Player2DTopdown8Dir.tscn");
	available_modules.push_back(module_2d_topdown_8dir);

	// 2D Platformer
	ProjectModule module_2d_platformer;
	module_2d_platformer.id = "player_controller_2d_platformer";
	module_2d_platformer.name = "2D Platformer Player Controller";
	module_2d_platformer.description = "Side-scrolling platformer controller with jumping, health, stamina, attack, dash, and interaction";
	module_2d_platformer.category = "Player Controllers";
	module_2d_platformer.files_to_create.push_back("scripts/PlayerController2DPlatformer.gd");
	module_2d_platformer.files_to_create.push_back("scripts/PlayerStats.gd");
	module_2d_platformer.files_to_create.push_back("scenes/Player2DPlatformer.tscn");
	available_modules.push_back(module_2d_platformer);

	// 3D Third-person
	ProjectModule module_3d_third_person;
	module_3d_third_person.id = "player_controller_3d_third_person";
	module_3d_third_person.name = "3D Third-Person Player Controller";
	module_3d_third_person.description = "Third-person 3D movement controller with camera follow, health, stamina, and combat";
	module_3d_third_person.category = "Player Controllers";
	module_3d_third_person.files_to_create.push_back("scripts/PlayerController3DThirdPerson.gd");
	module_3d_third_person.files_to_create.push_back("scripts/PlayerStats.gd");
	module_3d_third_person.files_to_create.push_back("scenes/Player3DThirdPerson.tscn");
	available_modules.push_back(module_3d_third_person);

	// 3D First-person
	ProjectModule module_3d_first_person;
	module_3d_first_person.id = "player_controller_3d_first_person";
	module_3d_first_person.name = "3D First-Person Player Controller";
	module_3d_first_person.description = "First-person 3D movement controller with mouse look, health, stamina, and combat";
	module_3d_first_person.category = "Player Controllers";
	module_3d_first_person.files_to_create.push_back("scripts/PlayerController3DFirstPerson.gd");
	module_3d_first_person.files_to_create.push_back("scripts/PlayerStats.gd");
	module_3d_first_person.files_to_create.push_back("scenes/Player3DFirstPerson.tscn");
	available_modules.push_back(module_3d_first_person);
}

void LupineModuleManager::_register_camera_controllers() {
	// Top-down RPG Camera
	CameraTopdownRPGModule* topdown_camera = new CameraTopdownRPGModule();
	register_module("camera_topdown_rpg", topdown_camera);

	ProjectModule topdown_mod;
	topdown_mod.id = "camera_topdown_rpg";
	topdown_mod.name = topdown_camera->get_module_name();
	topdown_mod.description = topdown_camera->get_module_description();
	topdown_mod.category = topdown_camera->get_module_category();
	topdown_mod.files_to_create = topdown_camera->get_files_to_create();
	available_modules.push_back(topdown_mod);

	// Sidescroller Camera
	CameraSidescrollerModule* sidescroller_camera = new CameraSidescrollerModule();
	register_module("camera_sidescroller", sidescroller_camera);

	ProjectModule sidescroller_mod;
	sidescroller_mod.id = "camera_sidescroller";
	sidescroller_mod.name = sidescroller_camera->get_module_name();
	sidescroller_mod.description = sidescroller_camera->get_module_description();
	sidescroller_mod.category = sidescroller_camera->get_module_category();
	sidescroller_mod.files_to_create = sidescroller_camera->get_files_to_create();
	available_modules.push_back(sidescroller_mod);

	// 3D Orbit Camera
	Camera3DOrbitModule* orbit_camera = new Camera3DOrbitModule();
	register_module("camera_3d_orbit", orbit_camera);

	ProjectModule orbit_mod;
	orbit_mod.id = "camera_3d_orbit";
	orbit_mod.name = orbit_camera->get_module_name();
	orbit_mod.description = orbit_camera->get_module_description();
	orbit_mod.category = orbit_camera->get_module_category();
	orbit_mod.files_to_create = orbit_camera->get_files_to_create();
	available_modules.push_back(orbit_mod);

	// 3D FPS Camera
	Camera3DFPSModule* fps_camera = new Camera3DFPSModule();
	register_module("camera_3d_fps", fps_camera);

	ProjectModule fps_mod;
	fps_mod.id = "camera_3d_fps";
	fps_mod.name = fps_camera->get_module_name();
	fps_mod.description = fps_camera->get_module_description();
	fps_mod.category = fps_camera->get_module_category();
	fps_mod.files_to_create = fps_camera->get_files_to_create();
	available_modules.push_back(fps_mod);
}

void LupineModuleManager::_register_visual_novel_modules() {
	// Visual Novel Script Parser
	VisualNovelScriptParserModule* script_parser = new VisualNovelScriptParserModule();
	register_module("visual_novel_script_parser", script_parser);

	ProjectModule script_parser_mod;
	script_parser_mod.id = "visual_novel_script_parser";
	script_parser_mod.name = script_parser->get_module_name();
	script_parser_mod.description = script_parser->get_module_description();
	script_parser_mod.category = script_parser->get_module_category();
	script_parser_mod.files_to_create = script_parser->get_files_to_create();
	available_modules.push_back(script_parser_mod);

	// Character Portrait System
	CharacterPortraitSystemModule* portrait_system = new CharacterPortraitSystemModule();
	register_module("character_portrait_system", portrait_system);

	ProjectModule portrait_mod;
	portrait_mod.id = "character_portrait_system";
	portrait_mod.name = portrait_system->get_module_name();
	portrait_mod.description = portrait_system->get_module_description();
	portrait_mod.category = portrait_system->get_module_category();
	portrait_mod.files_to_create = portrait_system->get_files_to_create();
	available_modules.push_back(portrait_mod);

	// Background Manager
	BackgroundManagerModule* background_manager = new BackgroundManagerModule();
	register_module("background_manager", background_manager);

	ProjectModule background_mod;
	background_mod.id = "background_manager";
	background_mod.name = background_manager->get_module_name();
	background_mod.description = background_manager->get_module_description();
	background_mod.category = background_manager->get_module_category();
	background_mod.files_to_create = background_manager->get_files_to_create();
	available_modules.push_back(background_mod);

	// Audio Manager
	AudioManagerModule* audio_manager = new AudioManagerModule();
	register_module("audio_manager", audio_manager);

	ProjectModule audio_mod;
	audio_mod.id = "audio_manager";
	audio_mod.name = audio_manager->get_module_name();
	audio_mod.description = audio_manager->get_module_description();
	audio_mod.category = audio_manager->get_module_category();
	audio_mod.files_to_create = audio_manager->get_files_to_create();
	available_modules.push_back(audio_mod);

	// Save/Load System
	SaveLoadSystemModule* save_load_system = new SaveLoadSystemModule();
	register_module("save_load_system", save_load_system);

	ProjectModule save_load_mod;
	save_load_mod.id = "save_load_system";
	save_load_mod.name = save_load_system->get_module_name();
	save_load_mod.description = save_load_system->get_module_description();
	save_load_mod.category = save_load_system->get_module_category();
	save_load_mod.files_to_create = save_load_system->get_files_to_create();
	available_modules.push_back(save_load_mod);

	// Choice System
	ChoiceSystemModule* choice_system = new ChoiceSystemModule();
	register_module("choice_system", choice_system);

	ProjectModule choice_mod;
	choice_mod.id = "choice_system";
	choice_mod.name = choice_system->get_module_name();
	choice_mod.description = choice_system->get_module_description();
	choice_mod.category = choice_system->get_module_category();
	choice_mod.files_to_create = choice_system->get_files_to_create();
	available_modules.push_back(choice_mod);

	// Visual Novel Main Scene
	VisualNovelMainSceneModule* main_scene = new VisualNovelMainSceneModule();
	register_module("visual_novel_main_scene", main_scene);

	ProjectModule main_scene_mod;
	main_scene_mod.id = "visual_novel_main_scene";
	main_scene_mod.name = main_scene->get_module_name();
	main_scene_mod.description = main_scene->get_module_description();
	main_scene_mod.category = main_scene->get_module_category();
	main_scene_mod.files_to_create = main_scene->get_files_to_create();
	available_modules.push_back(main_scene_mod);
}

void LupineModuleManager::register_module(const String &p_id, LupineModuleBase* p_generator) {
	module_generators[p_id] = p_generator;
}

void LupineModuleManager::create_module_file(const String &p_file_path, const String &p_module_id, const String &p_relative_path) {
	Ref<FileAccess> file = FileAccess::open(p_file_path, FileAccess::WRITE);
	if (!file.is_valid()) {
		ERR_PRINT("Failed to create file: " + p_file_path);
		return;
	}

	String filename = p_relative_path.get_file();

	// Check if we have a specialized generator for this module
	if (module_generators.has(p_module_id)) {
		LupineModuleBase* generator = module_generators[p_module_id];
		generator->generate_file(file, p_relative_path);
	} else {
		// Handle legacy player controllers and other modules
		if (p_relative_path.ends_with(".gd")) {
			_generate_legacy_script(file, p_module_id, filename);
		} else if (p_relative_path.ends_with(".tscn")) {
			_generate_legacy_scene(file, p_module_id, filename);
		}
	}

	file->close();
}

void LupineModuleManager::register_autoloads(const String &p_project_path, const Vector<String> &p_selected_modules) {
	// Read existing project.godot file
	String project_file_path = p_project_path.path_join("project.godot");
	Ref<ConfigFile> config = memnew(ConfigFile);

	Error err = config->load(project_file_path);
	if (err != OK) {
		ERR_PRINT("Failed to load project.godot file");
		return;
	}

	// Register autoloads for selected modules
	for (const String &module_id : p_selected_modules) {
		if (module_generators.has(module_id)) {
			LupineModuleBase* generator = module_generators[module_id];
			if (generator->is_global_script()) {
				String autoload_name = generator->get_autoload_name();
				Vector<String> files = generator->get_files_to_create();

				// Find the global script file
				for (const String &file : files) {
					if (file.begins_with("globals/") && file.ends_with(".gd")) {
						String script_path = "res://" + file;
						config->set_value("autoload", autoload_name, script_path);
						break;
					}
				}
			}
		}
	}

	// Save the updated project.godot file
	config->save(project_file_path);
}

void LupineModuleManager::create_main_scene(const String &p_project_path, const Vector<String> &p_selected_modules) {
	String main_scene_path = p_project_path.path_join("scenes/Main.tscn");
	Ref<FileAccess> file = FileAccess::open(main_scene_path, FileAccess::WRITE);
	if (!file.is_valid()) {
		ERR_PRINT("Failed to create main scene file: " + main_scene_path);
		return;
	}

	// Determine scene type based on selected modules
	bool is_3d = false;
	String player_scene = "";
	String camera_scene = "";

	// Check for 3D modules
	for (const String &module_id : p_selected_modules) {
		if (module_id.contains("3d")) {
			is_3d = true;
			break;
		}
	}

	// Find player and camera scenes
	for (const String &module_id : p_selected_modules) {
		if (module_id.begins_with("player_controller_")) {
			if (module_id.contains("2d_topdown")) {
				player_scene = "res://scenes/Player2DTopdown.tscn";
			} else if (module_id.contains("2d_topdown_8dir")) {
				player_scene = "res://scenes/Player2DTopdown8Dir.tscn";
			} else if (module_id.contains("2d_platformer")) {
				player_scene = "res://scenes/Player2DPlatformer.tscn";
			} else if (module_id.contains("3d_third_person")) {
				player_scene = "res://scenes/Player3DThirdPerson.tscn";
			} else if (module_id.contains("3d_first_person")) {
				player_scene = "res://scenes/Player3DFirstPerson.tscn";
			}
		} else if (module_id.begins_with("camera_")) {
			if (module_id.contains("topdown_rpg")) {
				camera_scene = "res://scenes/CameraTopdownRPG.tscn";
			} else if (module_id.contains("sidescroller")) {
				camera_scene = "res://scenes/CameraSidescroller.tscn";
			} else if (module_id.contains("3d_orbit")) {
				camera_scene = "res://scenes/Camera3DOrbit.tscn";
			} else if (module_id.contains("3d_fps")) {
				camera_scene = "res://scenes/Camera3DFPS.tscn";
			}
		}
	}

	// Generate main scene based on type
	if (is_3d) {
		_create_3d_main_scene(file, player_scene, camera_scene);
	} else {
		_create_2d_main_scene(file, player_scene, camera_scene);
	}

	file->close();
}

void LupineModuleManager::_generate_legacy_script(Ref<FileAccess> p_file, const String &p_module_id, const String &p_filename) {
	// Handle legacy player controller scripts
	if (p_module_id == "player_controller_2d_topdown") {
		_create_2d_topdown_controller(p_file);
	} else if (p_module_id == "player_controller_2d_topdown_8dir") {
		_create_2d_topdown_8dir_controller(p_file);
	} else if (p_module_id == "player_controller_2d_platformer") {
		_create_2d_platformer_controller(p_file);
	} else if (p_module_id == "player_controller_3d_third_person") {
		_create_3d_third_person_controller(p_file);
	} else if (p_module_id == "player_controller_3d_first_person") {
		_create_3d_first_person_controller(p_file);
	} else if (p_filename == "PlayerStats.gd") {
		// Use the PlayerStats module
		if (module_generators.has("player_stats")) {
			module_generators["player_stats"]->generate_script(p_file);
		}
	} else {
		// Default script template
		p_file->store_line("# Generated by Lupine Engine");
		p_file->store_line("# Module: " + p_module_id);
		p_file->store_line("");
		p_file->store_line("extends Node");
		p_file->store_line("");
		p_file->store_line("func _ready():");
		p_file->store_line("\tpass");
	}
}

void LupineModuleManager::_generate_legacy_scene(Ref<FileAccess> p_file, const String &p_module_id, const String &p_filename) {
	// Handle legacy player controller scenes
	if (p_module_id.begins_with("player_controller_2d")) {
		_create_2d_player_scene(p_file, p_filename);
	} else if (p_module_id.begins_with("player_controller_3d")) {
		_create_3d_player_scene(p_file, p_filename);
	} else {
		// Default scene template
		p_file->store_line("[gd_scene format=3]");
		p_file->store_line("");
		p_file->store_line("[node name=\"" + p_filename + "\" type=\"Node\"]");
	}
}
