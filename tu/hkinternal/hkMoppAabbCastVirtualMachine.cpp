// hkinternal/hkMoppAabbCastVirtualMachine.obj: from the records 0x69ddd0 0x69ef30
// @compiler xdk5632
// @flags /TP /O2 /Oc
// @address 0x69ddd0 ?query_ray_on_tree@phys_bvh_aabb_cast_vm@@IAAXPBUphys_bvh_aabb_cast_vm_query_int@@PBEQAUphys_bvh_aabb_cast_vm_query_float@@H@Z
#include "phys/phys_base.h"
#include "phys/phys_collide.h"
#include "phys/phys_solver_check.h"
#include "phys/phys_out_stream.h"
#include "phys/phys_error.h"


#include "phys/phys_dispatcher.h"


#include "phys/phys_collision_input.h"



#include "phys/phys_err_stream.h"


extern "C" {
// @address 0x7ceb7d g_havok_flyingcolors_mopp_aabb_cast
char g_havok_flyingcolors_mopp_aabb_cast;
}

#include "phys/phys_bvh_vm.h"

#include "phys/phys_bvh_commands.h"

#include "phys/phys_bvh_aabb_cast_vm.h"





__forceinline void phys_bvh_aabb_cast_vm::add_cast_hit(unsigned int key, const unsigned int properties[1], unsigned long chunk_id)
{
    const phys_shape_container *container = static_cast<const phys_bvh_shape *>(m_input->m_bvh_proxy->get_shape())->get_shape_container();

    {
        if (!m_input->m_collision_input->m_compound_filter->allows_collision(*m_input->m_collision_input, *m_input->m_cast_proxy,
            *m_input->m_bvh_proxy, *container, key))
        {
            return;
        }
    }

    phys_shape_buffer shape_scratch_buffer;
    const phys_shape *shape = container->get_child_shape(key, shape_scratch_buffer);

    phys_collision_proxy body(m_input->m_bvh_proxy);
    body.set_proxy_shape(shape, key);

    phys_shape_type shape_type_b = shape->get_type_id();
    phys_dispatcher::agent_funcs::sweep_cast_fn dispatch_linear_cast_fn =
        m_input->m_collision_input->m_agent_dispatcher->get_linear_cast_fn(m_cast_object_type, shape_type_b);
    dispatch_linear_cast_fn(*m_input->m_cast_proxy, body, *m_input->m_collision_input, *m_cast_collector, m_start_hit_collector);
    m_early_hit_frac = m_cast_collector->get_early_out_distance();
}

// @address 0x69ddd0
void phys_bvh_aabb_cast_vm::query_ray_on_tree(const phys_bvh_aabb_cast_vm_query_int *query, const unsigned char *PC,
    phys_bvh_aabb_cast_vm_query_float *const float_query, int bvh_chunk_offset)
{
    phys_bvh_aabb_cast_vm_query_int scaled_query;
    unsigned int jump_offset_high;
    unsigned int jump_offset_low;

    while (1)
    {
        const phys_bvh_commands command = phys_bvh_commands(PC[0]);
        int bvh_scale_index = 999;
        float lo;
        float hi;
        float from;
        float to;
        unsigned int bvh_left_offset;
        unsigned int bvh_right_offset;

        switch (command)
        {
        case PHYS_BVH_SPLIT_YZ:
            {
                hi = float(PC[1] * 2);
                lo = float(PC[2] * 2);
                const float extent = 2.0f * (query->m_cast_extents(1) + query->m_cast_extents(2));
                lo -= extent;
                hi += extent;
                from = float_query->m_ray_ends[0](1) + float_query->m_ray_ends[0](2);
                to = float_query->m_ray_ends[1](1) + float_query->m_ray_ends[1](2);
                break;
            }
        case PHYS_BVH_SPLIT_YMZ:
            {
                hi = float(PC[1] * 2 - 255);
                lo = float(PC[2] * 2 - 255);
                const float extent = 2.0f * (query->m_cast_extents(1) + query->m_cast_extents(2));
                lo -= extent;
                hi += extent;
                from = float_query->m_ray_ends[0](1) - float_query->m_ray_ends[0](2);
                to = float_query->m_ray_ends[1](1) - float_query->m_ray_ends[1](2);
                break;
            }
        case PHYS_BVH_SPLIT_XZ:
            {
                hi = float(PC[1] * 2);
                lo = float(PC[2] * 2);
                const float extent = 2.0f * (query->m_cast_extents(0) + query->m_cast_extents(2));
                lo -= extent;
                hi += extent;
                from = float_query->m_ray_ends[0](0) + float_query->m_ray_ends[0](2);
                to = float_query->m_ray_ends[1](0) + float_query->m_ray_ends[1](2);
                break;
            }
        case PHYS_BVH_SPLIT_XMZ:
            {
                hi = float(PC[1] * 2 - 255);
                lo = float(PC[2] * 2 - 255);
                const float extent = 2.0f * (query->m_cast_extents(0) + query->m_cast_extents(2));
                lo -= extent;
                hi += extent;
                from = float_query->m_ray_ends[0](0) - float_query->m_ray_ends[0](2);
                to = float_query->m_ray_ends[1](0) - float_query->m_ray_ends[1](2);
                break;
            }
        case PHYS_BVH_SPLIT_XY:
            {
                hi = float(PC[1] * 2);
                lo = float(PC[2] * 2);
                const float extent = 2.0f * (query->m_cast_extents(0) + query->m_cast_extents(1));
                lo -= extent;
                hi += extent;
                from = float_query->m_ray_ends[0](0) + float_query->m_ray_ends[0](1);
                to = float_query->m_ray_ends[1](0) + float_query->m_ray_ends[1](1);
                break;
            }
        case PHYS_BVH_SPLIT_XMY:
            {
                hi = float(PC[1] * 2 - 255);
                lo = float(PC[2] * 2 - 255);
                const float extent = 2.0f * (query->m_cast_extents(0) + query->m_cast_extents(1));
                lo -= extent;
                hi += extent;
                from = float_query->m_ray_ends[0](0) - float_query->m_ray_ends[0](1);
                to = float_query->m_ray_ends[1](0) - float_query->m_ray_ends[1](1);
                break;
            }
        case PHYS_BVH_SPLIT_XYZ:
            {
                hi = float(PC[1] * 3);
                lo = float(PC[2] * 3);
                const float extent = query->m_extents_sum3;
                lo -= extent;
                hi += extent;
                from = float_query->m_ray_ends[0](0) + float_query->m_ray_ends[0](1) + float_query->m_ray_ends[0](2);
                to = float_query->m_ray_ends[1](0) + float_query->m_ray_ends[1](1) + float_query->m_ray_ends[1](2);
                break;
            }
        case PHYS_BVH_SPLIT_XYMZ:
            {
                hi = float(PC[1] * 3 - 255);
                lo = float(PC[2] * 3 - 255);
                const float extent = query->m_extents_sum3;
                lo -= extent;
                hi += extent;
                from = float_query->m_ray_ends[0](0) + float_query->m_ray_ends[0](1) - float_query->m_ray_ends[0](2);
                to = float_query->m_ray_ends[1](0) + float_query->m_ray_ends[1](1) - float_query->m_ray_ends[1](2);
                break;
            }
        case PHYS_BVH_SPLIT_XMYZ:
            {
                hi = float(PC[1] * 3 - 255);
                lo = float(PC[2] * 3 - 255);
                const float extent = query->m_extents_sum3;
                lo -= extent;
                hi += extent;
                from = float_query->m_ray_ends[0](0) - float_query->m_ray_ends[0](1) + float_query->m_ray_ends[0](2);
                to = float_query->m_ray_ends[1](0) - float_query->m_ray_ends[1](1) + float_query->m_ray_ends[1](2);
                break;
            }
        case PHYS_BVH_SPLIT_XMYMZ:
            {
                hi = float(PC[1] * 3 - 510);
                lo = float(PC[2] * 3 - 510);
                const float extent = query->m_extents_sum3;
                lo -= extent;
                hi += extent;
                from = float_query->m_ray_ends[0](0) - float_query->m_ray_ends[0](1) - float_query->m_ray_ends[0](2);
                to = float_query->m_ray_ends[1](0) - float_query->m_ray_ends[1](1) - float_query->m_ray_ends[1](2);
                break;
            }
            // Single splits and split jumps decode their own branch offsets.
            do
            {
            case PHYS_BVH_SINGLE_SPLIT_X:
            case PHYS_BVH_SINGLE_SPLIT_Y:
            case PHYS_BVH_SINGLE_SPLIT_Z:
                bvh_scale_index = command - PHYS_BVH_SINGLE_SPLIT_X;
                lo = float(PC[1]);
                hi = lo + 1.0f;
                lo -= query->m_cast_extents(bvh_scale_index);
                hi += query->m_cast_extents(bvh_scale_index);
                from = (&float_query->m_ray_ends[0](0))[bvh_scale_index];
                to = (&float_query->m_ray_ends[1](0))[bvh_scale_index];
                PC += 3;
                bvh_left_offset = 0;
                bvh_right_offset = PC[-1];
                break;
            case PHYS_BVH_SPLIT_JUMP_X:
            case PHYS_BVH_SPLIT_JUMP_Y:
            case PHYS_BVH_SPLIT_JUMP_Z:
                bvh_scale_index = command - PHYS_BVH_SPLIT_JUMP_X;
                hi = float(PC[1]);
                lo = float(PC[2]);
                lo -= query->m_cast_extents(bvh_scale_index);
                hi += query->m_cast_extents(bvh_scale_index);
                from = (&float_query->m_ray_ends[0](0))[bvh_scale_index];
                to = (&float_query->m_ray_ends[1](0))[bvh_scale_index];
                bvh_left_offset = (PC[3] << 8) + PC[4];
                PC += 7;
                bvh_right_offset = (PC[-2] << 8) + PC[-1];
            } while (false);
            goto split_compare;
        case PHYS_BVH_SPLIT_X:
        case PHYS_BVH_SPLIT_Y:
        case PHYS_BVH_SPLIT_Z:
            bvh_scale_index = command - PHYS_BVH_SPLIT_X;
            hi = float(PC[1]);
            lo = float(PC[2]);
            lo -= query->m_cast_extents(bvh_scale_index);
            hi += query->m_cast_extents(bvh_scale_index);
            from = (&float_query->m_ray_ends[0](0))[bvh_scale_index];
            to = (&float_query->m_ray_ends[1](0))[bvh_scale_index];
            break;
            // Both double-cut encodings use the same ray clipping path.
            do
            {
            case PHYS_BVH_DOUBLE_CUT24_X:
            case PHYS_BVH_DOUBLE_CUT24_Y:
            case PHYS_BVH_DOUBLE_CUT24_Z:
                bvh_scale_index = command - PHYS_BVH_DOUBLE_CUT24_X;
                lo = float(read_u24(PC + 1)) * m_int_to_float_scale;
                hi = float(read_u24(PC + 4)) * m_int_to_float_scale;
                lo = lo * query->m_float_to_byte_scale;
                hi = hi * query->m_float_to_byte_scale;
                lo = lo - query->m_float_to_byte_offset(bvh_scale_index);
                hi = hi - query->m_float_to_byte_offset(bvh_scale_index);
                lo -= query->m_cast_extents(bvh_scale_index);
                hi += query->m_cast_extents(bvh_scale_index);
                PC += 7;
                break;
            case PHYS_BVH_DOUBLE_CUT_X:
            case PHYS_BVH_DOUBLE_CUT_Y:
            case PHYS_BVH_DOUBLE_CUT_Z:
                bvh_scale_index = command - PHYS_BVH_DOUBLE_CUT_X;
                lo = float(PC[1]);
                hi = float(PC[2]);
                lo -= query->m_cast_extents(bvh_scale_index);
                hi += query->m_cast_extents(bvh_scale_index);
                PC += 3;
            } while (false);
            {
                from = float_query->m_ray_ends[0](bvh_scale_index);
                to = float_query->m_ray_ends[1](bvh_scale_index);
                int flip;
                if (from < to)
                {
                    if (to < lo || from > hi)
                    {
                        return;
                    }
                    flip = 0;
                }
                else
                {
                    if (from < lo || to > hi)
                    {
                        return;
                    }
                    flip = 1;
                }
                phys_vec4 ray_from = float_query->m_ray_ends[0];
                phys_vec4 ray_to = float_query->m_ray_ends[1];
                {
                    const float a = from - hi;
                    const float b = to - hi;
                    if (a * b < 0.0f)
                    {
                        const float t = a / (a - b);
                        float_query->m_ray_ends[1 - flip].set_lerp4(ray_from, ray_to, phys_simd_scalar(t));
                    }
                }
                {
                    const float a = from - lo;
                    const float b = to - lo;
                    if (a * b < 0.0f)
                    {
                        const float t = a / (a - b);
                        float_query->m_ray_ends[flip].set_lerp4(ray_from, ray_to, phys_simd_scalar(t));
                    }
                }
                continue;
            }
        case PHYS_BVH_JUMP8:
            jump_offset_low = PC[1];
            PC += 2;
            PC += jump_offset_low;
            continue;
        case PHYS_BVH_JUMP16:
            jump_offset_high = PC[1];
            jump_offset_low = PC[2];
            PC += 3;
            PC += (jump_offset_high << 8) + jump_offset_low;
            continue;
        case PHYS_BVH_JUMP24:
            {
                jump_offset_high = PC[1];
                const unsigned int jump_offset_mid = PC[2];
                jump_offset_low = PC[3];
                PC += 4;
                PC += (jump_offset_high << 16) + (jump_offset_mid << 8) + jump_offset_low;
                continue;
            }
        case PHYS_BVH_JUMP_CHUNK:
            bvh_chunk_offset = (PC[1] << 8) | PC[2];
            PC = m_bvh_code->m_data.begin() + (bvh_chunk_offset << 9);
            continue;
        case PHYS_BVH_DATA_OFFSET:
            PC += 5;
            continue;
        case PHYS_BVH_SCALE1:
        case PHYS_BVH_SCALE2:
        case PHYS_BVH_SCALE3:
        case PHYS_BVH_SCALE4:
            {
                const int shift = command;
                phys_vec4 offset;
                offset.set(float(PC[1]), float(PC[2]), float(PC[3]), 0.0f);
                phys_vec4 scale;
                const float scale_factor = float(1 << shift);
                scale.set_all3(scale_factor);
                float_query->m_ray_ends[0].sub4(offset);
                float_query->m_ray_ends[1].sub4(offset);
                float_query->m_ray_ends[0].mul4(scale);
                float_query->m_ray_ends[1].mul4(scale);
                scaled_query.m_float_to_byte_offset.set_add4(query->m_float_to_byte_offset, offset);
                scaled_query.m_float_to_byte_offset.mul4(scale);
                scaled_query.m_bit_shift = query->m_bit_shift + shift;
                scaled_query.m_float_to_byte_scale = query->m_float_to_byte_scale * scale_factor;
                scaled_query.m_cast_extents.set_mul4(scale, query->m_cast_extents);
                scaled_query.m_extents_sum3 = query->m_extents_sum3 * scale_factor;
                for (int i = 0; i < 1; i++)
                {
                    scaled_query.m_properties[i] = query->m_properties[i];
                }
                scaled_query.m_primitive_offset = query->m_primitive_offset;
                query = &scaled_query;
                PC += 4;
                continue;
            }
        case PHYS_BVH_RETURN:
            return;
            // All terminal encodings use the same hit path.
            do
            {
            case PHYS_BVH_TERM8:
                jump_offset_low = PC[1];
                break;
            case PHYS_BVH_TERM16:
                jump_offset_high = PC[1];
                jump_offset_high <<= 8;
                jump_offset_low = PC[2];
                jump_offset_low += jump_offset_high;
                break;
            case PHYS_BVH_TERM24:
                jump_offset_low = PC[1];
                jump_offset_low <<= 16;
                jump_offset_high = PC[2];
                jump_offset_high <<= 8;
                jump_offset_high += PC[3];
                jump_offset_low += jump_offset_high;
                break;
            case PHYS_BVH_TERM32:
                jump_offset_low = PC[1];
                jump_offset_low <<= 24;
                jump_offset_high = PC[2];
                jump_offset_high <<= 16;
                jump_offset_low += jump_offset_high;
                jump_offset_high = PC[3];
                jump_offset_low += PC[4];
                jump_offset_high <<= 8;
                jump_offset_low += jump_offset_high;
                break;
            case 0x30: case 0x31: case 0x32: case 0x33: case 0x34: case 0x35: case 0x36: case 0x37:
            case 0x38: case 0x39: case 0x3a: case 0x3b: case 0x3c: case 0x3d: case 0x3e: case 0x3f:
            case 0x40: case 0x41: case 0x42: case 0x43: case 0x44: case 0x45: case 0x46: case 0x47:
            case 0x48: case 0x49: case 0x4a: case 0x4b: case 0x4c: case 0x4d: case 0x4e: case 0x4f:
                jump_offset_low = command - PHYS_BVH_TERM4_0;
            } while (false);
            {
                jump_offset_low += query->m_primitive_offset;
                const unsigned int key = (bvh_chunk_offset << 8) | jump_offset_low;
                add_cast_hit(key, query->m_properties);
                return;
            }
        case PHYS_BVH_TERM_REOFFSET8:
            {
                const unsigned int offset = PC[1];
                if (query != &scaled_query)
                {
                    scaled_query = *query;
                    query = &scaled_query;
                }
                scaled_query.m_primitive_offset = scaled_query.m_primitive_offset + offset;
                PC += 2;
                continue;
            }
        case PHYS_BVH_TERM_REOFFSET16:
            {
                const unsigned int offset = (PC[1] << 8) + PC[2];
                if (query != &scaled_query)
                {
                    scaled_query = *query;
                    query = &scaled_query;
                }
                scaled_query.m_primitive_offset = scaled_query.m_primitive_offset + offset;
                PC += 3;
                continue;
            }
        case PHYS_BVH_TERM_REOFFSET32:
            {
                const unsigned int offset = (PC[1] << 24) + (PC[2] << 16) + (PC[3] << 8) + PC[4];
                if (query != &scaled_query)
                {
                    scaled_query = *query;
                    query = &scaled_query;
                }
                scaled_query.m_primitive_offset = offset;
                PC += 5;
                continue;
            }
            // All property encodings share the query copy.
            do
            {
            case PHYS_BVH_PROPERTY8_0:
            case PHYS_BVH_PROPERTY8_1:
            case PHYS_BVH_PROPERTY8_2:
            case PHYS_BVH_PROPERTY8_3:
                {
                    const unsigned int property = PC[0] - PHYS_BVH_PROPERTY8_0;
                    const unsigned int value = PC[1];
                    PC += 2;
                    scaled_query.m_properties[property] = value;
                    break;
                }
            case PHYS_BVH_PROPERTY16_0:
            case PHYS_BVH_PROPERTY16_1:
            case PHYS_BVH_PROPERTY16_2:
            case PHYS_BVH_PROPERTY16_3:
                {
                    const unsigned int property = PC[0] - PHYS_BVH_PROPERTY16_0;
                    const unsigned int value = (PC[1] << 8) + PC[2];
                    scaled_query.m_properties[property] = value;
                    PC += 3;
                    break;
                }
            case PHYS_BVH_PROPERTY32_0:
            case PHYS_BVH_PROPERTY32_1:
            case PHYS_BVH_PROPERTY32_2:
            case PHYS_BVH_PROPERTY32_3:
                {
                    const unsigned int property = PC[0] - PHYS_BVH_PROPERTY32_0;
                    const unsigned int value = (PC[1] << 24) + (PC[2] << 16) + (PC[3] << 8) + PC[4];
                    scaled_query.m_properties[property] = value;
                    PC += 5;
                }
            } while (false);
            {
                const unsigned int v = scaled_query.m_properties[0];
                if (query != &scaled_query)
                {
                    scaled_query = *query;
                    query = &scaled_query;
                }
                scaled_query.m_properties[0] = v;
                continue;
            }
        default:
            {
                char assert_buf[512];
                phys_err_stream report_stream(assert_buf, sizeof(assert_buf));
                report_stream << "Unknown command - This mopp data has been corrupted (check for memory trashing), "
                     "or an hkMoppBvTreeShape has been pointed at invalid mopp data.\n";
                phys_singleton<phys_error>::s_instance->message(
                    phys_error::MESSAGE_ERROR, 0x1298fedd, assert_buf, ".\\collide\\mopp\\machine\\hkMoppAabbCastVirtualMachine.cpp", 467);
                continue;
            }
        }

        PC += 4;
        bvh_left_offset = 0;
        bvh_right_offset = PC[-1];
split_compare:
        if (to < lo && from < lo)
        {
            PC += bvh_left_offset;
            continue;
        }
        PC += bvh_right_offset;
        if (from > hi && to > hi)
        {
            continue;
        }
        {
            const float from_minus_hi = from - hi;
            const float from_minus_lo = from - lo;
            const float to_hi = to - hi;
            const float to_lo = to - lo;

            phys_bvh_aabb_cast_vm_query_float new_query;
            new_query.m_ray_ends[0] = float_query->m_ray_ends[0];
            new_query.m_ray_ends[1] = float_query->m_ray_ends[1];

            if (from_minus_hi < to_hi)
            {
                if (from_minus_hi * to_hi < 0.0f)
                {
                    const float t = from_minus_hi / (from_minus_hi - to_hi);
                    new_query.m_ray_ends[1].set_lerp4(float_query->m_ray_ends[0], float_query->m_ray_ends[1], phys_simd_scalar(t));
                }
                query_ray_on_tree(query, PC - bvh_right_offset + bvh_left_offset, &new_query, bvh_chunk_offset);
                if (from_minus_lo * to_lo < 0.0f)
                {
                    const float t = from_minus_lo / (from_minus_lo - to_lo);
                    float_query->m_ray_ends[0].set_lerp4(float_query->m_ray_ends[0], float_query->m_ray_ends[1], phys_simd_scalar(t));
                }
                if (m_early_hit_frac < m_ref_early_hit_frac)
                {
                    m_ref_early_hit_frac = m_early_hit_frac;
                    phys_vec4 new_to;
                    new_to.set_lerp4(m_input->m_ray_from, m_input->m_ray_to, phys_simd_scalar(m_early_hit_frac));
                    float_query->m_ray_ends[1].set_sub4(new_to, m_bvh_code->m_bvh_info.m_offset);
                    float_query->m_ray_ends[1].mul4(phys_simd_scalar(query->m_float_to_byte_scale));
                    float_query->m_ray_ends[1].sub4(query->m_float_to_byte_offset);
                    if (bvh_scale_index < 3 && (&float_query->m_ray_ends[1](0))[bvh_scale_index] < lo)
                    {
                        return;
                    }
                }
                continue;
            }
            else
            {
                if (from_minus_lo * to_lo < 0.0f)
                {
                    const float t = from_minus_lo / (from_minus_lo - to_lo);
                    new_query.m_ray_ends[1].set_lerp4(float_query->m_ray_ends[0], float_query->m_ray_ends[1], phys_simd_scalar(t));
                }
                query_ray_on_tree(query, PC, &new_query, bvh_chunk_offset);
                if (from_minus_hi * to_hi < 0.0f)
                {
                    const float t = from_minus_hi / (from_minus_hi - to_hi);
                    float_query->m_ray_ends[0].set_lerp4(float_query->m_ray_ends[0], float_query->m_ray_ends[1], phys_simd_scalar(t));
                }
                if (m_early_hit_frac < m_ref_early_hit_frac)
                {
                    m_ref_early_hit_frac = m_early_hit_frac;
                    phys_vec4 new_to;
                    new_to.set_lerp4(m_input->m_ray_from, m_input->m_ray_to, phys_simd_scalar(m_early_hit_frac));
                    float_query->m_ray_ends[1].set_sub4(new_to, m_bvh_code->m_bvh_info.m_offset);
                    float_query->m_ray_ends[1].mul4(phys_simd_scalar(query->m_float_to_byte_scale));
                    float_query->m_ray_ends[1].sub4(query->m_float_to_byte_offset);
                    if (bvh_scale_index < 3 && (&float_query->m_ray_ends[1](0))[bvh_scale_index] > hi)
                    {
                        return;
                    }
                }
                PC = PC - bvh_right_offset + bvh_left_offset;
                continue;
            }
        }
    }
}

// @address 0x69ef30 ?trace_aabb_cast@phys_bvh_aabb_cast_vm@@QAAXABUphys_bvh_aabb_cast_input@1@AAVphys_hit_collector@@PAV3@@Z
void phys_bvh_aabb_cast_vm::trace_aabb_cast(const phys_bvh_aabb_cast_input &input, phys_hit_collector &cast_hit_collector, phys_hit_collector *start_collector)
{
    if (g_havok_flyingcolors_mopp_aabb_cast == 0) {
        g_havok_flyingcolors_mopp_aabb_cast = process_flying_colors(4);
        if (g_havok_flyingcolors_mopp_aabb_cast == 0) {
            return;
        }
    }

    m_input = &input;
    m_bvh_code = static_cast<const phys_bvh_tree_shape *>(input.m_bvh_proxy->get_shape())->get_bvh_bytecode();
    m_cast_collector = &cast_hit_collector;
    m_start_hit_collector = start_collector;
    m_cast_object_type = input.m_cast_proxy->get_shape()->get_type_id();
    m_early_hit_frac = 1.0f;
    m_ref_early_hit_frac = m_early_hit_frac;

    const unsigned char *bvh_commands = &m_bvh_code->m_data[0];

    phys_bvh_aabb_cast_vm_query_int query;
    query.m_float_to_byte_offset.set_zero4();
    m_int_to_float_scale = 1.0f / m_bvh_code->m_bvh_info.m_offset(3);
    query.m_float_to_byte_scale = m_bvh_code->m_bvh_info.m_offset(3) / 65536.0f;
    query.m_cast_extents.set_mul4(phys_simd_scalar(query.m_float_to_byte_scale), input.m_cast_extents);
    query.m_extents_sum3 = input.m_cast_extents.horizontal_add3();
    query.m_extents_sum3 *= 3.0f;

    phys_bvh_aabb_cast_vm_query_float float_query;
    float_query.m_ray_ends[0].set_sub4(input.m_ray_from, m_bvh_code->m_bvh_info.m_offset);
    float_query.m_ray_ends[1].set_sub4(input.m_ray_to, m_bvh_code->m_bvh_info.m_offset);
    float_query.m_ray_ends[0].mul4(phys_simd_scalar(query.m_float_to_byte_scale));
    float_query.m_ray_ends[1].mul4(phys_simd_scalar(query.m_float_to_byte_scale));

    query.m_primitive_offset = 0;
    query.m_bit_shift = 0;
    for (int i = 0; i < 1; i++) {
        query.m_properties[i] = 0;
    }

    query_ray_on_tree(&query, bvh_commands, &float_query, 0);
}

