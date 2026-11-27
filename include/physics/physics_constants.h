#ifndef PHYSICS_PHYSICS_CONSTANTS_H
#define PHYSICS_PHYSICS_CONSTANTS_H

// Functions defined in tu/blamlibxbox/physics_constants.cpp.

float global_slip_surface_maximum_k_get(void);
void __tls_set_g_physics_constants_allocator(void *allocator);
void physics_constants_reset(void);
void physics_constants_dispose(void);

extern float g_default_slip_surface_maximum_angle;
template<long, void (*)(void *), void (*)(void *), void (*)(void *)>
class t_restricted_allocation_manager;
typedef t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0),
	static_cast<void (*)(void *)>(0), &__tls_set_g_physics_constants_allocator> t_physics_constants_allocation_manager;
extern t_physics_constants_allocation_manager g_physics_constants_allocator;

void physics_constants_initialize(void);
void physics_constants_initialize_for_new_map(void);
void physics_constants_dispose_from_old_map(void);

#endif // PHYSICS_PHYSICS_CONSTANTS_H
