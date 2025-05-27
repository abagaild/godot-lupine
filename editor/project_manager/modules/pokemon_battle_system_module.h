#ifndef POKEMON_BATTLE_SYSTEM_MODULE_H
#define POKEMON_BATTLE_SYSTEM_MODULE_H

#include "lupine_module_base.h"

class PokemonBattleSystemModule : public LupineModuleBase {
public:
	void generate_script(Ref<FileAccess> p_file) override;
	void generate_scene(Ref<FileAccess> p_file, const String &p_scene_name) override;
	
	String get_module_name() const override { return "Pokemon Battle System"; }
	String get_module_description() const override { return "Complete Pokemon-style turn-based battle system with moves, types, status effects, and classic battle mechanics"; }
	String get_module_category() const override { return "Combat Systems"; }
	Vector<String> get_files_to_create() const override {
		Vector<String> files;
		files.push_back("globals/PokemonBattleManager.gd");
		files.push_back("scripts/pokemon/PokemonBattle.gd");
		files.push_back("scripts/pokemon/PokemonMove.gd");
		files.push_back("scripts/pokemon/PokemonType.gd");
		files.push_back("scripts/pokemon/StatusEffect.gd");
		files.push_back("scripts/pokemon/BattleCalculator.gd");
		files.push_back("scripts/pokemon/BattleAI.gd");
		files.push_back("scripts/ui/PokemonBattleUI.gd");
		files.push_back("scripts/ui/MoveSelector.gd");
		files.push_back("scripts/ui/PokemonSwitcher.gd");
		files.push_back("scripts/ui/BattleAnimations.gd");
		files.push_back("scenes/pokemon/PokemonBattle.tscn");
		files.push_back("scenes/ui/PokemonBattleUI.tscn");
		files.push_back("scenes/ui/MoveSelector.tscn");
		files.push_back("scenes/ui/PokemonSwitcher.tscn");
		files.push_back("data/pokemon/moves.json");
		files.push_back("data/pokemon/types.json");
		files.push_back("data/pokemon/status_effects.json");
		return files;
	}

	bool is_global_script() const override { return true; }
	String get_autoload_name() const override { return "PokemonBattleManager"; }
};

#endif // POKEMON_BATTLE_SYSTEM_MODULE_H
