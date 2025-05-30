#include "topdown_enemy_types_module.h"

void TopdownEnemyTypesModule::generate_script(Ref<FileAccess> p_file) {
	String filename = p_file->get_path().get_file();

	if (filename == "EnemyPatrolGuard.gd") {
		p_file->store_line("# EnemyPatrolGuard.gd");
		p_file->store_line("# Generated by Lupine Engine - Patrol Guard Enemy");
		p_file->store_line("# Guards that patrol between waypoints and chase intruders");
		p_file->store_line("");
		p_file->store_line("extends EnemyBase");
		p_file->store_line("class_name EnemyPatrolGuard");
		p_file->store_line("");
		p_file->store_line("# Patrol settings");
		p_file->store_line("@export var patrol_points: Array[Vector2] = []");
		p_file->store_line("@export var patrol_speed: float = 80.0");
		p_file->store_line("@export var chase_speed: float = 120.0");
		p_file->store_line("@export var wait_time_at_point: float = 2.0");
		p_file->store_line("@export var return_to_patrol_time: float = 5.0");
		p_file->store_line("@export var patrol_detection_range: float = 100.0");
		p_file->store_line("@export var chase_detection_range: float = 200.0");
		p_file->store_line("");
		p_file->store_line("# State variables");
		p_file->store_line("var current_patrol_index: int = 0");
		p_file->store_line("var wait_timer: float = 0.0");
		p_file->store_line("var return_timer: float = 0.0");
		p_file->store_line("var original_position: Vector2");
		p_file->store_line("var patrol_direction: int = 1");
		p_file->store_line("");
		p_file->store_line("func _ready():");
		p_file->store_line("\tsuper._ready()");
		p_file->store_line("\toriginal_position = global_position");
		p_file->store_line("\t");
		p_file->store_line("\t# Setup default patrol if none provided");
		p_file->store_line("\tif patrol_points.is_empty():");
		p_file->store_line("\t\tpatrol_points = [");
		p_file->store_line("\t\t\toriginal_position + Vector2(-100, 0),");
		p_file->store_line("\t\t\toriginal_position + Vector2(100, 0)");
		p_file->store_line("\t\t]");
		p_file->store_line("\t");
		p_file->store_line("\tcurrent_state = AIState.PATROL");
		p_file->store_line("");
		p_file->store_line("func _update_ai_state(delta: float):");
		p_file->store_line("\tmatch current_state:");
		p_file->store_line("\t\tAIState.PATROL:");
		p_file->store_line("\t\t\t_handle_patrol_state(delta)");
		p_file->store_line("\t\tAIState.CHASE:");
		p_file->store_line("\t\t\t_handle_chase_state(delta)");
		p_file->store_line("\t\tAIState.ATTACK:");
		p_file->store_line("\t\t\t_handle_attack_state(delta)");
		p_file->store_line("\t\tAIState.RETURN:");
		p_file->store_line("\t\t\t_handle_return_state(delta)");
		p_file->store_line("");
		p_file->store_line("func _handle_movement(delta: float):");
		p_file->store_line("\tmatch current_state:");
		p_file->store_line("\t\tAIState.PATROL:");
		p_file->store_line("\t\t\t_move_to_patrol_point(delta)");
		p_file->store_line("\t\tAIState.CHASE:");
		p_file->store_line("\t\t\t_chase_player(delta)");
		p_file->store_line("\t\tAIState.RETURN:");
		p_file->store_line("\t\t\t_return_to_patrol(delta)");
		p_file->store_line("");
		p_file->store_line("func _handle_patrol_state(delta: float):");
		p_file->store_line("\t# Check for player in detection range");
		p_file->store_line("\tif player_target:");
		p_file->store_line("\t\tvar distance = get_distance_to_player()");
		p_file->store_line("\t\tif distance <= patrol_detection_range:");
		p_file->store_line("\t\t\tcurrent_state = AIState.CHASE");
		p_file->store_line("\t\t\treturn");
		p_file->store_line("\t");
		p_file->store_line("\t# Handle waiting at patrol point");
		p_file->store_line("\tif wait_timer > 0:");
		p_file->store_line("\t\twait_timer -= delta");
		p_file->store_line("\t\treturn");
		p_file->store_line("\t");
		p_file->store_line("\t# Check if reached current patrol point");
		p_file->store_line("\tvar target_point = patrol_points[current_patrol_index]");
		p_file->store_line("\tif global_position.distance_to(target_point) < 20.0:");
		p_file->store_line("\t\twait_timer = wait_time_at_point");
		p_file->store_line("\t\t_next_patrol_point()");
		p_file->store_line("");
		p_file->store_line("func _handle_chase_state(delta: float):");
		p_file->store_line("\tif not player_target:");
		p_file->store_line("\t\tcurrent_state = AIState.RETURN");
		p_file->store_line("\t\treturn_timer = return_to_patrol_time");
		p_file->store_line("\t\treturn");
		p_file->store_line("\t");
		p_file->store_line("\tvar distance = get_distance_to_player()");
		p_file->store_line("\t");
		p_file->store_line("\t# Attack if in range");
		p_file->store_line("\tif distance <= attack_range:");
		p_file->store_line("\t\tcurrent_state = AIState.ATTACK");
		p_file->store_line("\t\tstate_timer = 0.0");
		p_file->store_line("\t# Stop chasing if too far");
		p_file->store_line("\telif distance > chase_detection_range:");
		p_file->store_line("\t\tcurrent_state = AIState.RETURN");
		p_file->store_line("\t\treturn_timer = return_to_patrol_time");
		p_file->store_line("");
		p_file->store_line("func _handle_attack_state(delta: float):");
		p_file->store_line("\tif state_timer >= 1.0:  # Attack cooldown");
		p_file->store_line("\t\tif attack_player():");
		p_file->store_line("\t\t\tstate_timer = 0.0");
		p_file->store_line("\t\telse:");
		p_file->store_line("\t\t\tcurrent_state = AIState.CHASE");
		p_file->store_line("");
		p_file->store_line("func _handle_return_state(delta: float):");
		p_file->store_line("\treturn_timer -= delta");
		p_file->store_line("\t");
		p_file->store_line("\t# Check if player is nearby again");
		p_file->store_line("\tif player_target and get_distance_to_player() <= patrol_detection_range:");
		p_file->store_line("\t\tcurrent_state = AIState.CHASE");
		p_file->store_line("\t\treturn");
		p_file->store_line("\t");
		p_file->store_line("\t# Return to patrol if timer expires or reached patrol area");
		p_file->store_line("\tvar nearest_patrol = _get_nearest_patrol_point()");
		p_file->store_line("\tif return_timer <= 0 or global_position.distance_to(nearest_patrol) < 30.0:");
		p_file->store_line("\t\tcurrent_state = AIState.PATROL");
		p_file->store_line("\t\tcurrent_patrol_index = _get_nearest_patrol_index()");
		p_file->store_line("");
		p_file->store_line("func _move_to_patrol_point(delta: float):");
		p_file->store_line("\tif wait_timer > 0:");
		p_file->store_line("\t\tvelocity = Vector2.ZERO");
		p_file->store_line("\t\treturn");
		p_file->store_line("\t");
		p_file->store_line("\tvar target = patrol_points[current_patrol_index]");
		p_file->store_line("\tvar direction = (target - global_position).normalized()");
		p_file->store_line("\tvelocity = direction * patrol_speed");
		p_file->store_line("");
		p_file->store_line("func _chase_player(delta: float):");
		p_file->store_line("\tvar direction = get_direction_to_player()");
		p_file->store_line("\tvelocity = direction * chase_speed");
		p_file->store_line("");
		p_file->store_line("func _return_to_patrol(delta: float):");
		p_file->store_line("\tvar target = _get_nearest_patrol_point()");
		p_file->store_line("\tvar direction = (target - global_position).normalized()");
		p_file->store_line("\tvelocity = direction * patrol_speed");
		p_file->store_line("");
		p_file->store_line("func _next_patrol_point():");
		p_file->store_line("\tcurrent_patrol_index += patrol_direction");
		p_file->store_line("\t");
		p_file->store_line("\t# Reverse direction at endpoints");
		p_file->store_line("\tif current_patrol_index >= patrol_points.size():");
		p_file->store_line("\t\tcurrent_patrol_index = patrol_points.size() - 2");
		p_file->store_line("\t\tpatrol_direction = -1");
		p_file->store_line("\telif current_patrol_index < 0:");
		p_file->store_line("\t\tcurrent_patrol_index = 1");
		p_file->store_line("\t\tpatrol_direction = 1");
		p_file->store_line("");
		p_file->store_line("func _get_nearest_patrol_point() -> Vector2:");
		p_file->store_line("\tvar nearest = patrol_points[0]");
		p_file->store_line("\tvar min_distance = global_position.distance_to(nearest)");
		p_file->store_line("\t");
		p_file->store_line("\tfor point in patrol_points:");
		p_file->store_line("\t\tvar distance = global_position.distance_to(point)");
		p_file->store_line("\t\tif distance < min_distance:");
		p_file->store_line("\t\t\tmin_distance = distance");
		p_file->store_line("\t\t\tnearest = point");
		p_file->store_line("\t");
		p_file->store_line("\treturn nearest");
		p_file->store_line("");
		p_file->store_line("func _get_nearest_patrol_index() -> int:");
		p_file->store_line("\tvar nearest_index = 0");
		p_file->store_line("\tvar min_distance = global_position.distance_to(patrol_points[0])");
		p_file->store_line("\t");
		p_file->store_line("\tfor i in range(patrol_points.size()):");
		p_file->store_line("\t\tvar distance = global_position.distance_to(patrol_points[i])");
		p_file->store_line("\t\tif distance < min_distance:");
		p_file->store_line("\t\t\tmin_distance = distance");
		p_file->store_line("\t\t\tnearest_index = i");
		p_file->store_line("\t");
		p_file->store_line("\treturn nearest_index");
		p_file->store_line("");
		p_file->store_line("func get_enemy_type() -> String:");
		p_file->store_line("\treturn \"patrol_guard\"");
	} else if (filename == "EnemyRangedAttacker.gd") {
		p_file->store_line("# EnemyRangedAttacker.gd");
		p_file->store_line("# Generated by Lupine Engine - Ranged Attacker Enemy");
		p_file->store_line("# Enemies that attack from distance with projectiles");
		p_file->store_line("");
		p_file->store_line("extends EnemyBase");
		p_file->store_line("class_name EnemyRangedAttacker");
		p_file->store_line("");
		p_file->store_line("# Ranged attack settings");
		p_file->store_line("@export var projectile_scene: PackedScene");
		p_file->store_line("@export var ranged_attack_range: float = 300.0");
		p_file->store_line("@export var min_attack_distance: float = 150.0");
		p_file->store_line("@export var attack_cooldown: float = 2.0");
		p_file->store_line("@export var projectile_speed: float = 400.0");
		p_file->store_line("@export var retreat_speed: float = 120.0");
		p_file->store_line("@export var kiting_distance: float = 200.0");
		p_file->store_line("");
		p_file->store_line("# State variables");
		p_file->store_line("var attack_timer: float = 0.0");
		p_file->store_line("var last_attack_position: Vector2");
		p_file->store_line("");
		p_file->store_line("func _ready():");
		p_file->store_line("\tsuper._ready()");
		p_file->store_line("\tcurrent_state = AIState.IDLE");
		p_file->store_line("\tattack_range = ranged_attack_range");
		p_file->store_line("");
		p_file->store_line("func _update_ai_state(delta: float):");
		p_file->store_line("\tattack_timer -= delta");
		p_file->store_line("\t");
		p_file->store_line("\tmatch current_state:");
		p_file->store_line("\t\tAIState.IDLE:");
		p_file->store_line("\t\t\t_handle_idle_state()");
		p_file->store_line("\t\tAIState.CHASE:");
		p_file->store_line("\t\t\t_handle_chase_state()");
		p_file->store_line("\t\tAIState.ATTACK:");
		p_file->store_line("\t\t\t_handle_attack_state()");
		p_file->store_line("");
		p_file->store_line("func _handle_movement(delta: float):");
		p_file->store_line("\tmatch current_state:");
		p_file->store_line("\t\tAIState.CHASE:");
		p_file->store_line("\t\t\t_move_to_optimal_range(delta)");
		p_file->store_line("\t\tAIState.ATTACK:");
		p_file->store_line("\t\t\t_maintain_distance(delta)");
		p_file->store_line("");
		p_file->store_line("func _handle_idle_state():");
		p_file->store_line("\tif player_target:");
		p_file->store_line("\t\tvar distance = get_distance_to_player()");
		p_file->store_line("\t\tif distance <= detection_range:");
		p_file->store_line("\t\t\tcurrent_state = AIState.CHASE");
		p_file->store_line("");
		p_file->store_line("func _handle_chase_state():");
		p_file->store_line("\tif not player_target:");
		p_file->store_line("\t\tcurrent_state = AIState.IDLE");
		p_file->store_line("\t\treturn");
		p_file->store_line("\t");
		p_file->store_line("\tvar distance = get_distance_to_player()");
		p_file->store_line("\t");
		p_file->store_line("\t# Start attacking if in range");
		p_file->store_line("\tif distance <= ranged_attack_range and distance >= min_attack_distance:");
		p_file->store_line("\t\tcurrent_state = AIState.ATTACK");
		p_file->store_line("\t# Too far, keep chasing");
		p_file->store_line("\telif distance > ranged_attack_range:");
		p_file->store_line("\t\tpass  # Continue chasing");
		p_file->store_line("");
		p_file->store_line("func _handle_attack_state():");
		p_file->store_line("\tif not player_target:");
		p_file->store_line("\t\tcurrent_state = AIState.IDLE");
		p_file->store_line("\t\treturn");
		p_file->store_line("\t");
		p_file->store_line("\tvar distance = get_distance_to_player()");
		p_file->store_line("\t");
		p_file->store_line("\t# Too close, retreat");
		p_file->store_line("\tif distance < min_attack_distance:");
		p_file->store_line("\t\tcurrent_state = AIState.CHASE");
		p_file->store_line("\t# Too far, chase");
		p_file->store_line("\telif distance > ranged_attack_range:");
		p_file->store_line("\t\tcurrent_state = AIState.CHASE");
		p_file->store_line("\t# In range, attack");
		p_file->store_line("\telif attack_timer <= 0:");
		p_file->store_line("\t\t_fire_projectile()");
		p_file->store_line("\t\tattack_timer = attack_cooldown");
		p_file->store_line("");
		p_file->store_line("func _move_to_optimal_range(delta: float):");
		p_file->store_line("\tvar distance = get_distance_to_player()");
		p_file->store_line("\tvar direction = get_direction_to_player()");
		p_file->store_line("\t");
		p_file->store_line("\tif distance < min_attack_distance:");
		p_file->store_line("\t\t# Retreat");
		p_file->store_line("\t\tvelocity = -direction * retreat_speed");
		p_file->store_line("\telif distance > ranged_attack_range:");
		p_file->store_line("\t\t# Approach");
		p_file->store_line("\t\tvelocity = direction * move_speed");
		p_file->store_line("\telse:");
		p_file->store_line("\t\t# In optimal range, stop moving");
		p_file->store_line("\t\tvelocity = Vector2.ZERO");
		p_file->store_line("");
		p_file->store_line("func _maintain_distance(delta: float):");
		p_file->store_line("\tvar distance = get_distance_to_player()");
		p_file->store_line("\tvar direction = get_direction_to_player()");
		p_file->store_line("\t");
		p_file->store_line("\t# Kite around the player");
		p_file->store_line("\tif distance < kiting_distance:");
		p_file->store_line("\t\t# Move perpendicular to player direction");
		p_file->store_line("\t\tvar perpendicular = Vector2(-direction.y, direction.x)");
		p_file->store_line("\t\tvelocity = perpendicular * move_speed * 0.5");
		p_file->store_line("\telse:");
		p_file->store_line("\t\tvelocity = Vector2.ZERO");
		p_file->store_line("");
		p_file->store_line("func _fire_projectile():");
		p_file->store_line("\tif not projectile_scene:");
		p_file->store_line("\t\t# Fallback to instant damage");
		p_file->store_line("\t\tattack_player()");
		p_file->store_line("\t\treturn");
		p_file->store_line("\t");
		p_file->store_line("\tvar projectile = projectile_scene.instantiate()");
		p_file->store_line("\tget_tree().current_scene.add_child(projectile)");
		p_file->store_line("\tprojectile.global_position = global_position");
		p_file->store_line("\t");
		p_file->store_line("\t# Set projectile direction and speed");
		p_file->store_line("\tvar direction = get_direction_to_player()");
		p_file->store_line("\tif projectile.has_method(\"setup\"):");
		p_file->store_line("\t\tprojectile.setup(direction, projectile_speed, attack_damage)");
		p_file->store_line("");
		p_file->store_line("func get_enemy_type() -> String:");
		p_file->store_line("\treturn \"ranged_attacker\"");
	}
}

void TopdownEnemyTypesModule::generate_scene(Ref<FileAccess> p_file, const String &p_scene_name) {
	if (p_scene_name == "EnemyPatrolGuard") {
		p_file->store_line("[gd_scene load_steps=4 format=3 uid=\"uid://enemy_patrol_guard\"]");
		p_file->store_line("");
		p_file->store_line("[ext_resource type=\"Script\" path=\"res://scripts/enemies/EnemyPatrolGuard.gd\" id=\"1_patrol_script\"]");
		p_file->store_line("");
		p_file->store_line("[sub_resource type=\"RectangleShape2D\" id=\"RectangleShape2D_1\"]");
		p_file->store_line("size = Vector2(20, 20)");
		p_file->store_line("");
		p_file->store_line("[sub_resource type=\"CircleShape2D\" id=\"CircleShape2D_1\"]");
		p_file->store_line("radius = 100.0");
		p_file->store_line("");
		p_file->store_line("[node name=\"EnemyPatrolGuard\" type=\"CharacterBody2D\"]");
		p_file->store_line("script = ExtResource(\"1_patrol_script\")");
		p_file->store_line("");
		p_file->store_line("[node name=\"CollisionShape2D\" type=\"CollisionShape2D\" parent=\".\"]");
		p_file->store_line("shape = SubResource(\"RectangleShape2D_1\")");
		p_file->store_line("");
		p_file->store_line("[node name=\"Sprite2D\" type=\"Sprite2D\" parent=\".\"]");
		p_file->store_line("modulate = Color(0.8, 0.2, 0.2, 1)");
		p_file->store_line("");
		p_file->store_line("[node name=\"DetectionArea\" type=\"Area2D\" parent=\".\"]");
		p_file->store_line("");
		p_file->store_line("[node name=\"DetectionShape\" type=\"CollisionShape2D\" parent=\"DetectionArea\"]");
		p_file->store_line("shape = SubResource(\"CircleShape2D_1\")");
	} else if (p_scene_name == "EnemyRangedAttacker") {
		p_file->store_line("[gd_scene load_steps=4 format=3 uid=\"uid://enemy_ranged_attacker\"]");
		p_file->store_line("");
		p_file->store_line("[ext_resource type=\"Script\" path=\"res://scripts/enemies/EnemyRangedAttacker.gd\" id=\"1_ranged_script\"]");
		p_file->store_line("");
		p_file->store_line("[sub_resource type=\"RectangleShape2D\" id=\"RectangleShape2D_1\"]");
		p_file->store_line("size = Vector2(18, 18)");
		p_file->store_line("");
		p_file->store_line("[sub_resource type=\"CircleShape2D\" id=\"CircleShape2D_1\"]");
		p_file->store_line("radius = 300.0");
		p_file->store_line("");
		p_file->store_line("[node name=\"EnemyRangedAttacker\" type=\"CharacterBody2D\"]");
		p_file->store_line("script = ExtResource(\"1_ranged_script\")");
		p_file->store_line("");
		p_file->store_line("[node name=\"CollisionShape2D\" type=\"CollisionShape2D\" parent=\".\"]");
		p_file->store_line("shape = SubResource(\"RectangleShape2D_1\")");
		p_file->store_line("");
		p_file->store_line("[node name=\"Sprite2D\" type=\"Sprite2D\" parent=\".\"]");
		p_file->store_line("modulate = Color(0.2, 0.8, 0.2, 1)");
		p_file->store_line("");
		p_file->store_line("[node name=\"DetectionArea\" type=\"Area2D\" parent=\".\"]");
		p_file->store_line("");
		p_file->store_line("[node name=\"DetectionShape\" type=\"CollisionShape2D\" parent=\"DetectionArea\"]");
		p_file->store_line("shape = SubResource(\"CircleShape2D_1\")");
	}
}
