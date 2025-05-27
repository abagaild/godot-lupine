/**************************************************************************/
/*  project_dialog.cpp                                                    */
/**************************************************************************/
/*                         This file is part of:                          */
/*                             GODOT ENGINE                               */
/*                        https://godotengine.org                         */
/**************************************************************************/
/* Copyright (c) 2014-present Godot Engine contributors (see AUTHORS.md). */
/* Copyright (c) 2007-2014 Juan Linietsky, Ariel Manzur.                  */
/*                                                                        */
/* Permission is hereby granted, free of charge, to any person obtaining  */
/* a copy of this software and associated documentation files (the        */
/* "Software"), to deal in the Software without restriction, including    */
/* without limitation the rights to use, copy, modify, merge, publish,    */
/* distribute, sublicense, and/or sell copies of the Software, and to     */
/* permit persons to whom the Software is furnished to do so, subject to  */
/* the following conditions:                                              */
/*                                                                        */
/* The above copyright notice and this permission notice shall be         */
/* included in all copies or substantial portions of the Software.        */
/*                                                                        */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,        */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF     */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. */
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY   */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,   */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE      */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                 */
/**************************************************************************/

#include "project_dialog.h"

#include "core/config/project_settings.h"
#include "modules/lupine_module_manager.h"
#include "core/io/dir_access.h"
#include "core/io/zip_io.h"
#include "core/version.h"
#include "editor/editor_settings.h"
#include "editor/editor_string_names.h"
#include "editor/editor_vcs_interface.h"
#include "editor/gui/editor_file_dialog.h"
#include "editor/themes/editor_icons.h"
#include "editor/themes/editor_scale.h"
#include "scene/gui/check_box.h"
#include "scene/gui/check_button.h"
#include "scene/gui/line_edit.h"
#include "scene/gui/option_button.h"
#include "scene/gui/separator.h"
#include "scene/gui/texture_rect.h"
#include "scene/gui/grid_container.h"
#include "scene/gui/rich_text_label.h"
#include "scene/gui/scroll_container.h"
#include "scene/gui/box_container.h"
#include "scene/gui/label.h"

void ProjectDialog::_set_message(const String &p_msg, MessageType p_type, InputType p_input_type) {
	msg->set_text(p_msg);
	get_ok_button()->set_disabled(p_type == MESSAGE_ERROR);

	Ref<Texture2D> new_icon;
	switch (p_type) {
		case MESSAGE_ERROR: {
			msg->add_theme_color_override(SceneStringName(font_color), get_theme_color(SNAME("error_color"), EditorStringName(Editor)));
			new_icon = get_editor_theme_icon(SNAME("StatusError"));
		} break;
		case MESSAGE_WARNING: {
			msg->add_theme_color_override(SceneStringName(font_color), get_theme_color(SNAME("warning_color"), EditorStringName(Editor)));
			new_icon = get_editor_theme_icon(SNAME("StatusWarning"));
		} break;
		case MESSAGE_SUCCESS: {
			msg->add_theme_color_override(SceneStringName(font_color), get_theme_color(SNAME("success_color"), EditorStringName(Editor)));
			new_icon = get_editor_theme_icon(SNAME("StatusSuccess"));
		} break;
	}

	if (p_input_type == PROJECT_PATH) {
		project_status_rect->set_texture(new_icon);
	} else if (p_input_type == INSTALL_PATH) {
		install_status_rect->set_texture(new_icon);
	}
}

static bool is_zip_file(Ref<DirAccess> p_d, const String &p_path) {
	return p_path.get_extension() == "zip" && p_d->file_exists(p_path);
}

void ProjectDialog::_validate_path() {
	_set_message("", MESSAGE_SUCCESS, PROJECT_PATH);
	_set_message("", MESSAGE_SUCCESS, INSTALL_PATH);

	if (project_name->get_text().strip_edges().is_empty()) {
		_set_message(TTRC("It would be a good idea to name your project."), MESSAGE_ERROR);
		return;
	}

	Ref<DirAccess> d = DirAccess::create(DirAccess::ACCESS_FILESYSTEM);
	String path = project_path->get_text().simplify_path();

	String target_path = path;
	InputType target_path_input_type = PROJECT_PATH;

	if (mode == MODE_IMPORT) {
		if (path.get_file().strip_edges() == "project.godot") {
			path = path.get_base_dir();
			project_path->set_text(path);
		}

		if (is_zip_file(d, path)) {
			zip_path = path;
		} else if (is_zip_file(d, path.strip_edges())) {
			zip_path = path.strip_edges();
		} else {
			zip_path = "";
		}

		if (!zip_path.is_empty()) {
			target_path = install_path->get_text().simplify_path();
			target_path_input_type = INSTALL_PATH;

			create_dir->show();
			install_path_container->show();

			Ref<FileAccess> io_fa;
			zlib_filefunc_def io = zipio_create_io(&io_fa);

			unzFile pkg = unzOpen2(zip_path.utf8().get_data(), &io);
			if (!pkg) {
				_set_message(TTRC("Invalid \".zip\" project file; it is not in ZIP format."), MESSAGE_ERROR);
				unzClose(pkg);
				return;
			}

			int ret = unzGoToFirstFile(pkg);
			while (ret == UNZ_OK) {
				unz_file_info info;
				char fname[16384];
				ret = unzGetCurrentFileInfo(pkg, &info, fname, 16384, nullptr, 0, nullptr, 0);
				ERR_FAIL_COND_MSG(ret != UNZ_OK, "Failed to get current file info.");

				String name = String::utf8(fname);

				// Skip the __MACOSX directory created by macOS's built-in file zipper.
				if (name.begins_with("__MACOSX")) {
					ret = unzGoToNextFile(pkg);
					continue;
				}

				if (name.get_file() == "project.godot") {
					break; // ret == UNZ_OK.
				}

				ret = unzGoToNextFile(pkg);
			}

			if (ret == UNZ_END_OF_LIST_OF_FILE) {
				_set_message(TTRC("Invalid \".zip\" project file; it doesn't contain a \"project.godot\" file."), MESSAGE_ERROR);
				unzClose(pkg);
				return;
			}

			unzClose(pkg);
		} else if (d->dir_exists(path) && d->file_exists(path.path_join("project.godot"))) {
			zip_path = "";

			create_dir->hide();
			install_path_container->hide();

			_set_message(TTRC("Valid project found at path."), MESSAGE_SUCCESS);
		} else {
			create_dir->hide();
			install_path_container->hide();

			_set_message(TTRC("Please choose a \"project.godot\", a directory with one, or a \".zip\" file."), MESSAGE_ERROR);
			return;
		}
	}

	if (target_path.is_relative_path()) {
		_set_message(TTRC("The path specified is invalid."), MESSAGE_ERROR, target_path_input_type);
		return;
	}

	if (target_path.get_file() != OS::get_singleton()->get_safe_dir_name(target_path.get_file())) {
		_set_message(TTRC("The directory name specified contains invalid characters or trailing whitespace."), MESSAGE_ERROR, target_path_input_type);
		return;
	}

	String working_dir = d->get_current_dir();
	String executable_dir = OS::get_singleton()->get_executable_path().get_base_dir();
	if (target_path == working_dir || target_path == executable_dir) {
		_set_message(TTRC("Creating a project at the engine's working directory or executable directory is not allowed, as it would prevent the project manager from starting."), MESSAGE_ERROR, target_path_input_type);
		return;
	}

	// TODO: The following 5 lines could be simplified if OS.get_user_home_dir() or SYSTEM_DIR_HOME is implemented. See: https://github.com/godotengine/godot-proposals/issues/4851.
#ifdef WINDOWS_ENABLED
	String home_dir = OS::get_singleton()->get_environment("USERPROFILE");
#else
	String home_dir = OS::get_singleton()->get_environment("HOME");
#endif
	String documents_dir = OS::get_singleton()->get_system_dir(OS::SYSTEM_DIR_DOCUMENTS);
	if (target_path == home_dir || target_path == documents_dir) {
		_set_message(TTRC("You cannot save a project at the selected path. Please create a subfolder or choose a new path."), MESSAGE_ERROR, target_path_input_type);
		return;
	}

	is_folder_empty = true;
	if (mode == MODE_NEW || mode == MODE_INSTALL || mode == MODE_DUPLICATE || (mode == MODE_IMPORT && target_path_input_type == InputType::INSTALL_PATH)) {
		if (create_dir->is_pressed()) {
			if (!d->dir_exists(target_path.get_base_dir())) {
				_set_message(TTRC("The parent directory of the path specified doesn't exist."), MESSAGE_ERROR, target_path_input_type);
				return;
			}

			if (d->dir_exists(target_path)) {
				// The path is not necessarily empty here, but we will update the message later if it isn't.
				_set_message(TTRC("The project folder already exists and is empty."), MESSAGE_SUCCESS, target_path_input_type);
			} else {
				_set_message(TTRC("The project folder will be automatically created."), MESSAGE_SUCCESS, target_path_input_type);
			}
		} else {
			if (!d->dir_exists(target_path)) {
				_set_message(TTRC("The path specified doesn't exist."), MESSAGE_ERROR, target_path_input_type);
				return;
			}

			// The path is not necessarily empty here, but we will update the message later if it isn't.
			_set_message(TTRC("The project folder exists and is empty."), MESSAGE_SUCCESS, target_path_input_type);
		}

		// Check if the directory is empty. Not an error, but we want to warn the user.
		if (d->change_dir(target_path) == OK) {
			d->list_dir_begin();
			String n = d->get_next();
			while (!n.is_empty()) {
				if (n[0] != '.') {
					// Allow `.`, `..` (reserved current/parent folder names)
					// and hidden files/folders to be present.
					// For instance, this lets users initialize a Git repository
					// and still be able to create a project in the directory afterwards.
					is_folder_empty = false;
					break;
				}
				n = d->get_next();
			}
			d->list_dir_end();

			if (!is_folder_empty) {
				_set_message(TTRC("The selected path is not empty. Choosing an empty folder is highly recommended."), MESSAGE_WARNING, target_path_input_type);
			}
		}
	}
}

void ProjectDialog::_initialize_modules() {
	// Use the module manager to get available modules
	available_modules = module_manager->get_available_modules();
}

void ProjectDialog::_setup_template_ui() {
	// Template container is already created and added to VBox in constructor

	template_selection_label = memnew(Label);
	template_selection_label->set_text(TTRC("Project Template:"));
	template_container->add_child(template_selection_label);

	// Create a grid for template selection with modern card-style layout
	template_grid = memnew(GridContainer);
	template_grid->set_columns(2); // 2 columns for template cards
	template_grid->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	template_container->add_child(template_grid);

	// Template description area
	template_description = memnew(RichTextLabel);
	template_description->set_custom_minimum_size(Size2(0, 80) * EDSCALE);
	template_description->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	template_description->set_v_size_flags(Control::SIZE_SHRINK_CENTER);
	template_description->set_use_bbcode(true);
	template_description->set_text("[color=gray]Select a template to see its description[/color]");
	template_container->add_child(template_description);

	// Populate template grid with buttons
	for (const ProjectTemplate &tmpl : available_templates) {
		Button *template_btn = memnew(Button);
		template_btn->set_text(tmpl.name);
		template_btn->set_custom_minimum_size(Size2(200, 60) * EDSCALE);
		template_btn->set_h_size_flags(Control::SIZE_EXPAND_FILL);
		template_btn->set_text_alignment(HORIZONTAL_ALIGNMENT_CENTER);
		template_btn->set_clip_contents(true);
		template_btn->set_tooltip_text(tmpl.description);
		template_btn->connect(SceneStringName(pressed), callable_mp(this, &ProjectDialog::_template_selected).bind(tmpl.id));
		template_grid->add_child(template_btn);
	}

	// Template container will be shown/hidden based on mode
	template_container->set_visible(true);
}

void ProjectDialog::_setup_module_ui() {
	// Module container is already created and added to VBox in constructor

	module_selection_label = memnew(Label);
	module_selection_label->set_text(TTRC("Additional Modules:"));
	module_container->add_child(module_selection_label);

	// Scrollable module list
	module_scroll = memnew(ScrollContainer);
	module_scroll->set_custom_minimum_size(Size2(0, 200) * EDSCALE);
	module_scroll->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	module_scroll->set_v_size_flags(Control::SIZE_EXPAND_FILL);
	module_container->add_child(module_scroll);

	module_list = memnew(VBoxContainer);
	module_scroll->add_child(module_list);

	// Group modules by category
	HashMap<String, VBoxContainer*> category_containers;

	for (const ProjectModule &mod : available_modules) {
		if (!category_containers.has(mod.category)) {
			// Create category header
			Label *category_label = memnew(Label);
			category_label->set_text(mod.category);
			// Skip theme style override for now to avoid potential issues
			category_label->set_h_size_flags(Control::SIZE_EXPAND_FILL);
			module_list->add_child(category_label);

			VBoxContainer *category_vbox = memnew(VBoxContainer);
			module_list->add_child(category_vbox);
			category_containers[mod.category] = category_vbox;
		}

		VBoxContainer *category_vbox = category_containers[mod.category];

		CheckBox *module_checkbox = memnew(CheckBox);
		module_checkbox->set_text(mod.name);
		module_checkbox->set_tooltip_text(mod.description);
		module_checkbox->set_toggle_mode(true);
		// Store module ID as metadata for proper identification
		module_checkbox->set_meta("module_id", mod.id);
		module_checkbox->connect("toggled", callable_mp(this, &ProjectDialog::_module_toggled).bind(mod.id));
		category_vbox->add_child(module_checkbox);
	}

	// Module container will be shown/hidden based on mode
	module_container->set_visible(true);
}

void ProjectDialog::_template_selected(const String &p_template_id) {
	selected_template_id = p_template_id;

	// Update visual selection
	for (int i = 0; i < template_grid->get_child_count(); i++) {
		Button *btn = Object::cast_to<Button>(template_grid->get_child(i));
		if (btn) {
			btn->set_pressed(false);
		}
	}

	// Find and highlight selected template
	for (int i = 0; i < template_grid->get_child_count(); i++) {
		Button *btn = Object::cast_to<Button>(template_grid->get_child(i));
		if (btn) {
			// Check if this button corresponds to the selected template
			for (const ProjectTemplate &tmpl : available_templates) {
				if (tmpl.id == p_template_id && btn->get_text() == tmpl.name) {
					btn->set_pressed(true);
					break;
				}
			}
		}
	}

	_update_template_description();
	_update_module_dependencies();
}

void ProjectDialog::_module_toggled(const String &p_module_id, bool p_enabled) {
	if (p_enabled) {
		if (!selected_modules.has(p_module_id)) {
			selected_modules.push_back(p_module_id);
		}
	} else {
		selected_modules.erase(p_module_id);
	}

	_update_module_dependencies();
}

void ProjectDialog::_update_template_description() {
	if (selected_template_id.is_empty()) {
		template_description->set_text("[color=gray]Select a template to see its description[/color]");
		return;
	}

	for (const ProjectTemplate &tmpl : available_templates) {
		if (tmpl.id == selected_template_id) {
			String desc_text = "[b]" + tmpl.name + "[/b]\n";
			desc_text += "[color=lightblue]Category: " + tmpl.category + "[/color]\n\n";
			desc_text += tmpl.description;

			if (!tmpl.default_modules.is_empty()) {
				desc_text += "\n\n[color=yellow]Includes modules:[/color]";
				for (const String &module_id : tmpl.default_modules) {
					for (const ProjectModule &mod : available_modules) {
						if (mod.id == module_id) {
							desc_text += "\n• " + mod.name;
							break;
						}
					}
				}
			}

			template_description->set_text(desc_text);
			break;
		}
	}
}

void ProjectDialog::_update_module_dependencies() {
	// Auto-select default modules for the selected template
	if (!selected_template_id.is_empty()) {
		for (const ProjectTemplate &tmpl : available_templates) {
			if (tmpl.id == selected_template_id) {
				for (const String &module_id : tmpl.default_modules) {
					if (!selected_modules.has(module_id)) {
						selected_modules.push_back(module_id);
					}
				}
				break;
			}
		}
	}

	// Update checkbox states
	for (int i = 0; i < module_list->get_child_count(); i++) {
		Node *child = module_list->get_child(i);
		VBoxContainer *category_vbox = Object::cast_to<VBoxContainer>(child);
		if (category_vbox) {
			for (int j = 0; j < category_vbox->get_child_count(); j++) {
				CheckBox *checkbox = Object::cast_to<CheckBox>(category_vbox->get_child(j));
				if (checkbox && checkbox->has_meta("module_id")) {
					String module_id = checkbox->get_meta("module_id");
					checkbox->set_pressed(selected_modules.has(module_id));

					// Disable if it's a default module for the selected template
					bool is_default = false;
					if (!selected_template_id.is_empty()) {
						for (const ProjectTemplate &tmpl : available_templates) {
							if (tmpl.id == selected_template_id && tmpl.default_modules.has(module_id)) {
								is_default = true;
								break;
							}
						}
					}
					checkbox->set_disabled(is_default);
				}
			}
		}
	}
}

String ProjectDialog::_get_target_path() {
	if (mode == MODE_NEW || mode == MODE_INSTALL || mode == MODE_DUPLICATE) {
		return project_path->get_text();
	} else if (mode == MODE_IMPORT) {
		return install_path->get_text();
	} else {
		ERR_FAIL_V("");
	}
}
void ProjectDialog::_set_target_path(const String &p_text) {
	if (mode == MODE_NEW || mode == MODE_INSTALL || mode == MODE_DUPLICATE) {
		project_path->set_text(p_text);
	} else if (mode == MODE_IMPORT) {
		install_path->set_text(p_text);
	} else {
		ERR_FAIL();
	}
}

void ProjectDialog::_update_target_auto_dir() {
	String new_auto_dir;
	if (mode == MODE_NEW || mode == MODE_INSTALL || mode == MODE_DUPLICATE) {
		new_auto_dir = project_name->get_text();
	} else if (mode == MODE_IMPORT) {
		new_auto_dir = project_path->get_text().get_file().get_basename();
	}
	int naming_convention = (int)EDITOR_GET("project_manager/directory_naming_convention");
	switch (naming_convention) {
		case 0: // No convention
			break;
		case 1: // kebab-case
			new_auto_dir = new_auto_dir.to_kebab_case();
			break;
		case 2: // snake_case
			new_auto_dir = new_auto_dir.to_snake_case();
			break;
		case 3: // camelCase
			new_auto_dir = new_auto_dir.to_camel_case();
			break;
		case 4: // PascalCase
			new_auto_dir = new_auto_dir.to_pascal_case();
			break;
		case 5: // Title Case
			new_auto_dir = new_auto_dir.capitalize();
			break;
		default:
			ERR_FAIL_MSG("Invalid directory naming convention.");
			break;
	}
	new_auto_dir = OS::get_singleton()->get_safe_dir_name(new_auto_dir);

	if (create_dir->is_pressed()) {
		String target_path = _get_target_path();

		if (target_path.get_file() == auto_dir) {
			// Update target dir name to new project name / ZIP name.
			target_path = target_path.get_base_dir().path_join(new_auto_dir);
		}

		_set_target_path(target_path);
	}

	auto_dir = new_auto_dir;
}

void ProjectDialog::_create_dir_toggled(bool p_pressed) {
	String target_path = _get_target_path();

	if (create_dir->is_pressed()) {
		// (Re-)append target dir name.
		if (last_custom_target_dir.is_empty()) {
			target_path = target_path.path_join(auto_dir);
		} else {
			target_path = target_path.path_join(last_custom_target_dir);
		}
	} else {
		// Strip any trailing slash.
		target_path = target_path.rstrip("/\\");
		// Save and remove target dir name.
		if (target_path.get_file() == auto_dir) {
			last_custom_target_dir = "";
		} else {
			last_custom_target_dir = target_path.get_file();
		}
		target_path = target_path.get_base_dir();
	}

	_set_target_path(target_path);
	_validate_path();
}

void ProjectDialog::_project_name_changed() {
	if (mode == MODE_NEW || mode == MODE_INSTALL || mode == MODE_DUPLICATE) {
		_update_target_auto_dir();
	}

	_validate_path();
}

void ProjectDialog::_project_path_changed() {
	if (mode == MODE_IMPORT) {
		_update_target_auto_dir();
	}

	_validate_path();
}

void ProjectDialog::_install_path_changed() {
	_validate_path();
}

void ProjectDialog::_browse_project_path() {
	String path = project_path->get_text();
	if (path.is_relative_path()) {
		path = EDITOR_GET("filesystem/directories/default_project_path");
	}
	if (mode == MODE_IMPORT && install_path->is_visible_in_tree()) {
		// Select last ZIP file.
		fdialog_project->set_current_path(path);
	} else if ((mode == MODE_NEW || mode == MODE_INSTALL || mode == MODE_DUPLICATE) && create_dir->is_pressed()) {
		// Select parent directory of project path.
		fdialog_project->set_current_dir(path.get_base_dir());
	} else {
		// Select project path.
		fdialog_project->set_current_dir(path);
	}

	if (mode == MODE_IMPORT) {
		fdialog_project->set_file_mode(EditorFileDialog::FILE_MODE_OPEN_ANY);
		fdialog_project->clear_filters();
		fdialog_project->add_filter("project.godot", vformat("%s %s", GODOT_VERSION_NAME, TTR("Project")));
		fdialog_project->add_filter("*.zip", TTR("ZIP File"));
	} else {
		fdialog_project->set_file_mode(EditorFileDialog::FILE_MODE_OPEN_DIR);
	}

	hide();
	fdialog_project->popup_file_dialog();
}

void ProjectDialog::_browse_install_path() {
	ERR_FAIL_COND_MSG(mode != MODE_IMPORT, "Install path is only used for MODE_IMPORT.");

	String path = install_path->get_text();
	if (path.is_relative_path() || !DirAccess::dir_exists_absolute(path)) {
		path = EDITOR_GET("filesystem/directories/default_project_path");
	}
	if (create_dir->is_pressed()) {
		// Select parent directory of install path.
		fdialog_install->set_current_dir(path.get_base_dir());
	} else {
		// Select install path.
		fdialog_install->set_current_dir(path);
	}

	fdialog_install->set_file_mode(EditorFileDialog::FILE_MODE_OPEN_DIR);
	fdialog_install->popup_file_dialog();
}

void ProjectDialog::_project_path_selected(const String &p_path) {
	show_dialog(false);

	if (create_dir->is_pressed() && (mode == MODE_NEW || mode == MODE_INSTALL || mode == MODE_DUPLICATE)) {
		// Replace parent directory, but keep target dir name.
		project_path->set_text(p_path.path_join(project_path->get_text().get_file()));
	} else {
		project_path->set_text(p_path);
	}

	_project_path_changed();

	if (install_path->is_visible_in_tree()) {
		// ZIP is selected; focus install path.
		install_path->grab_focus();
	} else {
		get_ok_button()->grab_focus();
	}
}

void ProjectDialog::_install_path_selected(const String &p_path) {
	ERR_FAIL_COND_MSG(mode != MODE_IMPORT, "Install path is only used for MODE_IMPORT.");

	if (create_dir->is_pressed()) {
		// Replace parent directory, but keep target dir name.
		install_path->set_text(p_path.path_join(install_path->get_text().get_file()));
	} else {
		install_path->set_text(p_path);
	}

	_install_path_changed();

	get_ok_button()->grab_focus();
}

void ProjectDialog::_reset_name() {
	project_name->set_text(TTR("New Game Project"));
}

void ProjectDialog::_renderer_selected() {
	ERR_FAIL_NULL(renderer_button_group->get_pressed_button());

	String renderer_type = renderer_button_group->get_pressed_button()->get_meta(SNAME("rendering_method"));

	bool rd_error = false;

	if (renderer_type == "forward_plus") {
		renderer_info->set_text(
				String::utf8("•  ") + TTR("Supports desktop platforms only.") +
				String::utf8("\n•  ") + TTR("Advanced 3D graphics available.") +
				String::utf8("\n•  ") + TTR("Can scale to large complex scenes.") +
				String::utf8("\n•  ") + TTR("Uses RenderingDevice backend.") +
				String::utf8("\n•  ") + TTR("Slower rendering of simple scenes."));
		rd_error = !rendering_device_supported;
	} else if (renderer_type == "mobile") {
		renderer_info->set_text(
				String::utf8("•  ") + TTR("Supports desktop + mobile platforms.") +
				String::utf8("\n•  ") + TTR("Less advanced 3D graphics.") +
				String::utf8("\n•  ") + TTR("Less scalable for complex scenes.") +
				String::utf8("\n•  ") + TTR("Uses RenderingDevice backend.") +
				String::utf8("\n•  ") + TTR("Fast rendering of simple scenes."));
		rd_error = !rendering_device_supported;
	} else if (renderer_type == "gl_compatibility") {
		renderer_info->set_text(
				String::utf8("•  ") + TTR("Supports desktop, mobile + web platforms.") +
				String::utf8("\n•  ") + TTR("Least advanced 3D graphics.") +
				String::utf8("\n•  ") + TTR("Intended for low-end/older devices.") +
				String::utf8("\n•  ") + TTR("Uses OpenGL 3 backend (OpenGL 3.3/ES 3.0/WebGL2).") +
				String::utf8("\n•  ") + TTR("Fastest rendering of simple scenes."));
	} else {
		WARN_PRINT("Unknown renderer type. Please report this as a bug on GitHub.");
	}

	rd_not_supported->set_visible(rd_error);
	get_ok_button()->set_disabled(rd_error);
	if (rd_error) {
		// Needs to be set here since theme colors aren't available at startup.
		rd_not_supported->add_theme_color_override(SceneStringName(font_color), get_theme_color(SNAME("error_color"), EditorStringName(Editor)));
	}
}

void ProjectDialog::_nonempty_confirmation_ok_pressed() {
	is_folder_empty = true;
	ok_pressed();
}

void ProjectDialog::ok_pressed() {
	// Before we create a project, check that the target folder is empty.
	// If not, we need to ask the user if they're sure they want to do this.
	if (!is_folder_empty) {
		if (!nonempty_confirmation) {
			nonempty_confirmation = memnew(ConfirmationDialog);
			nonempty_confirmation->set_title(TTRC("Warning: This folder is not empty"));
			nonempty_confirmation->set_text(TTRC("You are about to create a Godot project in a non-empty folder.\nThe entire contents of this folder will be imported as project resources!\n\nAre you sure you wish to continue?"));
			nonempty_confirmation->get_ok_button()->connect(SceneStringName(pressed), callable_mp(this, &ProjectDialog::_nonempty_confirmation_ok_pressed));
			add_child(nonempty_confirmation);
		}
		nonempty_confirmation->popup_centered();
		return;
	}

	String path = project_path->get_text();

	if (mode == MODE_NEW) {
		if (create_dir->is_pressed()) {
			Ref<DirAccess> d = DirAccess::create(DirAccess::ACCESS_FILESYSTEM);
			if (!d->dir_exists(path) && d->make_dir(path) != OK) {
				_set_message(TTRC("Couldn't create project directory, check permissions."), MESSAGE_ERROR);
				return;
			}
		}

		PackedStringArray project_features = ProjectSettings::get_required_features();
		ProjectSettings::CustomMap initial_settings;

		// Be sure to change this code if/when renderers are changed.
		// Default values are "forward_plus" for the main setting, "mobile" for the mobile override,
		// and "gl_compatibility" for the web override.
		String renderer_type = renderer_button_group->get_pressed_button()->get_meta(SNAME("rendering_method"));
		initial_settings["rendering/renderer/rendering_method"] = renderer_type;

		EditorSettings::get_singleton()->set("project_manager/default_renderer", renderer_type);
		EditorSettings::get_singleton()->save();

		if (renderer_type == "forward_plus") {
			project_features.push_back("Forward Plus");
		} else if (renderer_type == "mobile") {
			project_features.push_back("Mobile");
		} else if (renderer_type == "gl_compatibility") {
			project_features.push_back("GL Compatibility");
			// Also change the default rendering method for the mobile override.
			initial_settings["rendering/renderer/rendering_method.mobile"] = "gl_compatibility";
		} else {
			WARN_PRINT("Unknown renderer type. Please report this as a bug on GitHub.");
		}

		project_features.sort();
		initial_settings["application/config/features"] = project_features;
		initial_settings["application/config/name"] = project_name->get_text().strip_edges();
		initial_settings["application/config/icon"] = "res://icon.svg";

		Error err = ProjectSettings::get_singleton()->save_custom(path.path_join("project.godot"), initial_settings, Vector<String>(), false);
		if (err != OK) {
			_set_message(TTRC("Couldn't create project.godot in project path."), MESSAGE_ERROR);
			return;
		}

		// Store default project icon in SVG format.
		Ref<FileAccess> fa_icon = FileAccess::open(path.path_join("icon.svg"), FileAccess::WRITE, &err);
		if (err != OK) {
			_set_message(TTRC("Couldn't create icon.svg in project path."), MESSAGE_ERROR);
			return;
		}
		fa_icon->store_string(get_default_project_icon());

		EditorVCSInterface::create_vcs_metadata_files(EditorVCSInterface::VCSMetadata(vcs_metadata_selection->get_selected()), path);

		// Ensures external editors and IDEs use UTF-8 encoding.
		const String editor_config_path = path.path_join(".editorconfig");
		Ref<FileAccess> f = FileAccess::open(editor_config_path, FileAccess::WRITE);
		if (f.is_null()) {
			// .editorconfig isn't so critical.
			ERR_PRINT("Couldn't create .editorconfig in project path.");
		} else {
			f->store_line("root = true");
			f->store_line("");
			f->store_line("[*]");
			f->store_line("charset = utf-8");
			f->close();
			FileAccess::set_hidden_attribute(editor_config_path, true);
		}

		// Lupine Engine: Create template-based project structure
		if (!selected_template_id.is_empty()) {
			_create_template_project();
		}
	}

	// Two cases for importing a ZIP.
	switch (mode) {
		case MODE_IMPORT: {
			if (zip_path.is_empty()) {
				break;
			}

			path = install_path->get_text().simplify_path();
			[[fallthrough]];
		}
		case MODE_INSTALL: {
			ERR_FAIL_COND(zip_path.is_empty());

			Ref<FileAccess> io_fa;
			zlib_filefunc_def io = zipio_create_io(&io_fa);

			unzFile pkg = unzOpen2(zip_path.utf8().get_data(), &io);
			if (!pkg) {
				dialog_error->set_text(TTRC("Error opening package file, not in ZIP format."));
				dialog_error->popup_centered();
				return;
			}

			// Find the first directory with a "project.godot".
			String zip_root;
			int ret = unzGoToFirstFile(pkg);
			while (ret == UNZ_OK) {
				unz_file_info info;
				char fname[16384];
				unzGetCurrentFileInfo(pkg, &info, fname, 16384, nullptr, 0, nullptr, 0);
				ERR_FAIL_COND_MSG(ret != UNZ_OK, "Failed to get current file info.");

				String name = String::utf8(fname);

				// Skip the __MACOSX directory created by macOS's built-in file zipper.
				if (name.begins_with("__MACOSX")) {
					ret = unzGoToNextFile(pkg);
					continue;
				}

				if (name.get_file() == "project.godot") {
					zip_root = name.get_base_dir();
					break;
				}

				ret = unzGoToNextFile(pkg);
			}

			if (ret == UNZ_END_OF_LIST_OF_FILE) {
				_set_message(TTRC("Invalid \".zip\" project file; it doesn't contain a \"project.godot\" file."), MESSAGE_ERROR);
				unzClose(pkg);
				return;
			}

			if (create_dir->is_pressed()) {
				Ref<DirAccess> d = DirAccess::create(DirAccess::ACCESS_FILESYSTEM);
				if (!d->dir_exists(path) && d->make_dir(path) != OK) {
					_set_message(TTRC("Couldn't create project directory, check permissions."), MESSAGE_ERROR);
					return;
				}
			}

			ret = unzGoToFirstFile(pkg);

			Vector<String> failed_files;
			while (ret == UNZ_OK) {
				//get filename
				unz_file_info info;
				char fname[16384];
				ret = unzGetCurrentFileInfo(pkg, &info, fname, 16384, nullptr, 0, nullptr, 0);
				ERR_FAIL_COND_MSG(ret != UNZ_OK, "Failed to get current file info.");

				String name = String::utf8(fname);

				// Skip the __MACOSX directory created by macOS's built-in file zipper.
				if (name.begins_with("__MACOSX")) {
					ret = unzGoToNextFile(pkg);
					continue;
				}

				String rel_path = name.trim_prefix(zip_root);
				if (rel_path.is_empty()) { // Root.
				} else if (rel_path.ends_with("/")) { // Directory.
					Ref<DirAccess> da = DirAccess::create(DirAccess::ACCESS_FILESYSTEM);
					da->make_dir(path.path_join(rel_path));
				} else { // File.
					Vector<uint8_t> uncomp_data;
					uncomp_data.resize(info.uncompressed_size);

					unzOpenCurrentFile(pkg);
					ret = unzReadCurrentFile(pkg, uncomp_data.ptrw(), uncomp_data.size());
					ERR_BREAK_MSG(ret < 0, vformat("An error occurred while attempting to read from file: %s. This file will not be used.", rel_path));
					unzCloseCurrentFile(pkg);

					Ref<FileAccess> f = FileAccess::open(path.path_join(rel_path), FileAccess::WRITE);
					if (f.is_valid()) {
						f->store_buffer(uncomp_data.ptr(), uncomp_data.size());
					} else {
						failed_files.push_back(rel_path);
					}
				}

				ret = unzGoToNextFile(pkg);
			}

			unzClose(pkg);

			if (failed_files.size()) {
				String err_msg = TTR("The following files failed extraction from package:") + "\n\n";
				for (int i = 0; i < failed_files.size(); i++) {
					if (i > 15) {
						err_msg += "\nAnd " + itos(failed_files.size() - i) + " more files.";
						break;
					}
					err_msg += failed_files[i] + "\n";
				}

				dialog_error->set_text(err_msg);
				dialog_error->popup_centered();
				return;
			}
		} break;
		default: {
		} break;
	}

	if (mode == MODE_DUPLICATE) {
		Ref<DirAccess> dir = DirAccess::open(original_project_path);
		Error err = FAILED;
		if (dir.is_valid()) {
			err = dir->copy_dir(".", path, -1, true);
		}
		if (err != OK) {
			dialog_error->set_text(vformat(TTR("Couldn't duplicate project (error %d)."), err));
			dialog_error->popup_centered();
			return;
		}
	}

	if (mode == MODE_RENAME || mode == MODE_INSTALL || mode == MODE_DUPLICATE) {
		// Load project.godot as ConfigFile to set the new name.
		ConfigFile cfg;
		String project_godot = path.path_join("project.godot");
		Error err = cfg.load(project_godot);
		if (err != OK) {
			dialog_error->set_text(vformat(TTR("Couldn't load project at '%s' (error %d). It may be missing or corrupted."), project_godot, err));
			dialog_error->popup_centered();
			return;
		}
		cfg.set_value("application", "config/name", project_name->get_text().strip_edges());
		err = cfg.save(project_godot);
		if (err != OK) {
			dialog_error->set_text(vformat(TTR("Couldn't save project at '%s' (error %d)."), project_godot, err));
			dialog_error->popup_centered();
			return;
		}
	}

	hide();
	if (mode == MODE_NEW || mode == MODE_IMPORT || mode == MODE_INSTALL) {
#ifdef ANDROID_ENABLED
		// Create a .nomedia file to hide assets from media apps on Android.
		const String nomedia_file_path = path.path_join(".nomedia");
		Ref<FileAccess> f2 = FileAccess::open(nomedia_file_path, FileAccess::WRITE);
		if (f2.is_null()) {
			// .nomedia isn't so critical.
			ERR_PRINT("Couldn't create .nomedia in project path.");
		} else {
			f2->close();
		}
#endif
		emit_signal(SNAME("project_created"), path, edit_check_box->is_pressed());
	} else if (mode == MODE_DUPLICATE) {
		emit_signal(SNAME("project_duplicated"), original_project_path, path, edit_check_box->is_visible() && edit_check_box->is_pressed());
	} else if (mode == MODE_RENAME) {
		emit_signal(SNAME("projects_updated"));
	}
}

void ProjectDialog::set_zip_path(const String &p_path) {
	zip_path = p_path;
}

void ProjectDialog::set_zip_title(const String &p_title) {
	zip_title = p_title;
}

void ProjectDialog::set_original_project_path(const String &p_path) {
	original_project_path = p_path;
}

void ProjectDialog::set_duplicate_can_edit(bool p_duplicate_can_edit) {
	duplicate_can_edit = p_duplicate_can_edit;
}

void ProjectDialog::set_mode(Mode p_mode) {
	mode = p_mode;
}

void ProjectDialog::set_project_name(const String &p_name) {
	project_name->set_text(p_name);
}

void ProjectDialog::set_project_path(const String &p_path) {
	project_path->set_text(p_path);
}

void ProjectDialog::ask_for_path_and_show() {
	_reset_name();
	_browse_project_path();
}

void ProjectDialog::show_dialog(bool p_reset_name) {
	if (mode == MODE_RENAME) {
		// Name and path are set in `ProjectManager::_rename_project`.
		project_path->set_editable(false);

		set_title(TTRC("Rename Project"));
		set_ok_button_text(TTRC("Rename"));

		create_dir->hide();
		project_status_rect->hide();
		project_browse->hide();
		edit_check_box->hide();

		name_container->show();
		install_path_container->hide();
		renderer_container->hide();
		default_files_container->hide();

		// Lupine Engine: Hide template and module containers for rename mode
		if (template_container) {
			template_container->hide();
		}
		if (module_container) {
			module_container->hide();
		}

		callable_mp((Control *)project_name, &Control::grab_focus).call_deferred();
		callable_mp(project_name, &LineEdit::select_all).call_deferred();
	} else {
		if (p_reset_name) {
			_reset_name();
		}
		project_path->set_editable(true);

		if (mode == MODE_DUPLICATE) {
			String original_dir = original_project_path.get_base_dir();
			project_path->set_text(original_dir);
			install_path->set_text(original_dir);
			fdialog_project->set_current_dir(original_dir);
		} else {
			String fav_dir = EDITOR_GET("filesystem/directories/default_project_path");
			fav_dir = fav_dir.simplify_path();
			if (!fav_dir.is_empty()) {
				project_path->set_text(fav_dir);
				install_path->set_text(fav_dir);
				fdialog_project->set_current_dir(fav_dir);
			} else {
				Ref<DirAccess> d = DirAccess::create(DirAccess::ACCESS_FILESYSTEM);
				project_path->set_text(d->get_current_dir());
				install_path->set_text(d->get_current_dir());
				fdialog_project->set_current_dir(d->get_current_dir());
			}
		}

		create_dir->show();
		project_status_rect->show();
		project_browse->show();
		edit_check_box->show();

		if (mode == MODE_IMPORT) {
			set_title(TTRC("Import Existing Project"));
			set_ok_button_text(TTRC("Import"));

			name_container->hide();
			install_path_container->hide();
			renderer_container->hide();
			default_files_container->hide();

			// Lupine Engine: Hide template and module containers for import mode
			if (template_container) {
				template_container->hide();
			}
			if (module_container) {
				module_container->hide();
			}

			// Project path dialog is also opened; no need to change focus.
		} else if (mode == MODE_NEW) {
			set_title(TTRC("Create New Lupine Project"));
			set_ok_button_text(TTRC("Create"));

			name_container->show();
			install_path_container->hide();
			renderer_container->show();
			default_files_container->show();

			// Lupine Engine: Show template and module selection for new projects
			if (template_container) {
				template_container->show();
			}
			if (module_container) {
				module_container->show();
			}

			callable_mp((Control *)project_name, &Control::grab_focus).call_deferred();
			callable_mp(project_name, &LineEdit::select_all).call_deferred();
		} else if (mode == MODE_INSTALL) {
			set_title(TTR("Install Project:") + " " + zip_title);
			set_ok_button_text(TTRC("Install"));

			project_name->set_text(zip_title);

			name_container->show();
			install_path_container->hide();
			renderer_container->hide();
			default_files_container->hide();

			// Lupine Engine: Hide template and module containers for install mode
			if (template_container) {
				template_container->hide();
			}
			if (module_container) {
				module_container->hide();
			}

			callable_mp((Control *)project_path, &Control::grab_focus).call_deferred();
		} else if (mode == MODE_DUPLICATE) {
			set_title(TTRC("Duplicate Project"));
			set_ok_button_text(TTRC("Duplicate"));

			name_container->show();
			install_path_container->hide();
			renderer_container->hide();
			default_files_container->hide();
			if (!duplicate_can_edit) {
				edit_check_box->hide();
			}

			// Lupine Engine: Hide template and module containers for duplicate mode
			if (template_container) {
				template_container->hide();
			}
			if (module_container) {
				module_container->hide();
			}

			callable_mp((Control *)project_name, &Control::grab_focus).call_deferred();
			callable_mp(project_name, &LineEdit::select_all).call_deferred();
		}

		auto_dir = "";
		last_custom_target_dir = "";
		_update_target_auto_dir();
		if (create_dir->is_pressed()) {
			// Append `auto_dir` to target path.
			_create_dir_toggled(true);
		}
	}

	_validate_path();

	popup_centered(Size2(500, 0) * EDSCALE);
}

void ProjectDialog::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_TRANSLATION_CHANGED: {
			_renderer_selected();
		} break;

		case NOTIFICATION_THEME_CHANGED: {
			create_dir->set_button_icon(get_editor_theme_icon(SNAME("FolderCreate")));
			project_browse->set_button_icon(get_editor_theme_icon(SNAME("FolderBrowse")));
			install_browse->set_button_icon(get_editor_theme_icon(SNAME("FolderBrowse")));
		} break;
		case NOTIFICATION_READY: {
			fdialog_project = memnew(EditorFileDialog);
			fdialog_project->set_previews_enabled(false); // Crucial, otherwise the engine crashes.
			fdialog_project->set_access(EditorFileDialog::ACCESS_FILESYSTEM);
			fdialog_project->connect("dir_selected", callable_mp(this, &ProjectDialog::_project_path_selected));
			fdialog_project->connect("file_selected", callable_mp(this, &ProjectDialog::_project_path_selected));
			fdialog_project->connect("canceled", callable_mp(this, &ProjectDialog::show_dialog).bind(false), CONNECT_DEFERRED);
			callable_mp((Node *)this, &Node::add_sibling).call_deferred(fdialog_project, false);
		} break;
	}
}

void ProjectDialog::_bind_methods() {
	ADD_SIGNAL(MethodInfo("project_created"));
	ADD_SIGNAL(MethodInfo("project_duplicated"));
	ADD_SIGNAL(MethodInfo("projects_updated"));
}

ProjectDialog::ProjectDialog() {
	VBoxContainer *vb = memnew(VBoxContainer);
	add_child(vb);

	name_container = memnew(VBoxContainer);
	vb->add_child(name_container);

	Label *l = memnew(Label);
	l->set_text(TTRC("Project Name:"));
	name_container->add_child(l);

	project_name = memnew(LineEdit);
	project_name->set_virtual_keyboard_show_on_focus(false);
	project_name->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	name_container->add_child(project_name);

	// Lupine Engine: Add template and module containers after name but before path
	// Create horizontal container for side-by-side layout
	HBoxContainer *template_module_hbox = memnew(HBoxContainer);
	template_module_hbox->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	vb->add_child(template_module_hbox);

	// Create placeholder containers that will be set up later
	template_container = memnew(VBoxContainer);
	template_container->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	template_container->set_custom_minimum_size(Size2(400, 0) * EDSCALE);
	template_module_hbox->add_child(template_container);

	// Add separator between template and module sections
	VSeparator *separator = memnew(VSeparator);
	template_module_hbox->add_child(separator);

	module_container = memnew(VBoxContainer);
	module_container->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	module_container->set_custom_minimum_size(Size2(400, 0) * EDSCALE);
	template_module_hbox->add_child(module_container);

	project_path_container = memnew(VBoxContainer);
	vb->add_child(project_path_container);

	HBoxContainer *pphb_label = memnew(HBoxContainer);
	project_path_container->add_child(pphb_label);

	l = memnew(Label);
	l->set_text(TTRC("Project Path:"));
	l->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	pphb_label->add_child(l);

	create_dir = memnew(CheckButton);
	create_dir->set_text(TTRC("Create Folder"));
	create_dir->set_pressed(true);
	pphb_label->add_child(create_dir);
	create_dir->connect(SceneStringName(toggled), callable_mp(this, &ProjectDialog::_create_dir_toggled));

	HBoxContainer *pphb = memnew(HBoxContainer);
	project_path_container->add_child(pphb);

	project_path = memnew(LineEdit);
	project_path->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	project_path->set_accessibility_name(TTRC("Project Path"));
	project_path->set_structured_text_bidi_override(TextServer::STRUCTURED_TEXT_FILE);
	pphb->add_child(project_path);

	install_path_container = memnew(VBoxContainer);
	vb->add_child(install_path_container);

	l = memnew(Label);
	l->set_text(TTRC("Project Installation Path:"));
	install_path_container->add_child(l);

	HBoxContainer *iphb = memnew(HBoxContainer);
	install_path_container->add_child(iphb);

	install_path = memnew(LineEdit);
	install_path->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	install_path->set_accessibility_name(TTRC("Install Path"));
	install_path->set_structured_text_bidi_override(TextServer::STRUCTURED_TEXT_FILE);
	iphb->add_child(install_path);

	// status icon
	project_status_rect = memnew(TextureRect);
	project_status_rect->set_stretch_mode(TextureRect::STRETCH_KEEP_CENTERED);
	pphb->add_child(project_status_rect);

	project_browse = memnew(Button);
	project_browse->set_text(TTRC("Browse"));
	project_browse->connect(SceneStringName(pressed), callable_mp(this, &ProjectDialog::_browse_project_path));
	pphb->add_child(project_browse);

	// install status icon
	install_status_rect = memnew(TextureRect);
	install_status_rect->set_stretch_mode(TextureRect::STRETCH_KEEP_CENTERED);
	iphb->add_child(install_status_rect);

	install_browse = memnew(Button);
	install_browse->set_text(TTRC("Browse"));
	install_browse->connect(SceneStringName(pressed), callable_mp(this, &ProjectDialog::_browse_install_path));
	iphb->add_child(install_browse);

	msg = memnew(Label);
	msg->set_focus_mode(Control::FOCUS_ACCESSIBILITY);
	msg->set_horizontal_alignment(HORIZONTAL_ALIGNMENT_CENTER);
	msg->set_custom_minimum_size(Size2(200, 0) * EDSCALE);
	msg->set_autowrap_mode(TextServer::AUTOWRAP_WORD_SMART);
	vb->add_child(msg);

	// Renderer selection.
	renderer_container = memnew(VBoxContainer);
	vb->add_child(renderer_container);
	l = memnew(Label);
	l->set_text(TTRC("Renderer:"));
	renderer_container->add_child(l);
	HBoxContainer *rshc = memnew(HBoxContainer);
	renderer_container->add_child(rshc);
	renderer_button_group.instantiate();

	// Left hand side, used for checkboxes to select renderer.
	Container *rvb = memnew(VBoxContainer);
	rshc->add_child(rvb);

	String default_renderer_type = "forward_plus";
	if (EditorSettings::get_singleton()->has_setting("project_manager/default_renderer")) {
		default_renderer_type = EditorSettings::get_singleton()->get_setting("project_manager/default_renderer");
	}

	rendering_device_supported = DisplayServer::is_rendering_device_supported();

	if (!rendering_device_supported) {
		default_renderer_type = "gl_compatibility";
	}

	Button *rs_button = memnew(CheckBox);
	rs_button->set_button_group(renderer_button_group);
	rs_button->set_text(TTRC("Forward+"));
#ifndef RD_ENABLED
	rs_button->set_disabled(true);
#endif
	rs_button->set_meta(SNAME("rendering_method"), "forward_plus");
	rs_button->connect(SceneStringName(pressed), callable_mp(this, &ProjectDialog::_renderer_selected));
	rvb->add_child(rs_button);
	if (default_renderer_type == "forward_plus") {
		rs_button->set_pressed(true);
	}
	rs_button = memnew(CheckBox);
	rs_button->set_button_group(renderer_button_group);
	rs_button->set_text(TTRC("Mobile"));
#ifndef RD_ENABLED
	rs_button->set_disabled(true);
#endif
	rs_button->set_meta(SNAME("rendering_method"), "mobile");
	rs_button->connect(SceneStringName(pressed), callable_mp(this, &ProjectDialog::_renderer_selected));
	rvb->add_child(rs_button);
	if (default_renderer_type == "mobile") {
		rs_button->set_pressed(true);
	}
	rs_button = memnew(CheckBox);
	rs_button->set_button_group(renderer_button_group);
	rs_button->set_text(TTRC("Compatibility"));
#if !defined(GLES3_ENABLED)
	rs_button->set_disabled(true);
#endif
	rs_button->set_meta(SNAME("rendering_method"), "gl_compatibility");
	rs_button->connect(SceneStringName(pressed), callable_mp(this, &ProjectDialog::_renderer_selected));
	rvb->add_child(rs_button);
#if defined(GLES3_ENABLED)
	if (default_renderer_type == "gl_compatibility") {
		rs_button->set_pressed(true);
	}
#endif
	rshc->add_child(memnew(VSeparator));

	// Right hand side, used for text explaining each choice.
	rvb = memnew(VBoxContainer);
	rvb->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	rshc->add_child(rvb);
	renderer_info = memnew(Label);
	renderer_info->set_focus_mode(Control::FOCUS_ACCESSIBILITY);
	renderer_info->set_modulate(Color(1, 1, 1, 0.7));
	rvb->add_child(renderer_info);

	rd_not_supported = memnew(Label);
	rd_not_supported->set_focus_mode(Control::FOCUS_ACCESSIBILITY);
	rd_not_supported->set_text(vformat(TTRC("RenderingDevice-based methods not available on this GPU:\n%s\nPlease use the Compatibility renderer."), RenderingServer::get_singleton()->get_video_adapter_name()));
	rd_not_supported->set_horizontal_alignment(HORIZONTAL_ALIGNMENT_CENTER);
	rd_not_supported->set_custom_minimum_size(Size2(200, 0) * EDSCALE);
	rd_not_supported->set_autowrap_mode(TextServer::AUTOWRAP_WORD_SMART);
	rd_not_supported->set_visible(false);
	renderer_container->add_child(rd_not_supported);

	_renderer_selected();

	l = memnew(Label);
	l->set_focus_mode(Control::FOCUS_ACCESSIBILITY);
	l->set_text(TTRC("The renderer can be changed later, but scenes may need to be adjusted."));
	// Add some extra spacing to separate it from the list above and the buttons below.
	l->set_custom_minimum_size(Size2(0, 40) * EDSCALE);
	l->set_horizontal_alignment(HORIZONTAL_ALIGNMENT_CENTER);
	l->set_vertical_alignment(VERTICAL_ALIGNMENT_CENTER);
	l->set_modulate(Color(1, 1, 1, 0.7));
	renderer_container->add_child(l);

	default_files_container = memnew(HBoxContainer);
	vb->add_child(default_files_container);
	l = memnew(Label);
	l->set_text(TTRC("Version Control Metadata:"));
	default_files_container->add_child(l);
	vcs_metadata_selection = memnew(OptionButton);
	vcs_metadata_selection->set_custom_minimum_size(Size2(100, 20));
	vcs_metadata_selection->add_item(TTRC("None"), (int)EditorVCSInterface::VCSMetadata::NONE);
	vcs_metadata_selection->add_item(TTRC("Git"), (int)EditorVCSInterface::VCSMetadata::GIT);
	vcs_metadata_selection->select((int)EditorVCSInterface::VCSMetadata::GIT);
	vcs_metadata_selection->set_accessibility_name(TTRC("Version Control Metadata"));
	default_files_container->add_child(vcs_metadata_selection);
	Control *spacer = memnew(Control);
	spacer->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	default_files_container->add_child(spacer);
	fdialog_install = memnew(EditorFileDialog);
	fdialog_install->set_previews_enabled(false); //Crucial, otherwise the engine crashes.
	fdialog_install->set_access(EditorFileDialog::ACCESS_FILESYSTEM);
	add_child(fdialog_install);

	Control *spacer2 = memnew(Control);
	spacer2->set_v_size_flags(Control::SIZE_EXPAND_FILL);
	vb->add_child(spacer2);

	edit_check_box = memnew(CheckBox);
	edit_check_box->set_text(TTRC("Edit Now"));
	edit_check_box->set_h_size_flags(Control::SIZE_SHRINK_CENTER);
	edit_check_box->set_pressed(true);
	vb->add_child(edit_check_box);

	project_name->connect(SceneStringName(text_changed), callable_mp(this, &ProjectDialog::_project_name_changed).unbind(1));
	project_name->connect(SceneStringName(text_submitted), callable_mp(this, &ProjectDialog::ok_pressed).unbind(1));

	project_path->connect(SceneStringName(text_changed), callable_mp(this, &ProjectDialog::_project_path_changed).unbind(1));
	project_path->connect(SceneStringName(text_submitted), callable_mp(this, &ProjectDialog::ok_pressed).unbind(1));

	install_path->connect(SceneStringName(text_changed), callable_mp(this, &ProjectDialog::_install_path_changed).unbind(1));
	install_path->connect(SceneStringName(text_submitted), callable_mp(this, &ProjectDialog::ok_pressed).unbind(1));

	fdialog_install->connect("dir_selected", callable_mp(this, &ProjectDialog::_install_path_selected));
	fdialog_install->connect("file_selected", callable_mp(this, &ProjectDialog::_install_path_selected));

	set_hide_on_ok(false);

	dialog_error = memnew(AcceptDialog);
	add_child(dialog_error);

	// Lupine Engine: Initialize module manager and templates
	module_manager.instantiate();
	_initialize_templates();
	_initialize_modules();
	_setup_template_ui();
	_setup_module_ui();
}

// Lupine Engine: Template and Module Implementation

void ProjectDialog::_initialize_templates() {
	available_templates.clear();

	// 2D Templates
	{
		ProjectTemplate template_2d_topdown_4dir;
		template_2d_topdown_4dir.id = "2d_topdown_rpg_4dir";
		template_2d_topdown_4dir.name = "2D Top-down RPG (4-Direction)";
		template_2d_topdown_4dir.description = "Complete 2D top-down RPG with 4-directional movement, extensive combat system, world elements, and progression. Perfect for retro-style adventures and dungeon crawlers.";
		template_2d_topdown_4dir.category = "2D RPG";
		template_2d_topdown_4dir.default_modules.push_back("player_controller_2d_topdown");
		template_2d_topdown_4dir.default_modules.push_back("camera_topdown_rpg");
		template_2d_topdown_4dir.default_modules.push_back("player_stats");
		template_2d_topdown_4dir.default_modules.push_back("dialogue_system");
		template_2d_topdown_4dir.default_modules.push_back("inventory_system");
		template_2d_topdown_4dir.default_modules.push_back("quest_system");
		template_2d_topdown_4dir.default_modules.push_back("enemy_ai");
		template_2d_topdown_4dir.default_modules.push_back("ability_system");
		template_2d_topdown_4dir.default_modules.push_back("topdown_rpg_main_scene");
		template_2d_topdown_4dir.default_modules.push_back("topdown_enemy_types");
		template_2d_topdown_4dir.default_modules.push_back("rpg_world_elements");
		template_2d_topdown_4dir.default_modules.push_back("topdown_combat");
		template_2d_topdown_4dir.default_modules.push_back("rpg_progression");
		template_2d_topdown_4dir.default_modules.push_back("collectible_system");
		template_2d_topdown_4dir.default_modules.push_back("popup_manager");
		template_2d_topdown_4dir.default_modules.push_back("hud_builder");
		template_2d_topdown_4dir.folder_structure.push_back("scenes/characters");
		template_2d_topdown_4dir.folder_structure.push_back("scenes/enemies");
		template_2d_topdown_4dir.folder_structure.push_back("scenes/world");
		template_2d_topdown_4dir.folder_structure.push_back("scenes/combat");
		template_2d_topdown_4dir.folder_structure.push_back("scenes/ui");
		template_2d_topdown_4dir.folder_structure.push_back("scripts/characters");
		template_2d_topdown_4dir.folder_structure.push_back("scripts/enemies");
		template_2d_topdown_4dir.folder_structure.push_back("scripts/world");
		template_2d_topdown_4dir.folder_structure.push_back("scripts/combat");
		template_2d_topdown_4dir.folder_structure.push_back("scripts/progression");
		template_2d_topdown_4dir.folder_structure.push_back("scripts/ui");
		template_2d_topdown_4dir.folder_structure.push_back("scripts/systems");
		template_2d_topdown_4dir.folder_structure.push_back("globals");
		template_2d_topdown_4dir.folder_structure.push_back("assets/sprites/characters");
		template_2d_topdown_4dir.folder_structure.push_back("assets/sprites/enemies");
		template_2d_topdown_4dir.folder_structure.push_back("assets/sprites/environments");
		template_2d_topdown_4dir.folder_structure.push_back("assets/sprites/items");
		template_2d_topdown_4dir.folder_structure.push_back("assets/sprites/effects");
		template_2d_topdown_4dir.folder_structure.push_back("assets/audio/music");
		template_2d_topdown_4dir.folder_structure.push_back("assets/audio/sfx");
		template_2d_topdown_4dir.folder_structure.push_back("data/dialogues");
		template_2d_topdown_4dir.folder_structure.push_back("data/quests");
		template_2d_topdown_4dir.folder_structure.push_back("data/items");
		template_2d_topdown_4dir.folder_structure.push_back("data/enemies");
		template_2d_topdown_4dir.main_scene_template = "Main2DTopdown.tscn";
		available_templates.push_back(template_2d_topdown_4dir);

		ProjectTemplate template_2d_topdown_8dir;
		template_2d_topdown_8dir.id = "2d_topdown_rpg_8dir";
		template_2d_topdown_8dir.name = "2D Top-down RPG (8-Direction)";
		template_2d_topdown_8dir.description = "Complete 2D top-down RPG with 8-directional movement, extensive combat system, world elements, and progression. Perfect for modern action RPGs with smooth character control.";
		template_2d_topdown_8dir.category = "2D RPG";
		template_2d_topdown_8dir.default_modules.push_back("player_controller_2d_topdown_8dir");
		template_2d_topdown_8dir.default_modules.push_back("camera_topdown_rpg");
		template_2d_topdown_8dir.default_modules.push_back("player_stats");
		template_2d_topdown_8dir.default_modules.push_back("dialogue_system");
		template_2d_topdown_8dir.default_modules.push_back("inventory_system");
		template_2d_topdown_8dir.default_modules.push_back("quest_system");
		template_2d_topdown_8dir.default_modules.push_back("enemy_ai");
		template_2d_topdown_8dir.default_modules.push_back("ability_system");
		template_2d_topdown_8dir.default_modules.push_back("topdown_rpg_main_scene");
		template_2d_topdown_8dir.default_modules.push_back("topdown_enemy_types");
		template_2d_topdown_8dir.default_modules.push_back("rpg_world_elements");
		template_2d_topdown_8dir.default_modules.push_back("topdown_combat");
		template_2d_topdown_8dir.default_modules.push_back("rpg_progression");
		template_2d_topdown_8dir.default_modules.push_back("collectible_system");
		template_2d_topdown_8dir.default_modules.push_back("popup_manager");
		template_2d_topdown_8dir.default_modules.push_back("hud_builder");
		template_2d_topdown_8dir.folder_structure = template_2d_topdown_4dir.folder_structure; // Same structure
		template_2d_topdown_8dir.main_scene_template = "Main2DTopdown8Dir.tscn";
		available_templates.push_back(template_2d_topdown_8dir);

		ProjectTemplate template_2d_sidescroller;
		template_2d_sidescroller.id = "2d_sidescroller_rpg";
		template_2d_sidescroller.name = "2D Sidescroller RPG";
		template_2d_sidescroller.description = "Side-scrolling RPG with platforming elements. Great for Metroidvania-style games with RPG progression.";
		template_2d_sidescroller.category = "2D RPG";
		template_2d_sidescroller.default_modules.push_back("enhanced_platformer_controller");
		template_2d_sidescroller.default_modules.push_back("parallax_camera");
		template_2d_sidescroller.default_modules.push_back("player_stats");
		template_2d_sidescroller.default_modules.push_back("dialogue_system");
		template_2d_sidescroller.default_modules.push_back("inventory_system");
		template_2d_sidescroller.default_modules.push_back("enemy_ai");
		template_2d_sidescroller.default_modules.push_back("ability_system");
		template_2d_sidescroller.default_modules.push_back("collectible_system");
		template_2d_sidescroller.default_modules.push_back("level_progression");
		template_2d_sidescroller.default_modules.push_back("quest_system");
		template_2d_sidescroller.default_modules.push_back("platformer_elements");
		template_2d_sidescroller.default_modules.push_back("popup_manager");
		template_2d_sidescroller.default_modules.push_back("screen_effects");
		template_2d_sidescroller.default_modules.push_back("passive_mobs");
		template_2d_sidescroller.default_modules.push_back("hud_builder");
		template_2d_sidescroller.folder_structure = template_2d_topdown_4dir.folder_structure; // Same base structure
		template_2d_sidescroller.folder_structure.push_back("scenes/enemies");
		template_2d_sidescroller.folder_structure.push_back("scenes/collectibles");
		template_2d_sidescroller.folder_structure.push_back("scenes/abilities");
		template_2d_sidescroller.folder_structure.push_back("scenes/projectiles");
		template_2d_sidescroller.folder_structure.push_back("scenes/world");
		template_2d_sidescroller.folder_structure.push_back("scripts/enemies");
		template_2d_sidescroller.folder_structure.push_back("scripts/collectibles");
		template_2d_sidescroller.folder_structure.push_back("scripts/abilities");
		template_2d_sidescroller.folder_structure.push_back("scripts/projectiles");
		template_2d_sidescroller.folder_structure.push_back("scripts/world");
		template_2d_sidescroller.folder_structure.push_back("scripts/items");
		template_2d_sidescroller.folder_structure.push_back("data/dialogues");
		template_2d_sidescroller.folder_structure.push_back("data/items");
		template_2d_sidescroller.folder_structure.push_back("data/abilities");
		template_2d_sidescroller.folder_structure.push_back("data/levels");
		template_2d_sidescroller.main_scene_template = "Main2DSidescroller.tscn";
		available_templates.push_back(template_2d_sidescroller);

		// 3D Platformer Template
		ProjectTemplate template_3d_platformer;
		template_3d_platformer.id = "3d_platformer";
		template_3d_platformer.name = "3D Platformer";
		template_3d_platformer.description = "Complete 3D platformer template with Mario 64/Banjo-Kazooie style gameplay, complex puzzles, enemies, bosses, and world progression.";
		template_3d_platformer.category = "3D";
		template_3d_platformer.folder_structure.push_back("scenes/player");
		template_3d_platformer.folder_structure.push_back("scenes/enemies");
		template_3d_platformer.folder_structure.push_back("scenes/collectibles");
		template_3d_platformer.folder_structure.push_back("scenes/levels");
		template_3d_platformer.folder_structure.push_back("scenes/puzzles");
		template_3d_platformer.folder_structure.push_back("scenes/world");
		template_3d_platformer.folder_structure.push_back("scenes/camera");
		template_3d_platformer.folder_structure.push_back("scripts/player");
		template_3d_platformer.folder_structure.push_back("scripts/enemies");
		template_3d_platformer.folder_structure.push_back("scripts/collectibles");
		template_3d_platformer.folder_structure.push_back("scripts/puzzles");
		template_3d_platformer.folder_structure.push_back("scripts/world");
		template_3d_platformer.folder_structure.push_back("scripts/camera");
		template_3d_platformer.folder_structure.push_back("assets/models");
		template_3d_platformer.folder_structure.push_back("assets/textures");
		template_3d_platformer.folder_structure.push_back("assets/materials");
		template_3d_platformer.folder_structure.push_back("data");
		template_3d_platformer.default_modules.push_back("platformer_3d_controller");
		template_3d_platformer.default_modules.push_back("camera_3d");
		template_3d_platformer.default_modules.push_back("world_state_3d");
		template_3d_platformer.default_modules.push_back("puzzle_system_3d");
		template_3d_platformer.default_modules.push_back("enemy_3d");
		template_3d_platformer.default_modules.push_back("location_manager_3d");
		template_3d_platformer.default_modules.push_back("player_stats");
		template_3d_platformer.default_modules.push_back("dialogue_system");
		template_3d_platformer.default_modules.push_back("quest_system");
		template_3d_platformer.default_modules.push_back("inventory_system");
		template_3d_platformer.default_modules.push_back("collectible_system");
		template_3d_platformer.default_modules.push_back("popup_manager");
		template_3d_platformer.default_modules.push_back("screen_effects");
		template_3d_platformer.default_modules.push_back("hud_builder");
		template_3d_platformer.main_scene_template = "Main3D.tscn";
		available_templates.push_back(template_3d_platformer);
	}

	// 3D Templates
	{
		ProjectTemplate template_3d_rpg;
		template_3d_rpg.id = "3d_third_person_rpg";
		template_3d_rpg.name = "3D Third-Person RPG";
		template_3d_rpg.description = "Full 3D RPG with third-person camera and exploration. Perfect for open-world adventures and story-driven games.";
		template_3d_rpg.category = "3D RPG";
		template_3d_rpg.default_modules.push_back("player_controller_3d_third_person");
		template_3d_rpg.default_modules.push_back("camera_follow_3d");
		template_3d_rpg.default_modules.push_back("inventory_advanced");
		template_3d_rpg.default_modules.push_back("dialogue_system");
		template_3d_rpg.default_modules.push_back("quest_system");
		template_3d_rpg.folder_structure.push_back("scenes/characters");
		template_3d_rpg.folder_structure.push_back("scenes/environments");
		template_3d_rpg.folder_structure.push_back("scenes/ui");
		template_3d_rpg.folder_structure.push_back("scripts/characters");
		template_3d_rpg.folder_structure.push_back("scripts/systems");
		template_3d_rpg.folder_structure.push_back("assets/models/characters");
		template_3d_rpg.folder_structure.push_back("assets/models/environments");
		template_3d_rpg.folder_structure.push_back("assets/textures");
		template_3d_rpg.folder_structure.push_back("assets/audio/music");
		template_3d_rpg.folder_structure.push_back("assets/audio/sfx");
		template_3d_rpg.main_scene_template = "Main3DRPG.tscn";
		available_templates.push_back(template_3d_rpg);

		ProjectTemplate template_3d_farming;
		template_3d_farming.id = "3d_farming_game";
		template_3d_farming.name = "3D Farming Game";
		template_3d_farming.description = "Peaceful farming simulation with crop management, animal care, and social elements. Inspired by Harvest Moon and Stardew Valley.";
		template_3d_farming.category = "3D Simulation";
		template_3d_farming.default_modules.push_back("player_controller_3d_third_person");
		template_3d_farming.default_modules.push_back("camera_follow_3d");
		template_3d_farming.default_modules.push_back("inventory_farming");
		template_3d_farming.default_modules.push_back("dialogue_system");
		template_3d_farming.default_modules.push_back("farming_system");
		template_3d_farming.default_modules.push_back("time_system");
		template_3d_farming.folder_structure = template_3d_rpg.folder_structure; // Same base structure
		template_3d_farming.folder_structure.push_back("assets/models/crops");
		template_3d_farming.folder_structure.push_back("assets/models/animals");
		template_3d_farming.main_scene_template = "MainFarming.tscn";
		available_templates.push_back(template_3d_farming);
	}

	// Visual Novel Templates
	{
		ProjectTemplate template_visual_novel;
		template_visual_novel.id = "visual_novel";
		template_visual_novel.name = "Visual Novel";
		template_visual_novel.description = "Story-focused visual novel with branching narratives, character portraits, and rich dialogue system.";
		template_visual_novel.category = "Visual Novel";
		template_visual_novel.default_modules.push_back("visual_novel_script_parser");
		template_visual_novel.default_modules.push_back("character_portrait_system");
		template_visual_novel.default_modules.push_back("background_manager");
		template_visual_novel.default_modules.push_back("audio_manager");
		template_visual_novel.default_modules.push_back("save_load_system");
		template_visual_novel.default_modules.push_back("choice_system");
		template_visual_novel.default_modules.push_back("visual_novel_main_scene");
		template_visual_novel.folder_structure.push_back("scenes/ui");
		template_visual_novel.folder_structure.push_back("scripts/dialogue");
		template_visual_novel.folder_structure.push_back("scripts/ui");
		template_visual_novel.folder_structure.push_back("globals");
		template_visual_novel.folder_structure.push_back("assets/portraits");
		template_visual_novel.folder_structure.push_back("assets/backgrounds");
		template_visual_novel.folder_structure.push_back("assets/music");
		template_visual_novel.folder_structure.push_back("assets/soundEffects");
		template_visual_novel.folder_structure.push_back("data/dialogue");
		template_visual_novel.main_scene_template = "MainVisualNovel.tscn";
		available_templates.push_back(template_visual_novel);

		ProjectTemplate template_vn_tactical;
		template_vn_tactical.id = "visual_novel_tactical";
		template_vn_tactical.name = "Visual Novel with Tactical Combat";
		template_vn_tactical.description = "Combines rich storytelling with strategic grid-based combat. Perfect for Fire Emblem-style games.";
		template_vn_tactical.category = "Visual Novel";
		template_vn_tactical.default_modules.push_back("visual_novel_script_parser");
		template_vn_tactical.default_modules.push_back("character_portrait_system");
		template_vn_tactical.default_modules.push_back("background_manager");
		template_vn_tactical.default_modules.push_back("audio_manager");
		template_vn_tactical.default_modules.push_back("save_load_system");
		template_vn_tactical.default_modules.push_back("choice_system");
		template_vn_tactical.default_modules.push_back("visual_novel_main_scene");
		template_vn_tactical.default_modules.push_back("tactical_combat_system");
		template_vn_tactical.default_modules.push_back("grid_movement_system");
		template_vn_tactical.default_modules.push_back("character_stats_system");
		template_vn_tactical.folder_structure = template_visual_novel.folder_structure;
		template_vn_tactical.folder_structure.push_back("scenes/combat");
		template_vn_tactical.folder_structure.push_back("scripts/combat");
		template_vn_tactical.folder_structure.push_back("data/units");
		template_vn_tactical.folder_structure.push_back("data/maps");
		template_vn_tactical.main_scene_template = "MainVNTactical.tscn";
		available_templates.push_back(template_vn_tactical);
	}

	// Monster Capture Templates
	{
		ProjectTemplate template_monster_2d;
		template_monster_2d.id = "monster_capture_2d";
		template_monster_2d.name = "Monster Capture 2D";
		template_monster_2d.description = "Pokemon-inspired monster collection game with authentic turn-based combat, type effectiveness, status effects, and creature management.";
		template_monster_2d.category = "Monster Capture";
		template_monster_2d.default_modules.push_back("player_controller_2d_topdown");
		template_monster_2d.default_modules.push_back("camera_topdown_rpg");
		template_monster_2d.default_modules.push_back("monster_system");
		template_monster_2d.default_modules.push_back("pokemon_battle_system");
		template_monster_2d.default_modules.push_back("monster_capture_system");
		template_monster_2d.default_modules.push_back("inventory_basic");
		template_monster_2d.default_modules.push_back("save_load_system");
		template_monster_2d.default_modules.push_back("popup_manager");
		template_monster_2d.folder_structure.push_back("scenes/overworld");
		template_monster_2d.folder_structure.push_back("scenes/combat");
		template_monster_2d.folder_structure.push_back("scenes/ui");
		template_monster_2d.folder_structure.push_back("scripts/monsters");
		template_monster_2d.folder_structure.push_back("scripts/combat");
		template_monster_2d.folder_structure.push_back("assets/sprites/monsters");
		template_monster_2d.folder_structure.push_back("assets/sprites/environments");
		template_monster_2d.folder_structure.push_back("data/monsters");
		template_monster_2d.folder_structure.push_back("data/moves");
		template_monster_2d.main_scene_template = "MainMonsterCapture2D.tscn";
		available_templates.push_back(template_monster_2d);

		ProjectTemplate template_monster_3d;
		template_monster_3d.id = "monster_capture_3d";
		template_monster_3d.name = "Monster Capture 3D";
		template_monster_3d.description = "3D monster collection game with immersive environments, Pokemon-style turn-based combat, and full 3D exploration.";
		template_monster_3d.category = "Monster Capture";
		template_monster_3d.default_modules.push_back("player_controller_3d_third_person");
		template_monster_3d.default_modules.push_back("camera_follow_3d");
		template_monster_3d.default_modules.push_back("monster_system");
		template_monster_3d.default_modules.push_back("pokemon_battle_system");
		template_monster_3d.default_modules.push_back("monster_capture_system");
		template_monster_3d.default_modules.push_back("inventory_basic");
		template_monster_3d.default_modules.push_back("save_load_system");
		template_monster_3d.default_modules.push_back("popup_manager");
		template_monster_3d.folder_structure = template_monster_2d.folder_structure;
		template_monster_3d.folder_structure.push_back("assets/models/monsters");
		template_monster_3d.folder_structure.push_back("assets/models/environments");
		template_monster_3d.main_scene_template = "MainMonsterCapture3D.tscn";
		available_templates.push_back(template_monster_3d);
	}

	// JRPG Templates
	{
		ProjectTemplate template_jrpg_2d;
		template_jrpg_2d.id = "jrpg_2d";
		template_jrpg_2d.name = "JRPG 2D (Classic)";
		template_jrpg_2d.description = "Classic JRPG with party management, turn-based combat, and overworld exploration. Features Final Fantasy/RPG Maker style gameplay with 4/8-directional party followers.";
		template_jrpg_2d.category = "JRPG";
		template_jrpg_2d.default_modules.push_back("player_controller_2d_topdown");
		template_jrpg_2d.default_modules.push_back("camera_topdown_rpg");
		template_jrpg_2d.default_modules.push_back("player_stats");
		template_jrpg_2d.default_modules.push_back("jrpg_party_system_2d");
		template_jrpg_2d.default_modules.push_back("jrpg_combat_system");
		template_jrpg_2d.default_modules.push_back("dialogue_system");
		template_jrpg_2d.default_modules.push_back("inventory_system");
		template_jrpg_2d.default_modules.push_back("quest_system");
		template_jrpg_2d.default_modules.push_back("enemy_ai");
		template_jrpg_2d.default_modules.push_back("ability_system");
		template_jrpg_2d.default_modules.push_back("rpg_progression");
		template_jrpg_2d.default_modules.push_back("save_load_system");
		template_jrpg_2d.default_modules.push_back("popup_manager");
		template_jrpg_2d.default_modules.push_back("hud_builder");
		template_jrpg_2d.folder_structure.push_back("scenes/party");
		template_jrpg_2d.folder_structure.push_back("scenes/combat");
		template_jrpg_2d.folder_structure.push_back("scenes/overworld");
		template_jrpg_2d.folder_structure.push_back("scenes/ui");
		template_jrpg_2d.folder_structure.push_back("scripts/party");
		template_jrpg_2d.folder_structure.push_back("scripts/combat");
		template_jrpg_2d.folder_structure.push_back("scripts/skills");
		template_jrpg_2d.folder_structure.push_back("scripts/ui");
		template_jrpg_2d.folder_structure.push_back("scripts/systems");
		template_jrpg_2d.folder_structure.push_back("globals");
		template_jrpg_2d.folder_structure.push_back("assets/sprites/characters");
		template_jrpg_2d.folder_structure.push_back("assets/sprites/enemies");
		template_jrpg_2d.folder_structure.push_back("assets/sprites/battle");
		template_jrpg_2d.folder_structure.push_back("assets/sprites/ui");
		template_jrpg_2d.folder_structure.push_back("assets/audio/music");
		template_jrpg_2d.folder_structure.push_back("assets/audio/sfx");
		template_jrpg_2d.folder_structure.push_back("data/skills");
		template_jrpg_2d.folder_structure.push_back("data/enemies");
		template_jrpg_2d.folder_structure.push_back("data/party");
		template_jrpg_2d.folder_structure.push_back("data/dialogues");
		template_jrpg_2d.folder_structure.push_back("data/quests");
		template_jrpg_2d.main_scene_template = "MainJRPG2D.tscn";
		available_templates.push_back(template_jrpg_2d);

		ProjectTemplate template_jrpg_3d;
		template_jrpg_3d.id = "jrpg_3d";
		template_jrpg_3d.name = "JRPG 3D (Modern)";
		template_jrpg_3d.description = "Modern 3D JRPG with party management, turn-based combat, and 3D overworld exploration. Features Persona/Tales-style gameplay with 3D party followers.";
		template_jrpg_3d.category = "JRPG";
		template_jrpg_3d.default_modules.push_back("player_controller_3d_third_person");
		template_jrpg_3d.default_modules.push_back("camera_3d_orbit");
		template_jrpg_3d.default_modules.push_back("player_stats");
		template_jrpg_3d.default_modules.push_back("jrpg_party_system_3d");
		template_jrpg_3d.default_modules.push_back("jrpg_combat_system");
		template_jrpg_3d.default_modules.push_back("dialogue_system");
		template_jrpg_3d.default_modules.push_back("inventory_system");
		template_jrpg_3d.default_modules.push_back("quest_system");
		template_jrpg_3d.default_modules.push_back("enemy_ai");
		template_jrpg_3d.default_modules.push_back("ability_system");
		template_jrpg_3d.default_modules.push_back("rpg_progression");
		template_jrpg_3d.default_modules.push_back("save_load_system");
		template_jrpg_3d.default_modules.push_back("popup_manager");
		template_jrpg_3d.default_modules.push_back("hud_builder");
		template_jrpg_3d.folder_structure = template_jrpg_2d.folder_structure;
		template_jrpg_3d.folder_structure.push_back("assets/models/characters");
		template_jrpg_3d.folder_structure.push_back("assets/models/enemies");
		template_jrpg_3d.folder_structure.push_back("assets/models/environments");
		template_jrpg_3d.folder_structure.push_back("assets/textures");
		template_jrpg_3d.folder_structure.push_back("assets/materials");
		template_jrpg_3d.main_scene_template = "MainJRPG3D.tscn";
		available_templates.push_back(template_jrpg_3d);
	}

	// Tactical RPG Templates
	{
		ProjectTemplate template_tactical_2d;
		template_tactical_2d.id = "tactical_rpg_2d";
		template_tactical_2d.name = "Tactical RPG 2D";
		template_tactical_2d.description = "Fire Emblem-style tactical RPG with grid-based combat, class system, recruitment, and commander gameplay. Features strategic positioning and turn-based battles.";
		template_tactical_2d.category = "Tactical RPG";
		template_tactical_2d.default_modules.push_back("tactical_party_system");
		template_tactical_2d.default_modules.push_back("tactical_combat_2d");
		template_tactical_2d.default_modules.push_back("player_stats");
		template_tactical_2d.default_modules.push_back("dialogue_system");
		template_tactical_2d.default_modules.push_back("inventory_system");
		template_tactical_2d.default_modules.push_back("quest_system");
		template_tactical_2d.default_modules.push_back("save_load_system");
		template_tactical_2d.default_modules.push_back("popup_manager");
		template_tactical_2d.default_modules.push_back("hud_builder");
		template_tactical_2d.folder_structure.push_back("scenes/tactical");
		template_tactical_2d.folder_structure.push_back("scenes/ui/tactical");
		template_tactical_2d.folder_structure.push_back("scenes/overworld");
		template_tactical_2d.folder_structure.push_back("scripts/tactical");
		template_tactical_2d.folder_structure.push_back("scripts/ui/tactical");
		template_tactical_2d.folder_structure.push_back("scripts/classes");
		template_tactical_2d.folder_structure.push_back("scripts/ai");
		template_tactical_2d.folder_structure.push_back("globals");
		template_tactical_2d.folder_structure.push_back("assets/sprites/units");
		template_tactical_2d.folder_structure.push_back("assets/sprites/classes");
		template_tactical_2d.folder_structure.push_back("assets/sprites/battlefields");
		template_tactical_2d.folder_structure.push_back("assets/sprites/ui");
		template_tactical_2d.folder_structure.push_back("assets/audio/music");
		template_tactical_2d.folder_structure.push_back("assets/audio/sfx");
		template_tactical_2d.folder_structure.push_back("data/classes");
		template_tactical_2d.folder_structure.push_back("data/units");
		template_tactical_2d.folder_structure.push_back("data/enemies");
		template_tactical_2d.folder_structure.push_back("data/battlefields");
		template_tactical_2d.folder_structure.push_back("data/dialogues");
		template_tactical_2d.folder_structure.push_back("data/quests");
		template_tactical_2d.main_scene_template = "MainTactical2D.tscn";
		available_templates.push_back(template_tactical_2d);

		ProjectTemplate template_tactical_3d;
		template_tactical_3d.id = "tactical_rpg_3d";
		template_tactical_3d.name = "Tactical RPG 3D";
		template_tactical_3d.description = "Modern 3D tactical RPG with height advantages, line of sight, and immersive 3D battlefields. Features advanced tactical mechanics and strategic depth.";
		template_tactical_3d.category = "Tactical RPG";
		template_tactical_3d.default_modules.push_back("tactical_party_system");
		template_tactical_3d.default_modules.push_back("tactical_combat_3d");
		template_tactical_3d.default_modules.push_back("player_stats");
		template_tactical_3d.default_modules.push_back("dialogue_system");
		template_tactical_3d.default_modules.push_back("inventory_system");
		template_tactical_3d.default_modules.push_back("quest_system");
		template_tactical_3d.default_modules.push_back("save_load_system");
		template_tactical_3d.default_modules.push_back("popup_manager");
		template_tactical_3d.default_modules.push_back("hud_builder");
		template_tactical_3d.folder_structure = template_tactical_2d.folder_structure;
		template_tactical_3d.folder_structure.push_back("assets/models/units");
		template_tactical_3d.folder_structure.push_back("assets/models/battlefields");
		template_tactical_3d.folder_structure.push_back("assets/models/environments");
		template_tactical_3d.folder_structure.push_back("assets/textures");
		template_tactical_3d.folder_structure.push_back("assets/materials");
		template_tactical_3d.main_scene_template = "MainTactical3D.tscn";
		available_templates.push_back(template_tactical_3d);

		ProjectTemplate template_vn_tactical_2d;
		template_vn_tactical_2d.id = "visual_novel_tactical_2d";
		template_vn_tactical_2d.name = "Visual Novel + Tactical Combat 2D";
		template_vn_tactical_2d.description = "Story-driven tactical RPG combining rich visual novel storytelling with Fire Emblem-style combat. Perfect for narrative-focused strategy games.";
		template_vn_tactical_2d.category = "Visual Novel";
		template_vn_tactical_2d.default_modules.push_back("visual_novel_script_parser");
		template_vn_tactical_2d.default_modules.push_back("character_portrait_system");
		template_vn_tactical_2d.default_modules.push_back("background_manager");
		template_vn_tactical_2d.default_modules.push_back("audio_manager");
		template_vn_tactical_2d.default_modules.push_back("tactical_party_system");
		template_vn_tactical_2d.default_modules.push_back("tactical_combat_2d");
		template_vn_tactical_2d.default_modules.push_back("save_load_system");
		template_vn_tactical_2d.default_modules.push_back("choice_system");
		template_vn_tactical_2d.default_modules.push_back("visual_novel_main_scene");
		template_vn_tactical_2d.default_modules.push_back("popup_manager");
		template_vn_tactical_2d.default_modules.push_back("hud_builder");
		template_vn_tactical_2d.folder_structure.push_back("scenes/ui");
		template_vn_tactical_2d.folder_structure.push_back("scenes/tactical");
		template_vn_tactical_2d.folder_structure.push_back("scripts/dialogue");
		template_vn_tactical_2d.folder_structure.push_back("scripts/ui");
		template_vn_tactical_2d.folder_structure.push_back("scripts/tactical");
		template_vn_tactical_2d.folder_structure.push_back("scripts/classes");
		template_vn_tactical_2d.folder_structure.push_back("globals");
		template_vn_tactical_2d.folder_structure.push_back("assets/portraits");
		template_vn_tactical_2d.folder_structure.push_back("assets/backgrounds");
		template_vn_tactical_2d.folder_structure.push_back("assets/sprites/units");
		template_vn_tactical_2d.folder_structure.push_back("assets/sprites/battlefields");
		template_vn_tactical_2d.folder_structure.push_back("assets/music");
		template_vn_tactical_2d.folder_structure.push_back("assets/soundEffects");
		template_vn_tactical_2d.folder_structure.push_back("data/dialogue");
		template_vn_tactical_2d.folder_structure.push_back("data/classes");
		template_vn_tactical_2d.folder_structure.push_back("data/units");
		template_vn_tactical_2d.folder_structure.push_back("data/enemies");
		template_vn_tactical_2d.folder_structure.push_back("data/battlefields");
		template_vn_tactical_2d.main_scene_template = "MainVNTactical2D.tscn";
		available_templates.push_back(template_vn_tactical_2d);

		ProjectTemplate template_vn_tactical_3d;
		template_vn_tactical_3d.id = "visual_novel_tactical_3d";
		template_vn_tactical_3d.name = "Visual Novel + Tactical Combat 3D";
		template_vn_tactical_3d.description = "Modern story-driven tactical RPG with 3D combat and visual novel elements. Combines cinematic storytelling with strategic 3D battles.";
		template_vn_tactical_3d.category = "Visual Novel";
		template_vn_tactical_3d.default_modules.push_back("visual_novel_script_parser");
		template_vn_tactical_3d.default_modules.push_back("character_portrait_system");
		template_vn_tactical_3d.default_modules.push_back("background_manager");
		template_vn_tactical_3d.default_modules.push_back("audio_manager");
		template_vn_tactical_3d.default_modules.push_back("tactical_party_system");
		template_vn_tactical_3d.default_modules.push_back("tactical_combat_3d");
		template_vn_tactical_3d.default_modules.push_back("save_load_system");
		template_vn_tactical_3d.default_modules.push_back("choice_system");
		template_vn_tactical_3d.default_modules.push_back("visual_novel_main_scene");
		template_vn_tactical_3d.default_modules.push_back("popup_manager");
		template_vn_tactical_3d.default_modules.push_back("hud_builder");
		template_vn_tactical_3d.folder_structure = template_vn_tactical_2d.folder_structure;
		template_vn_tactical_3d.folder_structure.push_back("assets/models/units");
		template_vn_tactical_3d.folder_structure.push_back("assets/models/battlefields");
		template_vn_tactical_3d.folder_structure.push_back("assets/models/environments");
		template_vn_tactical_3d.folder_structure.push_back("assets/textures");
		template_vn_tactical_3d.folder_structure.push_back("assets/materials");
		template_vn_tactical_3d.main_scene_template = "MainVNTactical3D.tscn";
		available_templates.push_back(template_vn_tactical_3d);
	}

	// Social/Relationship Templates
	{
		ProjectTemplate template_social_sim;
		template_social_sim.id = "social_simulation";
		template_social_sim.name = "Social Simulation (Stardew Valley Style)";
		template_social_sim.description = "Complete social simulation with friendship mechanics, gift giving, dating, marriage, festivals, and character schedules. Perfect for farming sims and life simulation games.";
		template_social_sim.category = "Social Simulation";
		template_social_sim.default_modules.push_back("player_controller_2d_topdown");
		template_social_sim.default_modules.push_back("camera_topdown_rpg");
		template_social_sim.default_modules.push_back("relationship_system");
		template_social_sim.default_modules.push_back("social_interaction_system");
		template_social_sim.default_modules.push_back("faction_reputation_system");
		template_social_sim.default_modules.push_back("dialogue_system");
		template_social_sim.default_modules.push_back("inventory_system");
		template_social_sim.default_modules.push_back("quest_system");
		template_social_sim.default_modules.push_back("save_load_system");
		template_social_sim.default_modules.push_back("popup_manager");
		template_social_sim.default_modules.push_back("hud_builder");
		template_social_sim.folder_structure.push_back("scenes/social");
		template_social_sim.folder_structure.push_back("scenes/ui/social");
		template_social_sim.folder_structure.push_back("scenes/locations");
		template_social_sim.folder_structure.push_back("scripts/social");
		template_social_sim.folder_structure.push_back("scripts/relationships");
		template_social_sim.folder_structure.push_back("scripts/ui/social");
		template_social_sim.folder_structure.push_back("scripts/characters");
		template_social_sim.folder_structure.push_back("globals");
		template_social_sim.folder_structure.push_back("assets/portraits");
		template_social_sim.folder_structure.push_back("assets/sprites/characters");
		template_social_sim.folder_structure.push_back("assets/sprites/gifts");
		template_social_sim.folder_structure.push_back("assets/sprites/locations");
		template_social_sim.folder_structure.push_back("assets/sprites/ui");
		template_social_sim.folder_structure.push_back("assets/audio/music");
		template_social_sim.folder_structure.push_back("assets/audio/sfx");
		template_social_sim.folder_structure.push_back("data/relationships");
		template_social_sim.folder_structure.push_back("data/social");
		template_social_sim.folder_structure.push_back("data/factions");
		template_social_sim.folder_structure.push_back("data/characters");
		template_social_sim.folder_structure.push_back("data/dialogues");
		template_social_sim.folder_structure.push_back("data/quests");
		template_social_sim.main_scene_template = "MainSocialSim.tscn";
		available_templates.push_back(template_social_sim);

		ProjectTemplate template_fe_enhanced;
		template_fe_enhanced.id = "fire_emblem_enhanced";
		template_fe_enhanced.name = "Fire Emblem Enhanced (Tactical + Social)";
		template_fe_enhanced.description = "Complete Fire Emblem-style game with tactical combat, support conversations, relationship building, and faction politics. Includes full social mechanics alongside strategic gameplay.";
		template_fe_enhanced.category = "Tactical RPG";
		template_fe_enhanced.default_modules.push_back("tactical_party_system");
		template_fe_enhanced.default_modules.push_back("tactical_combat_2d");
		template_fe_enhanced.default_modules.push_back("relationship_system");
		template_fe_enhanced.default_modules.push_back("support_conversation_system");
		template_fe_enhanced.default_modules.push_back("faction_reputation_system");
		template_fe_enhanced.default_modules.push_back("player_stats");
		template_fe_enhanced.default_modules.push_back("dialogue_system");
		template_fe_enhanced.default_modules.push_back("inventory_system");
		template_fe_enhanced.default_modules.push_back("quest_system");
		template_fe_enhanced.default_modules.push_back("save_load_system");
		template_fe_enhanced.default_modules.push_back("popup_manager");
		template_fe_enhanced.default_modules.push_back("hud_builder");
		template_fe_enhanced.folder_structure.push_back("scenes/tactical");
		template_fe_enhanced.folder_structure.push_back("scenes/social");
		template_fe_enhanced.folder_structure.push_back("scenes/ui/tactical");
		template_fe_enhanced.folder_structure.push_back("scenes/ui/social");
		template_fe_enhanced.folder_structure.push_back("scenes/overworld");
		template_fe_enhanced.folder_structure.push_back("scripts/tactical");
		template_fe_enhanced.folder_structure.push_back("scripts/social");
		template_fe_enhanced.folder_structure.push_back("scripts/relationships");
		template_fe_enhanced.folder_structure.push_back("scripts/support");
		template_fe_enhanced.folder_structure.push_back("scripts/classes");
		template_fe_enhanced.folder_structure.push_back("scripts/ai");
		template_fe_enhanced.folder_structure.push_back("globals");
		template_fe_enhanced.folder_structure.push_back("assets/portraits");
		template_fe_enhanced.folder_structure.push_back("assets/sprites/units");
		template_fe_enhanced.folder_structure.push_back("assets/sprites/battlefields");
		template_fe_enhanced.folder_structure.push_back("assets/sprites/ui");
		template_fe_enhanced.folder_structure.push_back("assets/audio/music");
		template_fe_enhanced.folder_structure.push_back("assets/audio/sfx");
		template_fe_enhanced.folder_structure.push_back("data/relationships");
		template_fe_enhanced.folder_structure.push_back("data/support");
		template_fe_enhanced.folder_structure.push_back("data/factions");
		template_fe_enhanced.folder_structure.push_back("data/classes");
		template_fe_enhanced.folder_structure.push_back("data/units");
		template_fe_enhanced.folder_structure.push_back("data/enemies");
		template_fe_enhanced.folder_structure.push_back("data/battlefields");
		template_fe_enhanced.folder_structure.push_back("data/dialogues");
		template_fe_enhanced.folder_structure.push_back("data/quests");
		template_fe_enhanced.main_scene_template = "MainFEEnhanced.tscn";
		available_templates.push_back(template_fe_enhanced);

		ProjectTemplate template_merchant_sim;
		template_merchant_sim.id = "merchant_simulation";
		template_merchant_sim.name = "Merchant Simulation (Potionomics Style)";
		template_merchant_sim.description = "Business simulation with character relationships, faction reputation, negotiation mechanics, and social interactions. Perfect for shop management and trading games.";
		template_merchant_sim.category = "Business Simulation";
		template_merchant_sim.default_modules.push_back("player_controller_2d_topdown");
		template_merchant_sim.default_modules.push_back("camera_topdown_rpg");
		template_merchant_sim.default_modules.push_back("relationship_system");
		template_merchant_sim.default_modules.push_back("social_interaction_system");
		template_merchant_sim.default_modules.push_back("faction_reputation_system");
		template_merchant_sim.default_modules.push_back("dialogue_system");
		template_merchant_sim.default_modules.push_back("inventory_system");
		template_merchant_sim.default_modules.push_back("shop_system");
		template_merchant_sim.default_modules.push_back("quest_system");
		template_merchant_sim.default_modules.push_back("save_load_system");
		template_merchant_sim.default_modules.push_back("popup_manager");
		template_merchant_sim.default_modules.push_back("hud_builder");
		template_merchant_sim.folder_structure.push_back("scenes/shop");
		template_merchant_sim.folder_structure.push_back("scenes/social");
		template_merchant_sim.folder_structure.push_back("scenes/ui/business");
		template_merchant_sim.folder_structure.push_back("scenes/ui/social");
		template_merchant_sim.folder_structure.push_back("scenes/locations");
		template_merchant_sim.folder_structure.push_back("scripts/business");
		template_merchant_sim.folder_structure.push_back("scripts/social");
		template_merchant_sim.folder_structure.push_back("scripts/relationships");
		template_merchant_sim.folder_structure.push_back("scripts/negotiation");
		template_merchant_sim.folder_structure.push_back("scripts/ui/business");
		template_merchant_sim.folder_structure.push_back("scripts/ui/social");
		template_merchant_sim.folder_structure.push_back("globals");
		template_merchant_sim.folder_structure.push_back("assets/portraits");
		template_merchant_sim.folder_structure.push_back("assets/sprites/characters");
		template_merchant_sim.folder_structure.push_back("assets/sprites/items");
		template_merchant_sim.folder_structure.push_back("assets/sprites/shops");
		template_merchant_sim.folder_structure.push_back("assets/sprites/ui");
		template_merchant_sim.folder_structure.push_back("assets/audio/music");
		template_merchant_sim.folder_structure.push_back("assets/audio/sfx");
		template_merchant_sim.folder_structure.push_back("data/relationships");
		template_merchant_sim.folder_structure.push_back("data/social");
		template_merchant_sim.folder_structure.push_back("data/factions");
		template_merchant_sim.folder_structure.push_back("data/business");
		template_merchant_sim.folder_structure.push_back("data/items");
		template_merchant_sim.folder_structure.push_back("data/characters");
		template_merchant_sim.folder_structure.push_back("data/dialogues");
		template_merchant_sim.folder_structure.push_back("data/quests");
		template_merchant_sim.main_scene_template = "MainMerchantSim.tscn";
		available_templates.push_back(template_merchant_sim);
	}
}

void ProjectDialog::_create_template_project() {
	// This method will be called from ok_pressed() when creating a new project with templates
	// For now, it will create the basic project structure and placeholder files

	String path = _get_target_path();
	if (path.is_empty()) {
		return;
	}

	// Create folder structure based on selected template
	Vector<String> folders = _get_template_folder_structure(selected_template_id);
	Ref<DirAccess> dir = DirAccess::create(DirAccess::ACCESS_FILESYSTEM);

	for (const String &folder : folders) {
		String full_path = path.path_join(folder);
		Error err = dir->make_dir_recursive(full_path);
		if (err != OK) {
			ERR_PRINT("Failed to create directory: " + full_path);
		}
	}

	// Create template-specific files
	_create_template_files(path, selected_template_id);

	// Create module files for selected modules
	for (const String &module_id : selected_modules) {
		for (const ProjectModule &mod : available_modules) {
			if (mod.id == module_id) {
				for (const String &file_path : mod.files_to_create) {
					String full_path = path.path_join(file_path);
					String dir_path = full_path.get_base_dir();

					// Ensure directory exists
					dir->make_dir_recursive(dir_path);

					// Create specialized file content based on module type
					_create_module_file(full_path, mod.id, file_path);
				}
				break;
			}
		}
	}

	// Register autoloads for global scripts
	module_manager->register_autoloads(path, selected_modules);

	// Create main scene based on selected modules
	module_manager->create_main_scene(path, selected_modules);
}

Vector<String> ProjectDialog::_get_template_folder_structure(const String &p_template_id) {
	for (const ProjectTemplate &tmpl : available_templates) {
		if (tmpl.id == p_template_id) {
			return tmpl.folder_structure;
		}
	}
	return Vector<String>();
}

void ProjectDialog::_create_template_files(const String &p_project_path, const String &p_template_id) {
	// Create main scene file based on template
	for (const ProjectTemplate &tmpl : available_templates) {
		if (tmpl.id == p_template_id) {
			String main_scene_path = p_project_path.path_join(tmpl.main_scene_template);
			Ref<FileAccess> file = FileAccess::open(main_scene_path, FileAccess::WRITE);
			if (file.is_valid()) {
				// Create a basic main scene based on template type
				if (tmpl.category.contains("2D")) {
					file->store_line("[gd_scene load_steps=2 format=3]");
					file->store_line("");
					file->store_line("[node name=\"Main\" type=\"Node2D\"]");
					file->store_line("");
					file->store_line("[node name=\"Player\" type=\"CharacterBody2D\" parent=\".\"]");
					file->store_line("");
					file->store_line("[node name=\"Camera2D\" type=\"Camera2D\" parent=\"Player\"]");
				} else if (tmpl.category.contains("3D")) {
					file->store_line("[gd_scene load_steps=2 format=3]");
					file->store_line("");
					file->store_line("[node name=\"Main\" type=\"Node3D\"]");
					file->store_line("");
					file->store_line("[node name=\"Player\" type=\"CharacterBody3D\" parent=\".\"]");
					file->store_line("");
					file->store_line("[node name=\"Camera3D\" type=\"Camera3D\" parent=\"Player\"]");
				} else if (tmpl.category.contains("Visual Novel")) {
					file->store_line("[gd_scene load_steps=2 format=3]");
					file->store_line("");
					file->store_line("[node name=\"Main\" type=\"Control\"]");
					file->store_line("layout_mode = 3");
					file->store_line("anchors_preset = 15");
					file->store_line("");
					file->store_line("[node name=\"DialogueUI\" type=\"Control\" parent=\".\"]");
					file->store_line("layout_mode = 1");
					file->store_line("anchors_preset = 15");
				} else {
					// Default scene
					file->store_line("[gd_scene format=3]");
					file->store_line("");
					file->store_line("[node name=\"Main\" type=\"Node\"]");
				}
				file->close();
			}

			// Create a README file with template information
			String readme_path = p_project_path.path_join("README.md");
			Ref<FileAccess> readme = FileAccess::open(readme_path, FileAccess::WRITE);
			if (readme.is_valid()) {
				readme->store_line("# " + tmpl.name);
				readme->store_line("");
				readme->store_line(tmpl.description);
				readme->store_line("");
				readme->store_line("## Template Category: " + tmpl.category);
				readme->store_line("");
				readme->store_line("## Included Modules:");
				for (const String &module_id : tmpl.default_modules) {
					for (const ProjectModule &mod : available_modules) {
						if (mod.id == module_id) {
							readme->store_line("- **" + mod.name + "**: " + mod.description);
							break;
						}
					}
				}
				readme->store_line("");
				readme->store_line("## Additional Selected Modules:");
				for (const String &module_id : selected_modules) {
					if (!tmpl.default_modules.has(module_id)) {
						for (const ProjectModule &mod : available_modules) {
							if (mod.id == module_id) {
								readme->store_line("- **" + mod.name + "**: " + mod.description);
								break;
							}
						}
					}
				}
				readme->store_line("");
				readme->store_line("---");
				readme->store_line("*Created with Lupine Engine - A narrative-focused game development platform*");
				readme->close();
			}
			break;
		}
	}
}

void ProjectDialog::_create_module_file(const String &p_file_path, const String &p_module_id, const String &p_relative_path) {
	// Use the module manager to create the file
	module_manager->create_module_file(p_file_path, p_module_id, p_relative_path);
}

// Old PlayerStats method removed - now handled by module system

void ProjectDialog::_create_2d_topdown_controller(Ref<FileAccess> p_file) {
	p_file->store_line("# PlayerController2DTopdown.gd");
	p_file->store_line("# Generated by Lupine Engine - 2D Top-down Player Controller");
	p_file->store_line("# 4-directional movement with health, stamina, combat, and interaction");
	p_file->store_line("");
	p_file->store_line("extends CharacterBody2D");
	p_file->store_line("class_name PlayerController2DTopdown");
	p_file->store_line("");
	p_file->store_line("# Movement settings");
	p_file->store_line("@export var base_speed: float = 200.0");
	p_file->store_line("@export var sprint_speed: float = 350.0");
	p_file->store_line("@export var sprint_stamina_cost: float = 30.0  # Per second");
	p_file->store_line("");
	p_file->store_line("# Combat settings");
	p_file->store_line("@export var attack_damage: float = 25.0");
	p_file->store_line("@export var attack_range: float = 50.0");
	p_file->store_line("@export var attack_stamina_cost: float = 15.0");
	p_file->store_line("@export var ranged_attack_damage: float = 20.0");
	p_file->store_line("@export var ranged_attack_range: float = 200.0");
	p_file->store_line("@export var ranged_attack_mana_cost: float = 10.0");
	p_file->store_line("");
	p_file->store_line("# Dash settings");
	p_file->store_line("@export var dash_distance: float = 100.0");
	p_file->store_line("@export var dash_duration: float = 0.2");
	p_file->store_line("@export var dash_stamina_cost: float = 20.0");
	p_file->store_line("@export var dash_cooldown: float = 1.0");
	p_file->store_line("");
	p_file->store_line("# Interaction settings");
	p_file->store_line("@export var interaction_range: float = 80.0");
	p_file->store_line("");
	p_file->store_line("# Node references");
	p_file->store_line("@onready var stats: PlayerStats = $PlayerStats");
	p_file->store_line("@onready var sprite: Sprite2D = $Sprite2D");
	p_file->store_line("@onready var collision: CollisionShape2D = $CollisionShape2D");
	p_file->store_line("@onready var interaction_area: Area2D = $InteractionArea");
	p_file->store_line("");
	p_file->store_line("# State variables");
	p_file->store_line("var current_direction: Vector2 = Vector2.ZERO");
	p_file->store_line("var is_sprinting: bool = false");
	p_file->store_line("var is_dashing: bool = false");
	p_file->store_line("var dash_timer: float = 0.0");
	p_file->store_line("var dash_cooldown_timer: float = 0.0");
	p_file->store_line("var dash_start_pos: Vector2");
	p_file->store_line("var dash_target_pos: Vector2");
	p_file->store_line("");
	p_file->store_line("# Signals");
	p_file->store_line("signal attacked(damage: float, position: Vector2)");
	p_file->store_line("signal ranged_attacked(damage: float, position: Vector2, target_position: Vector2)");
	p_file->store_line("signal interacted(target: Node)");
	p_file->store_line("signal dash_started()");
	p_file->store_line("signal dash_ended()");
	p_file->store_line("");
	p_file->store_line("func _ready():");
	p_file->store_line("\t# Ensure we have required components");
	p_file->store_line("\tif not stats:");
	p_file->store_line("\t\tstats = PlayerStats.new()");
	p_file->store_line("\t\tadd_child(stats)");
	p_file->store_line("\t");
	p_file->store_line("\t# Connect to stat signals for UI updates");
	p_file->store_line("\tstats.stat_depleted.connect(_on_stat_depleted)");
	p_file->store_line("");
	p_file->store_line("func _physics_process(delta: float):");
	p_file->store_line("\tif stats.is_dead:");
	p_file->store_line("\t\treturn");
	p_file->store_line("\t");
	p_file->store_line("\t# Handle dash");
	p_file->store_line("\tif is_dashing:");
	p_file->store_line("\t\t_handle_dash(delta)");
	p_file->store_line("\t\treturn");
	p_file->store_line("\t");
	p_file->store_line("\t# Update cooldowns");
	p_file->store_line("\tif dash_cooldown_timer > 0:");
	p_file->store_line("\t\tdash_cooldown_timer -= delta");
	p_file->store_line("\t");
	p_file->store_line("\t# Handle input");
	p_file->store_line("\t_handle_movement_input(delta)");
	p_file->store_line("\t_handle_action_input()");
	p_file->store_line("\t");
	p_file->store_line("\t# Apply movement");
	p_file->store_line("\tmove_and_slide()");
	p_file->store_line("");
	p_file->store_line("func _handle_movement_input(delta: float):");
	p_file->store_line("\t# Get input direction (4-directional)");
	p_file->store_line("\tvar input_dir = Vector2.ZERO");
	p_file->store_line("\t");
	p_file->store_line("\tif Input.is_action_pressed(\"move_up\") or Input.is_action_pressed(\"ui_up\"):");
	p_file->store_line("\t\tinput_dir.y -= 1");
	p_file->store_line("\tif Input.is_action_pressed(\"move_down\") or Input.is_action_pressed(\"ui_down\"):");
	p_file->store_line("\t\tinput_dir.y += 1");
	p_file->store_line("\tif Input.is_action_pressed(\"move_left\") or Input.is_action_pressed(\"ui_left\"):");
	p_file->store_line("\t\tinput_dir.x -= 1");
	p_file->store_line("\tif Input.is_action_pressed(\"move_right\") or Input.is_action_pressed(\"ui_right\"):");
	p_file->store_line("\t\tinput_dir.x += 1");
	p_file->store_line("\t");
	p_file->store_line("\t# Prioritize one direction for 4-directional movement");
	p_file->store_line("\tif input_dir.x != 0:");
	p_file->store_line("\t\tinput_dir.y = 0");
	p_file->store_line("\t");
	p_file->store_line("\tcurrent_direction = input_dir");
	p_file->store_line("\t");
	p_file->store_line("\t# Handle sprinting");
	p_file->store_line("\tis_sprinting = Input.is_action_pressed(\"sprint\") and input_dir != Vector2.ZERO");
	p_file->store_line("\tif is_sprinting and not stats.use_stamina(sprint_stamina_cost * delta):");
	p_file->store_line("\t\tis_sprinting = false");
	p_file->store_line("\t");
	p_file->store_line("\t# Calculate speed");
	p_file->store_line("\tvar current_speed = sprint_speed if is_sprinting else base_speed");
	p_file->store_line("\t");
	p_file->store_line("\t# Apply velocity");
	p_file->store_line("\tvelocity = input_dir * current_speed");
	p_file->store_line("");
	p_file->store_line("func _handle_action_input():");
	p_file->store_line("\t# Attack");
	p_file->store_line("\tif Input.is_action_just_pressed(\"attack\"):");
	p_file->store_line("\t\tperform_attack()");
	p_file->store_line("\t");
	p_file->store_line("\t# Ranged attack");
	p_file->store_line("\tif Input.is_action_just_pressed(\"ranged_attack\"):");
	p_file->store_line("\t\tperform_ranged_attack()");
	p_file->store_line("\t");
	p_file->store_line("\t# Dash");
	p_file->store_line("\tif Input.is_action_just_pressed(\"dash\"):");
	p_file->store_line("\t\tperform_dash()");
	p_file->store_line("\t");
	p_file->store_line("\t# Interact");
	p_file->store_line("\tif Input.is_action_just_pressed(\"interact\"):");
	p_file->store_line("\t\tperform_interaction()");
	p_file->store_line("");
	p_file->store_line("func perform_attack():");
	p_file->store_line("\tif not stats.use_stamina(attack_stamina_cost):");
	p_file->store_line("\t\treturn");
	p_file->store_line("\t");
	p_file->store_line("\t# Calculate attack position based on direction");
	p_file->store_line("\tvar attack_pos = global_position");
	p_file->store_line("\tif current_direction != Vector2.ZERO:");
	p_file->store_line("\t\tattack_pos += current_direction * attack_range");
	p_file->store_line("\telse:");
	p_file->store_line("\t\tattack_pos += Vector2.DOWN * attack_range  # Default direction");
	p_file->store_line("\t");
	p_file->store_line("\tattacked.emit(attack_damage, attack_pos)");
	p_file->store_line("");
	p_file->store_line("func perform_ranged_attack():");
	p_file->store_line("\tif not stats.use_mana(ranged_attack_mana_cost):");
	p_file->store_line("\t\treturn");
	p_file->store_line("\t");
	p_file->store_line("\t# Calculate target position");
	p_file->store_line("\tvar target_pos = global_position");
	p_file->store_line("\tif current_direction != Vector2.ZERO:");
	p_file->store_line("\t\ttarget_pos += current_direction * ranged_attack_range");
	p_file->store_line("\telse:");
	p_file->store_line("\t\ttarget_pos += Vector2.DOWN * ranged_attack_range  # Default direction");
	p_file->store_line("\t");
	p_file->store_line("\tranged_attacked.emit(ranged_attack_damage, global_position, target_pos)");
	p_file->store_line("");
	p_file->store_line("func perform_dash():");
	p_file->store_line("\tif is_dashing or dash_cooldown_timer > 0:");
	p_file->store_line("\t\treturn");
	p_file->store_line("\t");
	p_file->store_line("\tif not stats.use_stamina(dash_stamina_cost):");
	p_file->store_line("\t\treturn");
	p_file->store_line("\t");
	p_file->store_line("\t# Calculate dash direction");
	p_file->store_line("\tvar dash_dir = current_direction");
	p_file->store_line("\tif dash_dir == Vector2.ZERO:");
	p_file->store_line("\t\tdash_dir = Vector2.DOWN  # Default direction");
	p_file->store_line("\t");
	p_file->store_line("\t# Set up dash");
	p_file->store_line("\tis_dashing = true");
	p_file->store_line("\tdash_timer = dash_duration");
	p_file->store_line("\tdash_cooldown_timer = dash_cooldown");
	p_file->store_line("\tdash_start_pos = global_position");
	p_file->store_line("\tdash_target_pos = global_position + dash_dir * dash_distance");
	p_file->store_line("\t");
	p_file->store_line("\tdash_started.emit()");
	p_file->store_line("");
	p_file->store_line("func _handle_dash(delta: float):");
	p_file->store_line("\tdash_timer -= delta");
	p_file->store_line("\t");
	p_file->store_line("\tif dash_timer <= 0:");
	p_file->store_line("\t\tis_dashing = false");
	p_file->store_line("\t\tglobal_position = dash_target_pos");
	p_file->store_line("\t\tvelocity = Vector2.ZERO");
	p_file->store_line("\t\tdash_ended.emit()");
	p_file->store_line("\telse:");
	p_file->store_line("\t\t# Interpolate position during dash");
	p_file->store_line("\t\tvar progress = 1.0 - (dash_timer / dash_duration)");
	p_file->store_line("\t\tglobal_position = dash_start_pos.lerp(dash_target_pos, progress)");
	p_file->store_line("\t\tvelocity = Vector2.ZERO");
	p_file->store_line("");
	p_file->store_line("func perform_interaction():");
	p_file->store_line("\t# Find interactable objects in range");
	p_file->store_line("\tvar space_state = get_world_2d().direct_space_state");
	p_file->store_line("\tvar query = PhysicsPointQueryParameters2D.new()");
	p_file->store_line("\tquery.position = global_position");
	p_file->store_line("\tquery.collision_mask = 1  # Adjust collision mask as needed");
	p_file->store_line("\t");
	p_file->store_line("\tvar results = space_state.intersect_point(query, 10)");
	p_file->store_line("\tfor result in results:");
	p_file->store_line("\t\tvar collider = result.collider");
	p_file->store_line("\t\tif collider.has_method(\"interact\"):");
	p_file->store_line("\t\t\tvar distance = global_position.distance_to(collider.global_position)");
	p_file->store_line("\t\t\tif distance <= interaction_range:");
	p_file->store_line("\t\t\t\tinteracted.emit(collider)");
	p_file->store_line("\t\t\t\tcollider.interact()");
	p_file->store_line("\t\t\t\tbreak");
	p_file->store_line("");
	p_file->store_line("func _on_stat_depleted(stat_name: String):");
	p_file->store_line("\tif stat_name == \"health\":");
	p_file->store_line("\t\t# Handle death");
	p_file->store_line("\t\tvelocity = Vector2.ZERO");
	p_file->store_line("\t\tprint(\"Player died!\")");
	p_file->store_line("\telif stat_name == \"stamina\":");
	p_file->store_line("\t\t# Handle stamina depletion");
	p_file->store_line("\t\tis_sprinting = false");
	p_file->store_line("\t\tprint(\"Out of stamina!\")");
}

void ProjectDialog::_create_2d_topdown_8dir_controller(Ref<FileAccess> p_file) {
	p_file->store_line("# PlayerController2DTopdown8Dir.gd");
	p_file->store_line("# Generated by Lupine Engine - 2D Top-down Player Controller (8-Direction)");
	p_file->store_line("# 8-directional movement with health, stamina, combat, and interaction");
	p_file->store_line("");
	p_file->store_line("extends CharacterBody2D");
	p_file->store_line("class_name PlayerController2DTopdown8Dir");
	p_file->store_line("");
	p_file->store_line("# Movement settings");
	p_file->store_line("@export var base_speed: float = 200.0");
	p_file->store_line("@export var sprint_speed: float = 350.0");
	p_file->store_line("@export var sprint_stamina_cost: float = 30.0");
	p_file->store_line("");
	p_file->store_line("# Combat and other settings (same as 4-directional)");
	p_file->store_line("@export var attack_damage: float = 25.0");
	p_file->store_line("@export var attack_range: float = 50.0");
	p_file->store_line("@export var attack_stamina_cost: float = 15.0");
	p_file->store_line("@export var dash_distance: float = 100.0");
	p_file->store_line("@export var dash_duration: float = 0.2");
	p_file->store_line("@export var dash_stamina_cost: float = 20.0");
	p_file->store_line("@export var interaction_range: float = 80.0");
	p_file->store_line("");
	p_file->store_line("# Node references");
	p_file->store_line("@onready var stats: PlayerStats = $PlayerStats");
	p_file->store_line("@onready var sprite: Sprite2D = $Sprite2D");
	p_file->store_line("");
	p_file->store_line("# State variables");
	p_file->store_line("var current_direction: Vector2 = Vector2.ZERO");
	p_file->store_line("var is_sprinting: bool = false");
	p_file->store_line("var is_dashing: bool = false");
	p_file->store_line("var dash_timer: float = 0.0");
	p_file->store_line("var dash_cooldown_timer: float = 0.0");
	p_file->store_line("");
	p_file->store_line("func _ready():");
	p_file->store_line("\tif not stats:");
	p_file->store_line("\t\tstats = PlayerStats.new()");
	p_file->store_line("\t\tadd_child(stats)");
	p_file->store_line("");
	p_file->store_line("func _physics_process(delta: float):");
	p_file->store_line("\tif stats.is_dead or is_dashing:");
	p_file->store_line("\t\treturn");
	p_file->store_line("\t");
	p_file->store_line("\t# Handle input and movement (8-directional)");
	p_file->store_line("\tvar input_dir = Vector2.ZERO");
	p_file->store_line("\t");
	p_file->store_line("\tif Input.is_action_pressed(\"move_up\"):");
	p_file->store_line("\t\tinput_dir.y -= 1");
	p_file->store_line("\tif Input.is_action_pressed(\"move_down\"):");
	p_file->store_line("\t\tinput_dir.y += 1");
	p_file->store_line("\tif Input.is_action_pressed(\"move_left\"):");
	p_file->store_line("\t\tinput_dir.x -= 1");
	p_file->store_line("\tif Input.is_action_pressed(\"move_right\"):");
	p_file->store_line("\t\tinput_dir.x += 1");
	p_file->store_line("\t");
	p_file->store_line("\t# Normalize for 8-directional movement");
	p_file->store_line("\tinput_dir = input_dir.normalized()");
	p_file->store_line("\tcurrent_direction = input_dir");
	p_file->store_line("\t");
	p_file->store_line("\t# Handle sprinting");
	p_file->store_line("\tis_sprinting = Input.is_action_pressed(\"sprint\") and input_dir != Vector2.ZERO");
	p_file->store_line("\tif is_sprinting and not stats.use_stamina(sprint_stamina_cost * delta):");
	p_file->store_line("\t\tis_sprinting = false");
	p_file->store_line("\t");
	p_file->store_line("\t# Apply movement");
	p_file->store_line("\tvar current_speed = sprint_speed if is_sprinting else base_speed");
	p_file->store_line("\tvelocity = input_dir * current_speed");
	p_file->store_line("\tmove_and_slide()");
	p_file->store_line("\t");
	p_file->store_line("\t# Handle actions");
	p_file->store_line("\tif Input.is_action_just_pressed(\"attack\"):");
	p_file->store_line("\t\tperform_attack()");
	p_file->store_line("\tif Input.is_action_just_pressed(\"dash\"):");
	p_file->store_line("\t\tperform_dash()");
	p_file->store_line("\tif Input.is_action_just_pressed(\"interact\"):");
	p_file->store_line("\t\tperform_interaction()");
	p_file->store_line("");
	p_file->store_line("# Attack, dash, and interaction methods (similar to 4-directional)");
	p_file->store_line("func perform_attack():");
	p_file->store_line("\tif stats.use_stamina(attack_stamina_cost):");
	p_file->store_line("\t\tvar attack_pos = global_position + current_direction * attack_range");
	p_file->store_line("\t\tprint(\"Attack at: \", attack_pos)");
	p_file->store_line("");
	p_file->store_line("func perform_dash():");
	p_file->store_line("\tif not is_dashing and stats.use_stamina(dash_stamina_cost):");
	p_file->store_line("\t\tvar dash_dir = current_direction if current_direction != Vector2.ZERO else Vector2.DOWN");
	p_file->store_line("\t\tglobal_position += dash_dir * dash_distance");
	p_file->store_line("\t\tprint(\"Dashed!\")");
	p_file->store_line("");
	p_file->store_line("func perform_interaction():");
	p_file->store_line("\tprint(\"Interaction attempted\")");
}

void ProjectDialog::_create_2d_platformer_controller(Ref<FileAccess> p_file) {
	p_file->store_line("# PlayerController2DPlatformer.gd");
	p_file->store_line("# Generated by Lupine Engine - 2D Platformer Player Controller");
	p_file->store_line("# Side-scrolling platformer with jumping, health, stamina, combat, and interaction");
	p_file->store_line("");
	p_file->store_line("extends CharacterBody2D");
	p_file->store_line("class_name PlayerController2DPlatformer");
	p_file->store_line("");
	p_file->store_line("# Movement settings");
	p_file->store_line("@export var base_speed: float = 200.0");
	p_file->store_line("@export var sprint_speed: float = 350.0");
	p_file->store_line("@export var jump_velocity: float = -400.0");
	p_file->store_line("@export var gravity: float = 980.0");
	p_file->store_line("@export var sprint_stamina_cost: float = 30.0");
	p_file->store_line("@export var jump_stamina_cost: float = 10.0");
	p_file->store_line("");
	p_file->store_line("# Combat settings");
	p_file->store_line("@export var attack_damage: float = 25.0");
	p_file->store_line("@export var attack_range: float = 50.0");
	p_file->store_line("@export var attack_stamina_cost: float = 15.0");
	p_file->store_line("@export var dash_distance: float = 100.0");
	p_file->store_line("@export var dash_stamina_cost: float = 20.0");
	p_file->store_line("@export var interaction_range: float = 80.0");
	p_file->store_line("");
	p_file->store_line("# Node references");
	p_file->store_line("@onready var stats: PlayerStats = $PlayerStats");
	p_file->store_line("@onready var sprite: Sprite2D = $Sprite2D");
	p_file->store_line("");
	p_file->store_line("# State variables");
	p_file->store_line("var facing_direction: int = 1  # 1 for right, -1 for left");
	p_file->store_line("var is_sprinting: bool = false");
	p_file->store_line("var is_dashing: bool = false");
	p_file->store_line("var coyote_time: float = 0.1");
	p_file->store_line("var coyote_timer: float = 0.0");
	p_file->store_line("var jump_buffer_time: float = 0.1");
	p_file->store_line("var jump_buffer_timer: float = 0.0");
	p_file->store_line("");
	p_file->store_line("func _ready():");
	p_file->store_line("\tif not stats:");
	p_file->store_line("\t\tstats = PlayerStats.new()");
	p_file->store_line("\t\tadd_child(stats)");
	p_file->store_line("");
	p_file->store_line("func _physics_process(delta: float):");
	p_file->store_line("\tif stats.is_dead:");
	p_file->store_line("\t\treturn");
	p_file->store_line("\t");
	p_file->store_line("\t# Handle gravity");
	p_file->store_line("\tif not is_on_floor():");
	p_file->store_line("\t\tvelocity.y += gravity * delta");
	p_file->store_line("\t\tcoyote_timer -= delta");
	p_file->store_line("\telse:");
	p_file->store_line("\t\tcoyote_timer = coyote_time");
	p_file->store_line("\t");
	p_file->store_line("\t# Handle jump buffer");
	p_file->store_line("\tif jump_buffer_timer > 0:");
	p_file->store_line("\t\tjump_buffer_timer -= delta");
	p_file->store_line("\t");
	p_file->store_line("\t# Handle horizontal movement");
	p_file->store_line("\tvar input_dir = Input.get_axis(\"move_left\", \"move_right\")");
	p_file->store_line("\t");
	p_file->store_line("\tif input_dir != 0:");
	p_file->store_line("\t\tfacing_direction = int(sign(input_dir))");
	p_file->store_line("\t\tsprite.scale.x = facing_direction");
	p_file->store_line("\t");
	p_file->store_line("\t# Handle sprinting");
	p_file->store_line("\tis_sprinting = Input.is_action_pressed(\"sprint\") and input_dir != 0");
	p_file->store_line("\tif is_sprinting and not stats.use_stamina(sprint_stamina_cost * delta):");
	p_file->store_line("\t\tis_sprinting = false");
	p_file->store_line("\t");
	p_file->store_line("\t# Apply horizontal movement");
	p_file->store_line("\tvar current_speed = sprint_speed if is_sprinting else base_speed");
	p_file->store_line("\tvelocity.x = input_dir * current_speed");
	p_file->store_line("\t");
	p_file->store_line("\t# Handle jumping");
	p_file->store_line("\tif Input.is_action_just_pressed(\"jump\"):");
	p_file->store_line("\t\tjump_buffer_timer = jump_buffer_time");
	p_file->store_line("\t");
	p_file->store_line("\tif jump_buffer_timer > 0 and (is_on_floor() or coyote_timer > 0):");
	p_file->store_line("\t\tif stats.use_stamina(jump_stamina_cost):");
	p_file->store_line("\t\t\tvelocity.y = jump_velocity");
	p_file->store_line("\t\t\tjump_buffer_timer = 0");
	p_file->store_line("\t\t\tcoyote_timer = 0");
	p_file->store_line("\t");
	p_file->store_line("\t# Handle actions");
	p_file->store_line("\tif Input.is_action_just_pressed(\"attack\"):");
	p_file->store_line("\t\tperform_attack()");
	p_file->store_line("\tif Input.is_action_just_pressed(\"dash\"):");
	p_file->store_line("\t\tperform_dash()");
	p_file->store_line("\tif Input.is_action_just_pressed(\"interact\"):");
	p_file->store_line("\t\tperform_interaction()");
	p_file->store_line("\t");
	p_file->store_line("\tmove_and_slide()");
	p_file->store_line("");
	p_file->store_line("func perform_attack():");
	p_file->store_line("\tif stats.use_stamina(attack_stamina_cost):");
	p_file->store_line("\t\tvar attack_pos = global_position + Vector2(facing_direction * attack_range, 0)");
	p_file->store_line("\t\tprint(\"Attack at: \", attack_pos)");
	p_file->store_line("");
	p_file->store_line("func perform_dash():");
	p_file->store_line("\tif not is_dashing and stats.use_stamina(dash_stamina_cost):");
	p_file->store_line("\t\tglobal_position.x += facing_direction * dash_distance");
	p_file->store_line("\t\tprint(\"Dashed!\")");
	p_file->store_line("");
	p_file->store_line("func perform_interaction():");
	p_file->store_line("\tprint(\"Interaction attempted\")");
}

void ProjectDialog::_create_3d_third_person_controller(Ref<FileAccess> p_file) {
	p_file->store_line("# PlayerController3DThirdPerson.gd");
	p_file->store_line("# Generated by Lupine Engine - 3D Third-Person Player Controller");
	p_file->store_line("# Third-person 3D movement with camera follow, health, stamina, and combat");
	p_file->store_line("");
	p_file->store_line("extends CharacterBody3D");
	p_file->store_line("class_name PlayerController3DThirdPerson");
	p_file->store_line("");
	p_file->store_line("# Movement settings");
	p_file->store_line("@export var base_speed: float = 5.0");
	p_file->store_line("@export var sprint_speed: float = 8.0");
	p_file->store_line("@export var jump_velocity: float = 8.0");
	p_file->store_line("@export var gravity: float = 20.0");
	p_file->store_line("@export var sprint_stamina_cost: float = 30.0");
	p_file->store_line("@export var jump_stamina_cost: float = 15.0");
	p_file->store_line("");
	p_file->store_line("# Combat settings");
	p_file->store_line("@export var attack_damage: float = 25.0");
	p_file->store_line("@export var attack_range: float = 2.0");
	p_file->store_line("@export var attack_stamina_cost: float = 15.0");
	p_file->store_line("@export var dash_distance: float = 5.0");
	p_file->store_line("@export var dash_stamina_cost: float = 20.0");
	p_file->store_line("@export var interaction_range: float = 3.0");
	p_file->store_line("");
	p_file->store_line("# Camera settings");
	p_file->store_line("@export var mouse_sensitivity: float = 0.002");
	p_file->store_line("@export var camera_distance: float = 5.0");
	p_file->store_line("@export var camera_height: float = 2.0");
	p_file->store_line("");
	p_file->store_line("# Node references");
	p_file->store_line("@onready var stats: PlayerStats = $PlayerStats");
	p_file->store_line("@onready var mesh: MeshInstance3D = $MeshInstance3D");
	p_file->store_line("@onready var camera_pivot: Node3D = $CameraPivot");
	p_file->store_line("@onready var camera: Camera3D = $CameraPivot/Camera3D");
	p_file->store_line("");
	p_file->store_line("# State variables");
	p_file->store_line("var camera_rotation: Vector2 = Vector2.ZERO");
	p_file->store_line("var is_sprinting: bool = false");
	p_file->store_line("var is_dashing: bool = false");
	p_file->store_line("");
	p_file->store_line("func _ready():");
	p_file->store_line("\tif not stats:");
	p_file->store_line("\t\tstats = PlayerStats.new()");
	p_file->store_line("\t\tadd_child(stats)");
	p_file->store_line("\t");
	p_file->store_line("\t# Set up camera");
	p_file->store_line("\tcamera.position = Vector3(0, camera_height, camera_distance)");
	p_file->store_line("\tcamera.look_at(global_position, Vector3.UP)");
	p_file->store_line("\t");
	p_file->store_line("\t# Capture mouse");
	p_file->store_line("\tInput.mouse_mode = Input.MOUSE_MODE_CAPTURED");
	p_file->store_line("");
	p_file->store_line("func _input(event):");
	p_file->store_line("\tif event is InputEventMouseMotion and Input.mouse_mode == Input.MOUSE_MODE_CAPTURED:");
	p_file->store_line("\t\tcamera_rotation.x -= event.relative.y * mouse_sensitivity");
	p_file->store_line("\t\tcamera_rotation.y -= event.relative.x * mouse_sensitivity");
	p_file->store_line("\t\tcamera_rotation.x = clamp(camera_rotation.x, -PI/3, PI/3)");
	p_file->store_line("\t");
	p_file->store_line("\tif Input.is_action_just_pressed(\"ui_cancel\"):");
	p_file->store_line("\t\tInput.mouse_mode = Input.MOUSE_MODE_VISIBLE if Input.mouse_mode == Input.MOUSE_MODE_CAPTURED else Input.MOUSE_MODE_CAPTURED");
	p_file->store_line("");
	p_file->store_line("func _physics_process(delta: float):");
	p_file->store_line("\tif stats.is_dead:");
	p_file->store_line("\t\treturn");
	p_file->store_line("\t");
	p_file->store_line("\t# Handle gravity");
	p_file->store_line("\tif not is_on_floor():");
	p_file->store_line("\t\tvelocity.y -= gravity * delta");
	p_file->store_line("\t");
	p_file->store_line("\t# Handle jumping");
	p_file->store_line("\tif Input.is_action_just_pressed(\"jump\") and is_on_floor():");
	p_file->store_line("\t\tif stats.use_stamina(jump_stamina_cost):");
	p_file->store_line("\t\t\tvelocity.y = jump_velocity");
	p_file->store_line("\t");
	p_file->store_line("\t# Handle movement");
	p_file->store_line("\tvar input_dir = Input.get_vector(\"move_left\", \"move_right\", \"move_up\", \"move_down\")");
	p_file->store_line("\tvar direction = (transform.basis * Vector3(input_dir.x, 0, input_dir.y)).normalized()");
	p_file->store_line("\t");
	p_file->store_line("\t# Rotate direction based on camera");
	p_file->store_line("\tif direction != Vector3.ZERO:");
	p_file->store_line("\t\tdirection = direction.rotated(Vector3.UP, camera_rotation.y)");
	p_file->store_line("\t");
	p_file->store_line("\t# Handle sprinting");
	p_file->store_line("\tis_sprinting = Input.is_action_pressed(\"sprint\") and direction != Vector3.ZERO");
	p_file->store_line("\tif is_sprinting and not stats.use_stamina(sprint_stamina_cost * delta):");
	p_file->store_line("\t\tis_sprinting = false");
	p_file->store_line("\t");
	p_file->store_line("\t# Apply movement");
	p_file->store_line("\tvar current_speed = sprint_speed if is_sprinting else base_speed");
	p_file->store_line("\tvelocity.x = direction.x * current_speed");
	p_file->store_line("\tvelocity.z = direction.z * current_speed");
	p_file->store_line("\t");
	p_file->store_line("\t# Rotate player to face movement direction");
	p_file->store_line("\tif direction != Vector3.ZERO:");
	p_file->store_line("\t\tlook_at(global_position + direction, Vector3.UP)");
	p_file->store_line("\t");
	p_file->store_line("\t# Update camera");
	p_file->store_line("\tcamera_pivot.rotation.x = camera_rotation.x");
	p_file->store_line("\tcamera_pivot.rotation.y = camera_rotation.y");
	p_file->store_line("\t");
	p_file->store_line("\t# Handle actions");
	p_file->store_line("\tif Input.is_action_just_pressed(\"attack\"):");
	p_file->store_line("\t\tperform_attack()");
	p_file->store_line("\tif Input.is_action_just_pressed(\"dash\"):");
	p_file->store_line("\t\tperform_dash()");
	p_file->store_line("\tif Input.is_action_just_pressed(\"interact\"):");
	p_file->store_line("\t\tperform_interaction()");
	p_file->store_line("\t");
	p_file->store_line("\tmove_and_slide()");
	p_file->store_line("");
	p_file->store_line("func perform_attack():");
	p_file->store_line("\tif stats.use_stamina(attack_stamina_cost):");
	p_file->store_line("\t\tvar attack_pos = global_position + -transform.basis.z * attack_range");
	p_file->store_line("\t\tprint(\"3D Attack at: \", attack_pos)");
	p_file->store_line("");
	p_file->store_line("func perform_dash():");
	p_file->store_line("\tif not is_dashing and stats.use_stamina(dash_stamina_cost):");
	p_file->store_line("\t\tvar dash_dir = -transform.basis.z");
	p_file->store_line("\t\tglobal_position += dash_dir * dash_distance");
	p_file->store_line("\t\tprint(\"3D Dashed!\")");
	p_file->store_line("");
	p_file->store_line("func perform_interaction():");
	p_file->store_line("\tprint(\"3D Interaction attempted\")");
}

void ProjectDialog::_create_3d_first_person_controller(Ref<FileAccess> p_file) {
	p_file->store_line("# PlayerController3DFirstPerson.gd");
	p_file->store_line("# Generated by Lupine Engine - 3D First-Person Player Controller");
	p_file->store_line("# First-person 3D movement with mouse look, health, stamina, and combat");
	p_file->store_line("");
	p_file->store_line("extends CharacterBody3D");
	p_file->store_line("class_name PlayerController3DFirstPerson");
	p_file->store_line("");
	p_file->store_line("# Movement settings");
	p_file->store_line("@export var base_speed: float = 5.0");
	p_file->store_line("@export var sprint_speed: float = 8.0");
	p_file->store_line("@export var jump_velocity: float = 8.0");
	p_file->store_line("@export var gravity: float = 20.0");
	p_file->store_line("@export var sprint_stamina_cost: float = 30.0");
	p_file->store_line("@export var jump_stamina_cost: float = 15.0");
	p_file->store_line("");
	p_file->store_line("# Combat settings");
	p_file->store_line("@export var attack_damage: float = 25.0");
	p_file->store_line("@export var attack_range: float = 2.0");
	p_file->store_line("@export var attack_stamina_cost: float = 15.0");
	p_file->store_line("@export var dash_distance: float = 5.0");
	p_file->store_line("@export var dash_stamina_cost: float = 20.0");
	p_file->store_line("@export var interaction_range: float = 3.0");
	p_file->store_line("");
	p_file->store_line("# Camera settings");
	p_file->store_line("@export var mouse_sensitivity: float = 0.002");
	p_file->store_line("@export var camera_height: float = 1.8");
	p_file->store_line("");
	p_file->store_line("# Node references");
	p_file->store_line("@onready var stats: PlayerStats = $PlayerStats");
	p_file->store_line("@onready var camera: Camera3D = $Camera3D");
	p_file->store_line("@onready var collision: CollisionShape3D = $CollisionShape3D");
	p_file->store_line("");
	p_file->store_line("# State variables");
	p_file->store_line("var camera_rotation: Vector2 = Vector2.ZERO");
	p_file->store_line("var is_sprinting: bool = false");
	p_file->store_line("var is_dashing: bool = false");
	p_file->store_line("");
	p_file->store_line("func _ready():");
	p_file->store_line("\tif not stats:");
	p_file->store_line("\t\tstats = PlayerStats.new()");
	p_file->store_line("\t\tadd_child(stats)");
	p_file->store_line("\t");
	p_file->store_line("\t# Set up camera");
	p_file->store_line("\tcamera.position.y = camera_height");
	p_file->store_line("\t");
	p_file->store_line("\t# Capture mouse");
	p_file->store_line("\tInput.mouse_mode = Input.MOUSE_MODE_CAPTURED");
	p_file->store_line("");
	p_file->store_line("func _input(event):");
	p_file->store_line("\tif event is InputEventMouseMotion and Input.mouse_mode == Input.MOUSE_MODE_CAPTURED:");
	p_file->store_line("\t\tcamera_rotation.x -= event.relative.y * mouse_sensitivity");
	p_file->store_line("\t\tcamera_rotation.y -= event.relative.x * mouse_sensitivity");
	p_file->store_line("\t\tcamera_rotation.x = clamp(camera_rotation.x, -PI/2, PI/2)");
	p_file->store_line("\t");
	p_file->store_line("\tif Input.is_action_just_pressed(\"ui_cancel\"):");
	p_file->store_line("\t\tInput.mouse_mode = Input.MOUSE_MODE_VISIBLE if Input.mouse_mode == Input.MOUSE_MODE_CAPTURED else Input.MOUSE_MODE_CAPTURED");
	p_file->store_line("");
	p_file->store_line("func _physics_process(delta: float):");
	p_file->store_line("\tif stats.is_dead:");
	p_file->store_line("\t\treturn");
	p_file->store_line("\t");
	p_file->store_line("\t# Apply camera rotation");
	p_file->store_line("\tcamera.rotation.x = camera_rotation.x");
	p_file->store_line("\trotation.y = camera_rotation.y");
	p_file->store_line("\t");
	p_file->store_line("\t# Handle gravity");
	p_file->store_line("\tif not is_on_floor():");
	p_file->store_line("\t\tvelocity.y -= gravity * delta");
	p_file->store_line("\t");
	p_file->store_line("\t# Handle jumping");
	p_file->store_line("\tif Input.is_action_just_pressed(\"jump\") and is_on_floor():");
	p_file->store_line("\t\tif stats.use_stamina(jump_stamina_cost):");
	p_file->store_line("\t\t\tvelocity.y = jump_velocity");
	p_file->store_line("\t");
	p_file->store_line("\t# Handle movement");
	p_file->store_line("\tvar input_dir = Input.get_vector(\"move_left\", \"move_right\", \"move_up\", \"move_down\")");
	p_file->store_line("\tvar direction = (transform.basis * Vector3(input_dir.x, 0, input_dir.y)).normalized()");
	p_file->store_line("\t");
	p_file->store_line("\t# Handle sprinting");
	p_file->store_line("\tis_sprinting = Input.is_action_pressed(\"sprint\") and direction != Vector3.ZERO");
	p_file->store_line("\tif is_sprinting and not stats.use_stamina(sprint_stamina_cost * delta):");
	p_file->store_line("\t\tis_sprinting = false");
	p_file->store_line("\t");
	p_file->store_line("\t# Apply movement");
	p_file->store_line("\tvar current_speed = sprint_speed if is_sprinting else base_speed");
	p_file->store_line("\tvelocity.x = direction.x * current_speed");
	p_file->store_line("\tvelocity.z = direction.z * current_speed");
	p_file->store_line("\t");
	p_file->store_line("\t# Handle actions");
	p_file->store_line("\tif Input.is_action_just_pressed(\"attack\"):");
	p_file->store_line("\t\tperform_attack()");
	p_file->store_line("\tif Input.is_action_just_pressed(\"dash\"):");
	p_file->store_line("\t\tperform_dash()");
	p_file->store_line("\tif Input.is_action_just_pressed(\"interact\"):");
	p_file->store_line("\t\tperform_interaction()");
	p_file->store_line("\t");
	p_file->store_line("\tmove_and_slide()");
	p_file->store_line("");
	p_file->store_line("func perform_attack():");
	p_file->store_line("\tif stats.use_stamina(attack_stamina_cost):");
	p_file->store_line("\t\t# Raycast from camera to detect targets");
	p_file->store_line("\t\tvar space_state = get_world_3d().direct_space_state");
	p_file->store_line("\t\tvar query = PhysicsRayQueryParameters3D.create(camera.global_position, camera.global_position + -camera.global_transform.basis.z * attack_range)");
	p_file->store_line("\t\tvar result = space_state.intersect_ray(query)");
	p_file->store_line("\t\tif result:");
	p_file->store_line("\t\t\tprint(\"FPS Attack hit: \", result.collider)");
	p_file->store_line("\t\telse:");
	p_file->store_line("\t\t\tprint(\"FPS Attack missed\")");
	p_file->store_line("");
	p_file->store_line("func perform_dash():");
	p_file->store_line("\tif not is_dashing and stats.use_stamina(dash_stamina_cost):");
	p_file->store_line("\t\tvar dash_dir = -transform.basis.z");
	p_file->store_line("\t\tglobal_position += dash_dir * dash_distance");
	p_file->store_line("\t\tprint(\"FPS Dashed!\")");
	p_file->store_line("");
	p_file->store_line("func perform_interaction():");
	p_file->store_line("\t# Raycast from camera to detect interactables");
	p_file->store_line("\tvar space_state = get_world_3d().direct_space_state");
	p_file->store_line("\tvar query = PhysicsRayQueryParameters3D.create(camera.global_position, camera.global_position + -camera.global_transform.basis.z * interaction_range)");
	p_file->store_line("\tvar result = space_state.intersect_ray(query)");
	p_file->store_line("\tif result and result.collider.has_method(\"interact\"):");
	p_file->store_line("\t\tresult.collider.interact()");
	p_file->store_line("\t\tprint(\"FPS Interacted with: \", result.collider)");
	p_file->store_line("\telse:");
	p_file->store_line("\t\tprint(\"FPS No interaction target\")");
}

void ProjectDialog::_create_2d_player_scene(Ref<FileAccess> p_file, const String &p_scene_name) {
	p_file->store_line("[gd_scene load_steps=4 format=3 uid=\"uid://generated_by_lupine\"]");
	p_file->store_line("");
	p_file->store_line("[ext_resource type=\"Script\" path=\"scripts/" + p_scene_name.replace("2D", "2D") + ".gd\" id=\"1_player_script\"]");
	p_file->store_line("[ext_resource type=\"Script\" path=\"scripts/PlayerStats.gd\" id=\"2_stats_script\"]");
	p_file->store_line("");
	p_file->store_line("[sub_resource type=\"RectangleShape2D\" id=\"RectangleShape2D_1\"]");
	p_file->store_line("size = Vector2(32, 48)");
	p_file->store_line("");
	p_file->store_line("[sub_resource type=\"CircleShape2D\" id=\"CircleShape2D_1\"]");
	p_file->store_line("radius = 40.0");
	p_file->store_line("");
	p_file->store_line("[node name=\"" + p_scene_name + "\" type=\"CharacterBody2D\"]");
	p_file->store_line("script = ExtResource(\"1_player_script\")");
	p_file->store_line("");
	p_file->store_line("[node name=\"PlayerStats\" type=\"Node\" parent=\".\"]");
	p_file->store_line("script = ExtResource(\"2_stats_script\")");
	p_file->store_line("");
	p_file->store_line("[node name=\"Sprite2D\" type=\"Sprite2D\" parent=\".\"]");
	p_file->store_line("modulate = Color(0.5, 0.8, 1, 1)");
	p_file->store_line("texture = preload(\"res://icon.svg\")");
	p_file->store_line("scale = Vector2(0.25, 0.375)");
	p_file->store_line("");
	p_file->store_line("[node name=\"CollisionShape2D\" type=\"CollisionShape2D\" parent=\".\"]");
	p_file->store_line("shape = SubResource(\"RectangleShape2D_1\")");
	p_file->store_line("");
	p_file->store_line("[node name=\"InteractionArea\" type=\"Area2D\" parent=\".\"]");
	p_file->store_line("");
	p_file->store_line("[node name=\"CollisionShape2D\" type=\"CollisionShape2D\" parent=\"InteractionArea\"]");
	p_file->store_line("shape = SubResource(\"CircleShape2D_1\")");
}

void ProjectDialog::_create_3d_player_scene(Ref<FileAccess> p_file, const String &p_scene_name) {
	p_file->store_line("[gd_scene load_steps=5 format=3 uid=\"uid://generated_by_lupine_3d\"]");
	p_file->store_line("");
	p_file->store_line("[ext_resource type=\"Script\" path=\"scripts/" + p_scene_name.replace("3D", "3D") + ".gd\" id=\"1_player_script\"]");
	p_file->store_line("[ext_resource type=\"Script\" path=\"scripts/PlayerStats.gd\" id=\"2_stats_script\"]");
	p_file->store_line("");
	p_file->store_line("[sub_resource type=\"CapsuleShape3D\" id=\"CapsuleShape3D_1\"]");
	p_file->store_line("radius = 0.4");
	p_file->store_line("height = 1.8");
	p_file->store_line("");
	p_file->store_line("[sub_resource type=\"CapsuleMesh\" id=\"CapsuleMesh_1\"]");
	p_file->store_line("radius = 0.4");
	p_file->store_line("height = 1.8");
	p_file->store_line("");
	p_file->store_line("[sub_resource type=\"StandardMaterial3D\" id=\"StandardMaterial3D_1\"]");
	p_file->store_line("albedo_color = Color(0.5, 0.8, 1, 1)");
	p_file->store_line("");
	p_file->store_line("[node name=\"" + p_scene_name + "\" type=\"CharacterBody3D\"]");
	p_file->store_line("script = ExtResource(\"1_player_script\")");
	p_file->store_line("");
	p_file->store_line("[node name=\"PlayerStats\" type=\"Node\" parent=\".\"]");
	p_file->store_line("script = ExtResource(\"2_stats_script\")");
	p_file->store_line("");
	p_file->store_line("[node name=\"MeshInstance3D\" type=\"MeshInstance3D\" parent=\".\"]");
	p_file->store_line("mesh = SubResource(\"CapsuleMesh_1\")");
	p_file->store_line("surface_material_override/0 = SubResource(\"StandardMaterial3D_1\")");
	p_file->store_line("");
	p_file->store_line("[node name=\"CollisionShape3D\" type=\"CollisionShape3D\" parent=\".\"]");
	p_file->store_line("shape = SubResource(\"CapsuleShape3D_1\")");
	p_file->store_line("");

	if (p_scene_name.contains("ThirdPerson")) {
		p_file->store_line("[node name=\"CameraPivot\" type=\"Node3D\" parent=\".\"]");
		p_file->store_line("");
		p_file->store_line("[node name=\"Camera3D\" type=\"Camera3D\" parent=\"CameraPivot\"]");
		p_file->store_line("transform = Transform3D(1, 0, 0, 0, 0.866025, 0.5, 0, -0.5, 0.866025, 0, 2, 5)");
	} else {
		p_file->store_line("[node name=\"Camera3D\" type=\"Camera3D\" parent=\".\"]");
		p_file->store_line("transform = Transform3D(1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1.8, 0)");
	}
}