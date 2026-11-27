// hkinternal/hkMoppEarlyExitObbVirtualMachine.obj: from the records 0x6a5600 0x6a5f18 0x6a6238
// @compiler xdk5632
// @flags /TP /O2
#include "phys/phys_collide.h"
#include "phys/phys_out_stream.h"
#include "phys/phys_solver_check.h"

extern "C" {
// @address 0x7ceb7f g_phys_bvh_early_exit_obb_enabled
char g_phys_bvh_early_exit_obb_enabled;
}


#include "phys/phys_err_stream.h"

#include "phys/phys_error.h"

extern "C" {
}

#include "phys/phys_bvh_commands.h"

typedef int phys_bvh_fixed_point;


#include "phys/phys_bvh_vm.h"

#include "phys/phys_bvh_early_exit_obb_vm.h"

static __forceinline phys_simd_scalar phys_bvh_early_exit_obb_vm_length3(const phys_vec4 &v)
{
    __vector4 temp = __vmsum3fp(v.m_simd_quad, v.m_simd_quad);
    phys_simd_scalar res;
    res.set(__fsqrts(temp.v[0]));
    return res;
}

// @address 0x6a5600 ?query_obb_on_tree@phys_bvh_early_exit_obb_vm@@IAAXPBUphys_bvh_early_exit_obb_vm_query@@PBE@Z
void phys_bvh_early_exit_obb_vm::query_obb_on_tree(const phys_bvh_early_exit_obb_vm_query *query, const unsigned char *PC)
{
    phys_bvh_early_exit_obb_vm_query scaled_query;
    int jump_offset_high;
    int jump_offset_low;

    while (1)
    {
        const phys_bvh_commands command = phys_bvh_commands(PC[0]);
        if (m_obb_hit_found)
        {
            return;
        }
        int query_max;
        int x;
        int radius;
        int query_min;

        switch (command)
        {
        case PHYS_BVH_SPLIT_YZ:
            jump_offset_high = PC[1] * 2;
            jump_offset_low = PC[2] * 2;
            x = query->m_center_y + query->m_center_z;
            radius = query->m_center_radius + (query->m_center_radius >> 1) + 1;
            break;
        case PHYS_BVH_SPLIT_YMZ:
            jump_offset_high = PC[1] * 2;
            jump_offset_low = PC[2] * 2;
            x = query->m_center_y + (255 - query->m_center_z);
            radius = query->m_center_radius + (query->m_center_radius >> 1) + 1;
            break;
        case PHYS_BVH_SPLIT_XZ:
            jump_offset_high = PC[1] * 2;
            jump_offset_low = PC[2] * 2;
            x = query->m_center_x + query->m_center_z;
            radius = query->m_center_radius + (query->m_center_radius >> 1) + 1;
            break;
        case PHYS_BVH_SPLIT_XMZ:
            jump_offset_high = PC[1] * 2;
            jump_offset_low = PC[2] * 2;
            x = query->m_center_x + (255 - query->m_center_z);
            radius = query->m_center_radius + (query->m_center_radius >> 1) + 1;
            break;
        case PHYS_BVH_SPLIT_XY:
            jump_offset_high = PC[1] * 2;
            jump_offset_low = PC[2] * 2;
            x = query->m_center_x + query->m_center_y;
            radius = query->m_center_radius + (query->m_center_radius >> 1) + 1;
            break;
        case PHYS_BVH_SPLIT_XMY:
            jump_offset_high = PC[1] * 2;
            jump_offset_low = PC[2] * 2;
            x = query->m_center_x + (255 - query->m_center_y);
            radius = query->m_center_radius + (query->m_center_radius >> 1) + 1;
            break;
        case PHYS_BVH_SPLIT_XYZ:
            jump_offset_high = PC[1] * 2 + PC[1];
            jump_offset_low = PC[2] * 2 + PC[2];
            x = query->m_center_x + query->m_center_y + query->m_center_z;
            radius = query->m_center_radius * 4;
            break;
        case PHYS_BVH_SPLIT_XYMZ:
            jump_offset_high = PC[1] * 2 + PC[1];
            jump_offset_low = PC[2] * 2 + PC[2];
            x = query->m_center_x + query->m_center_y + (255 - query->m_center_z);
            radius = query->m_center_radius * 4;
            break;
        case PHYS_BVH_SPLIT_XMYZ:
            jump_offset_high = PC[1] * 2 + PC[1];
            jump_offset_low = PC[2] * 2 + PC[2];
            x = query->m_center_x + (255 - query->m_center_y) + query->m_center_z;
            radius = query->m_center_radius * 4;
            break;
        case PHYS_BVH_SPLIT_XMYMZ:
            jump_offset_high = PC[1] * 2 + PC[1];
            jump_offset_low = PC[2] * 2 + PC[2];
            x = query->m_center_x + (255 - query->m_center_y) + (255 - query->m_center_z);
            radius = query->m_center_radius * 4;
            break;
        case PHYS_BVH_SPLIT_X:
        case PHYS_BVH_SPLIT_Y:
        case PHYS_BVH_SPLIT_Z:
            {
                jump_offset_high = PC[1];
                jump_offset_low = PC[2];
                query_min = (&query->m_x_lo)[command - PHYS_BVH_SPLIT_X];
                query_max = (&query->m_x_hi)[command - PHYS_BVH_SPLIT_X];
                PC += 4;
                if (query_max <= jump_offset_low)
                {
                    continue;
                }
                const unsigned int offset = PC[-1];
                PC += offset;
                if (query_min >= jump_offset_high)
                {
                    continue;
                }
                query_obb_on_tree(query, PC - offset);
                continue;
            }
        case PHYS_BVH_SINGLE_SPLIT_X:
        case PHYS_BVH_SINGLE_SPLIT_Y:
        case PHYS_BVH_SINGLE_SPLIT_Z:
            {
                jump_offset_low = PC[1];
                query_min = (&query->m_x_lo)[command - PHYS_BVH_SINGLE_SPLIT_X];
                query_max = (&query->m_x_hi)[command - PHYS_BVH_SINGLE_SPLIT_X];
                PC += 3;
                if (query_max <= jump_offset_low)
                {
                    continue;
                }
                const unsigned int offset = PC[-1];
                PC += offset;
                if (query_min > jump_offset_low)
                {
                    continue;
                }
                query_obb_on_tree(query, PC - offset);
                continue;
            }
        case PHYS_BVH_SPLIT_JUMP_X:
        case PHYS_BVH_SPLIT_JUMP_Y:
        case PHYS_BVH_SPLIT_JUMP_Z:
            {
                jump_offset_high = PC[1];
                jump_offset_low = PC[2];
                query_min = (&query->m_x_lo)[command - PHYS_BVH_SPLIT_JUMP_X];
                query_max = (&query->m_x_hi)[command - PHYS_BVH_SPLIT_JUMP_X];
                const unsigned int bvh_left_offset = (PC[3] << 8) + PC[4];
                PC += 7;
                if (query_max <= jump_offset_low)
                {
                    PC += bvh_left_offset;
                    continue;
                }
                const unsigned int bvh_right_offset = (PC[-2] << 8) + PC[-1];
                PC += bvh_right_offset;
                if (query_min >= jump_offset_high)
                {
                    continue;
                }
                query_obb_on_tree(query, PC - bvh_right_offset + bvh_left_offset);
                continue;
            }
        case PHYS_BVH_DOUBLE_CUT_X:
        case PHYS_BVH_DOUBLE_CUT_Y:
        case PHYS_BVH_DOUBLE_CUT_Z:
            query_min = (&query->m_x_lo)[command - PHYS_BVH_DOUBLE_CUT_X];
            query_max = (&query->m_x_hi)[command - PHYS_BVH_DOUBLE_CUT_X];
            jump_offset_low = PC[1];
            jump_offset_high = PC[2];
            if (query_max < jump_offset_low || query_min >= jump_offset_high)
            {
                return;
            }
            else
            {
                PC += 3;
                continue;
            }
        case PHYS_BVH_DOUBLE_CUT24_X:
        case PHYS_BVH_DOUBLE_CUT24_Y:
        case PHYS_BVH_DOUBLE_CUT24_Z:
            query_min = (&m_x_lo)[command - PHYS_BVH_DOUBLE_CUT24_X];
            query_max = (&m_x_hi)[command - PHYS_BVH_DOUBLE_CUT24_X];
            jump_offset_low = read_u24(PC + 1);
            jump_offset_high = read_u24(PC + 4);
            if (query_max >= jump_offset_low && query_min <= jump_offset_high)
            {
                PC += 7;
                continue;
            }
            else
            {
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
                const int scaled_shift2 = 16 - scaled_query.m_bit_shift;
                scaled_query.m_x_lo = (m_x_lo >> scaled_shift2);
                scaled_query.m_x_lo -= scaled_query.m_obb_offset_x;
                scaled_query.m_y_lo = (m_y_lo >> scaled_shift2);
                scaled_query.m_y_lo -= scaled_query.m_obb_offset_y;
                scaled_query.m_z_lo = (m_z_lo >> scaled_shift2);
                scaled_query.m_z_lo -= scaled_query.m_obb_offset_z;
                scaled_query.m_x_hi = (m_x_hi >> scaled_shift2);
                scaled_query.m_x_hi += 1;
                scaled_query.m_x_hi -= scaled_query.m_obb_offset_x;
                scaled_query.m_y_hi = (m_y_hi >> scaled_shift2);
                scaled_query.m_y_hi += 1;
                scaled_query.m_y_hi -= scaled_query.m_obb_offset_y;
                scaled_query.m_z_hi = (m_z_hi >> scaled_shift2);
                scaled_query.m_z_hi += 1;
                scaled_query.m_z_hi -= scaled_query.m_obb_offset_z;
                scaled_query.m_center_x = (m_center_x >> scaled_shift2);
                scaled_query.m_center_x -= scaled_query.m_obb_offset_x;
                scaled_query.m_center_y = (m_center_y >> scaled_shift2);
                scaled_query.m_center_y -= scaled_query.m_obb_offset_y;
                scaled_query.m_center_z = (m_center_z >> scaled_shift2);
                scaled_query.m_center_z -= scaled_query.m_obb_offset_z;
                scaled_query.m_center_radius = (m_center_radius >> scaled_shift2) + 1;
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
        case PHYS_BVH_TERM8:
            jump_offset_low = PC[1];
            jump_offset_low += query->m_primitive_offset;
            add_cast_hit(jump_offset_low, query->m_properties);
            return;
        case PHYS_BVH_TERM16:
            jump_offset_high = PC[1];
            jump_offset_high <<= 8;
            jump_offset_low = PC[2];
            jump_offset_low += jump_offset_high;
            jump_offset_low += query->m_primitive_offset;
            add_cast_hit(jump_offset_low, query->m_properties);
            return;
        case PHYS_BVH_TERM24:
            jump_offset_low = PC[1];
            jump_offset_low <<= 16;
            jump_offset_high = PC[2];
            jump_offset_high <<= 8;
            jump_offset_high += PC[3];
            jump_offset_low += jump_offset_high;
            jump_offset_low += query->m_primitive_offset;
            add_cast_hit(jump_offset_low, query->m_properties);
            return;
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
            jump_offset_low += query->m_primitive_offset;
            add_cast_hit(jump_offset_low, query->m_properties);
            return;
        case 0x30: case 0x31: case 0x32: case 0x33: case 0x34: case 0x35: case 0x36: case 0x37:
        case 0x38: case 0x39: case 0x3a: case 0x3b: case 0x3c: case 0x3d: case 0x3e: case 0x3f:
        case 0x40: case 0x41: case 0x42: case 0x43: case 0x44: case 0x45: case 0x46: case 0x47:
        case 0x48: case 0x49: case 0x4a: case 0x4b: case 0x4c: case 0x4d: case 0x4e: case 0x4f:
            jump_offset_low = command - PHYS_BVH_TERM4_0;
            {
                jump_offset_low += query->m_primitive_offset;
                add_cast_hit(jump_offset_low, query->m_properties);
                return;
            }
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
                goto copy_properties;
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
            }
copy_properties:
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
                    phys_error::MESSAGE_ERROR, 0x1298fedd, assert_buf, ".\\collide\\mopp\\machine\\hkMoppEarlyExitObbVirtualMachine.cpp", 342);
                continue;
            }
        }
        PC += 4;
        if (x + radius <= jump_offset_low)
        {
            continue;
        }
        const unsigned int offset = PC[-1];
        PC += offset;
        if (x >= jump_offset_high + radius)
        {
            continue;
        }
        query_obb_on_tree(query, PC - offset);
        continue;

    }
}

// @address 0x6a5f18 ?generate_query_from_node@phys_bvh_early_exit_obb_vm@@IAAXABVphys_vec4@@ABVphys_transform@@ABMAAUphys_bvh_early_exit_obb_vm_query@@@Z
void phys_bvh_early_exit_obb_vm::generate_query_from_node(
    const phys_vec4 &extent, const phys_transform &bounding_to_world, const float &radius, phys_bvh_early_exit_obb_vm_query &query)
{
    const phys_rotation &rot = bounding_to_world.get_rotation();
    phys_vec4 ex;
    ex.set_broadcast3_clobber_w(extent, 0);
    phys_vec4 ey;
    ey.set_broadcast3_clobber_w(extent, 1);
    phys_vec4 ez;
    ez.set_broadcast3_clobber_w(extent, 2);

    phys_vec4 x;
    x.set_mul4(ex, rot.get_column(0));
    phys_vec4 y;
    y.set_mul4(ey, rot.get_column(1));
    phys_vec4 z;
    z.set_mul4(ez, rot.get_column(2));
    x.set_abs4(x);
    y.set_abs4(y);
    z.set_abs4(z);

    phys_vec4 aabb_max;
    aabb_max.set_add4(x, y);
    aabb_max.add4(z);
    phys_vec4 r;
    r.flags_set_all(radius);
    aabb_max.add4(r);
    phys_vec4 aabb_min;
    aabb_min.set_neg4(aabb_max);
    aabb_max.add4(bounding_to_world.get_translation());
    aabb_min.add4(bounding_to_world.get_translation());

    m_x_lo = to_int_min((aabb_min(0) - m_bvh_code->m_bvh_info.m_offset(0)) * m_bvh_code->m_bvh_info.get_offset_scale());
    m_x_hi = to_int_max((aabb_max(0) - m_bvh_code->m_bvh_info.m_offset(0)) * m_bvh_code->m_bvh_info.get_offset_scale());
    m_y_lo = to_int_min((aabb_min(1) - m_bvh_code->m_bvh_info.m_offset(1)) * m_bvh_code->m_bvh_info.get_offset_scale());
    m_y_hi = to_int_max((aabb_max(1) - m_bvh_code->m_bvh_info.m_offset(1)) * m_bvh_code->m_bvh_info.get_offset_scale());
    m_z_lo = to_int_min((aabb_min(2) - m_bvh_code->m_bvh_info.m_offset(2)) * m_bvh_code->m_bvh_info.get_offset_scale());
    m_z_hi = to_int_max((aabb_max(2) - m_bvh_code->m_bvh_info.m_offset(2)) * m_bvh_code->m_bvh_info.get_offset_scale());

    m_center_x = (m_x_lo + m_x_hi) >> 1;
    m_center_y = (m_y_lo + m_y_hi) >> 1;
    m_center_z = (m_z_lo + m_z_hi) >> 1;

    const float r2 = float(phys_bvh_early_exit_obb_vm_length3(extent)) + radius;
    m_center_radius = to_int_max(r2 * m_bvh_code->m_bvh_info.get_offset_scale());

    query.m_x_lo = (m_x_lo >> 16);
    query.m_x_hi = (m_x_hi >> 16) + 1;
    query.m_y_lo = (m_y_lo >> 16);
    query.m_y_hi = (m_y_hi >> 16) + 1;
    query.m_z_lo = (m_z_lo >> 16);
    query.m_z_hi = (m_z_hi >> 16) + 1;
    query.m_center_x = (m_center_x >> 16);
    query.m_center_y = (m_center_y >> 16);
    query.m_center_z = (m_center_z >> 16);
    query.m_center_radius = (m_center_radius >> 16) + 1;

    query.m_obb_offset_x = 0;
    query.m_obb_offset_y = 0;
    query.m_obb_offset_z = 0;
    query.m_primitive_offset = 0;
    query.m_bit_shift = 0;
    for (int i = 0; i < 1; i++) {
        query.m_properties[i] = 0;
    }
}

// @address 0x6a6238 ?query_obb@phys_bvh_early_exit_obb_vm@@QAA?AVphys_bool@@PBVphys_bvh_bytecode@@ABVphys_transform@@ABVphys_vec4@@ABM@Z
phys_bool phys_bvh_early_exit_obb_vm::query_obb(const phys_bvh_bytecode *code, const phys_transform &bounding_to_world,
    const phys_vec4 &extent, const float &radius)
{
    if (g_phys_bvh_early_exit_obb_enabled == '\0') {
        g_phys_bvh_early_exit_obb_enabled = process_flying_colors(4);
        if (g_phys_bvh_early_exit_obb_enabled == '\0') {
            return false;
        }
    }
    m_bvh_code = code;
    const unsigned char *bvh_commands = &code->m_data[0];

    phys_bvh_early_exit_obb_vm_query query;
    generate_query_from_node(extent, bounding_to_world, radius, query);
    query.m_properties[0] = 0;
    m_obb_hit_found = false;

    query_obb_on_tree(&query, bvh_commands);
    return m_obb_hit_found;
}
