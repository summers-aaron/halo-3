// hkinternal/hkMoppRayVirtualMachine.obj: from the records 0x67ae58 0x67b930
// @flags /TP /O2 /Oc
#include "phys/phys_base.h"
#include "phys/phys_base_types.h"
#include "phys/phys_solver_check.h"
#include "phys/phys_collide.h"
#include "phys/phys_out_stream.h"


#include "phys/phys_bvh_vm.h"
extern "C" {
// @address 0x7ceb78 g_phys_bvh_ray_flying_colors_flag
char g_phys_bvh_ray_flying_colors_flag;
}

#include "phys/phys_err_stream.h"
#include "phys/phys_error.h"
#include "phys/phys_bvh_commands.h"

#include "phys/phys_bvh_ray_vm.h"
#include "phys/phys_statistics_collector.h"
#include "phys/phys_writer.h"

// @address 0x67ae58 ?query_ray_on_tree@phys_bvh_ray_vm@@IAAXPBUphys_bvh_ray_vm_query@@PBE@Z
void phys_bvh_ray_vm::query_ray_on_tree(const phys_bvh_ray_vm_query *query, const unsigned char *PC)
{
    phys_bvh_ray_vm_query scaled_query;
    int jump_offset_high;
    int jump_offset_low;
    int start;
    int end;
    int radius;

    while (1)
    {
        const phys_bvh_commands command = phys_bvh_commands(PC[0]);
        switch (command)
        {
        case PHYS_BVH_SPLIT_YZ:
            jump_offset_high = PC[1] * 2;
            jump_offset_low = PC[2] * 2;
            start = query->m_start_y + query->m_start_z;
            end = query->m_end_y + query->m_end_z;
            radius = query->m_radius + (query->m_radius >> 1) + 1;
            break;
        case PHYS_BVH_SPLIT_YMZ:
            jump_offset_high = PC[1] * 2;
            jump_offset_low = PC[2] * 2;
            start = query->m_start_y + (255 - query->m_start_z);
            end = query->m_end_y + (255 - query->m_end_z);
            radius = query->m_radius + (query->m_radius >> 1) + 1;
            break;
        case PHYS_BVH_SPLIT_XZ:
            jump_offset_high = PC[1] * 2;
            jump_offset_low = PC[2] * 2;
            start = query->m_start_x + query->m_start_z;
            end = query->m_end_x + query->m_end_z;
            radius = query->m_radius + (query->m_radius >> 1) + 1;
            break;
        case PHYS_BVH_SPLIT_XMZ:
            jump_offset_high = PC[1] * 2;
            jump_offset_low = PC[2] * 2;
            start = query->m_start_x + (255 - query->m_start_z);
            end = query->m_end_x + (255 - query->m_end_z);
            radius = query->m_radius + (query->m_radius >> 1) + 1;
            break;
        case PHYS_BVH_SPLIT_XY:
            jump_offset_high = PC[1] * 2;
            jump_offset_low = PC[2] * 2;
            start = query->m_start_x + query->m_start_y;
            end = query->m_end_x + query->m_end_y;
            radius = query->m_radius + (query->m_radius >> 1) + 1;
            break;
        case PHYS_BVH_SPLIT_XMY:
            jump_offset_high = PC[1] * 2;
            jump_offset_low = PC[2] * 2;
            start = query->m_start_x + (255 - query->m_start_y);
            end = query->m_end_x + (255 - query->m_end_y);
            radius = query->m_radius + (query->m_radius >> 1) + 1;
            break;
        case PHYS_BVH_SPLIT_XYZ:
            jump_offset_high = PC[1] * 2 + PC[1];
            jump_offset_low = PC[2] * 2 + PC[2];
            start = query->m_start_x + query->m_start_y + query->m_start_z;
            end = query->m_end_x + query->m_end_y + query->m_end_z;
            radius = query->m_radius * 4;
            break;
        case PHYS_BVH_SPLIT_XYMZ:
            jump_offset_high = PC[1] * 2 + PC[1];
            jump_offset_low = PC[2] * 2 + PC[2];
            start = query->m_start_x + query->m_start_y + (255 - query->m_start_z);
            end = query->m_end_x + query->m_end_y + (255 - query->m_end_z);
            radius = query->m_radius * 4;
            break;
        case PHYS_BVH_SPLIT_XMYZ:
            jump_offset_high = PC[1] * 2 + PC[1];
            jump_offset_low = PC[2] * 2 + PC[2];
            start = query->m_start_x + (255 - query->m_start_y) + query->m_start_z;
            end = query->m_end_x + (255 - query->m_end_y) + query->m_end_z;
            radius = query->m_radius * 4;
            break;
        case PHYS_BVH_SPLIT_XMYMZ:
            jump_offset_high = PC[1] * 2 + PC[1];
            jump_offset_low = PC[2] * 2 + PC[2];
            start = query->m_start_x + (255 - query->m_start_y) + (255 - query->m_start_z);
            end = query->m_end_x + (255 - query->m_end_y) + (255 - query->m_end_z);
            radius = query->m_radius * 4;
            break;
        case PHYS_BVH_SPLIT_X:
        case PHYS_BVH_SPLIT_Y:
        case PHYS_BVH_SPLIT_Z:
            jump_offset_high = PC[1];
            jump_offset_low = PC[2];
            start = (&query->m_start_x)[command - PHYS_BVH_SPLIT_X];
            end = (&query->m_end_x)[command - PHYS_BVH_SPLIT_X];
            radius = query->m_radius;
            break;
        case PHYS_BVH_SINGLE_SPLIT_X:
        case PHYS_BVH_SINGLE_SPLIT_Y:
        case PHYS_BVH_SINGLE_SPLIT_Z:
            jump_offset_low = PC[1];
            start = (&query->m_start_x)[command - PHYS_BVH_SINGLE_SPLIT_X];
            end = (&query->m_end_x)[command - PHYS_BVH_SINGLE_SPLIT_X];
            radius = query->m_radius;
            PC += 3;
            if (end + radius < jump_offset_low && start + radius < jump_offset_low)
            {
                continue;
            }
            {
                const unsigned int offset = PC[-1];
                PC += offset;
                if (start > jump_offset_low + radius + 1 && end > jump_offset_low + radius + 1)
                {
                    continue;
                }
                query_ray_on_tree(query, PC - offset);
                continue;
            }
        case PHYS_BVH_SPLIT_JUMP_X:
        case PHYS_BVH_SPLIT_JUMP_Y:
        case PHYS_BVH_SPLIT_JUMP_Z:
            {
                jump_offset_high = PC[1];
                jump_offset_low = PC[2];
                start = (&query->m_start_x)[command - PHYS_BVH_SPLIT_JUMP_X];
                end = (&query->m_end_x)[command - PHYS_BVH_SPLIT_JUMP_X];
                radius = query->m_radius;
                const unsigned int bvh_left_offset = (PC[3] << 8) + PC[4];
                PC += 7;
                if (end + radius < jump_offset_low && start + radius < jump_offset_low)
                {
                    PC += bvh_left_offset;
                    continue;
                }
                const unsigned int bvh_right_offset = (PC[-2] << 8) + PC[-1];
                PC += bvh_right_offset;
                if (start > jump_offset_high + radius && end > jump_offset_high + radius)
                {
                    continue;
                }
                query_ray_on_tree(query, PC - bvh_right_offset + bvh_left_offset);
                continue;
            }
        case PHYS_BVH_DOUBLE_CUT_X:
        case PHYS_BVH_DOUBLE_CUT_Y:
        case PHYS_BVH_DOUBLE_CUT_Z:
            start = (&query->m_start_x)[command - PHYS_BVH_DOUBLE_CUT_X];
            end = (&query->m_end_x)[command - PHYS_BVH_DOUBLE_CUT_X];
            jump_offset_low = PC[1];
            jump_offset_high = PC[2];
            radius = query->m_radius;
            if ((end + radius < jump_offset_low && start + radius < jump_offset_low) || (start > jump_offset_high + radius && end > jump_offset_high + radius))
            {
                return;
            }
            PC += 3;
            continue;
        case PHYS_BVH_DOUBLE_CUT24_X:
        case PHYS_BVH_DOUBLE_CUT24_Y:
        case PHYS_BVH_DOUBLE_CUT24_Z:
            start = (&m_start_x)[command - PHYS_BVH_DOUBLE_CUT24_X];
            end = (&m_end_x)[command - PHYS_BVH_DOUBLE_CUT24_X];
            jump_offset_low = read_u24(PC + 1);
            jump_offset_high = read_u24(PC + 4);
            radius = query->m_radius;
            if ((end + radius < jump_offset_low && start + radius < jump_offset_low) || (start > jump_offset_high + radius && end > jump_offset_high + radius))
            {
                return;
            }
            PC += 7;
            continue;
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
        case PHYS_BVH_SCALE1:
        case PHYS_BVH_SCALE2:
        case PHYS_BVH_SCALE3:
        case PHYS_BVH_SCALE4:
            {
                const int shift = command;
                scaled_query.m_obb_offset_x = (PC[1] + query->m_obb_offset_x) << shift;
                scaled_query.m_obb_offset_y = (PC[2] + query->m_obb_offset_y) << shift;
                scaled_query.m_obb_offset_z = (PC[3] + query->m_obb_offset_z) << shift;
                scaled_query.m_bit_shift = query->m_bit_shift + shift;
                const int right_shift = 16 - scaled_query.m_bit_shift;
                scaled_query.m_start_x = m_start_x >> right_shift;
                scaled_query.m_start_x -= scaled_query.m_obb_offset_x;
                scaled_query.m_start_y = m_start_y >> right_shift;
                scaled_query.m_start_y -= scaled_query.m_obb_offset_y;
                scaled_query.m_start_z = m_start_z >> right_shift;
                scaled_query.m_start_z -= scaled_query.m_obb_offset_z;
                scaled_query.m_end_x = m_end_x >> right_shift;
                scaled_query.m_end_x -= scaled_query.m_obb_offset_x;
                scaled_query.m_end_y = m_end_y >> right_shift;
                scaled_query.m_end_y -= scaled_query.m_obb_offset_y;
                scaled_query.m_end_z = m_end_z >> right_shift;
                scaled_query.m_end_z -= scaled_query.m_obb_offset_z;
                scaled_query.m_radius = (m_radius >> right_shift) + 1;
                for (int i = 0; i < 1; i++)
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
        case PHYS_BVH_RETURN:
            return;
            // The terminal cases share the hit path after this block.
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
                add_cast_hit(jump_offset_low, query->m_properties);
                return;
            }
            // The property cases share the query copy after this block.
            do
            {
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
                    break;
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
                    phys_error::MESSAGE_ERROR, 0x1298fedd, assert_buf, ".\\collide\\mopp\\machine\\hkMoppRayVirtualMachine.cpp", 309);
                continue;
            }
        }
        PC += 4;
        if (end + radius < jump_offset_low && start + radius < jump_offset_low)
        {
            continue;
        }
        {
            const unsigned int offset = PC[-1];
            PC += offset;
            if (start > jump_offset_high + radius && end > jump_offset_high + radius)
            {
                continue;
            }
            query_ray_on_tree(query, PC - offset);
            continue;
        }
    }
}
// @address 0x67b930 ?query_ray@phys_bvh_ray_vm@@QAAXPBVphys_bvh_bytecode@@ABVphys_vec4@@1MPAV?$phys_array@Vphys_bvh_primitive_info@@@@@Z
void phys_bvh_ray_vm::query_ray(
    const phys_bvh_bytecode *code, const phys_vec4 &start, const phys_vec4 &end, float radius, phys_array<phys_bvh_primitive_info> *found_primitives)
{
    if (!g_phys_bvh_ray_flying_colors_flag)
    {
        g_phys_bvh_ray_flying_colors_flag = process_flying_colors(4);
        if (!g_phys_bvh_ray_flying_colors_flag)
            return;
    }

    const unsigned char *bvh_commands = &code->m_data.m_data[0];
    m_out_primitives = found_primitives;
    const phys_bvh_bytecode::bytecode_info &info = code->m_bvh_info;

    m_start_x = to_int_min((start(0) - info.m_offset(0)) * info.m_offset(3));
    m_start_y = to_int_min((start(1) - info.m_offset(1)) * info.m_offset(3));
    m_start_z = to_int_min((start(2) - info.m_offset(2)) * info.m_offset(3));

    m_end_x = to_int_max((end(0) - info.m_offset(0)) * info.m_offset(3));
    m_end_y = to_int_max((end(1) - info.m_offset(1)) * info.m_offset(3));
    m_end_z = to_int_max((end(2) - info.m_offset(2)) * info.m_offset(3));

    m_radius = to_int_max(radius * info.m_offset(3));

    phys_bvh_ray_vm_query query;
    query.m_start_x = m_start_x >> 16;
    query.m_start_y = m_start_y >> 16;
    query.m_start_z = m_start_z >> 16;
    query.m_end_x = m_end_x >> 16;
    query.m_end_y = m_end_y >> 16;
    query.m_end_z = m_end_z >> 16;
    query.m_radius = (m_radius >> 16) + 1;
    query.m_obb_offset_x = 0;
    query.m_obb_offset_y = 0;
    query.m_obb_offset_z = 0;
    query.m_primitive_offset = 0;
    query.m_bit_shift = 0;
    for (int i = 0; i < 1; i++)
    {
        query.m_properties[i] = 0;
    }

    query_ray_on_tree(&query, bvh_commands);
}
