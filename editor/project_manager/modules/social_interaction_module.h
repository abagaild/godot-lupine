#ifndef SOCIAL_INTERACTION_MODULE_H
#define SOCIAL_INTERACTION_MODULE_H

#include "lupine_module_base.h"

class SocialInteractionModule : public LupineModuleBase {
public:
	void generate_script(Ref<FileAccess> p_file) override;
	void generate_scene(Ref<FileAccess> p_file, const String &p_scene_name) override;
	
	String get_module_name() const override { return "Social Interaction System"; }
	String get_module_description() const override { return "Stardew Valley/Harvest Moon style social interactions with gifts, events, dating, marriage, and friendship mechanics"; }
	String get_module_category() const override { return "Social Systems"; }
	Vector<String> get_files_to_create() const override {
		Vector<String> files;
		files.push_back("globals/SocialInteractionManager.gd");
		files.push_back("scripts/social/GiftSystem.gd");
		files.push_back("scripts/social/DateSystem.gd");
		files.push_back("scripts/social/MarriageSystem.gd");
		files.push_back("scripts/social/FriendshipEvent.gd");
		files.push_back("scripts/social/SocialCalendar.gd");
		files.push_back("scripts/social/CharacterSchedule.gd");
		files.push_back("scripts/ui/GiftUI.gd");
		files.push_back("scripts/ui/SocialCalendarUI.gd");
		files.push_back("scripts/ui/RelationshipStatusUI.gd");
		files.push_back("scripts/ui/DatePlannerUI.gd");
		files.push_back("scenes/ui/GiftUI.tscn");
		files.push_back("scenes/ui/SocialCalendarUI.tscn");
		files.push_back("scenes/ui/RelationshipStatusUI.tscn");
		files.push_back("scenes/ui/DatePlannerUI.tscn");
		files.push_back("data/social/gift_preferences.json");
		files.push_back("data/social/friendship_events.json");
		files.push_back("data/social/character_schedules.json");
		files.push_back("data/social/date_activities.json");
		return files;
	}

	bool is_global_script() const override { return true; }
	String get_autoload_name() const override { return "SocialInteractionManager"; }
};

#endif // SOCIAL_INTERACTION_MODULE_H
