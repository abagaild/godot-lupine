#ifndef CHARACTER_PORTRAIT_SYSTEM_MODULE_H
#define CHARACTER_PORTRAIT_SYSTEM_MODULE_H

#include "lupine_module_base.h"

class CharacterPortraitSystemModule : public LupineModuleBase {
public:
	void generate_script(Ref<FileAccess> p_file) override;
	void generate_scene(Ref<FileAccess> p_file, const String &p_scene_name) override;
	void generate_file(Ref<FileAccess> p_file, const String &p_relative_path) override;

	String get_module_name() const override { return "Character Portrait System"; }
	String get_module_description() const override { return "Character portrait display with emotion system, positioning, and automatic asset discovery"; }
	String get_module_category() const override { return "Visual Novel"; }
	Vector<String> get_files_to_create() const override {
		Vector<String> files;
		files.push_back("globals/CharacterPortraitSystem.gd");
		files.push_back("scenes/ui/PortraitDisplay.tscn");
		files.push_back("scripts/ui/PortraitDisplay.gd");
		return files;
	}

	bool is_global_script() const override { return true; }
	String get_autoload_name() const override { return "CharacterPortraitSystem"; }
};

#endif // CHARACTER_PORTRAIT_SYSTEM_MODULE_H
