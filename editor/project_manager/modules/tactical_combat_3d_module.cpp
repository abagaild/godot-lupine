#include "tactical_combat_3d_module.h"

void TacticalCombat3DModule::generate_script(Ref<FileAccess> p_file) {
	String filename = p_file->get_path().get_file();

	if (filename == "TacticalGrid3D.gd") {
		p_file->store_line("# TacticalGrid3D.gd");
		p_file->store_line("# Generated by Lupine Engine - 3D Tactical Grid");
		p_file->store_line("# 3D grid system with height levels and line of sight");
		p_file->store_line("");
		p_file->store_line("extends Node3D");
		p_file->store_line("class_name TacticalGrid3D");
		p_file->store_line("");
		p_file->store_line("# Grid settings");
		p_file->store_line("@export var grid_size: Vector2i = Vector2i(20, 15)");
		p_file->store_line("@export var tile_size: float = 2.0");
		p_file->store_line("@export var height_levels: int = 5");
		p_file->store_line("@export var height_step: float = 1.0");
		p_file->store_line("");
		p_file->store_line("# Visual components");
		p_file->store_line("@onready var grid_mesh: MeshInstance3D = $GridMesh");
		p_file->store_line("@onready var highlight_container: Node3D = $HighlightContainer");
		p_file->store_line("");
		p_file->store_line("# Grid data");
		p_file->store_line("var grid_heights: Array[Array] = []");
		p_file->store_line("var occupied_tiles: Dictionary = {}");
		p_file->store_line("var deployment_zones: Dictionary = {}");
		p_file->store_line("var highlight_tiles: Array[MeshInstance3D] = []");
		p_file->store_line("");
		p_file->store_line("# Materials");
		p_file->store_line("var movement_material: StandardMaterial3D");
		p_file->store_line("var attack_material: StandardMaterial3D");
		p_file->store_line("var deployment_material: StandardMaterial3D");
		p_file->store_line("");
		p_file->store_line("func _ready():");
		p_file->store_line("\t_setup_materials()");
		p_file->store_line("\t_generate_grid()");
		p_file->store_line("");
		p_file->store_line("func _setup_materials():");
		p_file->store_line("\t# Movement highlight");
		p_file->store_line("\tmovement_material = StandardMaterial3D.new()");
		p_file->store_line("\tmovement_material.albedo_color = Color.BLUE");
		p_file->store_line("\tmovement_material.transparency = BaseMaterial3D.TRANSPARENCY_ALPHA");
		p_file->store_line("\tmovement_material.albedo_color.a = 0.5");
		p_file->store_line("\t");
		p_file->store_line("\t# Attack highlight");
		p_file->store_line("\tattack_material = StandardMaterial3D.new()");
		p_file->store_line("\tattack_material.albedo_color = Color.RED");
		p_file->store_line("\tattack_material.transparency = BaseMaterial3D.TRANSPARENCY_ALPHA");
		p_file->store_line("\tattack_material.albedo_color.a = 0.5");
		p_file->store_line("\t");
		p_file->store_line("\t# Deployment highlight");
		p_file->store_line("\tdeployment_material = StandardMaterial3D.new()");
		p_file->store_line("\tdeployment_material.albedo_color = Color.GREEN");
		p_file->store_line("\tdeployment_material.transparency = BaseMaterial3D.TRANSPARENCY_ALPHA");
		p_file->store_line("\tdeployment_material.albedo_color.a = 0.5");
		p_file->store_line("");
		p_file->store_line("func _generate_grid():");
		p_file->store_line("\t# Initialize height array");
		p_file->store_line("\tgrid_heights.resize(grid_size.x)");
		p_file->store_line("\tfor x in range(grid_size.x):");
		p_file->store_line("\t\tgrid_heights[x] = []");
		p_file->store_line("\t\tgrid_heights[x].resize(grid_size.y)");
		p_file->store_line("\t\tfor y in range(grid_size.y):");
		p_file->store_line("\t\t\t# Default height is 0, can be customized");
		p_file->store_line("\t\t\tgrid_heights[x][y] = 0");
		p_file->store_line("\t");
		p_file->store_line("\t# Generate visual grid");
		p_file->store_line("\t_create_grid_mesh()");
		p_file->store_line("");
		p_file->store_line("func _create_grid_mesh():");
		p_file->store_line("\t# Create a simple grid mesh for visualization");
		p_file->store_line("\tvar array_mesh = ArrayMesh.new()");
		p_file->store_line("\tvar vertices = PackedVector3Array()");
		p_file->store_line("\tvar indices = PackedInt32Array()");
		p_file->store_line("\t");
		p_file->store_line("\t# Generate grid lines");
		p_file->store_line("\tfor x in range(grid_size.x + 1):");
		p_file->store_line("\t\tvar start = Vector3(x * tile_size, 0, 0)");
		p_file->store_line("\t\tvar end = Vector3(x * tile_size, 0, grid_size.y * tile_size)");
		p_file->store_line("\t\tvertices.append(start)");
		p_file->store_line("\t\tvertices.append(end)");
		p_file->store_line("\t");
		p_file->store_line("\tfor y in range(grid_size.y + 1):");
		p_file->store_line("\t\tvar start = Vector3(0, 0, y * tile_size)");
		p_file->store_line("\t\tvar end = Vector3(grid_size.x * tile_size, 0, y * tile_size)");
		p_file->store_line("\t\tvertices.append(start)");
		p_file->store_line("\t\tvertices.append(end)");
		p_file->store_line("\t");
		p_file->store_line("\t# Create indices for lines");
		p_file->store_line("\tfor i in range(vertices.size()):");
		p_file->store_line("\t\tindices.append(i)");
		p_file->store_line("\t");
		p_file->store_line("\t# Create mesh");
		p_file->store_line("\tvar arrays = []");
		p_file->store_line("\tarrays.resize(Mesh.ARRAY_MAX)");
		p_file->store_line("\tarrays[Mesh.ARRAY_VERTEX] = vertices");
		p_file->store_line("\tarrays[Mesh.ARRAY_INDEX] = indices");
		p_file->store_line("\t");
		p_file->store_line("\tarray_mesh.add_surface_from_arrays(Mesh.PRIMITIVE_LINES, arrays)");
		p_file->store_line("\tgrid_mesh.mesh = array_mesh");
		p_file->store_line("");
		p_file->store_line("func grid_to_world(grid_pos: Vector2i, height: int = 0) -> Vector3:");
		p_file->store_line("\treturn Vector3(");
		p_file->store_line("\t\tgrid_pos.x * tile_size,");
		p_file->store_line("\t\theight * height_step,");
		p_file->store_line("\t\tgrid_pos.y * tile_size");
		p_file->store_line("\t)");
		p_file->store_line("");
		p_file->store_line("func world_to_grid(world_pos: Vector3) -> Vector2i:");
		p_file->store_line("\treturn Vector2i(");
		p_file->store_line("\t\tint(world_pos.x / tile_size),");
		p_file->store_line("\t\tint(world_pos.z / tile_size)");
		p_file->store_line("\t)");
		p_file->store_line("");
		p_file->store_line("func get_tile_height(grid_pos: Vector2i) -> int:");
		p_file->store_line("\tif not is_valid_position(grid_pos):");
		p_file->store_line("\t\treturn 0");
		p_file->store_line("\treturn grid_heights[grid_pos.x][grid_pos.y]");
		p_file->store_line("");
		p_file->store_line("func set_tile_height(grid_pos: Vector2i, height: int):");
		p_file->store_line("\tif not is_valid_position(grid_pos):");
		p_file->store_line("\t\treturn");
		p_file->store_line("\tgrid_heights[grid_pos.x][grid_pos.y] = clamp(height, 0, height_levels - 1)");
		p_file->store_line("");
		p_file->store_line("func is_valid_position(grid_pos: Vector2i) -> bool:");
		p_file->store_line("\treturn grid_pos.x >= 0 and grid_pos.x < grid_size.x and grid_pos.y >= 0 and grid_pos.y < grid_size.y");
		p_file->store_line("");
		p_file->store_line("func is_tile_occupied(grid_pos: Vector2i) -> bool:");
		p_file->store_line("\treturn occupied_tiles.has(grid_pos)");
		p_file->store_line("");
		p_file->store_line("func place_unit(unit: TacticalUnit3D, grid_pos: Vector2i):");
		p_file->store_line("\tif not is_valid_position(grid_pos) or is_tile_occupied(grid_pos):");
		p_file->store_line("\t\treturn");
		p_file->store_line("\t");
		p_file->store_line("\toccupied_tiles[grid_pos] = unit");
		p_file->store_line("\tvar world_pos = grid_to_world(grid_pos, get_tile_height(grid_pos))");
		p_file->store_line("\tunit.global_position = world_pos");
		p_file->store_line("\tunit.grid_position = grid_pos");
		p_file->store_line("");
		p_file->store_line("func move_unit(unit: TacticalUnit3D, new_grid_pos: Vector2i):");
		p_file->store_line("\tif not is_valid_position(new_grid_pos) or is_tile_occupied(new_grid_pos):");
		p_file->store_line("\t\treturn");
		p_file->store_line("\t");
		p_file->store_line("\t# Remove from old position");
		p_file->store_line("\toccupied_tiles.erase(unit.grid_position)");
		p_file->store_line("\t");
		p_file->store_line("\t# Place at new position");
		p_file->store_line("\tplace_unit(unit, new_grid_pos)");
		p_file->store_line("");
		p_file->store_line("func remove_unit(unit: TacticalUnit3D):");
		p_file->store_line("\toccupied_tiles.erase(unit.grid_position)");
		p_file->store_line("");
		p_file->store_line("func highlight_tiles(positions: Array[Vector2i], highlight_type: String):");
		p_file->store_line("\tclear_highlights()");
		p_file->store_line("\t");
		p_file->store_line("\tvar material: StandardMaterial3D");
		p_file->store_line("\tmatch highlight_type:");
		p_file->store_line("\t\t\"movement\":");
		p_file->store_line("\t\t\tmaterial = movement_material");
		p_file->store_line("\t\t\"attack\":");
		p_file->store_line("\t\t\tmaterial = attack_material");
		p_file->store_line("\t\t\"deployment\":");
		p_file->store_line("\t\t\tmaterial = deployment_material");
		p_file->store_line("\t\t_:");
		p_file->store_line("\t\t\tmaterial = movement_material");
		p_file->store_line("\t");
		p_file->store_line("\tfor pos in positions:");
		p_file->store_line("\t\tvar highlight = MeshInstance3D.new()");
		p_file->store_line("\t\thighlight.mesh = QuadMesh.new()");
		p_file->store_line("\t\thighlight.mesh.size = Vector2(tile_size * 0.9, tile_size * 0.9)");
		p_file->store_line("\t\thighlight.material_override = material");
		p_file->store_line("\t\t");
		p_file->store_line("\t\tvar world_pos = grid_to_world(pos, get_tile_height(pos))");
		p_file->store_line("\t\thighlight.global_position = world_pos + Vector3(0, 0.01, 0)");
		p_file->store_line("\t\thighlight.rotation_degrees = Vector3(-90, 0, 0)");
		p_file->store_line("\t\t");
		p_file->store_line("\t\thighlight_container.add_child(highlight)");
		p_file->store_line("\t\thighlight_tiles.append(highlight)");
		p_file->store_line("");
		p_file->store_line("func clear_highlights():");
		p_file->store_line("\tfor highlight in highlight_tiles:");
		p_file->store_line("\t\thighlight.queue_free()");
		p_file->store_line("\thighlight_tiles.clear()");
		p_file->store_line("");
		p_file->store_line("func set_deployment_zone(faction: String, positions: Array[Vector2i]):");
		p_file->store_line("\tdeployment_zones[faction] = positions");
		p_file->store_line("");
		p_file->store_line("func is_valid_deployment_position(grid_pos: Vector2i, faction: String) -> bool:");
		p_file->store_line("\tif not deployment_zones.has(faction):");
		p_file->store_line("\t\treturn true  # No restrictions");
		p_file->store_line("\t");
		p_file->store_line("\treturn deployment_zones[faction].has(grid_pos) and not is_tile_occupied(grid_pos)");
		p_file->store_line("");
		p_file->store_line("func get_height_difference(pos1: Vector2i, pos2: Vector2i) -> int:");
		p_file->store_line("\treturn get_tile_height(pos2) - get_tile_height(pos1)");
		p_file->store_line("");
		p_file->store_line("func has_line_of_sight(from: Vector2i, to: Vector2i) -> bool:");
		p_file->store_line("\t# Simple line of sight check - can be enhanced");
		p_file->store_line("\tvar from_height = get_tile_height(from)");
		p_file->store_line("\tvar to_height = get_tile_height(to)");
		p_file->store_line("\t");
		p_file->store_line("\t# Check if any tiles in between block the view");
		p_file->store_line("\tvar line_tiles = get_line_tiles(from, to)");
		p_file->store_line("\tfor i in range(1, line_tiles.size() - 1):  # Skip start and end");
		p_file->store_line("\t\tvar tile_height = get_tile_height(line_tiles[i])");
		p_file->store_line("\t\tif tile_height > min(from_height, to_height) + 1:");
		p_file->store_line("\t\t\treturn false");
		p_file->store_line("\t");
		p_file->store_line("\treturn true");
		p_file->store_line("");
		p_file->store_line("func get_line_tiles(from: Vector2i, to: Vector2i) -> Array[Vector2i]:");
		p_file->store_line("\t# Bresenham's line algorithm for grid tiles");
		p_file->store_line("\tvar tiles: Array[Vector2i] = []");
		p_file->store_line("\tvar dx = abs(to.x - from.x)");
		p_file->store_line("\tvar dy = abs(to.y - from.y)");
		p_file->store_line("\tvar sx = 1 if from.x < to.x else -1");
		p_file->store_line("\tvar sy = 1 if from.y < to.y else -1");
		p_file->store_line("\tvar err = dx - dy");
		p_file->store_line("\t");
		p_file->store_line("\tvar current = from");
		p_file->store_line("\twhile true:");
		p_file->store_line("\t\ttiles.append(current)");
		p_file->store_line("\t\tif current == to:");
		p_file->store_line("\t\t\tbreak");
		p_file->store_line("\t\t");
		p_file->store_line("\t\tvar e2 = 2 * err");
		p_file->store_line("\t\tif e2 > -dy:");
		p_file->store_line("\t\t\terr -= dy");
		p_file->store_line("\t\t\tcurrent.x += sx");
		p_file->store_line("\t\tif e2 < dx:");
		p_file->store_line("\t\t\terr += dx");
		p_file->store_line("\t\t\tcurrent.y += sy");
		p_file->store_line("\t");
		p_file->store_line("\treturn tiles");
	}
}

void TacticalCombat3DModule::generate_scene(Ref<FileAccess> p_file, const String &p_scene_name) {
	if (p_scene_name == "TacticalBattlefield3D") {
		p_file->store_line("[gd_scene load_steps=2 format=3 uid=\"uid://tactical_battlefield_3d\"]");
		p_file->store_line("");
		p_file->store_line("[ext_resource type=\"Script\" path=\"res://scripts/tactical/TacticalGrid3D.gd\" id=\"1_grid_script\"]");
		p_file->store_line("");
		p_file->store_line("[node name=\"TacticalBattlefield3D\" type=\"Node3D\"]");
		p_file->store_line("script = ExtResource(\"1_grid_script\")");
		p_file->store_line("");
		p_file->store_line("[node name=\"GridMesh\" type=\"MeshInstance3D\" parent=\".\"]");
		p_file->store_line("");
		p_file->store_line("[node name=\"HighlightContainer\" type=\"Node3D\" parent=\".\"]");
		p_file->store_line("");
		p_file->store_line("[node name=\"UnitsContainer\" type=\"Node3D\" parent=\".\"]");
		p_file->store_line("");
		p_file->store_line("[node name=\"EffectsContainer\" type=\"Node3D\" parent=\".\"]");
	} else if (p_scene_name == "TacticalUnit3D") {
		p_file->store_line("[gd_scene load_steps=4 format=3 uid=\"uid://tactical_unit_3d\"]");
		p_file->store_line("");
		p_file->store_line("[ext_resource type=\"Script\" path=\"res://scripts/tactical/TacticalUnit3D.gd\" id=\"1_unit_script\"]");
		p_file->store_line("");
		p_file->store_line("[sub_resource type=\"CapsuleShape3D\" id=\"CapsuleShape3D_1\"]");
		p_file->store_line("radius = 0.4");
		p_file->store_line("height = 1.8");
		p_file->store_line("");
		p_file->store_line("[sub_resource type=\"CapsuleMesh\" id=\"CapsuleMesh_1\"]");
		p_file->store_line("radius = 0.4");
		p_file->store_line("height = 1.8");
		p_file->store_line("");
		p_file->store_line("[node name=\"TacticalUnit3D\" type=\"CharacterBody3D\"]");
		p_file->store_line("script = ExtResource(\"1_unit_script\")");
		p_file->store_line("");
		p_file->store_line("[node name=\"CollisionShape3D\" type=\"CollisionShape3D\" parent=\".\"]");
		p_file->store_line("shape = SubResource(\"CapsuleShape3D_1\")");
		p_file->store_line("");
		p_file->store_line("[node name=\"MeshInstance3D\" type=\"MeshInstance3D\" parent=\".\"]");
		p_file->store_line("mesh = SubResource(\"CapsuleMesh_1\")");
		p_file->store_line("");
		p_file->store_line("[node name=\"HealthBar3D\" type=\"Control\" parent=\".\"]");
		p_file->store_line("layout_mode = 3");
		p_file->store_line("anchors_preset = 8");
		p_file->store_line("anchor_left = 0.5");
		p_file->store_line("anchor_top = 0.5");
		p_file->store_line("anchor_right = 0.5");
		p_file->store_line("anchor_bottom = 0.5");
		p_file->store_line("offset_left = -25.0");
		p_file->store_line("offset_top = -50.0");
		p_file->store_line("offset_right = 25.0");
		p_file->store_line("offset_bottom = -40.0");
		p_file->store_line("");
		p_file->store_line("[node name=\"ProgressBar\" type=\"ProgressBar\" parent=\"HealthBar3D\"]");
		p_file->store_line("layout_mode = 1");
		p_file->store_line("anchors_preset = 15");
		p_file->store_line("anchor_right = 1.0");
		p_file->store_line("anchor_bottom = 1.0");
		p_file->store_line("max_value = 100.0");
		p_file->store_line("value = 100.0");
	}
}
