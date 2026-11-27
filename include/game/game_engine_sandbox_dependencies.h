#ifndef GAME_GAME_ENGINE_SANDBOX_DEPENDENCIES_H
#define GAME_GAME_ENGINE_SANDBOX_DEPENDENCIES_H

#include "math/real_math_types.h"
#include "game/map_variant_object_properties_types.h"

class c_map_variant;
struct game_engine_sandbox_editor_action {
  long action;
  long definition_index;
  long owner_index;
  long player_index;
  float value;
  unsigned char minimum_count;
  unsigned char maximum_count;
  real_point3d position;
  long subtype;
  s_variant_multiplayer_object_properties_definition properties;
};

typedef char sandbox_editor_action_size[(sizeof(game_engine_sandbox_editor_action) == 0x40) ? 1 : -1];
typedef char sandbox_editor_action_properties_offset[(offsetof(game_engine_sandbox_editor_action, properties) == 0x28) ? 1 : -1];
typedef char sandbox_editor_action_position_offset[(offsetof(game_engine_sandbox_editor_action, position) == 0x18) ? 1 : -1];
union real_vector3d;

extern "C" int game_engine_sandbox_update_variant_object_properties(c_map_variant *variant, long object_index);
#if !defined(_XBOX) || defined(__clang__)
extern "C" void object_set_movable(long object_index, unsigned char enable);
#else
extern "C" int object_set_movable(...);
#endif
extern "C" void simulation_queue_editor_action_insert(game_engine_sandbox_editor_action *action);
extern "C" void object_update_soft_ceiling(long object_index, const real_vector3d *position);

// Window manager the sandbox engine reads.
class c_window_manager;
extern "C" c_window_manager g_window_manager;

#endif // GAME_GAME_ENGINE_SANDBOX_DEPENDENCIES_H
