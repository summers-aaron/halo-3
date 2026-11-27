#ifndef MEMORY_TLS_EFFECT_DATA_H
#define MEMORY_TLS_EFFECT_DATA_H
#include "effects/effect_array_globals.h"
#include "effects/particle.h"

struct s_data_array;

// These slots belong to
// particles (particle_system.cpp) and contrails (contrails.cpp).
static __forceinline s_data_array **tls_particle_data_address()
{
    return &g_particle_data;
}

static __forceinline s_data_array **tls_contrail_data_address()
{
    return &g_contrail_system_data;
}

#endif
