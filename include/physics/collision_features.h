#ifndef PHYSICS_COLLISION_FEATURES_H
#define PHYSICS_COLLISION_FEATURES_H

// Functions defined in tu/blamlibxbox/collision_features.cpp that other units call.

struct collision_bsp;
struct collision_bsp_test_sphere_result;
struct collision_feature_list;
struct collision_plane;
struct real_matrix4x3;
struct real_plane3d;
union real_point3d;
union real_vector3d;

void collision_features_from_point(const real_point3d *point, float height, float radius, long user0, unsigned long user1, long user2,
    long user3, unsigned char user4, unsigned char user5, short user6, collision_feature_list *list);
void collision_features_from_polygon(short point_count, const real_point3d *points, const real_plane3d *plane, float height, float value,
    long user0, unsigned long user1, long user2, long user3, unsigned char user4, unsigned char user5, short user6, collision_feature_list *list);
void collision_bsp_get_features_in_sphere(struct collision_bsp const * bsp, struct collision_bsp_test_sphere_result const * result,
    struct real_matrix4x3 const * matrix, float radius, float height, long object_index, unsigned long flags,
    long instance_index, long structure_bsp_index, struct collision_feature_list * features);
bool collision_features_test_vector(struct collision_feature_list const * list, union real_point3d const * point,
    union real_vector3d const * vector, struct collision_plane * result);
extern "C" void collision_features_from_line(const real_point3d *point, const real_vector3d *vector, float radius, float height, long object_index,
    unsigned long flags, long instance_index, long edge_left, unsigned char breakable,
    unsigned char left_flags, short material, collision_feature_list *features);

#endif // PHYSICS_COLLISION_FEATURES_H
