#ifndef EFFECTS_PARTICLE_LOCATION_RUNTIME_API_H
#define EFFECTS_PARTICLE_LOCATION_RUNTIME_API_H
#include "effects/particle_array_globals.h"
#include "effects/particle_location_runtime_types.h"
extern "C" {
// PC port: particle_emitter.cpp defines extern "C" unsigned short; declaring int reads dirty upper %eax on i686.
unsigned short particle_emitter_clear_particles(class c_particle_emitter *emitter);
}
static __forceinline s_data_array *particle_location_gpu_data_get()
{
    return g_particle_gpu_block_data;
}
#endif
