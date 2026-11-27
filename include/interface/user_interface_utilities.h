#ifndef INTERFACE_USER_INTERFACE_UTILITIES_H
#define INTERFACE_USER_INTERFACE_UTILITIES_H

// Functions defined in tu/interfacelibxbox/user_interface_utilities.cpp.

#include "interface/user_interface_sound.h"

class c_gui_widget;
struct s_player_appearance;
enum e_experience_rank;
enum e_experience_grade;
template<long k_maximum_count> class c_static_wchar_string;

bool user_interface_get_player_rank_string(e_experience_rank experience_rank, e_experience_grade experience_grade, c_static_wchar_string<1024> *string);
bool user_interface_get_player_grade_string(e_experience_rank experience_rank, e_experience_grade experience_grade, bool long_form, bool use_blank,
    c_static_wchar_string<1024> *string);
bool user_interface_get_player_rank_and_grade_string(e_experience_rank experience_rank, e_experience_grade experience_grade, bool long_form,
    c_static_wchar_string<1024> *string);
bool user_interface_get_player_total_xp(long total_xp, c_static_wchar_string<1024> *string);
bool user_interface_get_bytes_size_string(long bytes, c_static_wchar_string<1024> *string);
bool user_interface_get_date_string(__int64 time, c_static_wchar_string<1024> *string);
bool user_interface_get_percentage_string(float percentage, c_static_wchar_string<1024> *output);

void tint_widget_to_change_color(c_gui_widget *widget, long color_index, bool team_color);
void user_interface_update_main_menu_character_model(const s_player_appearance *appearance);
extern "C" bool user_interface_get_formatted_string(long user_index, long id, long string_id, wchar_t *output, long capacity);
extern "C" long user_interface_find_menu_character(const s_player_appearance *appearance);
extern "C" int user_interface_precache_font_charset(long font);
extern "C" bool user_interface_parse_date_string(void *context, wchar_t *buffer, long buffer_length);

struct game_player_options;
union real_argb_color;
struct s_machine_identifier;

extern "C" real_argb_color *get_tint_color(int tint_type, int color_index, real_argb_color *color);
extern "C" bool user_interface_global_string_get(int handle, unsigned short *out);
extern "C" bool user_interface_get_day_number(void *context, wchar_t *output, long capacity);
extern "C" bool user_interface_get_month_number(void *context, wchar_t *output, long capacity);
extern "C" bool user_interface_get_year_number(void *context, wchar_t *output, long capacity);
extern "C" void user_interface_build_local_machine_and_players(s_machine_identifier *local_machine_identifier, long *local_player_count,
    game_player_options *local_players);

extern "C" bool parse_current_day_of_week(void *context, wchar_t *buffer, long buffer_length);
extern "C" bool user_interface_get_month_day_name(void *context, wchar_t *buffer, long buffer_length);

#include "interface/experience_rank_types.h"
#include "interface/experience_grade_types.h"

extern "C" {
extern long g_experience_rank_icon_element_handles[k_experience_rank_count][k_experience_grade_count];
}

#endif // INTERFACE_USER_INTERFACE_UTILITIES_H
