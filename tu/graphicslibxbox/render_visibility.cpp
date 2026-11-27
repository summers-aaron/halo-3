// graphicslibxbox/render_visibility.obj: from the records 0x153fb8
// @flags /TP /O2 /Oc
#include "render/render_camera_types.h"
#include "objects/object_reference_types.h"
// @address 0x153fb8 ?render_visibility_build_projection@@YAXPBUrender_camera@@PBUrender_projection@@Us_cluster_reference@@PAUvisibility_projection@@@Z
#include <xtl.h>
#include "math/matrix_math.h"
#include "visibility/visibility_projections_and_volumes.h"
#include "render/render_visibility.h"
#include "visibility/visibility_projection_types.h"




static __forceinline long render_visibility_polygon2d_from_bounds(const real_rectangle2d *bounds, long count, real_point2d *polygon)
{
    polygon[0].x = bounds->x0;
    polygon[0].y = bounds->y0;
    polygon[1].x = bounds->x1;
    polygon[1].y = bounds->y0;
    polygon[2].x = bounds->x1;
    polygon[2].y = bounds->y1;
    polygon[3].x = bounds->x0;
    polygon[3].y = bounds->y1;
    return 4;
}

void render_visibility_build_projection(const render_camera *render_camera, const render_projection *render_projection,
    s_cluster_reference cluster_reference, visibility_projection *visibility_projection)
{
    memset(visibility_projection, 0, sizeof(*visibility_projection));
    visibility_projection->mirrored = false;
    visibility_projection->basis_to_world.rotation.left = render_camera->up;
    normalize3d(&visibility_projection->basis_to_world.rotation.left);
    negate_vector3d(&render_camera->forward, &visibility_projection->basis_to_world.rotation.up);
    normalize3d(&visibility_projection->basis_to_world.rotation.up);
    cross_product3d(&visibility_projection->basis_to_world.rotation.left, &visibility_projection->basis_to_world.rotation.up,
        &visibility_projection->basis_to_world.rotation.forward);
    normalize3d(&visibility_projection->basis_to_world.rotation.forward);
    visibility_projection->basis_to_world.position = render_camera->position;
    visibility_projection->basis_to_world.scale = 1.0f;
    matrix4x3_inverse(&visibility_projection->basis_to_world, &visibility_projection->world_to_basis);
    visibility_projection->near_plane_valid = true;
    visibility_projection->near_clip = render_camera->near_clip;
    visibility_projection->near_plane.normal = render_camera->forward;
    visibility_projection->near_plane.distance = dot_product3d(&render_camera->position, &render_camera->forward) +
        render_camera->near_clip;
    visibility_projection->far_plane_valid = true;
    visibility_projection->far_distance = render_camera->far_clip;
    visibility_projection->polygon_count = render_visibility_polygon2d_from_bounds(&render_projection->frustum, 4, visibility_projection->polygon);
    visibility_projection->volume_valid = true;
    // March asserts success here (render_visibility.cpp:176); the release build drops the assert.
    bool success = visibility_volume_build(visibility_projection, 0, &render_projection->frustum, &visibility_projection->volume);
}
