// blamlibxbox/bsp2d.obj: from the records 0xba938
// @compiler xdk6130
// @flags /TP /O2 /Oc
// @address 0xba938 ?bsp2d_test_point@@YAJPBUbsp2d@@PBTreal_point2d@@J@Z

#include "math/real_math.h"
#include "physics/bsp2d.h"
#include "tag_files/tag_types.h"
struct bsp2d
{
    long node_count;
    TAG_POINTER(bsp2d_node) nodes;
};

long bsp2d_test_point(const bsp2d *bsp, const real_point2d *point, long index)
{
    while (!(index & 0x8000))
    {
        const bsp2d_node *node = &bsp->nodes[index & 0x7fff];
        float distance = plane2d_distance_to_point(&node->plane, point);
        index = node->children[distance >= 0.0f];
    }
    return index != -1 ? (index & 0x7fff) : -1;
}
