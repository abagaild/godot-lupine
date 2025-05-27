#ifndef RELATIONSHIP_SYSTEM_MODULE_H
#define RELATIONSHIP_SYSTEM_MODULE_H

#include "lupine_module_base.h"

class RelationshipSystemModule : public LupineModuleBase {
public:
	void generate_script(Ref<FileAccess> p_file) override;
	void generate_scene(Ref<FileAccess> p_file, const String &p_scene_name) override;
	
	String get_module_name() const override { return "Relationship System"; }
	String get_module_description() const override { return "Complete relationship and support conversation system with Fire Emblem-style supports, friendship mechanics, and faction reputation"; }
	String get_module_category() const override { return "Social Systems"; }
	Vector<String> get_files_to_create() const override {
		Vector<String> files;
		files.push_back("globals/RelationshipManager.gd");
		files.push_back("scripts/relationships/Character.gd");
		files.push_back("scripts/relationships/Relationship.gd");
		files.push_back("scripts/relationships/SupportConversation.gd");
		files.push_back("scripts/relationships/FactionReputation.gd");
		files.push_back("scripts/relationships/InteractionEvent.gd");
		files.push_back("scripts/ui/RelationshipUI.gd");
		files.push_back("scripts/ui/SupportConversationUI.gd");
		files.push_back("scripts/ui/FactionStatusUI.gd");
		files.push_back("scripts/ui/CharacterProfileUI.gd");
		files.push_back("scenes/ui/RelationshipUI.tscn");
		files.push_back("scenes/ui/SupportConversationUI.tscn");
		files.push_back("scenes/ui/FactionStatusUI.tscn");
		files.push_back("scenes/ui/CharacterProfileUI.tscn");
		files.push_back("data/relationships/characters.json");
		files.push_back("data/relationships/factions.json");
		files.push_back("data/relationships/support_conversations.json");
		return files;
	}

	bool is_global_script() const override { return true; }
	String get_autoload_name() const override { return "RelationshipManager"; }
};

#endif // RELATIONSHIP_SYSTEM_MODULE_H
