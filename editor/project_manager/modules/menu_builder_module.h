#ifndef MENU_BUILDER_MODULE_H
#define MENU_BUILDER_MODULE_H

#include "lupine_module_base.h"

class MenuBuilderModule : public LupineModuleBase {
public:
	void generate_script(Ref<FileAccess> p_file) override;
	void generate_scene(Ref<FileAccess> p_file, const String &p_scene_name) override;
	void generate_file(Ref<FileAccess> p_file, const String &p_relative_path) override;

	String get_module_name() const override { return "Menu Builder"; }
	String get_module_description() const override { return "Extensible menu system with customizable layouts, themes, and actions for main menus, pause menus, and settings"; }
	String get_module_category() const override { return "UI Systems"; }
	Vector<String> get_files_to_create() const override {
		Vector<String> files;
		files.push_back("globals/MenuBuilder.gd");
		files.push_back("scripts/ui/MenuBase.gd");
		files.push_back("scripts/ui/MainMenu.gd");
		files.push_back("scripts/ui/PauseMenu.gd");
		files.push_back("scripts/ui/SettingsMenu.gd");
		files.push_back("scenes/ui/MenuBase.tscn");
		files.push_back("scenes/ui/MainMenu.tscn");
		files.push_back("scenes/ui/PauseMenu.tscn");
		files.push_back("scenes/ui/SettingsMenu.tscn");
		files.push_back("data/menus/menu_config.json");
		return files;
	}

	bool is_global_script() const override { return true; }
	String get_autoload_name() const override { return "MenuBuilder"; }
};

#endif // MENU_BUILDER_MODULE_H
