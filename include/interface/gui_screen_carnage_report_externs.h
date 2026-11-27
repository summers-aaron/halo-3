#include "interface/user_interface_matchmaking_progress.h"
#include "game/campaign_metagame_globals.h"
#ifndef INTERFACE_GUI_SCREEN_CARNAGE_REPORT_EXTERNS_H
#define INTERFACE_GUI_SCREEN_CARNAGE_REPORT_EXTERNS_H

#include "cseries/cseries_unicode.h"

// External dependencies used by gui_screen_carnage_report.
#include "math/real_math_types.h"
template<long k_maximum_count> class c_static_wchar_string;
class c_gui_widget;
struct cs_point;

extern "C" bool levels_get_campaign_map_display_name(void *context, wchar_t *buffer, long buffer_length);
extern "C" bool parse_xml_campaign_time(void *context, wchar_t *buffer, long buffer_length);
extern "C" bool parse_xml_campaign_skull_count(void *context, wchar_t *buffer, long buffer_length);
extern "C" bool parse_xml_campaign_player_scale(void *context, wchar_t *buffer, long buffer_length);
extern "C" bool parse_xml_campaign_skull_bonus(void *context, wchar_t *buffer, long buffer_length);
extern "C" bool parse_xml_campaign_difficulty_scale(void *context, wchar_t *buffer, long buffer_length);
extern "C" bool parse_xml_campaign_time_multiplier(void *context, wchar_t *buffer, long buffer_length);
extern "C" bool parse_xml_campaign_total_multiplier(void *context, wchar_t *buffer, long buffer_length);
extern "C" bool parse_xml_campaign_total_score(void *context, wchar_t *buffer, long buffer_length);
extern "C" void text_group_get_string(long tag_index, long string_id, c_static_wchar_string<1024> *string);
extern "C" c_gui_widget *gui_widget_find_child_by_name(c_gui_widget *widget, long type);
extern "C" c_gui_widget *gui_widget_get_next_sibling_of_type(c_gui_widget *widget, long type);
extern "C" short players_get_nearest_player_distance(const cs_point *);
extern "C" float scripted_sound_new_mission_dialogue_at_point(const real_point3d *, const real_vector3d *, long, long);

class c_gui_screen_widget;
#include "game/game_globals_storage.h"
struct s_campaing_metagame_globals;

#endif // INTERFACE_GUI_SCREEN_CARNAGE_REPORT_EXTERNS_H
