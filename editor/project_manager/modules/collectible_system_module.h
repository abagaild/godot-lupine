#ifndef COLLECTIBLE_SYSTEM_MODULE_H
#define COLLECTIBLE_SYSTEM_MODULE_H

#include "lupine_module_base.h"

class CollectibleSystemModule : public LupineModuleBase {
public:
	void generate_script(Ref<FileAccess> p_file) override;
	void generate_scene(Ref<FileAccess> p_file, const String &p_scene_name) override;

	String get_module_name() const override { return "Collectible System"; }
	String get_module_description() const override { return "Collectible items system with coins, power-ups, health pickups, and treasure chests"; }
	String get_module_category() const override { return "RPG Systems"; }
	Vector<String> get_files_to_create() const override {
		Vector<String> files;
		files.push_back("scripts/collectibles/CollectibleBase.gd");
		files.push_back("scripts/collectibles/Coin.gd");
		files.push_back("scripts/collectibles/HealthPickup.gd");
		files.push_back("scripts/collectibles/PowerUp.gd");
		files.push_back("scripts/collectibles/TreasureChest.gd");
		files.push_back("scenes/collectibles/Coin.tscn");
		files.push_back("scenes/collectibles/HealthPickup.tscn");
		files.push_back("scenes/collectibles/PowerUp.tscn");
		files.push_back("scenes/collectibles/TreasureChest.tscn");
		return files;
	}

	bool is_global_script() const override { return false; }
};

#endif // COLLECTIBLE_SYSTEM_MODULE_H
