#ifndef GAME_PLAYER_TRAINING_TYPES_H
#define GAME_PLAYER_TRAINING_TYPES_H

#include "simulation/player_action.h"
#include "game/player_types.h"
#include "game/player_training_constants.h"
#include "interface/c_player_profile.h"

class c_hold_watcher
{
public:
    void check_action(long input_user_index, const player_action *action);
    unsigned short action_type() const { return m_action_type; }
    long object_index() const { return m_object_index; }
    unsigned short hold_ticks() const { return m_hold_ticks; }
    unsigned short secondary_hold_ticks() const { return m_secondary_hold_ticks; }
    unsigned short action_count() const { return m_action_count; }
private:
    unsigned short m_action_type;
    long m_object_index;
    unsigned short m_hold_ticks;
    unsigned short m_secondary_hold_ticks;
    unsigned short m_action_count;
};

struct s_recent_action                 // 8 bytes
{
    long time;
    unsigned short type;
    short result; // action_submit records the submission result in the final halfword.
};

class c_recent_actions
{
public:
    bool did_player_recently_perform_action(long action, long ticks);
    __forceinline s_recent_action *entries() { return m_actions; }
private:
    s_recent_action m_actions[4];
};

struct s_training_profile
{
    union {
        unsigned long bits[8];
        s_player_training_profile_data completion;
    };
};

struct s_training_entry               // 4 bytes
{
    unsigned short state;
    short ticks;
};

#pragma pack(push, 4)
// Per-training progress state: each check latches its starting value on the first update.
struct s_player_training_look_state     // 16 bytes
{
    unsigned char initialized;
    float yaw;
    float pitch;
    unsigned char left_seen;
    unsigned char right_seen;
};

struct s_player_training_move_state     // 16 bytes
{
    unsigned char initialized;
    real_point3d position;
};

struct s_player_training_zoom_state     // 8 bytes
{
    unsigned char initialized;
    long ticks;
};

struct s_player_training_entry_data     // 532 bytes (March 460)
{
    unsigned __int64 identifier;
    s_training_entry entries[k_player_training_count];
    long unit_index;
    long state_ticks;
    unsigned long active_bits[2];
    player_action current_action;
    player_action last_action;
    c_hold_watcher hold_watcher;
    c_recent_actions recent_actions;
    s_training_profile profile;
    unsigned char trigger_flags;
    unsigned char device_touched_ticks;
    unsigned char weapon_switch_ticks;
    unsigned char weapon_switch_count;
    unsigned char grenade_kill_ticks;
    unsigned char headshot_kill_ticks;
    unsigned char melee_kill_ticks;
    unsigned char weapon_kill_ticks;
    unsigned char grenade_pickup_ticks;
    unsigned char equipment_pickup_ticks;
    unsigned char : 8;
    unsigned char assassination_ticks;
    unsigned char loaded;
    s_player_training_look_state look;
    s_player_training_move_state move;
    s_player_training_zoom_state zoom;
};
#pragma pack(pop)



typedef char player_training_entry_size[(sizeof(s_player_training_entry_data) == 0x214) ? 1 : -1];
typedef char player_training_actions_offset[(offsetof(s_player_training_entry_data, recent_actions) == 0x19c) ? 1 : -1];

typedef char player_training_grenade_pickup_offset[(offsetof(s_player_training_entry_data, grenade_pickup_ticks) == 0x1e4) ? 1 : -1];
typedef char player_training_equipment_pickup_offset[(offsetof(s_player_training_entry_data, equipment_pickup_ticks) == 0x1e5) ? 1 : -1];
typedef char player_training_look_offset[(offsetof(s_player_training_entry_data, look) == 0x1ec) ? 1 : -1];
typedef char player_training_move_offset[(offsetof(s_player_training_entry_data, move) == 0x1fc) ? 1 : -1];
typedef char player_training_zoom_offset[(offsetof(s_player_training_entry_data, zoom) == 0x20c) ? 1 : -1];

#endif
