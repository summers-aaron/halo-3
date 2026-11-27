#ifndef CUTSCENE_CINEMATIC_NEW_GLOBALS_TYPES_H
#define CUTSCENE_CINEMATIC_NEW_GLOBALS_TYPES_H

#include <stddef.h>
#include "math/real_math_types.h"

struct s_cinematics_name_entry { char name[32]; long object_index; unsigned long group_tag; };

struct s_cinematics_new_group { real_vector3d start; real_vector3d end; long frame0; long frame1; long count; long object_indices[24]; };
struct s_cinematic_new_globals_definition
{
    long current_scene;
    long scene_tag_index;
    long current_shot;
    long early_exit;
    long count;
    s_cinematics_name_entry entries[100];
    long group_count;
    s_cinematics_new_group groups[32];
};
typedef char cinematic_new_globals_size[(sizeof(s_cinematic_new_globals_definition) == 0x2038) ? 1 : -1];
typedef char cinematic_new_entries_offset[(offsetof(s_cinematic_new_globals_definition, entries) == 0x14) ? 1 : -1];

// Per-thread pointer to the active cinematic state.
extern __declspec(thread) s_cinematic_new_globals_definition *cinematic_new_globals;

typedef char cinematic_new_group_count_offset[(offsetof(s_cinematic_new_globals_definition, group_count) == 0xfb4) ? 1 : -1];
typedef char cinematic_new_groups_offset[(offsetof(s_cinematic_new_globals_definition, groups) == 0xfb8) ? 1 : -1];
typedef char cinematic_new_group_size[(sizeof(s_cinematics_new_group) == 0x84) ? 1 : -1];
#endif
