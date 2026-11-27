#ifndef RENDER_METHODS_RENDER_METHOD_DATA_H
#define RENDER_METHODS_RENDER_METHOD_DATA_H

#include "objects/cluster_reference.h"

class c_render_method_data
{
public:
    static unsigned long m_change_colors[4];
    static bool m_valid;
    static float m_cubemap_blend_factor;
    static float (*m_context_interface)(long, long, long);
    static long m_lightmap_bsp_index;
    static s_cluster_reference m_lightmap_cluster_reference;
    static long m_lightmap_instance_index;
    static s_cluster_reference m_cubemap_dynamic_0_cluster_reference;
    static long m_cubemap_dynamic_0_index;
    static s_cluster_reference m_cubemap_dynamic_1_cluster_reference;
    static long m_cubemap_dynamic_1_index;
    static long m_context;
};

#endif // RENDER_METHODS_RENDER_METHOD_DATA_H
