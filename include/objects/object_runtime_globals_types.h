#ifndef OBJECTS_OBJECT_RUNTIME_GLOBALS_TYPES_H
#define OBJECTS_OBJECT_RUNTIME_GLOBALS_TYPES_H
#include <stddef.h>
#include "render/render_lighting_types.h"
// Object runtime storage; owner: objects.cpp. The per-type name table begins at 0x24c.
struct s_object_globals {
    bool can_connect_to_map;
    bool garbage_collect_now;
    unsigned char garbage_collect_aggressive;
    bool objects_updating;
    long recyclable_garbage_object_count;
    long first_garbage_object_index;
    long active_garbage_object_count;
    long last_garbage_fail_time;
    long last_loud_weapon_fire_time;
    long object_identifier_counter;
    long current_object_update_absolute_index;
    render_lighting cinematic_lighting;
    float shadowless_values[4];
    bool custom_animations_hold_on_last_frame;
    bool custom_animations_prevent_lipsync_head_movement;
    bool warthog_chaingun_light_disabled;
    char alignment24b;
    long first_name_index[14];                  // by object type
    long object_indices[5880];
};
typedef char object_globals_size[(sizeof(s_object_globals) == 0x5e64) ? 1 : -1];
typedef char object_globals_identifier_offset[(offsetof(s_object_globals, object_identifier_counter) == 0x18) ? 1 : -1];
typedef char object_globals_update_index_offset[(offsetof(s_object_globals, current_object_update_absolute_index) == 0x1c) ? 1 : -1];
typedef char object_globals_loud_weapon_offset[(offsetof(s_object_globals, last_loud_weapon_fire_time) == 0x14) ? 1 : -1];
typedef char object_globals_cinematic_lighting_offset[(offsetof(s_object_globals, cinematic_lighting) == 0x20) ? 1 : -1];
typedef char object_globals_shadowless_offset[(offsetof(s_object_globals, shadowless_values) == 0x238) ? 1 : -1];
typedef char object_globals_chaingun_offset[(offsetof(s_object_globals, warthog_chaingun_light_disabled) == 0x24a) ? 1 : -1];

// March 2007 TLS slot 0x831df6b0.
extern __declspec(thread) s_object_globals *object_globals;

#endif
