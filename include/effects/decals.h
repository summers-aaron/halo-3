#ifndef EFFECTS_DECALS_H
#define EFFECTS_DECALS_H


struct s_render_geometry;

void render_mesh_part_no_material(const s_render_geometry *geometry, long mesh_index, long part_index, unsigned long *subpart_mask);


// Functions defined in tu/blamlibxbox/decals.cpp.

void __tls_set_g_decal_array_allocator(void *object);
void __tls_set_g_decal_system_allocator(void *object);
extern "C" void decal_system_destroy_all(void);

union real_point3d;
union real_vector3d;
struct s_cluster_reference;
struct s_decals_cluster_request;

struct s_game_non_bsp_zone_set;
class c_scenario_resource_registry;
extern "C" void decal_system_remove_invalid_definitions(const s_game_non_bsp_zone_set *old_zone_set, const s_game_non_bsp_zone_set *new_zone_set,
    c_scenario_resource_registry *pending_zone_registry);
extern "C" void decal_system_remove_cluster_from_pvs(const s_cluster_reference *cluster_reference);
extern "C" void decals_add_visible_system_decals(const s_decals_cluster_request *request);
extern "C" long decal_system_create(unsigned char type, long system_index, long definition_index, long ignore_object_index, bool preplaced,
    const real_point3d *point, const real_vector3d *forward, const real_vector3d *up);

class c_decal;
class c_decal_system;
void __tls_set_g_decal_message_queue_allocator(void *address);
#include "cache/restricted_memory_regions.h"
typedef t_restricted_allocation_manager<4, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_decal_message_queue_allocator> s_decals_restricted_allocation;
extern s_decals_restricted_allocation g_decal_counts_allocator;
extern "C" void *decals_reserve_counts_memory(s_decals_restricted_allocation *allocation, const char *name, const char *type, unsigned int size);
extern "C" void __tls_set_g_decal_data_allocator(void *object);
extern "C" void decal_destroy(long child_index);
extern "C" void decals_add_decal_to_render_list(const c_decal *decal);
float decals_system_get_distance_fade(const c_decal_system *system);



#endif // EFFECTS_DECALS_H
