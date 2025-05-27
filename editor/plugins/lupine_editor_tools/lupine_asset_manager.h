#ifndef LUPINE_ASSET_MANAGER_H
#define LUPINE_ASSET_MANAGER_H

#include "lupine_editor_plugin.h"
#include "scene/gui/tree.h"
#include "scene/gui/item_list.h"
#include "scene/gui/file_dialog.h"
#include "scene/gui/texture_rect.h"
#include "scene/gui/rich_text_label.h"
#include "scene/gui/progress_bar.h"
#include "scene/gui/split_container.h"
#include "scene/gui/box_container.h"
#include "scene/gui/line_edit.h"
#include "scene/gui/option_button.h"
#include "scene/gui/button.h"
#include "scene/gui/text_edit.h"
#include "scene/resources/texture.h"
#include "core/io/dir_access.h"
#include "core/io/file_access.h"
#include "editor/editor_file_system.h"

// Asset category definitions for Lupine-specific organization
struct LupineAssetCategory {
	String id;
	String name;
	String description;
	String folder_path;
	Vector<String> file_extensions;
	Ref<Texture2D> icon;
	Color category_color;

	LupineAssetCategory() {}
	LupineAssetCategory(const String &p_id, const String &p_name, const String &p_folder) :
		id(p_id), name(p_name), folder_path(p_folder) {}
};

// Individual asset information
struct LupineAssetInfo {
	String path;
	String name;
	String category_id;
	String description;
	Ref<Texture2D> thumbnail;
	Ref<Resource> resource;
	int64_t file_size;
	uint64_t modified_time;
	Vector<String> tags;
	Dictionary metadata;

	LupineAssetInfo() : file_size(0), modified_time(0) {}
};

// Enhanced Asset Manager for Lupine-specific asset organization
class LupineAssetManager : public LupineEditorTool {
	GDCLASS(LupineAssetManager, LupineEditorTool);

private:
	// UI Components
	HSplitContainer *main_split = nullptr;
	VBoxContainer *left_panel = nullptr;
	VBoxContainer *right_panel = nullptr;

	// Category browser
	Tree *category_tree = nullptr;
	LineEdit *search_filter = nullptr;
	OptionButton *sort_options = nullptr;

	// Asset browser
	ItemList *asset_list = nullptr;
	ProgressBar *loading_progress = nullptr;

	// Asset preview
	VBoxContainer *preview_panel = nullptr;
	TextureRect *preview_image = nullptr;
	RichTextLabel *asset_details = nullptr;
	Button *import_button = nullptr;
	Button *export_button = nullptr;

	// Asset management
	Vector<LupineAssetCategory> asset_categories;
	HashMap<String, LupineAssetCategory*> category_map;
	Vector<LupineAssetInfo> asset_cache;
	HashMap<String, LupineAssetInfo*> asset_map;

	// State
	String current_category_id;
	String selected_asset_path;
	bool is_loading_assets = false;

	// Methods
	void _create_ui();
	void _setup_asset_categories();
	void _populate_category_tree();
	void _scan_assets_for_category(const String &p_category_id);
	void _update_asset_list();
	void _update_asset_preview();
	void _generate_thumbnail(const String &p_path);

	// Event handlers
	void _on_category_selected();
	void _on_search_changed(const String &p_text);
	void _on_sort_changed(int p_index);
	void _on_asset_selected(int p_index);
	void _on_asset_double_clicked(int p_index);
	void _on_import_pressed();
	void _on_export_pressed();

	// Asset operations
	void _import_asset_to_category(const String &p_source_path, const String &p_category_id);
	void _export_asset(const String &p_asset_path);
	void _perform_export(const String &p_target_path, const String &p_source_path);
	void _refresh_asset_cache();
	void _scan_directory_recursive(const String &p_path, const String &p_category_id);

protected:
	static void _bind_methods();

public:
	LupineAssetManager();
	virtual ~LupineAssetManager();

	// LupineEditorTool interface
	virtual void initialize() override;
	virtual void cleanup() override;
	virtual Control *get_tool_panel() override { return tool_panel; }
	virtual String get_tool_name() const override { return "Asset Manager"; }
	virtual String get_tool_description() const override { return "Enhanced asset browser with Lupine-specific organization"; }
	virtual String get_tool_category() const override { return "Asset Management"; }

	// Asset management
	void add_asset_category(const LupineAssetCategory &p_category);
	void remove_asset_category(const String &p_id);
	LupineAssetCategory *get_asset_category(const String &p_id);
	Vector<LupineAssetCategory> get_all_categories() const { return asset_categories; }

	// Asset operations
	LupineAssetInfo *get_asset_info(const String &p_path);
	Vector<LupineAssetInfo> get_assets_by_category(const String &p_category_id);
	Vector<LupineAssetInfo> search_assets(const String &p_query);
	void refresh_assets();

	// Template integration
	void setup_template_categories(const String &p_template_name);
	void import_template_assets(const String &p_template_path);
};

// Asset Preview Generator - creates thumbnails for different asset types
class LupineAssetPreviewGenerator : public RefCounted {
	GDCLASS(LupineAssetPreviewGenerator, RefCounted);

private:
	static LupineAssetPreviewGenerator *singleton;
	HashMap<String, Ref<Texture2D>> thumbnail_cache;

protected:
	static void _bind_methods();

public:
	static LupineAssetPreviewGenerator *get_singleton();

	LupineAssetPreviewGenerator();
	virtual ~LupineAssetPreviewGenerator();

	// Thumbnail generation
	Ref<Texture2D> generate_thumbnail(const String &p_path, const Size2i &p_size = Size2i(64, 64));
	Ref<Texture2D> get_cached_thumbnail(const String &p_path);
	void clear_thumbnail_cache();

	// Type-specific generators
	Ref<Texture2D> generate_texture_thumbnail(const String &p_path, const Size2i &p_size);
	Ref<Texture2D> generate_scene_thumbnail(const String &p_path, const Size2i &p_size);
	Ref<Texture2D> generate_script_thumbnail(const String &p_path, const Size2i &p_size);
	Ref<Texture2D> generate_audio_thumbnail(const String &p_path, const Size2i &p_size);
	Ref<Texture2D> generate_default_thumbnail(const String &p_extension, const Size2i &p_size);
};

// Asset Import Dialog - specialized import dialog for Lupine assets
class LupineAssetImportDialog : public FileDialog {
	GDCLASS(LupineAssetImportDialog, FileDialog);

private:
	LupineAssetManager *asset_manager = nullptr;
	OptionButton *category_selector = nullptr;
	LineEdit *asset_name_edit = nullptr;
	TextEdit *asset_description = nullptr;
	LineEdit *tags_edit = nullptr;
	String target_category;

	void _on_category_selected(int p_index);
	void _on_files_selected(const PackedStringArray &p_paths);

protected:
	static void _bind_methods();

public:
	LupineAssetImportDialog(LupineAssetManager *p_manager);
	virtual ~LupineAssetImportDialog();

	void setup_for_import();
	void set_target_category(const String &p_category_id);
};

#endif // LUPINE_ASSET_MANAGER_H
