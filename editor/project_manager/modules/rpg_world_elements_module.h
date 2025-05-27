#ifndef RPG_WORLD_ELEMENTS_MODULE_H
#define RPG_WORLD_ELEMENTS_MODULE_H

#include "lupine_module_base.h"

class RPGWorldElementsModule : public LupineModuleBase {
public:
	void generate_script(Ref<FileAccess> p_file) override;
	void generate_scene(Ref<FileAccess> p_file, const String &p_scene_name) override;
	
	String get_module_name() const override { return "RPG World Elements"; }
	String get_module_description() const override { return "Interactive world elements for RPG: chests, doors, switches, teleporters, NPCs, shops, and collectibles"; }
	String get_module_category() const override { return "World Elements"; }
	Vector<String> get_files_to_create() const override {
		Vector<String> files;
		files.push_back("scripts/world/InteractableBase.gd");
		files.push_back("scripts/world/Chest.gd");
		files.push_back("scripts/world/Door.gd");
		files.push_back("scripts/world/Switch.gd");
		files.push_back("scripts/world/Teleporter.gd");
		files.push_back("scripts/world/NPC.gd");
		files.push_back("scripts/world/Shop.gd");
		files.push_back("scripts/world/Collectible.gd");
		files.push_back("scenes/world/Chest.tscn");
		files.push_back("scenes/world/Door.tscn");
		files.push_back("scenes/world/Switch.tscn");
		files.push_back("scenes/world/Teleporter.tscn");
		files.push_back("scenes/world/NPC.tscn");
		files.push_back("scenes/world/Shop.tscn");
		files.push_back("scenes/world/Collectible.tscn");
		return files;
	}

	bool is_global_script() const override { return false; }
};

#endif // RPG_WORLD_ELEMENTS_MODULE_H
