#ifndef PASSIVE_MOBS_MODULE_H
#define PASSIVE_MOBS_MODULE_H

#include "lupine_module_base.h"

class PassiveMobsModule : public LupineModuleBase {
public:
	void generate_script(Ref<FileAccess> p_file) override;
	void generate_scene(Ref<FileAccess> p_file, const String &p_scene_name) override;

	String get_module_name() const override { return "Passive Mobs"; }
	String get_module_description() const override { return "Friendly creatures like cows and cats that show barks when approached and hearts when interacted with"; }
	String get_module_category() const override { return "World Elements"; }
	Vector<String> get_files_to_create() const override {
		Vector<String> files;
		files.push_back("scripts/mobs/PassiveMob.gd");
		files.push_back("scripts/mobs/Cow.gd");
		files.push_back("scripts/mobs/Cat.gd");
		files.push_back("scripts/effects/BarkBubble.gd");
		files.push_back("scripts/effects/HeartEffect.gd");
		files.push_back("scenes/mobs/Cow.tscn");
		files.push_back("scenes/mobs/Cat.tscn");
		files.push_back("scenes/effects/BarkBubble.tscn");
		files.push_back("scenes/effects/HeartEffect.tscn");
		return files;
	}

	bool is_global_script() const override { return false; }
};

#endif // PASSIVE_MOBS_MODULE_H
