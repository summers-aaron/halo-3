#ifndef UNITS_GIANTS_H
#define UNITS_GIANTS_H

// Functions defined in tu/objectslibxbox/giants.cpp.

void giant_setup_foot_tracking_data(long giant_index);
void giant_setup_buckling_parameters(long giant_index);
bool giant_handle_motor_update(long giant_index, bool update_node_matrices);
extern "C" float giant_get_slider_value(long giant_index);
extern "C" long giant_is_animation_state_current(long giant_index);

class c_animation_manager;
void giant_animation_setup_mixing_board(c_animation_manager *animation_manager);

struct real_matrix4x3;
union real_point3d;
union real_vector3d;

bool giant_find_ground_point(long giant_index, union real_point3d const * foot_point, union real_point3d * start_out,
    union real_vector3d * vector_out, union real_point3d * ground_out);
extern "C" long giant_get_variant_animation_state(long giant_index);
void setup_buckling_globals(long giant_index, long x_step_count, long y_step_count, const real_matrix4x3 *world_center_matrix,
    float x_start, float y_start, float x_step, float y_step);
bool giant_solve_future_marker_position(long giant_index, long marker_index, long frame, real_matrix4x3 *matrix);

class c_sector_ref;

void giant_find_pathfinding_location(long giant_index, short *, long *, c_sector_ref * sector, real_point3d *point, long *object_index, unsigned long *flags);

struct s_buckling_map;

// match: 0x3600d0: __restrict on this first declaration pins the I in the mangled name
float compute_score_of_buckle_angles(const s_buckling_map *__restrict map, float pitch, float roll,
    const real_matrix4x3 *max_depth_matrix, real_matrix4x3 *output_world_matrix);

struct real_orientation;
extern "C" real_orientation g_giant_scratch_orientations[255];

bool giant_ik_foot_chain(long object_index, real_point3d *point, float weight, long marker_name, float minimum_angle,
    float maximum_angle, long node_index, real_matrix4x3 *node_matrices);

#endif // UNITS_GIANTS_H
