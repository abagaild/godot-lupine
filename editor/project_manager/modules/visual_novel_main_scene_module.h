#ifndef VISUAL_NOVEL_MAIN_SCENE_MODULE_H
#define VISUAL_NOVEL_MAIN_SCENE_MODULE_H

#include "lupine_module_base.h"

class VisualNovelMainSceneModule : public LupineModuleBase {
public:
	void generate_script(Ref<FileAccess> p_file) override;
	void generate_scene(Ref<FileAccess> p_file, const String &p_scene_name) override;
	void generate_file(Ref<FileAccess> p_file, const String &p_relative_path) override;
	void generate_readme(Ref<FileAccess> p_file);

	String get_module_name() const override { return "Visual Novel Main Scene"; }
	String get_module_description() const override { return "Complete main scene with dialogue UI, menus, and all visual novel components integrated"; }
	String get_module_category() const override { return "Visual Novel"; }
	Vector<String> get_files_to_create() const override {
		Vector<String> files;
		files.push_back("scenes/Main.tscn");
		files.push_back("scripts/Main.gd");
		files.push_back("scenes/ui/DialogueUI.tscn");
		files.push_back("scripts/ui/DialogueUI.gd");
		files.push_back("README_VisualNovel.md");
		return files;
	}

	bool is_global_script() const override { return false; }
	String get_autoload_name() const override { return ""; }
};

#endif // VISUAL_NOVEL_MAIN_SCENE_MODULE_H
