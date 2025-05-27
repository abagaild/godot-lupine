#ifndef SUPPORT_CONVERSATION_MODULE_H
#define SUPPORT_CONVERSATION_MODULE_H

#include "lupine_module_base.h"

class SupportConversationModule : public LupineModuleBase {
public:
	void generate_script(Ref<FileAccess> p_file) override;
	void generate_scene(Ref<FileAccess> p_file, const String &p_scene_name) override;
	
	String get_module_name() const override { return "Support Conversation System"; }
	String get_module_description() const override { return "Fire Emblem-style support conversations with branching dialogue, character development, and relationship progression"; }
	String get_module_category() const override { return "Social Systems"; }
	Vector<String> get_files_to_create() const override {
		Vector<String> files;
		files.push_back("globals/SupportConversationManager.gd");
		files.push_back("scripts/support/SupportRank.gd");
		files.push_back("scripts/support/SupportPair.gd");
		files.push_back("scripts/support/SupportDialogue.gd");
		files.push_back("scripts/support/SupportCondition.gd");
		files.push_back("scripts/ui/SupportListUI.gd");
		files.push_back("scripts/ui/SupportViewerUI.gd");
		files.push_back("scripts/ui/SupportProgressUI.gd");
		files.push_back("scenes/ui/SupportListUI.tscn");
		files.push_back("scenes/ui/SupportViewerUI.tscn");
		files.push_back("scenes/ui/SupportProgressUI.tscn");
		files.push_back("data/support/support_pairs.json");
		files.push_back("data/support/support_dialogues.json");
		files.push_back("data/support/support_conditions.json");
		return files;
	}

	bool is_global_script() const override { return true; }
	String get_autoload_name() const override { return "SupportConversationManager"; }
};

#endif // SUPPORT_CONVERSATION_MODULE_H
