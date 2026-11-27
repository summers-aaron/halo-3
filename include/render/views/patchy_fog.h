#ifndef RENDER_VIEWS_PATCHY_FOG_H
#define RENDER_VIEWS_PATCHY_FOG_H

#include "math/real_math_types.h"
struct s_tag_reference;

struct s_patchy_fog_data
{
    real_point3d position;
    real_vector3d forward;
    real_vector3d left;
};

class c_patchy_fog
{
public:
    __forceinline c_patchy_fog();
    static void frame_advance_all(float dt) { ms_dt = dt; }
    s_patchy_fog_data m_data;
    float m_rotation;
    float m_near_distance;
    float m_sheet_distance;
    long m_sheet_index;
    float m_sheet_u[100];
    float m_sheet_v[100];
    void render_patchy_fog(const real_point3d *point, const real_vector3d *forward,
        const real_vector3d *up, const rectangle2d *viewport, const rectangle2d *surface,
        const real_matrix4x3 *view, float near_distance, const float (*const projection)[4]);
private:
    static void set_rasterizer_state(const s_tag_reference *tag_reference);
    void get_relative_movement(const real_point3d *point, const real_vector3d *vector, const real_vector3d *vector2, real_vector3d *result,
        real_euler_angles3d *euler_angles);
    static float ms_dt;
};

typedef char verified_patchy_fog_size[(sizeof(c_patchy_fog) == 0x354) ? 1 : -1];

#endif
