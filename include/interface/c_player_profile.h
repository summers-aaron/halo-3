#ifndef INTERFACE_C_PLAYER_PROFILE_H
#define INTERFACE_C_PLAYER_PROFILE_H

#include "interface/look_sensitivity_types.h"

#include "interface/voice_mask_types.h"

#include "interface/player_model_choice.h"

#include "cseries/language_types.h"


#include "shell/shell.h"
#include "game/game_types.h"
#include "game/player_types.h"
#include "cseries/async_xoverlapped_types.h"
#include <stddef.h>

#include "interface/player_profile_types.h"
enum e_player_color_index;
enum e_voice_output_setting;
enum e_campaign_game_mode;
struct game_state_header;

extern "C" void campaign_metagame_set_profile_progress_bit(long object_index, short skull);
extern "C" void campaign_metagame_set_profile_bit_and_grant_achievement(long object_index, short skull);

// Thirty-two bytes, persisted in the player profile.
struct s_player_training_profile_data
{
    unsigned long completion_count_low_bits[2];
    unsigned long completion_count_high_bits[2];
    // The profile merge ORs these bits without counting them as completion changes.
    unsigned long merge_only_bits[4];
};

class c_string_verify_task : public c_overlapped_task
{
public:
    c_string_verify_task();
    c_string_verify_task(const char *file, long line);
    __forceinline virtual ~c_string_verify_task() {}
    virtual const char *get_context_string() const;
    virtual unsigned long start(void *overlapped);
    virtual void success(unsigned long return_result);
    virtual void reset();

    void initialize(e_language language, const wchar_t **strings, long count);

    long get_strings_to_verify_count() const { return m_result_count; }
    const wchar_t *get_string_to_verify(long index) const { return m_strings[index]; }
    bool string_verify_succeeded(long index) const { return m_string_verified[index]; }

private:
    void initialize_strings();

public:
    long m_language;
    wchar_t m_strings[4][256];
    long m_result_count;
    bool m_string_verified[4];
    s_player_profile_string_data m_string_data[4];
    char m_verify_results[22];
};

typedef char string_verify_task_size[(sizeof(void *) != 4 || ((sizeof(c_string_verify_task) == 0x844) )) ? 1 : -1];
typedef char string_verify_task_string_data_offset[(sizeof(void *) != 4 || ((offsetof(c_string_verify_task, m_string_data) == 0x814) )) ? 1 : -1];

// The profile keeps its settings in plain records: the constructor
// value-initializes each one and initialize_to_default_settings clears each
// with a single memset.
struct s_player_profile_campaign
{
    e_look_sensitivity look_sensitivity;
    short last_campaign_played_absolute_index;
    short last_campaign_map_played_absolute_index;
    unsigned char map_completed_flags[2][32];
    unsigned __int64 last_campaign_played_time[2];
    long primary_skulls_found;
    long secondary_skulls_found;
    short unknown_setting_98;
    short unknown_setting_9a;
    unsigned long campaign_map_unlocked[1];
    unsigned long campaign_map_completion[4];
};

struct s_player_profile_last_played
{
    short last_network_game_hopper_played;
    long last_custom_multiplayer_map_played;
    long last_variant_game_engine;
    long last_variant_played;
    e_campaign_difficulty_level campaign_difficulty;
    int unknown_setting_c4;
};

struct s_player_profile_customization
{
    e_player_color_index primary_change_color;
    e_player_color_index secondary_change_color;
    e_player_color_index tertiary_change_color;
    unsigned int player_model_choice;
    s_emblem_info emblem_info;
    char model_customization_selections[2][4];
    wchar_t service_tag[4];
    wchar_t desired_service_tag[4];
    bool service_tag_was_randomly_generated;
    bool service_tag_failed_verification;
};

struct s_player_profile_voice_settings
{
    e_voice_output_setting voice_output_setting;
    e_voice_mask voice_mask;
    int unknown_setting_104;
    int unknown_setting_108;
    // Cached XPROFILE_OPTION_VOICE_THRU_SPEAKERS / XPROFILE_OPTION_VOICE_MUTED.
    long xbox_voice_output_setting;
    bool xbox_voice_muted;
};

// The controller presets. The profile's copy assignment moves them as one record.
struct s_player_profile_controls
{
    s_player_profile_controls() :
        button_preset(button_preset_default),
        joystick_preset(joystick_preset_default),
        unknown_setting_1c(0)
    {
    }

    e_button_presets button_preset;
    e_joystick_presets joystick_preset;
    int unknown_setting_1c;
};

// The start menu camera settings: horizontal inversion, panning, look sensitivity, speed and
// thrust. initialize_to_default_settings clears them with one memset.
struct s_player_profile_camera_settings
{
    int unknown_setting_20;
    int unknown_setting_24;
    int unknown_setting_28;
    int unknown_setting_2c;
    int unknown_setting_30;
};

class c_player_profile_interface { public:
    c_player_profile_interface();

    unsigned int flags;
    long profile_read_retry_count;
    long profile_read_retry_time;
    int controller_index;
    unsigned char unknown_byte_10;
    s_player_profile_controls controls;
    s_player_profile_camera_settings camera;
    s_player_profile_campaign campaign;
    s_player_profile_last_played last_played;
    s_player_profile_customization customization;
    s_player_profile_voice_settings voice;
    bool unknown_bool_114;
    bool unknown_bool_115;
    bool unknown_bool_116;
    long gamer_region;
    long gamer_zone;
    s_player_training_profile_data training_data;
    c_string_verify_task m_verify_task;

    void set_controller_look_inverted(bool enabled, bool dirty);
    void set_controller_auto_center_look(bool enabled, bool dirty);
    void set_controller_flight_inverted(bool enabled, bool dirty);
    void set_controller_crouch_lock_enabled(bool enabled, bool dirty);
    void set_button_preset(e_button_presets value, bool dirty);
    void set_joystick_preset(e_joystick_presets value, bool dirty);
    void set_look_sensitivity(e_look_sensitivity value, bool dirty);
    void set_campaign_difficulty(e_campaign_difficulty_level value, bool dirty);
    void set_primary_change_color(e_player_color_index value, bool dirty);
    void set_secondary_change_color(e_player_color_index value, bool dirty);
    void set_tertiary_change_color(e_player_color_index value, bool dirty);
    void set_service_tag_failed_verification(bool failed, bool dirty);
    void set_training_data(const s_player_training_profile_data *data, bool dirty);
    __forceinline void dirty_or_from_gamercard(bool force) { dirty_or(force); }
    void propagate_settings();
    void update(e_controller_index controller_index);
    long get_total_achievement_value() const;
    bool get_hidden_unlockable_unlocked(long unlockable) const;
    // match: 0x47e070: existing inline twin of the placed out-of-line getter.
    __forceinline bool get_unknown_level_flag_40_inline(long table, long level, e_campaign_difficulty_level difficulty) const
    {
        bool result = false;
        if ((level == -1 || level >= 0 && level < sizeof(campaign.map_completed_flags[table])) && difficulty >= 0 && difficulty < 4)
            result = level != -1 ? (campaign.map_completed_flags[table][level] & (1 << difficulty)) != 0 : false;
        return result;
    }
    __forceinline long get_skulls_found_count() const
    {
        long count = 0;
        long primary = campaign.primary_skulls_found;
        long secondary = campaign.secondary_skulls_found;
        for (long skull = 0; skull < 32; skull++) {
            if (primary & (1 << skull))
                count++;
            if (secondary & (1 << skull))
                count++;
        }
        return count;
    }

    bool get_controller_look_inverted() const { return (flags & 1) != 0; }
    bool get_controller_flight_inverted() const { return (flags & 4) != 0; }
    bool get_controller_auto_center_look() const { return (flags & 8) != 0; }
    bool get_controller_crouch_lock_enabled() const { return (flags & 16) != 0; }
    // match: 0x211ea8: player_control inlines this; a direct controls read swaps the operands of its fmuls at +0x5d8.
    e_button_presets get_button_preset() const
    {
        const s_player_profile_controls *profile_controls = &controls;
        return profile_controls->button_preset;
    }
    e_joystick_presets get_joystick_preset() const { return controls.joystick_preset; }
    e_look_sensitivity get_look_sensitivity() const { return campaign.look_sensitivity; }
    e_campaign_difficulty_level get_campaign_difficulty() const { return last_played.campaign_difficulty; }
    e_player_color_index get_primary_change_color() const { return customization.primary_change_color; }
    e_player_color_index get_secondary_change_color() const { return customization.secondary_change_color; }
    e_player_color_index get_tertiary_change_color() const { return customization.tertiary_change_color; }
    bool get_service_tag_failed_verification() const { return customization.service_tag_failed_verification; }
    bool get_controller_vibration_enabled() const { return (flags & 2) == 0; }
    e_controller_index get_controller_index() const { return (e_controller_index)controller_index; }
    short get_last_campaign_played_absolute_index() const { return campaign.last_campaign_played_absolute_index; }
    short get_last_campaign_map_played_absolute_index() const { return campaign.last_campaign_map_played_absolute_index; }
    unsigned short get_last_network_game_hopper_played() const { return last_played.last_network_game_hopper_played; }
    void set_last_campaign_played_time(e_campaign_game_mode mode, unsigned __int64 time, bool dirty)
    {
        if (mode >= 0 && mode < sizeof(campaign.last_campaign_played_time) / sizeof(campaign.last_campaign_played_time[0]))
        {
            dirty_or(dirty && campaign.last_campaign_played_time[mode] != get_last_campaign_played_time(mode));
            campaign.last_campaign_played_time[mode] = time;
        }
    }
    unsigned __int64 get_last_campaign_played_time(e_campaign_game_mode mode) const { return campaign.last_campaign_played_time[mode]; }
    __forceinline bool get_unknown_flag_9c(short index) const
    {
        bool enabled = false;
        if (index >= 0 && index < 32)
            enabled = ((1 << (index & 31)) & campaign.campaign_map_unlocked[index >> 5]) != 0;
        return enabled;
    }
    // match: 0x4a5ce0: inline twin of the placed out-of-line getter (difficulty unchecked).
    __forceinline bool get_map_completed_at_difficulty_level_inline(short map, long difficulty) const
    {
        bool completed = false;
        if (map >= 0 && map < 32)
        {
            long bit = map * 4 + difficulty;
            completed = (campaign.campaign_map_completion[bit >> 5] & (1 << (bit & 31))) != 0;
        }
        return completed;
    }
    __forceinline void get_last_variant_played(long *game_engine, long *variant_index) const
    {
        if (game_engine && variant_index)
        {
            *game_engine = last_played.last_variant_game_engine;
            *variant_index = last_played.last_variant_played;
        }
    }
    __forceinline long get_player_model_choice() const
    {
        long choice = customization.player_model_choice;
        if ((unsigned long)choice >= 2)
            choice = 0;
        return choice;
    }
    // match: 0x4a5ce0: inline twin of the placed out-of-line getter (no tag lookup).
    __forceinline long get_model_customization_selection_inline(long model, long part) const
    {
        if (model >= 0 && (unsigned long)model < 2 && part >= 0 && (unsigned long)part < 4)
            return customization.model_customization_selections[model][part];
        else
            return 0;
    }
    const wchar_t *get_desired_service_tag() const { return customization.desired_service_tag; }
    const wchar_t *get_service_tag_validated() const { return customization.service_tag; }
    short get_unknown_setting_98() const { return campaign.unknown_setting_98; }
    short get_unknown_setting_9a() const { return campaign.unknown_setting_9a; }
    bool get_async_io_busy() const { return (flags & 0x100) != 0; }
    bool get_unsafe_to_save() const { return (flags & 0x400) != 0; }
    bool get_title_specific_settings_previously_retrieved() const { return (flags & 0x200) != 0; }
    void mark_for_update_from_storage() { flags |= 0x40; }
    __forceinline void synchronization_with_storage_complete()
    {
        flags &= ~0x20ul;
        flags &= ~0x40ul;
        flags &= ~0x100ul;
    }
    void set_async_io_busy(bool busy) { if (busy) flags |= 0x100; else flags &= ~0x100; }
    void set_unsafe_to_save(bool unsafe) { if (unsafe) flags |= 0x400; else flags &= ~0x400; }
    void set_title_specific_settings_previously_retrieved(bool retrieved) { if (retrieved) flags |= 0x200; else flags &= ~0x200; }
    void set_controller_vibration_enabled(bool enabled, bool dirty)
    {
        dirty_or(dirty && enabled != get_controller_vibration_enabled());
        if (enabled)
            flags &= ~2;
        else
            flags |= 2;
    }
    void set_last_campaign_played_absolute_index(short index, bool dirty)
    {
        dirty_or(dirty && index != campaign.last_campaign_played_absolute_index);
        campaign.last_campaign_played_absolute_index = index;
    }
    void set_last_network_game_hopper_played(short hopper, bool dirty)
    {
        dirty_or(dirty && hopper != last_played.last_network_game_hopper_played);
        last_played.last_network_game_hopper_played = hopper;
    }
    void set_last_variant_played(long game_engine, long variant, bool dirty)
    {
        if (game_engine >= 0 && game_engine < 11) {
            dirty_or(dirty && game_engine != last_played.last_variant_game_engine || variant != last_played.last_variant_played);
            last_played.last_variant_game_engine = game_engine;
            last_played.last_variant_played = variant;
        }
    }
    void set_last_custom_multiplayer_map_played(long map, bool dirty)
    {
        if (map == -1 || map >= 0) {
            dirty_or(dirty && map != last_played.last_custom_multiplayer_map_played);
            last_played.last_custom_multiplayer_map_played = map;
        }
    }
    void set_unknown_setting_108(int value, bool dirty)
    {
        dirty_or(dirty && value != voice.unknown_setting_108);
        voice.unknown_setting_108 = value;
    }
    // match: 0x47e5a8,0x4bf678: these shared setters must inline into their callers.
    __forceinline void set_unknown_bool_114(bool value, bool dirty)
    {
        dirty_or(dirty && value != unknown_bool_114);
        unknown_bool_114 = value;
    }
    // match: 0x47e5a8,0x4bf678: these shared setters must inline into their callers.
    __forceinline void set_unknown_bool_115(bool value, bool dirty)
    {
        dirty_or(dirty && value != unknown_bool_115);
        unknown_bool_115 = value;
    }
    // match: 0x47e5a8,0x4bf678: these shared setters must inline into their callers.
    __forceinline void set_unknown_bool_116(bool value, bool dirty)
    {
        dirty_or(dirty && value != unknown_bool_116);
        unknown_bool_116 = value;
    }
    void set_map_completed_at_difficulty_level(short map, short difficulty, bool completed, bool dirty);
    // match: 0x47e5a8: existing inline twin of a placed out-of-line setter.
    __forceinline void set_button_preset_inline(e_button_presets value, bool dirty)
    {
        if (value < 6) {
            dirty_or(dirty && value != controls.button_preset);
            controls.button_preset = value;
        }
    }
    // match: 0x47e5a8: existing inline twin of a placed out-of-line setter.
    __forceinline void set_joystick_preset_inline(e_joystick_presets value, bool dirty)
    {
        if (value < 4) {
            dirty_or(dirty && value != controls.joystick_preset);
            controls.joystick_preset = value;
        }
    }
    // match: 0x47e5a8: existing inline twin of a placed out-of-line setter.
    __forceinline void set_unknown_setting_1c_inline(int value, bool dirty)
    {
        if (value < 10) {
            dirty_or(dirty && value != controls.unknown_setting_1c);
            controls.unknown_setting_1c = value;
        }
    }
    // match: 0x47e5a8: existing inline twin of a placed out-of-line setter.
    __forceinline void set_unknown_setting_20_inline(int value, bool dirty)
    {
        if (value < 2) {
            dirty_or(dirty && value != camera.unknown_setting_20);
            camera.unknown_setting_20 = value;
        }
    }
    // match: 0x47e5a8: existing inline twin of a placed out-of-line setter.
    __forceinline void set_unknown_setting_24_inline(int value, bool dirty)
    {
        if (value < 2) {
            dirty_or(dirty && value != camera.unknown_setting_24);
            camera.unknown_setting_24 = value;
        }
    }
    // match: 0x47e5a8: existing inline twin of a placed out-of-line setter.
    __forceinline void set_unknown_setting_28_inline(int value, bool dirty)
    {
        if (value < 4) {
            dirty_or(dirty && value != camera.unknown_setting_28);
            camera.unknown_setting_28 = value;
        }
    }
    // match: 0x47e5a8: existing inline twin of a placed out-of-line setter.
    __forceinline void set_unknown_setting_2c_inline(int value, bool dirty)
    {
        if (value < 4) {
            dirty_or(dirty && value != camera.unknown_setting_2c);
            camera.unknown_setting_2c = value;
        }
    }
    // match: 0x47e5a8: existing inline twin of a placed out-of-line setter.
    __forceinline void set_unknown_setting_30_inline(int value, bool dirty)
    {
        if (value < 3) {
            dirty_or(dirty && value != camera.unknown_setting_30);
            camera.unknown_setting_30 = value;
        }
    }
    // match: 0x47e5a8: existing inline twin of a placed out-of-line setter.
    __forceinline void set_look_sensitivity_inline(e_look_sensitivity value, bool dirty)
    {
        if (value < 4) {
            dirty_or(dirty && value != campaign.look_sensitivity);
            campaign.look_sensitivity = value;
        }
    }
    // match: 0x47e5a8: existing inline twin of a placed out-of-line setter.
    __forceinline void set_primary_change_color_inline(e_player_color_index value, bool dirty)
    {
        if (value < 32) {
            dirty_or(dirty && value != customization.primary_change_color);
            customization.primary_change_color = value;
        }
    }
    // match: 0x47e5a8: existing inline twin of a placed out-of-line setter.
    __forceinline void set_secondary_change_color_inline(e_player_color_index value, bool dirty)
    {
        if (value < 32) {
            dirty_or(dirty && value != customization.secondary_change_color);
            customization.secondary_change_color = value;
        }
    }
    // match: 0x47e5a8: existing inline twin of a placed out-of-line setter.
    __forceinline void set_tertiary_change_color_inline(e_player_color_index value, bool dirty)
    {
        if (value < 32) {
            dirty_or(dirty && value != customization.tertiary_change_color);
            customization.tertiary_change_color = value;
        }
    }
    // match: 0x47e5a8: existing inline twin of a placed out-of-line setter.
    __forceinline void set_voice_output_setting_inline(e_voice_output_setting value, bool dirty)
    {
        if (value < 2) {
            dirty_or(dirty && value != voice.voice_output_setting);
            voice.voice_output_setting = value;
        }
    }
    // match: 0x47e5a8: existing inline twin of a placed out-of-line setter.
    __forceinline void set_voice_mask_inline(e_voice_mask value, bool dirty)
    {
        if (value < 2) {
            dirty_or(dirty && value != voice.voice_mask);
            voice.voice_mask = value;
        }
    }
    // match: 0x47e5a8: existing inline twin of a placed out-of-line setter.
    __forceinline void set_unknown_setting_104_inline(int value, bool dirty)
    {
        if (value < 2) {
            dirty_or(dirty && value != voice.unknown_setting_104);
            voice.unknown_setting_104 = value;
        }
    }
    // match: 0x47e5a8: existing inline twin of a placed out-of-line setter.
    __forceinline void set_campaign_difficulty_inline(e_campaign_difficulty_level value, bool dirty)
    {
        if (value < 5) {
            dirty_or(dirty && value != last_played.campaign_difficulty);
            last_played.campaign_difficulty = value;
        }
    }
    // match: 0x47e5a8: existing inline twin of a placed out-of-line setter.
    __forceinline void set_player_model_choice_inline(e_player_model_choice choice, bool dirty)
    {
        if (choice != spartan && choice != elite) {
            return;
        }
        unsigned int mode = customization.player_model_choice;
        if (2 <= mode) {
            mode = 0;
        }
        dirty_or(dirty && choice != mode);
        customization.player_model_choice = choice;
    }


    void set_unknown_setting_1c(int value, bool dirty);
    void set_unknown_setting_20(int value, bool dirty);
    void set_unknown_setting_24(int value, bool dirty);
    void set_unknown_setting_28(int value, bool dirty);
    void set_unknown_setting_2c(int value, bool dirty);
    void set_unknown_setting_30(int value, bool dirty);
    bool get_map_completed_at_difficulty_level(short map, short difficulty) const;
    bool get_unknown_level_flag_40(long table, long level, e_campaign_difficulty_level difficulty) const;
    void set_unknown_level_flag_40(long table, long level, e_campaign_difficulty_level difficulty, bool value, bool notify);
    void set_unknown_flag_9c(short index, bool enabled, bool dirty);
    void set_unknown_setting_98(short value, bool dirty);
    void set_unknown_setting_9a(short value, bool dirty);
    void set_voice_output_setting(e_voice_output_setting value, bool dirty);
    void set_voice_mask(e_voice_mask value, bool dirty);
    void set_unknown_setting_c4(int value, bool dirty);
    void set_unknown_setting_104(int value, bool dirty);
    void set_last_campaign_map_played_absolute_index(short index, bool notify);
    void set_player_model_choice(e_player_model_choice choice, bool dirty);
    void set_emblem_info(const s_emblem_info *info, bool dirty);
    s_emblem_info get_emblem_info() const { return customization.emblem_info; }
    void get_player_appearance(s_player_appearance *appearance);
    char get_model_customization_selection(e_player_model_choice model, long part) const;
    void set_model_customization_selection(e_player_model_choice model, long part, char choice, bool dirty);
    static bool is_service_tag_validly_formatted(const wchar_t *text);

    void set_desired_service_tag(const wchar_t *text, bool compare);
    void set_service_tag_validated(const wchar_t *text, bool compare);

private:
    friend struct c_controller_interface;
    friend void campaign_metagame_set_profile_progress_bit(long object_index, short skull);
    friend void campaign_metagame_set_profile_bit_and_grant_achievement(long object_index, short skull);
    friend void saved_game_files_write_saved_game_state(const game_state_header *header, long header_size, const void *buffer, long buffer_size);
    static void generate_random_service_tag(wchar_t *const text);
    void initialize_to_default_settings();
    void update_service_tag();
    void dirty_or(bool force);
};

typedef char player_profile_size_check[(sizeof(void *) != 4 || ((sizeof(c_player_profile_interface) == 0x988) )) ? 1 : -1];
typedef char player_profile_verify_task_offset_check[(offsetof(c_player_profile_interface, m_verify_task) == 0x140) ? 1 : -1];
typedef char player_profile_read_retry_count_offset[(offsetof(c_player_profile_interface, profile_read_retry_count) == 0x4) ? 1 : -1];
typedef char player_profile_read_retry_time_offset[(offsetof(c_player_profile_interface, profile_read_retry_time) == 0x8) ? 1 : -1];
typedef char player_profile_map_unlocked_offset[(offsetof(c_player_profile_interface, campaign.campaign_map_unlocked) == 0x9c) ? 1 : -1];
typedef char player_profile_custom_map_offset[(offsetof(c_player_profile_interface, last_played.last_custom_multiplayer_map_played) == 0xb4) ? 1 : -1];
typedef char player_profile_voice_output_offset[(offsetof(c_player_profile_interface, voice.voice_output_setting) == 0xfc) ? 1 : -1];
typedef char player_profile_gamer_region_offset[(offsetof(c_player_profile_interface, gamer_region) == 0x118) ? 1 : -1];

// Functions defined in tu/interfacelibxbox/c_player_profile.cpp that other units call.

extern "C" void player_profile_set_global_value(long value);

typedef char player_profile_xbox_voice_output_offset[(offsetof(c_player_profile_interface, voice.xbox_voice_output_setting) == 0x10c) ? 1 : -1];
typedef char player_profile_xbox_voice_muted_offset[(offsetof(c_player_profile_interface, voice.xbox_voice_muted) == 0x110) ? 1 : -1];
typedef char player_profile_following_settings_offset[(offsetof(c_player_profile_interface, unknown_bool_114) == 0x114) ? 1 : -1];

#endif // INTERFACE_C_PLAYER_PROFILE_H
