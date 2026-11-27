// blamlibxbox/bsp3d.obj: from the records 0x9bc18
// @flags /TP /O2
// @address 0x9bc18 ?bsp3d_test_point@@YAJPBUbsp3d@@JPBTreal_point3d@@@Z
#include <ppcintrinsics.h>

#include "math/real_math.h"
#include "physics/bsp3d.h"
#include "render/render_transparents.h"
#include "tag_files/tag_types.h"
struct bsp3d {
    int node_count;
    TAG_POINTER(bsp3d_node) nodes;
    TAG_POINTER(void) node_definition;
    int plane_count;
    TAG_POINTER(real_plane3d) planes;
};
#define BSP3D_CHILD_INDEX(child) ((child) & 0x7fffff)
#define BSP3D_CHILD_IS_LEAF(child) ((child) & 0x800000)

// bsp3d_test_point: the leaf containing the point (-1 outside), prefetching both children of each node
long bsp3d_test_point(const bsp3d *bsp, long unused, const real_point3d *point)
{
    bsp3d_node *nodes = bsp->nodes;
    const real_plane3d *planes = bsp->planes;
    int child = 0;
    long *unused_address = &unused;
    (void)unused_address;
    do {
        bsp3d_node node = nodes[BSP3D_CHILD_INDEX(child)];
        int back = (int)node.back_child;
        int front = (int)node.front_child;
        __dcbt(BSP3D_CHILD_INDEX(back) * sizeof(bsp3d_node), nodes);
        __dcbt(BSP3D_CHILD_INDEX(front) * sizeof(bsp3d_node), nodes);
// match: 0x9bc18: keeps the plane-distance call at 0x9bc78 inside the node loop.
#pragma inline_depth(0)
        child = plane3d_distance_to_point(&planes[node.plane], point) < 0.0f ? back : front;
    } while (!BSP3D_CHILD_IS_LEAF(child));
    return child != -1 ? BSP3D_CHILD_INDEX(child) : -1;
}
