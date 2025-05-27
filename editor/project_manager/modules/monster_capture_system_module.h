#ifndef MONSTER_CAPTURE_SYSTEM_MODULE_H
#define MONSTER_CAPTURE_SYSTEM_MODULE_H

#include "lupine_module_base.h"

class MonsterCaptureSystemModule : public LupineModuleBase {
public:
	void generate_script(Ref<FileAccess> p_file) override;
	void generate_scene(Ref<FileAccess> p_file, const String &p_scene_name) override;
	
	String get_module_name() const override { return "Monster Capture System"; }
	String get_module_description() const override { return "Pokemon-style monster capture mechanics with Pokeballs, catch rates, and capture animations"; }
	String get_module_category() const override { return "Monster Capture"; }
	Vector<String> get_files_to_create() const override {
		Vector<String> files;
		files.push_back("globals/CaptureManager.gd");
		files.push_back("scripts/capture/Pokeball.gd");
		files.push_back("scripts/capture/CaptureCalculator.gd");
		files.push_back("scripts/capture/CaptureAnimation.gd");
		files.push_back("scripts/capture/WildPokemon.gd");
		files.push_back("scripts/ui/CaptureUI.gd");
		files.push_back("scenes/capture/Pokeball.tscn");
		files.push_back("scenes/capture/CaptureAnimation.tscn");
		files.push_back("scenes/ui/CaptureUI.tscn");
		files.push_back("data/capture/pokeballs.json");
		files.push_back("data/capture/catch_rates.json");
		return files;
	}

	bool is_global_script() const override { return true; }
	String get_autoload_name() const override { return "CaptureManager"; }
};

#endif // MONSTER_CAPTURE_SYSTEM_MODULE_H
