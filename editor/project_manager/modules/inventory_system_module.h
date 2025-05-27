#ifndef INVENTORY_SYSTEM_MODULE_H
#define INVENTORY_SYSTEM_MODULE_H

#include "lupine_module_base.h"

class InventorySystemModule : public LupineModuleBase {
public:
	void generate_script(Ref<FileAccess> p_file) override;
	void generate_scene(Ref<FileAccess> p_file, const String &p_scene_name) override;

	String get_module_name() const override { return "Inventory System"; }
	String get_module_description() const override { return "RPG inventory system with items, equipment, consumables, and drag-and-drop UI"; }
	String get_module_category() const override { return "RPG Systems"; }
	Vector<String> get_files_to_create() const override {
		Vector<String> files;
		files.push_back("globals/InventorySystem.gd");
		files.push_back("scripts/items/Item.gd");
		files.push_back("scripts/items/Equipment.gd");
		files.push_back("scripts/items/Consumable.gd");
		files.push_back("scripts/ui/InventoryUI.gd");
		files.push_back("scripts/ui/ItemSlot.gd");
		files.push_back("scenes/ui/InventoryUI.tscn");
		files.push_back("scenes/ui/ItemSlot.tscn");
		return files;
	}

	bool is_global_script() const override { return true; }
	String get_autoload_name() const override { return "InventorySystem"; }
};

#endif // INVENTORY_SYSTEM_MODULE_H
