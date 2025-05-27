#ifndef AUDIO_MANAGER_MODULE_H
#define AUDIO_MANAGER_MODULE_H

#include "lupine_module_base.h"

class AudioManagerModule : public LupineModuleBase {
public:
	void generate_script(Ref<FileAccess> p_file) override;
	void generate_scene(Ref<FileAccess> p_file, const String &p_scene_name) override;
	
	String get_module_name() const override { return "Audio Manager"; }
	String get_module_description() const override { return "Music and sound effects management with crossfading, looping, and automatic asset discovery"; }
	String get_module_category() const override { return "Visual Novel"; }
	Vector<String> get_files_to_create() const override {
		Vector<String> files;
		files.push_back("globals/AudioManager.gd");
		return files;
	}

	bool is_global_script() const override { return true; }
	String get_autoload_name() const override { return "AudioManager"; }
};

#endif // AUDIO_MANAGER_MODULE_H
