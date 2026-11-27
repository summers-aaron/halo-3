#ifndef OBJECTS_MULTIPLAYER_GAME_OBJECTS_H
#define OBJECTS_MULTIPLAYER_GAME_OBJECTS_H

// Functions defined in tu/objectslibxbox/multiplayer_game_objects.cpp.

union real_point3d;
union real_vector3d;
struct s_multiplayer_object_boundary_geometry_data;

void multiplayer_object_calculate_boundary_geometry(s_multiplayer_object_boundary_geometry_data *out_geometry_data, long target_object_index);
void multiplayer_get_spawn_object_position_and_facing_vectors(long object_index, real_point3d *position, real_vector3d *forward, real_vector3d *up);
bool point_in_multiplayer_object_boundary(const real_point3d *point, const s_multiplayer_object_boundary_geometry_data *boundary);
bool player_in_multiplayer_object_boundary(long player_index, const s_multiplayer_object_boundary_geometry_data *boundary);

struct s_multiplayer_object_properties;

void multiplayer_object_calculate_box_boundary_geometry(const s_multiplayer_object_properties *multiplayer_object_properties,
    s_multiplayer_object_boundary_geometry_data *boundary_geometry);

void multiplayer_object_calculate_boundary_geometry_from_properties(long object_index,
    const s_multiplayer_object_properties *multiplayer_object_properties, s_multiplayer_object_boundary_geometry_data *boundary_geometry,
    bool use_opaque_shader);

#endif // OBJECTS_MULTIPLAYER_GAME_OBJECTS_H
