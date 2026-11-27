#ifndef OBJECTS_OBJECT_TYPE_DEFINITION_H
#define OBJECTS_OBJECT_TYPE_DEFINITION_H

#include <stddef.h>
#include "objects/object_types.h"

struct object_placement_data;
struct s_scenario_object;
struct s_game_cluster_bit_vectors;
struct real_matrix4x3;
union real_point3d;
union real_vector3d;

// The mover data of an object type that moves under a motor (biped, vehicle, creature, giant): March
// biped_mover_definition_data at 0x8279dbcc is {4, 0xf0, 2}. mover_new sizes the motor task, motor program and
// action blocks from it.
class c_mover_definition_data
{
public:
    short task_count;
    short program_size;
    short program_alignment;
};

extern c_mover_definition_data biped_mover_definition_data;
extern c_mover_definition_data vehicle_mover_definition_data;
extern c_mover_definition_data creature_mover_definition_data;
extern c_mover_definition_data giant_mover_definition_data;

// One object type (biped, vehicle, weapon, ...); object_type_definitions[] (object_types.cpp) indexes them by
// e_object_type and March reaches one through object_type_definition_get(e_object_type).
// Offsets from tu/objectslibxbox/object_types.cpp's copy (the owner) and object_placement.cpp, checked against
// the per-type tables: the type name ("biped",
// "vehicle"), +0x08 the datum size (biped 0x6b4, vehicle 0x784, weapon 0x28c), +0x14 the mixing board definition
// index (biped 0, vehicle 1, weapon -1; mover_update tests it against -1) and +0x18 the mover data (null for
// weapons; March mover_new reads it at +24 and asserts it).
struct object_type_definition
{
    const char *name;
    unsigned long group_tag;
    unsigned short datum_size;
    short scenario_datums_offset;
    short scenario_palette_offset;
    short scenario_datum_size;
    long maximum_placement_count;
    long mixing_board_definition_index;
    c_mover_definition_data *mover_definition_data;
    void (*initialize)();
    void (*dispose)();
    void (*initialize_for_new_map)();
    void (*dispose_from_old_map)();
    void (*initialize_for_new_structure_bsp)(unsigned long activating_structure_bsp_mask);
    void (*dispose_from_old_structure_bsp)(unsigned long deactivating_structure_bsp_mask);
    void (*adjust_placement)(object_placement_data *placement);
    bool (*new_proc)(long object_index, object_placement_data *placement, bool *out_of_objects);
    void (*place)(long object_index, const s_scenario_object *scenario_object);
    void (*unplace)(long object_index);
    void (*create_children)(long object_index);
    void (*delete_proc)(long object_index);
    bool (*update)(long object_index);
    bool (*post_update)(long object_index);
    void (*move)(long object_index);
    bool (*compute_activation)(long object_index, const s_game_cluster_bit_vectors *cluster_activation, bool *in_active_clusters);
    bool (*compute_function_value)(long object_index, long function_name, long function_index, float *value, bool *active,
        bool *deterministic);
    void (*attach_gamestate_entity)(long object_index);
    void (*detach_gamestate_entity)(long object_index);
    void (*attach_to_marker)(long object_index, long parent_object_index, long parent_marker_name, long child_marker_name);
    void (*attach_to_node)(long parent_object_index, long child_object_index, short node_index);
    void (*detach_from_parent)(long object_index);
    void (*handle_deleted_object)(long object_index, long deleted_object_index);
    unsigned long inherited_mask;
    void (*handle_deleted_player)(long object_index, long player_index);
    bool (*handle_region_destroyed)(long object_index, short region_index, unsigned long damage_section_flags);
    bool (*handle_parent_destroyed)(long object_index);
    void (*fix_transform)(long object_index, real_point3d *position, real_vector3d *forward, real_vector3d *up);
    void (*fix_transform_to_physics)(long object_index, real_matrix4x3 *matrix);
    void (*fix_transform_from_physics)(long object_index, real_matrix4x3 *matrix);
    void (*preprocess_node_orientations)(long object_index, const c_static_flags<255> *node_mask, long node_count,
        real_orientation *node_orientations);
    void (*preprocess_root_node_matrix)(long object_index, real_matrix4x3 *matrix);
    void (*postprocess_node_matrices)(long object_index, long node_count, real_matrix4x3 *matrices);
    void (*update_part)(long object_index);
    void (*scripted_sound)(long object_index, long sound_index, long impulse_index);
    void (*cleanup_lightmap)(long object_index);
    object_type_definition *parts[16];
    object_type_definition *next;
    unsigned long mask;
};

typedef char object_type_definition_mixing_offset[(sizeof(void *) != 4 || offsetof(object_type_definition, mixing_board_definition_index) == 0x14) ? 1 : -1];
typedef char object_type_definition_mover_offset[(sizeof(void *) != 4 || offsetof(object_type_definition, mover_definition_data) == 0x18) ? 1 : -1];
typedef char object_type_definition_parts_offset[(sizeof(void *) != 4 || offsetof(object_type_definition, parts) == 0xac) ? 1 : -1];
typedef char object_type_definition_attach_to_node_offset[(sizeof(void *) != 4 || offsetof(object_type_definition, attach_to_node) == 0x6c) ? 1 : -1];
typedef char object_type_definition_size[(sizeof(void *) != 4 || sizeof(object_type_definition) == 0xf4) ? 1 : -1];

#endif // OBJECTS_OBJECT_TYPE_DEFINITION_H
