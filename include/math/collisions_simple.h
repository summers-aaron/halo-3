#ifndef MATH_COLLISIONS_SIMPLE_H
#define MATH_COLLISIONS_SIMPLE_H

// Functions defined in tu/blamlibxbox/collisions_simple.cpp that other units call.

struct real_matrix4x3;
union real_point3d;
union real_vector3d;

bool collision_point_in_sphere(const real_point3d *point, const real_point3d *sphere_center, float radius);
bool collision_point_in_cylinder(const real_point3d *point, const real_matrix4x3 *cylinder_matrix, float radius, float height, float *distance);
bool collision_point_in_pill(const real_point3d *point, const real_matrix4x3 *pill_matrix, float radius, float height, float *);
bool collision_point_in_cone(const real_point3d *p, const real_matrix4x3 *cone, const real_vector3d *axis, float length,
	float radius, float *out_length, float *out_radius);
bool collision_point_in_box(const real_point3d *point, const real_matrix4x3 *box_matrix, float width, float length, float height);

#endif // MATH_COLLISIONS_SIMPLE_H
