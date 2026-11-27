#ifndef GAME_GAME_ENGINE_SANDBOX_STATE_TYPES_H
#define GAME_GAME_ENGINE_SANDBOX_STATE_TYPES_H

#include <stddef.h>

// Shared by the sandbox engine and its simulation codec (136-byte wire state).
struct game_engine_sandbox_update_data
{
    char globals[52];
    union { short round_time; unsigned short total_budget; };
    union { unsigned short active_player_flags; unsigned short player_roles; };
    union { long owners[16]; long edited_object_ids[16]; };
    union { long edited_object_index; long remove_object_map_variant_index; };
    long object_definition_index;
    long variant_object_count;
    union { long game_engine_subtype; long game_type; };
};

typedef char sandbox_state_size[(sizeof(game_engine_sandbox_update_data) == 0x88) ? 1 : -1];
typedef char sandbox_state_budget_offset[(offsetof(game_engine_sandbox_update_data, total_budget) == 0x34) ? 1 : -1];
typedef char sandbox_state_owners_offset[(offsetof(game_engine_sandbox_update_data, owners) == 0x38) ? 1 : -1];
typedef char sandbox_state_definition_offset[(offsetof(game_engine_sandbox_update_data, object_definition_index) == 0x7c) ? 1 : -1];
typedef char sandbox_state_count_offset[(offsetof(game_engine_sandbox_update_data, variant_object_count) == 0x80) ? 1 : -1];

#endif
