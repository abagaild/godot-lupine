#ifndef PLAYER_STATS_MODULE_H
#define PLAYER_STATS_MODULE_H

#include "lupine_module_base.h"

class PlayerStatsModule : public LupineModuleBase {
public:
	void generate_script(Ref<FileAccess> p_file) override;
	void generate_scene(Ref<FileAccess> p_file, const String &p_scene_name) override;

	String get_module_name() const override { return "Player Statistics System"; }
	String get_module_description() const override { return "Health, stamina, and mana system with easy extensibility"; }
	String get_module_category() const override { return "Core Systems"; }
	Vector<String> get_files_to_create() const override {
		Vector<String> files;
		files.push_back("globals/PlayerStats.gd");
		return files;
	}

	bool is_global_script() const override { return true; }
	String get_autoload_name() const override { return "PlayerStats"; }
};

#endif // PLAYER_STATS_MODULE_H
