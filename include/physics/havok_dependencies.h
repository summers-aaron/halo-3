#ifndef PHYSICS_HAVOK_DEPENDENCIES_H
#define PHYSICS_HAVOK_DEPENDENCIES_H
#include "math/matrix_math_types.h"
#include "multithreading/registered_thread_index.h"
#include "cache/restricted_address.h"
#include "objects/object_header_data.h"
#include "game/game_globals_storage.h"
#include "tag_files/tag_index.h"
#include "cache/restricted_memory_regions.h"
#include "objects/object_globals.h"
#include "physics/havok_proxies_globals.h"
struct s_penetration_collector;
class phys_critical_section;
struct s_data_array;
struct s_impact_data_array;
struct s_havok_memory_usage;
struct scenario;
class c_havok_component;
class phys_collision_proxy;
class phys_shape;
class phys_transform;
class phys_entity;
class c_restricted_memory;
struct havok_shape_collides_phantom;
#include "physics/impacts_globals_types.h"
#include "effects/effect_lightprobe_memory.h"
extern "C" {
unsigned char havok_component_has_active_entity(c_havok_component *component);
void havok_component_scalar_delete(c_havok_component *component, long flag);
extern bool g_shell_floating_point_exceptions_enabled;
}
extern scenario *global_scenario;
extern unsigned long g_active_structure_bsp_mask;
extern c_restricted_memory g_restricted_regions[];
void __tls_set_g_tasks_data_allocator(void *allocator);
extern t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_tasks_data_allocator> g_havok_game_state_allocator;
void havok_compute_memory_in_use(s_havok_memory_usage *havok_memory_usage);
extern "C" void havok_penetration_collector_add_cd_body_pair(s_penetration_collector *self, const phys_collision_proxy &body_a,
    const phys_collision_proxy &body_b);
extern "C" unsigned int _clearfp(void);
extern "C" unsigned int _controlfp(unsigned int new_value, unsigned int mask);
extern "C" void object_activation_regions_update_for_object(int object_index, unsigned char allocate_region);
extern "C" void impacts_post_process_havok_entity(phys_entity *entity);
extern "C" long effect_new_unattached_from_point(c_tag_index definition_index, const real_point3d *position, const real_vector3d *forward,
    const real_vector3d *up, long match_all_markers, long deterministic, const void *plane, const void *velocity);
#if defined(_MSC_VER) && !defined(__clang__)
// match: 0x1bd218: havok_cleanup_large_simulation_islands reads g_havok_world through this C function symbol, so the
// compiler builds the address (lis, addi) and loads from it; a plain read folds the address (-4B).
namespace havok_cleanup { extern "C" void g_havok_world(); }
#endif
#endif
