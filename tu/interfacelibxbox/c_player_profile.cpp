#include "interface/user_interface_controller_globals.h"
// interfacelibxbox/c_player_profile.obj: from the records 0x8a590 0x8a6f0 0x47c7a8 0x47c8a0 0x47c928 0x47c9b0 0x47ca38 0x47caa0 0x47cb08 0x47cb70 0x47cbd8 0x47cc40 0x47cca8 0x47cd10 0x47cd78 0x47cde0 0x47ce60 0x47ced0 0x47cf80 0x47d080 0x47d0e8 0x47d1a0 0x47d208 0x47d270 0x47d2f8 0x47d360 0x47d3c0 0x47d4e0 0x47d548 0x47d5b0 0x47d618 0x47d698 0x47d7a0 0x47d8a0 0x47d988 0x47da50 0x47db28 0x47dc18 0x47dd08 0x47dd70 0x47ddd8 0x47de40 0x47dea8 0x47df40 0x47dfe8 0x47e070 0x47e4f8 0x47e5a8 0x47e968
// @compiler xdk6130
// @flags /TP /O2 /Oc
#include "math/real_math_types.h"
#include "interface/voice_output_setting_types.h"
#include "game/player_color_types.h"
#include "game/game_achievements_types.h"
#include "multithreading/thread_tls.h"
#include "game/game_types.h"
#include "shell/shell.h"
#include "game/player_types.h"
#include "cseries/async_xoverlapped.h"
#include "input/input_abstraction.h"
#include "input/input_abstraction_types.h"
#include "networking/online/online_xenon.h"
#include "main/levels.h"
#include "interface/c_player_profile.h"
#include "interface/c_player_profile_string_ids.h"
#include "networking/online/online_guide_xenon.h"
#include "interface/c_player_profile_storage.h"
#include "interface/user_interface_error_manager.h"
#include "interface/controller_types.h"
#include "interface/voice_mask_types.h"
#include "interface/look_sensitivity_types.h"
#include "interface/customization_types.h"
#include "multithreading/threads.h"
#include "main/game_preferences_defaults.h"
#include "cseries/language_types.h"
#include "cseries/language.h"
// @address 0x74b540 g_current_language
// @address 0x11401e8 g_controller_globals
// @address 0x27298 k_achievement_name_ids
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "networking/session/network_session_class.h"
#include "cseries/async_xoverlapped_types.h"
#include "main/thread_globals_lock.h"
#include "interface/user_interface_controller.h"

// @address 0x8a4c8 ?is_signed_in_to_machine@c_controller_interface@@QBA_NXZ
// @address 0x1ab068 ?ustrnupr@@YAPA_WPA_WJ@Z
// @address 0x1b7030 ?in_use@c_controller_interface@@QBA_NXZ
// @address 0x354d48 ?get_multiplayer_customized_model_character@@YAPAUs_multiplayer_customized_model_character@@W4e_player_model_choice@@@Z
// @address 0x3a53c0 ?gamer_has_achievement@@YA_NW4e_controller_index@@W4e_achievement@@@Z
// @address 0x47c8a0 ?set_controller_look_inverted@c_player_profile_interface@@QAAX_N0@Z
// @address 0x47c928 ?set_controller_auto_center_look@c_player_profile_interface@@QAAX_N0@Z
// @address 0x47c9b0 ?set_controller_flight_inverted@c_player_profile_interface@@QAAX_N0@Z
// @address 0x47ca38 ?set_button_preset@c_player_profile_interface@@QAAXW4e_button_presets@@_N@Z
// @address 0x47caa0 ?set_joystick_preset@c_player_profile_interface@@QAAXW4e_joystick_presets@@_N@Z
// @address 0x47cb08 ?set_unknown_setting_1c@c_player_profile_interface@@QAAXH_N@Z
// @address 0x47cb70 ?set_unknown_setting_20@c_player_profile_interface@@QAAXH_N@Z
// @address 0x47cbd8 ?set_unknown_setting_24@c_player_profile_interface@@QAAXH_N@Z
// @address 0x47cc40 ?set_unknown_setting_28@c_player_profile_interface@@QAAXH_N@Z
// @address 0x47cca8 ?set_unknown_setting_2c@c_player_profile_interface@@QAAXH_N@Z
// @address 0x47cd10 ?set_unknown_setting_30@c_player_profile_interface@@QAAXH_N@Z
// @address 0x47cd78 ?set_look_sensitivity@c_player_profile_interface@@QAAXW4e_look_sensitivity@@_N@Z
// @address 0x47cde0 ?set_last_campaign_map_played_absolute_index@c_player_profile_interface@@QAAXF_N@Z
// @address 0x47ce60 ?get_unknown_level_flag_40@c_player_profile_interface@@QBA_NJJW4e_campaign_difficulty_level@@@Z
// @address 0x47ced0 ?set_unknown_level_flag_40@c_player_profile_interface@@QAAXJJW4e_campaign_difficulty_level@@_N1@Z
// @address 0x47cf80 ?set_map_completed_at_difficulty_level@c_player_profile_interface@@QAAXFF_N0@Z
// @address 0x47d080 ?get_map_completed_at_difficulty_level@c_player_profile_interface@@QBA_NFF@Z
// @address 0x47d0e8 ?set_unknown_flag_9c@c_player_profile_interface@@QAAXF_N0@Z
// @address 0x47d1a0 ?set_unknown_setting_98@c_player_profile_interface@@QAAXF_N@Z
// @address 0x47d208 ?set_unknown_setting_9a@c_player_profile_interface@@QAAXF_N@Z
// @address 0x47d270 ?set_controller_crouch_lock_enabled@c_player_profile_interface@@QAAX_N0@Z
// @address 0x47d2f8 ?set_campaign_difficulty@c_player_profile_interface@@QAAXW4e_campaign_difficulty_level@@_N@Z
// @address 0x47d360 ?set_unknown_setting_c4@c_player_profile_interface@@QAAXH_N@Z
// @address 0x47d3c0 ?get_player_appearance@c_player_profile_interface@@QAAXPAUs_player_appearance@@@Z
// @address 0x47d4e0 ?set_primary_change_color@c_player_profile_interface@@QAAXW4e_player_color_index@@_N@Z
// @address 0x47d548 ?set_secondary_change_color@c_player_profile_interface@@QAAXW4e_player_color_index@@_N@Z
// @address 0x47d5b0 ?set_tertiary_change_color@c_player_profile_interface@@QAAXW4e_player_color_index@@_N@Z
// @address 0x47d618 ?set_player_model_choice@c_player_profile_interface@@QAAXW4e_player_model_choice@@_N@Z
// @address 0x47d698 ?set_emblem_info@c_player_profile_interface@@QAAXPBUs_emblem_info@@_N@Z
// @address 0x47d7a0 ?get_model_customization_selection@c_player_profile_interface@@QBADW4e_player_model_choice@@J@Z
// @address 0x47d8a0 ?set_model_customization_selection@c_player_profile_interface@@QAAXW4e_player_model_choice@@JD_N@Z
// @address 0x47d988 ?generate_random_service_tag@c_player_profile_interface@@CAXQA_W@Z
// @address 0x47da50 ?is_service_tag_validly_formatted@c_player_profile_interface@@SA_NPB_W@Z
// @address 0x47db28 ?set_desired_service_tag@c_player_profile_interface@@QAAXPB_W_N@Z
// @address 0x47dc18 ?set_service_tag_validated@c_player_profile_interface@@QAAXPB_W_N@Z
// @address 0x47dd08 ?set_service_tag_failed_verification@c_player_profile_interface@@QAAX_N0@Z
// @address 0x47dd70 ?set_voice_output_setting@c_player_profile_interface@@QAAXW4e_voice_output_setting@@_N@Z
// @address 0x47ddd8 ?set_voice_mask@c_player_profile_interface@@QAAXW4e_voice_mask@@_N@Z
// @address 0x47de40 ?set_unknown_setting_104@c_player_profile_interface@@QAAXH_N@Z
// @address 0x47dea8 player_profile_set_global_value
// @address 0x47df40 ?set_training_data@c_player_profile_interface@@QAAXPBUs_player_training_profile_data@@_N@Z
// @address 0x47dfe8 ?get_total_achievement_value@c_player_profile_interface@@QBAJXZ
// @address 0x47e070 ?get_hidden_unlockable_unlocked@c_player_profile_interface@@QBA_NJ@Z
// @address 0x47e4f8 ?dirty_or@c_player_profile_interface@@AAAX_N@Z
// @address 0x47e5a8 ?initialize_to_default_settings@c_player_profile_interface@@AAAXXZ
// @address 0x8a590 ?update@c_player_profile_interface@@QAAXW4e_controller_index@@@Z
// @address 0x8a6f0 ?update_service_tag@c_player_profile_interface@@AAAXXZ
// @address 0x3b3110 ?network_squad_session_get_session_class@@YA?AW4e_network_session_class@@XZ
// @address 0x19f3b0 ??0c_thread_globals_lock@@QAA@XZ
// @address 0x19f408 ??1c_thread_globals_lock@@QAA@XZ
// @address 0x47e968 ?propagate_settings@c_player_profile_interface@@QAAXXZ
// @address 0x20e3e8 ?input_abstraction_get_default_preferences@@YAXPAUs_gamepad_input_preferences@@@Z
// @address 0x46fc30 ?user_interface_controller_update_network_properties@@YAXW4e_controller_index@@@Z
#include "cseries/cseries.h"
#include "text/unicode.h"
#include "cseries/cseries_unicode.h"
#include <wchar.h>
// @address 0x58f5b0 wcsncmp
#include "objects/model_customization.h"
#include "networking/logic/network_session_interface.h"
#include "networking/online/online_achievements.h"
#include "interface/c_player_profile_externs.h"
struct s_code { union { wchar_t text[4]; __int64 bits; }; };









#include "objects/model_customization_types.h"









extern "C" {
// @address 0x1328 k_two_string_format
const wchar_t k_two_string_format[] = L"%s%s";
}

#include "main/game_preferences_types.h"
typedef char profile_brightness_offset[
    (sizeof(void *) != 4 || ((offsetof(s_global_preferences, primary_file)
    + offsetof(s_game_preferences_file_image, data)
    + offsetof(s_global_preferences_data, brightness_setting) == 0x2c16c))) ? 1 : -1];


static __forceinline int current_player_model_choice(c_player_profile_interface *profile)
{
    unsigned int mode = profile->customization.player_model_choice;
    if (2 <= mode) {
        mode = 0;
    }
    return mode;
}

static __forceinline bool char_in_range(wchar_t c, wchar_t low, wchar_t high)
{
    return c >= low && c <= high;
}



// 0x47c8a0
void c_player_profile_interface::set_controller_look_inverted(bool enabled, bool dirty)
{
    dirty_or(dirty && enabled != get_controller_look_inverted());
    if (enabled)
        flags |= 1;
    else
        flags &= ~1;
}

// 0x47c928
void c_player_profile_interface::set_controller_auto_center_look(bool enabled, bool dirty)
{
    dirty_or(dirty && enabled != get_controller_auto_center_look());
    if (enabled)
        flags |= 8;
    else
        flags &= ~8;
}

// 0x47c9b0
void c_player_profile_interface::set_controller_flight_inverted(bool enabled, bool dirty)
{
    dirty_or(dirty && enabled != get_controller_flight_inverted());
    if (enabled)
        flags |= 4;
    else
        flags &= ~4;
}

// 0x47ca38
void c_player_profile_interface::set_button_preset(e_button_presets value, bool dirty)
{
    if (value < 6) {
        dirty_or(dirty && value != get_button_preset());
        controls.button_preset = value;
    }
}

// 0x47caa0
void c_player_profile_interface::set_joystick_preset(e_joystick_presets value, bool dirty)
{
    if (value < 4) {
        dirty_or(dirty && value != get_joystick_preset());
        controls.joystick_preset = value;
    }
}

void c_player_profile_interface::set_unknown_setting_1c(int value, bool dirty)
{
    if (value < 10) {
        dirty_or(dirty && value != controls.unknown_setting_1c);
        controls.unknown_setting_1c = value;
    }
}

void c_player_profile_interface::set_unknown_setting_20(int value, bool dirty)
{
    if (value < 2) {
        dirty_or(dirty && value != camera.unknown_setting_20);
        camera.unknown_setting_20 = value;
    }
}

void c_player_profile_interface::set_unknown_setting_24(int value, bool dirty)
{
    if (value < 2) {
        dirty_or(dirty && value != camera.unknown_setting_24);
        camera.unknown_setting_24 = value;
    }
}

void c_player_profile_interface::set_unknown_setting_28(int value, bool dirty)
{
    if (value < 4) {
        dirty_or(dirty && value != camera.unknown_setting_28);
        camera.unknown_setting_28 = value;
    }
}

void c_player_profile_interface::set_unknown_setting_2c(int value, bool dirty)
{
    if (value < 4) {
        dirty_or(dirty && value != camera.unknown_setting_2c);
        camera.unknown_setting_2c = value;
    }
}

// 0x47cd10
void c_player_profile_interface::set_unknown_setting_30(int value, bool dirty)
{
    if (value < 3) {
        dirty_or(dirty && value != camera.unknown_setting_30);
        camera.unknown_setting_30 = value;
    }
}

// 0x47cd78
void c_player_profile_interface::set_look_sensitivity(e_look_sensitivity value, bool dirty)
{
    if (value < 4) {
        dirty_or(dirty && value != get_look_sensitivity());
        campaign.look_sensitivity = value;
    }
}

void c_player_profile_interface::set_last_campaign_map_played_absolute_index(short index, bool notify)
{
    if ((index < 0 || index >= 32) && index != -1)
        return;
    dirty_or(notify && index != campaign.last_campaign_map_played_absolute_index);
    campaign.last_campaign_map_played_absolute_index = index;
}

// 0x47ce60
bool c_player_profile_interface::get_unknown_level_flag_40(long table, long level, e_campaign_difficulty_level difficulty) const
{
    bool result = false;
    if ((level == -1 || level >= 0 && level < sizeof(campaign.map_completed_flags[table])) && difficulty >= 0 && difficulty < 4)
        result = level != -1 ? (campaign.map_completed_flags[table][level] & (1 << difficulty)) != 0 : false;
    return result;
}

// 0x47ced0
void c_player_profile_interface::set_unknown_level_flag_40(long table, long level, e_campaign_difficulty_level difficulty, bool value, bool notify)
{
    if (level >= 0 && (unsigned int)level < 32 && difficulty >= 0 && difficulty < 4)
    {
        dirty_or(notify && value != get_unknown_level_flag_40(table, level, difficulty));
        if (value)
            campaign.map_completed_flags[table][level] |= 1 << difficulty;
        else
            campaign.map_completed_flags[table][level] &= ~(1 << difficulty);
    }
}

#define BIT_VECTOR_OR_FLAG(vector, bit) ((vector)[(bit) >> 5] |= (1 << ((bit) & 31)))
#define BIT_VECTOR_AND_FLAG(vector, bit) ((vector)[(bit) >> 5] &= ~(1 << ((bit) & 31)))
#define BIT_VECTOR_SET_FLAG(vector, bit, enable) { if (enable) BIT_VECTOR_OR_FLAG(vector, bit); else BIT_VECTOR_AND_FLAG(vector, bit); }

// 0x47cf80
void c_player_profile_interface::set_map_completed_at_difficulty_level(short map, short difficulty, bool completed, bool dirty)
{
    unsigned long old_campaign_map_completion[4];
    memcpy(old_campaign_map_completion, campaign.campaign_map_completion, sizeof(campaign.campaign_map_completion));
    if (map >= 0 && map < 32 && difficulty >= 0 && difficulty < 4)
        BIT_VECTOR_SET_FLAG(campaign.campaign_map_completion, map * 4 + difficulty, completed);
    dirty_or(dirty && memcmp(old_campaign_map_completion, campaign.campaign_map_completion, sizeof(campaign.campaign_map_completion)) != 0);
}

// 0x47d080
bool c_player_profile_interface::get_map_completed_at_difficulty_level(short map, short difficulty) const
{
    bool completed = false;
    if (map >= 0 && map < 32 && difficulty >= 0 && difficulty < 4) {
        long bit = map * 4 + difficulty;
        completed = (campaign.campaign_map_completion[bit >> 5] & (1 << (bit & 31))) != 0;
    }
    return completed;
}

// 0x47d0e8
void c_player_profile_interface::set_unknown_flag_9c(short index, bool enabled, bool dirty)
{
    unsigned long old_campaign_map_unlocked[1];
    memcpy(old_campaign_map_unlocked, campaign.campaign_map_unlocked, sizeof(campaign.campaign_map_unlocked));
    if (index >= 0 && index < 32) {
        if (enabled)
            campaign.campaign_map_unlocked[index >> 5] |= 1 << (index & 31);
        else
            campaign.campaign_map_unlocked[index >> 5] &= ~(1 << (index & 31));
    }
    dirty_or(dirty && memcmp(old_campaign_map_unlocked, campaign.campaign_map_unlocked, sizeof(campaign.campaign_map_unlocked)) != 0);
}

// 0x47d1a0
void c_player_profile_interface::set_unknown_setting_98(short value, bool dirty)
{
    dirty_or(dirty && value != campaign.unknown_setting_98);
    campaign.unknown_setting_98 = value;
}

// 0x47d208
void c_player_profile_interface::set_unknown_setting_9a(short value, bool dirty)
{
    dirty_or(dirty && value != campaign.unknown_setting_9a);
    campaign.unknown_setting_9a = value;
}

// 0x47d270
void c_player_profile_interface::set_controller_crouch_lock_enabled(bool enabled, bool dirty)
{
    dirty_or(dirty && enabled != get_controller_crouch_lock_enabled());
    if (enabled)
        flags |= 16;
    else
        flags &= ~16;
}

// 0x47d2f8
void c_player_profile_interface::set_campaign_difficulty(e_campaign_difficulty_level value, bool dirty)
{
    if (value < 5) {
        dirty_or(dirty && value != get_campaign_difficulty());
        last_played.campaign_difficulty = value;
    }
}

void c_player_profile_interface::set_unknown_setting_c4(int value, bool dirty)
{
    dirty_or(dirty && value != last_played.unknown_setting_c4);
    last_played.unknown_setting_c4 = value;
}

// 0x47d3c0
void c_player_profile_interface::get_player_appearance(s_player_appearance *appearance)
{
    appearance->flags = 0;
    appearance->flags = (unsigned char)((flags >> 4) & 1);
    appearance->primary_color = (signed char)customization.primary_change_color;
    appearance->secondary_color = (signed char)customization.secondary_change_color;
    appearance->tertiary_color = (signed char)customization.tertiary_change_color;
    unsigned int model = customization.player_model_choice;
    if (model >= 2)
        model = 0;
    appearance->model_choice = (signed char)model;
    appearance->emblem = get_emblem_info();
    int choice = 0;
    do {
        int index = 0;
        do {
            appearance->selections[choice][index] = get_model_customization_selection((e_player_model_choice)choice, index);
            index++;
        } while (index < 4);
        choice++;
    } while (choice < 2);
    if (network_squad_session_get_session_class() == 2)
        ustrnzcpy_inline(appearance->service_tag, customization.service_tag, 4);
    else
        ustrnzcpy_inline(appearance->service_tag, customization.desired_service_tag, 4);
}

// 0x47d4e0
void c_player_profile_interface::set_primary_change_color(e_player_color_index value, bool dirty)
{
    if (value < 32) {
        dirty_or(dirty && value != get_primary_change_color());
        customization.primary_change_color = value;
    }
}

// 0x47d548
void c_player_profile_interface::set_secondary_change_color(e_player_color_index value, bool dirty)
{
    if (value < 32) {
        dirty_or(dirty && value != get_secondary_change_color());
        customization.secondary_change_color = value;
    }
}

// 0x47d5b0
void c_player_profile_interface::set_tertiary_change_color(e_player_color_index value, bool dirty)
{
    if (value < 32) {
        dirty_or(dirty && value != get_tertiary_change_color());
        customization.tertiary_change_color = value;
    }
}

void c_player_profile_interface::set_player_model_choice(e_player_model_choice choice, bool dirty)
{
    if (choice != spartan && choice != elite) {
        return;
    }
    dirty_or(dirty && choice != current_player_model_choice(this));
    customization.player_model_choice = choice;
}

void c_player_profile_interface::set_emblem_info(const s_emblem_info *info, bool dirty)
{
    if (info->foreground >= 0 && info->foreground < 80 &&
        info->background >= 0 && info->background < 48 &&
        info->primary_color >= 0 && info->primary_color < 32 &&
        info->secondary_color >= 0 && info->secondary_color < 32 &&
        info->background_color >= 0 && info->background_color < 32)
    {
        dirty_or(dirty && memcmp(info, &customization.emblem_info, sizeof(s_emblem_info)) != 0);
        customization.emblem_info = *info;
    }
}


// 0x47d7a0
char c_player_profile_interface::get_model_customization_selection(e_player_model_choice model, long part) const
{
    char result = 0;
    s_multiplayer_customized_model_character *model_character = get_multiplayer_customized_model_character(model);
    if (model_character && model >= 0 && (unsigned long)model < 2 && part >= 0 && part < model_character->regions.count()) {
        s_multiplayer_customized_model_region *selection = &((s_multiplayer_customized_model_region *)model_character->regions.address)[part];
        if (customization.model_customization_selections[model][part] >= 0 &&
            customization.model_customization_selections[model][part] < selection->permutations.count()) {
            s_multiplayer_customized_model_permutation *entry = &(
                (s_multiplayer_customized_model_permutation *)selection->permutations.address)[customization.model_customization_selections[model][part]];
            bool allowed = true;
            if (entry->flags & 1) {
                if (!get_hidden_unlockable_unlocked(entry->unlockable))
                    allowed = false;
            }
            if (entry->flags & 2) {
                if (!get_hidden_unlockable_unlocked(entry->unlockable))
                    allowed = false;
            }
            if (allowed)
                result = customization.model_customization_selections[model][part];
        }
    }
    return result;
}

void c_player_profile_interface::set_model_customization_selection(e_player_model_choice model, long part, char choice, bool dirty)
{
    s_multiplayer_customized_model_character *model_character = get_multiplayer_customized_model_character(model);
    if (model_character && model >= 0 && (unsigned long)model < 2 && part >= 0 && part < model_character->regions.count()) {
        s_multiplayer_customized_model_region *selection = &((s_multiplayer_customized_model_region *)model_character->regions.address)[part];
        if (choice >= 0 && choice < selection->permutations.count()) {
            dirty_or(dirty && customization.model_customization_selections[model][part] != choice);
            customization.model_customization_selections[model][part] = choice;
        } else {
            customization.model_customization_selections[model][part] = 0;
        }
    }
}

void c_player_profile_interface::generate_random_service_tag(wchar_t *const text)
{
    text[0] = 'A' + rand() % 26;
    text[1] = '0' + rand() % 10;
    int r = rand();
    text[3] = 0;
    text[2] = '0' + r % 10;
}

bool c_player_profile_interface::is_service_tag_validly_formatted(const wchar_t *text)
{
    wchar_t buffer[4];
    int i = 0;
    wchar_t *destination = buffer;
    for (; i <= 3; i++) {
        *destination = *text;
        if (!*text)
            break;
        text++;
        destination++;
    }
    buffer[3] = 0;
    unsigned int length;
    for (length = 0; length < 4 && buffer[length]; length++)
        ;
    bool valid = (length == 3) & 1;
    valid = valid & char_in_range(buffer[0], 'A', 'Z');
    valid = valid & char_in_range(buffer[1], '0', '9');
    return valid & char_in_range(buffer[2], '0', '9');
}



static __forceinline void copy_code(wchar_t *destination, const wchar_t *source, int count)
{
    for (int i = 0; i <= count - 1; ++i) {
        if ((destination[i] = *source) == 0)
            break;
        ++source;
    }
    destination[count - 1] = 0;
}


void c_player_profile_interface::set_desired_service_tag(const wchar_t *text, bool compare)
{
    if (is_service_tag_validly_formatted(text)) {
        s_code code;
        memset(&code, 0, sizeof(code));
        ustrnzcpy_inline(code.text, text, 4);
        ustrnupr(code.text, 4);
        dirty_or(compare && memcmp(&code, &customization.desired_service_tag, sizeof(s_code)) != 0);
        memcpy(&customization.desired_service_tag, &code, sizeof(code));
        customization.service_tag_was_randomly_generated = 0;
        customization.service_tag_failed_verification = 0;
    }
}

void c_player_profile_interface::set_service_tag_validated(const wchar_t *text, bool compare)
{
    if (is_service_tag_validly_formatted(text)) {
        s_code code;
        memset(&code, 0, sizeof(code));
        copy_code(code.text, text, 4);
        ustrnupr(code.text, 4);
        dirty_or(compare && memcmp(&code, customization.service_tag, sizeof(code)) != 0);
        memcpy(customization.service_tag, &code, sizeof(code));
        customization.service_tag_failed_verification = 0;
        customization.service_tag_was_randomly_generated = 0;
    }
}


// 0x47dd08
void c_player_profile_interface::set_service_tag_failed_verification(bool failed, bool dirty)
{
    dirty_or(dirty && failed != get_service_tag_failed_verification());
    customization.service_tag_failed_verification = failed;
}

// 0x47dd70
void c_player_profile_interface::set_voice_output_setting(e_voice_output_setting value, bool dirty)
{
    if (value < 2) {
        dirty_or(dirty && value != voice.voice_output_setting);
        voice.voice_output_setting = value;
    }
}

// 0x47ddd8
void c_player_profile_interface::set_voice_mask(e_voice_mask value, bool dirty)
{
    if (value < 2) {
        dirty_or(dirty && value != voice.voice_mask);
        voice.voice_mask = value;
    }
}

// 0x47de40
void c_player_profile_interface::set_unknown_setting_104(int value, bool dirty)
{
    if (value < 2) {
        dirty_or(dirty && value != voice.unknown_setting_104);
        voice.unknown_setting_104 = value;
    }
}

// 0x47dea8: stores value (below 5) into the thread globals under their lock and marks them dirty
extern "C" void player_profile_set_global_value(long value)
{
    if (value < 5) {
        if (!global_preferences_should_use_defaults()) {
            c_thread_globals_lock lock;
            s_global_preferences *globals = thread_global_preferences;
            globals->primary_file.data.brightness_setting = value;
            globals->dirty = true;
        }
    }
}

// 0x47df40
void c_player_profile_interface::set_training_data(const s_player_training_profile_data *data, bool dirty)
{
    dirty_or(dirty && memcmp(data, &training_data, sizeof(training_data)) != 0);
    memcpy(&training_data, data, sizeof(training_data));
}

extern "C" {
// @address 0x8086b8 g_achievement_progress
unsigned long g_achievement_progress[49];
}


static __forceinline unsigned long c_player_profile_get_achievement_value(long achievement)
{
    if ((unsigned long)achievement <= 48)
        return g_achievement_progress[achievement];
    return 0;
}

// 0x47dfe8
long c_player_profile_interface::get_total_achievement_value() const
{
    long total = 0;
    for (long achievement = 0; achievement < 49; achievement++) {
        if (gamer_has_achievement((e_controller_index)controller_index, (e_achievement)achievement))
            total += c_player_profile_get_achievement_value(achievement);
    }
    return total;
}

// the debug override that unlocks every hidden unlockable; off in this build
static __forceinline bool c_player_profile_unlock_all_hidden_unlockables()
{
    return false;
}

static __forceinline bool c_player_profile_ui_controller_test_flag(long controller_index, long bit)
{
    return ((g_user_interface_controller_globals.controllers[controller_index].flags >> bit) & 1) != 0;
}

static __forceinline long c_player_profile_achievement_from_name(long name)
{
    for (long achievement = 0; achievement < 49; achievement++) {
        if (k_achievement_name_ids[achievement] == name)
            return achievement;
    }
    return -1;
}

static __forceinline bool c_player_profile_gamer_has_achievement_by_name(e_controller_index controller_index, long name)
{
    e_controller_index controller = controller_index;
    long achievement = c_player_profile_achievement_from_name(name);
    if (achievement >= 0 && achievement < 49)
        return gamer_has_achievement(controller, (e_achievement)achievement);
    return false;
}

#if !defined(_XBOX) || defined(__clang__)
extern "C" long port_achievements_earned_gamerscore(long user_index);
#endif

// 0x47e070
bool c_player_profile_interface::get_hidden_unlockable_unlocked(long unlockable) const
{
    bool unlocked;
    switch (unlockable) {
    case _player_profile_string_community:
        unlocked = c_player_profile_ui_controller_test_flag(controller_index, 3) || c_player_profile_unlock_all_hidden_unlockables()
#if !defined(_XBOX) || defined(__clang__)
            || port_achievements_earned_gamerscore(controller_index) >= 1750
#endif
            ;
        break;
    case _player_profile_string_community2:
        unlocked = c_player_profile_ui_controller_test_flag(controller_index, 4) || c_player_profile_unlock_all_hidden_unlockables();
        break;
    case _player_profile_string_community3:
        unlocked = c_player_profile_ui_controller_test_flag(controller_index, 5) || c_player_profile_unlock_all_hidden_unlockables();
        break;
    case _player_profile_string_bungie:
        unlocked = c_player_profile_ui_controller_test_flag(controller_index, 2) || c_player_profile_unlock_all_hidden_unlockables();
        break;
    case _player_profile_string_achievements_250:
        unlocked = get_total_achievement_value() >= 250 || c_player_profile_unlock_all_hidden_unlockables();
        break;
    case _player_profile_string_achievements_500:
        unlocked = get_total_achievement_value() >= 500 || c_player_profile_unlock_all_hidden_unlockables();
        break;
    case _player_profile_string_achievements_750:
        unlocked = get_total_achievement_value() >= 750 || c_player_profile_unlock_all_hidden_unlockables();
        break;
    case _player_profile_string_achievements_1000:
        unlocked = get_total_achievement_value() >= 1000 || c_player_profile_unlock_all_hidden_unlockables();
        break;
    case _player_profile_string_skulls_first_tier:
        unlocked = get_skulls_found_count() >= 5 || c_player_profile_unlock_all_hidden_unlockables();
        break;
    case _player_profile_string_skulls_second_tier:
        unlocked = get_skulls_found_count() >= 9 || c_player_profile_unlock_all_hidden_unlockables();
        break;
    case _player_profile_string_skulls_third_tier:
        unlocked = get_skulls_found_count() >= 13 || c_player_profile_unlock_all_hidden_unlockables();
        break;
    case _player_profile_string_completed_030_heroic_or_legendary: {
        short level = levels_get_campaign_map_index(1, 3030);
        unlocked = get_unknown_level_flag_40_inline(0, level, _campaign_difficulty_level_heroic)
            || get_unknown_level_flag_40_inline(0, level, _campaign_difficulty_level_legendary)
            || get_unknown_level_flag_40_inline(1, level, _campaign_difficulty_level_heroic)
            || get_unknown_level_flag_40_inline(1, level, _campaign_difficulty_level_legendary)
            || c_player_profile_unlock_all_hidden_unlockables();
        break;
    }
    case _player_profile_string_completed_030_legendary: {
        short level = levels_get_campaign_map_index(1, 3030);
        unlocked = get_unknown_level_flag_40_inline(0, level, _campaign_difficulty_level_legendary)
            || get_unknown_level_flag_40_inline(1, level, _campaign_difficulty_level_legendary)
            || c_player_profile_unlock_all_hidden_unlockables();
        break;
    }
    case _player_profile_string_completed_070_heroic_or_legendary: {
        short level = levels_get_campaign_map_index(1, 3070);
        unlocked = get_unknown_level_flag_40_inline(0, level, _campaign_difficulty_level_heroic)
            || get_unknown_level_flag_40_inline(0, level, _campaign_difficulty_level_legendary)
            || get_unknown_level_flag_40_inline(1, level, _campaign_difficulty_level_heroic)
            || get_unknown_level_flag_40_inline(1, level, _campaign_difficulty_level_legendary)
            || c_player_profile_unlock_all_hidden_unlockables();
        break;
    }
    case _player_profile_string_completed_070_legendary: {
        short level = levels_get_campaign_map_index(1, 3070);
        unlocked = get_unknown_level_flag_40_inline(0, level, _campaign_difficulty_level_legendary)
            || get_unknown_level_flag_40_inline(1, level, _campaign_difficulty_level_legendary)
            || c_player_profile_unlock_all_hidden_unlockables();
        break;
    }
    default:
        unlocked = (unlockable != -1 && unlockable != 0
                && c_player_profile_gamer_has_achievement_by_name((e_controller_index)controller_index, unlockable))
            || c_player_profile_unlock_all_hidden_unlockables();
        break;
    }
    return unlocked;
}

void c_player_profile_interface::dirty_or(bool force)
{
    int update = (((flags >> 5) & 1) | force) & 0xff;
    if (update) {
        if (controller_index >= 0 && controller_index < 4) {
            c_controller_interface *controller = &g_controller_globals.controllers[controller_index];
            if (controller->is_signed_in_to_machine())
                flags |= 0x20;
            if (controller->in_use())
                flags |= 0x80;
        }
    }
}


// @address 0x47c7a8 ??0c_player_profile_interface@@QAA@XZ
c_player_profile_interface::c_player_profile_interface() :
    flags(0),
    profile_read_retry_count(0),
    profile_read_retry_time(0),
    controller_index(-1),
    controls(),
    campaign(),
    last_played(),
    customization(),
    voice(),
    gamer_region(100),
    gamer_zone(1),
    training_data(),
    m_verify_task("..\\interface\\c_player_profile.cpp", 55)
{
    initialize_to_default_settings();
}

// 0x47e5a8
void c_player_profile_interface::initialize_to_default_settings()
{
    flags = 0;
    profile_read_retry_count = 0;
    profile_read_retry_time = 0;
    controls.button_preset = button_preset_default;
    controls.joystick_preset = joystick_preset_default;
    controls.unknown_setting_1c = 0;
    memset(&camera, 0, sizeof(camera));
    memset(&campaign, 0, sizeof(campaign));
    memset(&last_played, 0, sizeof(last_played));
    memset(&customization, 0, sizeof(customization));
    memset(&voice, 0, sizeof(voice));
    memset(&unknown_bool_114, 0, sizeof(unknown_bool_114) + sizeof(unknown_bool_115) + sizeof(unknown_bool_116));
    gamer_region = 100;
    gamer_zone = 1;
    memset(&training_data, 0, sizeof(training_data));

    srand((unsigned int)time(NULL));
    set_controller_look_inverted(false, false);
    set_controller_vibration_enabled(true, false);
    set_controller_flight_inverted(false, false);
    set_button_preset_inline(button_preset_default, false);
    set_joystick_preset_inline(joystick_preset_default, false);
    set_unknown_setting_1c_inline(2, false);
    set_unknown_setting_20_inline(0, false);
    set_unknown_setting_24_inline(0, false);
    set_unknown_setting_28_inline(1, false);
    set_unknown_setting_2c_inline(1, false);
    set_unknown_setting_30_inline(1, false);
    set_look_sensitivity_inline((e_look_sensitivity)_campaign_difficulty_level_normal, false);
    set_last_campaign_played_absolute_index(-1, false);
    set_last_campaign_map_played_absolute_index(-1, false);
    memset(campaign.map_completed_flags, 0, sizeof(campaign.map_completed_flags));
    set_last_network_game_hopper_played(-1, false);
    set_last_variant_played(0, -1, false);
    set_last_custom_multiplayer_map_played(-1, false);
    set_primary_change_color_inline(player_color_13, false);
    set_secondary_change_color_inline(player_color_13, false);
    set_tertiary_change_color_inline(player_color_13, false);
    set_player_model_choice_inline(spartan, false);

    s_emblem_info emblem;
    memset(&emblem, 0, sizeof(emblem));
    emblem.foreground = rand() % 80;
    emblem.background = rand() % 48;
    emblem.primary_color = 27;
    emblem.secondary_color = 10;
    emblem.background_color = 2;
    emblem.flags = 0;
    set_emblem_info(&emblem, false);

    memset((long *)customization.model_customization_selections, 0, sizeof(customization.model_customization_selections));
    customization.service_tag[0] = '0';
    customization.service_tag[1] = '0';
    customization.service_tag[2] = '0';
    customization.service_tag[3] = 0;
    generate_random_service_tag(customization.desired_service_tag);
    customization.service_tag_was_randomly_generated = true;
    customization.service_tag_failed_verification = false;
    set_voice_output_setting_inline(_voice_output_setting_default, false);
    set_voice_mask_inline(voice_mask_none, false);
    set_unknown_setting_104_inline(0, false);
    set_unknown_setting_108(0, false);
    set_unknown_bool_114(true, false);
    set_unknown_bool_115(true, false);
    set_unknown_bool_116(true, false);
    set_campaign_difficulty_inline(_campaign_difficulty_level_easy, false);
    set_unknown_setting_c4(-1, false);
}

// 0x8a6f0
void c_player_profile_interface::update_service_tag()
{
    c_controller_interface *profile_controller = &g_controller_globals.controllers[controller_index];
    c_controller_interface *controller = (c_controller_interface *)profile_controller;
    bool in_use;
    in_use = controller->is_signed_in_to_machine() || profile_controller->is_unsigned_in_user();
    if (in_use && ::online_local_xuid_is_online_enabled() && wcsncmp(customization.desired_service_tag, customization.service_tag, 4) != 0 &&
        !customization.service_tag_failed_verification) {
        if (!m_verify_task.busy()) {
            wchar_t strings[3][256];
            const wchar_t *strings_to_verify[3];
            const wchar_t *player_name = profile_controller->display_name;
            ustrnzcpy_inline(strings[0], customization.desired_service_tag, 256);
            usnzprintf(strings[1], 256, k_two_string_format, customization.desired_service_tag, player_name);
            usnzprintf(strings[2], 256, k_two_string_format, player_name, customization.desired_service_tag);
            strings_to_verify[0] = strings[0];
            strings_to_verify[1] = strings[1];
            strings_to_verify[2] = strings[2];
            bool strings_match = false;
            if (m_verify_task.get_strings_to_verify_count() == 3) {
                strings_match = true;
                for (int i = 0; i < 3; i++)
                    strings_match &= wcsncmp(m_verify_task.get_string_to_verify(i), strings[i], 256) == 0;
            }
            if (strings_match) {
                if ((m_verify_task.string_verify_succeeded(2) & m_verify_task.string_verify_succeeded(1) & m_verify_task.string_verify_succeeded(0) & 1) != 0) {
                    set_service_tag_validated(customization.desired_service_tag, true);
                } else if (customization.service_tag_was_randomly_generated) {
                    generate_random_service_tag(customization.desired_service_tag);
                    m_verify_task.reset();
                    long language = g_current_language;
                    if (language == -1) {
                        language = ::get_e_language_from_xbox_language(XTLGetLanguage());
                        g_current_language = (e_language)language;
                    }
                    string_verify_task_set_strings(&m_verify_task, language, strings_to_verify, 3);
                    overlapped_task_start_internal(&m_verify_task, "..\\interface\\c_player_profile.cpp", 213);
                } else {
                    set_service_tag_failed_verification(true, true);
                    g_gui_error_manager.post_error(0x2005f, (e_controller_index)controller_index, false);
                }
            } else {
                m_verify_task.reset();
                long language = g_current_language;
                if (language == -1) {
                    language = ::get_e_language_from_xbox_language(XTLGetLanguage());
                    g_current_language = (e_language)language;
                }
                string_verify_task_set_strings(&m_verify_task, language, strings_to_verify, 3);
                overlapped_task_start_internal(&m_verify_task, "..\\interface\\c_player_profile.cpp", 229);
            }
        }
    }
}

// 0x8a590
void c_player_profile_interface::update(e_controller_index controller_index)
{
    c_controller_interface *profile_controller = &g_controller_globals.controllers[this->controller_index];
    c_controller_interface *controller = (c_controller_interface *)profile_controller;
    if (controller->is_signed_in_to_machine() && !controller->is_guest()) {
        unsigned int profile_flags = flags;
        bool bit8 = (bool)(_rotl(profile_flags, 24) & 1);
        if (!bit8) {
            profile_read_retry_count = 0;
            profile_read_retry_time = 0;
            if ((int)(profile_flags & 0x40) != 0) {
                ::read_player_profile_interface_from_xenon_profile(controller_index, this);
            }
            else {
                update_service_tag();
                if ((int)(flags & 0x20) != 0)
                    ::write_player_profile_interface_to_xenon_profile(controller_index, this);
            }
        }
    }
    bool in_use = controller->is_signed_in_to_machine() || profile_controller->is_unsigned_in_user();
    if (in_use && (int)(flags & 0x80) != 0)
        propagate_settings();
}


extern "C" {
// @address 0x58ae4 literal_110_0f
const float literal_110_0f = 110.0f;
// @address 0x58ae0 literal_140_0f
const float literal_140_0f = 140.0f;
// @address 0x58adc literal_220_0f
const float literal_220_0f = 220.0f;
// @address 0x58ad8 literal_240_0f
const float literal_240_0f = 240.0f;
// @address 0x58ad4 literal_260_0f
const float literal_260_0f = 260.0f;
}

static __forceinline void c_player_profile_set_controller_preferences(e_controller_index controller_index, s_controller_look_settings *settings)
{
    for (long action_index = 0; action_index < 28; action_index++) {
    }
    for (long button_index = 0; button_index < 16; button_index++)
        input_abstraction_globals.input_states[controller_index].buttons[button_index].unlock();
    memcpy(&input_abstraction_globals.look_settings[controller_index], settings, sizeof(*settings));
}

void c_player_profile_interface::propagate_settings()
{
    e_controller_index controller = (e_controller_index)controller_index;
    float vertical_sensitivity[10] = {40.0f, 50.0f, 60.0f, 70.0f, 80.0f, 90.0f, 100.0f, literal_110_0f, 120.0f, 130.0f};
    float horizontal_sensitivity[10] = {80.0f, 100.0f, 120.0f, literal_140_0f, 160.0f, 180.0f, 200.0f, literal_220_0f, literal_240_0f, literal_260_0f};
    float setting2c_values[4] = {0.3f, 0.8f, 1.5f, 2.0f};
    float setting30_values[3] = {0.5f, 1.0f, 1.5f};
    s_controller_look_settings settings;
    input_abstraction_get_default_preferences(&settings.preferences);
    settings.preferences.vertical_look_sensitivity = vertical_sensitivity[controls.unknown_setting_1c];
    settings.preferences.horizontal_look_sensitivity = horizontal_sensitivity[controls.unknown_setting_1c];
    settings.camera_control_scheme = (short)camera.unknown_setting_24;
    settings.preferences.joystick_preset = (short)controls.joystick_preset;
    settings.camera_speed = setting2c_values[camera.unknown_setting_2c];
    settings.camera_boost_scale = setting30_values[camera.unknown_setting_30];
    switch (controls.button_preset) {
    case button_preset_south_paw:
        settings.preferences.buttons[7] = 1;
        settings.preferences.buttons[8] = 0;
        settings.preferences.buttons[9] = 1;
        break;
    case button_preset_boxer:
        settings.preferences.buttons[5] = 0;
        settings.preferences.buttons[7] = 11;
        break;
    case button_preset_green_thumb:
        settings.preferences.buttons[5] = 9;
        settings.preferences.buttons[14] = 11;
        break;
    case button_preset_professional:
        settings.preferences.buttons[5] = 15;
        settings.preferences.buttons[3] = 11;
        settings.preferences.buttons[0] = 14;
        settings.preferences.buttons[1] = 10;
        settings.preferences.buttons[10] = 15;
        settings.preferences.buttons[4] = 10;
        break;
    case button_preset_walkie_talkie:
        settings.preferences.buttons[1] = 12;
        settings.preferences.buttons[4] = 12;
        settings.preferences.buttons[3] = 11;
        settings.preferences.buttons[5] = 15;
        settings.preferences.buttons[10] = 15;
        settings.preferences.buttons[26] = 14;
        settings.preferences.buttons[17] = 14;
        settings.preferences.buttons[6] = 2;
        break;
    }
    settings.preferences.look_inverted = (flags & 1) != 0;
    settings.preferences.flight_stick = ((flags >> 1) & 1) != 0;
    c_player_profile_set_controller_preferences(controller, &settings);
    user_interface_controller_update_network_properties(controller);
    flags &= ~0x80u;
}
