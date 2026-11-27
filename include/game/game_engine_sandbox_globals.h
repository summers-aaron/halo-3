#ifndef GAME_GAME_ENGINE_SANDBOX_GLOBALS_H
#define GAME_GAME_ENGINE_SANDBOX_GLOBALS_H

#include <stddef.h>
#include "math/real_math_types.h"
#include "memory/static_arrays.h"
#include "game/map_variant_object_properties_types.h"

struct s_core_quota {
  long object_definition_index;
  unsigned char maximum_count;
  unsigned char minimum_count;
  unsigned char placed_on_map;
};

struct s_sandbox_globals {
  s_core_quota quota;
  short active_player_flags;
  long owners[16];
  float edit_times[16];
  c_static_array<real_point3d, 16> edit_position;
  c_static_array<real_vector3d, 16> probe_normals;
  c_static_array<real_vector3d, 16> player_axes;
  long handle_slots[16];
  bool object_rotated[16];             // held object turned since its last position snapshot
  long object_indices[64];
  real_vector3d positions[64];
  long object_count;
  long edited_map_variant_index;
  long spawn_definition_index;
  bool variant_file_exists;
  long boundary_object_index;          // edited object whose influence boundary is drawn
  s_variant_multiplayer_object_properties_definition edited_object_properties;
};

typedef char sandbox_core_quota_size[(sizeof(s_core_quota) == 8) ? 1 : -1];
typedef char sandbox_edited_map_variant_index_offset[(offsetof(s_sandbox_globals, edited_map_variant_index) == 0x720) ? 1 : -1];
typedef char sandbox_layout_check_1[(sizeof(s_sandbox_globals) == 0x748) ? 1 : -1];
typedef char sandbox_layout_check_2[(offsetof(s_sandbox_globals, edit_position) == 0x8c) ? 1 : -1];
typedef char sandbox_layout_check_3[(offsetof(s_sandbox_globals, player_axes) == 0x20c) ? 1 : -1];
typedef char sandbox_layout_check_4[(offsetof(s_sandbox_globals, object_indices) == 0x31c) ? 1 : -1];
typedef char sandbox_layout_check_5[(offsetof(s_sandbox_globals, object_count) == 0x71c) ? 1 : -1];
typedef char sandbox_layout_check_6[(offsetof(s_sandbox_globals, variant_file_exists) == 0x728) ? 1 : -1];
typedef char sandbox_layout_check_7[(offsetof(s_sandbox_globals, owners) == 0xc) ? 1 : -1];
typedef char sandbox_layout_check_8[(offsetof(s_sandbox_globals, edited_object_properties) == 0x730) ? 1 : -1];

#endif
