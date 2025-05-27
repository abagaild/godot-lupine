#ifndef CHOICE_SYSTEM_MODULE_H
#define CHOICE_SYSTEM_MODULE_H

#include "lupine_module_base.h"

class ChoiceSystemModule : public LupineModuleBase {
public:
	void generate_script(Ref<FileAccess> p_file) override;
	void generate_scene(Ref<FileAccess> p_file, const String &p_scene_name) override;
	void generate_file(Ref<FileAccess> p_file, const String &p_relative_path) override;

	String get_module_name() const override { return "Choice System"; }
	String get_module_description() const override { return "Branching dialogue choices with conditional options, timed choices, and choice history tracking"; }
	String get_module_category() const override { return "Visual Novel"; }
	Vector<String> get_files_to_create() const override {
		Vector<String> files;
		files.push_back("globals/ChoiceSystem.gd");
		files.push_back("scenes/ui/ChoiceMenu.tscn");
		files.push_back("scripts/ui/ChoiceMenu.gd");
		return files;
	}

	bool is_global_script() const override { return true; }
	String get_autoload_name() const override { return "ChoiceSystem"; }
};

#endif // CHOICE_SYSTEM_MODULE_H
