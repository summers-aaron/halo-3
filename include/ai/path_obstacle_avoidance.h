#ifndef AI_PATH_OBSTACLE_AVOIDANCE_H
#define AI_PATH_OBSTACLE_AVOIDANCE_H

// Functions defined in tu/ailibxbox/path_obstacle_avoidance.cpp.

struct obstacles;
struct s_obstacle_profile;

extern "C" bool path_obstacle_avoidance_add_object_discs(long actor_index, long object_index, s_obstacle_profile *profile,
	short projection_axis, bool projection_sign, obstacles *obstacle_list, obstacles *ignorable_obstacle_list);

struct obstacle_path;

bool path_iterate(obstacle_path *path, bool partial);

class c_sector_ref;
union real_point3d;
union real_vector3d;
struct path_state;
bool actor_test_ignorable_obstacles(long, const obstacles *, short, bool, const real_point3d *,
	c_sector_ref, const real_vector3d *, float, float, long *, short *);
short path_add_step(obstacle_path *path, const real_point3d *point, c_sector_ref sector, short obstacle_index,
	bool side, bool turning_point, short disc_index, float base_cost, short parent_index);
bool path_find(const path_state *state, obstacle_path *path, bool flag, const obstacles *o, float radius, const real_point3d *start,
	c_sector_ref start_sector, const real_point3d *goal, c_sector_ref goal_sector, short projection_axis, bool projection_sign,
	bool partial, bool skip_flagged);

#endif // AI_PATH_OBSTACLE_AVOIDANCE_H
