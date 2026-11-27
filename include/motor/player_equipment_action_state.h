#ifndef MOTOR_PLAYER_EQUIPMENT_ACTION_STATE_H
#define MOTOR_PLAYER_EQUIPMENT_ACTION_STATE_H
#include <stddef.h>
// This action storage is shared across use, update, and throw.
// The motor-owned prefix precedes the equipment state at +6.
struct s_player_equipment_action_state
{
    char motor_storage[6];
    char active;
    unsigned char interruptible;
    char released;
    union { unsigned char primary_key_ticks; char threshold; };
    unsigned char duration_ticks;
    char alignment;
    long source_unit_index;
    long definition_index;
};
typedef char equipment_action_active_offset[(offsetof(s_player_equipment_action_state, active) == 6) ? 1 : -1];
typedef char equipment_action_source_offset[(offsetof(s_player_equipment_action_state, source_unit_index) == 0xc) ? 1 : -1];
typedef char equipment_action_definition_offset[(offsetof(s_player_equipment_action_state, definition_index) == 0x10) ? 1 : -1];
#endif
