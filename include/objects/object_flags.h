#ifndef OBJECTS_OBJECT_FLAGS_H
#define OBJECTS_OBJECT_FLAGS_H

// Per-object state flags. Bit positions are fixed; each name describes what its bit controls.
enum e_object_data_flags {
    _object_hidden_bit = 0,
    _object_always_active_bit = 1,
    _object_being_created_bit = 2,
    _object_data_flag_3 = 3,
    _object_has_attached_lights_bit = 4,
    _object_has_unattached_lights_bit = 5,
    _object_has_attached_looping_sounds_bit = 6,
    _object_in_limbo_bit = 7,
    _object_connected_to_map_bit = 8,
    _object_has_collision_model_bit = 9,
    _object_mirrored_bit = 10,
    _object_has_model_instances_bit = 11,
    _object_data_flag_12 = 12,
    _object_data_flag_13 = 13,
    _object_data_flag_14 = 14,
    _object_data_flag_15 = 15,
    _object_always_visible_bit = 16,
    _object_deleted_when_deactivated_bit = 17,
    _object_outside_map_bit = 18,
    _object_cinematic_lod_bit = 19,
    _object_cinematic_collision_bit = 20,
    _object_cinematic_visibility_bit = 21,
    _object_data_flag_22 = 22,
    _object_data_flag_23 = 23,
    _object_data_flag_24 = 24,
    _object_data_flag_25 = 25,
    _object_data_flag_26 = 26,
    _object_data_flag_27 = 27,
    _object_has_script_reference_bit = 28,
    _object_frozen_bit = 29,
    _object_uses_render_only_node_mask_bit = 30,
    _object_data_flag_31 = 31,
    k_object_data_flags_count = 32
};

// March vehicle_try_to_join_local_physics uses unsigned-long storage and nineteen bits.
enum e_object_physics_flags {
    _object_physics_flag_1 = 1,
    _object_physics_flag_2 = 2,
    _object_physics_joined_local_physics_bit = 3,
    _object_physics_connected_bit = 7,
    _object_physics_bit9 = 9,
    _object_physics_in_water_bit = 10,
    _object_physics_phantom_active_bit = 13,
    _object_physics_has_havok_proxy_bit = 14,
    k_object_physics_flags_count = 19
};

#endif // OBJECTS_OBJECT_FLAGS_H
