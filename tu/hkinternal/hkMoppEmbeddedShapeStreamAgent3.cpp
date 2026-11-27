// hkinternal/hkMoppEmbeddedShapeStreamAgent3.obj: from the records 0x68cf70 0x68cf80 0x68cfd0 0x68d370 0x68d7b8 0x68d808
// @flags /TP /O2 /Oc
// @compiler xdk5632
// @address 0x68cf70 ?destroy@phys_bvh_embedded_stream_narrow_agent@@YAXPAUphys_agent_entry@@PAXPAVphys_contact_manager@@AAVphys_constraint_owner@@PAVphys_dispatcher@@@Z
// @address 0x68cf80 ?create@phys_bvh_embedded_stream_narrow_agent@@YAPAXABUphys_narrow_agent_input@@PAUphys_agent_entry@@PAX@Z
// @address 0x68cfd0 ?bvh_compute_bounds_query_tree@@YA?AW4phys_result@@ABVphys_collision_proxy@@0ABVphys_transform@@ABVphys_vec4@@ABUphys_process_collision_input@@PAVphys_aabb@@PAIAAH@Z
// @address 0x68d370 ?process@phys_bvh_embedded_stream_narrow_agent@@YAPAXABUphys_narrow_agent_process_input@@PAUphys_agent_entry@@PAXPAVphys_vec4@@AAUphys_process_collision_output@@@Z
// @address 0x68d7b8 ?init_agent_funcs@phys_bvh_embedded_stream_narrow_agent@@YAXAAUnarrow_agent_funcs@phys_dispatcher@@@Z
// @address 0x68d808 ?register_narrow_agent@phys_bvh_embedded_stream_narrow_agent@@YAXPAVphys_dispatcher@@@Z

#include "phys/phys_dynamics.h"
#include "phys/phys_batch_agent_machine.h"
#include "phys/phys_batch_agent_track.h"
#include "phys/phys_dispatcher.h"

#include "phys/phys_bvh_embedded_stream_narrow_agent.h"
#include "phys/phys_agent_entry.h"
#include "phys/phys_collide.h"
#include "phys/phys_agent.h"
#include "phys/phys_constraint_owner.h"
#include "phys/phys_collision_input.h"
#include "phys/phys_bvh_embedded_shape.h"
#include "phys/phys_algorithm_sort.h"
#include "phys/phys_quad_constants.h"
#include "phys/phys_local_buffer.h"
#include "phys/phys_swept_transform_util.h"

void phys_bvh_embedded_stream_narrow_agent::destroy(
    phys_agent_entry *entry, void *data, phys_contact_manager *agent_contact_manager, phys_constraint_owner &owner, phys_dispatcher *dispatcher)
{
    phys_batch_agent_destroy(*static_cast<phys_batch_agent_track *>(data), dispatcher, agent_contact_manager, owner);
}

void *phys_bvh_embedded_stream_narrow_agent::create(const phys_narrow_agent_input &input, phys_agent_entry *entry, void *data)
{
    if (data != 0) {
        new (data) phys_batch_agent_track;
    }
    phys_batch_agent_create(data);
    return (char *)data + 16;
}

static __forceinline void phys_bvh_embedded_stream_set_cross(phys_vec4 &out, const phys_vec4 &x, const phys_vec4 &y)
{
    const unsigned int mask_lo_yzx = 0x63;
    const unsigned int swiz_two_rhs = 0x87;

    __vector4 lhs_one_tmp = __vpermwi(x.m_simd_quad, mask_lo_yzx);
    __vector4 rhs_reg_op = __vpermwi(y.m_simd_quad, swiz_two_rhs);
    __vector4 swiz_prod_lo = __vmulfp(lhs_one_tmp, rhs_reg_op);

    lhs_one_tmp = __vpermwi(x.m_simd_quad, swiz_two_rhs);
    rhs_reg_op = __vpermwi(y.m_simd_quad, mask_lo_yzx);
    __vector4 vec_mul_out = __vmulfp(lhs_one_tmp, rhs_reg_op);

    out.m_simd_quad = __vsubfp(swiz_prod_lo, vec_mul_out);
}

static __forceinline void phys_bvh_embedded_stream_set_all3(phys_vec4 &out, float a)
{
    out.m_simd_quad.v[0] = a;
    out.m_simd_quad = __vspltw(out.m_simd_quad, 0);
}

static __forceinline void phys_bvh_embedded_stream_set_neg4(phys_vec4 &out, const phys_vec4 &x)
{
    out.m_simd_quad = __vxor(x.m_simd_quad, g_phys_quad_sign_mask);
}

// @address 0x68cfd0
__forceinline phys_result bvh_compute_bounds_query_tree(const phys_collision_proxy &agent_body_a,
    const phys_collision_proxy &agent_body_b, const phys_transform &b_from_a,
                                const phys_vec4 &lin_sweep_info, const phys_process_collision_input &input,
                                phys_aabb *cached_aabb, phys_shape_key *out_hit_list, int &hit_cnt_out)
{
    phys_aabb aabb;
    {
        const phys_motion_state *motion_a_state = agent_body_a.get_motion_state();
        const phys_motion_state *motion_b_state = agent_body_b.get_motion_state();

        phys_vec4 sweep_info;
        sweep_info.set_rotated_inverse_dir(agent_body_b.get_transform().m_rotation, lin_sweep_info);

        phys_vec4 aabb_extents;
        if (input.m_quality_info->m_use_ccd) {
            const float second_order_err_a = (motion_a_state->m_delta_angle(3) + motion_b_state->m_delta_angle(3)) * motion_a_state->m_object_radius;
            const float second_order_err_b = motion_b_state->m_delta_angle(3) * motion_b_state->m_delta_angle(3) * motion_b_state->m_object_radius;
            const float check_epsilon = input.m_box_tolerance * 0.5f + (second_order_err_a + second_order_err_b);
            agent_body_a.get_shape()->get_handle_aabb(b_from_a, check_epsilon, aabb);

            phys_vec4 mass_center_a_in_b;
            {
                phys_vec4 radius_vec4;
                phys_bvh_embedded_stream_set_all3(radius_vec4, motion_a_state->m_object_radius + input.m_box_tolerance * 0.5f + second_order_err_b);
                mass_center_a_in_b.set_transformed_inverse_pos(agent_body_b.get_transform(), motion_a_state->get_swept_transform().m_center_of_mass1);
                phys_vec4 max_reach;
                max_reach.set_add4(mass_center_a_in_b, radius_vec4);
                phys_vec4 min_radius_corner;
                min_radius_corner.set_sub4(mass_center_a_in_b, radius_vec4);
                aabb.m_min_limit.set_max4(aabb.m_min_limit, min_radius_corner);
                aabb.m_max_limit.set_min4(aabb.m_max_limit, max_reach);
            }

            aabb_extents.set_sub4(aabb.m_max_limit, aabb.m_min_limit);

            {
                if (motion_b_state->m_delta_angle(3) > 0.0f) {
                    phys_vec4 rel_pos;
                    rel_pos.set_sub4(mass_center_a_in_b, motion_b_state->get_swept_transform().m_center_of_mass_local);
                    phys_vec4 offset_out;
                    phys_bvh_embedded_stream_set_cross(offset_out, rel_pos, motion_b_state->m_delta_angle);
                    sweep_info.add_scaled4(input.m_step_info.m_delta_time * motion_b_state->get_swept_transform().get_inv_delta_time(), offset_out);
                }

                phys_vec4 zero;
                zero.set_zero4();
                phys_vec4 min_path;
                min_path.set_min4(zero, sweep_info);
                phys_vec4 bvh_max_path;
                bvh_max_path.set_max4(zero, sweep_info);
                aabb.m_min_limit.add4(min_path);
                aabb.m_max_limit.add4(bvh_max_path);
            }
        } else {
            const float check_epsilon = input.m_box_tolerance * 0.5f;
            agent_body_a.get_shape()->get_handle_aabb(b_from_a, check_epsilon, aabb);
            aabb_extents.set_sub4(aabb.m_max_limit, aabb.m_min_limit);
        }

        if (cached_aabb) {
            if (cached_aabb->contains(aabb)) {
                return PHYS_FAILURE;
            }

            phys_vec4 zero;
            zero.set_zero4();
            phys_vec4 min_path;
            min_path.set_min4(zero, sweep_info);
            phys_vec4 bvh_max_path;
            bvh_max_path.set_max4(zero, sweep_info);

            phys_vec4 expand_vec;
            phys_bvh_embedded_stream_set_all3(expand_vec, input.m_box_tolerance * 0.5f);
            aabb.m_min_limit.sub4(expand_vec);
            aabb.m_max_limit.add4(expand_vec);

            const float max_expand = 0.4f;
            const float frames_look_ahead = -2.0f;

            phys_vec4 min_extent_path;
            min_extent_path.set_mul4(frames_look_ahead, bvh_max_path);
            phys_vec4 max_extent_path;
            max_extent_path.set_mul4(frames_look_ahead, min_path);

            phys_vec4 max_expand_vec;
            max_expand_vec.set_mul4(max_expand, aabb_extents);
            max_extent_path.set_min4(max_extent_path, max_expand_vec);
            phys_vec4 min_expand4;
            phys_bvh_embedded_stream_set_neg4(min_expand4, max_expand_vec);
            min_extent_path.set_max4(min_extent_path, min_expand4);

            aabb.m_min_limit.add4(min_extent_path);
            aabb.m_max_limit.add4(max_extent_path);
            *cached_aabb = aabb;
        }
    }

    const phys_bvh_embedded_shape *bvh_shape_b = static_cast<const phys_bvh_embedded_shape *>(agent_body_b.get_shape());
    hit_cnt_out = bvh_shape_b->query_aabb(aabb, out_hit_list, 512);
    phys_algorithm::quick_sort(out_hit_list, hit_cnt_out);
    out_hit_list[hit_cnt_out] = PHYS_INVALID_SHAPE_KEY;
    return PHYS_SUCCESS;
}

// @address 0x68d370
void *phys_bvh_embedded_stream_narrow_agent::process(
    const phys_narrow_agent_process_input &input, phys_agent_entry *entry, void *agent_data, phys_vec4 *simplex_separating_normal,
        phys_process_collision_output &output)
{
    phys_narrow_agent_process_input modified_input;
    modified_input.m_body_a = input.m_body_a;
    modified_input.m_body_b = input.m_body_b;
    modified_input.m_contact_manager = input.m_contact_manager;
    modified_input.m_input = input.m_input;

    const phys_motion_state *motion_a_state = input.m_body_a->get_motion_state();
    const phys_motion_state *motion_b_state = input.m_body_b->get_motion_state();
    phys_swept_transform_util::compute_sweep_info(*motion_a_state, *motion_b_state, input.m_input->m_step_info.m_delta_time, modified_input.m_lin_sweep_info);
    modified_input.m_a_to_b.set_mul_inverse_mul(motion_a_state->get_transform(), motion_b_state->get_transform());

    phys_local_buffer<phys_shape_key> hit_key_list(512);
    int bvh_hit_count = 0;
    phys_result result;
    {
        phys_transform b_from_a;
        b_from_a.set_to_inverse(modified_input.m_a_to_b);
        result = bvh_compute_bounds_query_tree(*input.m_body_a, *input.m_body_b, b_from_a, modified_input.m_lin_sweep_info,
            *input.m_input, 0, hit_key_list.begin(), bvh_hit_count);
    }

    phys_batch_agent_track *batch_agent_track = static_cast<phys_batch_agent_track *>(agent_data);
    const phys_shape_container *shape_container = input.m_body_b->get_shape()->get_shape_container();

    if (result == PHYS_SUCCESS) {
        phys_batch_agent_track *batch_agent_track_ppu =
            reinterpret_cast<phys_batch_agent_track *>(reinterpret_cast<char *>(batch_agent_track) + input.m_sector_offset_delta);
        phys_batch_agent_process(*batch_agent_track, batch_agent_track_ppu, modified_input, shape_container, hit_key_list.begin(), bvh_hit_count, output);
    } else {
        phys_batch_agent_track *batch_agent_track_ppu =
            reinterpret_cast<phys_batch_agent_track *>(reinterpret_cast<char *>(batch_agent_track) + input.m_sector_offset_delta);
        phys_batch_agent_process(*batch_agent_track, batch_agent_track_ppu, modified_input, shape_container, 0, 0, output);
    }

    return reinterpret_cast<char *>(agent_data) + 16;
}

void phys_bvh_embedded_stream_narrow_agent::init_agent_funcs(phys_dispatcher::narrow_agent_funcs &functions)
{
    functions.create = phys_bvh_embedded_stream_narrow_agent::create;
    functions.process = phys_bvh_embedded_stream_narrow_agent::process;
    functions.compute_sep_normal = 0;
    functions.cleanup = 0;
    functions.bridge_remove_point = 0;
    functions.commit_potential = 0;
    functions.mark_zombie_entry = 0;
    functions.update_agent_filter = 0;
    functions.destroy = phys_bvh_embedded_stream_narrow_agent::destroy;
    functions.agent_is_predictive = 1;
}

void phys_bvh_embedded_stream_narrow_agent::register_narrow_agent(phys_dispatcher *dispatcher)
{
    phys_dispatcher::narrow_agent_funcs functions;
    phys_bvh_embedded_stream_narrow_agent::init_agent_funcs(functions);
    dispatcher->register_narrow_agent(functions, (phys_shape_type)-1, PHYS_SHAPE_BVH_EMBEDDED);
}

