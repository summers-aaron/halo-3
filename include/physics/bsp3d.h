#ifndef PHYSICS_BSP3D_H
#define PHYSICS_BSP3D_H

// Functions defined in tu/blamlibxbox/bsp3d.cpp that other units call, and the node they walk: one 64-bit word with front_child in
// bits 63..40, back_child in 39..16 and plane in 15..0; the console allocates bit-fields from the most significant
// bit, x86 from the least, so the declaration order is reversed for clang.

struct bsp3d;
union real_point3d;

struct bsp3d_node {
#if defined(_XBOX) && !defined(__clang__)
    __int64 front_child : 24;
    __int64 back_child : 24;
    __int64 plane : 16;
#else
    __int64 plane : 16;
    __int64 back_child : 24;
    __int64 front_child : 24;
#endif
};

long bsp3d_test_point(const bsp3d *bsp, long unused, const real_point3d *point);

#endif // PHYSICS_BSP3D_H
