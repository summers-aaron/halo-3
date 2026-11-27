// blamlibxbox/point_physics.obj: from the records 0x23cda0 0x23cdd8
// @flags /TP /O2 /Oc
#include "cseries/cseries.h"
#include "physics/collision_types.h"
#include "physics/collision_test_flags.h"
#include "math/real_math.h"
#include "objects/object_header.h"
#include "cseries/bit_operations.h"
#include "scenario/scenario.h"
#include "physics/point_physics.h"
#include "physics/physics_constants_globals.h"
#include "physics/collision_test_vector.h"

#if !defined(_XBOX) || defined(__clang__)
#include "objects/widgets/antenna.h"

// The Xbox reference at 0x7c6058 resolves to the point-physics float pool.
extern "C" {
float antenna_update_time_step = 0.0333f;
}
#endif

// @split_bss
// @sparse_bss 0x827e88b4..0x827e8928
// @address 0x7e88b4 g_point_physics_water_mass_scale
static float g_point_physics_water_mass_scale;
static float s_point_physics_air_mass_scale;

// The March release map folds these empty game-system callbacks into the shared blr at 0x5bec98.
// @address 0x5bec98 ?point_physics_initialize@@YAXXZ
void point_physics_initialize()
{
}

// @address 0x5bec98 ?point_physics_dispose@@YAXXZ
void point_physics_dispose()
{
}

void point_physics_initialize_for_new_map()
{
    // match: 0x23cda0: keeps water_density in a register across the air_mass_scale store (without it c2 reloads it)
    const s_physics_constants *__restrict constants = global_physics_constants_get();
    s_point_physics_air_mass_scale = constants->air_density * 118613.34f;
    g_point_physics_water_mass_scale = constants->water_density * 118613.34f;
}

// @address 0x5bec98 ?point_physics_dispose_from_old_map@@YAXXZ
void point_physics_dispose_from_old_map()
{
}

// @address 0x23cdd8
unsigned long point_physics_update(long, const point_physics_definition *definition, s_location *location, short, real_point3d *out_point,
    real_vector3d *in_point, const real_vector3d *, real_vector3d *, c_global_material_type *, float, float scale)
{
    unsigned long result = 0;
    float damping;
    float factor;
    short bounce;
    float mass_factor;
    float density;
    float mass;
    float divisor;
    s_collision_test_flags test_flags;
    real_vector3d perpendicular;
    s_location new_location;
    real_vector3d scaled_vector;
    real_vector3d parallel;

    scale = 0.0333f;
    mass = definition->runtime_mass_over_radius_cubed;
    density = definition->density;
    test_flags = k_collision_test_none;
    SET_BIT(result, 0, true);
    BLAM_ASSUME(result == 1);
    mass_factor = definition->air_friction;
    mass_factor *= 0.0004f;
    mass = s_point_physics_air_mass_scale + mass;
    damping = density * 2.0f / (global_physics_constants_get()->air_density + density);
    damping = 1.0f - damping;
    if (TEST_BIT(definition->flags, _point_physics_no_gravity_bit))
        damping = 0.0f;
    divisor = mass * 7.999999e-06f;
    if (divisor != 0.0f)
    {
        factor = scale / divisor;
        factor *= mass_factor;
        factor = PIN(factor, 0.0f, 1.0f);
    }
    else
        factor = mass_factor != 0.0f ? 1.0f : 0.0f;
    in_point->k += damping * global_gravity_get() * scale;
    const real_vector3d zero_current = {0.0f, 0.0f, 0.0f};
    in_point->j += factor * (zero_current.j - in_point->j);
    in_point->i = in_point->i + factor * (zero_current.i - in_point->i);
    in_point->k += factor * (zero_current.k - in_point->k);
    if (TEST_BIT(definition->flags, _point_physics_collides_with_structures_bit))
    {
        test_flags.collision_flags |= 0x809;
    }
    if (TEST_BIT(definition->flags, _point_physics_collides_with_water_surface_bit))
    {
        test_flags.collision_flags |= 0x802;
    }
    if (test_flags.collision_flags ? test_flags.collision_flags : test_flags.object_flags)
    {
        s_collision_test_flags template_copy = test_flags;
        bounce = 0;
        do
        {
            if (bounce >= 3)
                break;
            collision_result collision;
            scale_vector3d(in_point, scale, &scaled_vector);
            if (!collision_test_vector(template_copy, false, out_point, &scaled_vector, -1, -1, -1, &collision))
            {
                if (collision.location.cluster_reference.bsp_index != -1)
                    *location = collision.location;
                *out_point = collision.point;
                break;
            }
            if (collision.type == 2)
                SET_BIT(result, 3, true);
            else if (collision.type == 1)
                SET_BIT(result, 2, true);
            component_vectors_from_normal3d(in_point, &collision.plane.normal, &parallel, &perpendicular);
            in_point->i = (1.0f - definition->surface_friction) * perpendicular.i - definition->elasticity * parallel.i;
            in_point->j = (1.0f - definition->surface_friction) * perpendicular.j - definition->elasticity * parallel.j;
            in_point->k = (1.0f - definition->surface_friction) * perpendicular.k - definition->elasticity * parallel.k;
            if (collision.location.cluster_reference.bsp_index != -1)
                *location = collision.location;
            scale = scale - collision.fraction * scale;
            bounce++;
            out_point->x = collision.plane.normal.i * 0.005f + collision.point.x;
            out_point->y = collision.plane.normal.j * 0.005f + collision.point.y;
            out_point->z = collision.plane.normal.k * 0.005f + collision.point.z;
        } while (scale != 0.0f);
    }
    else
    {
        point_from_line3d(out_point, in_point, scale, out_point);
        scenario_location_from_point(&new_location, out_point);
        if (((const s_cluster_reference *)&new_location)->bsp_index != -1)
            *location = new_location;
    }
    return result;
}
