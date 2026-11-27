#ifndef CUBEMAPS_CUBEMAP_SAMPLE_H
#define CUBEMAPS_CUBEMAP_SAMPLE_H

#include <stddef.h>
#include "objects/object_reference_types.h"
union real_point3d;

// These cluster helpers have no asserted header in March; keep them beside their first shared user.
static __forceinline s_cluster_reference *cluster_reference_invalidate(s_cluster_reference *reference)
{
    *(short *)reference = -1;
    return reference;
}

static __forceinline s_cluster_reference invalid_cluster_reference_get()
{
    s_cluster_reference reference;
    return *cluster_reference_invalidate(&reference);
}

class c_dynamic_cubemap_sample
{
public:
    static long get_cluster_cubemap_index(s_cluster_reference cluster_reference);
    bool search_for_cubemap_sample_in_cluster(s_cluster_reference cluster_reference, const real_point3d *point, float tolerance);
    bool update(const real_point3d *current_position, float blend);

    void set_defaults()
    {
        m_blend_factor = 1.0f;
        m_current.cluster_reference = invalid_cluster_reference_get();
        m_current.cubemap_index = -1;
        m_current.next_cubemap_index = -1;
        m_last.cluster_reference = invalid_cluster_reference_get();
        m_last.cubemap_index = -1;
        m_last.next_cubemap_index = -1;
    }

    struct s_sample
    {
        s_cluster_reference cluster_reference;
        short cubemap_index;
        short next_cubemap_index;
    };

    s_sample m_current;
    s_sample m_last;
    float m_blend_factor;
};

typedef char cubemap_sample_size[(sizeof(c_dynamic_cubemap_sample) == 0x10) ? 1 : -1];
typedef char cubemap_current_offset[(offsetof(c_dynamic_cubemap_sample, m_current) == 0) ? 1 : -1];
typedef char cubemap_last_offset[(offsetof(c_dynamic_cubemap_sample, m_last) == 6) ? 1 : -1];
typedef char cubemap_blend_offset[(offsetof(c_dynamic_cubemap_sample, m_blend_factor) == 0xc) ? 1 : -1];

#endif // CUBEMAPS_CUBEMAP_SAMPLE_H
