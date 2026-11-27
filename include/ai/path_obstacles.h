#ifndef AI_PATH_OBSTACLES_H
#define AI_PATH_OBSTACLES_H

// Functions defined in tu/ailibxbox/path_obstacles.cpp that other units call.

struct obstacles;
union real_point3d;
union real_vector3d;
struct s_obstacle_profile;

void obstacle_profile_set_actor(long actor_index, union real_point3d const * position, union real_vector3d const * direction,
    short type, bool enabled, struct s_obstacle_profile * profile);
bool obstacles_add_disc(obstacles *list, long object_index, short flags, const real_point3d *point, float radius);
void obstacles_recompute(struct obstacles * list, float margin);

struct profile_source;
struct special_movement;

extern "C" void obstacle_profile_initialize(s_obstacle_profile *profile, const real_point3d *position, float radius, long flags,
    profile_source *source, const real_vector3d *direction, long index1, long index2);
extern "C" void obstacles_add_nearby_discs(long p0, long p1, short p2, bool p3, special_movement *movement, obstacles *obs, obstacles *obs2,
    const real_point3d *point, const real_point3d *actor_position, float radius, const real_vector3d *vect, const real_vector3d *vect2, long p11, long p12);

union real_point2d;
union real_vector2d;

void obstacles_disc_tangents(const obstacles *obs, short disc_index, const real_point2d *point, float radius,
    real_vector2d *tangent0, real_vector2d *tangent1, float *distance);

struct path_state;
struct path_step;
extern "C" bool obstacles_find_avoidance_path(long actor_index, const path_state *state, short point_count, const path_step *points,
    bool allow_partial, short *output_count, path_step *output_points, bool *final_point, long *ignorable_object_index,
    short *ignorable_flags, bool *blocked);

#endif // AI_PATH_OBSTACLES_H
