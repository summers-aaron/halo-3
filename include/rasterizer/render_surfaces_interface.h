#ifndef RASTERIZER_RENDER_SURFACES_INTERFACE_H
#define RASTERIZER_RENDER_SURFACES_INTERFACE_H

#include "rasterizer/rasterizer_render_target_types.h"
#include "rasterizer/rasterizer_surface_globals.h"

class c_render_surfaces_interface
{
    static bool m_initialized;
public:
    static __forceinline c_render_surface *get_render_surface(c_rasterizer::e_surface surface)
    {
        return g_rasterizer_surface_groups[surface].get_specialized_surface(surface);
    }
    static __forceinline bool &initialized() { return m_initialized; }
};

#endif // RASTERIZER_RENDER_SURFACES_INTERFACE_H
