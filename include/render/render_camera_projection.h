#ifndef RENDER_RENDER_CAMERA_PROJECTION_H
#define RENDER_RENDER_CAMERA_PROJECTION_H

struct render_projection;
union real_point3d;

extern "C" float render_camera_compute_projected_diameter(const render_projection *projection, const real_point3d *center, float radius);

struct render_cameras_projection_factors
{
    float tangent;
    float aspect;
};

struct render_cameras_vertical_factors
{
    float lower;
    float upper;
    float scale;
};

#endif // RENDER_RENDER_CAMERA_PROJECTION_H
