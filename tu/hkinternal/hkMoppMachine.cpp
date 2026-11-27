// hkinternal/hkMoppMachine.obj: from the records 0x157af0 0x677808 0x677858 0x677888 0x6778d0 0x677910 0x677948 0x677978 0x6779a8 0x677d10 0x678078 0x678160 0x678248 0x679330 0x67a0a8
// @compiler xdk5632
// @flags /TP /O2 /Oc
// @address 0x677808 phys_bvh_early_exit_obb_vm_query_obb
// @address 0x677858 phys_bvh_sphere_vm_query_sphere
// @address 0x677888 phys_bvh_kdop_geom_vm_run
// @address 0x6778d0 phys_bvh_find_all_vm_get_all_keys
// @address 0x677910 phys_bvh_obb_vm_query_obb
// @address 0x677948 phys_bvh_obb_vm_query_aabb
// @address 0x677978 phys_bvh_modify_vm_query_aabb
// @address 0x6779a8 phys_bvh_float_aabb_vm_query_planes
// @address 0x677d10 phys_bvh_float_aabb_vm_query_planes_opt
// @address 0x678078 phys_bvh_float_aabb_vm_query_sphere
// @address 0x678160 phys_bvh_float_aabb_vm_query_sphere_opt
#include "phys/phys_base_types.h"
#include <ppcintrinsics.h>
#include <vectorintrinsics.h>
#include "phys/phys_error.h"
#include "phys/phys_kdop26.h"
#include "phys/phys_bvh_commands.h"
#include "phys/phys_bvh_kdop_vm.h"
#include "phys/phys_bvh_modify_vm.h"
#include "phys/phys_bvh_early_exit_obb_vm.h"
#include "phys/phys_bvh_find_all_vm.h"
#include "phys/phys_bvh_sphere_vm.h"
#include "phys/phys_err_stream.h"
#include "phys/phys_thread_memory.h"
#include "phys/phys_aabb_types.h"
#include "phys/phys_collide.h"
#include "phys/phys_math.h"
#include "phys/phys_statistics_collector.h"
#include "phys/phys_writer.h"
#include "phys/phys_bvh_float_aabb_vm.h"






extern "C" bool phys_bvh_early_exit_obb_vm_query_obb(const phys_bvh_bytecode *code,
    const phys_transform &transform, const phys_vec4 &box_half_extents, const float &radius)
{
    phys_bvh_early_exit_obb_vm machine;
    return machine.query_obb(code, transform, box_half_extents, radius);
}

extern "C" void phys_bvh_sphere_vm_query_sphere(const phys_bvh_bytecode *code,
    const phys_sphere &sphere, phys_array<phys_bvh_primitive_info> *hits)
{
    phys_bvh_sphere_vm machine;
    machine.shape_query_sphere(code, sphere, hits);
}
extern "C" void phys_bvh_kdop_geom_vm_run(const phys_bvh_bytecode *code,
    const phys_bvh_kdop_query &query, phys_bvh_info *info)
{
    phys_bvh_kdop_vm machine;
    machine.query_bvh(code, query, info);
}
extern "C" void phys_bvh_find_all_vm_get_all_keys(const phys_bvh_bytecode *code,
    phys_array<phys_bvh_primitive_info> *hits)
{
    phys_bvh_find_all_vm machine;
    machine.query_all_leaves(code, hits);
}
extern "C" void phys_bvh_obb_vm_query_obb(const phys_bvh_bytecode *code,
    const phys_transform &transform, const phys_vec4 &box_half_extents, float radius,
    phys_array<phys_bvh_primitive_info> *hits)
{
    phys_bvh_obb_vm machine;
    machine.query_obb(code, transform, box_half_extents, radius, hits);
}
extern "C" void phys_bvh_obb_vm_query_aabb(const phys_bvh_bytecode *code,
    const phys_aabb &aabb, phys_array<phys_bvh_primitive_info> *hits)
{
    phys_bvh_obb_vm machine;
    machine.query_aabb(code, aabb, hits);
}
extern "C" void phys_bvh_modify_vm_query_aabb(const phys_bvh_bytecode *code,
    const phys_aabb &aabb, phys_bvh_modifier *modifier)
{
    phys_bvh_modify_vm machine;
    machine.query_aabb(code, aabb, modifier);
}






// @address 0x678248 ?query_bvh_tree@?$phys_bvh_float_aabb_vm@Uphys_bvh_planes_query_input@@Vphys_bvh_planes_query@@@@IAAXPBUphys_bvh_float_aabb_vm_query_int@1@PBEAAUphys_bvh_temp_aabb@1@@Z
// @address 0x6779a8
extern "C" void phys_bvh_float_aabb_vm_query_planes(const phys_bvh_bytecode *code,
    const phys_bvh_planes_query_input &query, phys_array<phys_bvh_primitive_info> *partial_hits_out,
    phys_array<phys_bvh_primitive_info> *fully_included_hits_out)
{
    phys_bvh_float_aabb_vm<phys_bvh_planes_query_input, phys_bvh_planes_query> machine;
    machine.query_bvh(code, query, partial_hits_out, fully_included_hits_out);
}


// @address 0x157af0 ?query_bvh_tree@?$phys_bvh_float_aabb_vm@Uphys_bvh_planes_query_input@@Vphys_bvh_planes_query_fast@@@@IAAXPBUphys_bvh_float_aabb_vm_query_int@1@PBEAAUphys_bvh_temp_aabb@1@@Z
template <class vm_query, class vm_query_object>
void phys_bvh_float_aabb_vm<vm_query, vm_query_object>::query_bvh_tree(
    const phys_bvh_float_aabb_vm_query_int *query, const unsigned char *PC, phys_bvh_temp_aabb &temp_aabb)
{
    phys_bvh_float_aabb_vm_query_int scaled_query;
    unsigned int jump_offset_high;
    unsigned int jump_offset_low;

    while (1)
    {
        const unsigned int command = PC[0];
        phys_bvh_temp_aabb new_aabb;

        switch (command)
        {
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
                jump_offset_high = PC[3];
                PC += 4;
                new_aabb.m_min_limit = temp_aabb.m_min_limit;
                new_aabb.m_max_limit = temp_aabb.m_max_limit;
                query_bvh_tree(query, PC, new_aabb);
                PC += jump_offset_high;
                continue;
            }
        case PHYS_BVH_SPLIT_X:
        case PHYS_BVH_SPLIT_Y:
        case PHYS_BVH_SPLIT_Z:
            {
                const int axis = command - PHYS_BVH_SPLIT_X;
                new_aabb.m_min_limit = temp_aabb.m_min_limit;
                new_aabb.m_max_limit = temp_aabb.m_max_limit;
                jump_offset_high = PC[1];
                jump_offset_low = PC[2];
                temp_aabb.m_max_limit(axis) = float(int((jump_offset_high << query->m_bit_shift) + (&query->m_obb_offset_x)[axis])) * m_reciprocal_scale;
                new_aabb.m_min_limit(axis) = float(int((jump_offset_low << query->m_bit_shift) + (&query->m_obb_offset_x)[axis])) * m_reciprocal_scale;
                jump_offset_low = 0;
                jump_offset_high = PC[3];
                PC += 4;
                goto split_common;
            }
        case PHYS_BVH_SINGLE_SPLIT_X:
        case PHYS_BVH_SINGLE_SPLIT_Y:
        case PHYS_BVH_SINGLE_SPLIT_Z:
            {
                const int axis = command - PHYS_BVH_SINGLE_SPLIT_X;
                new_aabb.m_min_limit = temp_aabb.m_min_limit;
                new_aabb.m_max_limit = temp_aabb.m_max_limit;
                jump_offset_low = PC[1];
                jump_offset_high = jump_offset_low + 1;
                jump_offset_low = jump_offset_low << query->m_bit_shift;
                jump_offset_high = jump_offset_high << query->m_bit_shift;
                jump_offset_low = jump_offset_low + (&query->m_obb_offset_x)[axis];
                jump_offset_high = jump_offset_high + (&query->m_obb_offset_x)[axis];
                new_aabb.m_min_limit(axis) = float(int(jump_offset_low)) * m_reciprocal_scale;
                temp_aabb.m_max_limit(axis) = float(int(jump_offset_high)) * m_reciprocal_scale;
                jump_offset_low = 0;
                jump_offset_high = PC[2];
                PC += 3;
                goto split_common;
            }
        case PHYS_BVH_SPLIT_JUMP_X:
        case PHYS_BVH_SPLIT_JUMP_Y:
        case PHYS_BVH_SPLIT_JUMP_Z:
            {
                const int axis = command - PHYS_BVH_SPLIT_JUMP_X;
                new_aabb.m_min_limit = temp_aabb.m_min_limit;
                new_aabb.m_max_limit = temp_aabb.m_max_limit;
                jump_offset_high = PC[1];
                jump_offset_low = PC[2];
                temp_aabb.m_max_limit(axis) = float(int((jump_offset_high << query->m_bit_shift) + (&query->m_obb_offset_x)[axis])) * m_reciprocal_scale;
                new_aabb.m_min_limit(axis) = float(int((jump_offset_low << query->m_bit_shift) + (&query->m_obb_offset_x)[axis])) * m_reciprocal_scale;
                jump_offset_low = (PC[3] << 8) + PC[4];
                jump_offset_high = (PC[5] << 8) + PC[6];
                PC += 7;
            }
split_common:
            {
                while (1)
                {
                    const int cut_plane = PC[jump_offset_low];
                    if (cut_plane < PHYS_BVH_DOUBLE_CUT_X || cut_plane > PHYS_BVH_DOUBLE_CUT24_Z)
                    {
                        break;
                    }
                    if (cut_plane >= PHYS_BVH_DOUBLE_CUT24_X)
                    {
                        const int axis = cut_plane - PHYS_BVH_DOUBLE_CUT24_X;
                        temp_aabb.m_min_limit(axis) = float(read_u24(PC + jump_offset_low + 1)) * m_reciprocal_scale;
                        temp_aabb.m_max_limit(axis) = float(read_u24(PC + jump_offset_low + 4)) * m_reciprocal_scale;
                        jump_offset_low += 7;
                    }
                    else
                    {
                        const int axis = cut_plane - PHYS_BVH_DOUBLE_CUT_X;
                        temp_aabb.m_min_limit(axis) = float(int((PC[jump_offset_low + 1] << query->m_bit_shift) +
                            (&query->m_obb_offset_x)[axis])) * m_reciprocal_scale;
                        temp_aabb.m_max_limit(axis) = float(int((PC[jump_offset_low + 2] << query->m_bit_shift) +
                            (&query->m_obb_offset_x)[axis])) * m_reciprocal_scale;
                        jump_offset_low += 3;
                    }
                }
                while (1)
                {
                    const int cut_plane = PC[jump_offset_high];
                    if (cut_plane < PHYS_BVH_DOUBLE_CUT_X || cut_plane > PHYS_BVH_DOUBLE_CUT24_Z)
                    {
                        break;
                    }
                    if (cut_plane >= PHYS_BVH_DOUBLE_CUT24_X)
                    {
                        const int axis = cut_plane - PHYS_BVH_DOUBLE_CUT24_X;
                        new_aabb.m_min_limit(axis) = float(read_u24(PC + jump_offset_high + 1)) * m_reciprocal_scale;
                        new_aabb.m_max_limit(axis) = float(read_u24(PC + jump_offset_high + 4)) * m_reciprocal_scale;
                        jump_offset_high += 7;
                    }
                    else
                    {
                        const int axis = cut_plane - PHYS_BVH_DOUBLE_CUT_X;
                        new_aabb.m_min_limit(axis) = float(int((PC[jump_offset_high + 1] << query->m_bit_shift) +
                            (&query->m_obb_offset_x)[axis])) * m_reciprocal_scale;
                        new_aabb.m_max_limit(axis) = float(int((PC[jump_offset_high + 2] << query->m_bit_shift) +
                            (&query->m_obb_offset_x)[axis])) * m_reciprocal_scale;
                        jump_offset_high += 3;
                    }
                }
                const phys_bvh_query_object::phys_check_collision_result right_hit = m_query_object.check_bounds_collision(new_aabb,
                    (const phys_bvh_find_all_vm::phys_bvh_find_all_vm_query *)query, PC + jump_offset_high);
                const phys_bvh_query_object::phys_check_collision_result left_hit = m_query_object.check_bounds_collision(temp_aabb,
                    (const phys_bvh_find_all_vm::phys_bvh_find_all_vm_query *)query, PC + jump_offset_low);
                if (right_hit != phys_bvh_query_object::PHYS_BVH_PARTIAL_HIT)
                {
                    if (left_hit != phys_bvh_query_object::PHYS_BVH_PARTIAL_HIT)
                    {
                        return;
                    }
                    PC += jump_offset_low;
                    continue;
                }
                if (left_hit != phys_bvh_query_object::PHYS_BVH_PARTIAL_HIT)
                {
                    temp_aabb.m_min_limit = new_aabb.m_min_limit;
                    temp_aabb.m_max_limit = new_aabb.m_max_limit;
                    PC += jump_offset_high;
                    continue;
                }
                query_bvh_tree(query, PC + jump_offset_high, new_aabb);
                PC += jump_offset_low;
                continue;
            }
        case PHYS_BVH_DOUBLE_CUT24_X:
        case PHYS_BVH_DOUBLE_CUT24_Y:
        case PHYS_BVH_DOUBLE_CUT24_Z:
            {
                const int axis = command - PHYS_BVH_DOUBLE_CUT24_X;
                temp_aabb.m_min_limit(axis) = float(read_u24(PC + 1)) * m_reciprocal_scale;
                temp_aabb.m_max_limit(axis) = float(read_u24(PC + 4)) * m_reciprocal_scale;
                PC += 7;
                goto double_cut_common;
            }
        case PHYS_BVH_DOUBLE_CUT_X:
        case PHYS_BVH_DOUBLE_CUT_Y:
        case PHYS_BVH_DOUBLE_CUT_Z:
            {
                const int axis = command - PHYS_BVH_DOUBLE_CUT_X;
                temp_aabb.m_min_limit(axis) = float(int((PC[1] << query->m_bit_shift) + (&query->m_obb_offset_x)[axis])) * m_reciprocal_scale;
                temp_aabb.m_max_limit(axis) = float(int((PC[2] << query->m_bit_shift) + (&query->m_obb_offset_x)[axis])) * m_reciprocal_scale;
                PC += 3;
            }
double_cut_common:
            {
                if (PC[0] >= PHYS_BVH_DOUBLE_CUT_X && PC[0] <= PHYS_BVH_DOUBLE_CUT24_Z)
                {
                    continue;
                }
                const phys_bvh_query_object::phys_check_collision_result hit = m_query_object.check_bounds_collision(temp_aabb,
                    (const phys_bvh_find_all_vm::phys_bvh_find_all_vm_query *)query, PC);
                if (hit == phys_bvh_query_object::PHYS_BVH_PARTIAL_HIT)
                {
                    continue;
                }
                else
                {
                    return;
                }
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
                scaled_query.m_obb_offset_x = (PC[1] << query->m_bit_shift) + query->m_obb_offset_x;
                scaled_query.m_obb_offset_y = (PC[2] << query->m_bit_shift) + query->m_obb_offset_y;
                scaled_query.m_obb_offset_z = (PC[3] << query->m_bit_shift) + query->m_obb_offset_z;
                scaled_query.m_bit_shift = query->m_bit_shift - shift;
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
        case PHYS_BVH_RETURN:
            {
                return;
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
            break;
        case PHYS_BVH_PROPERTY8_0:
        case PHYS_BVH_PROPERTY8_1:
        case PHYS_BVH_PROPERTY8_2:
        case PHYS_BVH_PROPERTY8_3:
            {
                const int property = PC[0] - PHYS_BVH_PROPERTY8_0;
                const unsigned int value = PC[1];
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
                const int property = PC[0] - PHYS_BVH_PROPERTY16_0;
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
                const int property = PC[0] - PHYS_BVH_PROPERTY32_0;
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
                phys_error::get_singleton_instance().message(phys_error::MESSAGE_ERROR, 0x1298fedd, assert_buf,
                    "..\\hkinternal/collide/mopp/machine/hkMoppUsingFloatAabbVirtualMachine.inl", 301);
                continue;
            }
        }
        jump_offset_low += query->m_primitive_offset;
        add_cast_hit(jump_offset_low, query->m_properties);
        return;
    }
}

// @address 0x677d10
extern "C" void phys_bvh_float_aabb_vm_query_planes_opt(const phys_bvh_bytecode *code,
    const phys_bvh_planes_query_input &query, phys_array<phys_bvh_primitive_info> *partial_hits_out,
    phys_array<phys_bvh_primitive_info> *fully_included_hits_out)
{
    phys_bvh_float_aabb_vm<phys_bvh_planes_query_input, phys_bvh_planes_query_fast> machine;
    machine.query_bvh(code, query, partial_hits_out, fully_included_hits_out);
}

// @address 0x679330 ?query_bvh_tree@?$phys_bvh_float_aabb_vm@Vphys_sphere@@Vphys_bvh_sphere_query@@@@IAAXPBUphys_bvh_float_aabb_vm_query_int@1@PBEAAUphys_bvh_temp_aabb@1@@Z
// @address 0x678078
extern "C" void phys_bvh_float_aabb_vm_query_sphere(const phys_bvh_bytecode *code,
    const phys_sphere &query, phys_array<phys_bvh_primitive_info> *partial_hits_out,
    phys_array<phys_bvh_primitive_info> *fully_included_hits_out)
{
    phys_bvh_float_aabb_vm<phys_sphere, phys_bvh_sphere_query> machine;
    machine.query_bvh(code, query, partial_hits_out, fully_included_hits_out);
}


// @address 0x67a0a8 ?query_bvh_tree@?$phys_bvh_float_aabb_vm@Vphys_sphere@@Vphys_bvh_sphere_query_fast@@@@IAAXPBUphys_bvh_float_aabb_vm_query_int@1@PBEAAUphys_bvh_temp_aabb@1@@Z
// @address 0x678160
extern "C" void phys_bvh_float_aabb_vm_query_sphere_opt(const phys_bvh_bytecode *code,
    const phys_sphere &query, phys_array<phys_bvh_primitive_info> *partial_hits_out,
    phys_array<phys_bvh_primitive_info> *fully_included_hits_out)
{
    phys_bvh_float_aabb_vm<phys_sphere, phys_bvh_sphere_query_fast> machine;
    machine.query_bvh(code, query, partial_hits_out, fully_included_hits_out);
}

