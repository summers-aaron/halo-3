#ifndef EFFECTS_PARTICLE_LOCATION_RUNTIME_API_H
#define EFFECTS_PARTICLE_LOCATION_RUNTIME_API_H
#include "effects/particle_array_globals.h"
#include "effects/particle_location_runtime_types.h"
extern "C" {
#if !defined(_XBOX) || defined(__clang__)
unsigned short particle_emitter_clear_particles(class c_particle_emitter *emitter);
#else
// Xbox codegen subtracts the result directly from the active particle count.
int particle_emitter_clear_particles(class c_particle_emitter *emitter);
#endif
}
static __forceinline s_data_array *particle_location_gpu_data_get()
{
    return g_particle_gpu_block_data;
}
#endif
