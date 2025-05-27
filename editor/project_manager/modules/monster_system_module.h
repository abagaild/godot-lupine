#ifndef MONSTER_SYSTEM_MODULE_H
#define MONSTER_SYSTEM_MODULE_H

#include "lupine_module_base.h"

class MonsterSystemModule : public LupineModuleBase {
public:
	void generate_script(Ref<FileAccess> p_file) override;
	void generate_scene(Ref<FileAccess> p_file, const String &p_scene_name) override;
	
	String get_module_name() const override { return "Monster System"; }
	String get_module_description() const override { return "Complete Pokemon-style monster management system with stats, evolution, breeding, and party management"; }
	String get_module_category() const override { return "Monster Capture"; }
	Vector<String> get_files_to_create() const override {
		Vector<String> files;
		files.push_back("globals/MonsterManager.gd");
		files.push_back("scripts/monsters/Pokemon.gd");
		files.push_back("scripts/monsters/PokemonSpecies.gd");
		files.push_back("scripts/monsters/PokemonStats.gd");
		files.push_back("scripts/monsters/PokemonParty.gd");
		files.push_back("scripts/monsters/PokemonBox.gd");
		files.push_back("scripts/monsters/Evolution.gd");
		files.push_back("scripts/monsters/Nature.gd");
		files.push_back("scripts/monsters/Ability.gd");
		files.push_back("scripts/ui/PartyUI.gd");
		files.push_back("scripts/ui/PokemonSummary.gd");
		files.push_back("scripts/ui/BoxUI.gd");
		files.push_back("scenes/ui/PartyUI.tscn");
		files.push_back("scenes/ui/PokemonSummary.tscn");
		files.push_back("scenes/ui/BoxUI.tscn");
		files.push_back("data/monsters/species.json");
		files.push_back("data/monsters/natures.json");
		files.push_back("data/monsters/abilities.json");
		files.push_back("data/monsters/evolutions.json");
		return files;
	}

	bool is_global_script() const override { return true; }
	String get_autoload_name() const override { return "MonsterManager"; }
};

#endif // MONSTER_SYSTEM_MODULE_H
