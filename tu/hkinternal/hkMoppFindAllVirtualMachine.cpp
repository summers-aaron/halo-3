// hkinternal/hkMoppFindAllVirtualMachine.obj: from the records 0x6a7608 0x6a7b58 0x6a8068
// @compiler xdk5632
// @flags /TP /O2 /Oc
#include "phys/phys_base.h"
#include "phys/phys_out_stream.h"


#include "phys/phys_bvh_vm.h"






#include "phys/phys_err_stream.h"

#include "phys/phys_error.h"

extern "C" {
}

#include "phys/phys_bvh_vm.h"

#include "phys/phys_bvh_commands.h"

#include "phys/phys_bvh_find_all_vm.h"
#include "phys/phys_collide.h"
#include "phys/phys_statistics_collector.h"
#include "phys/phys_writer.h"

// @address 0x6a7608 ?query_on_tree@phys_bvh_find_all_vm@@QAAXPBUphys_bvh_find_all_vm_query@1@PBE@Z
void phys_bvh_find_all_vm::query_on_tree(const phys_bvh_find_all_vm_query *query, const unsigned char *PC)
{
    phys_bvh_find_all_vm_query scaled_query;
    unsigned int jump_offset_high;
    unsigned int jump_offset_low;

    while (1)
    {
        const phys_bvh_commands command = phys_bvh_commands(PC[0]);

        switch (command)
        {
        case PHYS_BVH_SPLIT_X:
        case PHYS_BVH_SPLIT_Y:
        case PHYS_BVH_SPLIT_Z:
        case PHYS_BVH_SPLIT_YZ:
        case PHYS_BVH_SPLIT_YMZ:
        case PHYS_BVH_SPLIT_XZ:
        case PHYS_BVH_SPLIT_XMZ:
        case PHYS_BVH_SPLIT_XY:
        case PHYS_BVH_SPLIT_XMY:
        case PHYS_BVH_SPLIT_XYZ:
        case PHYS_BVH_SPLIT_XYMZ:
        case PHYS_BVH_SPLIT_XMYZ:
        case PHYS_BVH_SPLIT_XMYMZ:
            {
                const unsigned int offset = PC[3];
                PC += 4;
                query_on_tree(query, PC);
                PC += offset;
                continue;
            }
        case PHYS_BVH_SINGLE_SPLIT_X:
        case PHYS_BVH_SINGLE_SPLIT_Y:
        case PHYS_BVH_SINGLE_SPLIT_Z:
            {
                const unsigned int offset = PC[2];
                PC += 3;
                query_on_tree(query, PC);
                PC += offset;
                continue;
            }
        case PHYS_BVH_SPLIT_JUMP_X:
        case PHYS_BVH_SPLIT_JUMP_Y:
        case PHYS_BVH_SPLIT_JUMP_Z:
            {
                const unsigned int bvh_left_offset = (PC[3] << 8) + PC[4];
                const unsigned int bvh_right_offset = (PC[5] << 8) + PC[6];
                PC += 7;
                query_on_tree(query, PC + bvh_left_offset);
                PC += bvh_right_offset;
                continue;
            }
        case PHYS_BVH_DOUBLE_CUT_X:
        case PHYS_BVH_DOUBLE_CUT_Y:
        case PHYS_BVH_DOUBLE_CUT_Z:
            {
                PC += 3;
                continue;
            }
        case PHYS_BVH_DOUBLE_CUT24_X:
        case PHYS_BVH_DOUBLE_CUT24_Y:
        case PHYS_BVH_DOUBLE_CUT24_Z:
            {
                PC += 7;
                continue;
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
                phys_singleton<phys_error>::s_instance->message(
                    phys_error::MESSAGE_ERROR, 0x1298fedd, assert_buf, ".\\collide\\mopp\\machine\\hkMoppFindAllVirtualMachine.cpp", 113);
                continue;
            }
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
            {
                break;
            }
        case PHYS_BVH_RETURN:
            {
                return;
            }
        }
        jump_offset_low += query->m_primitive_offset;
        add_cast_hit(jump_offset_low, query->m_properties);
        return;
    }
}

// @address 0x6a7b58 ?query_on_tree_right@phys_bvh_find_all_vm@@QAAXPBUphys_bvh_find_all_vm_query@1@PBE@Z
void phys_bvh_find_all_vm::query_on_tree_right(const phys_bvh_find_all_vm_query *query, const unsigned char *PC)
{
    phys_bvh_find_all_vm_query scaled_query;
    unsigned int jump_offset_high;
    unsigned int jump_offset_low;

    while (1)
    {
        const phys_bvh_commands command = phys_bvh_commands(PC[0]);

        switch (command)
        {
        case PHYS_BVH_SPLIT_X:
        case PHYS_BVH_SPLIT_Y:
        case PHYS_BVH_SPLIT_Z:
        case PHYS_BVH_SPLIT_YZ:
        case PHYS_BVH_SPLIT_YMZ:
        case PHYS_BVH_SPLIT_XZ:
        case PHYS_BVH_SPLIT_XMZ:
        case PHYS_BVH_SPLIT_XY:
        case PHYS_BVH_SPLIT_XMY:
        case PHYS_BVH_SPLIT_XYZ:
        case PHYS_BVH_SPLIT_XYMZ:
        case PHYS_BVH_SPLIT_XMYZ:
        case PHYS_BVH_SPLIT_XMYMZ:
            {
                const unsigned int offset = PC[3];
                PC += 4;
                PC += offset;
                continue;
            }
        case PHYS_BVH_SINGLE_SPLIT_X:
        case PHYS_BVH_SINGLE_SPLIT_Y:
        case PHYS_BVH_SINGLE_SPLIT_Z:
            {
                const unsigned int offset = PC[2];
                PC += 3;
                PC += offset;
                continue;
            }
        case PHYS_BVH_SPLIT_JUMP_X:
        case PHYS_BVH_SPLIT_JUMP_Y:
        case PHYS_BVH_SPLIT_JUMP_Z:
            {
                const unsigned int bvh_left_offset = (PC[3] << 8) + PC[4];
                const unsigned int bvh_right_offset = (PC[5] << 8) + PC[6];
                PC += 7;
                
                PC += bvh_right_offset;
                continue;
            }
        case PHYS_BVH_DOUBLE_CUT_X:
        case PHYS_BVH_DOUBLE_CUT_Y:
        case PHYS_BVH_DOUBLE_CUT_Z:
            {
                PC += 3;
                continue;
            }
        case PHYS_BVH_DOUBLE_CUT24_X:
        case PHYS_BVH_DOUBLE_CUT24_Y:
        case PHYS_BVH_DOUBLE_CUT24_Z:
            {
                PC += 7;
                continue;
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
                phys_singleton<phys_error>::s_instance->message(
                    phys_error::MESSAGE_ERROR, 0x1298fedd, assert_buf, ".\\collide\\mopp\\machine\\hkMoppFindAllVirtualMachine.cpp", 211);
                continue;
            }
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
            {
                break;
            }
        case PHYS_BVH_RETURN:
            {
                return;
            }
        }
        jump_offset_low += query->m_primitive_offset;
        add_cast_hit(jump_offset_low, query->m_properties);
        return;
    }
}

// @address 0x6a8068 ?query_on_tree_left@phys_bvh_find_all_vm@@QAAXPBUphys_bvh_find_all_vm_query@1@PBE@Z
void phys_bvh_find_all_vm::query_on_tree_left(const phys_bvh_find_all_vm_query *query, const unsigned char *PC)
{
    phys_bvh_find_all_vm_query scaled_query;
    unsigned int jump_offset_high;
    unsigned int jump_offset_low;

    while (1)
    {
        const phys_bvh_commands command = phys_bvh_commands(PC[0]);

        switch (command)
        {
        case PHYS_BVH_SPLIT_X:
        case PHYS_BVH_SPLIT_Y:
        case PHYS_BVH_SPLIT_Z:
        case PHYS_BVH_SPLIT_YZ:
        case PHYS_BVH_SPLIT_YMZ:
        case PHYS_BVH_SPLIT_XZ:
        case PHYS_BVH_SPLIT_XMZ:
        case PHYS_BVH_SPLIT_XY:
        case PHYS_BVH_SPLIT_XMY:
        case PHYS_BVH_SPLIT_XYZ:
        case PHYS_BVH_SPLIT_XYMZ:
        case PHYS_BVH_SPLIT_XMYZ:
        case PHYS_BVH_SPLIT_XMYMZ:
            {
                const unsigned int offset = PC[3];
                PC += 4;
                continue;
            }
        case PHYS_BVH_SINGLE_SPLIT_X:
        case PHYS_BVH_SINGLE_SPLIT_Y:
        case PHYS_BVH_SINGLE_SPLIT_Z:
            {
                const unsigned int offset = PC[2];
                PC += 3;
                continue;
            }
        case PHYS_BVH_SPLIT_JUMP_X:
        case PHYS_BVH_SPLIT_JUMP_Y:
        case PHYS_BVH_SPLIT_JUMP_Z:
            {
                const unsigned int bvh_left_offset = (PC[3] << 8) + PC[4];
                const unsigned int bvh_right_offset = (PC[5] << 8) + PC[6];
                PC += 7;
                
                PC += bvh_left_offset;
                continue;
            }
        case PHYS_BVH_DOUBLE_CUT_X:
        case PHYS_BVH_DOUBLE_CUT_Y:
        case PHYS_BVH_DOUBLE_CUT_Z:
            {
                PC += 3;
                continue;
            }
        case PHYS_BVH_DOUBLE_CUT24_X:
        case PHYS_BVH_DOUBLE_CUT24_Y:
        case PHYS_BVH_DOUBLE_CUT24_Z:
            {
                PC += 7;
                continue;
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
                phys_singleton<phys_error>::s_instance->message(
                    phys_error::MESSAGE_ERROR, 0x1298fedd, assert_buf, ".\\collide\\mopp\\machine\\hkMoppFindAllVirtualMachine.cpp", 305);
                continue;
            }
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
            {
                break;
            }
        case PHYS_BVH_RETURN:
            {
                return;
            }
        }
        jump_offset_low += query->m_primitive_offset;
        add_cast_hit(jump_offset_low, query->m_properties);
        return;
    }
}
