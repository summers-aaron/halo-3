// interfacelibxbox/user_interface_utilities.obj: from the records 0x1e92b0 0x4835a0 0x483608 0x483698 0x4838c0 0x483a10 0x483c40 0x483d68 0x483e80 0x483fa8 0x484068 0x484158 0x484240 0x4842c0 0x484370 0x484520 0x4845f0 0x484760 0x484b50 0x484c40 0x484c80 0x484cc0 0x484d20 0x4dfa10 0x4e3bc8 0x4e3cb8
#include <stdio.h>
#include <wchar.h>
#include "cseries/crt_xdk.h"
#include "text/draw_string_types.h"
// @address 0x187780 _vsnwprintf_s
#include "saved_games/saved_game_item_metadata.h"
#include "saved_games/saved_game_files.h"
// @flags /TP /O2 /Oc
#include "cseries/cseries_unicode.h"
#include "text/font_cache_lock.h"
#include "scenario/scenario_objects.h"
#include "scenario/scenario_object_name_types.h"
#include "text/font_id_types.h"
#include "scenario/scenario.h"
#include "memory/data_types.h"
#include "objects/object_header.h"
#include "tag_files/tag_groups.h"
#include "math/real_math_types.h"
#include "memory/static_arrays.h"
#include "tag_files/tag_types.h"
#include "shell/shell.h"
#include "game/player_types.h"
#include "game/game_options.h"
#include "game/game_globals_storage.h"
#include "objects/object_iterator.h"
#include "objects/object_datum.h"
#include "objects/object_types.h"
#include "objects/object_placement.h"
#include "ai/actor_datum.h"
#include "interface/gui_widget_types.h"
#include "interface/gui_widget.h"
#include "interface/gui_data.h"
#include "cseries/language_types.h"
#include "interface/user_interface_sound_effect_types.h"
#include "shell/shell_types.h"
// @address 0x73e2dc ?object_type_definitions@@3PAPAUobject_type_definition@@A
// @address 0x1e92b0 ?get_metadata_text@s_saved_game_item_metadata@@QBA_NJPAV?$c_static_wchar_string@$0EAA@@@@Z
// @address 0x4e3bc8 parse_current_day_of_week
// @address 0x483d68 user_interface_show_device_selection_dialog
// @address 0x483a10 user_interface_find_menu_character
// @address 0x4845f0 ?user_interface_get_date_string@@YA_N_JPAV?$c_static_wchar_string@$0EAA@@@@Z
// @address 0x484370 ?user_interface_get_bytes_size_string@@YA_NJPAV?$c_static_wchar_string@$0EAA@@@@Z
// @address 0x4e3cb8 user_interface_get_month_day_name
// @address 0x4835a0 user_interface_global_string_get
// @address 0x483608 user_interface_get_formatted_string
// @address 0x483fa8 ?user_interface_get_player_rank_string@@YA_NW4e_experience_rank@@W4e_experience_grade@@PAV?$c_static_wchar_string@$0EAA@@@@Z
// @address 0x484068 ?user_interface_get_player_grade_string@@YA_NW4e_experience_rank@@W4e_experience_grade@@_N2PAV?$c_static_wchar_string@$0EAA@@@@Z
// @address 0x484158 ?user_interface_get_player_rank_and_grade_string@@YA_NW4e_experience_rank@@W4e_experience_grade@@_NPAV?$c_static_wchar_string@$0EAA@@@@Z
// @address 0x484240 ?user_interface_get_player_total_xp@@YA_NJPAV?$c_static_wchar_string@$0EAA@@@@Z
// @address 0x484520 ?user_interface_get_percentage_string@@YA_NMPAV?$c_static_wchar_string@$0EAA@@@@Z
// @address 0x484b50 get_tint_color
// @address 0x484c40 user_interface_get_day_number
// @address 0x484c80 user_interface_get_month_number
// @address 0x484cc0 user_interface_get_year_number
// @address 0x4842c0 ?tint_widget_to_change_color@@YAXPAVc_gui_widget@@J_N@Z
// @address 0x484d20 ?print@?$c_static_wchar_string@$0EAA@@@QAAPB_WPB_WZZ
// @address 0x4dfa10 user_interface_parse_date_string
// @address 0x4838c0 user_interface_precache_font_charset
// @address 0x189498 ??0c_draw_string@@QAA@XZ
// @address 0x189960 ??0c_font_cache@@QAA@XZ

#include <stdarg.h>
#include "interface/gui_screens/scoreboard/gui_screen_scoreboard.h"
#include "interface/user_interface.h"
#include "interface/user_interface_memory.h"
#include "objects/objects.h"
#include "sound/game_sound.h"
#include "text/unicode.h"
#include "interface/user_interface_text.h"
#include "interface/user_interface_text_parser.h"
#include "interface/user_interface_utilities.h"
#include "interface/user_interface_utilities_string_ids.h"
#include "tag_files/string_ids.h"
#include "objects/model_customization.h"
#include "sound/sound_definitions.h"
#include "cseries/cseries.h"
#include "interface/game_tag_parser.h"
#include "interface/user_interface_error_manager.h"
#include "interface/controller_types.h"
#include "scenario/scenario_tag.h"
#include "sound/sound_cache_file_definitions.h"
#include "objects/object_type_definition.h"
#include "interface/user_interface_utilities_externs.h"
class c_font_cache;
#include "interface/user_interface_controller.h"
class c_message;
struct utf8;

#include "interface/user_interface_shared_globals.h"
#include "cseries/global_real_argb_colors.h"
#include "text/unicode_constants.h"
#include "cseries/language.h"
#include "ai/squad_definitions.h"
// @address 0x11401e8 g_controller_globals
// @address 0x1143b50 g_gui_error_manager
// @address 0xa74 k_empty_wide_string
// @address 0x10b0 k_font_cache_vtable
// @address 0x7e3640 g_sound_cache_file_gestalt
// @address 0x74b540 g_current_language


#include "interface/experience_rank_types.h"
#include "interface/experience_grade_types.h"

#include <time.h>
#include <string.h>
#include <ppcintrinsics.h>
#include <stdlib.h>
#include "interface/user_interface_messages.h"
#include <new>
#include "units/biped_types.h"
#include "memory/tls_globals.h"
#include "text/font_cache_types.h"
#include "sound/game_sound_impulse.h"
#include "sound/sound_cache_file_gestalt_globals.h"
#include "sound/sound_cache_file_gestalt.h"
#include "networking/transport/transport_security_globals.h"
#include "memory/datum_index.h"

// Element handles of the experience-rank icons by rank and grade (-1 where a rank has no icon at that grade).
// @address 0x7c8748 g_experience_rank_icon_element_handles
long g_experience_rank_icon_element_handles[k_experience_rank_count][k_experience_grade_count] = {
    { 0, -1, -1, -1 },
    { 1, -1, -1, -1 },
    { 2, 3, -1, -1 },
    { 4, 5, -1, -1 },
    { 6, 7, -1, -1 },
    { 8, 9, 10, -1 },
    { 11, 12, 13, 14 },
    { 15, 16, 17, 18 },
    { 19, 20, 21, 22 },
    { 23, 24, 25, 26 },
    { 27, 28, 29, 30 },
    { 31, 32, 33, 34 },
    { 35, 36, 37, 38 },
    { 39, 40, 41, 42 },
};


struct s_user_interface_sound_effects { s_tag_reference effects[20]; };

bool s_saved_game_item_metadata::get_metadata_text(long metadata_string_id, c_static_wchar_string<1024> *output_string) const
{
    wchar_t description_text[NUMBEROF(description)];
    utf8_string_to_wchar_string((const utf8 *)description, description_text, NUMBEROF(description_text), NULL);
    return saved_game_files_try_and_get_metadata_string(unique_id, name, description_text, author,
        file_type, author_is_online, author_id, size_in_bytes, date, length_seconds, campaign_id, map_id,
        game_engine_type, campaign_difficulty, campaign_insertion_point, game_id, metadata_string_id, output_string);
}
void tint_widget_to_change_color(c_gui_widget *widget, long color_index, bool team_color)
{
    if (widget) {
        long tint_type = -1;
        switch (widget->get_type()) {
        case _gui_text_widget:
            tint_type = team_color ? 1 : 0;
            break;
        case _gui_bitmap_widget:
            tint_type = (team_color ? 1 : 0) + 2;
            break;
        }
        if (tint_type != -1) {
            real_argb_color tint;
            widget->modulate_tint_color(get_tint_color(tint_type, color_index, &tint));
        }
    }
}

real_argb_color *get_tint_color(int tint_type, int color_index, real_argb_color *color)
{
    s_user_interface_shared_globals *definition = ((s_user_interface_shared_globals *)::user_interface_shared_tag_globals_try_and_get());
    const real_argb_color &default_color = global_real_argb_colors[_real_argb_white];
    *color = default_color;
    if (definition && definition->tint_color_lists.count > 0) {
        s_tint_color_list *list = (s_tint_color_list *)definition->tint_color_lists.address;
        switch (tint_type) {
        case 0: break;
        case 1: list += 1; break;
        case 2: list += 2; break;
        case 3: list += 3; break;
        // match: 0x484b50: __assume(0) keeps the 4-case jump table instead of compare chains
        default: __assume(0);
        }
        if (list && color_index >= 0 && color_index < list->count)
            *color = list->colors[color_index];
    }
    return color;
}


bool user_interface_global_string_get(int handle, unsigned short *out)
{
    bool result = false;
    *out = 0;
    if (handle != -1) {
        const s_user_interface_shared_globals *definition = ::user_interface_shared_tag_globals_try_and_get();
        if (definition && definition->global_strings.index != -1)
            result = text_group_get_string(definition->global_strings.index, handle, out);
    }
    return result;
}


static __forceinline bool user_interface_utilities_global_string_get(long string_id, c_static_wchar_string<1024> *string)
{
    bool result = false;
    string->m_string[0] = 0;
    if (string_id != -1)
    {
        const s_user_interface_shared_globals *definition = ::user_interface_shared_tag_globals_try_and_get();
        if (definition && definition->global_strings.index != -1)
            result = text_group_get_string(definition->global_strings.index, string_id, (unsigned short *)string->m_string);
    }
    return result;
}

#define user_interface_utilities_pin(v, l, u) (((v) > (l)) ? (((v) < (u)) ? (v) : (u)) : (l))


extern "C" bool user_interface_get_formatted_string(long user_index, long id, long string_id, wchar_t *output, long capacity)
{
    bool result = false;
    c_static_wchar_string<1024> string;
    bool overflowed;
    if (user_interface_global_string_get(string_id, (unsigned short *)string.m_string) &&
        user_interface_text_parse_xml(string.m_string, output, capacity, &overflowed, id) &&
        user_interface_parse_string(user_index, output, capacity))
    {
        result = true;
    }
    return result;
}



__forceinline long user_interface_get_player_rank_element_handle(e_experience_rank experience_rank, e_experience_grade experience_grade)
{
    long element_handle = -1;
    if (experience_rank >= 0 && experience_rank < k_experience_rank_count && experience_grade >= 0 && experience_grade < k_experience_grade_count)
        element_handle = g_experience_rank_icon_element_handles[experience_rank][experience_grade];
    return element_handle;
}

bool user_interface_get_player_rank_string(e_experience_rank experience_rank, e_experience_grade experience_grade, c_static_wchar_string<1024> *string)
{
    bool result = false;
    long element_handle = user_interface_get_player_rank_element_handle(experience_rank, experience_grade);
    c_gui_data *datasource = interface_get_global_datasource(_user_interface_utilities_string_player_rank);
    long string_id = -1;
    if (datasource && element_handle != -1 && datasource->get_string_id_value(element_handle, _string_id_name, &string_id))
        result = user_interface_global_string_get(string_id, (unsigned short *)string);
    return result;
}

bool user_interface_get_player_grade_string(e_experience_rank experience_rank, e_experience_grade experience_grade, bool long_form,
    bool use_blank, c_static_wchar_string<1024> *string)
{
    bool result = false;
    long element_handle = user_interface_get_player_rank_element_handle(experience_rank, experience_grade);
    c_gui_data *datasource = interface_get_global_datasource(_user_interface_utilities_string_player_rank);
    long string_id = -1;
    if (datasource && element_handle != -1 && datasource->get_string_id_value(element_handle,
        long_form ? _user_interface_utilities_string_player_grade_short : _user_interface_utilities_string_player_grade, &string_id))
    {
        result = user_interface_global_string_get(string_id, (unsigned short *)string);
    }
    else if (use_blank)
    {
        string->set(L" ");
        result = true;
    }
    return result;
}

// 0x484158
bool user_interface_get_player_rank_and_grade_string(
    e_experience_rank experience_rank, e_experience_grade experience_grade, bool long_form, c_static_wchar_string<1024> *string)
{
    bool success = false;
    c_static_wchar_string<1024> rank_string;
    if (user_interface_get_player_rank_string(experience_rank, experience_grade, &rank_string))
    {
        c_static_wchar_string<1024> grade_string;
        if (user_interface_get_player_grade_string(experience_rank, experience_grade, true, false, &grade_string))
        {
            c_static_wchar_string<1024> format_string;
            if (user_interface_utilities_global_string_get(_user_interface_utilities_string_player_rank_and_grade, &format_string))
            {
                string->print(format_string.get_string(), rank_string.get_string(), grade_string.get_string());
                success = true;
            }
        }
        else
        {
            string->set(rank_string.get_string());
            success = true;
        }
    }
    return success;
}

// 0x484240
bool user_interface_get_player_total_xp(long total_xp, c_static_wchar_string<1024> *string)
{
    bool success = false;
    c_static_wchar_string<1024> format_string;
    if (user_interface_utilities_global_string_get(_user_interface_utilities_string_xp_format, &format_string))
    {
        string->print(format_string.get_string(), total_xp);
        success = true;
    }
    return success;
}


bool user_interface_get_bytes_size_string(long bytes, c_static_wchar_string<1024> *string)
{
    bool success = false;
    long string_id;
    float value;
    if (bytes < 0)
        bytes = 0;
    if (bytes < 1024)
    {
        string_id = _user_interface_utilities_string_bytes;
        value = (float)bytes;
    }
    else if (bytes < 1024 * 1024)
    {
        string_id = _user_interface_utilities_string_kilobytes;
        value = (float)bytes * (1.0f / 1024.0f);
    }
    else if (bytes < 1024 * 1024 * 1024)
    {
        string_id = _user_interface_utilities_string_megabytes;
        value = (float)bytes * (1.0f / (1024.0f * 1024.0f));
    }
    else
    {
        string_id = _user_interface_utilities_string_gigabytes;
        value = (float)bytes * (1.0f / (1024.0f * 1024.0f * 1024.0f));
    }
    c_static_wchar_string<1024> suffix;
    if (user_interface_global_string_get(string_id, (unsigned short *)suffix.m_string))
    {
        if ((long)(value * 10.0f) % 10 == 0)
            string->print(L"%d %s", (long)value, suffix.m_string);
        else
            string->print(L"%.1f %s", (double)value, suffix.m_string);
        success = true;
    }
    return success;
}


template<long k_maximum_count>
const wchar_t *c_static_wchar_string<k_maximum_count>::print(const wchar_t *format, ...)
{
    va_list args;
    va_start(args, format);
    _vsnwprintf_s(m_string, k_maximum_count - 1, (unsigned int)-1, format, args);
    m_string[k_maximum_count - 1] = 0;
    return m_string;
}
template const wchar_t *c_static_wchar_string<1024>::print(const wchar_t *format, ...);



// 0x4e3bc8
extern "C" bool parse_current_day_of_week(void *context, wchar_t *buffer, long buffer_length)
{
    bool success = false;
    __time64_t current_time = _time64(NULL);
    struct tm local_time;
    if (_localtime64_s(&local_time, &current_time) == 0)
    {
        long string_id = -1;
        switch (local_time.tm_wday)
        {
        case 0: string_id = _user_interface_utilities_string_sunday; break;
        case 1: string_id = _user_interface_utilities_string_monday; break;
        case 2: string_id = _user_interface_utilities_string_tuesday; break;
        case 3: string_id = _user_interface_utilities_string_wednesday; break;
        case 4: string_id = _user_interface_utilities_string_thursday; break;
        case 5: string_id = _user_interface_utilities_string_friday; break;
        case 6: string_id = _user_interface_utilities_string_saturday; break;
        }
        if (string_id != -1)
        {
            c_static_wchar_string<1024> day_string;
            if (user_interface_global_string_get(string_id, (unsigned short *)day_string.m_string))
            {
                long length = buffer_length;
                if (length > sizeof(day_string.m_string) / sizeof(wchar_t))
                    length = sizeof(day_string.m_string) / sizeof(wchar_t);
                ustrnzcpy(buffer, day_string.m_string, length);
                success = true;
            }
        }
    }
    return success;
}

// 0x4e3cb8
extern "C" bool user_interface_get_month_day_name(void *context, wchar_t *buffer, long buffer_length)
{
    bool success = false;
    __time64_t current_time = _time64(NULL);
    struct tm local_time;
    if (_localtime64_s(&local_time, &current_time) == 0)
    {
        long string_id = -1;
        switch (local_time.tm_mday)
        {
        case 1: string_id = _user_interface_utilities_string_1; break;
        case 2: string_id = _user_interface_utilities_string_2; break;
        case 3: string_id = _user_interface_utilities_string_3; break;
        case 4: string_id = _user_interface_utilities_string_4; break;
        case 5: string_id = _user_interface_utilities_string_5; break;
        case 6: string_id = _user_interface_utilities_string_6; break;
        case 7: string_id = _user_interface_utilities_string_7; break;
        case 8: string_id = _user_interface_utilities_string_8; break;
        case 9: string_id = _user_interface_utilities_string_9; break;
        case 10: string_id = _user_interface_utilities_string_10; break;
        case 11: string_id = _user_interface_utilities_string_11; break;
        case 12: string_id = _user_interface_utilities_string_12; break;
        case 13: string_id = _user_interface_utilities_string_13; break;
        case 14: string_id = _user_interface_utilities_string_14; break;
        case 15: string_id = _user_interface_utilities_string_15; break;
        case 16: string_id = _user_interface_utilities_string_16; break;
        case 17: string_id = _user_interface_utilities_string_17; break;
        case 18: string_id = _user_interface_utilities_string_18; break;
        case 19: string_id = _user_interface_utilities_string_19; break;
        case 20: string_id = _user_interface_utilities_string_20; break;
        case 21: string_id = _user_interface_utilities_string_21; break;
        case 22: string_id = _user_interface_utilities_string_22; break;
        case 23: string_id = _user_interface_utilities_string_23; break;
        case 24: string_id = _user_interface_utilities_string_24; break;
        case 25: string_id = _user_interface_utilities_string_25; break;
        case 26: string_id = _user_interface_utilities_string_26; break;
        case 27: string_id = _user_interface_utilities_string_27; break;
        case 28: string_id = _user_interface_utilities_string_28; break;
        case 29: string_id = _user_interface_utilities_string_29; break;
        case 30: string_id = _user_interface_utilities_string_30; break;
        case 31: string_id = _user_interface_utilities_string_31; break;
        }
        if (string_id != -1)
        {
            c_static_wchar_string<1024> day_string;
            if (user_interface_global_string_get(string_id, (unsigned short *)day_string.m_string))
            {
                long length = buffer_length;
                if (length > sizeof(day_string.m_string) / sizeof(wchar_t))
                    length = sizeof(day_string.m_string) / sizeof(wchar_t);
                ustrnzcpy(buffer, day_string.m_string, length);
                success = true;
            }
        }
    }
    return success;
}

static __forceinline bool user_interface_controller_in_use(const c_controller_interface *controller)
{
    return controller->is_signed_in_to_machine() || controller->is_unsigned_in_user();
}

// 0x483d68
extern "C" void user_interface_show_device_selection_dialog(e_controller_index controller_index, long screen_name, long parent_screen_index)
{
    const c_controller_interface *controller = &g_controller_globals.controllers[controller_index];
    if (user_interface_controller_in_use(controller))
    {
        if (controller->is_signed_in_to_machine() && !controller->is_guest() && !controller->is_unsigned_in_user())
        {
            c_load_dialog_screen_message *message = 0;
            void *memory = ::user_interface_malloc_tracked(sizeof(c_load_dialog_screen_message), NULL, 0);
            if (memory)
                message = new (memory) c_load_dialog_screen_message(
                    controller_index, _console_window, screen_name,
                    _user_interface_utilities_string_need_to_select_storage_device_to_save, screen_name);
            if (message)
            {
                message->set_parent_screen_index(parent_screen_index);
                ::user_interface_messaging_post(message);
            }
        }
        else
        {
            g_gui_error_manager.post_error_with_custom_message(0x20083, 0, 0, controller_index, false, true);
        }
    }
}





// 0x483a10
extern "C" long user_interface_find_menu_character(const s_player_appearance *appearance)
{
    long object_index = NONE;
    if (tls_game_globals()->options.game_mode == 3)
    {
        const s_user_interface_shared_globals *shared_globals = ((s_user_interface_shared_globals *)::user_interface_shared_tag_globals_try_and_get());
        if (shared_globals)
        {
            const char *object_name = 0;
            const char *squad_name = 0;
            long starting_location_name = NONE;
            if (appearance->model_choice == 0)
            {
                object_name = shared_globals->characters[0].object_name;
                squad_name = shared_globals->characters[0].squad_name;
                starting_location_name = shared_globals->characters[0].starting_location_name;
            }
            else if (appearance->model_choice == 1)
            {
                object_name = shared_globals->characters[1].object_name;
                squad_name = shared_globals->characters[1].squad_name;
                starting_location_name = shared_globals->characters[1].starting_location_name;
            }
            if ((object_name && *object_name) || (squad_name && *squad_name && starting_location_name != NONE))
            {
                c_object_iterator<biped_datum> iterator;
                iterator.begin(1, 0);
                while (iterator.next() && object_index == NONE)
                {
                    const biped_datum *biped = iterator.get_datum();
                    if (object_name && *object_name)
                    {
                        const char *biped_object_name = 0;
                        if (biped->object.object_name_index != NONE)
                            biped_object_name = ((const s_scenario_object_name *)global_scenario->object_names.address)[biped->object.object_name_index].name;
                        if (biped_object_name && *biped_object_name && ::csstricmp((char const *)(biped_object_name), (char const *)(object_name)) == 0)
                            object_index = iterator.get_index();
                    }
                    else if (squad_name && *squad_name && starting_location_name != NONE)
                    {
                        if (biped->unit.actor_index != NONE)
                        {
                            const actor_datum *actor = (const actor_datum *)(tls_actor_data()->data +
                                datum_index_to_absolute_index(biped->unit.actor_index) * sizeof(actor_datum));
                            if (actor->squad_index != NONE && actor->cell_index != NONE && actor->starting_location_index != NONE)
                            {
                                const squad_definition *squad = &((const squad_definition *)global_scenario->squads.address)[
                                    datum_index_to_absolute_index(actor->squad_index)];
                                const fire_team_definition *cell = &((const fire_team_definition *)squad->fire_team_block.address)[actor->cell_index];
                                const actor_starting_location_definition *starting_location = &(
                                    (const actor_starting_location_definition *)cell->starting_locations.address)[actor->starting_location_index];
                                if (::csstricmp((char const *)(squad->name), (char const *)(squad_name)) == 0 &&
                                    starting_location->starting_location_name == starting_location_name)
                                    object_index = iterator.get_index();
                            }
                        }
                    }
                }
            }
        }
    }
    return object_index;
}


void user_interface_update_main_menu_character_model(const s_player_appearance *appearance)
{
    if (tls_game_globals()->options.game_mode == 3)
    {
        long object_index = user_interface_find_menu_character(appearance);
        if (object_index != NONE)
        {
            scenario *scenario = global_scenario;
            biped_datum *biped = (biped_datum *)object_get_and_verify_type(object_index, NONE);
            short placement_index = biped->object.scenario_datum_index;
            if (placement_index != NONE)
            {
                const s_scenario_object *scenario_object = 0;
                object_type_definition *definition = object_type_definitions[_object_type_biped];
                s_tag_block *datums = (s_tag_block *)((char *)scenario + definition->scenario_datums_offset);
                if (placement_index >= 0 && placement_index < datums->count)
                    scenario_object = (const s_scenario_object *)((char *)datums->address + definition->scenario_datum_size * placement_index);
                long palette_offset = definition->scenario_palette_offset;
                object_placement_data placement;
                if (object_placement_data_new_from_scenario_object(
                        _object_type_biped, placement_index, scenario_object,
                        (s_tag_block *)((char *)scenario + palette_offset), true, false, &placement)
                    && placement.definition_index != NONE)
                {
                    ::model_customization_get_overrides_for_placement(appearance, &placement);
                    object_type_unplace(object_index);
                    object_reinitialize_from_placement(object_index, &placement, false);
                    object_type_place(object_index, scenario_object);
                }
            }
        }
    }
}


// 0x484520
bool user_interface_get_percentage_string(float percentage, c_static_wchar_string<1024> *output)
{
    bool success = false;
    long value = (long)(user_interface_utilities_pin(percentage, 0.0f, 1.0f) * 100.0f);
    c_static_wchar_string<1024> suffix;
    if (user_interface_utilities_global_string_get(_user_interface_utilities_string_percentage, &suffix))
    {
        output->print(L"%d%s", value, suffix.get_string());
        success = true;
    }
    return success;
}

// 0x4845f0
bool user_interface_get_date_string(__int64 time, c_static_wchar_string<1024> *string)
{
    bool success = false;
    struct tm local_time;
    if (_localtime64_s(&local_time, &time) == 0)
    {
        wchar_t date_text[1024];
        date_text[0] = *(const wchar_t *)k_empty_wide_string;
        memset(&date_text[1], 0, sizeof(date_text) - sizeof(wchar_t));
        c_magic_string_game_tag_parser day_parser(L"<loc_day", &local_time, user_interface_get_day_number);
        c_magic_string_game_tag_parser month_parser(L"<loc_month", &local_time, user_interface_get_month_number);
        c_magic_string_game_tag_parser year_parser(L"<loc_year", &local_time, user_interface_get_year_number);
        g_parse_manager.register_parser(&day_parser, 'date');
        g_parse_manager.register_parser(&month_parser, 'date');
        g_parse_manager.register_parser(&year_parser, 'date');
        if (user_interface_get_formatted_string(NONE, 'date', _user_interface_utilities_string_ui_date_format, date_text, 1024))
        {
            string->set(date_text);
            success = date_text[0] != 0;
        }
        user_interface_text_parser_unregister(&day_parser, 'date');
        user_interface_text_parser_unregister(&month_parser, 'date');
        user_interface_text_parser_unregister(&year_parser, 'date');
    }
    return success;
}



// 0x484c40
extern "C" bool user_interface_get_day_number(void *context, wchar_t *output, long capacity)
{
    const struct tm *time = (const struct tm *)context;
    usnzprintf(output, capacity, L"%02d", time->tm_mday);
    return true;
}

// 0x484c80
extern "C" bool user_interface_get_month_number(void *context, wchar_t *output, long capacity)
{
    const struct tm *time = (const struct tm *)context;
    usnzprintf(output, capacity, L"%02d", time->tm_mon + 1);
    return true;
}

// 0x484cc0
extern "C" bool user_interface_get_year_number(void *context, wchar_t *output, long capacity)
{
    const struct tm *time = (const struct tm *)context;
    usnzprintf(output, capacity, L"%02d", (time->tm_year + 1900) % 100);
    return true;
}

// 0x4838c0
static __forceinline bool uiu_precache_charset(const wchar_t *charset, long font)
{
    c_user_interface_draw_string draw;
    c_font_cache cache;
    return draw.precache_wchar_string_for_font(&cache, charset, (e_font_id)font);
}

extern "C" int user_interface_precache_font_charset(long font)
{
    bool punctuation = uiu_precache_charset(L"`-=[]\\;',./~!@#$%^&*()_+{}||:\"<>?", font);
    bool digits = uiu_precache_charset(L"0123456789", font);
    bool uppercase = uiu_precache_charset(L"ABCDEFGHIJKLMNOPQRSTUVWXYZ", font);
    bool lowercase = uiu_precache_charset(L"abcdefghijklmnopqrstuvwxyz", font);
    return lowercase & uppercase & digits & punctuation & 1;
}

// 0x4dfa10
extern "C" bool user_interface_parse_date_string(void *context, wchar_t *buffer, long buffer_length)
{
    bool result = false;
    c_static_wchar_string<1024> date_string;
    __int64 current_time;
    _time64(&current_time);
    user_interface_get_date_string(current_time, &date_string);
    if (date_string.length() > 0)
    {
        ustrnzcpy(buffer, date_string.m_string, buffer_length);
        result = true;
    }
    return result;
}

extern "C" {
// @address 0x59f080 XTLGetLanguage
}
static __forceinline long user_interface_utilities_pitch_range_count(const s_cache_file_sound_definition *sound) {
    return sound->pitch_range_count;
}
static __forceinline s_cache_file_sound_pitch_range *user_interface_utilities_pitch_range(s_cache_file_sound_definition *sound, long index) {
    return &g_sound_cache_file_gestalt.data->pitch_ranges[sound->pitch_range_index + index];
}
static __forceinline long user_interface_utilities_pitch_range_name(const s_cache_file_sound_pitch_range *range) {
    return ((long *)g_sound_cache_file_gestalt.data->import_names.address)[range->name_index];
}
#define user_interface_utilities_current_language(result) do { \
    result = g_current_language; \
    if (result == -1) { \
        result = ((long)::get_e_language_from_xbox_language(XTLGetLanguage())); \
        g_current_language = (e_language)result; \
    } \
} while (0)

// 0x483698
// @address 0x483698 ?user_interface_play_sound@@YAXW4e_user_interface_sound_effect@@J@Z
void user_interface_play_sound(enum e_user_interface_sound_effect sound_effect, long override_index)
{
    const s_user_interface_shared_globals *globals = ::user_interface_shared_tag_globals_try_and_get();
    if (globals) {
        long sound_index = -1;
        if (((int)sound_effect) < 20) {
            if (override_index != -1) {
                s_user_interface_sound_effects *effects = (s_user_interface_sound_effects *)tag_get(0x75697365, override_index);
                sound_index = effects->effects[((int)sound_effect)].index;
            }
            if (sound_index == -1 && globals->sound_effects.index != -1) {
                s_user_interface_sound_effects *effects = (s_user_interface_sound_effects *)tag_get(0x75697365, globals->sound_effects.index);
                sound_index = effects->effects[((int)sound_effect)].index;
            }
        }
        if (sound_index != -1) {
            s_cache_file_sound_definition *sound = (s_cache_file_sound_definition *)tag_get(0x736e6421, sound_index);
            if (sound->flags & 0x40) {
                long name = -1;
                long language;
                user_interface_utilities_current_language(language);
                switch (language) {
                default: name = _user_interface_utilities_string_english; break;
                case _language_japanese: name = _user_interface_utilities_string_japanese; break;
                case _language_german: name = _user_interface_utilities_string_german; break;
                case _language_french: name = _user_interface_utilities_string_french; break;
                case _language_spanish: name = _user_interface_utilities_string_spanish; break;
                case _language_mexican: name = _user_interface_utilities_string_mexican; break;
                case _language_italian: name = _user_interface_utilities_string_italian; break;
                case _language_korean: name = _user_interface_utilities_string_korean; break;
                case _language_chinese_traditional: name = _user_interface_utilities_string_chinese; break;
                case _language_chinese_simplified: name = _user_interface_utilities_string_chinese; break;
                case _language_portuguese: name = _user_interface_utilities_string_portuguese; break;
                case _language_polish: name = _user_interface_utilities_string_polish; break;
                }
                if (name != -1) {
                    long count = user_interface_utilities_pitch_range_count(sound);
                    for (long index = 0; index < count; ++index) {
                        s_cache_file_sound_pitch_range *range = user_interface_utilities_pitch_range(sound, index);
                        if (user_interface_utilities_pitch_range_name(range) == name) {
                            s_game_sound_deterministic_permutation permutation;
                            permutation.pitch_range = (char)index;
                            permutation.permutation = (char)::sound_definition_next_permutation(sound, (short)index);
                            c_game_sound_impulse_parameters parameters(sound_index, 1.0f);
                            parameters.set_deterministic_permutation(&permutation);
                            (&parameters)->submit_unspatialized_sound();
                            break;
                        }
                    }
                }
            } else {
                ::unspatialized_impulse_sound_new(sound_index, 1.0f);
            }
        }
    }
}



// @address 0x483e80 user_interface_build_local_machine_and_players
extern "C" void user_interface_build_local_machine_and_players(
    s_machine_identifier *local_machine_identifier, long *local_player_count, game_player_options *local_players)
{
    const unsigned char *machine_identifier = g_transport_security_globals.local_identifier;
    memcpy(local_machine_identifier, machine_identifier, sizeof(s_machine_identifier));
    long player_count = 0;
    memset(local_players, 0, 16 * sizeof(game_player_options));
    long controller_index = 0;
    do {
        c_controller_interface *controller = &g_controller_globals.controllers[controller_index];
        unsigned char profile_valid = user_interface_controller_in_use(controller);
        if (profile_valid) {
            game_player_options *player = &local_players[player_count++];
            player->valid = true;
            memcpy(&player->machine_identifier, local_machine_identifier, sizeof(s_machine_identifier));
            player->user_index = (short)(unsigned short)g_controller_globals.controllers[controller_index].user_index;
            player->controller_index = controller_index;
            ::user_interface_controller_get_player_configuration((e_controller_index)controller_index,
                &player->player_identifier, &player->configuration);
        }
        long next = -1;
        if ((unsigned long)controller_index <= 2) next = controller_index + 1;
        controller_index = next;
    } while (controller_index != -1);
    *local_player_count = player_count;
}

// @address 0x484760 user_interface_get_timespan_string
extern "C" bool user_interface_get_timespan_string(long seconds, long format, bool leading_zero, c_static_wchar_string<1024> *output)
{
    bool result = false;
    long days = seconds / 60 / 60 / 24;
    long hours = (seconds / 60 / 60) % 24;
    long minutes = (seconds / 60) % 60;
    long remainder = seconds % 60;
    c_static_wchar_string<1024> format_string;
    if (format == 0 && days == 0)
        format = 1;
    if (format == 1 && hours == 0)
        format = 2;
    if (format == 2 && minutes == 0)
        format = 3;
    switch (format) {
    case 0:
        if (user_interface_utilities_global_string_get(_user_interface_utilities_string_date_format_days_hours_minutes_seconds, &format_string)) {
            output->print(format_string.get_string(), days, hours, minutes, remainder);
            result = true;
        }
        break;
    case 1:
        if (user_interface_utilities_global_string_get(_user_interface_utilities_string_date_format_hours_minutes_seconds, &format_string)) {
            output->print(format_string.get_string(), hours, minutes, remainder);
            result = true;
        }
        break;
    case 2:
        if (user_interface_utilities_global_string_get(_user_interface_utilities_string_date_format_minutes_seconds, &format_string)) {
            output->print(format_string.get_string(), minutes, remainder);
            result = true;
        }
        break;
    case 3:
        if (user_interface_utilities_global_string_get(_user_interface_utilities_string_date_format_seconds, &format_string)) {
            output->print(format_string.get_string(), remainder);
            result = true;
        }
        break;
    }
    return result;
}
