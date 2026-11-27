#ifndef PHYSICS_BREAKABLE_SURFACES_H
#define PHYSICS_BREAKABLE_SURFACES_H

// Functions defined in tu/blamlibxbox/breakable_surfaces.cpp.

struct s_breakable_surface_set;
struct s_damage_data;

long breakable_surface_get_surface_set_count(long slot_index, long variant_index);
struct s_breakable_surface_set * breakable_surface_get_first_set(long slot_index, long variant_index);
void breakable_surfaces_reset(void);
bool breakable_surface_extant(long slot_index, long variant_index, long set_index, long surface_index);
void breakable_surface_render_cracks(long bsp_index, long variant_index, long set_index);
void breakable_surface_damage(long bsp_index, long variant_index, long set_index, long surface_index, const s_damage_data *damage_data, long record_index);

union real_point3d;
struct s_breakable_surfaces_event_material;

extern "C" void breakable_surface_damage_area_of_effect(const s_breakable_surfaces_event_material *material, const real_point3d *position, float radius);
extern "C" bool breakable_surfaces_is_in_soft_ceiling(long object_index);
extern "C" void breakable_surfaces_update(void);

union real_vector3d;

extern "C" void breakable_surfaces_get_particle_emission(long event_index, real_point3d *point, real_vector3d *direction);

struct real_matrix4x3;
struct s_breakable_surface_hash;
struct collision_bsp;

void __tls_set_g_breakable_surface_globals_allocator(void *value);
extern "C" void breakable_surfaces_dispose(void);
extern "C" void breakable_surfaces_initialize_for_new_map(void);
extern "C" void breakable_surfaces_dispose_from_old_map(void);
extern "C" unsigned char breakable_surface_is_valid_set(long slot_index, long variant_index, long surface_a, long surface_b, long set_index);
extern "C" void breakable_surface_mark_broken(long slot_index, long variant_index, long set_index, long surface_index, long unused);
extern "C" void breakable_surfaces_dispose_from_old_structure_bsp(unsigned long structure_bsp_mask);
extern "C" unsigned char breakable_surfaces_is_shortest_edge(collision_bsp *bsp, long surface_index, long edge_index);
extern "C" long breakable_surfaces_new_slot(void);
extern "C" real_matrix4x3 *breakable_surface_get_bsp(long object_index, long node_index, const void **node_out);
extern "C" void breakable_surface_break_surface(long bsp_index, long variant_index, long set_index, long surface_index, long record_index,
    const real_point3d *position, const s_breakable_surfaces_event_material *material, const real_vector3d *direction);
extern "C" float breakable_surfaces_compute_surface_area(collision_bsp *bsp, long surface_index, float scale);
extern "C" bool breakable_surface_hash_insert(s_breakable_surface_hash *hash, long first_hash, long second_hash, unsigned short key, long value);
extern "C" void breakable_surfaces_new_crack(const real_point3d *position, long slot_index, long variant_index, long record_index);
extern "C" void breakable_surfaces_break_disconnected(long bsp_index, long variant_index, long set_index, long surface_index, collision_bsp *bsp,
    const unsigned long *flags, const real_point3d *position, const s_breakable_surfaces_event_material *material);
extern "C" void breakable_surfaces_propagate_break(long bsp_index, long variant_index, long record_index,
    const real_point3d *position, const s_breakable_surfaces_event_material *material);
extern "C" void breakable_surfaces_handle_pending_break(void);

union real_rectangle3d;
struct s_breakable_surfaces_list;

extern "C" void breakable_surfaces_collect_broken_surfaces(s_breakable_surfaces_list *list, real_rectangle3d *bounds, long surface_index, long set_index,
    const unsigned long *flags, float *area, float scale, collision_bsp *bsp);


// Shared declarations used by breakable_surfaces.cpp.
void __tls_set_g_simulation_gamestate_entity_data_allocator(void *object);

void breakable_surfaces_initialize(void);
void breakable_surfaces_initialize_for_new_structure_bsp(unsigned long structure_bsp_mask);

#endif // PHYSICS_BREAKABLE_SURFACES_H
