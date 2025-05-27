#ifndef POPUP_MANAGER_MODULE_H
#define POPUP_MANAGER_MODULE_H

#include "lupine_module_base.h"

class PopupManagerModule : public LupineModuleBase {
public:
	void generate_script(Ref<FileAccess> p_file) override;
	void generate_scene(Ref<FileAccess> p_file, const String &p_scene_name) override;

	String get_module_name() const override { return "Popup Manager"; }
	String get_module_description() const override { return "Popup notification system for level ups, quest completion, status effects, and mini-notifications"; }
	String get_module_category() const override { return "UI Systems"; }
	Vector<String> get_files_to_create() const override {
		Vector<String> files;
		files.push_back("globals/PopupManager.gd");
		files.push_back("scripts/ui/PopupNotification.gd");
		files.push_back("scenes/ui/PopupNotification.tscn");
		return files;
	}

	bool is_global_script() const override { return true; }
	String get_autoload_name() const override { return "PopupManager"; }
};

#endif // POPUP_MANAGER_MODULE_H
