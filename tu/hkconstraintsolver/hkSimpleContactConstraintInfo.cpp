// hkconstraintsolver/hkSimpleContactConstraintInfo.obj: from the records 0x675160 0x6751e0 0x6751f8
// @compiler xdk4314
// @flags /TP /O2 /Oc
// @address 0x675160 ?compute_contact_rhs@@YAMPBVphys_contact_point@@AAVphys_contact_point_props@@MMAAH@Z
// @address 0x6751e0 phys_simple_contact_add_viscosity
// @address 0x6751f8 phys_simple_contact_build_jacobian
#include <ppcintrinsics.h>
#include <vectorintrinsics.h>
#include "phys/phys_vel_accum.h"
#include "phys/phys_math.h"
#include "phys/phys_collide.h"
#include "phys/phys_constraint_query_in.h"
#include "phys/phys_simple_contact_info.h"
#include "phys/phys_contact_point_props.h"
#include "phys/phys_jacobian_element.h"
#include "phys/phys_header_schema.h"
#include "phys/phys_atoms.h"
#include "phys/phys_contact_schema_types.h"

#include "phys/phys_constraint_query_out.h"
#include "phys/phys_contact_point.h"
#include "phys/phys_base_types.h"

typedef signed char phys_i8;
typedef float phys_real;
typedef unsigned char phys_u8;
typedef unsigned short phys_u16;
typedef unsigned int phys_u32;
typedef __vector4 phys_simd_vec;

// Port link (P5): globals owned by hkconstraintsolver:hkSimpleContactConstraintInfo.obj
// (data-owners.tsv). Contents match the reference image exactly (default.pe,
// file offset = rva): 0x7263fc = 3f800000 (1.0f), 0x726400 = 3d4ccccd (0.05f).
// @address 0x7263fc ?PHYS_CONTACT_LIN_ERROR_RECOVERY_VEL@@3MA
float PHYS_CONTACT_LIN_ERROR_RECOVERY_VEL = 1.0f;
// @address 0x726400 ?PHYS_CONTACT_EXP_ERROR_RECOVERY_VEL@@3MA
float PHYS_CONTACT_EXP_ERROR_RECOVERY_VEL = 0.05f;

namespace {

class phys_jacobian_builder {
public:
    static __forceinline void init_builder(const phys_vel_accum *mA, const phys_vel_accum *mB, phys_real inv_delta_time) {}
    static __forceinline void exit_builder() {}
    static __forceinline void copy_jac_reg_to_jac1() {}
    static __forceinline void build_lin_begin(const phys_vel_accum *mA, const phys_vel_accum *mB, const phys_vec4 &position,
        const phys_vec4 &direction, phys_real virt_mass_factor, phys_1lin2ang_jacobian &lin_ang_jac)
    {
        lin_ang_jac.m_lin_part_0 = direction;
        phys_vec4 dA; dA.set_sub4(position, mA->get_center_of_mass_in_world());
        phys_vec4 dB; dB.set_sub4(position, mB->get_center_of_mass_in_world());
        phys_vec4 h; h.set_cross(dA, direction);
        if (mA->m_mat_is_identity) {
            lin_ang_jac.m_ang_jacobian[0] = h;
        } else {
            lin_ang_jac.m_ang_jacobian[0].set_rotated_dir(mA->get_core_from_world(), h);
        }
        phys_vec4 g; g.set_cross(direction, dB);
        if (mB->m_mat_is_identity) {
            lin_ang_jac.m_ang_jacobian[1] = g;
        } else {
            lin_ang_jac.m_ang_jacobian[1].set_rotated_dir(mB->get_core_from_world(), g);
        }
        phys_real diag_term = lin_ang_jac.get_jacobian_diag(*mA, *mB);
        lin_ang_jac.set_user_value(diag_term);
        lin_ang_jac.set_inv_jac_diag(virt_mass_factor / diag_term);
    }
    static __forceinline void build_lin_end(phys_1lin2ang_jacobian &lin_ang_jac, phys_real rhs, phys_real rhs_factor) { lin_ang_jac.set_rhs(rhs * rhs_factor); }
    static __forceinline void build_ang_begin(const phys_vel_accum *mA, const phys_vel_accum *mB,
        const phys_vec4 &direction, phys_real virt_mass_factor, phys_2ang_jacobian &lin_ang_jac)
    {
        if (mA->m_mat_is_identity) {
            lin_ang_jac.m_ang_jacobian[0] = direction;
        } else {
            lin_ang_jac.m_ang_jacobian[0].set_rotated_dir(mA->get_core_from_world(), direction);
        }
        if (mB->m_mat_is_identity) {
            lin_ang_jac.m_ang_jacobian[1].set_neg4(direction);
        } else {
            phys_vec4 neg_dir; neg_dir.set_neg4(direction);
            lin_ang_jac.m_ang_jacobian[1].set_rotated_dir(mB->get_core_from_world(), neg_dir);
        }
        phys_real diag_term = lin_ang_jac.get_ang_diag(*mA, *mB);
        lin_ang_jac.set_inv_jac_diag(virt_mass_factor / diag_term);
    }
    static __forceinline void build_ang_end(phys_2ang_jacobian &lin_ang_jac, phys_real rhs, phys_real rhs_factor) { lin_ang_jac.set_ang_rhs(rhs * rhs_factor); }
    static __forceinline void mul_inv_jac_diag(phys_2ang_jacobian *lin_ang_jac,
        phys_real factor) { lin_ang_jac->set_inv_jac_diag(lin_ang_jac->get_inv_jac_diag() * factor); }
    static __forceinline phys_real inv_jac01_fast(
        const phys_vel_accum *mA, const phys_vel_accum *mB, phys_1lin2ang_jacobian *lin_ang_jac, phys_real virt_mass_factor, phys_real lever_factor)
    {
        phys_real non_diag = lin_ang_jac[1].get_off_diag_fast(*mA, *mB, lin_ang_jac[0]);
        phys_real diag_0 = lin_ang_jac[0].get_user_value();
        phys_real diag_axis1 = lin_ang_jac[1].get_user_value();
        non_diag *= lever_factor;
        phys_real contact_inv_det = virt_mass_factor / (diag_0 * diag_axis1 - non_diag * non_diag);
        lin_ang_jac[0].set_user_value(diag_axis1 * contact_inv_det);
        lin_ang_jac[1].set_user_value(diag_0 * contact_inv_det);
        return -non_diag * contact_inv_det;
    }
    static __forceinline void add_last_position(const phys_vec4 &position, const phys_vec4 &normal, phys_vec4 &position_sum, phys_vec4 &normal_sum) {
        position_sum.add4(position);
        normal_sum.add4(normal);
    }
};

} // anonymous namespace

// match: 0x6751f8: keep the contact tag store without the shared setter's address local.
static __forceinline void phys_simple_contact_info_set_type(phys_schema *jacobian_schema, phys_schema::joint_schema_kind type)
{
    *reinterpret_cast<phys_schema::joint_schema_kind *>(phys_add_byte_offset(jacobian_schema, PHYS_SCHEMA_TAG_OFFSET)) = type;
}
static __forceinline void phys_simple_contact_info_init_header(phys_header_schema *jacobian_schema,
    int schema_body_a_index, int schema_body_b_index, phys_solver_results *sr, int stride)
{
    phys_simple_contact_info_set_type(jacobian_schema, phys_schema::SCHEMA_KIND_HEADER);
    jacobian_schema->m_body_a_index = static_cast<unsigned short>(schema_body_a_index);
    jacobian_schema->m_body_b_index = static_cast<unsigned short>(schema_body_b_index);
    jacobian_schema->m_solver_results_main = sr;
    jacobian_schema->m_solver_result_stride = static_cast<unsigned char>(stride);
}


inline void phys_pair_contact_schema::init_pair_contact(const phys_double_bilateral_jacobian &lin_ang_jac, phys_real inv_jac01, int accessor_check)
{
    m_inv_jac01 = inv_jac01;
    schema_kind t = schema_kind(SCHEMA_KIND_PAIR_CONTACT + accessor_check);
    phys_simple_contact_info_set_type(this, t);
}

inline void phys_2d_friction_schema::init_2d_friction(const phys_double_bilateral_jacobian &lin_ang_jac,
    phys_real contact_max_friction, phys_real inv_jac01, phys_solver_results *sr)
{
    m_inv_jac01 = inv_jac01;
    m_max_friction_force = contact_max_friction;
    m_solver_results = sr;
    phys_simple_contact_info_set_type(this, SCHEMA_KIND_2D_FRICTION);
}

inline void phys_3d_friction_schema::init_3d_friction(const phys_triple_bilateral_ang_jacobian &lin_ang_jac,
    phys_real contact_max_friction, phys_real inv_jac01, phys_real radius, phys_solver_results *sr)
{
    m_inv_jac01 = inv_jac01;
    m_max_friction_force = contact_max_friction;
    m_friction_radius = radius;
    m_solver_results = sr;
    phys_simple_contact_info_set_type(this, SCHEMA_KIND_3D_FRICTION);
}

inline void phys_single_contact_schema::init_single_contact(const phys_1lin2ang_jacobian &lin_ang_jac, int request_accessor_check)
{
    schema_kind t = schema_kind(SCHEMA_KIND_SINGLE_CONTACT + request_accessor_check);
    phys_simple_contact_info_set_type(this, t);
}

static __forceinline phys_real compute_contact_rhs(
    const phys_contact_point *point, phys_contact_point_props &properties,
    phys_real velocity, phys_real body_delta_time, int &flags,
    phys_real neg_exp_recovery = -PHYS_CONTACT_EXP_ERROR_RECOVERY_VEL,
    phys_real lin_err_recovery_vel = PHYS_CONTACT_LIN_ERROR_RECOVERY_VEL)
{
    phys_real current_vel = properties.m_internal_data_a;
    phys_real rhs = velocity - current_vel;
    phys_real separating_vel = -properties.get_internal_solver_data();
    phys_real error = separating_vel - rhs;
    phys_real lin_recovery = body_delta_time * lin_err_recovery_vel;
    phys_real recovery = phys_math::min_of_two(current_vel * neg_exp_recovery, lin_recovery);
    current_vel += recovery;
    rhs -= recovery;
    if (error > lin_recovery + recovery * 2.0f) {
        current_vel -= error;
        rhs += error;
    }
    properties.m_internal_data_a = phys_math::min_of_two(current_vel, -1.1920929e-7f);
    return rhs;
}

phys_real compute_contact_rhs(const phys_contact_point *point, phys_contact_point_props &properties,
                    phys_real velocity, phys_real body_delta_time, int &flags)
{
    return compute_contact_rhs(point, properties, velocity, body_delta_time, flags,
        -PHYS_CONTACT_EXP_ERROR_RECOVERY_VEL, PHYS_CONTACT_LIN_ERROR_RECOVERY_VEL);
}

extern "C" void phys_simple_contact_add_viscosity(phys_simple_contact_data_info *info)
{
    info->m_internal_data1 = 0.0f;
    info->m_internal_data2 = 0.0f;
    info->m_rhs_rolling = 0.0f;
}

// @address 0x6751f8
extern "C" void phys_simple_contact_build_jacobian(
    phys_simple_contact_atom *atom, const phys_constraint_query_in &in, phys_bool write_header_schema, phys_constraint_query_out &out)
{
    int flags = 0;
    phys_contact_point *cp = atom->get_contact_points();
    int ray_contact_count = atom->m_contact_point_count;
    phys_contact_point_props *contact_props = atom->get_contact_point_props();
    phys_contact_point_props *first_props = contact_props;
    phys_simple_contact_data_info *info = &atom->m_bvh_info;

    phys_vec4 position_sum; position_sum.set_zero4();
    phys_vec4 normal_sum; normal_sum.set_zero4();
    phys_real impulse_sum = 0.0f;
    phys_real contact_cnt_real = phys_real(ray_contact_count);
    phys_real inv_contact_cnt = 1.0f / contact_cnt_real;

    phys_schema *jacobian_schema = out.m_jacobian_schemas;
    const phys_vel_accum *mA = in.m_body_a;
    const phys_vel_accum *mB = in.m_body_b;

    if (write_header_schema) {
        phys_header_schema *hs = reinterpret_cast<phys_header_schema *>(jacobian_schema);
        phys_simple_contact_info_init_header(hs, in.m_accum_a_index, in.m_accum_b_index, first_props, (int)sizeof(phys_contact_point_props));
        jacobian_schema = hs + 1;
    }
    phys_jacobian_builder::init_builder(mA, mB, in.m_substep_inv_dt);

    // match: 0x6751f8: pointer __restrict qualifier prevents alias reload
    phys_contact_point *__restrict c = cp;
    phys_contact_point_props *p = contact_props;
    int friction_sum = 0;
    int last_accessor_check = 0;
    for (int i = 1; i <= ray_contact_count; i++) {
        int request_accessor_check = 1 - (phys_u32(-(i & 7)) >> 31);
        int is_paired_point = (p->m_flags >> 1) & 1;
        // match: 0x6751f8: pointer __restrict qualifier prevents alias reload
        phys_1lin2ang_jacobian *__restrict lin_ang_jac = reinterpret_cast<phys_1lin2ang_jacobian *>(jacobian_schema);
        phys_jacobian_builder::build_lin_begin(mA, mB, c->get_contact_position(), c->get_contact_normal(), in.m_virt_mass_factor, *lin_ang_jac);
        friction_sum += p->get_friction_8_8();
        impulse_sum += p->get_applied_impulse();
        phys_real rhs = -compute_contact_rhs(c, *p, c->get_distance(), in.m_frame_dt, flags,
            -PHYS_CONTACT_EXP_ERROR_RECOVERY_VEL, PHYS_CONTACT_LIN_ERROR_RECOVERY_VEL);
        phys_jacobian_builder::build_lin_end(*lin_ang_jac, rhs, in.m_rhs_factor);
        phys_jacobian_builder::add_last_position(c->get_contact_position(), c->get_contact_normal(), position_sum, normal_sum);
        if (!is_paired_point) {
            phys_single_contact_schema *single = reinterpret_cast<phys_single_contact_schema *>(jacobian_schema);
            phys_jacobian_builder::copy_jac_reg_to_jac1();
            single->init_single_contact(*lin_ang_jac, request_accessor_check);
            jacobian_schema = single + 1;
        } else {
            phys_schema *s = jacobian_schema;
            phys_pair_contact_schema *pair = reinterpret_cast<phys_pair_contact_schema *>(reinterpret_cast<phys_1lin2ang_jacobian *>(s) - 1);
            phys_real lever_factor = 0.99f;
            phys_real inv_jac01 = phys_jacobian_builder::inv_jac01_fast(mA, mB, lin_ang_jac - 1, in.m_virt_mass_factor, lever_factor);
            pair->init_pair_contact(*reinterpret_cast<phys_double_bilateral_jacobian *>(lin_ang_jac - 1), inv_jac01,
                last_accessor_check + request_accessor_check);
            jacobian_schema = pair + 1;
        }
        p++;
        c++;
        last_accessor_check = request_accessor_check;
    }

    phys_real friction_factor = (1.0f / 256.0f) * phys_real(friction_sum);
    phys_real friction = impulse_sum * friction_factor * inv_contact_cnt;
    if (friction > 0.0f) {
        phys_real inv_contact_cnt_sq = inv_contact_cnt * inv_contact_cnt;
        phys_real length_sq_quad = normal_sum.length_sq_3d();
        if (length_sq_quad * inv_contact_cnt_sq > 0.9999f) {
            normal_sum = cp->get_contact_normal();
        } else if (length_sq_quad > 0.1f) {
            normal_sum.normalize_xyz();
        } else {
            normal_sum = cp->get_contact_normal();
            length_sq_quad = normal_sum.dot3(normal_sum);
            if (length_sq_quad < 0.9f) {
                friction = 0.0f;
                normal_sum.set_zero4();
                normal_sum(1) = 1.0f;
            }
        }

        phys_vec4 binormal;
        phys_vec4 tangent;
        binormal = phys_transform::get_identity().get_column(info->m_binormal_axis);
        tangent.set_cross(binormal, normal_sum);
        phys_simd_scalar tangent_len_sq = tangent.dot3(tangent);
        if (!(phys_real(tangent_len_sq) > 0.1f)) {
            phys_vec4 abs_normal; abs_normal.set_abs4(normal_sum);
            if (abs_normal(0) < abs_normal(1)) {
                info->m_binormal_axis = (abs_normal(0) < abs_normal(2)) ? phys_u16(0) : phys_u16(2);
            } else {
                info->m_binormal_axis = (abs_normal(1) < abs_normal(2)) ? phys_u16(1) : phys_u16(2);
            }
            info->m_internal_data1 = 0.0f;
            info->m_internal_data2 = 0.0f;
            binormal = phys_transform::get_identity().get_column(info->m_binormal_axis);
            tangent.set_cross(binormal, normal_sum);
        }
        tangent.normalize_xyz();
        binormal.set_cross(normal_sum, tangent);
        position_sum.mul4(phys_simd_scalar::create(inv_contact_cnt));

        phys_1lin2ang_jacobian *lin_ang_jac = reinterpret_cast<phys_1lin2ang_jacobian *>(jacobian_schema);
        phys_jacobian_builder::build_lin_begin(mA, mB, position_sum, binormal, in.m_virt_mass_factor, lin_ang_jac[0]);
        phys_jacobian_builder::build_lin_end(lin_ang_jac[0], info->m_internal_data1, in.m_friction_rhs_factor);
        phys_jacobian_builder::copy_jac_reg_to_jac1();
        phys_jacobian_builder::build_lin_begin(mA, mB, position_sum, tangent, in.m_virt_mass_factor, lin_ang_jac[1]);
        phys_jacobian_builder::build_lin_end(lin_ang_jac[1], info->m_internal_data2, in.m_friction_rhs_factor);

        phys_2d_friction_schema *friction_2d_schema = reinterpret_cast<phys_2d_friction_schema *>(jacobian_schema);
        phys_real inv_jac01 = phys_jacobian_builder::inv_jac01_fast(mA, mB, lin_ang_jac, in.m_virt_mass_factor, 1.0f);
        phys_real contact_max_friction = friction * in.m_inv_total_micro_steps;
        phys_solver_results *sr = reinterpret_cast<phys_solver_results *>(&info->m_rolling_friction_mul);
        friction_2d_schema->init_2d_friction(*reinterpret_cast<phys_double_bilateral_jacobian *>(lin_ang_jac), contact_max_friction, inv_jac01, sr);
        jacobian_schema = friction_2d_schema + 1;

        if (ray_contact_count > 1) {
            for (;;) {
                if (!(info->m_flags & phys_simple_contact_data_info::PHYS_FLAG_AREA_CHANGED)) {
                    phys_real inv_radius = 1.0f / info->m_contact_radius;
                    phys_jacobian_builder::build_ang_begin(mA, mB, normal_sum, in.m_virt_mass_factor, *reinterpret_cast<phys_2ang_jacobian *>(lin_ang_jac + 2));
                    phys_jacobian_builder::build_ang_end(*reinterpret_cast<phys_2ang_jacobian *>(lin_ang_jac + 2), info->m_rhs_rolling,
                        in.m_friction_rhs_factor);
                    phys_jacobian_builder::mul_inv_jac_diag(reinterpret_cast<phys_2ang_jacobian *>(lin_ang_jac + 2), inv_radius);
                    phys_3d_friction_schema *friction_3d_schema = reinterpret_cast<phys_3d_friction_schema *>(friction_2d_schema);
                    friction_3d_schema->init_3d_friction(*reinterpret_cast<phys_triple_bilateral_ang_jacobian *>(lin_ang_jac),
                        contact_max_friction, inv_jac01, info->m_contact_radius, sr);
                    jacobian_schema = friction_3d_schema + 1;
                    break;
                }
                info->m_contact_radius = 0.0f;
                // match: 0x6751f8: pointer __restrict qualifier prevents alias reload
                phys_contact_point *__restrict cc = cp;
                for (int k = ray_contact_count - 1; k >= 0; k--) {
                    phys_vec4 diff; diff.set_sub4(cc->get_contact_position(), position_sum);
                    info->m_contact_radius += phys_real(diff.length_3d());
                    cc++;
                }
                info->m_contact_radius *= inv_contact_cnt;
                if (info->m_contact_radius < 1e-6f) {
                    break;
                }
                info->m_flags &= ~phys_simple_contact_data_info::PHYS_FLAG_AREA_CHANGED;
            }
        }
    }
    out.m_jacobian_schemas = jacobian_schema;
    phys_jacobian_builder::exit_builder();
}

