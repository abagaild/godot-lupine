#ifndef DIALOGUE_SYSTEM_MODULE_H
#define DIALOGUE_SYSTEM_MODULE_H

#include "lupine_module_base.h"

class DialogueSystemModule : public LupineModuleBase {
public:
	void generate_script(Ref<FileAccess> p_file) override;
	void generate_scene(Ref<FileAccess> p_file, const String &p_scene_name) override;

	String get_module_name() const override { return "Dialogue System"; }
	String get_module_description() const override { return "RPG dialogue system with NPC interactions, branching conversations, and quest integration"; }
	String get_module_category() const override { return "RPG Systems"; }
	Vector<String> get_files_to_create() const override {
		Vector<String> files;
		files.push_back("globals/DialogueSystem.gd");
		files.push_back("scripts/ui/DialogueUI.gd");
		files.push_back("scripts/characters/NPC.gd");
		files.push_back("scenes/ui/DialogueUI.tscn");
		files.push_back("scenes/characters/NPC.tscn");
		return files;
	}

	bool is_global_script() const override { return true; }
	String get_autoload_name() const override { return "DialogueSystem"; }
};

#endif // DIALOGUE_SYSTEM_MODULE_H
