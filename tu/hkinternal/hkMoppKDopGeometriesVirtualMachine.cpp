// hkinternal/hkMoppKDopGeometriesVirtualMachine.obj: from the records 0x6a62e0 0x6a6308 0x6a6320 0x6a6338 0x6a6398 0x6a64d0 0x6a7488 0x6a74b8 0x6a7518
// @compiler xdk5632
// @flags /TP /O2 /Oc
// @address 0x6a62e0 ?double_cut@@YAXAAVphys_kdop26@@W4dop_axis@1@MMAAM2@Z
// @address 0x6a6308 ?shift_dop_low@@YAMAAVphys_kdop26@@W4dop_axis@1@M@Z
// @address 0x6a6320 ?shift_dop_high@@YAMAAVphys_kdop26@@W4dop_axis@1@M@Z
// @address 0x6a7488 ??0phys_bvh_kdop_vm@@QAA@XZ
// @address 0x6a74b8 ??1phys_bvh_kdop_vm@@QAA@XZ
#include "phys/phys_base.h"
#include <string.h>
#include <vectorintrinsics.h>
#include "phys/phys_collide.h"
#include "phys/phys_out_stream.h"

#include "phys/phys_bvh_kdop_vm.h"
#include "phys/phys_err_stream.h"
#include "phys/phys_error.h"
// @address 0x17cbc k_phys_bvh_kdop_vm_source_file
// @rdata 0x82017cbc..0x82017cfc
const char k_phys_bvh_kdop_vm_source_file[64] = ".\\collide\\mopp\\machine\\hkMoppKDopGeometriesVirtualMachine.cpp";
// @rdata-end
#include "phys/phys_bvh_commands.h"
#include "phys/phys_bvh_vm.h"
#include "phys/phys_statistics_collector.h"
#include "phys/phys_writer.h"

void double_cut(phys_kdop26 &cut_kdop, phys_kdop26::dop_axis axis, float low, float high, float &prev_low, float &prev_high)
{
    prev_low = cut_kdop.get_low_bound(axis);
    prev_high = cut_kdop.get_high_bound(axis);
    cut_kdop.set_dop_low(axis, low);
    cut_kdop.set_dop_high(axis, high);
}
float shift_dop_low(phys_kdop26 &cut_kdop, phys_kdop26::dop_axis axis, float low)
{
    float prev_low = cut_kdop.get_low_bound(axis);
    cut_kdop.set_dop_low(axis, low);
    return prev_low;
}
float shift_dop_high(phys_kdop26 &cut_kdop, phys_kdop26::dop_axis axis, float high)
{
    float prev_high = cut_kdop.get_high_bound(axis);
    cut_kdop.set_dop_high(axis, high);
    return prev_high;
}
// @address 0x6a6338 ?push_kdop@phys_bvh_kdop_vm@@IAAXVphys_bool@@I@Z
void phys_bvh_kdop_vm::push_kdop(phys_bool has_shape_key, unsigned int kdop_shape_key)
{
    kdop_geometry *geometry = m_next_geometry;
    geometry->m_has_shape_key = has_shape_key;
    if (geometry->m_has_shape_key) geometry->m_shape_key = kdop_shape_key;
    geometry->m_kdop_level = (char)m_kdop_level;
    memcpy(&geometry->m_query_kdop, &m_current_kdop, sizeof(phys_kdop26));
    ++m_next_geometry;
}
phys_bvh_kdop_vm::phys_bvh_kdop_vm() {}
phys_bvh_kdop_vm::~phys_bvh_kdop_vm() {}

static __forceinline void phys_bvh_kdop_vm_push_kdop(phys_bvh_kdop_vm::kdop_geometry *&next,
    phys_kdop26 &current_kdop, const int &level, phys_bool has_shape_key, unsigned int kdop_shape_key)
{
    phys_bvh_kdop_vm::kdop_geometry *geometry = next;
    geometry->m_has_shape_key = has_shape_key;
    if (geometry->m_has_shape_key) geometry->m_shape_key = kdop_shape_key;
    geometry->m_kdop_level = (char)level;
    memcpy(&geometry->m_query_kdop, &current_kdop, sizeof(phys_kdop26));
    ++next;
}
// @address 0x6a6398 ?add_cast_hit@phys_bvh_kdop_vm@@IAAXIQBI@Z
void phys_bvh_kdop_vm::add_cast_hit(unsigned int id, unsigned int const *const properties)
{
    if (m_keys.array_index_of(id) == -1)
    {
        phys_bool is_query_key = m_kdop_query.m_use_shape_key && id == m_kdop_query.m_shape_key;
        if (!m_kdop_query.m_use_shape_key || is_query_key)
        {
            phys_bvh_kdop_vm_push_kdop(m_next_geometry, m_current_kdop, m_kdop_level, true, id);
            m_kdop_query.m_has_hit = m_kdop_query.m_has_hit || is_query_key;
        }
        m_keys.array_push_back(id);
    }
    if (m_kdop_query.m_has_hit) m_has_hit = true;
}
// @address 0x6a64d0 ?query_bvh_kdop_geoms_rec@phys_bvh_kdop_vm@@IAAXPBUphys_bvh_kdop_vm_query@1@PBE@Z
void phys_bvh_kdop_vm::query_bvh_kdop_geoms_rec(const phys_bvh_kdop_vm_query *query, const unsigned char *PC)
{
    phys_bvh_kdop_vm_query scaled_query;
    unsigned int jump_offset_high;
    unsigned int jump_offset_low;
    float split_high;
    float split_low;
    int query_axis_idx;

    while (1)
    {
        const phys_bvh_commands command = phys_bvh_commands(PC[0]);
        if (m_has_hit)
        {
            return;
        }
        int simplex_dim = 999;

        switch (command)
        {
        case PHYS_BVH_SPLIT_YZ:
            split_high = float((PC[1] * 2) << (16 - query->m_bit_shift)) + float(query->m_obb_offset_y + query->m_obb_offset_z);
            split_low = float((PC[2] * 2) << (16 - query->m_bit_shift)) + float(query->m_obb_offset_y + query->m_obb_offset_z);
            split_high *= m_int_to_float_scale;
            split_low *= m_int_to_float_scale;
            query_axis_idx = 3;
            PC += 4;
            jump_offset_low = 0;
            jump_offset_high = PC[-1];
            break;
        case PHYS_BVH_SPLIT_YMZ:
            split_high = float((PC[1] * 2 - 255) << (16 - query->m_bit_shift)) + float(query->m_obb_offset_y - query->m_obb_offset_z);
            split_low = float((PC[2] * 2 - 255) << (16 - query->m_bit_shift)) + float(query->m_obb_offset_y - query->m_obb_offset_z);
            split_high *= m_int_to_float_scale;
            split_low *= m_int_to_float_scale;
            query_axis_idx = 4;
            PC += 4;
            jump_offset_low = 0;
            jump_offset_high = PC[-1];
            break;
        case PHYS_BVH_SPLIT_XZ:
            split_high = float((PC[1] * 2) << (16 - query->m_bit_shift)) + float(query->m_obb_offset_x + query->m_obb_offset_z);
            split_low = float((PC[2] * 2) << (16 - query->m_bit_shift)) + float(query->m_obb_offset_x + query->m_obb_offset_z);
            split_high *= m_int_to_float_scale;
            split_low *= m_int_to_float_scale;
            query_axis_idx = 5;
            PC += 4;
            jump_offset_low = 0;
            jump_offset_high = PC[-1];
            break;
        case PHYS_BVH_SPLIT_XMZ:
            split_high = float((PC[1] * 2 - 255) << (16 - query->m_bit_shift)) + float(query->m_obb_offset_x - query->m_obb_offset_z);
            split_low = float((PC[2] * 2 - 255) << (16 - query->m_bit_shift)) + float(query->m_obb_offset_x - query->m_obb_offset_z);
            split_high *= m_int_to_float_scale;
            split_low *= m_int_to_float_scale;
            query_axis_idx = 6;
            PC += 4;
            jump_offset_low = 0;
            jump_offset_high = PC[-1];
            break;
        case PHYS_BVH_SPLIT_XY:
            split_high = float((PC[1] * 2) << (16 - query->m_bit_shift)) + float(query->m_obb_offset_x + query->m_obb_offset_y);
            split_low = float((PC[2] * 2) << (16 - query->m_bit_shift)) + float(query->m_obb_offset_x + query->m_obb_offset_y);
            split_high *= m_int_to_float_scale;
            split_low *= m_int_to_float_scale;
            query_axis_idx = 7;
            PC += 4;
            jump_offset_low = 0;
            jump_offset_high = PC[-1];
            break;
        case PHYS_BVH_SPLIT_XMY:
            split_high = float((PC[1] * 2 - 255) << (16 - query->m_bit_shift)) + float(query->m_obb_offset_x - query->m_obb_offset_y);
            split_low = float((PC[2] * 2 - 255) << (16 - query->m_bit_shift)) + float(query->m_obb_offset_x - query->m_obb_offset_y);
            split_high *= m_int_to_float_scale;
            split_low *= m_int_to_float_scale;
            query_axis_idx = 8;
            PC += 4;
            jump_offset_low = 0;
            jump_offset_high = PC[-1];
            break;
        case PHYS_BVH_SPLIT_XYZ:
            split_high = float((PC[1] * 3) << (16 - query->m_bit_shift)) + float(query->m_obb_offset_x + query->m_obb_offset_y + query->m_obb_offset_z);
            split_low = float((PC[2] * 3) << (16 - query->m_bit_shift)) + float(query->m_obb_offset_x + query->m_obb_offset_y + query->m_obb_offset_z);
            split_high *= m_int_to_float_scale;
            split_low *= m_int_to_float_scale;
            query_axis_idx = 9;
            PC += 4;
            jump_offset_low = 0;
            jump_offset_high = PC[-1];
            break;
        case PHYS_BVH_SPLIT_XYMZ:
            split_high = float((PC[1] * 3 - 255) << (16 - query->m_bit_shift)) + float(query->m_obb_offset_x + query->m_obb_offset_y - query->m_obb_offset_z);
            split_low = float((PC[2] * 3 - 255) << (16 - query->m_bit_shift)) + float(query->m_obb_offset_x + query->m_obb_offset_y - query->m_obb_offset_z);
            split_high *= m_int_to_float_scale;
            split_low *= m_int_to_float_scale;
            query_axis_idx = 10;
            PC += 4;
            jump_offset_low = 0;
            jump_offset_high = PC[-1];
            break;
        case PHYS_BVH_SPLIT_XMYZ:
            split_high = float((PC[1] * 3 - 255) << (16 - query->m_bit_shift)) + float(query->m_obb_offset_x - query->m_obb_offset_y + query->m_obb_offset_z);
            split_low = float((PC[2] * 3 - 255) << (16 - query->m_bit_shift)) + float(query->m_obb_offset_x - query->m_obb_offset_y + query->m_obb_offset_z);
            split_high *= m_int_to_float_scale;
            split_low *= m_int_to_float_scale;
            query_axis_idx = 11;
            PC += 4;
            jump_offset_low = 0;
            jump_offset_high = PC[-1];
            break;
        case PHYS_BVH_SPLIT_XMYMZ:
            split_high = float((PC[1] * 3 - 510) << (16 - query->m_bit_shift)) + float(query->m_obb_offset_x - query->m_obb_offset_y - query->m_obb_offset_z);
            split_low = float((PC[2] * 3 - 510) << (16 - query->m_bit_shift)) + float(query->m_obb_offset_x - query->m_obb_offset_y - query->m_obb_offset_z);
            split_high *= m_int_to_float_scale;
            split_low *= m_int_to_float_scale;
            query_axis_idx = 12;
            PC += 4;
            jump_offset_low = 0;
            jump_offset_high = PC[-1];
            break;
        case PHYS_BVH_SPLIT_X:
        case PHYS_BVH_SPLIT_Y:
        case PHYS_BVH_SPLIT_Z:
            simplex_dim = command - PHYS_BVH_SPLIT_X;
            split_high = float((PC[1] << (16 - query->m_bit_shift)) + (&query->m_obb_offset_x)[simplex_dim]) * m_int_to_float_scale;
            split_low = float((PC[2] << (16 - query->m_bit_shift)) + (&query->m_obb_offset_x)[simplex_dim]) * m_int_to_float_scale;
            query_axis_idx = simplex_dim;
            PC += 4;
            jump_offset_low = 0;
            jump_offset_high = PC[-1];
            break;
        case PHYS_BVH_SINGLE_SPLIT_X:
        case PHYS_BVH_SINGLE_SPLIT_Y:
        case PHYS_BVH_SINGLE_SPLIT_Z:
            simplex_dim = command - PHYS_BVH_SINGLE_SPLIT_X;
            split_low = float((PC[1] << (16 - query->m_bit_shift)) + (&query->m_obb_offset_x)[simplex_dim]);
            split_high = split_low + float(1 << (16 - query->m_bit_shift));
            split_low *= m_int_to_float_scale;
            split_high *= m_int_to_float_scale;
            query_axis_idx = simplex_dim;
            PC += 3;
            jump_offset_low = 0;
            jump_offset_high = PC[-1];
            break;
        case PHYS_BVH_SPLIT_JUMP_X:
        case PHYS_BVH_SPLIT_JUMP_Y:
        case PHYS_BVH_SPLIT_JUMP_Z:
            simplex_dim = command - PHYS_BVH_SPLIT_JUMP_X;
            split_high = float((PC[1] << (16 - query->m_bit_shift)) + (&query->m_obb_offset_x)[simplex_dim]) * m_int_to_float_scale;
            split_low = float((PC[2] << (16 - query->m_bit_shift)) + (&query->m_obb_offset_x)[simplex_dim]) * m_int_to_float_scale;
            query_axis_idx = simplex_dim;
            jump_offset_low = (PC[3] << 8) + PC[4];
            PC += 7;
            jump_offset_high = (PC[-2] << 8) + PC[-1];
            break;
        case PHYS_BVH_DOUBLE_CUT_X:
        case PHYS_BVH_DOUBLE_CUT_Y:
        case PHYS_BVH_DOUBLE_CUT_Z:
            {
                simplex_dim = command - PHYS_BVH_DOUBLE_CUT_X;
                split_low = float((PC[1] << (16 - query->m_bit_shift)) + (&query->m_obb_offset_x)[simplex_dim]) * m_int_to_float_scale;
                split_high = float((PC[2] << (16 - query->m_bit_shift)) + (&query->m_obb_offset_x)[simplex_dim]) * m_int_to_float_scale;
                query_axis_idx = simplex_dim;
                phys_bool pushed = (m_kdop_query.m_kdop_depth == 0 || m_kdop_level < m_kdop_query.m_kdop_depth);
                if (pushed)
                {
                    push_kdop();
                }
                float prev_low;
                float prev_high;
                double_cut(m_current_kdop, phys_kdop26::dop_axis(query_axis_idx), split_low, split_high, prev_low, prev_high);
                PC += 3;
                m_kdop_level++;
                query_bvh_kdop_geoms_rec(query, PC);
                m_kdop_level--;
                if (pushed && m_kdop_query.m_use_shape_key && !m_has_hit)
                {
                    pop_kdop();
                }
                double_cut(m_current_kdop, phys_kdop26::dop_axis(query_axis_idx), prev_low, prev_high, split_low, split_high);
                return;
            }
        case PHYS_BVH_DOUBLE_CUT24_X:
        case PHYS_BVH_DOUBLE_CUT24_Y:
        case PHYS_BVH_DOUBLE_CUT24_Z:
            {
                simplex_dim = command - PHYS_BVH_DOUBLE_CUT24_X;
                // match: 0x6a64d0: the byte cursor preserves read24 scratch-register coloring.
                const unsigned char *low_code_bytes = PC + 1;
                split_low = float(read_u24(low_code_bytes)) * m_int_to_float_scale;
                split_high = float(read_u24(PC + 4)) * m_int_to_float_scale;
                query_axis_idx = simplex_dim;
                phys_bool pushed = (m_kdop_query.m_kdop_depth == 0 || m_kdop_level < m_kdop_query.m_kdop_depth);
                if (pushed)
                {
                    push_kdop();
                }
                float prev_low;
                float prev_high;
                double_cut(m_current_kdop, phys_kdop26::dop_axis(query_axis_idx), split_low, split_high, prev_low, prev_high);
                PC += 7;
                m_kdop_level++;
                query_bvh_kdop_geoms_rec(query, PC);
                m_kdop_level--;
                if (pushed && m_kdop_query.m_use_shape_key)
                {
                    pop_kdop();
                }
                double_cut(m_current_kdop, phys_kdop26::dop_axis(query_axis_idx), prev_low, prev_high, split_low, split_high);
                return;
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
        case PHYS_BVH_RETURN:
        case PHYS_BVH_SCALE1:
        case PHYS_BVH_SCALE2:
        case PHYS_BVH_SCALE3:
        case PHYS_BVH_SCALE4:
            {
                const int shift = command;
                scaled_query.m_obb_offset_x = (PC[1] << (16 - query->m_bit_shift)) + query->m_obb_offset_x;
                scaled_query.m_obb_offset_y = (PC[2] << (16 - query->m_bit_shift)) + query->m_obb_offset_y;
                scaled_query.m_obb_offset_z = (PC[3] << (16 - query->m_bit_shift)) + query->m_obb_offset_z;
                scaled_query.m_bit_shift = query->m_bit_shift + shift;
                for (int i = 0; i < phys_bvh_bytecode::PRIMITIVE_PROP_MAX; i++)
                {
                    scaled_query.m_properties[i] = query->m_properties[i];
                }
                scaled_query.m_primitive_offset = query->m_primitive_offset;
                query = &scaled_query;
                PC += 4;
                continue;
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
        case PHYS_BVH_TERM8:
            m_terminal_offset = PC[1];
            goto add_terminal;
        case PHYS_BVH_TERM16:
            simplex_dim = PC[1];
            simplex_dim <<= 8;
            m_terminal_offset = PC[2];
            m_terminal_offset += simplex_dim;
            goto add_terminal;
        case PHYS_BVH_TERM24:
            simplex_dim = PC[1];
            simplex_dim <<= 16;
            m_terminal_offset = PC[2];
            m_terminal_offset <<= 8;
            m_terminal_offset += PC[3];
            m_terminal_offset += simplex_dim;
            goto add_terminal;
        case 0x30: case 0x31: case 0x32: case 0x33: case 0x34: case 0x35: case 0x36: case 0x37:
        case 0x38: case 0x39: case 0x3a: case 0x3b: case 0x3c: case 0x3d: case 0x3e: case 0x3f:
        case 0x40: case 0x41: case 0x42: case 0x43: case 0x44: case 0x45: case 0x46: case 0x47:
        case 0x48: case 0x49: case 0x4a: case 0x4b: case 0x4c: case 0x4d: case 0x4e: case 0x4f:
            m_terminal_offset = command - PHYS_BVH_TERM4_0;
add_terminal:
            m_terminal_offset += query->m_primitive_offset;
            add_cast_hit(m_terminal_offset, query->m_properties);
            m_terminal_offset = (unsigned int)-1;
            return;
        case PHYS_BVH_PROPERTY8_0:
        case PHYS_BVH_PROPERTY8_1:
        case PHYS_BVH_PROPERTY8_2:
        case PHYS_BVH_PROPERTY8_3:
            {
                unsigned int property;
                property = PC[0] - PHYS_BVH_PROPERTY8_0;
                unsigned int value;
                value = PC[1];
                PC += 2;
                scaled_query.m_properties[property] = value;
copy_properties:
                const unsigned int v = scaled_query.m_properties[0];
                if (query != &scaled_query)
                {
                    scaled_query = *query;
                    query = &scaled_query;
                }
                scaled_query.m_properties[0] = v;
                continue;
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
                goto copy_properties;
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
                goto copy_properties;
            }
        default:
            {
                char assert_buf[512];
                phys_err_stream report_stream(assert_buf, sizeof(assert_buf));
                report_stream << "Unknown command - This mopp data has been corrupted (check for memory trashing), "
                     "or an hkMoppBvTreeShape has been pointed at invalid mopp data.\n";
                phys_singleton<phys_error>::s_instance->message(phys_error::MESSAGE_ERROR, 0x1298fedd, assert_buf, k_phys_bvh_kdop_vm_source_file, 593);
                continue;
            }
        }
        {
            float prev_bound = shift_dop_high(m_current_kdop, phys_kdop26::dop_axis(query_axis_idx), split_high);
            PC += jump_offset_low;
            query_bvh_kdop_geoms_rec(query, PC);
            shift_dop_high(m_current_kdop, phys_kdop26::dop_axis(query_axis_idx), prev_bound);
            prev_bound = shift_dop_low(m_current_kdop, phys_kdop26::dop_axis(query_axis_idx), split_low);
            PC -= jump_offset_low - jump_offset_high;
            query_bvh_kdop_geoms_rec(query, PC);
            shift_dop_low(m_current_kdop, phys_kdop26::dop_axis(query_axis_idx), prev_bound);
            return;
        }

    }
}
// @address 0x6a7518 ?query_bvh@phys_bvh_kdop_vm@@QAAXPBVphys_bvh_bytecode@@ABUphys_bvh_kdop_query@@PAUphys_bvh_info@@@Z
void phys_bvh_kdop_vm::query_bvh(const phys_bvh_bytecode *code, const phys_bvh_kdop_query &kdop_query_input, phys_bvh_info *kdop_geometries)
{
    m_next_geometry = kdop_geometries;
    m_int_to_float_scale = 1.0f / code->m_bvh_info.get_offset_scale();
    m_offset = code->m_bvh_info.m_offset;
    m_has_hit = false;
    m_kdop_query = kdop_query_input;

    phys_bvh_kdop_vm_query query;
    query.m_obb_offset_x = 0;
    query.m_obb_offset_y = 0;
    query.m_obb_offset_z = 0;
    query.m_bit_shift = 0;
    query.m_primitive_offset = 0;

    m_terminal_offset = (unsigned int)-1;
    m_kdop_level = 0;

    const float scale = 16777216.0f * m_int_to_float_scale;
    for (int i = 0; i < 13; i++)
    {
        m_current_kdop.set_dop_low(i, -scale * 2.0f);
        m_current_kdop.set_dop_high(i, scale * 2.0f);
    }

    const unsigned char *PC = &code->m_data.m_data[0];
    for (int i = 0; i < phys_bvh_bytecode::PRIMITIVE_PROP_MAX; i++)
    {
        query.m_properties[i] = 0;
    }
    query_bvh_kdop_geoms_rec(&query, PC);
}

