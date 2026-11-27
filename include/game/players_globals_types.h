#ifndef GAME_PLAYERS_GLOBALS_TYPES_H
#define GAME_PLAYERS_GLOBALS_TYPES_H

#include <stddef.h>
#include "game/player_configuration.h"
#include "math/real_math_types.h"

enum e_player_respawn_failure
{
    _player_respawn_failure_none = 0,
    _player_respawn_failure_in_editor,
    _player_respawn_failure_cinematic_playing,
    _player_respawn_failure_scripting_says_it_isnt_safe,
    _player_respawn_failure_waiting_on_respawn_timer,
    _player_respawn_failure_friend_in_combat,
    _player_respawn_failure_nearby_enemies_pursuing_friend,
    _player_respawn_failure_friend_airborne_or_moving,
    _player_respawn_failure_friend_in_vehicle,
    _player_respawn_failure_everybody_is_dead,
    _player_respawn_failure_waiting_for_checkpoint,
    _player_respawn_failure_can_not_find_valid_location,
};

struct s_players_globals {
    long count;
    bool inhibited;
    bool mostly_inhibited;
    bool input_flag;
    unsigned char : 8;
    unsigned long machine_valid_mask;
    s_machine_identifier machines[16];
    bool network_valid;
    s_machine_identifier machine;
    unsigned char : 8;
    long local_machine_index;
    bool respawn_unsafe;
    unsigned char : 8;
    unsigned short respawn_failure;
    bool pending;
    union { real_vector3d camera_position; real_point3d position; };
    union { real_vector3d camera_forward; real_vector3d forward; };
    union { signed char camera_user_index; signed char old_zone; };
    union { signed char camera_controller_index; signed char new_zone; };
    short switch_count;
    union { long camera_unit_index; long player_index; };
    union { long camera_object_index; long unit_index; long zone_set_index; };
    union { char transient_state[44]; unsigned long disabled[11]; };
    long camera_target_index;
    union { unsigned short transient_count; short terminals_read; };
    short terminals_accessed;
    bool suppressed;
    unsigned char : 8;
    unsigned char : 8;
    unsigned char : 8;
    unsigned long : 32;
    unsigned long : 32;
    unsigned long : 32;
    unsigned long : 32;
    unsigned long : 32;
    unsigned long : 32;
    unsigned long : 32;
    unsigned long : 32;
    unsigned long : 32;
    unsigned long : 32;
    unsigned long : 32;
    unsigned long : 32;
    unsigned long : 32;
    unsigned long : 32;
    unsigned long : 32;
    unsigned long : 32;
};
typedef char players_globals_size[(sizeof(s_players_globals) == 0x11c) ? 1 : -1];
typedef char players_globals_switch_position_offset[(offsetof(s_players_globals, position) == 0x80) ? 1 : -1];
typedef char players_globals_suppressed_offset[(offsetof(s_players_globals, suppressed) == 0xd8) ? 1 : -1];

typedef char players_globals_terminals_accessed_offset[(offsetof(s_players_globals, terminals_accessed) == 0xd6) ? 1 : -1];

#endif
