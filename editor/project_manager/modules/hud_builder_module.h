#ifndef HUD_BUILDER_MODULE_H
#define HUD_BUILDER_MODULE_H

#include "lupine_module_base.h"

class HUDBuilderModule : public LupineModuleBase {
public:
	void generate_script(Ref<FileAccess> p_file) override;
	void generate_scene(Ref<FileAccess> p_file, const String &p_scene_name) override;
	void generate_file(Ref<FileAccess> p_file, const String &p_relative_path) override;

	String get_module_name() const override { return "HUD Builder"; }
	String get_module_description() const override { return "Modular HUD system with health bars, mini-maps, inventory slots, and real-time data binding for different game types"; }
	String get_module_category() const override { return "UI Systems"; }
	Vector<String> get_files_to_create() const override {
		Vector<String> files;
		files.push_back("globals/HUDBuilder.gd");
		files.push_back("scripts/ui/HUDBase.gd");
		files.push_back("scripts/ui/HealthBar.gd");
		files.push_back("scripts/ui/MiniMap.gd");
		files.push_back("scripts/ui/InventorySlot.gd");
		files.push_back("scripts/ui/StatusDisplay.gd");
		files.push_back("scripts/ui/ActionBar.gd");
		files.push_back("scenes/ui/HUDBase.tscn");
		files.push_back("scenes/ui/HealthBar.tscn");
		files.push_back("scenes/ui/MiniMap.tscn");
		files.push_back("scenes/ui/InventorySlot.tscn");
		files.push_back("scenes/ui/StatusDisplay.tscn");
		files.push_back("scenes/ui/ActionBar.tscn");
		files.push_back("data/hud/hud_config.json");
		return files;
	}

	bool is_global_script() const override { return true; }
	String get_autoload_name() const override { return "HUDBuilder"; }
};

#endif // HUD_BUILDER_MODULE_H
