#ifndef AI_PATH_SMOOTHING_H
#define AI_PATH_SMOOTHING_H

// Functions defined in tu/ailibxbox/path_smoothing.cpp.

struct path_state;
struct path_step;
union real_point3d;

bool find_turning_point(short structure_bsp_index, const real_point3d *point, float radius, long link_index, bool clockwise,
    short projection, bool sign, bool unused, real_point3d *turning_point);
void find_avoidance_point(union real_point3d const * a, union real_point3d const * center, union real_point3d const * b, float radius,
    bool left, short index, bool flag, union real_point3d * out);
void path_smooth(path_state *state, short raw_step_count, const path_step *raw_steps, 
    short *smoothed_step_count, path_step *smoothed_steps, bool *steps_finish_path);

union real_point2d;

void find_tangent_point(const real_point2d *point, const real_point2d *center, float radius, bool left, real_point2d *tangent);

#endif // AI_PATH_SMOOTHING_H
