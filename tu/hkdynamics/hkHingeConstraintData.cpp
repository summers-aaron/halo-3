// hkdynamics/hkHingeConstraintData.obj: from the records 0x64c7f0 0x64c808 0x64c838 0x64c848 0x64c850 0x64c9e0 0x64cc18 0x64cca8 0x64ccc8 0x64ccd8 0x64cda0
// @flags /TP /O2
// @address 0x64c7f0 ?destroy_loaded_hinge_data@@YAXPAX@Z
#include "phys/phys_base.h"
#include <vectorintrinsics.h>
#include <math.h>
#include "phys/phys_atoms.h"
#include "phys/phys_vec4_util.h"
// @address 0x64c808 ?get_runtime_info@phys_hinge_data@@UBAXVphys_bool@@AAUruntime_info@phys_constraint_data@@@Z
// @address 0x64c838 ?get_constraint_info@phys_hinge_data@@UBAXAAUconstraint_info@phys_constraint_data@@@Z
// @address 0x64c848 ?get_type_id@phys_hinge_data@@UBAHXZ
// @address 0x64cc18 ?is_valid_aabb@phys_hinge_data@@UBA?AVphys_bool@@XZ
// @address 0x64ccc8 ?get_hinge_data_vtable@@YAPBXXZ
// @address 0x64cca8 ?fixup_loaded_hinge_data@@YAXPAX@Z

// @address 0x5bec98 ?__first_virtual_table_function__@phys_object@@UAAXXZ
class phys_constraint;
class phys_solver_results;
// @address 0x5bec98 ?build_jacobian@phys_constraint_data@@UAAXABVphys_constraint_query_in@@AAVphys_constraint_query_out@@@Z
// @address 0x5bec98 ?build_jacobian_callback@phys_constraint_data@@UAAXABVphys_constraint_query_in@@@Z
// @address 0x64d650 ??_Ephys_hinge_data@@UAAPAXI@Z
#include "phys/phys_hinge_data.h"
#include "phys/phys_constraint_query_in.h"
#include "phys/phys_constraint_query_out.h"


void destroy_loaded_hinge_data(void *storage)
{
    ((phys_hinge_data *)storage)->~phys_hinge_data();
}

void phys_hinge_data::get_runtime_info(phys_bool want_runtime, phys_constraint_data::runtime_info &info) const
{
    if (want_runtime) {
        info.m_solver_result_count = 5;
        info.m_external_runtime_size = 40;
    } else {
        info.m_external_runtime_size = 0;
        info.m_solver_result_count = 0;
    }
}

void phys_hinge_data::get_constraint_info(phys_constraint_data::constraint_info &info) const
{
    phys_constraint_data::gather_constraint_info(m_data_atoms.get_atoms(), m_data_atoms.get_size_of_all_atoms(), info);
}

int phys_hinge_data::get_type_id() const
{
    return 1;
}

// @address 0x64cc18 ?is_valid_aabb@phys_hinge_data@@UBA?AVphys_bool@@XZ
phys_bool phys_hinge_data::is_valid_aabb() const
{
    return m_data_atoms.m_transform_atom.m_transform_a.get_rotation().is_orthonormal(0.001f)
        && m_data_atoms.m_transform_atom.m_transform_b.get_rotation().is_orthonormal(0.001f);
}

namespace {
}
void fixup_loaded_hinge_data(void *storage)
{
    phys_fixup_tag flag;
    new (storage) phys_hinge_data(flag);
}

const void *get_hinge_data_vtable()
{
    __declspec(align(16)) char storage[sizeof(phys_hinge_data)];
    phys_fixup_tag flag;
    phys_hinge_data *object = new (storage) phys_hinge_data(flag);
    return *reinterpret_cast<const void *const *>(object);
}

// match: 0x242920: Direct reciprocal square root preserves the shared constant anchor.
static __forceinline void phys_hinge_data_normalize3(phys_vec4 &v)
{
    __vector4 temp = __vmsum3fp(v.m_simd_quad, v.m_simd_quad);
    __vector4 scale;
    scale.v[0] = 1.0f / __fsqrts(temp.v[0]);
    scale = __vspltw(scale, 0);
    v.m_simd_quad = __vmulfp(v.m_simd_quad, scale);
}

// @address 0x64c9e0 ?set_in_body_space@phys_hinge_data@@QAAXABVphys_vec4@@000@Z
void phys_hinge_data::set_in_body_space(const phys_vec4 &joint_pivot_a, const phys_vec4 &joint_pivot_b, const phys_vec4 &axis_a, const phys_vec4 &axis_b)
{
#ifdef HALO3_64BIT
    m_data_atoms.m_transform_atom.m_transform_a.m_translation = joint_pivot_a;
    m_data_atoms.m_transform_atom.m_transform_b.m_translation = joint_pivot_b;
#else
    __stvx(joint_pivot_a.m_simd_quad, this, 80);
    __stvx(joint_pivot_b.m_simd_quad, this, 144);
#endif
    phys_vec4 *base_a = &m_data_atoms.m_transform_atom.m_transform_a.m_rotation.m_column0;
    phys_vec4 axis_a_unit;
    axis_a_unit.m_simd_quad = __lvx(&axis_a, 0);
    phys_hinge_data_normalize3(axis_a_unit);
    __stvx(axis_a_unit.m_simd_quad, base_a, 0);
    phys_vec4_util::compute_perpendicular_vec(base_a[0], base_a[1]);
    phys_hinge_data_normalize3(base_a[1]);
    base_a[2].set_cross(base_a[0], base_a[1]);
    phys_vec4 *base_b = &m_data_atoms.m_transform_atom.m_transform_b.m_rotation.m_column0;
    phys_vec4 axis_b_unit;
    axis_b_unit.m_simd_quad = __lvx(&axis_b, 0);
    phys_hinge_data_normalize3(axis_b_unit);
    __stvx(axis_b_unit.m_simd_quad, base_b, 0);
    phys_vec4_util::compute_perpendicular_vec(base_b[0], base_b[1]);
    phys_hinge_data_normalize3(base_b[1]);
    base_b[2].set_cross(base_b[0], base_b[1]);
}

// @address 0x64c850 ?set_in_world_space@phys_hinge_data@@QAAXABVphys_transform@@0ABVphys_vec4@@1@Z
void phys_hinge_data::set_in_world_space(const phys_transform &body_a_transform, const phys_transform &body_b_transform,
    const phys_vec4 &pivot, const phys_vec4 &axis)
{
    phys_vec4 perp_axle_1;
    phys_vec4 perp_axle_2;
    phys_vec4_util::compute_perpendicular_vec(axis, perp_axle_1);
    phys_hinge_data_normalize3(perp_axle_1);
    perp_axle_2.set_cross(axis, perp_axle_1);
    m_data_atoms.m_transform_atom.m_transform_a.m_rotation.m_column0.vec_set_rotated_inverse_dir(body_a_transform.m_rotation, axis);
    m_data_atoms.m_transform_atom.m_transform_a.m_rotation.m_column1.vec_set_rotated_inverse_dir(body_a_transform.m_rotation, perp_axle_1);
    m_data_atoms.m_transform_atom.m_transform_a.m_rotation.m_column2.vec_set_rotated_inverse_dir(body_a_transform.m_rotation, perp_axle_2);
    m_data_atoms.m_transform_atom.m_transform_a.m_translation.vec_set_transformed_inverse_pos(body_a_transform, pivot);
    m_data_atoms.m_transform_atom.m_transform_b.m_rotation.m_column0.vec_set_rotated_inverse_dir(body_b_transform.m_rotation, axis);
    m_data_atoms.m_transform_atom.m_transform_b.m_rotation.m_column1.vec_set_rotated_inverse_dir(body_b_transform.m_rotation, perp_axle_1);
    m_data_atoms.m_transform_atom.m_transform_b.m_rotation.m_column2.vec_set_rotated_inverse_dir(body_b_transform.m_rotation, perp_axle_2);
    m_data_atoms.m_transform_atom.m_transform_b.m_translation.vec_set_transformed_inverse_pos(body_b_transform, pivot);
}

// @address 0x64cda0 ?needs_jacobian_callback@phys_constraint_data@@UAA?AVphys_bool@@XZ
phys_bool phys_constraint_data::needs_jacobian_callback()
{
    return false;
}

// @address 0x64ccd8 ??0phys_hinge_data@@QAA@XZ
phys_hinge_data::phys_hinge_data()
{
    m_data_atoms.m_transform_atom.m_transform_a.set_to_identity();
    m_data_atoms.m_transform_atom.m_transform_b.set_to_identity();

    m_data_atoms.m_ang_2d.m_free_rot_axis = 0;
}

