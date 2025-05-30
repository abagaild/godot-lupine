# PortraitDisplay.gd
# Generated by Lupine Engine - Portrait Display UI Script
# Manages the portrait display UI for the Character Portrait System

extends Control

# Portrait position references
@onready var left_portrait = $LeftPortrait
@onready var center_portrait = $CenterPortrait
@onready var right_portrait = $RightPortrait

func _ready():
	# Register this display with the Character Portrait System
	if CharacterPortraitSystem:
		CharacterPortraitSystem.register_portrait_display("left", left_portrait)
		CharacterPortraitSystem.register_portrait_display("center", center_portrait)
		CharacterPortraitSystem.register_portrait_display("right", right_portrait)

	print("Portrait Display UI initialized")

# Optional: Handle portrait visibility animations
func show_portrait(portrait_position: String):
	var portrait_node = get_portrait_node(portrait_position)
	if portrait_node:
		portrait_node.visible = true
		# Add fade-in animation if desired
		var tween = create_tween()
		portrait_node.modulate.a = 0.0
		tween.tween_property(portrait_node, "modulate:a", 1.0, 0.3)

func hide_portrait(portrait_position: String):
	var portrait_node = get_portrait_node(portrait_position)
	if portrait_node:
		# Add fade-out animation if desired
		var tween = create_tween()
		tween.tween_property(portrait_node, "modulate:a", 0.0, 0.3)
		tween.tween_callback(func(): portrait_node.visible = false)

func get_portrait_node(portrait_position: String) -> Control:
	match portrait_position:
		"left":
			return left_portrait
		"center":
			return center_portrait
		"right":
			return right_portrait
		_:
			return null
