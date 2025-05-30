#include "camera_3d_fps_module.h"

void Camera3DFPSModule::generate_script(Ref<FileAccess> p_file) {
	p_file->store_line("# Camera3DFPS.gd");
	p_file->store_line("# Generated by Lupine Engine - 3D First-Person Camera");
	p_file->store_line("# 3D first-person camera with mouse look, zoom controls, and smooth movement");
	p_file->store_line("");
	p_file->store_line("extends Camera3D");
	p_file->store_line("class_name Camera3DFPS");
	p_file->store_line("");
	p_file->store_line("# Camera settings");
	p_file->store_line("@export var mouse_sensitivity: float = 0.002");
	p_file->store_line("@export var follow_speed: float = 10.0");
	p_file->store_line("@export var rotation_smoothing: float = 8.0");
	p_file->store_line("@export var enable_smooth_rotation: bool = true");
	p_file->store_line("");
	p_file->store_line("# FOV and zoom");
	p_file->store_line("@export var default_fov: float = 75.0");
	p_file->store_line("@export var min_fov: float = 30.0");
	p_file->store_line("@export var max_fov: float = 120.0");
	p_file->store_line("@export var zoom_speed: float = 3.0");
	p_file->store_line("@export var fov_step: float = 5.0");
	p_file->store_line("@export var smooth_zoom: bool = true");
	p_file->store_line("");
	p_file->store_line("# Vertical rotation limits");
	p_file->store_line("@export var min_pitch: float = -90.0");
	p_file->store_line("@export var max_pitch: float = 90.0");
	p_file->store_line("");
	p_file->store_line("# Offset and positioning");
	p_file->store_line("@export var camera_offset: Vector3 = Vector3(0, 1.8, 0)");
	p_file->store_line("@export var enable_head_bob: bool = false");
	p_file->store_line("@export var head_bob_intensity: float = 0.05");
	p_file->store_line("@export var head_bob_frequency: float = 2.0");
	p_file->store_line("");
	p_file->store_line("# Target and state");
	p_file->store_line("var target: Node3D");
	p_file->store_line("var current_fov: float");
	p_file->store_line("var target_fov: float");
	p_file->store_line("var rotation_x: float = 0.0");
	p_file->store_line("var rotation_y: float = 0.0");
	p_file->store_line("var target_rotation_x: float = 0.0");
	p_file->store_line("var target_rotation_y: float = 0.0");
	p_file->store_line("var mouse_captured: bool = false");
	p_file->store_line("var head_bob_time: float = 0.0");
	p_file->store_line("var is_moving: bool = false");
	p_file->store_line("");
	p_file->store_line("# Signals");
	p_file->store_line("signal fov_changed(new_fov: float)");
	p_file->store_line("signal target_changed(new_target: Node3D)");
	p_file->store_line("");
	p_file->store_line("func _ready():");
	p_file->store_line("\tcurrent_fov = default_fov");
	p_file->store_line("\ttarget_fov = default_fov");
	p_file->store_line("\tfov = default_fov");
	p_file->store_line("\t");
	p_file->store_line("\t# Make this camera current");
	p_file->store_line("\tmake_current()");
	p_file->store_line("\t");
	p_file->store_line("\t# Capture mouse initially");
	p_file->store_line("\tInput.mouse_mode = Input.MOUSE_MODE_CAPTURED");
	p_file->store_line("\tmouse_captured = true");
	p_file->store_line("");
	p_file->store_line("func _input(event):");
	p_file->store_line("\t# Toggle mouse capture");
	p_file->store_line("\tif Input.is_action_just_pressed(\"ui_cancel\"):");
	p_file->store_line("\t\ttoggle_mouse_capture()");
	p_file->store_line("\t");
	p_file->store_line("\t# Handle mouse movement for look");
	p_file->store_line("\tif event is InputEventMouseMotion and mouse_captured:");
	p_file->store_line("\t\ttarget_rotation_y -= event.relative.x * mouse_sensitivity");
	p_file->store_line("\t\ttarget_rotation_x -= event.relative.y * mouse_sensitivity");
	p_file->store_line("\t\ttarget_rotation_x = clamp(target_rotation_x, deg_to_rad(min_pitch), deg_to_rad(max_pitch))");
	p_file->store_line("\t");
	p_file->store_line("\t# Handle zoom input (FOV)");
	p_file->store_line("\tif Input.is_action_just_pressed(\"zoom_in\"):");
	p_file->store_line("\t\ttarget_fov = clamp(target_fov - fov_step, min_fov, max_fov)");
	p_file->store_line("\t\tfov_changed.emit(target_fov)");
	p_file->store_line("\telif Input.is_action_just_pressed(\"zoom_out\"):");
	p_file->store_line("\t\ttarget_fov = clamp(target_fov + fov_step, min_fov, max_fov)");
	p_file->store_line("\t\tfov_changed.emit(target_fov)");
	p_file->store_line("\t");
	p_file->store_line("\t# Reset FOV");
	p_file->store_line("\tif Input.is_action_just_pressed(\"reset_zoom\"):");
	p_file->store_line("\t\ttarget_fov = default_fov");
	p_file->store_line("\t\tfov_changed.emit(target_fov)");
	p_file->store_line("");
	p_file->store_line("func _process(delta: float):");
	p_file->store_line("\t# Smooth rotation");
	p_file->store_line("\tif enable_smooth_rotation:");
	p_file->store_line("\t\trotation_x = lerp(rotation_x, target_rotation_x, rotation_smoothing * delta)");
	p_file->store_line("\t\trotation_y = lerp(rotation_y, target_rotation_y, rotation_smoothing * delta)");
	p_file->store_line("\telse:");
	p_file->store_line("\t\trotation_x = target_rotation_x");
	p_file->store_line("\t\trotation_y = target_rotation_y");
	p_file->store_line("\t");
	p_file->store_line("\t# Apply rotation");
	p_file->store_line("\trotation.x = rotation_x");
	p_file->store_line("\trotation.y = rotation_y");
	p_file->store_line("\t");
	p_file->store_line("\t# Smooth FOV");
	p_file->store_line("\tif smooth_zoom:");
	p_file->store_line("\t\tcurrent_fov = lerp(current_fov, target_fov, zoom_speed * delta)");
	p_file->store_line("\telse:");
	p_file->store_line("\t\tcurrent_fov = target_fov");
	p_file->store_line("\tfov = current_fov");
	p_file->store_line("\t");
	p_file->store_line("\t# Follow target");
	p_file->store_line("\tif target:");
	p_file->store_line("\t\t_follow_target(delta)");
	p_file->store_line("\t");
	p_file->store_line("\t# Handle head bob");
	p_file->store_line("\tif enable_head_bob:");
	p_file->store_line("\t\t_handle_head_bob(delta)");
	p_file->store_line("");
	p_file->store_line("func _follow_target(delta: float):");
	p_file->store_line("\tvar target_position = target.global_position + camera_offset");
	p_file->store_line("\tglobal_position = global_position.lerp(target_position, follow_speed * delta)");
	p_file->store_line("\t");
	p_file->store_line("\t# Check if target is moving for head bob");
	p_file->store_line("\tif target.has_method(\"get_velocity\"):");
	p_file->store_line("\t\tvar velocity = target.get_velocity()");
	p_file->store_line("\t\tis_moving = velocity.length() > 0.1");
	p_file->store_line("\telif target.has_method(\"velocity\"):");
	p_file->store_line("\t\tis_moving = target.velocity.length() > 0.1");
	p_file->store_line("\telse:");
	p_file->store_line("\t\tis_moving = false");
	p_file->store_line("");
	p_file->store_line("func _handle_head_bob(delta: float):");
	p_file->store_line("\tif is_moving:");
	p_file->store_line("\t\thead_bob_time += delta * head_bob_frequency");
	p_file->store_line("\t\tvar bob_offset = Vector3(");
	p_file->store_line("\t\t\tsin(head_bob_time * 2) * head_bob_intensity * 0.5,");
	p_file->store_line("\t\t\tsin(head_bob_time) * head_bob_intensity,");
	p_file->store_line("\t\t\t0");
	p_file->store_line("\t\t)");
	p_file->store_line("\t\tposition += bob_offset");
	p_file->store_line("\telse:");
	p_file->store_line("\t\thead_bob_time = 0.0");
	p_file->store_line("");
	p_file->store_line("# Public methods");
	p_file->store_line("func set_target(new_target: Node3D):");
	p_file->store_line("\ttarget = new_target");
	p_file->store_line("\ttarget_changed.emit(new_target)");
	p_file->store_line("");
	p_file->store_line("func set_fov(new_fov: float):");
	p_file->store_line("\ttarget_fov = clamp(new_fov, min_fov, max_fov)");
	p_file->store_line("\tfov_changed.emit(target_fov)");
	p_file->store_line("");
	p_file->store_line("func toggle_mouse_capture():");
	p_file->store_line("\tmouse_captured = not mouse_captured");
	p_file->store_line("\tInput.mouse_mode = Input.MOUSE_MODE_CAPTURED if mouse_captured else Input.MOUSE_MODE_VISIBLE");
	p_file->store_line("");
	p_file->store_line("func get_current_fov() -> float:");
	p_file->store_line("\treturn current_fov");
	p_file->store_line("");
	p_file->store_line("func reset_to_default():");
	p_file->store_line("\ttarget_fov = default_fov");
	p_file->store_line("\trotation_x = 0.0");
	p_file->store_line("\trotation_y = 0.0");
	p_file->store_line("\ttarget_rotation_x = 0.0");
	p_file->store_line("\ttarget_rotation_y = 0.0");
	p_file->store_line("\thead_bob_time = 0.0");
}

void Camera3DFPSModule::generate_scene(Ref<FileAccess> p_file, const String &p_scene_name) {
	p_file->store_line("[gd_scene load_steps=2 format=3 uid=\"uid://camera_3d_fps\"]");
	p_file->store_line("");
	p_file->store_line("[ext_resource type=\"Script\" path=\"res://scripts/Camera3DFPS.gd\" id=\"1_camera_script\"]");
	p_file->store_line("");
	p_file->store_line("[node name=\"" + p_scene_name + "\" type=\"Camera3D\"]");
	p_file->store_line("script = ExtResource(\"1_camera_script\")");
	p_file->store_line("fov = 75.0");
}
