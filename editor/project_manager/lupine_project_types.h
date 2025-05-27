#ifndef LUPINE_PROJECT_TYPES_H
#define LUPINE_PROJECT_TYPES_H

#include "core/string/ustring.h"
#include "core/templates/vector.h"

// Shared structures for Lupine Engine project templates and modules

struct ProjectTemplate {
	String id;
	String name;
	String description;
	String icon_path;
	String category;
	Vector<String> default_modules;
	Vector<String> folder_structure;
	String main_scene_template;
};

struct ProjectModule {
	String id;
	String name;
	String description;
	String category;
	Vector<String> dependencies;
	Vector<String> conflicts;
	Vector<String> files_to_create;
};

#endif // LUPINE_PROJECT_TYPES_H
