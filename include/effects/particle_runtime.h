#ifndef INCLUDE_EFFECTS_PARTICLE_RUNTIME_H
#define INCLUDE_EFFECTS_PARTICLE_RUNTIME_H

class c_particle;
class c_particle_location;
class c_particle_state_list;

extern "C" int particle_location_update(::c_particle_location *location, float delta, c_particle_state_list *state_list);
extern "C" void particle_state_list_get_state_values(
    const c_particle *particle, long unused, unsigned char state_a, unsigned char state_b, float *value_a, float *value_b);

#endif // INCLUDE_EFFECTS_PARTICLE_RUNTIME_H
