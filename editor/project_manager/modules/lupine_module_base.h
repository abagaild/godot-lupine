#ifndef LUPINE_MODULE_BASE_H
#define LUPINE_MODULE_BASE_H

#include "core/io/file_access.h"
#include "core/string/ustring.h"

// Base class for Lupine Engine module generators
class LupineModuleBase {
public:
	// Generate script content for a module
	virtual void generate_script(Ref<FileAccess> p_file) = 0;

	// Generate scene content for a module
	virtual void generate_scene(Ref<FileAccess> p_file, const String &p_scene_name) = 0;

	// Generate specific file content (override for custom file handling)
	virtual void generate_file(Ref<FileAccess> p_file, const String &p_relative_path) {
		String filename = p_relative_path.get_file();
		if (p_relative_path.ends_with(".gd")) {
			generate_script(p_file);
		} else if (p_relative_path.ends_with(".tscn")) {
			generate_scene(p_file, filename.get_basename());
		}
	}

	// Get module information
	virtual String get_module_name() const = 0;
	virtual String get_module_description() const = 0;
	virtual String get_module_category() const = 0;
	virtual Vector<String> get_files_to_create() const = 0;

	// Global script support
	virtual bool is_global_script() const { return false; }
	virtual String get_autoload_name() const { return ""; }

	virtual ~LupineModuleBase() {}
};

#endif // LUPINE_MODULE_BASE_H
