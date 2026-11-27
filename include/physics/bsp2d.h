#ifndef PHYSICS_BSP2D_H
#define PHYSICS_BSP2D_H

// The 2D BSP node, and the functions defined in tu/blamlibxbox/bsp2d.cpp that other units call.

#include "math/real_math_types.h"

struct bsp2d_node
{
    real_plane2d plane;
    short children[2];
};
typedef char bsp2d_node_size_check[(sizeof(bsp2d_node) == 0x10) ? 1 : -1];

struct bsp2d;

long bsp2d_test_point(const bsp2d *bsp, const real_point2d *point, long index);

#endif // PHYSICS_BSP2D_H
