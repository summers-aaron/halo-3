#ifndef INTERFACE_CHUD_CHUD_USER_DATA_TYPES_H
#define INTERFACE_CHUD_CHUD_USER_DATA_TYPES_H

#include "interface/chud/chud.h"
#include "interface/chud/chud_manager.h"
#include "interface/chud/chud_navpoint_manager.h"
#include "interface/chud/chud_draw_types.h"
#include "interface/chud/chud_impulse_manager.h"
#include "interface/chud/chud_messaging_manager.h"
#include "interface/chud/chud_directional_damage.h"
#include "interface/chud/chud_motion_sensor.h"
#include "objects/emblem_info.h"
#include <stddef.h>

struct s_chud_color_labels { unsigned short labels[4]; };

struct s_chud_persistent_display_state
{
    bool display_active;

    wchar_t player_training_text[256];

    long flash_ticks;
    wchar_t cutscene_title[256];
    float weapon_state_value;
    float highlight_value;
    wchar_t text_410[20];
    wchar_t text_438[32];
    wchar_t text_478[32];
    wchar_t text_4b8[32];
    long count_text_width;
    long icon_4fc;
    long icon_500;
    long icon_504;
    float visible_opacity;
    bool visible_initialized;

    union { float perspective_opacity; float curvature_amount; };
    bool perspective_changed;
    union { bool weapon_zoomed; bool point_tracker_enabled; };

    real_point3d point_tracker_position;
    c_chud_point_tracker point_tracker;
    c_chud_cortana_effect cortana_effect;
    long widget_list_heads[2];                  /* [0] every anchor but 7, [1] anchor 7 */
    long color_group;
    real_argb_color colors[4];
    union
    {
        emblem_info emblems[4];
        s_chud_color_labels color_labels[4];
    };
    float equipment_opacity;
    union
    {
        struct
        {
            float scores[12];
            float external_input_718;
            float external_input_71c;
            float external_input_720;
            float external_input_724;
        };
        real_vector4d curved_hud_constants[4];
    };
    long resolution;
};
typedef char chud_persistent_display_state_size[(sizeof(s_chud_persistent_display_state) == 0x72c) ? 1 : -1];


struct chud_persistent_weapon_entry { long definition_index; long count; long object_index; };

class c_chud_persistent_user_data
{
    friend void ::chud_persistent_user_data_initialize(c_chud_persistent_user_data *data, long user_index);
public:
    void reset();
    __forceinline void initialize_sounds(long user_index);
    void update_widget_external_inputs(long user_index, s_chud_runtime_widget_datum *widget, c_chud_update_user_data *update_data);
    void update(long user_index, float dt);
    void update_from_update_data(long user_index, c_chud_update_user_data *update_data);
private:
    __forceinline long get_active_chud_definition_index(long definition_index, long weapon_index);
    __forceinline long add_active_chud_definition_index(long definition_index, long weapon_index, bool *reset_required);
    __forceinline void active_chud_definition_notify_widget_created(long active_definition_index);
    __forceinline void active_chud_definition_notify_widget_deleted(long active_definition_index);
    __forceinline void update_sounds(long user_index, c_chud_update_user_data *update_data);

public:
    chud_persistent_weapon_entry m_weapons[8];
    long m_updates_since_spawn;
    long m_user_index;
    long m_unit_index;
    bool m_flags[3];

    unsigned __int64 m_player_hash_bits;
    c_chud_directional_damage m_directional_damage;
    c_chud_cinematic_fade m_cinematic_fade;
    c_chud_motion_sensor m_motion_sensor;
    s_chud_persistent_display_state m_display_state;
    c_chud_navpoint_manager m_navpoint_manager;
    c_chud_impulse_manager m_impulse_manager;
    c_chud_messaging_manager m_messaging_manager;
    float m_last_shield_vitality;
    long m_shield_recharge_ticks;
    long m_shield_update_time;
    float m_target_distance;
    float m_target_height;
    long m_tail_indices[8];
    unsigned short m_tail_flags;
};

typedef char chud_persistent_data_size[(sizeof(c_chud_persistent_user_data) == 0x2880) ? 1 : -1];
typedef char chud_display_state_flash_ticks_offset[
    (offsetof(s_chud_persistent_display_state, flash_ticks) == 0x204) ? 1 : -1];
typedef char chud_display_state_point_tracker_position_offset[
    (offsetof(s_chud_persistent_display_state, point_tracker_position) == 0x518) ? 1 : -1];
typedef char chud_display_state_cortana_effect_offset[
    (offsetof(s_chud_persistent_display_state, cortana_effect) == 0x550) ? 1 : -1];
typedef char chud_display_state_widget_lists_offset[
    (offsetof(s_chud_persistent_display_state, widget_list_heads) == 0x678) ? 1 : -1];
typedef char chud_display_state_emblems_offset[
    (offsetof(s_chud_persistent_display_state, emblems) == 0x6c4) ? 1 : -1];
typedef char chud_persistent_flags_offset[
    (offsetof(c_chud_persistent_user_data, m_flags) == 0x6c) ? 1 : -1];
typedef char chud_persistent_updates_since_spawn_offset[
    (offsetof(c_chud_persistent_user_data, m_updates_since_spawn) == 0x60) ? 1 : -1];
typedef char chud_persistent_player_hash_offset[
    (offsetof(c_chud_persistent_user_data, m_player_hash_bits) == 0x70) ? 1 : -1];
typedef char chud_persistent_directional_damage_offset[
    (offsetof(c_chud_persistent_user_data, m_directional_damage) == 0x78) ? 1 : -1];
typedef char chud_persistent_motion_sensor_offset[
    (offsetof(c_chud_persistent_user_data, m_motion_sensor) == 0x244) ? 1 : -1];
typedef char chud_persistent_display_state_offset[
    (offsetof(c_chud_persistent_user_data, m_display_state) == 0xf08) ? 1 : -1];
typedef char chud_persistent_navpoint_offset[
    (offsetof(c_chud_persistent_user_data, m_navpoint_manager) == 0x1634) ? 1 : -1];
typedef char chud_persistent_impulse_offset[
    (offsetof(c_chud_persistent_user_data, m_impulse_manager) == 0x1f84) ? 1 : -1];
typedef char chud_persistent_messaging_offset[
    (offsetof(c_chud_persistent_user_data, m_messaging_manager) == 0x1fb0) ? 1 : -1];
typedef char chud_persistent_shield_vitality_offset[
    (offsetof(c_chud_persistent_user_data, m_last_shield_vitality) == 0x2848) ? 1 : -1];

static __forceinline c_chud_persistent_user_data *chud_persistent_user_data()
{
    return c_chud_manager::x_persistent_user_data;
}

#endif
