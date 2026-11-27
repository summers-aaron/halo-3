#ifndef PHYSICS_WALKER_PHYSICS_H
#define PHYSICS_WALKER_PHYSICS_H

// Functions defined in tu/blamlibxbox/walker_physics.cpp.

float calculate_law_off_cosines_angle(float a, float b, float c);

struct s_walker_physics_definition;
struct real_matrix3x3;
struct real_matrix4x3;
union real_vector3d;

class c_walker_physics
{
public:
    static void keyframe_leg_group(const s_walker_physics_definition *, const real_matrix3x3 *, long, long, long,
        const long *, const real_vector3d *, float);
    static long change_leg_group_stuck_state(const s_walker_physics_definition *, long, long, bool);
    static bool calculate_foot_group_center_matrix(const s_walker_physics_definition *, long, long, real_matrix4x3 *);
private:
    static void keyframe_leg(const s_walker_physics_definition *, long, long, bool, const real_matrix4x3 *,
        const real_matrix4x3 *, const real_vector3d *, float);
};

extern "C" void walker_physics_calculate_foot_matrix(
    const s_walker_physics_definition *definition, long object_index, long leg_index, bool use_marker_position,
    real_matrix4x3 *matrix);

#endif // PHYSICS_WALKER_PHYSICS_H
