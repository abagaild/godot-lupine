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
#include "menu_builder_module.h"
#include "hud_builder_module.h"
#include "dialogue_system_module.h"
#include "inventory_system_module.h"
#include "enemy_ai_module.h"
#include "ability_system_module.h"
#include "topdown_rpg_main_scene_module.h"
#include "topdown_enemy_types_module.h"
#include "rpg_world_elements_module.h"
#include "topdown_combat_module.h"
#include "rpg_progression_module.h"
#include "collectible_system_module.h"
#include "level_progression_module.h"
#include "platformer_elements_module.h"
#include "enhanced_platformer_controller_module.h"
#include "parallax_camera_module.h"
#include "quest_system_module.h"
#include "popup_manager_module.h"
#include "screen_effects_module.h"
#include "passive_mobs_module.h"
#include "platformer_3d_controller_module.h"
#include "camera_3d_module.h"
#include "world_state_3d_module.h"
#include "puzzle_system_3d_module.h"
#include "enemy_3d_module.h"
#include "location_manager_3d_module.h"
#include "jrpg_party_system_2d_module.h"
#include "jrpg_party_system_3d_module.h"
#include "jrpg_combat_system_module.h"
#include "tactical_party_system_module.h"
#include "tactical_combat_2d_module.h"
#include "tactical_combat_3d_module.h"
#include "relationship_system_module.h"
#include "support_conversation_module.h"
#include "social_interaction_module.h"
#include "faction_reputation_module.h"
#include "pokemon_battle_system_module.h"
#include "monster_system_module.h"
#include "monster_capture_system_module.h"

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
	_register_rpg_systems();
	_register_3d_platformer_modules();
	_register_visual_novel_modules();
	_register_ui_systems();
	_register_monster_capture_modules();
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

void LupineModuleManager::_register_rpg_systems() {
	// Dialogue System
	DialogueSystemModule* dialogue_system = new DialogueSystemModule();
	register_module("dialogue_system", dialogue_system);

	ProjectModule dialogue_mod;
	dialogue_mod.id = "dialogue_system";
	dialogue_mod.name = dialogue_system->get_module_name();
	dialogue_mod.description = dialogue_system->get_module_description();
	dialogue_mod.category = dialogue_system->get_module_category();
	dialogue_mod.files_to_create = dialogue_system->get_files_to_create();
	available_modules.push_back(dialogue_mod);

	// Inventory System
	InventorySystemModule* inventory_system = new InventorySystemModule();
	register_module("inventory_system", inventory_system);

	ProjectModule inventory_mod;
	inventory_mod.id = "inventory_system";
	inventory_mod.name = inventory_system->get_module_name();
	inventory_mod.description = inventory_system->get_module_description();
	inventory_mod.category = inventory_system->get_module_category();
	inventory_mod.files_to_create = inventory_system->get_files_to_create();
	available_modules.push_back(inventory_mod);

	// Enemy AI System
	EnemyAIModule* enemy_ai = new EnemyAIModule();
	register_module("enemy_ai", enemy_ai);

	ProjectModule enemy_mod;
	enemy_mod.id = "enemy_ai";
	enemy_mod.name = enemy_ai->get_module_name();
	enemy_mod.description = enemy_ai->get_module_description();
	enemy_mod.category = enemy_ai->get_module_category();
	enemy_mod.files_to_create = enemy_ai->get_files_to_create();
	available_modules.push_back(enemy_mod);

	// Ability System
	AbilitySystemModule* ability_system = new AbilitySystemModule();
	register_module("ability_system", ability_system);

	ProjectModule ability_mod;
	ability_mod.id = "ability_system";
	ability_mod.name = ability_system->get_module_name();
	ability_mod.description = ability_system->get_module_description();
	ability_mod.category = ability_system->get_module_category();
	ability_mod.files_to_create = ability_system->get_files_to_create();
	available_modules.push_back(ability_mod);

	// Collectible System
	CollectibleSystemModule* collectible_system = new CollectibleSystemModule();
	register_module("collectible_system", collectible_system);

	ProjectModule collectible_mod;
	collectible_mod.id = "collectible_system";
	collectible_mod.name = collectible_system->get_module_name();
	collectible_mod.description = collectible_system->get_module_description();
	collectible_mod.category = collectible_system->get_module_category();
	collectible_mod.files_to_create = collectible_system->get_files_to_create();
	available_modules.push_back(collectible_mod);

	// Level Progression System
	LevelProgressionModule* level_progression = new LevelProgressionModule();
	register_module("level_progression", level_progression);

	ProjectModule level_mod;
	level_mod.id = "level_progression";
	level_mod.name = level_progression->get_module_name();
	level_mod.description = level_progression->get_module_description();
	level_mod.category = level_progression->get_module_category();
	level_mod.files_to_create = level_progression->get_files_to_create();
	available_modules.push_back(level_mod);

	// Platformer Elements
	PlatformerElementsModule* platformer_elements = new PlatformerElementsModule();
	register_module("platformer_elements", platformer_elements);

	ProjectModule platformer_mod;
	platformer_mod.id = "platformer_elements";
	platformer_mod.name = platformer_elements->get_module_name();
	platformer_mod.description = platformer_elements->get_module_description();
	platformer_mod.category = platformer_elements->get_module_category();
	platformer_mod.files_to_create = platformer_elements->get_files_to_create();
	available_modules.push_back(platformer_mod);

	// Enhanced Platformer Controller
	EnhancedPlatformerControllerModule* enhanced_controller = new EnhancedPlatformerControllerModule();
	register_module("enhanced_platformer_controller", enhanced_controller);

	ProjectModule enhanced_mod;
	enhanced_mod.id = "enhanced_platformer_controller";
	enhanced_mod.name = enhanced_controller->get_module_name();
	enhanced_mod.description = enhanced_controller->get_module_description();
	enhanced_mod.category = enhanced_controller->get_module_category();
	enhanced_mod.files_to_create = enhanced_controller->get_files_to_create();
	available_modules.push_back(enhanced_mod);

	// Parallax Camera
	ParallaxCameraModule* parallax_camera = new ParallaxCameraModule();
	register_module("parallax_camera", parallax_camera);

	ProjectModule parallax_mod;
	parallax_mod.id = "parallax_camera";
	parallax_mod.name = parallax_camera->get_module_name();
	parallax_mod.description = parallax_camera->get_module_description();
	parallax_mod.category = parallax_camera->get_module_category();
	parallax_mod.files_to_create = parallax_camera->get_files_to_create();
	available_modules.push_back(parallax_mod);

	// Quest System
	QuestSystemModule* quest_system = new QuestSystemModule();
	register_module("quest_system", quest_system);

	ProjectModule quest_mod;
	quest_mod.id = "quest_system";
	quest_mod.name = quest_system->get_module_name();
	quest_mod.description = quest_system->get_module_description();
	quest_mod.category = quest_system->get_module_category();
	quest_mod.files_to_create = quest_system->get_files_to_create();
	available_modules.push_back(quest_mod);

	// Popup Manager
	PopupManagerModule* popup_manager = new PopupManagerModule();
	register_module("popup_manager", popup_manager);

	ProjectModule popup_mod;
	popup_mod.id = "popup_manager";
	popup_mod.name = popup_manager->get_module_name();
	popup_mod.description = popup_manager->get_module_description();
	popup_mod.category = popup_manager->get_module_category();
	popup_mod.files_to_create = popup_manager->get_files_to_create();
	available_modules.push_back(popup_mod);

	// Screen Effects
	ScreenEffectsModule* screen_effects = new ScreenEffectsModule();
	register_module("screen_effects", screen_effects);

	ProjectModule screen_mod;
	screen_mod.id = "screen_effects";
	screen_mod.name = screen_effects->get_module_name();
	screen_mod.description = screen_effects->get_module_description();
	screen_mod.category = screen_effects->get_module_category();
	screen_mod.files_to_create = screen_effects->get_files_to_create();
	available_modules.push_back(screen_mod);

	// Passive Mobs
	PassiveMobsModule* passive_mobs = new PassiveMobsModule();
	register_module("passive_mobs", passive_mobs);

	ProjectModule mobs_mod;
	mobs_mod.id = "passive_mobs";
	mobs_mod.name = passive_mobs->get_module_name();
	mobs_mod.description = passive_mobs->get_module_description();
	mobs_mod.category = passive_mobs->get_module_category();
	mobs_mod.files_to_create = passive_mobs->get_files_to_create();
	available_modules.push_back(mobs_mod);

	// Top-down RPG Main Scene
	TopdownRPGMainSceneModule* topdown_main = new TopdownRPGMainSceneModule();
	register_module("topdown_rpg_main_scene", topdown_main);

	ProjectModule topdown_main_mod;
	topdown_main_mod.id = "topdown_rpg_main_scene";
	topdown_main_mod.name = topdown_main->get_module_name();
	topdown_main_mod.description = topdown_main->get_module_description();
	topdown_main_mod.category = topdown_main->get_module_category();
	topdown_main_mod.files_to_create = topdown_main->get_files_to_create();
	available_modules.push_back(topdown_main_mod);

	// Top-down Enemy Types
	TopdownEnemyTypesModule* topdown_enemies = new TopdownEnemyTypesModule();
	register_module("topdown_enemy_types", topdown_enemies);

	ProjectModule topdown_enemies_mod;
	topdown_enemies_mod.id = "topdown_enemy_types";
	topdown_enemies_mod.name = topdown_enemies->get_module_name();
	topdown_enemies_mod.description = topdown_enemies->get_module_description();
	topdown_enemies_mod.category = topdown_enemies->get_module_category();
	topdown_enemies_mod.files_to_create = topdown_enemies->get_files_to_create();
	available_modules.push_back(topdown_enemies_mod);

	// RPG World Elements
	RPGWorldElementsModule* world_elements = new RPGWorldElementsModule();
	register_module("rpg_world_elements", world_elements);

	ProjectModule world_elements_mod;
	world_elements_mod.id = "rpg_world_elements";
	world_elements_mod.name = world_elements->get_module_name();
	world_elements_mod.description = world_elements->get_module_description();
	world_elements_mod.category = world_elements->get_module_category();
	world_elements_mod.files_to_create = world_elements->get_files_to_create();
	available_modules.push_back(world_elements_mod);

	// Top-down Combat System
	TopdownCombatModule* topdown_combat = new TopdownCombatModule();
	register_module("topdown_combat", topdown_combat);

	ProjectModule topdown_combat_mod;
	topdown_combat_mod.id = "topdown_combat";
	topdown_combat_mod.name = topdown_combat->get_module_name();
	topdown_combat_mod.description = topdown_combat->get_module_description();
	topdown_combat_mod.category = topdown_combat->get_module_category();
	topdown_combat_mod.files_to_create = topdown_combat->get_files_to_create();
	available_modules.push_back(topdown_combat_mod);

	// RPG Progression System
	RPGProgressionModule* rpg_progression = new RPGProgressionModule();
	register_module("rpg_progression", rpg_progression);

	ProjectModule rpg_progression_mod;
	rpg_progression_mod.id = "rpg_progression";
	rpg_progression_mod.name = rpg_progression->get_module_name();
	rpg_progression_mod.description = rpg_progression->get_module_description();
	rpg_progression_mod.category = rpg_progression->get_module_category();
	rpg_progression_mod.files_to_create = rpg_progression->get_files_to_create();
	available_modules.push_back(rpg_progression_mod);

	// JRPG Party System 2D
	JRPGPartySystem2DModule* jrpg_party_2d = new JRPGPartySystem2DModule();
	register_module("jrpg_party_system_2d", jrpg_party_2d);

	ProjectModule jrpg_party_2d_mod;
	jrpg_party_2d_mod.id = "jrpg_party_system_2d";
	jrpg_party_2d_mod.name = jrpg_party_2d->get_module_name();
	jrpg_party_2d_mod.description = jrpg_party_2d->get_module_description();
	jrpg_party_2d_mod.category = jrpg_party_2d->get_module_category();
	jrpg_party_2d_mod.files_to_create = jrpg_party_2d->get_files_to_create();
	available_modules.push_back(jrpg_party_2d_mod);

	// JRPG Party System 3D
	JRPGPartySystem3DModule* jrpg_party_3d = new JRPGPartySystem3DModule();
	register_module("jrpg_party_system_3d", jrpg_party_3d);

	ProjectModule jrpg_party_3d_mod;
	jrpg_party_3d_mod.id = "jrpg_party_system_3d";
	jrpg_party_3d_mod.name = jrpg_party_3d->get_module_name();
	jrpg_party_3d_mod.description = jrpg_party_3d->get_module_description();
	jrpg_party_3d_mod.category = jrpg_party_3d->get_module_category();
	jrpg_party_3d_mod.files_to_create = jrpg_party_3d->get_files_to_create();
	available_modules.push_back(jrpg_party_3d_mod);

	// JRPG Combat System
	JRPGCombatSystemModule* jrpg_combat = new JRPGCombatSystemModule();
	register_module("jrpg_combat_system", jrpg_combat);

	ProjectModule jrpg_combat_mod;
	jrpg_combat_mod.id = "jrpg_combat_system";
	jrpg_combat_mod.name = jrpg_combat->get_module_name();
	jrpg_combat_mod.description = jrpg_combat->get_module_description();
	jrpg_combat_mod.category = jrpg_combat->get_module_category();
	jrpg_combat_mod.files_to_create = jrpg_combat->get_files_to_create();
	available_modules.push_back(jrpg_combat_mod);

	// Tactical Party System
	TacticalPartySystemModule* tactical_party = new TacticalPartySystemModule();
	register_module("tactical_party_system", tactical_party);

	ProjectModule tactical_party_mod;
	tactical_party_mod.id = "tactical_party_system";
	tactical_party_mod.name = tactical_party->get_module_name();
	tactical_party_mod.description = tactical_party->get_module_description();
	tactical_party_mod.category = tactical_party->get_module_category();
	tactical_party_mod.files_to_create = tactical_party->get_files_to_create();
	available_modules.push_back(tactical_party_mod);

	// Tactical Combat 2D
	TacticalCombat2DModule* tactical_combat_2d = new TacticalCombat2DModule();
	register_module("tactical_combat_2d", tactical_combat_2d);

	ProjectModule tactical_combat_2d_mod;
	tactical_combat_2d_mod.id = "tactical_combat_2d";
	tactical_combat_2d_mod.name = tactical_combat_2d->get_module_name();
	tactical_combat_2d_mod.description = tactical_combat_2d->get_module_description();
	tactical_combat_2d_mod.category = tactical_combat_2d->get_module_category();
	tactical_combat_2d_mod.files_to_create = tactical_combat_2d->get_files_to_create();
	available_modules.push_back(tactical_combat_2d_mod);

	// Tactical Combat 3D
	TacticalCombat3DModule* tactical_combat_3d = new TacticalCombat3DModule();
	register_module("tactical_combat_3d", tactical_combat_3d);

	ProjectModule tactical_combat_3d_mod;
	tactical_combat_3d_mod.id = "tactical_combat_3d";
	tactical_combat_3d_mod.name = tactical_combat_3d->get_module_name();
	tactical_combat_3d_mod.description = tactical_combat_3d->get_module_description();
	tactical_combat_3d_mod.category = tactical_combat_3d->get_module_category();
	tactical_combat_3d_mod.files_to_create = tactical_combat_3d->get_files_to_create();
	available_modules.push_back(tactical_combat_3d_mod);

	// Relationship System
	RelationshipSystemModule* relationship_system = new RelationshipSystemModule();
	register_module("relationship_system", relationship_system);

	ProjectModule relationship_system_mod;
	relationship_system_mod.id = "relationship_system";
	relationship_system_mod.name = relationship_system->get_module_name();
	relationship_system_mod.description = relationship_system->get_module_description();
	relationship_system_mod.category = relationship_system->get_module_category();
	relationship_system_mod.files_to_create = relationship_system->get_files_to_create();
	available_modules.push_back(relationship_system_mod);

	// Support Conversation System
	SupportConversationModule* support_conversation = new SupportConversationModule();
	register_module("support_conversation_system", support_conversation);

	ProjectModule support_conversation_mod;
	support_conversation_mod.id = "support_conversation_system";
	support_conversation_mod.name = support_conversation->get_module_name();
	support_conversation_mod.description = support_conversation->get_module_description();
	support_conversation_mod.category = support_conversation->get_module_category();
	support_conversation_mod.files_to_create = support_conversation->get_files_to_create();
	available_modules.push_back(support_conversation_mod);

	// Social Interaction System
	SocialInteractionModule* social_interaction = new SocialInteractionModule();
	register_module("social_interaction_system", social_interaction);

	ProjectModule social_interaction_mod;
	social_interaction_mod.id = "social_interaction_system";
	social_interaction_mod.name = social_interaction->get_module_name();
	social_interaction_mod.description = social_interaction->get_module_description();
	social_interaction_mod.category = social_interaction->get_module_category();
	social_interaction_mod.files_to_create = social_interaction->get_files_to_create();
	available_modules.push_back(social_interaction_mod);

	// Faction Reputation System
	FactionReputationModule* faction_reputation = new FactionReputationModule();
	register_module("faction_reputation_system", faction_reputation);

	ProjectModule faction_reputation_mod;
	faction_reputation_mod.id = "faction_reputation_system";
	faction_reputation_mod.name = faction_reputation->get_module_name();
	faction_reputation_mod.description = faction_reputation->get_module_description();
	faction_reputation_mod.category = faction_reputation->get_module_category();
	faction_reputation_mod.files_to_create = faction_reputation->get_files_to_create();
	available_modules.push_back(faction_reputation_mod);
}

void LupineModuleManager::_register_3d_platformer_modules() {
	// 3D Player Controller
	Platformer3DControllerModule* platformer_3d_controller = new Platformer3DControllerModule();
	register_module("platformer_3d_controller", platformer_3d_controller);

	ProjectModule controller_3d_mod;
	controller_3d_mod.id = "platformer_3d_controller";
	controller_3d_mod.name = platformer_3d_controller->get_module_name();
	controller_3d_mod.description = platformer_3d_controller->get_module_description();
	controller_3d_mod.category = platformer_3d_controller->get_module_category();
	controller_3d_mod.files_to_create = platformer_3d_controller->get_files_to_create();
	available_modules.push_back(controller_3d_mod);

	// 3D Camera System
	Camera3DModule* camera_3d = new Camera3DModule();
	register_module("camera_3d", camera_3d);

	ProjectModule camera_3d_mod;
	camera_3d_mod.id = "camera_3d";
	camera_3d_mod.name = camera_3d->get_module_name();
	camera_3d_mod.description = camera_3d->get_module_description();
	camera_3d_mod.category = camera_3d->get_module_category();
	camera_3d_mod.files_to_create = camera_3d->get_files_to_create();
	available_modules.push_back(camera_3d_mod);

	// World State 3D
	WorldState3DModule* world_state_3d = new WorldState3DModule();
	register_module("world_state_3d", world_state_3d);

	ProjectModule world_state_3d_mod;
	world_state_3d_mod.id = "world_state_3d";
	world_state_3d_mod.name = world_state_3d->get_module_name();
	world_state_3d_mod.description = world_state_3d->get_module_description();
	world_state_3d_mod.category = world_state_3d->get_module_category();
	world_state_3d_mod.files_to_create = world_state_3d->get_files_to_create();
	available_modules.push_back(world_state_3d_mod);

	// Puzzle System 3D
	PuzzleSystem3DModule* puzzle_system_3d = new PuzzleSystem3DModule();
	register_module("puzzle_system_3d", puzzle_system_3d);

	ProjectModule puzzle_3d_mod;
	puzzle_3d_mod.id = "puzzle_system_3d";
	puzzle_3d_mod.name = puzzle_system_3d->get_module_name();
	puzzle_3d_mod.description = puzzle_system_3d->get_module_description();
	puzzle_3d_mod.category = puzzle_system_3d->get_module_category();
	puzzle_3d_mod.files_to_create = puzzle_system_3d->get_files_to_create();
	available_modules.push_back(puzzle_3d_mod);

	// Enemy System 3D
	Enemy3DModule* enemy_3d = new Enemy3DModule();
	register_module("enemy_3d", enemy_3d);

	ProjectModule enemy_3d_mod;
	enemy_3d_mod.id = "enemy_3d";
	enemy_3d_mod.name = enemy_3d->get_module_name();
	enemy_3d_mod.description = enemy_3d->get_module_description();
	enemy_3d_mod.category = enemy_3d->get_module_category();
	enemy_3d_mod.files_to_create = enemy_3d->get_files_to_create();
	available_modules.push_back(enemy_3d_mod);

	// Location Manager 3D
	LocationManager3DModule* location_manager_3d = new LocationManager3DModule();
	register_module("location_manager_3d", location_manager_3d);

	ProjectModule location_3d_mod;
	location_3d_mod.id = "location_manager_3d";
	location_3d_mod.name = location_manager_3d->get_module_name();
	location_3d_mod.description = location_manager_3d->get_module_description();
	location_3d_mod.category = location_manager_3d->get_module_category();
	location_3d_mod.files_to_create = location_manager_3d->get_files_to_create();
	available_modules.push_back(location_3d_mod);
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

void LupineModuleManager::_register_ui_systems() {
	// Menu Builder System
	MenuBuilderModule* menu_builder = new MenuBuilderModule();
	register_module("menu_builder", menu_builder);

	ProjectModule menu_mod;
	menu_mod.id = "menu_builder";
	menu_mod.name = menu_builder->get_module_name();
	menu_mod.description = menu_builder->get_module_description();
	menu_mod.category = menu_builder->get_module_category();
	menu_mod.files_to_create = menu_builder->get_files_to_create();
	available_modules.push_back(menu_mod);

	// HUD Builder System
	HUDBuilderModule* hud_builder = new HUDBuilderModule();
	register_module("hud_builder", hud_builder);

	ProjectModule hud_mod;
	hud_mod.id = "hud_builder";
	hud_mod.name = hud_builder->get_module_name();
	hud_mod.description = hud_builder->get_module_description();
	hud_mod.category = hud_builder->get_module_category();
	hud_mod.files_to_create = hud_builder->get_files_to_create();
	available_modules.push_back(hud_mod);
}

void LupineModuleManager::_register_monster_capture_modules() {
	// Pokemon Battle System
	PokemonBattleSystemModule* pokemon_battle = new PokemonBattleSystemModule();
	register_module("pokemon_battle_system", pokemon_battle);

	ProjectModule pokemon_battle_mod;
	pokemon_battle_mod.id = "pokemon_battle_system";
	pokemon_battle_mod.name = pokemon_battle->get_module_name();
	pokemon_battle_mod.description = pokemon_battle->get_module_description();
	pokemon_battle_mod.category = pokemon_battle->get_module_category();
	pokemon_battle_mod.files_to_create = pokemon_battle->get_files_to_create();
	available_modules.push_back(pokemon_battle_mod);

	// Monster System
	MonsterSystemModule* monster_system = new MonsterSystemModule();
	register_module("monster_system", monster_system);

	ProjectModule monster_system_mod;
	monster_system_mod.id = "monster_system";
	monster_system_mod.name = monster_system->get_module_name();
	monster_system_mod.description = monster_system->get_module_description();
	monster_system_mod.category = monster_system->get_module_category();
	monster_system_mod.files_to_create = monster_system->get_files_to_create();
	available_modules.push_back(monster_system_mod);

	// Monster Capture System
	MonsterCaptureSystemModule* monster_capture = new MonsterCaptureSystemModule();
	register_module("monster_capture_system", monster_capture);

	ProjectModule monster_capture_mod;
	monster_capture_mod.id = "monster_capture_system";
	monster_capture_mod.name = monster_capture->get_module_name();
	monster_capture_mod.description = monster_capture->get_module_description();
	monster_capture_mod.category = monster_capture->get_module_category();
	monster_capture_mod.files_to_create = monster_capture->get_files_to_create();
	available_modules.push_back(monster_capture_mod);
}
