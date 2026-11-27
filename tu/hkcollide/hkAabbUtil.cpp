// hkcollide/hkAabbUtil.obj: from the records 0x6262d0
// @flags /TP /O2 /Oc
// @address 0x6262d0 ?compute_bounds@phys_aabb_util@@SAXPBMHHAAVphys_aabb@@@Z
#include <vectorintrinsics.h>
#include "phys/phys_aabb.h"
#include "phys/phys_aabb_util.h"
void phys_aabb_util::compute_bounds(const float *vertices, int vertex_count, int vertex_stride, phys_aabb &aabb) {
    phys_vec4 minimum;
    minimum.load_xyz(vertices);
    phys_vec4 maximum;
    maximum = minimum;
    for (int vertex_index = 1; vertex_index < vertex_count; ++vertex_index) {
        phys_vec4 vertex;
        vertex.load_xyz(phys_add_byte_offset_const(vertices, vertex_index * vertex_stride));
        minimum.set_min4(minimum, vertex);
        maximum.set_max4(maximum, vertex);
    }
    aabb.m_min_limit.set_xyz0(minimum);
    aabb.m_max_limit.set_xyz0(maximum);
}

