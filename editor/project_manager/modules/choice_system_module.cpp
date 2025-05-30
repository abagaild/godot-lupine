#include "choice_system_module.h"

void ChoiceSystemModule::generate_script(Ref<FileAccess> p_file) {
	p_file->store_line("# ChoiceSystem.gd");
	p_file->store_line("# Generated by Lupine Engine - Choice System");
	p_file->store_line("# Branching dialogue choices with conditional options and history tracking");
	p_file->store_line("");
	p_file->store_line("extends Node");
	p_file->store_line("");
	p_file->store_line("# Signals");
	p_file->store_line("signal choices_presented(choices: Array)");
	p_file->store_line("signal choice_selected(choice_index: int, choice_text: String, target_node: String)");
	p_file->store_line("signal choices_hidden()");
	p_file->store_line("signal timed_choice_expired()");
	p_file->store_line("");
	p_file->store_line("# Current choice state");
	p_file->store_line("var current_choices: Array = []");
	p_file->store_line("var choice_menu: Control = null");
	p_file->store_line("var is_showing_choices: bool = false");
	p_file->store_line("");
	p_file->store_line("# Choice history and statistics");
	p_file->store_line("var choice_history: Array = []");
	p_file->store_line("var choice_statistics: Dictionary = {}");
	p_file->store_line("");
	p_file->store_line("# Timed choices");
	p_file->store_line("var choice_timer: float = 0.0");
	p_file->store_line("var choice_time_limit: float = 0.0");
	p_file->store_line("var default_choice_index: int = -1");
	p_file->store_line("var is_timed_choice: bool = false");
	p_file->store_line("");
	p_file->store_line("func _ready():");
	p_file->store_line("\t# Connect to VN Script Parser");
	p_file->store_line("\tif VNScriptParser:");
	p_file->store_line("\t\tVNScriptParser.choice_presented.connect(_on_choices_presented)");
	p_file->store_line("\t");
	p_file->store_line("\tprint(\"Choice System initialized\")");
	p_file->store_line("");
	p_file->store_line("func _process(delta):");
	p_file->store_line("\t# Handle timed choices");
	p_file->store_line("\tif is_timed_choice and is_showing_choices:");
	p_file->store_line("\t\tchoice_timer -= delta");
	p_file->store_line("\t\tif choice_timer <= 0.0:");
	p_file->store_line("\t\t\thandle_timed_choice_expiry()");
	p_file->store_line("");
	p_file->store_line("# Register choice menu UI");
	p_file->store_line("func register_choice_menu(menu: Control):");
	p_file->store_line("\tchoice_menu = menu");
	p_file->store_line("\tprint(\"Registered choice menu\")");
	p_file->store_line("");
	p_file->store_line("# Handle choices from script parser");
	p_file->store_line("func _on_choices_presented(choices: Array):");
	p_file->store_line("\tshow_choices(choices)");
	p_file->store_line("");
	p_file->store_line("# Show choices to player");
	p_file->store_line("func show_choices(choices: Array, time_limit: float = 0.0, default_choice: int = -1):");
	p_file->store_line("\tif choices.is_empty():");
	p_file->store_line("\t\treturn");
	p_file->store_line("\t");
	p_file->store_line("\tcurrent_choices = choices");
	p_file->store_line("\tis_showing_choices = true");
	p_file->store_line("\t");
	p_file->store_line("\t# Set up timed choice if specified");
	p_file->store_line("\tif time_limit > 0.0:");
	p_file->store_line("\t\tis_timed_choice = true");
	p_file->store_line("\t\tchoice_time_limit = time_limit");
	p_file->store_line("\t\tchoice_timer = time_limit");
	p_file->store_line("\t\tdefault_choice_index = default_choice");
	p_file->store_line("\telse:");
	p_file->store_line("\t\tis_timed_choice = false");
	p_file->store_line("\t");
	p_file->store_line("\t# Filter choices based on conditions");
	p_file->store_line("\tvar available_choices = filter_available_choices(choices)");
	p_file->store_line("\t");
	p_file->store_line("\t# Update choice menu");
	p_file->store_line("\tif choice_menu:");
	p_file->store_line("\t\tupdate_choice_menu(available_choices)");
	p_file->store_line("\t");
	p_file->store_line("\tchoices_presented.emit(available_choices)");
	p_file->store_line("");
	p_file->store_line("# Filter choices based on conditions");
	p_file->store_line("func filter_available_choices(choices: Array) -> Array:");
	p_file->store_line("\tvar available = []");
	p_file->store_line("\t");
	p_file->store_line("\tfor i in range(choices.size()):");
	p_file->store_line("\t\tvar choice = choices[i]");
	p_file->store_line("\t\t");
	p_file->store_line("\t\t# Check if choice has conditions");
	p_file->store_line("\t\tif choice.has(\"condition\") and not choice[\"condition\"].is_empty():");
	p_file->store_line("\t\t\tif VNScriptParser and not VNScriptParser.evaluate_condition(choice[\"condition\"]):");
	p_file->store_line("\t\t\t\tcontinue  # Skip this choice");
	p_file->store_line("\t\t");
	p_file->store_line("\t\t# Check if choice was already made (for one-time choices)");
	p_file->store_line("\t\tif choice.has(\"once\") and choice[\"once\"]:");
	p_file->store_line("\t\t\tvar choice_id = choice.get(\"id\", choice[\"text\"])");
	p_file->store_line("\t\t\tif was_choice_made(choice_id):");
	p_file->store_line("\t\t\t\tcontinue  # Skip this choice");
	p_file->store_line("\t\t");
	p_file->store_line("\t\t# Add choice with its original index");
	p_file->store_line("\t\tvar available_choice = choice.duplicate()");
	p_file->store_line("\t\tavailable_choice[\"original_index\"] = i");
	p_file->store_line("\t\tavailable.append(available_choice)");
	p_file->store_line("\t");
	p_file->store_line("\treturn available");
	p_file->store_line("");
	p_file->store_line("# Update choice menu UI");
	p_file->store_line("func update_choice_menu(choices: Array):");
	p_file->store_line("\tif not choice_menu:");
	p_file->store_line("\t\treturn");
	p_file->store_line("\t");
	p_file->store_line("\t# Call update method on choice menu");
	p_file->store_line("\tif choice_menu.has_method(\"update_choices\"):");
	p_file->store_line("\t\tchoice_menu.update_choices(choices, is_timed_choice, choice_timer)");
	p_file->store_line("");
	p_file->store_line("# Make a choice");
	p_file->store_line("func make_choice(choice_index: int):");
	p_file->store_line("\tif not is_showing_choices or choice_index < 0 or choice_index >= current_choices.size():");
	p_file->store_line("\t\treturn");
	p_file->store_line("\t");
	p_file->store_line("\tvar choice = current_choices[choice_index]");
	p_file->store_line("\tvar choice_text = choice[\"text\"]");
	p_file->store_line("\tvar target_node = choice[\"target\"]");
	p_file->store_line("\t");
	p_file->store_line("\t# Record choice in history");
	p_file->store_line("\trecord_choice(choice, choice_index)");
	p_file->store_line("\t");
	p_file->store_line("\t# Hide choices");
	p_file->store_line("\thide_choices()");
	p_file->store_line("\t");
	p_file->store_line("\t# Emit signal");
	p_file->store_line("\tchoice_selected.emit(choice_index, choice_text, target_node)");
	p_file->store_line("\t");
	p_file->store_line("\t# Continue script");
	p_file->store_line("\tif VNScriptParser:");
	p_file->store_line("\t\tVNScriptParser.make_choice(choice_index)");
	p_file->store_line("");
	p_file->store_line("# Hide choices");
	p_file->store_line("func hide_choices():");
	p_file->store_line("\tis_showing_choices = false");
	p_file->store_line("\tis_timed_choice = false");
	p_file->store_line("\tchoice_timer = 0.0");
	p_file->store_line("\tcurrent_choices.clear()");
	p_file->store_line("\t");
	p_file->store_line("\tif choice_menu and choice_menu.has_method(\"hide_choices\"):");
	p_file->store_line("\t\tchoice_menu.hide_choices()");
	p_file->store_line("\t");
	p_file->store_line("\tchoices_hidden.emit()");
	p_file->store_line("");
	p_file->store_line("# Handle timed choice expiry");
	p_file->store_line("func handle_timed_choice_expiry():");
	p_file->store_line("\tis_timed_choice = false");
	p_file->store_line("\t");
	p_file->store_line("\tif default_choice_index >= 0 and default_choice_index < current_choices.size():");
	p_file->store_line("\t\t# Make default choice");
	p_file->store_line("\t\tmake_choice(default_choice_index)");
	p_file->store_line("\telse:");
	p_file->store_line("\t\t# No default choice, just hide and continue");
	p_file->store_line("\t\thide_choices()");
	p_file->store_line("\t\tif VNScriptParser:");
	p_file->store_line("\t\t\tVNScriptParser.continue_script()");
	p_file->store_line("\t");
	p_file->store_line("\ttimed_choice_expired.emit()");
	p_file->store_line("");
	p_file->store_line("# Record choice in history");
	p_file->store_line("func record_choice(choice: Dictionary, choice_index: int):");
	p_file->store_line("\tvar choice_record = {");
	p_file->store_line("\t\t\"text\": choice[\"text\"],");
	p_file->store_line("\t\t\"target\": choice[\"target\"],");
	p_file->store_line("\t\t\"index\": choice_index,");
	p_file->store_line("\t\t\"timestamp\": Time.get_unix_time_from_system(),");
	p_file->store_line("\t\t\"node_id\": VNScriptParser.current_node_id if VNScriptParser else \"\"");
	p_file->store_line("\t}");
	p_file->store_line("\t");
	p_file->store_line("\t# Add choice ID if available");
	p_file->store_line("\tif choice.has(\"id\"):");
	p_file->store_line("\t\tchoice_record[\"id\"] = choice[\"id\"]");
	p_file->store_line("\t");
	p_file->store_line("\tchoice_history.append(choice_record)");
	p_file->store_line("\t");
	p_file->store_line("\t# Update statistics");
	p_file->store_line("\tvar choice_id = choice.get(\"id\", choice[\"text\"])");
	p_file->store_line("\tif not choice_statistics.has(choice_id):");
	p_file->store_line("\t\tchoice_statistics[choice_id] = 0");
	p_file->store_line("\tchoice_statistics[choice_id] += 1");
	p_file->store_line("");
	p_file->store_line("# Check if a choice was made before");
	p_file->store_line("func was_choice_made(choice_id: String) -> bool:");
	p_file->store_line("\tfor record in choice_history:");
	p_file->store_line("\t\tif record.get(\"id\", record[\"text\"]) == choice_id:");
	p_file->store_line("\t\t\treturn true");
	p_file->store_line("\treturn false");
	p_file->store_line("");
	p_file->store_line("# Get choice history");
	p_file->store_line("func get_choice_history() -> Array:");
	p_file->store_line("\treturn choice_history.duplicate()");
	p_file->store_line("");
	p_file->store_line("# Get choice statistics");
	p_file->store_line("func get_choice_statistics() -> Dictionary:");
	p_file->store_line("\treturn choice_statistics.duplicate()");
	p_file->store_line("");
	p_file->store_line("# Clear choice history");
	p_file->store_line("func clear_choice_history():");
	p_file->store_line("\tchoice_history.clear()");
	p_file->store_line("\tchoice_statistics.clear()");
	p_file->store_line("");
	p_file->store_line("# Get choices made in current session");
	p_file->store_line("func get_session_choices() -> Array:");
	p_file->store_line("\tvar session_start = Time.get_unix_time_from_system() - 3600  # Last hour");
	p_file->store_line("\tvar session_choices = []");
	p_file->store_line("\t");
	p_file->store_line("\tfor record in choice_history:");
	p_file->store_line("\t\tif record[\"timestamp\"] >= session_start:");
	p_file->store_line("\t\t\tsession_choices.append(record)");
	p_file->store_line("\t");
	p_file->store_line("\treturn session_choices");
	p_file->store_line("");
	p_file->store_line("# Undo last choice (if possible)");
	p_file->store_line("func undo_last_choice() -> bool:");
	p_file->store_line("\tif choice_history.is_empty():");
	p_file->store_line("\t\treturn false");
	p_file->store_line("\t");
	p_file->store_line("\tvar last_choice = choice_history[-1]");
	p_file->store_line("\tvar node_id = last_choice.get(\"node_id\", \"\")");
	p_file->store_line("\t");
	p_file->store_line("\tif node_id.is_empty() or not VNScriptParser:");
	p_file->store_line("\t\treturn false");
	p_file->store_line("\t");
	p_file->store_line("\t# Remove from history");
	p_file->store_line("\tchoice_history.pop_back()");
	p_file->store_line("\t");
	p_file->store_line("\t# Update statistics");
	p_file->store_line("\tvar choice_id = last_choice.get(\"id\", last_choice[\"text\"])");
	p_file->store_line("\tif choice_statistics.has(choice_id):");
	p_file->store_line("\t\tchoice_statistics[choice_id] -= 1");
	p_file->store_line("\t\tif choice_statistics[choice_id] <= 0:");
	p_file->store_line("\t\t\tchoice_statistics.erase(choice_id)");
	p_file->store_line("\t");
	p_file->store_line("\t# Return to previous node");
	p_file->store_line("\tVNScriptParser.current_node_id = node_id");
	p_file->store_line("\tVNScriptParser.execute_current_node()");
	p_file->store_line("\t");
	p_file->store_line("\treturn true");
	p_file->store_line("");
	p_file->store_line("# Get current choice state");
	p_file->store_line("func is_choice_active() -> bool:");
	p_file->store_line("\treturn is_showing_choices");
	p_file->store_line("");
	p_file->store_line("func get_current_choices() -> Array:");
	p_file->store_line("\treturn current_choices.duplicate()");
	p_file->store_line("");
	p_file->store_line("func get_remaining_time() -> float:");
	p_file->store_line("\treturn choice_timer if is_timed_choice else 0.0");
}

void ChoiceSystemModule::generate_file(Ref<FileAccess> p_file, const String &p_relative_path) {
	String filename = p_relative_path.get_file();

	if (filename == "ChoiceMenu.gd") {
		// Generate the UI script for choice menu
		p_file->store_line("# ChoiceMenu.gd");
		p_file->store_line("# Generated by Lupine Engine - Choice Menu UI Script");
		p_file->store_line("# Manages the choice display UI for the Choice System");
		p_file->store_line("");
		p_file->store_line("extends Control");
		p_file->store_line("");
		p_file->store_line("# UI references");
		p_file->store_line("@onready var choice_container = $ChoiceContainer");
		p_file->store_line("@onready var timer_label = $TimerLabel");
		p_file->store_line("");
		p_file->store_line("# Current choices");
		p_file->store_line("var current_choices: Array = []");
		p_file->store_line("var choice_buttons: Array = []");
		p_file->store_line("");
		p_file->store_line("func _ready():");
		p_file->store_line("\t# Register this menu with the Choice System");
		p_file->store_line("\tif ChoiceSystem:");
		p_file->store_line("\t\tChoiceSystem.register_choice_menu(self)");
		p_file->store_line("\t");
		p_file->store_line("\t# Hide initially");
		p_file->store_line("\tvisible = false");
		p_file->store_line("\tprint(\"Choice Menu UI initialized\")");
		p_file->store_line("");
		p_file->store_line("func _process(delta):");
		p_file->store_line("\t# Update timer display if showing timed choices");
		p_file->store_line("\tif ChoiceSystem and ChoiceSystem.is_choice_active():");
		p_file->store_line("\t\tvar remaining_time = ChoiceSystem.get_remaining_time()");
		p_file->store_line("\t\tif remaining_time > 0:");
		p_file->store_line("\t\t\ttimer_label.text = \"Time: \" + str(int(remaining_time + 1))");
		p_file->store_line("\t\t\ttimer_label.visible = true");
		p_file->store_line("\t\telse:");
		p_file->store_line("\t\t\ttimer_label.visible = false");
		p_file->store_line("\telse:");
		p_file->store_line("\t\ttimer_label.visible = false");
		p_file->store_line("");
		p_file->store_line("# Called by ChoiceSystem to update choices");
		p_file->store_line("func update_choices(choices: Array, is_timed: bool = false, time_limit: float = 0.0):");
		p_file->store_line("\tcurrent_choices = choices");
		p_file->store_line("\t");
		p_file->store_line("\t# Clear existing buttons");
		p_file->store_line("\tclear_choice_buttons()");
		p_file->store_line("\t");
		p_file->store_line("\t# Create new choice buttons");
		p_file->store_line("\tfor i in range(choices.size()):");
		p_file->store_line("\t\tvar choice = choices[i]");
		p_file->store_line("\t\tvar button = create_choice_button(choice[\"text\"], i)");
		p_file->store_line("\t\tchoice_buttons.append(button)");
		p_file->store_line("\t\tchoice_container.add_child(button)");
		p_file->store_line("\t");
		p_file->store_line("\t# Show the menu");
		p_file->store_line("\tvisible = true");
		p_file->store_line("\t");
		p_file->store_line("\t# Set up timer if timed choice");
		p_file->store_line("\tif is_timed:");
		p_file->store_line("\t\ttimer_label.visible = true");
		p_file->store_line("\t\ttimer_label.text = \"Time: \" + str(int(time_limit))");
		p_file->store_line("\telse:");
		p_file->store_line("\t\ttimer_label.visible = false");
		p_file->store_line("");
		p_file->store_line("# Create a choice button");
		p_file->store_line("func create_choice_button(text: String, index: int) -> Button:");
		p_file->store_line("\tvar button = Button.new()");
		p_file->store_line("\tbutton.text = text");
		p_file->store_line("\tbutton.size_flags_horizontal = Control.SIZE_EXPAND_FILL");
		p_file->store_line("\tbutton.custom_minimum_size.y = 40");
		p_file->store_line("\t");
		p_file->store_line("\t# Connect button press");
		p_file->store_line("\tbutton.pressed.connect(_on_choice_selected.bind(index))");
		p_file->store_line("\t");
		p_file->store_line("\treturn button");
		p_file->store_line("");
		p_file->store_line("# Handle choice selection");
		p_file->store_line("func _on_choice_selected(choice_index: int):");
		p_file->store_line("\tif ChoiceSystem:");
		p_file->store_line("\t\tChoiceSystem.make_choice(choice_index)");
		p_file->store_line("");
		p_file->store_line("# Called by ChoiceSystem to hide choices");
		p_file->store_line("func hide_choices():");
		p_file->store_line("\tvisible = false");
		p_file->store_line("\tclear_choice_buttons()");
		p_file->store_line("");
		p_file->store_line("# Clear all choice buttons");
		p_file->store_line("func clear_choice_buttons():");
		p_file->store_line("\tfor button in choice_buttons:");
		p_file->store_line("\t\tif button and is_instance_valid(button):");
		p_file->store_line("\t\t\tbutton.queue_free()");
		p_file->store_line("\tchoice_buttons.clear()");
		p_file->store_line("\t");
		p_file->store_line("\t# Clear container children");
		p_file->store_line("\tfor child in choice_container.get_children():");
		p_file->store_line("\t\tchild.queue_free()");
		p_file->store_line("");
		p_file->store_line("# Handle keyboard input for choice selection");
		p_file->store_line("func _input(event):");
		p_file->store_line("\tif not visible or current_choices.is_empty():");
		p_file->store_line("\t\treturn");
		p_file->store_line("\t");
		p_file->store_line("\tif event is InputEventKey and event.pressed:");
		p_file->store_line("\t\t# Number keys 1-9 for quick choice selection");
		p_file->store_line("\t\tif event.keycode >= KEY_1 and event.keycode <= KEY_9:");
		p_file->store_line("\t\t\tvar choice_index = event.keycode - KEY_1");
		p_file->store_line("\t\t\tif choice_index < current_choices.size():");
		p_file->store_line("\t\t\t\t_on_choice_selected(choice_index)");
		p_file->store_line("\t\t\t\tget_viewport().set_input_as_handled()");
	} else {
		// Use default file generation
		LupineModuleBase::generate_file(p_file, p_relative_path);
	}
}

void ChoiceSystemModule::generate_scene(Ref<FileAccess> p_file, const String &p_scene_name) {
	p_file->store_line("[gd_scene load_steps=2 format=3 uid=\"uid://choice_menu\"]");
	p_file->store_line("");
	p_file->store_line("[ext_resource type=\"Script\" path=\"res://scripts/ui/ChoiceMenu.gd\" id=\"1_choice_script\"]");
	p_file->store_line("");
	p_file->store_line("[node name=\"ChoiceMenu\" type=\"Control\"]");
	p_file->store_line("layout_mode = 3");
	p_file->store_line("anchors_preset = 15");
	p_file->store_line("anchor_right = 1.0");
	p_file->store_line("anchor_bottom = 1.0");
	p_file->store_line("script = ExtResource(\"1_choice_script\")");
	p_file->store_line("");
	p_file->store_line("[node name=\"ChoiceContainer\" type=\"VBoxContainer\" parent=\".\"]");
	p_file->store_line("layout_mode = 1");
	p_file->store_line("anchors_preset = 7");
	p_file->store_line("anchor_left = 0.5");
	p_file->store_line("anchor_top = 1.0");
	p_file->store_line("anchor_right = 0.5");
	p_file->store_line("anchor_bottom = 1.0");
	p_file->store_line("offset_left = -200.0");
	p_file->store_line("offset_top = -150.0");
	p_file->store_line("offset_right = 200.0");
	p_file->store_line("offset_bottom = -50.0");
	p_file->store_line("");
	p_file->store_line("[node name=\"TimerLabel\" type=\"Label\" parent=\".\"]");
	p_file->store_line("layout_mode = 1");
	p_file->store_line("anchors_preset = 1");
	p_file->store_line("anchor_left = 1.0");
	p_file->store_line("anchor_right = 1.0");
	p_file->store_line("offset_left = -100.0");
	p_file->store_line("offset_top = 10.0");
	p_file->store_line("offset_right = -10.0");
	p_file->store_line("offset_bottom = 40.0");
	p_file->store_line("text = \"Time: 10\"");
	p_file->store_line("horizontal_alignment = 2");
	p_file->store_line("visible = false");
}
