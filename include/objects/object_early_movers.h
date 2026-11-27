#ifndef OBJECTS_OBJECT_EARLY_MOVERS_H
#define OBJECTS_OBJECT_EARLY_MOVERS_H

template<long k_region_index, void (*k_adopt_function)(void *), void (*k_post_copy_function)(void *),
    void (*k_set_allocator_function)(void *)>
class t_restricted_allocation_manager;

// Functions defined in tu/objectslibxbox/object_early_movers.cpp.

void object_early_movers_initialize(void);
void object_early_movers_initialize_for_new_map(void);
void object_early_movers_dispose_from_old_map(void);

// Defined in tu/objectslibxbox/object_scripting.cpp.
void object_early_movers_dispose(void);


void __tls_set_g_object_early_movers_globals_allocator(void *address);
long object_early_mover_local_space_index(long object_index);
void object_in_early_mover_join(long source_index, long object_index);
void object_in_early_mover_detach(long object_index);
void object_in_early_mover_physics_update(long object_index);
void object_in_early_mover_physics_move(long object_index, void (*move)(long));
void object_early_mover_notify_local_objects(long early_mover_object_index);
void object_early_mover_delete(long object_index);
extern "C" void object_early_mover_update_state(long object_index, long early_mover_index);
extern "C" void object_early_mover_move(long object_index, long early_mover_index);
extern "C" bool object_early_mover_is_owner(long object_index, bool check_definition);
extern "C" bool object_early_mover_get_node_and_rigid_body_index(long index, long *node, long *rigid);

union real_point3d;
union real_vector3d;

bool object_get_early_mover_local_space_velocity(long object_index, const real_point3d *point, real_vector3d *offset, real_vector3d *axis_out,
    bool check_owner, bool primary);
bool object_move_in_early_mover_local_space(long index, real_vector3d *mover_velocity, real_vector3d *object_velocity, bool use_physics);

extern t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_object_early_movers_globals_allocator> g_object_early_movers_globals_allocator;

#endif // OBJECTS_OBJECT_EARLY_MOVERS_H
