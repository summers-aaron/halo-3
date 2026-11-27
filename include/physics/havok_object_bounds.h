#ifndef PHYSICS_HAVOK_OBJECT_BOUNDS_H
#define PHYSICS_HAVOK_OBJECT_BOUNDS_H

union real_rectangle3d;

// Functions defined in tu/blamlibxbox/havok_utilities.cpp.
extern "C" void havok_update_bodies_in_bounds(unsigned long filter_info, const real_rectangle3d *bounds);
extern "C" int havok_get_object_bounds_and_group(long object_index, real_rectangle3d *rectangle);

#endif // PHYSICS_HAVOK_OBJECT_BOUNDS_H
