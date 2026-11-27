#ifndef NETWORKING_ONLINE_VOICE_OBSERVER_TYPES_H
#define NETWORKING_ONLINE_VOICE_OBSERVER_TYPES_H

#include <stddef.h>
#include "memory/static_arrays.h"
#include "math/real_math_types.h"
#include "shell/shell_types.h"
#include "networking/online/voice_destination_types.h"

struct s_voice_speaker_volume;
struct s_remote_player_voice_destination;

class c_voice_observer;
extern "C" unsigned long voice_observer_get_team_chat_mask(void *observer, long player_index);
extern "C" void voice_observer_update_local_player_in_game_masks(c_voice_observer *observer);

class c_voice_observer
{
    friend unsigned long voice_observer_get_team_chat_mask(void *observer, long player_index);
    friend void voice_observer_update_local_player_in_game_masks(c_voice_observer *observer);
public:
    __forceinline unsigned long get_player_incoming_voice_to_speakers_mask(long player_index)
    {
        unsigned long mask = 0;
        if (m_initialized)
            mask = m_player_incoming_voice_to_speakers_mask[player_index];
        return mask;
    }
    __forceinline unsigned long get_player_incoming_voice_to_headset_mask(long player_index)
    {
        unsigned long mask = 0;
        if (m_initialized)
            mask = m_player_incoming_voice_to_headset_mask[player_index];
        return mask;
    }
    void initialize();
    void update();
    unsigned long get_player_in_game_mask(long player_index);
    unsigned long get_player_proximity_mask(long player_index)
    {
        unsigned long mask = 0;
        if (m_initialized)
            mask = m_player_proximity_mask[player_index];
        return mask;
    }
    unsigned long get_player_push_to_talk_mask(long player_index)
    {
        unsigned long mask = 0;
        if (m_initialized)
            mask = m_player_push_to_talk_mask[player_index];
        return mask;
    }
    unsigned long get_player_in_game_mask_internal(long player_index)
    {
        unsigned long mask = 0;
        if (m_initialized)
            mask = m_player_in_game_mask[player_index];
        return mask;
    }
    unsigned long get_player_voice_state_mask(long player_index) { unsigned long mask = 0; if (m_initialized) mask = m_voice_state[player_index]; return mask; }
    unsigned long get_player_team_mask(long player_index) { unsigned long mask = 0; if (m_initialized) mask = m_player_team_mask[player_index]; return mask; }
    bool player_is_voice_repeater(long player_index)
    {
        bool repeater = false;
        if (m_initialized)
            repeater = ((1 << player_index) & m_voice_repeater_mask) != 0;
        return repeater;
    }
    unsigned long get_player_out_of_game_mask(long player_index);
    void set_local_listeners_to_incoming_remote_player_voice(long remote_player_index, unsigned long voice_flags);
private:
    void set_voice_destination_headset(unsigned long player_mask, long local_user_index);
    float get_proximity_voice_distance(long local_player_index, long remote_player_index);
    e_voice_destination get_voice_actual_destination(long local_player_index, long remote_player_index, e_voice_destination desired_destination);
    __forceinline e_voice_destination get_voice_desired_destination_in_game(long local_player_index, long remote_player_index, unsigned long voice_flags);
    void update_player_team_masks();
    bool players_can_team_chat(long local_player_index, long remote_player_index);
    static bool player_is_alive_or_wounded(long player_index);
    static bool player_ever_alive(long player_index);
    static bool player_get_unit_body_position(long player_index, real_point3d *position, bool use_alternate_unit);
    void update_local_player_proximity_masks();
    void update_local_player_death_gurgles();
    void update_local_player_dead_masks();
    void update_voice_repeater_mask();
    void build_speaker_volume_for_proximity_voice(long local_player_index, const real_point3d *remote_position, s_voice_speaker_volume *speaker_volume_out);
    void set_voice_destination(long remote_player_index, s_remote_player_voice_destination *destinations, long destination_count);
    void update_local_players_out_of_game_incoming_voice_status();
    void update_local_player_push_to_talk_masks();
    void update_local_player_push_to_talk_timers(long player_index, e_controller_index controller_index);
public:
    bool m_initialized;
    c_static_array<unsigned long, 16> m_player_proximity_mask;
    c_static_array<unsigned long, 16> m_player_push_to_talk_mask;
    c_static_array<unsigned long, 16> m_player_in_game_mask;
    c_static_array<unsigned long, 16> m_voice_state;
    c_static_array<unsigned long, 16> m_player_team_mask;
    c_static_array<unsigned long, 16> m_player_incoming_voice_to_speakers_mask;
    c_static_array<unsigned long, 16> m_player_incoming_voice_to_headset_mask;
    c_static_array<unsigned long, 16> m_player_death_gurgle_mask;
    c_static_array<unsigned long, 16> m_player_push_to_talk_start_time;
    c_static_array<unsigned long, 16> m_player_push_to_talk_last_time;
    c_static_array<long, 16> m_player_push_to_talk_sound_index;
    c_static_array<long, 16> m_player_mute_mask;
    c_static_array<long, 16> m_player_voice_flags;
    long m_incoming_voice_mask;
    long m_voice_repeater_mask;
};

#if defined(_M_PPCBE) || defined(_XBOX)
typedef char voice_observer_size[(sizeof(c_voice_observer) == 0x34c) ? 1 : -1];
typedef char voice_observer_team_mask_offset[(offsetof(c_voice_observer, m_player_team_mask) == 0x104) ? 1 : -1];
typedef char voice_observer_repeater_mask_offset[(offsetof(c_voice_observer, m_voice_repeater_mask) == 0x348) ? 1 : -1];
#endif

#endif
