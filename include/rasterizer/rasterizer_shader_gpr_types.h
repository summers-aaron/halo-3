#ifndef RASTERIZER_RASTERIZER_SHADER_GPR_TYPES_H
#define RASTERIZER_RASTERIZER_SHADER_GPR_TYPES_H

// The current limits are stored together with the configured vertex and pixel limits.
struct s_rasterizer_shader_gpr_globals
{
    unsigned long maximum[2];
    unsigned long configured_vertex;
    unsigned long configured_pixel;
    __forceinline unsigned long &operator[](long index) { return maximum[index]; }
};

extern "C" long g_rasterizer_gpr_allocation;

#endif
