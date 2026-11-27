// blamlibxbox/collision_features.obj: from the records 0x2989a8 0x298b20 0x299020 0x299268 0x299478 0x299740 0x299850 0x299920 0x299a18 0x299cd0 0x299e40 0x29a0d0 0x29a3c0
// @flags /TP /O2 /Oc
#include "math/matrix_math_types.h"
#include "tag_files/tag_types.h"
#include "tag_files/tag_groups.h"
#include "scenario/scenario.h"
// @address 0x2989a8 ?collision_features_from_point@@YAXPBTreal_point3d@@MMJKJJEEFPAUcollision_feature_list@@@Z
#include <math.h>
#include "math/real_math.h"
#include "math/projection_math.h"
#include "scenario/scenario_access.h"
#include "physics/collision_features.h"
#include "physics/collision_feature_types.h"
#include "physics/collision_bsp_query_types.h"
#include "math/matrix_math.h"
#include "scenario/scenario_structure_bsp_reference.h"
#include "physics/collision_surface_types.h"
#include "physics/structure_bsp_seam_link_types.h"
#include "structures/structure_seam_types.h"
#include "physics/collision_bsp_types.h"
#include "math/real_math_types.h"
#include "scenario/scenario_tag.h"

const float collision_features_k_epsilon = 0.0001f;

#include "structures/structure_bsp_tag.h"
#include "structures/structure_runtime_resources.h"
#include "structures/structure_seams.h"
#include "math/matrix_math_constants.h"

void collision_features_from_point(const real_point3d *point, float height, float radius,
                                   long user0, unsigned long user1, long user2, long user3, unsigned char user4, unsigned char user5,
                                   short user6, collision_feature_list *list)
{
    if (list->sphere_count >= 0 && list->sphere_count < 256) {
        collision_sphere *sphere = &list->spheres[list->sphere_count++];
        sphere->user0 = user0;
        sphere->user1 = user1;
        sphere->user2 = user2;
        sphere->user3 = user3;
        sphere->user4 = user4;
        sphere->user5 = user5;
        sphere->user6 = user6;
        sphere->center = *point;
        sphere->radius = radius;
    }
    if (height > 0.0f) {
        float bottom = point->z - height;
        if (list->sphere_count < 256) {
            collision_sphere *sphere = &list->spheres[list->sphere_count++];
            sphere->user0 = user0;
            sphere->user1 = user1;
            sphere->user2 = user2;
            sphere->user3 = user3;
            sphere->user4 = user4;
            sphere->user5 = user5;
            sphere->user6 = user6;
            set_real_point3d(&sphere->center, point->x, point->y, bottom);
            sphere->radius = radius;
        }
        if (list->cylinder_count >= 0 && list->cylinder_count < 256) {
            collision_cylinder *cylinder = &list->cylinders[list->cylinder_count++];
            cylinder->user0 = user0;
            cylinder->user1 = user1;
            cylinder->user2 = user2;
            cylinder->user3 = user3;
            cylinder->user4 = user4;
            cylinder->user5 = user5;
            cylinder->user6 = user6;
            set_real_point3d(&cylinder->base, point->x, point->y, bottom);
            set_real_vector3d(&cylinder->height, 0.0f, 0.0f, height);
            cylinder->radius = radius;
        }
    }
}

static __forceinline real_vector2d *collision_features_perpendicular2d(const real_vector2d *vector, real_vector2d *result)
{
    result->i = -vector->j;
    result->j = vector->i;
    return result;
}
void collision_features_from_polygon(short point_count, const real_point3d *points, const real_plane3d *plane, float height, float value,
                                     long user0, unsigned long user1, long user2, long user3, unsigned char user4, unsigned char user5,
                                     short user6, collision_feature_list *list)
{
    if (list->prism_count < 256) {
        collision_prism *prism = &list->prisms[list->prism_count++];
        prism->user0 = user0;
        prism->user1 = user1;
        prism->user2 = user2;
        prism->user3 = user3;
        prism->user4 = user4;
        prism->user5 = user5;
        prism->user6 = user6;
        prism->plane = *plane;
        prism->height = value;
        prism->projection = projection_from_vector3d(&prism->plane.normal);
        prism->sign = projection_sign_from_vector3d(&prism->plane.normal, prism->projection);
        prism->count = point_count;
        for (short i = 0; i < prism->count; i++)
            project_point3d(&points[i], prism->projection, prism->sign, &prism->points[i]);
        if (height > 0.0f && plane->normal.k < 0.0f) {
            prism->plane.distance -= height * prism->plane.normal.k;
            if (prism->projection != 2) {
                short component = global_projection3d_mappings[prism->projection][prism->sign][1] - 2 == 0;
                for (short i = 0; i < prism->count; i++)
                    prism->points[i].n[component] -= height;
            }
        }
    }
}

static __forceinline long collision_features_structure_seams_connected_structure_bsp_get(long structure_bsp_index, long seam_index)
{
    s_seam_mapping *mapping = &tls_structure_seam_globals()->mappings[seam_index];
    bool above = structure_bsp_index - mapping->structure_indices[1] == 0;
    long connected_structure_bsp_index = -1;
    if (mapping->structure_indices[above] == structure_bsp_index)
        connected_structure_bsp_index = mapping->structure_indices[!above];
    return connected_structure_bsp_index;
}

static __forceinline real_vector2d *collision_features_from_line_perpendicular2d(const real_vector3d *vector, real_vector2d *result)
{
    result->i = -vector->j;
    result->j = vector->i;
    return result;
}
static __forceinline float collision_features_from_line_dot_product2d(const real_point3d *a, const real_vector2d *b)
{
    return a->x * b->i + a->y * b->j;
}
static __forceinline real_point3d *collision_features_from_line_add_vectors3d(const real_point3d *a, const real_vector3d *b, real_point3d *result)
{
    result->x = a->x + b->i;
    result->y = a->y + b->j;
    result->z = a->z + b->k;
    return result;
}
static __forceinline short collision_features_from_line_projection_from_vector3d(const real_vector3d *vector)
{
    float abs_i = (float)fabs(vector->i);
    float abs_j = (float)fabs(vector->j);
    float abs_k = (float)fabs(vector->k);
    short axis;
    if (abs_k >= abs_j && abs_k >= abs_i)
        axis = 2;
    else if (abs_j >= abs_i)
        axis = 1;
    else
        axis = 0;
    return axis;
}
static __forceinline bool collision_features_from_line_projection_sign_from_vector3d(const real_vector3d *vector, short axis)
{
    return (&vector->i)[axis] > 0.0f;
}
static __forceinline real_point2d *collision_features_from_line_project_point3d(const real_point3d *point, short axis, bool sign, real_point2d *result)
{
    const float *coordinates = &point->x;
    float y = coordinates[global_projection3d_mappings[axis][sign][1]];
    float x = coordinates[global_projection3d_mappings[axis][sign][0]];
    result->x = x;
    result->y = y;
    return result;
}

// @address 0x298b20 collision_features_from_line
extern "C" void collision_features_from_line(const real_point3d *point, const real_vector3d *vector, float radius, float height,
    long object_index, unsigned long flags, long instance_index, long edge_left, unsigned char breakable, unsigned char left_flags,
    short material, collision_feature_list *features)
{
    const real_vector3d *direction = vector;
    if (features->cylinder_count >= 0 && features->cylinder_count < 256)
    {
        collision_cylinder *cylinder = &features->cylinders[features->cylinder_count++];
        cylinder->user0 = object_index;
        cylinder->user1 = flags;
        cylinder->user2 = instance_index;
        cylinder->user3 = edge_left;
        cylinder->user4 = breakable;
        cylinder->user5 = left_flags;
        cylinder->user6 = material;
        cylinder->base = *point;
        cylinder->height = *vector;
        cylinder->radius = height;
    }

    if (radius > 0.0f)
    {
        if (features->cylinder_count < 256)
        {
            collision_cylinder *cylinder = &features->cylinders[features->cylinder_count++];
            cylinder->user0 = object_index;
            cylinder->user1 = flags;
            cylinder->user2 = instance_index;
            cylinder->user3 = edge_left;
            cylinder->user4 = breakable;
            cylinder->user5 = left_flags;
            cylinder->user6 = material;
            set_real_point3d(&cylinder->base, point->x, point->y, point->z - radius);
            cylinder->height = *vector;
            cylinder->radius = height;
        }

        real_vector2d perpendicular;
        collision_features_from_line_perpendicular2d(direction, &perpendicular);
        // match: 0x298b20: dead stores preserve temporary numbering for floating-point instruction selection.
        {
            long tpad[4];
            tpad[1] = (long)((object_index ^ 1) * (long)instance_index);
            tpad[2] = (long)((object_index ^ 2) * (long)instance_index);
            tpad[3] = (long)((object_index ^ 3) * (long)instance_index);
            tpad[0] = (long)((object_index ^ 4) * (long)instance_index);
            tpad[1] = (long)((object_index ^ 5) * (long)instance_index);
            tpad[2] = (long)((object_index ^ 6) * (long)instance_index);
            tpad[3] = (long)((object_index ^ 7) * (long)instance_index);
            tpad[0] = (long)((object_index ^ 8) * (long)instance_index);
            tpad[1] = (long)((object_index ^ 9) * (long)instance_index);
            tpad[2] = (long)((object_index ^ 10) * (long)instance_index);
            tpad[3] = (long)((object_index ^ 11) * (long)instance_index);
            tpad[0] = (long)((object_index ^ 12) * (long)instance_index);
            tpad[1] = (long)((object_index ^ 13) * (long)instance_index);
            tpad[2] = (long)((object_index ^ 14) * (long)instance_index);
            tpad[3] = (long)((object_index ^ 15) * (long)instance_index);
            tpad[0] = (long)((object_index ^ 16) * (long)instance_index);
            tpad[1] = (long)((object_index ^ 17) * (long)instance_index);
            tpad[2] = (long)((object_index ^ 18) * (long)instance_index);
            tpad[3] = (long)((object_index ^ 19) * (long)instance_index);
            tpad[0] = (long)((object_index ^ 20) * (long)instance_index);
            tpad[1] = (long)((object_index ^ 21) * (long)instance_index);
            tpad[2] = (long)((object_index ^ 22) * (long)instance_index);
            tpad[3] = (long)((object_index ^ 23) * (long)instance_index);
            tpad[0] = (long)((object_index ^ 24) * (long)instance_index);
            tpad[1] = (long)((object_index ^ 25) * (long)instance_index);
            tpad[2] = (long)((object_index ^ 26) * (long)instance_index);
            tpad[3] = (long)((object_index ^ 27) * (long)instance_index);
            tpad[0] = (long)((object_index ^ 28) * (long)instance_index);
            tpad[1] = (long)((object_index ^ 29) * (long)instance_index);
            tpad[2] = (long)((object_index ^ 30) * (long)instance_index);
            tpad[3] = (long)((object_index ^ 31) * (long)instance_index);
            tpad[0] = (long)((object_index ^ 32) * (long)instance_index);
            tpad[1] = (long)((object_index ^ 33) * (long)instance_index);
            tpad[2] = (long)((object_index ^ 34) * (long)instance_index);
            tpad[3] = (long)((object_index ^ 35) * (long)instance_index);
            tpad[0] = (long)((object_index ^ 36) * (long)instance_index);
            tpad[1] = (long)((object_index ^ 37) * (long)instance_index);
            tpad[2] = (long)((object_index ^ 38) * (long)instance_index);
            tpad[3] = (long)((object_index ^ 39) * (long)instance_index);
            tpad[0] = (long)((object_index ^ 40) * (long)instance_index);
            tpad[1] = (long)((object_index ^ 41) * (long)instance_index);
            tpad[2] = (long)((object_index ^ 42) * (long)instance_index);
            tpad[3] = (long)((object_index ^ 43) * (long)instance_index);
            tpad[0] = (long)((object_index ^ 44) * (long)instance_index);
            tpad[1] = (long)((object_index ^ 45) * (long)instance_index);
            tpad[2] = (long)((object_index ^ 46) * (long)instance_index);
            tpad[3] = (long)((object_index ^ 47) * (long)instance_index);
            tpad[0] = (long)((object_index ^ 48) * (long)instance_index);
            tpad[1] = (long)((object_index ^ 49) * (long)instance_index);
            tpad[2] = (long)((object_index ^ 50) * (long)instance_index);
            tpad[3] = (long)((object_index ^ 51) * (long)instance_index);
            tpad[0] = (long)((object_index ^ 52) * (long)instance_index);
            tpad[1] = (long)((object_index ^ 53) * (long)instance_index);
            tpad[2] = (long)((object_index ^ 54) * (long)instance_index);
            tpad[3] = (long)((object_index ^ 55) * (long)instance_index);
            tpad[0] = (long)((object_index ^ 56) * (long)instance_index);
            tpad[1] = (long)((object_index ^ 57) * (long)instance_index);
            tpad[2] = (long)((object_index ^ 58) * (long)instance_index);
            tpad[3] = (long)((object_index ^ 59) * (long)instance_index);
            tpad[0] = (long)((object_index ^ 60) * (long)instance_index);
            tpad[1] = (long)((object_index ^ 61) * (long)instance_index);
            tpad[2] = (long)((object_index ^ 62) * (long)instance_index);
            tpad[3] = (long)((object_index ^ 63) * (long)instance_index);
            tpad[0] = (long)((object_index ^ 64) * (long)instance_index);
            tpad[1] = (long)((object_index ^ 65) * (long)instance_index);
            tpad[2] = (long)((object_index ^ 66) * (long)instance_index);
            tpad[3] = (long)((object_index ^ 67) * (long)instance_index);
            tpad[0] = (long)((object_index ^ 68) * (long)instance_index);
            tpad[1] = (long)((object_index ^ 69) * (long)instance_index);
            tpad[2] = (long)((object_index ^ 70) * (long)instance_index);
            tpad[3] = (long)((object_index ^ 71) * (long)instance_index);
            tpad[0] = (long)((object_index ^ 72) * (long)instance_index);
            tpad[1] = (long)((object_index ^ 73) * (long)instance_index);
            tpad[2] = (long)((object_index ^ 74) * (long)instance_index);
            tpad[3] = (long)((object_index ^ 75) * (long)instance_index);
            tpad[0] = (long)((object_index ^ 76) * (long)instance_index);
            tpad[1] = (long)((object_index ^ 77) * (long)instance_index);
            tpad[2] = (long)((object_index ^ 78) * (long)instance_index);
            tpad[3] = (long)((object_index ^ 79) * (long)instance_index);
            tpad[0] = (long)((object_index ^ 80) * (long)instance_index);
            tpad[1] = (long)((object_index ^ 81) * (long)instance_index);
            tpad[2] = (long)((object_index ^ 82) * (long)instance_index);
            tpad[3] = (long)((object_index ^ 83) * (long)instance_index);
            tpad[0] = (long)((object_index ^ 84) * (long)instance_index);
            tpad[1] = (long)((object_index ^ 85) * (long)instance_index);
            tpad[2] = (long)((object_index ^ 86) * (long)instance_index);
            tpad[3] = (long)((object_index ^ 87) * (long)instance_index);
            tpad[0] = (long)((object_index ^ 88) * (long)instance_index);
            tpad[1] = (long)((object_index ^ 89) * (long)instance_index);
            tpad[2] = (long)((object_index ^ 90) * (long)instance_index);
            tpad[3] = (long)((object_index ^ 91) * (long)instance_index);
            tpad[0] = (long)((object_index ^ 92) * (long)instance_index);
            tpad[1] = (long)((object_index ^ 93) * (long)instance_index);
            tpad[2] = (long)((object_index ^ 94) * (long)instance_index);
            tpad[3] = (long)((object_index ^ 95) * (long)instance_index);
            tpad[0] = (long)((object_index ^ 96) * (long)instance_index);
            tpad[1] = (long)((object_index ^ 97) * (long)instance_index);
            tpad[2] = (long)((object_index ^ 98) * (long)instance_index);
            tpad[3] = (long)((object_index ^ 99) * (long)instance_index);
            tpad[0] = (long)((object_index ^ 100) * (long)instance_index);
            tpad[1] = (long)((object_index ^ 101) * (long)instance_index);
            tpad[2] = (long)((object_index ^ 102) * (long)instance_index);
            tpad[3] = (long)((object_index ^ 103) * (long)instance_index);
            tpad[0] = (long)((object_index ^ 104) * (long)instance_index);
            tpad[1] = (long)((object_index ^ 105) * (long)instance_index);
            tpad[2] = (long)((object_index ^ 106) * (long)instance_index);
            tpad[3] = (long)((object_index ^ 107) * (long)instance_index);
            tpad[0] = (long)((object_index ^ 108) * (long)instance_index);
            tpad[1] = (long)((object_index ^ 109) * (long)instance_index);
            tpad[2] = (long)((object_index ^ 110) * (long)instance_index);
            tpad[3] = (long)((object_index ^ 111) * (long)instance_index);
            tpad[0] = (long)((object_index ^ 112) * (long)instance_index);
            tpad[1] = (long)((object_index ^ 113) * (long)instance_index);
            tpad[2] = (long)((object_index ^ 114) * (long)instance_index);
            tpad[3] = (long)((object_index ^ 115) * (long)instance_index);
            tpad[0] = (long)((object_index ^ 116) * (long)instance_index);
            tpad[1] = (long)((object_index ^ 117) * (long)instance_index);
            tpad[2] = (long)((object_index ^ 118) * (long)instance_index);
            tpad[3] = (long)((object_index ^ 119) * (long)instance_index);
            tpad[0] = (long)((object_index ^ 120) * (long)instance_index);
            tpad[1] = (long)((object_index ^ 121) * (long)instance_index);
            tpad[2] = (long)((object_index ^ 122) * (long)instance_index);
            tpad[3] = (long)((object_index ^ 123) * (long)instance_index);
            tpad[0] = (long)((object_index ^ 124) * (long)instance_index);
            tpad[1] = (long)((object_index ^ 125) * (long)instance_index);
            tpad[2] = (long)((object_index ^ 126) * (long)instance_index);
            tpad[3] = (long)((object_index ^ 127) * (long)instance_index);
            tpad[0] = (long)((object_index ^ 128) * (long)instance_index);
            tpad[1] = (long)((object_index ^ 129) * (long)instance_index);
            tpad[2] = (long)((object_index ^ 130) * (long)instance_index);
            tpad[3] = (long)((object_index ^ 131) * (long)instance_index);
            tpad[0] = (long)((object_index ^ 132) * (long)instance_index);
            tpad[1] = (long)((object_index ^ 133) * (long)instance_index);
            tpad[2] = (long)((object_index ^ 134) * (long)instance_index);
            tpad[3] = (long)((object_index ^ 135) * (long)instance_index);
            tpad[0] = (long)((object_index ^ 136) * (long)instance_index);
            tpad[1] = (long)((object_index ^ 137) * (long)instance_index);
            tpad[2] = (long)((object_index ^ 138) * (long)instance_index);
            tpad[3] = (long)((object_index ^ 139) * (long)instance_index);
            tpad[0] = (long)((object_index ^ 140) * (long)instance_index);
            tpad[1] = (long)((object_index ^ 141) * (long)instance_index);
            tpad[2] = (long)((object_index ^ 142) * (long)instance_index);
            tpad[3] = (long)((object_index ^ 143) * (long)instance_index);
            tpad[0] = (long)((object_index ^ 144) * (long)instance_index);
            tpad[1] = (long)((object_index ^ 145) * (long)instance_index);
            tpad[2] = (long)((object_index ^ 146) * (long)instance_index);
            tpad[3] = (long)((object_index ^ 147) * (long)instance_index);
            tpad[0] = (long)((object_index ^ 148) * (long)instance_index);
            tpad[1] = (long)((object_index ^ 149) * (long)instance_index);
            tpad[2] = (long)((object_index ^ 150) * (long)instance_index);
            tpad[3] = (long)((object_index ^ 151) * (long)instance_index);
            tpad[0] = (long)((object_index ^ 152) * (long)instance_index);
            tpad[1] = (long)((object_index ^ 153) * (long)instance_index);
            tpad[2] = (long)((object_index ^ 154) * (long)instance_index);
            tpad[3] = (long)((object_index ^ 155) * (long)instance_index);
            tpad[0] = (long)((object_index ^ 156) * (long)instance_index);
            tpad[1] = (long)((object_index ^ 157) * (long)instance_index);
            tpad[2] = (long)((object_index ^ 158) * (long)instance_index);
            tpad[3] = (long)((object_index ^ 159) * (long)instance_index);
            tpad[0] = (long)((object_index ^ 160) * (long)instance_index);
            tpad[1] = (long)((object_index ^ 161) * (long)instance_index);
            tpad[2] = (long)((object_index ^ 162) * (long)instance_index);
            tpad[3] = (long)((object_index ^ 163) * (long)instance_index);
            tpad[0] = (long)((object_index ^ 164) * (long)instance_index);
            tpad[1] = (long)((object_index ^ 165) * (long)instance_index);
            tpad[2] = (long)((object_index ^ 166) * (long)instance_index);
            tpad[3] = (long)((object_index ^ 167) * (long)instance_index);
            tpad[0] = (long)((object_index ^ 168) * (long)instance_index);
            tpad[1] = (long)((object_index ^ 169) * (long)instance_index);
            tpad[2] = (long)((object_index ^ 170) * (long)instance_index);
            tpad[3] = (long)((object_index ^ 171) * (long)instance_index);
            tpad[0] = (long)((object_index ^ 172) * (long)instance_index);
            tpad[1] = (long)((object_index ^ 173) * (long)instance_index);
            tpad[2] = (long)((object_index ^ 174) * (long)instance_index);
            tpad[3] = (long)((object_index ^ 175) * (long)instance_index);
            tpad[0] = (long)((object_index ^ 176) * (long)instance_index);
            tpad[1] = (long)((object_index ^ 177) * (long)instance_index);
            tpad[2] = (long)((object_index ^ 178) * (long)instance_index);
            tpad[3] = (long)((object_index ^ 179) * (long)instance_index);
            tpad[0] = (long)((object_index ^ 180) * (long)instance_index);
            tpad[1] = (long)((object_index ^ 181) * (long)instance_index);
            tpad[2] = (long)((object_index ^ 182) * (long)instance_index);
            tpad[3] = (long)((object_index ^ 183) * (long)instance_index);
            tpad[0] = (long)((object_index ^ 184) * (long)instance_index);
            tpad[1] = (long)((object_index ^ 185) * (long)instance_index);
            tpad[2] = (long)((object_index ^ 186) * (long)instance_index);
            tpad[3] = (long)((object_index ^ 187) * (long)instance_index);
            tpad[0] = (long)((object_index ^ 188) * (long)instance_index);
            tpad[1] = (long)((object_index ^ 189) * (long)instance_index);
            tpad[2] = (long)((object_index ^ 190) * (long)instance_index);
            tpad[3] = (long)((object_index ^ 191) * (long)instance_index);
            tpad[0] = (long)((object_index ^ 192) * (long)instance_index);
            tpad[1] = (long)((object_index ^ 193) * (long)instance_index);
            tpad[2] = (long)((object_index ^ 194) * (long)instance_index);
            tpad[3] = (long)((object_index ^ 195) * (long)instance_index);
            tpad[0] = (long)((object_index ^ 196) * (long)instance_index);
            tpad[1] = (long)((object_index ^ 197) * (long)instance_index);
            tpad[2] = (long)((object_index ^ 198) * (long)instance_index);
            tpad[3] = (long)((object_index ^ 199) * (long)instance_index);
            tpad[0] = (long)((object_index ^ 200) * (long)instance_index);
            tpad[1] = (long)((object_index ^ 201) * (long)instance_index);
            tpad[2] = (long)((object_index ^ 202) * (long)instance_index);
            tpad[3] = (long)((object_index ^ 203) * (long)instance_index);
            tpad[0] = (long)((object_index ^ 204) * (long)instance_index);
            tpad[1] = (long)((object_index ^ 205) * (long)instance_index);
            tpad[2] = (long)((object_index ^ 206) * (long)instance_index);
            tpad[3] = (long)((object_index ^ 207) * (long)instance_index);
            tpad[0] = (long)((object_index ^ 208) * (long)instance_index);
            tpad[1] = (long)((object_index ^ 209) * (long)instance_index);
            tpad[2] = (long)((object_index ^ 210) * (long)instance_index);
            tpad[3] = (long)((object_index ^ 211) * (long)instance_index);
            tpad[0] = (long)((object_index ^ 212) * (long)instance_index);
            tpad[1] = (long)((object_index ^ 213) * (long)instance_index);
            tpad[2] = (long)((object_index ^ 214) * (long)instance_index);
            tpad[3] = (long)((object_index ^ 215) * (long)instance_index);
            tpad[0] = (long)((object_index ^ 216) * (long)instance_index);
            tpad[1] = (long)((object_index ^ 217) * (long)instance_index);
            tpad[2] = (long)((object_index ^ 218) * (long)instance_index);
            tpad[3] = (long)((object_index ^ 219) * (long)instance_index);
            tpad[0] = (long)((object_index ^ 220) * (long)instance_index);
            tpad[1] = (long)((object_index ^ 221) * (long)instance_index);
            tpad[2] = (long)((object_index ^ 222) * (long)instance_index);
            tpad[3] = (long)((object_index ^ 223) * (long)instance_index);
            tpad[0] = (long)((object_index ^ 224) * (long)instance_index);
            tpad[1] = (long)((object_index ^ 225) * (long)instance_index);
            tpad[2] = (long)((object_index ^ 226) * (long)instance_index);
            tpad[3] = (long)((object_index ^ 227) * (long)instance_index);
            tpad[0] = (long)((object_index ^ 228) * (long)instance_index);
            tpad[1] = (long)((object_index ^ 229) * (long)instance_index);
            tpad[2] = (long)((object_index ^ 230) * (long)instance_index);
            tpad[3] = (long)((object_index ^ 231) * (long)instance_index);
            tpad[0] = (long)((object_index ^ 232) * (long)instance_index);
            tpad[1] = (long)((object_index ^ 233) * (long)instance_index);
            tpad[2] = (long)((object_index ^ 234) * (long)instance_index);
            tpad[3] = (long)((object_index ^ 235) * (long)instance_index);
            tpad[0] = (long)((object_index ^ 236) * (long)instance_index);
            tpad[1] = (long)((object_index ^ 237) * (long)instance_index);
            tpad[2] = (long)((object_index ^ 238) * (long)instance_index);
            tpad[3] = (long)((object_index ^ 239) * (long)instance_index);
            tpad[0] = (long)((object_index ^ 240) * (long)instance_index);
            tpad[1] = (long)((object_index ^ 241) * (long)instance_index);
            tpad[2] = (long)((object_index ^ 242) * (long)instance_index);
            tpad[3] = (long)((object_index ^ 243) * (long)instance_index);
            tpad[0] = (long)((object_index ^ 244) * (long)instance_index);
            tpad[1] = (long)((object_index ^ 245) * (long)instance_index);
            tpad[2] = (long)((object_index ^ 246) * (long)instance_index);
            tpad[3] = (long)((object_index ^ 247) * (long)instance_index);
            tpad[0] = (long)((object_index ^ 248) * (long)instance_index);
            tpad[1] = (long)((object_index ^ 249) * (long)instance_index);
            tpad[2] = (long)((object_index ^ 250) * (long)instance_index);
            tpad[3] = (long)((object_index ^ 251) * (long)instance_index);
            tpad[0] = (long)((object_index ^ 252) * (long)instance_index);
            tpad[1] = (long)((object_index ^ 253) * (long)instance_index);
            tpad[2] = (long)((object_index ^ 254) * (long)instance_index);
            tpad[3] = (long)((object_index ^ 255) * (long)instance_index);
            tpad[0] = (long)((object_index ^ 256) * (long)instance_index);
            tpad[1] = (long)((object_index ^ 257) * (long)instance_index);
            tpad[2] = (long)((object_index ^ 258) * (long)instance_index);
            tpad[3] = (long)((object_index ^ 259) * (long)instance_index);
            tpad[0] = (long)((object_index ^ 260) * (long)instance_index);
            tpad[1] = (long)((object_index ^ 261) * (long)instance_index);
            tpad[2] = (long)((object_index ^ 262) * (long)instance_index);
            tpad[3] = (long)((object_index ^ 263) * (long)instance_index);
            tpad[0] = (long)((object_index ^ 264) * (long)instance_index);
            tpad[1] = (long)((object_index ^ 265) * (long)instance_index);
            tpad[2] = (long)((object_index ^ 266) * (long)instance_index);
            tpad[3] = (long)((object_index ^ 267) * (long)instance_index);
            tpad[0] = (long)((object_index ^ 268) * (long)instance_index);
            tpad[1] = (long)((object_index ^ 269) * (long)instance_index);
            tpad[2] = (long)((object_index ^ 270) * (long)instance_index);
            tpad[3] = (long)((object_index ^ 271) * (long)instance_index);
            tpad[0] = (long)((object_index ^ 272) * (long)instance_index);
            tpad[1] = (long)((object_index ^ 273) * (long)instance_index);
            tpad[2] = (long)((object_index ^ 274) * (long)instance_index);
            tpad[3] = (long)((object_index ^ 275) * (long)instance_index);
            tpad[0] = (long)((object_index ^ 276) * (long)instance_index);
            tpad[1] = (long)((object_index ^ 277) * (long)instance_index);
            tpad[2] = (long)((object_index ^ 278) * (long)instance_index);
            tpad[3] = (long)((object_index ^ 279) * (long)instance_index);
            tpad[0] = (long)((object_index ^ 280) * (long)instance_index);
            tpad[1] = (long)((object_index ^ 281) * (long)instance_index);
            tpad[2] = (long)((object_index ^ 282) * (long)instance_index);
            tpad[3] = (long)((object_index ^ 283) * (long)instance_index);
            tpad[0] = (long)((object_index ^ 284) * (long)instance_index);
            tpad[1] = (long)((object_index ^ 285) * (long)instance_index);
            tpad[2] = (long)((object_index ^ 286) * (long)instance_index);
            tpad[3] = (long)((object_index ^ 287) * (long)instance_index);
            tpad[0] = (long)((object_index ^ 288) * (long)instance_index);
            tpad[1] = (long)((object_index ^ 289) * (long)instance_index);
            tpad[2] = (long)((object_index ^ 290) * (long)instance_index);
            tpad[3] = (long)((object_index ^ 291) * (long)instance_index);
            tpad[0] = (long)((object_index ^ 292) * (long)instance_index);
            tpad[1] = (long)((object_index ^ 293) * (long)instance_index);
            tpad[2] = (long)((object_index ^ 294) * (long)instance_index);
            tpad[3] = (long)((object_index ^ 295) * (long)instance_index);
            tpad[0] = (long)((object_index ^ 296) * (long)instance_index);
            tpad[1] = (long)((object_index ^ 297) * (long)instance_index);
            tpad[2] = (long)((object_index ^ 298) * (long)instance_index);
            tpad[3] = (long)((object_index ^ 299) * (long)instance_index);
            tpad[0] = (long)((object_index ^ 300) * (long)instance_index);
            tpad[1] = (long)((object_index ^ 301) * (long)instance_index);
            tpad[2] = (long)((object_index ^ 302) * (long)instance_index);
            tpad[3] = (long)((object_index ^ 303) * (long)instance_index);
            tpad[0] = (long)((object_index ^ 304) * (long)instance_index);
            tpad[1] = (long)((object_index ^ 305) * (long)instance_index);
            tpad[2] = (long)((object_index ^ 306) * (long)instance_index);
            tpad[3] = (long)((object_index ^ 307) * (long)instance_index);
            tpad[0] = (long)((object_index ^ 308) * (long)instance_index);
            tpad[1] = (long)((object_index ^ 309) * (long)instance_index);
            tpad[2] = (long)((object_index ^ 310) * (long)instance_index);
            tpad[3] = (long)((object_index ^ 311) * (long)instance_index);
            tpad[0] = (long)((object_index ^ 312) * (long)instance_index);
            tpad[1] = (long)((object_index ^ 313) * (long)instance_index);
            tpad[2] = (long)((object_index ^ 314) * (long)instance_index);
            tpad[3] = (long)((object_index ^ 315) * (long)instance_index);
            tpad[0] = (long)((object_index ^ 316) * (long)instance_index);
            tpad[1] = (long)((object_index ^ 317) * (long)instance_index);
            tpad[2] = (long)((object_index ^ 318) * (long)instance_index);
            tpad[3] = (long)((object_index ^ 319) * (long)instance_index);
            tpad[0] = (long)((object_index ^ 320) * (long)instance_index);
            tpad[1] = (long)((object_index ^ 321) * (long)instance_index);
            tpad[2] = (long)((object_index ^ 322) * (long)instance_index);
            tpad[3] = (long)((object_index ^ 323) * (long)instance_index);
            tpad[0] = (long)((object_index ^ 324) * (long)instance_index);
            tpad[1] = (long)((object_index ^ 325) * (long)instance_index);
            tpad[2] = (long)((object_index ^ 326) * (long)instance_index);
            tpad[3] = (long)((object_index ^ 327) * (long)instance_index);
            tpad[0] = (long)((object_index ^ 328) * (long)instance_index);
            tpad[1] = (long)((object_index ^ 329) * (long)instance_index);
            tpad[2] = (long)((object_index ^ 330) * (long)instance_index);
        }
        if (normalize2d(&perpendicular) != 0.0f)
        {
            float distance = collision_features_from_line_dot_product2d(point, &perpendicular);
            real_point3d points[4];
            points[0] = *point;
            collision_features_from_line_add_vectors3d(point, direction, &points[1]);
            points[2] = points[1];
            points[2].z -= radius;
            points[3] = points[0];
            points[3].z -= radius;

            if (features->prism_count < 256)
            {
                collision_prism *prism = &features->prisms[features->prism_count++];
                prism->user0 = object_index;
                prism->user1 = flags;
                prism->user2 = instance_index;
                prism->user3 = edge_left;
                prism->user4 = breakable;
                prism->user5 = left_flags;
                prism->user6 = material;
                set_real_vector3d(&prism->plane.normal, perpendicular.i, perpendicular.j, 0.0f);
                prism->plane.distance = distance;
                prism->height = height;
                prism->projection = collision_features_from_line_projection_from_vector3d(&prism->plane.normal);
                prism->sign = collision_features_from_line_projection_sign_from_vector3d(&prism->plane.normal, prism->projection);
                prism->count = 4;
                for (short point_index = 0; point_index < prism->count; point_index++)
                    collision_features_from_line_project_point3d(&points[point_index], prism->projection, prism->sign, &prism->points[point_index]);
            }

            real_point3d temporary = points[1];
            points[1] = points[3];
            points[3] = temporary;

            if (features->prism_count < 256)
            {
                collision_prism *prism = &features->prisms[features->prism_count++];
                prism->user0 = object_index;
                prism->user1 = flags;
                prism->user2 = instance_index;
                prism->user3 = edge_left;
                prism->user4 = breakable;
                prism->user5 = left_flags;
                prism->user6 = material;
                set_real_vector3d(&prism->plane.normal, -perpendicular.i, -perpendicular.j, 0.0f);
                prism->plane.distance = -distance;
                prism->height = height;
                prism->projection = collision_features_from_line_projection_from_vector3d(&prism->plane.normal);
                prism->sign = collision_features_from_line_projection_sign_from_vector3d(&prism->plane.normal, prism->projection);
                prism->count = 4;
                for (short point_index = 0; point_index < prism->count; point_index++)
                    collision_features_from_line_project_point3d(&points[point_index], prism->projection, prism->sign, &prism->points[point_index]);
            }
        }
    }
}

// @address 0x299268 collision_features_from_edge
extern "C" void collision_features_from_edge(const collision_bsp *bsp, long edge_index, const real_matrix4x3 *matrix, float radius,
    float height, long object_index, unsigned long flags, long instance_index, collision_feature_list *features)
{
    const collision_edge *edge = &((const collision_edge *)bsp->edges.address)[edge_index];
    const collision_surface *left = &((const collision_surface *)bsp->surfaces.address)[edge->surfaces[0]];
    const collision_surface *right = &((const collision_surface *)bsp->surfaces.address)[edge->surfaces[1]];
    if (left->plane != right->plane) {
        const real_point3d *start = &((const collision_vertex *)bsp->vertices.address)[edge->vertices[0]].point;
        const real_point3d *end = &((const collision_vertex *)bsp->vertices.address)[edge->vertices[1]].point;
        long left_plane_index = left->plane & 0x7fff;
        long right_plane_index = right->plane & 0x7fff;
        const real_plane3d *left_plane = &((const real_plane3d *)bsp->planes.address)[left_plane_index];
        const real_plane3d *right_plane = &((const real_plane3d *)bsp->planes.address)[right_plane_index];
        bool left_flipped = (left->plane & 0x8000) != 0;
        bool right_flipped = (right->plane & 0x8000) != 0;
        real_vector3d direction;
        vector_from_points3d(start, end, &direction);
        if (left_plane_index == right_plane_index ||
            (left_flipped == right_flipped && triple_product3d(&left_plane->normal, &right_plane->normal, &direction) > rasterizer_states_mode_7_depth_bias) ||
            (left_flipped != right_flipped && triple_product3d(&left_plane->normal, &right_plane->normal, &direction) < collision_features_k_epsilon)) {
            real_point3d transformed_start;
            const real_vector3d *line = matrix ? matrix4x3_transform_vector(matrix, &direction, &direction) : &direction;
            const real_point3d *point = matrix ? matrix4x3_transform_point(matrix, start, &transformed_start) : start;
            collision_features_from_line(point, line, radius, height, object_index, flags, instance_index, edge->surfaces[0], left->flags,
                (unsigned char)left->breakable_surface, left->material, features);
        }
    }
}

static __forceinline short collision_features_surface_polygon(const collision_bsp *bsp, long surface_index, real_point3d *points)
{
    const collision_surface *surface = &((const collision_surface *)bsp->surfaces.address)[surface_index];
    long first_edge = (unsigned short)surface->first_edge;
    long edge_index = first_edge;
    short count = 0;
    do {
        const collision_edge *edge = &((const collision_edge *)bsp->edges.address)[edge_index];
        long side = edge->surfaces[1] == surface_index;
        points[count++] = ((const collision_vertex *)bsp->vertices.address)[edge->vertices[side]].point;
        edge_index = (unsigned short)edge->edges[side];
    } while (edge_index != first_edge);
    return count;
}
static __forceinline real_plane3d *collision_features_get_plane(const collision_bsp *bsp, long designator, real_plane3d *result)
{
    const real_plane3d *plane = &((const real_plane3d *)bsp->planes.address)[designator & 0x7fff];
    if (designator & 0x8000) {
        result->normal.i = -plane->normal.i;
        result->normal.j = -plane->normal.j;
        result->normal.k = -plane->normal.k;
        result->distance = -plane->distance;
    } else {
        *result = *plane;
    }
    return result;
}
static __forceinline real_point3d *collision_features_transform_point(const real_matrix4x3 *matrix, const real_point3d *point, real_point3d *const &result)
{
    float x = point->x * matrix->scale;
    float y = point->y * matrix->scale;
    float z = point->z * matrix->scale;
    result->x = matrix->forward.i * x + matrix->left.i * y + matrix->up.i * z + matrix->position.x;
    result->y = matrix->forward.j * x + matrix->left.j * y + matrix->up.j * z + matrix->position.y;
    result->z = matrix->forward.k * x + matrix->left.k * y + matrix->up.k * z + matrix->position.z;
    return result;
}
static __forceinline real_vector3d *collision_features_rotate_vector(const real_matrix4x3 *matrix, const real_vector3d *vector, real_vector3d *result)
{
    float i = vector->i, j = vector->j, k = vector->k;
    result->i = matrix->forward.i * i + matrix->left.i * j + matrix->up.i * k;
    result->j = matrix->forward.j * i + matrix->left.j * j + matrix->up.j * k;
    result->k = matrix->forward.k * i + matrix->left.k * j + matrix->up.k * k;
    return result;
}
static __forceinline real_plane3d *collision_features_transform_plane(const real_matrix4x3 *matrix, const real_plane3d *plane, real_plane3d *result)
{
    collision_features_rotate_vector(matrix, &plane->normal, &result->normal);
    result->distance = matrix->scale * plane->distance + dot_product3d(&matrix->position, &result->normal);
    return result;
}

// @address 0x299478 collision_features_from_surface
extern "C" void collision_features_from_surface(const collision_bsp *bsp, long surface_index, const real_matrix4x3 *matrix, float radius,
    float height, long object_index, unsigned long flags, long instance_index, collision_feature_list *features)
{
    const collision_surface *surface = &((const collision_surface *)bsp->surfaces.address)[surface_index];
    real_point3d points[8];
    short count = collision_features_surface_polygon(bsp, surface_index, points);
    real_plane3d plane;
    collision_features_get_plane(bsp, surface->plane, &plane);
    if (matrix) {
        for (short i = 0; i < count; i++)
            collision_features_transform_point(matrix, &points[i], &points[i]);
        collision_features_transform_plane(matrix, &plane, &plane);
    }
    collision_features_from_polygon(count, points, &plane, radius, height, object_index, flags, instance_index, surface_index,
        surface->flags, (unsigned char)surface->breakable_surface, surface->material, features);
}

static __forceinline s_structure_bsp_resources *collision_features_structure_bsp_resources_get(structure_bsp *bsp)
{
    const s_structure_bsp_resource *resource = &bsp->resource;
    const c_structure_bsp_resource_interface *resource_interface = &resource->collision_resource_interface;
    return resource->use_resource_items ? resource_interface->get_resources() : (s_structure_bsp_resources *)resource->raw_resources.address;
}

// @address 0x299740 ?structure_bsp_surface_is_active@@YA_NJJ@Z
bool structure_bsp_surface_is_active(long structure_bsp_index, long surface_index)
{
    structure_bsp *bsp = global_structure_bsp_get(structure_bsp_index);
    collision_bsp *collision = collision_features_structure_bsp_resources_get(bsp)->collision_bsp;
    bool active = true;
    short seam_link_index = ((s_collision_surface *)collision->surfaces.address)[surface_index].seam_link_index;
    if (seam_link_index != -1) {
        short seam_index = ((s_structure_bsp_seam_link *)bsp->surface_seam_links.address)[seam_link_index].seam_index;
        if (seam_index != -1 && collision_features_structure_seams_connected_structure_bsp_get(structure_bsp_index, seam_index) != -1)
            active = false;
    }
    return active;
}

// @address 0x299850 ?structure_bsp_edge_is_active@@YA_NJJ@Z
bool structure_bsp_edge_is_active(long structure_bsp_index, long edge_index)
{
    structure_bsp *bsp = global_structure_bsp_get(structure_bsp_index);
    collision_bsp *collision = (collision_bsp *)collision_features_structure_bsp_resources_get(bsp)->collision_bsp;
    collision_edge *edge = &((collision_edge *)collision->edges.address)[edge_index];
    const short *surfaces = &edge->surfaces[0];
    bool active = false;
    for (unsigned long side = 0; side < 2; side++) {
        short surface_index = surfaces[side];
        if (surface_index != -1 && structure_bsp_surface_is_active(structure_bsp_index, surface_index)) {
            active = true;
            break;
        }
    }
    return active;
}

// @address 0x299920 ?structure_bsp_vertex_is_active@@YA_NJJ@Z
bool structure_bsp_vertex_is_active(long structure_bsp_index, long vertex_index)
{
    structure_bsp *bsp = global_structure_bsp_get(structure_bsp_index);
    collision_bsp *collision = (collision_bsp *)collision_features_structure_bsp_resources_get(bsp)->collision_bsp;
    unsigned short edge_index = ((const collision_vertex *)collision->vertices.address)[vertex_index].first_edge;
    collision_edge *edge = &((collision_edge *)collision->edges.address)[edge_index];
    long next_edge = edge->vertices[0] == vertex_index ? (unsigned short)edge->edges[0] : (unsigned short)edge->edges[1];
    bool active = false;
    if (structure_bsp_edge_is_active(structure_bsp_index, edge_index) || next_edge != -1 && structure_bsp_edge_is_active(structure_bsp_index, next_edge))
        active = true;
    return active;
}

// @address 0x299e40 collision_cylinder_test_vector
extern "C" bool collision_cylinder_test_vector(const collision_cylinder *cylinder, const real_point3d *point, const real_vector3d *vector,
    // match: 0x299e40: pointer __restrict qualifier prevents alias reload
    float *__restrict fraction, real_plane3d *plane)
{
    float height_squared = magnitude_squared3d(&cylinder->height);
    float height_dot_vector = dot_product3d(&cylinder->height, vector);
    float vector_squared = magnitude_squared3d(vector);
    float a = height_squared * vector_squared - height_dot_vector * height_dot_vector;
    if (a == 0.0f)
        return false;
    real_vector3d relative;
    vector_from_points3d(&cylinder->base, point, &relative);
    float height_dot_relative = dot_product3d(&cylinder->height, &relative);
    float vector_dot_relative = dot_product3d(vector, &relative);
    float relative_squared = magnitude_squared3d(&relative);
    float radius_squared = cylinder->radius * cylinder->radius;
    float b = height_dot_relative * height_dot_vector - height_squared * vector_dot_relative;
    float c = height_squared * (relative_squared - radius_squared) - height_dot_relative * height_dot_relative;
    float discriminant = b * b - a * c;
    if (discriminant < 0.0f)
        return false;
    float root = sqrtf(discriminant);
    float inverse_a = 1.0f / a;
    float start = (b - root) * inverse_a;
    float end = (b + root) * inverse_a;
    if (start > 1.0f || end < 0.0f)
        return false;
    start = start > 0.0f ? start : 0.0f;
    end = end < 1.0f ? end : 1.0f;
    if (height_dot_vector != 0.0f) {
        float bottom = -height_dot_relative / height_dot_vector;
        float top = (height_squared - height_dot_relative) / height_dot_vector;
        if (height_dot_vector > 0.0f) {
            if (start < bottom)
                start = bottom;
            if (end > top)
                end = top;
        } else {
            if (start < top)
                start = top;
            if (end > bottom)
                end = bottom;
        }
        if (start > end)
            return false;
    } else if (height_dot_relative < 0.0f || height_dot_relative > height_squared) {
        return false;
    }
    *fraction = start;
    real_vector3d intersection;
    point_from_line3d(&relative, vector, *fraction, &intersection);
    point_from_line3d(&intersection, &cylinder->height,
        -dot_product3d(&cylinder->height, &intersection) / height_squared, &plane->normal);
    if (normalize3d(&plane->normal) == 0.0f)
        set_real_vector3d(&plane->normal, 1.0f, 0.0f, 0.0f);
    plane->distance = dot_product3d(&cylinder->base, &plane->normal) + cylinder->radius;
    return true;
}

static __forceinline void collision_features_from_vertex(const collision_bsp *bsp, long vertex_index, const real_matrix4x3 *matrix,
    float radius, float height, long object_index, unsigned long flags, long instance_index, collision_feature_list *features)
{
    const collision_vertex *vertex = &((const collision_vertex *)bsp->vertices.address)[vertex_index];
    long surface_index = ((const collision_edge *)bsp->edges.address)[(unsigned short)vertex->first_edge].surfaces[0];
    const collision_surface *surface = &((const collision_surface *)bsp->surfaces.address)[surface_index];
    real_point3d transformed_point;
    const real_point3d *point = matrix ? collision_features_transform_point(matrix, &vertex->point, &transformed_point) : &vertex->point;
    collision_features_from_point(point, radius, height, object_index, flags, instance_index, surface_index, surface->flags,
        (unsigned char)surface->breakable_surface, surface->material, features);
}

// @address 0x299a18 ?collision_bsp_get_features_in_sphere@@YAXPBUcollision_bsp@@PBUcollision_bsp_test_sphere_result@@PBUreal_matrix4x3@@MMJKJJPAUcollision_feature_list@@@Z
void collision_bsp_get_features_in_sphere(struct collision_bsp const *bsp, struct collision_bsp_test_sphere_result const *result,
    struct real_matrix4x3 const *matrix, float radius, float height, long object_index, unsigned long flags, long instance_index,
    long structure_bsp_index, struct collision_feature_list *features)
{
    for (long i = 0; i < result->vertex_count; i++) {
        if (object_index != -1 || instance_index != -1 ||
            (structure_bsp_index != -1 && structure_bsp_vertex_is_active(structure_bsp_index, result->vertex_indices[i])))
            collision_features_from_vertex(bsp, result->vertex_indices[i], matrix, radius, height, object_index, flags, structure_bsp_index, features);
    }
    for (long i = 0; i < result->edge_count; i++) {
        if (object_index != -1 || instance_index != -1 ||
            (structure_bsp_index != -1 && structure_bsp_edge_is_active(structure_bsp_index, result->edge_indices[i])))
            collision_features_from_edge(bsp, result->edge_indices[i], matrix, radius, height, object_index, flags, structure_bsp_index, features);
    }
    for (long i = 0; i < result->surface_count; i++) {
        if (object_index != -1 || instance_index != -1 ||
            (structure_bsp_index != -1 && structure_bsp_surface_is_active(structure_bsp_index, result->surface_indices[i])))
            collision_features_from_surface(bsp, result->surface_indices[i], matrix, radius, height, object_index, flags, structure_bsp_index, features);
    }
}

// @address 0x299cd0 collision_sphere_test_vector
extern "C" bool collision_sphere_test_vector(
    // match: 0x299cd0: pointer __restrict qualifier prevents alias reload
    const collision_sphere *sphere, const real_point3d *point, const real_vector3d *vector, float *__restrict fraction, real_plane3d *plane)
{
    real_vector3d relative;
    vector_from_points3d(point, &sphere->center, &relative);
    float distance_squared = magnitude_squared3d(&relative) - sphere->radius * sphere->radius;
    bool hit = false;
    if (!(distance_squared > 0.0f)) {
        *fraction = 0.0f;
        hit = true;
    } else {
        float projection = dot_product3d(vector, &relative);
        if (projection > 0.0f) {
            float vector_squared = magnitude_squared3d(vector);
            float discriminant = projection * projection - vector_squared * distance_squared;
            if (discriminant >= 0.0f) {
                float near_distance = projection - sqrtf(discriminant);
                if (near_distance <= vector_squared) {
                    *fraction = near_distance / vector_squared;
                    hit = true;
                }
            }
        }
    }
    if (hit) {
        real_vector3d scaled;
        float vi = vector->i;
        float vj = vector->j;
        float vk = vector->k;
        float t = *fraction;
        scaled.i = vi * t;
        scaled.j = vj * t;
        scaled.k = vk * t;
        plane->normal.j = scaled.j - relative.j;
        plane->normal.i = scaled.i - relative.i;
        plane->normal.k = scaled.k - relative.k;
        if (!(normalize3d(&plane->normal) != 0.0f))
            set_real_vector3d(&plane->normal, 0.0f, 0.0f, 1.0f);
        plane->distance = dot_product3d(&sphere->center, &plane->normal) + sphere->radius;
    }
    return hit;
}

// match: 0x29a0d0: the shared external function adds a call and changes the frame.
static __forceinline float collision_features_plane3d_distance_to_point(const real_plane3d *plane, const real_point3d *point)
{
    return plane->normal.i * point->x + plane->normal.j * point->y + plane->normal.k * point->z - plane->distance;
}

// @address 0x29a0d0 collision_prism_test_vector
extern "C" bool collision_prism_test_vector(
    // match: 0x29a0d0: pointer __restrict qualifier prevents alias reload
    const collision_prism *prism, const real_point3d *point, const real_vector3d *vector, float *__restrict fraction, real_plane3d *plane)
{
    float lower = 0.0f, upper = 1.0f;
    float distance = collision_features_plane3d_distance_to_point(&prism->plane, point);
    float dot = dot_product3d(vector, &prism->plane.normal);
    if (fabsf(dot - 0.0f) >= collision_features_k_epsilon) {
        float inverse = 1.0f / dot;
        float near_fraction = -inverse * distance;
        float far_fraction = -inverse * (distance - prism->height);
        if (dot > 0.0f) {
            if (lower < near_fraction)
                lower = near_fraction;
            if (upper > far_fraction)
                upper = far_fraction;
        } else {
            if (lower < far_fraction)
                lower = far_fraction;
            if (upper > near_fraction)
                upper = near_fraction;
        }
        if (lower > upper)
            return false;
    } else if (distance < 0.0f || distance >= prism->height) {
        return false;
    }
    real_point3d projected_point;
    point_from_line3d(point, &prism->plane.normal, -distance, &projected_point);
    real_vector3d projected_vector;
    point_from_line3d(vector, &prism->plane.normal, -dot, &projected_vector);
    real_point2d point2;
    project_point3d(&projected_point, prism->projection, prism->sign, &point2);
    real_vector2d vector2;
    project_vector3d(&projected_vector, prism->projection, prism->sign, &vector2);
    for (long i = 0; i < prism->count; i++) {
        const real_point2d *a = &prism->points[i];
        const real_point2d *b = &prism->points[i + 1 < prism->count ? i + 1 : 0];
        real_vector2d edge, offset;
        vector_from_points2d(a, b, &edge);
        vector_from_points2d(a, &point2, &offset);
        float denominator = cross_product2d(&vector2, &edge);
        float numerator = cross_product2d(&edge, &offset);
        if (fabsf(denominator - 0.0f) >= collision_features_k_epsilon) {
            float f = numerator / denominator;
            if (denominator < 0.0f) {
                if (lower < f)
                    lower = f;
            } else {
                if (upper > f)
                    upper = f;
            }
            if (lower > upper)
                return false;
        } else if (numerator < 0.0f) {
            return false;
        }
    }
    *fraction = lower;
    plane->normal = prism->plane.normal;
    plane->distance = prism->plane.distance + prism->height;
    return true;
}

template<class T> static __forceinline void collision_features_plane_set_user(collision_plane *result, const T *feature)
{
    result->user0 = feature->user0;
    result->user1 = feature->user1;
    result->user2 = feature->user3;
    result->user4 = feature->user4;
    result->user5 = feature->user5;
    result->user6 = feature->user6;
}

// @address 0x29a3c0 ?collision_features_test_vector@@YA_NPBUcollision_feature_list@@PBTreal_point3d@@PBTreal_vector3d@@PAUcollision_plane@@@Z
bool collision_features_test_vector(struct collision_feature_list const *list, union real_point3d const *point,
    union real_vector3d const *vector, struct collision_plane *result)
{
    float best_t = 3.4028234663852886e+38f;
    float threshold = rasterizer_states_mode_7_depth_bias;
    short best_type = -1;
    short best_index = -1;
    real_plane3d best_plane;
    for (short type = 0; type < 3; type++) {
        for (short index = 0; index < list->counts[type]; index++) {
            float t;
            real_plane3d plane;
            if ((type == 0 && collision_sphere_test_vector(&list->spheres[index], point, vector, &t, &plane)) ||
                (type == 1 && collision_cylinder_test_vector(&list->cylinders[index], point, vector, &t, &plane)) ||
                (type == 2 && collision_prism_test_vector(&list->prisms[index], point, vector, &t, &plane))) {
                if (best_t > t && dot_product3d(vector, &plane.normal) < threshold) {
                    best_t = t;
                    best_type = type;
                    best_index = index;
                    best_plane = plane;
                }
            }
        }
    }
    if (best_type != -1) {
        result->t = best_t;
        point_from_line3d(point, vector, best_t, &result->point);
        result->plane = best_plane;
        switch (best_type) {
        case 0:
            collision_features_plane_set_user(result, &list->spheres[best_index]);
            break;
        case 1:
            collision_features_plane_set_user(result, &list->cylinders[best_index]);
            break;
        case 2:
            collision_features_plane_set_user(result, &list->prisms[best_index]);
            break;
        }
        return true;
    }
    result->t = 1.0f;
    point_from_line3d(point, vector, 1.0f, &result->point);
    return false;
}
