#ifndef EFFECTS_PARTICLE_LOCATION_H
#define EFFECTS_PARTICLE_LOCATION_H

// Functions defined in tu/blamlibxbox/particle_location.cpp that other units call.

extern "C" void particle_location_delete(unsigned long location_index);
extern "C" bool g_particle_location_type_created[];

void __tls_set_g_particle_location_data_array_allocator(void *allocation);

template<long, void (*)(void *), void (*)(void *), void (*)(void *)>
class t_restricted_allocation_manager;
extern t_restricted_allocation_manager<2, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_particle_location_data_array_allocator> g_particle_location_data_array_allocator;

struct s_particle_location_datum;
class c_particle_state_list;

extern "C" int particle_location_update(s_particle_location_datum *location, float delta, c_particle_state_list *state_list);
extern "C" long particle_location_new(unsigned char type, long system_index, unsigned long flags);

#endif // EFFECTS_PARTICLE_LOCATION_H
