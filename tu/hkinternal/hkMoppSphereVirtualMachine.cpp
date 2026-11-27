// hkinternal/hkMoppSphereVirtualMachine.obj: from the records 0x68d888 0x68e248
// @compiler xdk5632
// @flags /TP /O2
#include "phys/phys_base.h"
#include "phys/phys_collide.h"
// @address 0x68e248 ?shape_query_sphere@phys_bvh_sphere_vm@@QAAXPBVphys_bvh_bytecode@@ABVphys_sphere@@PAV?$phys_array@Vphys_bvh_primitive_info@@@@@Z
#include "phys/phys_solver_check.h"
#include "phys/phys_out_stream.h"


#include "phys/phys_bvh_vm.h"






#include "phys/phys_err_stream.h"

#include "phys/phys_error.h"

extern "C" {
// @address 0x7ceb7a g_phys_bvh_sphere_flying_colors_flag
char g_phys_bvh_sphere_flying_colors_flag;
}

#include "phys/phys_bvh_vm.h"

#include "phys/phys_bvh_commands.h"

#include "phys/phys_bvh_sphere_vm.h"
#include "phys/phys_sphere.h"
#include "phys/phys_statistics_collector.h"
#include "phys/phys_writer.h"

// @address 0x68d888 ?query_sphere_on_tree@phys_bvh_sphere_vm@@IAAXPBUphys_bvh_sphere_vm_query@1@PBE@Z
void phys_bvh_sphere_vm::query_sphere_on_tree(const phys_bvh_sphere_vm_query *query, const unsigned char *PC)
{
    if (!g_phys_bvh_sphere_flying_colors_flag)
    {
        g_phys_bvh_sphere_flying_colors_flag = process_flying_colors(4);
        if (!g_phys_bvh_sphere_flying_colors_flag)
        {
            return;
        }
    }

    phys_bvh_sphere_vm_query scaled_query;
    unsigned int jump_offset_high;
    unsigned int jump_offset_low;

    while (1)
    {
        const phys_bvh_commands command = phys_bvh_commands(PC[0]);
        int x;
        int radius;

        switch (command)
        {
        case PHYS_BVH_SPLIT_YZ:
            jump_offset_high = PC[1] * 2;
            jump_offset_low = PC[2] * 2;
            x = query->m_y + query->m_soa_z;
            radius = query->m_radius + (query->m_radius >> 1) + 1;
            break;
        case PHYS_BVH_SPLIT_YMZ:
            jump_offset_high = PC[1] * 2;
            jump_offset_low = PC[2] * 2;
            x = query->m_y + (255 - query->m_soa_z);
            radius = query->m_radius + (query->m_radius >> 1) + 1;
            break;
        case PHYS_BVH_SPLIT_XZ:
            jump_offset_high = PC[1] * 2;
            jump_offset_low = PC[2] * 2;
            x = query->m_x + query->m_soa_z;
            radius = query->m_radius + (query->m_radius >> 1) + 1;
            break;
        case PHYS_BVH_SPLIT_XMZ:
            jump_offset_high = PC[1] * 2;
            jump_offset_low = PC[2] * 2;
            x = query->m_x + (255 - query->m_soa_z);
            radius = query->m_radius + (query->m_radius >> 1) + 1;
            break;
        case PHYS_BVH_SPLIT_XY:
            jump_offset_high = PC[1] * 2;
            jump_offset_low = PC[2] * 2;
            x = query->m_x + query->m_y;
            radius = query->m_radius + (query->m_radius >> 1) + 1;
            break;
        case PHYS_BVH_SPLIT_XMY:
            jump_offset_high = PC[1] * 2;
            jump_offset_low = PC[2] * 2;
            x = query->m_x + (255 - query->m_y);
            radius = query->m_radius + (query->m_radius >> 1) + 1;
            break;
        case PHYS_BVH_SPLIT_XYZ:
            jump_offset_high = PC[1] * 2 + PC[1];
            jump_offset_low = PC[2] * 2 + PC[2];
            x = query->m_x + query->m_y + query->m_soa_z;
            radius = query->m_radius * 4;
            break;
        case PHYS_BVH_SPLIT_XYMZ:
            jump_offset_high = PC[1] * 2 + PC[1];
            jump_offset_low = PC[2] * 2 + PC[2];
            x = query->m_x + query->m_y + (255 - query->m_soa_z);
            radius = query->m_radius * 4;
            break;
        case PHYS_BVH_SPLIT_XMYZ:
            jump_offset_high = PC[1] * 2 + PC[1];
            jump_offset_low = PC[2] * 2 + PC[2];
            x = query->m_x + (255 - query->m_y) + query->m_soa_z;
            radius = query->m_radius * 4;
            break;
        case PHYS_BVH_SPLIT_XMYMZ:
            jump_offset_high = PC[1] * 2 + PC[1];
            jump_offset_low = PC[2] * 2 + PC[2];
            x = query->m_x + (255 - query->m_y) + (255 - query->m_soa_z);
            radius = query->m_radius * 4;
            break;
        case PHYS_BVH_SPLIT_X:
        case PHYS_BVH_SPLIT_Y:
        case PHYS_BVH_SPLIT_Z:
            jump_offset_high = PC[1];
            jump_offset_low = PC[2];
            x = (&query->m_x)[command - PHYS_BVH_SPLIT_X];
            radius = query->m_radius;
            break;
        case PHYS_BVH_SINGLE_SPLIT_X:
        case PHYS_BVH_SINGLE_SPLIT_Y:
        case PHYS_BVH_SINGLE_SPLIT_Z:
            {
                jump_offset_low = PC[1];
                radius = query->m_radius;
                x = (&query->m_x)[command - PHYS_BVH_SINGLE_SPLIT_X];
                PC += 3;
                if (int(x + radius) < int(jump_offset_low))
                {
                    continue;
                }
                const unsigned int offset = PC[-1];
                PC += offset;
                if (x > int(jump_offset_low + radius + 1))
                {
                    continue;
                }
                query_sphere_on_tree(query, PC - offset);
                continue;
            }
        case PHYS_BVH_SPLIT_JUMP_X:
        case PHYS_BVH_SPLIT_JUMP_Y:
        case PHYS_BVH_SPLIT_JUMP_Z:
            {
                jump_offset_high = PC[1];
                jump_offset_low = PC[2];
                x = (&query->m_x)[command - PHYS_BVH_SPLIT_JUMP_X];
                radius = query->m_radius;
                const unsigned int bvh_left_offset = (PC[3] << 8) + PC[4];
                PC += 7;
                if (int(x + radius) < int(jump_offset_low))
                {
                    PC += bvh_left_offset;
                    continue;
                }
                const unsigned int bvh_right_offset = (PC[-2] << 8) + PC[-1];
                PC += bvh_right_offset;
                if (x > int(jump_offset_high + radius))
                {
                    continue;
                }
                query_sphere_on_tree(query, PC - bvh_right_offset + bvh_left_offset);
                continue;
            }
        case PHYS_BVH_DOUBLE_CUT_X:
        case PHYS_BVH_DOUBLE_CUT_Y:
        case PHYS_BVH_DOUBLE_CUT_Z:
            x = (&query->m_x)[command - PHYS_BVH_DOUBLE_CUT_X];
            radius = query->m_radius;
            jump_offset_low = PC[1];
            jump_offset_high = PC[2];
            if (int(x + radius) >= int(jump_offset_low) && x <= int(jump_offset_high + radius))
            {
                PC += 3;
                continue;
            }
            else
            {
                return;
            }
        case PHYS_BVH_DOUBLE_CUT24_X:
        case PHYS_BVH_DOUBLE_CUT24_Y:
        case PHYS_BVH_DOUBLE_CUT24_Z:
            x = (&m_x)[command - PHYS_BVH_DOUBLE_CUT24_X];
            radius = m_radius;
            jump_offset_low = read_u24(PC + 1);
            jump_offset_high = read_u24(PC + 4);
            if (int(x + radius) >= int(jump_offset_low) && x <= int(jump_offset_high + radius))
            {
                PC += 7;
                continue;
            }
            else
            {
                return;
            }
        case PHYS_BVH_JUMP8:
            {
                jump_offset_low = PC[1];
                PC += 2;
                PC += jump_offset_low;
                continue;
            }
        case PHYS_BVH_JUMP16:
            {
                jump_offset_high = PC[1];
                jump_offset_low = PC[2];
                PC += 3;
                PC += (jump_offset_high << 8) + jump_offset_low;
                continue;
            }
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
                const int next_shift = query->m_bit_shift - shift;
                scaled_query.m_bit_shift = next_shift;
                const int offset_x = (PC[1] + query->m_obb_offset_x) << shift;
                scaled_query.m_obb_offset_x = offset_x;
                scaled_query.m_x = (m_x >> next_shift) - offset_x;
                const int offset_y = (PC[2] + query->m_obb_offset_y) << shift;
                scaled_query.m_obb_offset_y = offset_y;
                scaled_query.m_y = (m_y >> next_shift) - offset_y;
                const int offset_z = (PC[3] + query->m_obb_offset_z) << shift;
                scaled_query.m_obb_offset_z = offset_z;
                scaled_query.m_soa_z = (m_soa_z >> next_shift) - offset_z;
                scaled_query.m_radius = (m_radius >> next_shift) + 1;
                scaled_query.m_primitive_offset = query->m_primitive_offset;
                for (int i = 0; i < 1; i++)
                {
                    scaled_query.m_properties[i] = query->m_properties[i];
                }
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
            {
                return;
            }
            // The terminal cases share the hit path after this block.
            do
            {
            case PHYS_BVH_TERM8:
                {
                    jump_offset_low = PC[1];
                    break;
                }
            case PHYS_BVH_TERM16:
                {
                    jump_offset_high = PC[1];
                    jump_offset_high <<= 8;
                    jump_offset_low = PC[2];
                    jump_offset_low += jump_offset_high;
                    break;
                }
            case PHYS_BVH_TERM24:
                {
                    jump_offset_low = PC[1];
                    jump_offset_low <<= 16;
                    jump_offset_high = PC[2];
                    jump_offset_high <<= 8;
                    jump_offset_high += PC[3];
                    jump_offset_low += jump_offset_high;
                    break;
                }
            case PHYS_BVH_TERM32:
                {
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
                }
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
                    phys_error::MESSAGE_ERROR, 0x1298fedd, assert_buf, ".\\collide\\mopp\\machine\\hkMoppSphereVirtualMachine.cpp", 292);
                continue;
            }
        }
        PC += 4;
        if (int(x + radius) < int(jump_offset_low))
        {
            continue;
        }
        {
            const unsigned int offset = PC[-1];
            PC += offset;
            if (x > int(jump_offset_high + radius))
            {
                continue;
            }
            query_sphere_on_tree(query, PC - offset);
            continue;
        }
    }
}

// The console reads the high halfword of a quantized coordinate through a short pointer at +0 (big-endian);
// other hosts take the same high halfword arithmetically.
static __forceinline short quant_coord_high_half(const int &coordinate)
{
#if defined(_XBOX) && !defined(__clang__)
    return *(const short *)&coordinate;
#else
    return static_cast<short>(coordinate >> 16);
#endif
}

void phys_bvh_sphere_vm::shape_query_sphere(const phys_bvh_bytecode *code, const phys_sphere &sphere, phys_array<phys_bvh_primitive_info> *results)
{
    const unsigned char *bvh_commands = code->m_data.begin();
    int quant_radius;
    phys_bvh_sphere_vm_query query;
    m_out_primitives = results;
    query.m_obb_offset_x = 0;
    query.m_obb_offset_y = 0;
    query.m_obb_offset_z = 0;
    query.m_primitive_offset = 0;
    query.m_bit_shift = 16;
    query.m_properties[0] = 0;
    m_x = int((sphere.get_contact_position()(0) - code->m_bvh_info.m_offset(0)) * code->m_bvh_info.get_offset_scale()) - 1;
    // match: 0x68e248: read the signed big-endian high halfword of the quantized coordinate.
    query.m_x = quant_coord_high_half(m_x);
    m_y = int((sphere.get_contact_position()(1) - code->m_bvh_info.m_offset(1)) * code->m_bvh_info.get_offset_scale()) - 1;
    // match: 0x68e248: read the signed big-endian high halfword of the quantized coordinate.
    query.m_y = quant_coord_high_half(m_y);
    m_soa_z = int((sphere.get_contact_position()(2) - code->m_bvh_info.m_offset(2)) * code->m_bvh_info.get_offset_scale()) - 1;
    // match: 0x68e248: read the signed big-endian high halfword of the quantized coordinate.
    query.m_soa_z = quant_coord_high_half(m_soa_z);
    quant_radius = int(sphere.get_convex_radius() * code->m_bvh_info.get_offset_scale()) + 2;
    // match: 0x68e248: read the signed big-endian high halfword of the quantized coordinate.
    query.m_soa_z = quant_coord_high_half(m_soa_z);
    m_radius = quant_radius;
    query.m_radius = (quant_radius >> 16) + 1;
    query_sphere_on_tree(&query, bvh_commands);
}
