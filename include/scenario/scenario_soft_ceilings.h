#ifndef SCENARIO_SCENARIO_SOFT_CEILINGS_H
#define SCENARIO_SCENARIO_SOFT_CEILINGS_H

// Functions defined in tu/blamlibxbox/scenario_soft_ceilings.cpp that other units call.

#include <stddef.h>

struct s_scenario_soft_ceiling {
  unsigned short flags;
  short pad2;
  long name;
  long pad8;
};

typedef char scenario_soft_ceiling_size[(sizeof(s_scenario_soft_ceiling) == 12) ? 1 : -1];
typedef char scenario_soft_ceiling_name_offset[(offsetof(s_scenario_soft_ceiling, name) == 4) ? 1 : -1];

s_scenario_soft_ceiling *scenario_soft_ceiling_from_name(long name);

struct collision_feature_list;
union real_point3d;
struct s_soft_ceiling_triangle;

extern "C" bool scenario_soft_ceilings_is_point_in_triangle(const long *count_pointer, const s_soft_ceiling_triangle *shape, const real_point3d *point,
    float tolerance, float *distance);
extern "C" bool scenario_soft_ceilings_push_point_out(real_point3d *point, float height, float radius);
extern "C" void scenario_soft_ceilings_add_collision_features(collision_feature_list *list, const real_point3d *point, float radius, float height,
    float feature_radius);

// The allocator of the per-thread soft ceiling state (restricted region 3), defined in scenario_soft_ceilings.cpp.
void __tls_set_g_scenario_soft_ceilings_allocator(void *allocation);
template<long, void (*)(void *), void (*)(void *), void (*)(void *)>
class t_restricted_allocation_manager;
extern t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_scenario_soft_ceilings_allocator> g_scenario_soft_ceilings_allocator;

#endif // SCENARIO_SCENARIO_SOFT_CEILINGS_H
