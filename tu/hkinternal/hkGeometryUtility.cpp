// hkinternal/hkGeometryUtility.obj: from the records 0x687ae0 0x687ae8 0x687b28 0x687fe8 0x688100 0x688128 0x688188 0x688430 0x688588 0x6886e8 0x688850 0x688ab8 0x688c00 0x688c70 0x688d78 0x688ed0 0x689130 0x6896b0 0x689790 0x6898f8 0x68a338 0x68a450 0x68a4c8 0x68a580
// @compiler xdk5632
// @flags /TP /O2
// @address 0x687ae0 ?vertices_from_planes@phys_geometry_util@@SAXABV?$phys_array@Vphys_vec4@@@@AAV2@@Z
// Stack arrays use trivial hkVector4 construction.
#define PHYS_DISABLE_VEC4_CONSTRUCTORS
#include "phys/phys_base_types.h"
#include <stddef.h>
#include "phys/phys_strided_vertices.h"
#include "phys/phys_memory_class.h"
#include "phys/phys_collide.h"
#include "phys/phys_hull_builder.h"
#include "phys/phys_geom_hull.h"
#include "phys/phys_geometry.h"
#include "phys/phys_local_array.h"
#include "phys/phys_math_allocator.h"
#include "phys/phys_thread_memory_allocator.h"
// @address 0x687ae8 ?expand_planes@phys_geometry_util@@SAXAAV?$phys_array@Vphys_vec4@@@@M@Z
// @address 0x688100 ?approx_equals@@YA_NABM0M@Z
// @address 0x68a450 ?build_convex_geometry@phys_geometry_util@@SAXABUphys_strided_vertices@@AAUphys_geometry@@W4phys_hull_mode@@@Z
#include <math.h>
#include "phys/phys_scalar_types.h"
#include "phys/phys_plane_equation_util.h"
#include "phys/phys_geometry_util.h"
#include "phys/phys_hull_mode.h"
#include "phys/phys_hull_tolerances.h"



void phys_geometry_util::vertices_from_planes(const phys_array<phys_vec4> &planes, phys_array<phys_vec4> &vertices)
{
    phys_plane_equation_util::hull_verts_from_planes(planes, vertices);
}

void phys_geometry_util::expand_planes(phys_array<phys_vec4> &planes, float plane_expansion)
{
    for (int plane_index = 0; plane_index < planes.getSize(); ++plane_index)
        planes[plane_index](3) -= plane_expansion;
}

bool approx_equals(const float &a, const float &b, float tolerance)
{
    if ((float)fabs(a - b) < tolerance)
        return true;
    return false;
}

// @address 0x688c00 ?make_bounding_sphere@@YA?AVphys_sphere@@ABVphys_vec4@@0M@Z
phys_sphere make_bounding_sphere(const phys_vec4 &a, const phys_vec4 &b, float radius)
{
    phys_vec4 diff; diff.set_sub4(b, a);
    phys_vec4 half_diff; half_diff.set_mul4(0.5f, diff);
    phys_vec4 center; center.set_add4(a, half_diff);
    return phys_sphere(center, static_cast<float>(half_diff.length_3d()) + radius);
}

// @address 0x68a4c8 ?geometry_from_planes@phys_geometry_util@@SAXABV?$phys_array@Vphys_vec4@@@@AAUphys_geometry@@@Z
void phys_geometry_util::geometry_from_planes(const phys_array<phys_vec4> &planes, phys_geometry &geometry)
{
    phys_array<phys_vec4> vertices;
    phys_plane_equation_util::hull_verts_from_planes(planes, vertices);

    if (vertices.getSize() > 0)
    {
        phys_strided_vertices strided_vertex_view;
        strided_vertex_view.m_vertices = &vertices[0](0);
        strided_vertex_view.m_vertex_stride_bytes = sizeof(phys_vec4);
        strided_vertex_view.m_hull_vertex_count = vertices.getSize();
        build_convex_geometry(strided_vertex_view, geometry, PHYS_HULL_MODE_FAST);
    }
    else
    {
        geometry.m_triangle_list.clear();
        geometry.m_vertices.clear();
    }
}

// @address 0x687b28 ?make_eigenvectors@@YA?AVphys_bool@@AAVphys_mat3@@H@Z
phys_bool make_eigenvectors(phys_mat3 &matrix, int max_iterations)
{
    float max_column_len = 0.0f;
    for (int c = 0; c < 3; c++)
    {
        float length = matrix.get_column(c).length_3d();
        if (length > max_column_len)
            max_column_len = length;
    }
    if (max_column_len != 0.0f)
    {
        for (int i = 0; i < 3; i++)
            for (int j = 0; j < 3; j++)
                matrix(i, j) = matrix(i, j) / max_column_len;
    }

    float diag_term[3];
    float subdivisions[3];
    if (matrix(0, 2) != 0.0f)
    {
        diag_term[0] = matrix(0, 0);
        subdivisions[2] = 0.0f;
        float length = sqrtf(matrix(0, 1) * matrix(0, 1) + matrix(0, 2) * matrix(0, 2));
        float m01 = matrix(0, 1) / length;
        float m02 = matrix(0, 2) / length;
        float m10 = matrix(1, 0);
        float m20 = matrix(2, 0);
        float m21 = matrix(2, 1);
        float m00 = matrix(0, 0);
        float m11 = matrix(1, 1);
        float q = 2.0f * m01 * matrix(1, 2) + m02 * (matrix(2, 2) - m11);
        diag_term[1] = m11 + m02 * q;
        diag_term[2] = matrix(2, 2) - m02 * q;
        subdivisions[0] = length;
        subdivisions[1] = matrix(1, 2) - m01 * q;
        matrix(0, 0) = 1.0f; matrix(0, 1) = 0.0f; matrix(0, 2) = 0.0f;
        matrix(1, 0) = 0.0f; matrix(1, 1) = m01;  matrix(1, 2) = m02;
        matrix(2, 0) = 0.0f; matrix(2, 1) = m02;  matrix(2, 2) = -m01;
    }
    else
    {
        diag_term[0] = matrix(0, 0);
        diag_term[1] = matrix(1, 1);
        diag_term[2] = matrix(2, 2);
        subdivisions[0] = matrix(0, 1);
        subdivisions[1] = matrix(1, 2);
        subdivisions[2] = 0.0f;
        phys_vec4 zero;
        zero.set_zero4();
        matrix.get_column(0) = zero;
        matrix.get_column(1) = zero;
        matrix.get_column(2) = zero;
        matrix(0, 0) = 1.0f;
        matrix(1, 1) = 1.0f;
        matrix(2, 2) = 1.0f;
    }

    for (int i0 = 0; i0 < 3; i0++)
    {
        int iteration;
        for (iteration = 0; iteration < max_iterations; iteration++)
        {
            int i2;
            for (i2 = i0; i2 <= 1; i2++)
            {
                if (fabsf(subdivisions[i2]) < (1e-6f))
                    break;
            }
            if (i2 == i0)
                break;

            float g = (diag_term[i0 + 1] - diag_term[i0]) / (2.0f * subdivisions[i0]);
            float r = sqrtf(g * g + 1.0f);
            if (g < 0.0f)
                g = diag_term[i2] - diag_term[i0] + subdivisions[i0] / (g - r);
            else
                g = diag_term[i2] - diag_term[i0] + subdivisions[i0] / (g + r);

            float s = 1.0f;
            float c = 1.0f;
            float p = 0.0f;
            for (int fourth_vertex_idx = i2 - 1; fourth_vertex_idx >= i0; fourth_vertex_idx--)
            {
                float f = s * subdivisions[fourth_vertex_idx];
                float b = c * subdivisions[fourth_vertex_idx];
                if (fabsf(f) > fabsf(g))
                {
                    float t = -g / f;
                    s = -1.0f / sqrtf(1.0f + t * t);
                    c = s * t;
                    s = -s;
                    subdivisions[fourth_vertex_idx + 1] = f / s;
                }
                else
                {
                    float t = -f / g;
                    c = -1.0f / sqrtf(1.0f + t * t);
                    s = c * t;
                    c = -c;
                    subdivisions[fourth_vertex_idx + 1] = g / c;
                }
                g = diag_term[fourth_vertex_idx + 1] - p;
                r = (diag_term[fourth_vertex_idx] - g) * s + 2.0f * b * c;
                p = s * r;
                diag_term[fourth_vertex_idx + 1] = g + p;
                g = c * r - b;
                for (int k = 0; k < 3; k++)
                {
                    r = matrix(k, fourth_vertex_idx + 1);
                    matrix(k, fourth_vertex_idx + 1) = s * matrix(k, fourth_vertex_idx) + c * r;
                    matrix(k, fourth_vertex_idx) = c * matrix(k, fourth_vertex_idx) - s * r;
                }
            }
            diag_term[i0] -= p;
            subdivisions[i0] = g;
            subdivisions[i2] = 0.0f;
        }
        if (iteration == max_iterations)
            return phys_bool(false);
    }
    return phys_bool(true);
}
// @address 0x687fe8 ?get_axes_from_covariance@@YA?AVphys_bool@@AAVphys_mat3@@AAVphys_vec4@@11@Z
class phys_bool get_axes_from_covariance(class phys_mat3 &cov_mat, class phys_vec4 &first_axis, class phys_vec4 &axis1, class phys_vec4 &axis2)
{
    phys_bool result = make_eigenvectors(cov_mat, 30);
    if (!result)
    {
        first_axis.set(1.0f, 0.0f, 0.0f, 0.0f);
        axis1.set(0.0f, 1.0f, 0.0f, 0.0f);
        axis2.set(0.0f, 0.0f, 1.0f, 0.0f);
    }
    else
    {
        cov_mat.get_columns(first_axis, axis1, axis2);
        phys_vec4 cross;
        cross.set_cross(first_axis, axis1);
        if (cross.dot3(axis2) < 0.0f)
            axis2.set_neg4(axis2);
    }
    return result;
}

static __forceinline float phys_geometry_util_max2(float x, float y) { return (float)__fsel(x - y, x, y); }

// @address 0x688128 ?is_axis_multiple@@YA_NABVphys_vec4@@M@Z
bool is_axis_multiple(class phys_vec4 const &v, float float_epsilon)
{
    phys_vec4 &vv = const_cast<phys_vec4 &>(v);
    phys_vec4 abs_vec;
    abs_vec.set((float)fabs(vv(0)), (float)fabs(vv(1)), (float)fabs(vv(2)), 0.0f);
    float m = phys_geometry_util_max2(abs_vec(2), phys_geometry_util_max2(abs_vec(0), abs_vec(1)));
    bool result = (float)fabs(m - 1.0f) < float_epsilon;
    return result;
}

// @address 0x688188 ?make_obb_from_axes@@YAXPBMHHAAVphys_vec4@@PBV1@1@Z
void make_obb_from_axes(const float *vertices_in, int hull_vertex_count, int stride, phys_vec4 &box_half_extents, const phys_vec4 *axes, phys_vec4 &center)
{
    phys_vec4 min;
    min.flags_set_all(1.3611281e38f);
    phys_vec4 max;
    max.flags_set_all(-1.3611281e38f);
    const float *vertices = vertices_in;
    phys_vec4 v;
    for (int i = 0; i < hull_vertex_count / 3; i++)
    {
        v.set(vertices[0], vertices[1], vertices[2]);
        vertices = phys_add_byte_offset_const(vertices, stride);
        for (int j = 0; j < 3; j++)
        {
            float d = axes[j].dot3(v);
            if (d < min(j))
                min(j) = d;
            if (d > max(j))
                max(j) = d;
        }
        v.set(vertices[0], vertices[1], vertices[2]);
        vertices = phys_add_byte_offset_const(vertices, stride);
        for (int j = 0; j < 3; j++)
        {
            float d = axes[j].dot3(v);
            if (d < min(j))
                min(j) = d;
            if (d > max(j))
                max(j) = d;
        }
        v.set(vertices[0], vertices[1], vertices[2]);
        vertices = phys_add_byte_offset_const(vertices, stride);
        for (int j = 0; j < 3; j++)
        {
            float d = axes[j].dot3(v);
            if (d < min(j))
                min(j) = d;
            if (d > max(j))
                max(j) = d;
        }
    }
    phys_vec4 obb_center_pos;
    obb_center_pos.set_add4(max, min);
    obb_center_pos.mul4(0.5f);
    box_half_extents.set_sub4(max, min);
    box_half_extents.mul4(0.5f);
    center.set_mul4(obb_center_pos(0), axes[0]);
    center.add_scaled4(obb_center_pos(1), axes[1]);
    center.add_scaled4(obb_center_pos(2), axes[2]);
}



// @address 0x688430 ?get_volume_from_axes@@YAMPBMHHPBVphys_vec4@@@Z
float get_volume_from_axes(const float *vertices_in, int hull_vertex_count, int stride, const phys_vec4 *axes_in)
{
    phys_vec4 axes[3];
    axes[0] = axes_in[0];
    axes[1] = axes_in[1];
    axes[0].normalize_xyz();
    float d = axes_in[0].dot3(axes_in[1]);
    axes[1].sub_mul4(d, axes_in[0]);
    axes[1].normalize_xyz();
    axes[2].set_cross(axes_in[0], axes_in[1]);
    axes[2].normalize_xyz();
    phys_vec4 box_half_extents;
    phys_vec4 center;
    make_obb_from_axes(vertices_in, hull_vertex_count, stride, box_half_extents, axes, center);
    return box_half_extents(0) * box_half_extents(1) * box_half_extents(2);
}
// @address 0x688c70 ?make_bounding_sphere@@YA?AVphys_sphere@@ABVphys_vec4@@00M@Z
phys_sphere make_bounding_sphere(const phys_vec4 &p0, const phys_vec4 &p1, const phys_vec4 &p2, float radius)
{
    phys_vec4 a; a.set_sub4(p1, p0);
    phys_vec4 b; b.set_sub4(p2, p0);
    phys_vec4 n; n.set_cross(a, b);
    float inv_denom = 0.5f / static_cast<float>(n.dot3(n));
    phys_vec4 center = p0;
    phys_vec4 o; o.set_zero4();
    phys_vec4 t;
    t.set_cross(n, a); o.add_scaled4(b.dot3(b), t);
    t.set_cross(b, n); o.add_scaled4(a.dot3(a), t);
    o.mul4(inv_denom);
    center.add4(o);
    return phys_sphere(center, static_cast<float>(o.length_3d()) + radius);
}

// @address 0x688d78 ?make_bounding_sphere@@YA?AVphys_sphere@@ABVphys_vec4@@000M@Z
phys_sphere make_bounding_sphere(const phys_vec4 &p0, const phys_vec4 &p1, const phys_vec4 &p2, const phys_vec4 &p3, float radius)
{
    phys_vec4 a; a.set_sub4(p1, p0);
    phys_vec4 b; b.set_sub4(p2, p0);
    phys_vec4 c; c.set_sub4(p3, p0);
    float capsule_determinant = a(0) * (b(1) * c(2) - b(2) * c(1)) - a(1) * (b(0) * c(2) - b(2) * c(0)) + a(2) * (b(0) * c(1) - b(1) * c(0));
    phys_vec4 o; o.set_zero4();
    phys_vec4 t;
    t.set_cross(a, b); o.add_scaled4(c.dot3(c), t);
    t.set_cross(c, a); o.add_scaled4(b.dot3(b), t);
    t.set_cross(b, c); o.add_scaled4(a.dot3(a), t);
    o.mul4(0.5f / capsule_determinant);
    phys_vec4 center; center.set_add4(p0, o);
    return phys_sphere(center, static_cast<float>(o.length_3d()) + radius);
}

// @address 0x688ed0 ?mini_sphere_recurse@@YA?AVphys_sphere@@PAVphys_vec4@@HHM@Z
phys_sphere mini_sphere_recurse(phys_vec4 *points, int contact_reserve_count, int boundary_cnt, float radius_epsilon)
{
    phys_sphere sphere;
    switch (boundary_cnt)
    {
    case 0:
    {
        phys_vec4 zero; zero.set_zero4();
        sphere = phys_sphere(zero, -1.0f);
        break;
    }
    case 1:
        sphere = phys_sphere(points[-1], radius_epsilon);
        break;
    case 2:
        sphere = make_bounding_sphere(points[-1], points[-2], radius_epsilon);
        break;
    case 3:
        sphere = make_bounding_sphere(points[-1], points[-2], points[-3], radius_epsilon);
        break;
    default:
        sphere = make_bounding_sphere(points[-1], points[-2], points[-3], points[-4], radius_epsilon);
        return sphere;
    }
    for (int i = 0; i < contact_reserve_count; i++)
    {
        phys_vec4 diff; diff.set_sub4(points[i], sphere.get_contact_position());
        if (diff.length_3d() > sphere.get_convex_radius())
        {
            for (int j = i; j > 0; j--)
            {
                phys_vec4 t = points[j];
                points[j] = points[j - 1];
                points[j - 1] = t;
            }
            sphere = mini_sphere_recurse(points + 1, i, boundary_cnt + 1, radius_epsilon);
        }
    }
    return sphere;
}



// @address 0x688588 ?get_volume_gradient@@YAXPBMHHPBVphys_vec4@@MPAV1@@Z
void get_volume_gradient(const float *vertices_in, int hull_vertex_count, int stride, const phys_vec4 *axes, float step, phys_vec4 *volume_gradient)
{
    float delta = step;
    phys_vec4 axes_copy[3];
    axes_copy[0] = axes[0];
    axes_copy[1] = axes[1];
    float volume = get_volume_from_axes(vertices_in, hull_vertex_count, stride, axes);
    float inv_delta = 1.0f / delta;
    for (int i = 0; i < 2; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            axes_copy[i](j) = delta + axes_copy[i](j);
            float new_volume = get_volume_from_axes(vertices_in, hull_vertex_count, stride, axes_copy);
            volume_gradient[i](j) = inv_delta * (new_volume - volume);
            axes_copy[i](j) = const_cast<phys_vec4 &>(axes[i])(j);
        }
        volume_gradient[i].mul4(-1.0f);
    }
    float dir_length_sq = volume_gradient[0].length_sq_3d() + volume_gradient[1].length_sq_3d();
    if (dir_length_sq > 1.0f)
    {
        float inv_length = 1.0f / __fsqrts(dir_length_sq);
        volume_gradient[0].mul4(inv_length);
        volume_gradient[1].mul4(inv_length);
    }
}

// @address 0x6886e8 ?take_step_axes@@YAXPBVphys_vec4@@PAV1@M1@Z
void take_step_axes(const phys_vec4 *axes, phys_vec4 *direction, float step, phys_vec4 *axes_out)
{
    for (int i = 0; i < 2; i++)
    {
        axes_out[i] = axes[i];
        axes_out[i].add_scaled4(step, direction[i]);
    }
    axes_out[0].normalize_xyz();
    float d = axes_out[0].dot3(axes_out[1]);
    axes_out[1].sub_mul4(d, axes_out[0]);
    axes_out[1].normalize_xyz();
    axes_out[2].set_cross(axes_out[0], axes_out[1]);
    axes_out[2].normalize_xyz();
}


// @address 0x688850 ?minimize_along_line@@YAMPBMHHPBVphys_vec4@@MPAV1@H@Z
float minimize_along_line(const float *vertices_in, int hull_vertex_count, int stride, const phys_vec4 *axes, float step,
    phys_vec4 *direction, int max_iterations)
{
    float fb = 0.0f;
    float fc = 0.0f;
    float a = 0.0f;
    float fa = get_volume_from_axes(vertices_in, hull_vertex_count, stride, axes);
    phys_vec4 new_axes[3];
    take_step_axes(axes, direction, step, new_axes);
    float step_volume = get_volume_from_axes(vertices_in, hull_vertex_count, stride, new_axes);
    float b;
    float c;
    if (step_volume < fa)
    {
        b = step;
        fb = step_volume;
        c = b;
        for (int i = 0; i < max_iterations; i++)
        {
            c *= 1.2f;
            take_step_axes(axes, direction, c, new_axes);
            fc = get_volume_from_axes(vertices_in, hull_vertex_count, stride, new_axes);
            if (fb < fc)
            {
                break;
            }
            else
            {
                b = c;
                fb = fc;
            }
            if (i == max_iterations - 1)
                return c;
        }
    }
    else
    {
        c = step;
        fc = step_volume;
        b = c;
        for (int i = 0; i < max_iterations; i++)
        {
            b = 0.5f * (a + b);
            take_step_axes(axes, direction, b, new_axes);
            fb = get_volume_from_axes(vertices_in, hull_vertex_count, stride, new_axes);
            if (fb < fa && fb < fc)
                break;
            if (i == max_iterations - 1)
                return -1.0f;
        }
    }
    if (a > b || b > c || fa < fb || fc < fb)
        return -1.0f;
    float ratio = 0.5f * (3.0f - __fsqrts(5.0f));
    for (int i = 0; i < max_iterations; i++)
    {
        float ab = b - a;
        float bc = c - b;
        float x;
        float fx;
        if (ab > bc)
        {
            x = b - ratio * ab;
            take_step_axes(axes, direction, x, new_axes);
            fx = get_volume_from_axes(vertices_in, hull_vertex_count, stride, new_axes);
            if (fx < fb)
            {
                c = b;
                fc = fb;
                b = x;
                fb = fx;
            }
            else
            {
                a = x;
                fa = fx;
            }
        }
        else
        {
            x = b + ratio * bc;
            take_step_axes(axes, direction, x, new_axes);
            fx = get_volume_from_axes(vertices_in, hull_vertex_count, stride, new_axes);
            if (fx < fb)
            {
                a = b;
                fa = fb;
                b = x;
                fb = fx;
            }
            else
            {
                c = x;
                fc = fx;
            }
        }
    }
    return b;
}
// @address 0x688ab8 ?steepest_descent@@YAMPBMHHPBVphys_vec4@@PAV1@H@Z
float steepest_descent(const float *vertices_in, int hull_vertex_count, int stride, const phys_vec4 *axes_in, phys_vec4 *axes_out, int max_iterations)
{
    float delta = 0.0001f;
    float step = 0.001f;
    float volume = get_volume_from_axes(vertices_in, hull_vertex_count, stride, axes_in);
    axes_out[0] = axes_in[0];
    axes_out[1] = axes_in[1];
    axes_out[2] = axes_in[2];
    for (int i = 0; i < max_iterations; i++)
    {
        phys_vec4 volume_gradient[2];
        phys_vec4 prev_axes[3];
        prev_axes[0] = axes_out[0];
        prev_axes[1] = axes_out[1];
        prev_axes[2] = axes_out[2];
        get_volume_gradient(vertices_in, hull_vertex_count, stride, axes_out, delta, volume_gradient);
        step = minimize_along_line(vertices_in, hull_vertex_count, stride, axes_out, step, volume_gradient, max_iterations);
        if (step < 0.0f)
            break;
        take_step_axes(axes_out, volume_gradient, step, axes_out);
        float prev_volume = volume;
        volume = get_volume_from_axes(vertices_in, hull_vertex_count, stride, axes_out);
        if (prev_volume < volume)
        {
            volume = prev_volume;
            axes_out[0] = prev_axes[0];
            axes_out[1] = prev_axes[1];
            axes_out[2] = prev_axes[2];
            break;
        }
        if (step < 1.1920929e-6f)
            break;
    }
    return volume;
}


// @address 0x689790 ?hull_from_strided_verts@@YAXABUphys_strided_vertices@@AAVphys_geom_hull@@AAV?$phys_array@Vphys_vec4@@@@W4phys_hull_mode@@@Z
void hull_from_strided_verts(const phys_strided_vertices &strided_verts_in, phys_geom_hull &geom_hull,
    phys_array<phys_vec4> &used_vertices, phys_hull_mode mode)
{
    if (strided_verts_in.m_vertex_stride_bytes == sizeof(phys_vec4) && (reinterpret_cast<uintptr_t>(strided_verts_in.m_vertices) & 0xf) == 0)
    {
        phys_hull_builder::generate_convex_hull(
            reinterpret_cast<const phys_vec4 *>(strided_verts_in.m_vertices), strided_verts_in.m_hull_vertex_count, geom_hull, used_vertices, mode);
    }
    else
    {
        phys_array<phys_vec4> vertices;

        int stride = strided_verts_in.m_vertex_stride_bytes / 4;
        int index = 0;
        for (int i = 0; i < strided_verts_in.m_hull_vertex_count; i++)
        {
            phys_vec4 &v = *vertices.expand_by(1);
            v(0) = strided_verts_in.m_vertices[index];
            v(1) = strided_verts_in.m_vertices[index + 1];
            v(2) = strided_verts_in.m_vertices[index + 2];
            index += stride;
        }

        phys_hull_builder::generate_convex_hull(vertices.begin(), strided_verts_in.m_hull_vertex_count, geom_hull, used_vertices, mode);
    }
}
void phys_geometry_util::build_convex_geometry(const phys_strided_vertices &vertices, phys_geometry &geometry, phys_hull_mode mode)
{
    phys_geom_hull geom_hull;
    hull_from_strided_verts(vertices, geom_hull, geometry.m_vertices, mode);
    phys_hull_builder::build_display_geometry(geom_hull, geometry);
}
// @address 0x68a338 ?build_convex_geometry@phys_geometry_util@@SAXABUphys_strided_vertices@@AAUphys_geometry@@AAV?$phys_array@Vphys_vec4@@@@W4phys_hull_mode@@@Z
void phys_geometry_util::build_convex_geometry(const phys_strided_vertices &vertices, phys_geometry &geometry,
    phys_array<phys_vec4> &planes, phys_hull_mode mode)
{
    phys_geom_hull geom_hull;
    hull_from_strided_verts(vertices, geom_hull, geometry.m_vertices, mode);
    phys_hull_builder::build_display_geometry(geom_hull, geometry);
    phys_hull_tolerances hull_tolerances;
    hull_tolerances.m_accurate_but_slow = (mode == 2);
    phys_hull_builder::build_plane_equations(hull_tolerances, geom_hull, geometry.m_vertices, planes);
}

// @address 0x6896b0 ?min_bounding_sphere@phys_geometry_util@@SA?AVphys_sphere@@ABUphys_strided_vertices@@M@Z
phys_sphere phys_geometry_util::min_bounding_sphere(const phys_strided_vertices &strided_verts_in, float radius_epsilon)
{
    phys_vec4 *points = phys_alloc<phys_vec4>(strided_verts_in.m_hull_vertex_count, PHYS_MEM_GEOMETRY);
    int stride = strided_verts_in.m_vertex_stride_bytes / 4;
    int index = 0;
    for (int i = 0; i < strided_verts_in.m_hull_vertex_count; i++)
    {
        for (int j = 0; j < 3; j++)
            points[i](j) = strided_verts_in.m_vertices[index + j];
        index += stride;
    }
    phys_sphere sphere = mini_sphere_recurse(points, strided_verts_in.m_hull_vertex_count, 0, radius_epsilon);
    phys_free_memory(points);
    return sphere;
}

// @address 0x689130 ?compute_mean_covariance@@YAXPBMHHAAVphys_mat3@@AAVphys_vec4@@Vphys_bool@@@Z
void compute_mean_covariance(const float *vertices_in, int hull_vertex_count, int stride, phys_mat3 &cov_mat, phys_vec4 &mean, phys_bool use_diagonal)
{
    int hull_triangle_count = hull_vertex_count / 3;
    float total_area = 0.0f;
    phys_vec4 sum;
    sum.set_zero4();
    phys_vec4 *tri_centroids = new phys_vec4[hull_triangle_count];
    cov_mat.set_zero();
    const float *v = vertices_in;
    for (int i = 0; i < hull_triangle_count; i++)
    {
        phys_vec4 p0;
        p0.set(v[0], v[1], v[2], 0.0f);
        v = phys_add_byte_offset_const(v, stride);
        phys_vec4 p1;
        p1.set(v[0], v[1], v[2], 0.0f);
        v = phys_add_byte_offset_const(v, stride);
        phys_vec4 p2;
        p2.set(v[0], v[1], v[2], 0.0f);
        v = phys_add_byte_offset_const(v, stride);
        float area;
        if (use_diagonal)
        {
            phys_vec4 rand_dir_vec;
            rand_dir_vec.set(phys_math::rand_unit(), phys_math::rand_unit(), phys_math::rand_unit(), 0.0f);
            phys_vec4 e1;
            e1.set_mul4(0.01f, rand_dir_vec);
            e1.add4(p0);
            e1.sub4(p1);
            phys_vec4 e2;
            e2.set_sub4(p0, p2);
            phys_vec4 n;
            n.set_cross(e1, e2);
            area = static_cast<float>(n.length_3d()) * 0.5f;
        }
        else
        {
            phys_vec4 e1;
            e1.set_sub4(p0, p1);
            phys_vec4 e2;
            e2.set_sub4(p0, p2);
            phys_vec4 n;
            n.set_cross(e1, e2);
            area = static_cast<float>(n.length_3d()) * 0.5f;
        }
        if (area > 0.0f)
        {
            total_area += area;
            phys_vec4 c;
            c.set_add4(p0, p1);
            c.add4(p2);
            c.mul4(1.0f / 3.0f);
            tri_centroids[i] = c;
            phys_vec4 t;
            t.set_mul4(area, tri_centroids[i]);
            sum.add4(t);
            for (int j = 0; j < 3; j++)
            {
                for (int k = 0; k < 3; k++)
                {
                    cov_mat(j, k) += area * (9.0f * tri_centroids[i](j) * tri_centroids[i](k) + p0(j) * p0(k) + p1(j) * p1(k) + p2(j) * p2(k));
                }
            }
        }
        else
        {
            tri_centroids[i].set_add4(p0, p1);
            tri_centroids[i].add4(p2);
            tri_centroids[i].mul4(1.0f / 3.0f);
            for (int j = 0; j < 3; j++)
            {
                for (int k = 0; k < 3; k++)
                {
                    cov_mat(j, k) += (p0(j) * p0(k) + p1(j) * p1(k) + p2(j) * p2(k));
                }
            }
        }
    }
    if (total_area <= 0.0f)
    {
        mean.set_zero4();
        cov_mat.set_to_identity();
    }
    else
    {
        sum.mul4(1.0f / total_area);
        for (int j = 0; j < 3; j++)
        {
            for (int k = 0; k < 3; k++)
            {
                cov_mat(j, k) = cov_mat(j, k) / 12.0f - sum(j) * sum(k) * total_area;
            }
        }
        mean = sum;
    }
    delete[] tri_centroids;
}
// @address 0x6898f8 ?is_box_geometry@@YA_NABV?$phys_array@Vphys_vec4@@@@AAVphys_vec4@@AAVphys_transform@@M@Z
bool is_box_geometry(const phys_array<phys_vec4> &vertices, phys_vec4 &box_half_extents, phys_transform &transform, float tolerance)
{
    phys_sphere sphere;
    {
        phys_strided_vertices strided_verts;
        strided_verts.m_hull_vertex_count = vertices.getSize();
        strided_verts.m_vertex_stride_bytes = sizeof(phys_vec4);
        strided_verts.m_vertices = &vertices[0](0);
        sphere = phys_geometry_util::min_bounding_sphere(strided_verts, 1e-5f);
    }
    phys_vec4 center = sphere.get_contact_position();
    float radius = sphere.get_convex_radius();

    phys_array<phys_vec4> box(8);
    phys_array<phys_vec4> corners;
    int i = 0;
    int hull_vertex_count = vertices.getSize();
    for (i = 0; i < hull_vertex_count; i++)
    {
        phys_vec4 diff;
        diff.set_sub4(vertices[i], center);
        if ((float)fabs(static_cast<float>(diff.length_3d()) - radius) < tolerance)
        {
            corners.array_push_back(vertices[i]);
        }
    }
    if (corners.getSize() != 8)
    {
        return false;
    }
    for (i = 0; i < 8; i++)
    {
        box[i] = corners[i];
    }

    phys_vec4 edge0 = box[0];
    phys_vec4 b1 = box[1];
    edge0.sub4(b1);
    phys_vec4 edge1 = box[0];
    phys_vec4 b2 = box[2];
    edge1.sub4(b2);
    for (i = 3; i < 8; i++)
    {
        phys_vec4 edge = box[0];
        edge.sub4(box[i]);
        if (edge.dot3(edge) < edge0.dot3(edge0))
        {
            if (edge0.dot3(edge0) < edge1.dot3(edge1))
            {
                edge1 = edge0;
            }
            edge0 = edge;
        }
        else
        {
            if (edge.dot3(edge) < edge1.dot3(edge1))
            {
                edge1 = edge;
            }
        }
    }
    if (edge0.dot3(edge1) > 1e-5f)
    {
        return false;
    }

    phys_vec4 edge2;
    edge2.set_cross(edge0, edge1);
    edge0.normalize_xyz();
    edge1.normalize_xyz();
    edge2.normalize_xyz();
    phys_rotation rotation;
    rotation.set_cols(edge0, edge1, edge2);
    rotation.try_invert(0.01f);
    for (i = 0; i < 8; i++)
    {
        box[i].vec_set_rotated_dir(rotation, box[i]);
    }

    float box_min_x = box[0](0);
    float box_max_x = box[0](0);
    float box_min_y = box[0](1);
    float box_max_y = box[0](1);
    float box_min_z = box[0](2);
    float box_max_z = box[0](2);
    for (i = 1; i < 8; i++)
    {
        if (box_min_x > box[i](0))
            box_min_x = box[i](0);
        else if (box_max_x < box[i](0))
            box_max_x = box[i](0);
        if (box_min_y > box[i](1))
            box_min_y = box[i](1);
        else if (box_max_y < box[i](1))
            box_max_y = box[i](1);
        if (box_min_z > box[i](2))
            box_min_z = box[i](2);
        else if (box_max_z < box[i](2))
            box_max_z = box[i](2);
    }
    for (i = 0; i < 8; i++)
    {
        const phys_vec4 &b = box[i];
        if ((approx_equals(b(0), box_min_x, 0.001f) || approx_equals(b(0), box_max_x, 0.001f)) &&
            (approx_equals(b(1), box_min_y, 0.001f) || approx_equals(b(1), box_max_y, 0.001f)) &&
            (approx_equals(b(2), box_min_z, 0.001f) || approx_equals(b(2), box_max_z, 0.001f)))
        {
            continue;
        }
        else
        {
            return false;
        }
    }

    box_half_extents(0) = box_max_x - box_min_x;
    box_half_extents(1) = box_max_y - box_min_y;
    box_half_extents(2) = box_max_z - box_min_z;
    box_half_extents.mul4(0.5f);
    phys_vec4 box_center_pos;
    box_center_pos.set((box_max_x + box_min_x) * 0.5f, (box_max_y + box_min_y) * 0.5f, (box_max_z + box_min_z) * 0.5f, 0.0f);
    box_center_pos.mul4(-1.0f);
    transform.set_to_inverse(phys_transform(rotation, box_center_pos));

    if (is_axis_multiple(rotation.get_column(0), tolerance) && is_axis_multiple(rotation.get_column(1),
        tolerance) && is_axis_multiple(rotation.get_column(2), tolerance))
    {
        box_half_extents.vec_set_rotated_dir(transform.get_rotation(), box_half_extents);
        box_half_extents(3) = 0.0f;
        box_half_extents.set_abs4(box_half_extents);
        rotation.set_to_identity();
        transform.set_rotation(rotation);
    }

    phys_transform inverse;
    inverse.set_to_inverse(transform);
    for (i = 0; i < vertices.getSize(); i++)
    {
        phys_vec4 p;
        p.vec_set_transformed_pos(inverse, vertices[i]);
        for (int j = 0; j < 3; j++)
        {
            if ((float)fabs(p(j)) > box_half_extents(j) * 1.001f)
            {
                return false;
            }
        }
    }
    return true;
}
// @address 0x68a580 ?compute_obb@phys_geometry_util@@SAXABUphys_strided_vertices@@AAVphys_vec4@@AAVphys_transform@@W4phys_geom_obb_mode@@@Z
void phys_geometry_util::compute_obb(const phys_strided_vertices &strided_verts_in, phys_vec4 &box_half_extents,
    phys_transform &transform, phys_geom_obb_mode mode)
{
    phys_geometry wire_body_cap;
    build_convex_geometry(strided_verts_in, wire_body_cap, (phys_hull_mode)1);

    phys_local_array<phys_vec4> verts(wire_body_cap.m_triangle_list.getSize() * 3);
    verts.set_array_size(wire_body_cap.m_triangle_list.getSize() * 3);
    for (int i = 0; i < wire_body_cap.m_triangle_list.getSize(); i++)
    {
        verts[i * 3] = wire_body_cap.m_vertices[wire_body_cap.m_triangle_list[i].m_a];
        verts[i * 3 + 1] = wire_body_cap.m_vertices[wire_body_cap.m_triangle_list[i].m_b];
        verts[i * 3 + 2] = wire_body_cap.m_vertices[wire_body_cap.m_triangle_list[i].m_c];
    }

    bool is_box_geom = is_box_geometry(wire_body_cap.m_vertices, box_half_extents, transform, 0.01f);
    if (is_box_geom)
    {
        return;
    }

    const float *vertices_in = &verts[0](0);
    int hull_vertex_count = verts.getSize();
    int stride = sizeof(phys_vec4);

    phys_vec4 axes[3];
    axes[0].set(1.0f, 0.0f, 0.0f, 0.0f);
    axes[1].set(0.0f, 1.0f, 0.0f, 0.0f);
    axes[2].set(0.0f, 0.0f, 1.0f, 0.0f);
    phys_vec4 obb1_half_extents;
    phys_vec4 obb1_center;
    make_obb_from_axes(vertices_in, hull_vertex_count, stride, obb1_half_extents, axes, obb1_center);
    float obb1_volume = obb1_half_extents(0) * obb1_half_extents(1) * obb1_half_extents(2);

    phys_mat3 cov_mat;
    phys_vec4 mean;
    compute_mean_covariance(vertices_in, hull_vertex_count, stride, cov_mat, mean, phys_bool(false));
    phys_vec4 axes_alt[3];
    get_axes_from_covariance(cov_mat, axes_alt[0], axes_alt[1], axes_alt[2]);
    phys_vec4 obb2_half_extents;
    phys_vec4 obb2_center;
    make_obb_from_axes(vertices_in, hull_vertex_count, stride, obb2_half_extents, axes_alt, obb2_center);
    float obb2_volume = obb2_half_extents(0) * obb2_half_extents(1) * obb2_half_extents(2);

    if (mode == PHYS_OBB_MODE_BETTER_FIT_BUT_SLOWER)
    {
        obb1_volume = steepest_descent(vertices_in, hull_vertex_count, stride, axes, axes, 20);
        make_obb_from_axes(vertices_in, hull_vertex_count, stride, obb1_half_extents, axes, obb1_center);
        obb2_volume = steepest_descent(vertices_in, hull_vertex_count, stride, axes_alt, axes_alt, 20);
        make_obb_from_axes(vertices_in, hull_vertex_count, stride, obb2_half_extents, axes_alt, obb2_center);
    }

    phys_vec4 translation;
    phys_vec4 *best_axes;
    if (obb1_volume < obb2_volume)
    {
        best_axes = axes;
        box_half_extents = obb1_half_extents;
        translation = obb1_center;
    }
    else
    {
        best_axes = axes_alt;
        box_half_extents = obb2_half_extents;
        translation = obb2_center;
    }
    phys_rotation rotation;
    rotation.set_cols(best_axes[0], best_axes[1], best_axes[2]);
    transform = phys_transform(rotation, translation);
}



