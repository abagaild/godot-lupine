#ifndef VISUAL_NOVEL_SCRIPT_PARSER_MODULE_H
#define VISUAL_NOVEL_SCRIPT_PARSER_MODULE_H

#include "lupine_module_base.h"

class VisualNovelScriptParserModule : public LupineModuleBase {
public:
	void generate_script(Ref<FileAccess> p_file) override;
	void generate_scene(Ref<FileAccess> p_file, const String &p_scene_name) override;
	void generate_file(Ref<FileAccess> p_file, const String &p_relative_path) override;

	String get_module_name() const override { return "Visual Novel Script Parser"; }
	String get_module_description() const override { return "Writer-forward script parser with node-based dialogue, conditionals, and command system"; }
	String get_module_category() const override { return "Visual Novel"; }
	Vector<String> get_files_to_create() const override {
		Vector<String> files;
		files.push_back("globals/VNScriptParser.gd");
		files.push_back("scripts/dialogue/VNDialogueNode.gd");
		files.push_back("scripts/dialogue/VNScriptLoader.gd");
		files.push_back("data/dialogue/example_conversation.vn");
		return files;
	}

	bool is_global_script() const override { return true; }
	String get_autoload_name() const override { return "VNScriptParser"; }
};

#endif // VISUAL_NOVEL_SCRIPT_PARSER_MODULE_H
