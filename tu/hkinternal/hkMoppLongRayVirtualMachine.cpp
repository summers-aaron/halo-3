// hkinternal/hkMoppLongRayVirtualMachine.obj: from the records 0x68f220 0x690310 0x690438
// @compiler xdk5632
// @flags /TP /O2 /Oc
#include "phys/phys_base.h"
#include <vectorintrinsics.h>
#include "phys/phys_solver_check.h"
#include "phys/phys_collide.h"
#include "phys/phys_out_stream.h"

typedef unsigned int phys_shape_key;
typedef __declspec(align(16)) char phys_shape_buffer[512];















#include "phys/phys_err_stream.h"

#include "phys/phys_error.h"
#include "phys/phys_statistics_collector.h"
#include "phys/phys_writer.h"

extern "C" {
// @address 0x7ceb7c g_phys_bvh_long_ray_flying_colors_flag
char g_phys_bvh_long_ray_flying_colors_flag;
}

#include "phys/phys_bvh_vm.h"

#include "phys/phys_bvh_commands.h"
#include "phys/phys_bvh_long_ray_vm.h"

extern "C" {
// @address 0x20d68 k_phys_u8_to_real
const float k_phys_u8_to_real[256] = {
    0.0f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f,
    8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f,
    16.0f, 17.0f, 18.0f, 19.0f, 20.0f, 21.0f, 22.0f, 23.0f,
    24.0f, 25.0f, 26.0f, 27.0f, 28.0f, 29.0f, 30.0f, 31.0f,
    32.0f, 33.0f, 34.0f, 35.0f, 36.0f, 37.0f, 38.0f, 39.0f,
    40.0f, 41.0f, 42.0f, 43.0f, 44.0f, 45.0f, 46.0f, 47.0f,
    48.0f, 49.0f, 50.0f, 51.0f, 52.0f, 53.0f, 54.0f, 55.0f,
    56.0f, 57.0f, 58.0f, 59.0f, 60.0f, 61.0f, 62.0f, 63.0f,
    64.0f, 65.0f, 66.0f, 67.0f, 68.0f, 69.0f, 70.0f, 71.0f,
    72.0f, 73.0f, 74.0f, 75.0f, 76.0f, 77.0f, 78.0f, 79.0f,
    80.0f, 81.0f, 82.0f, 83.0f, 84.0f, 85.0f, 86.0f, 87.0f,
    88.0f, 89.0f, 90.0f, 91.0f, 92.0f, 93.0f, 94.0f, 95.0f,
    96.0f, 97.0f, 98.0f, 99.0f, 100.0f, 101.0f, 102.0f, 103.0f,
    104.0f, 105.0f, 106.0f, 107.0f, 108.0f, 109.0f, 110.0f, 111.0f,
    112.0f, 113.0f, 114.0f, 115.0f, 116.0f, 117.0f, 118.0f, 119.0f,
    120.0f, 121.0f, 122.0f, 123.0f, 124.0f, 125.0f, 126.0f, 127.0f,
    128.0f, 129.0f, 130.0f, 131.0f, 132.0f, 133.0f, 134.0f, 135.0f,
    136.0f, 137.0f, 138.0f, 139.0f, 140.0f, 141.0f, 142.0f, 143.0f,
    144.0f, 145.0f, 146.0f, 147.0f, 148.0f, 149.0f, 150.0f, 151.0f,
    152.0f, 153.0f, 154.0f, 155.0f, 156.0f, 157.0f, 158.0f, 159.0f,
    160.0f, 161.0f, 162.0f, 163.0f, 164.0f, 165.0f, 166.0f, 167.0f,
    168.0f, 169.0f, 170.0f, 171.0f, 172.0f, 173.0f, 174.0f, 175.0f,
    176.0f, 177.0f, 178.0f, 179.0f, 180.0f, 181.0f, 182.0f, 183.0f,
    184.0f, 185.0f, 186.0f, 187.0f, 188.0f, 189.0f, 190.0f, 191.0f,
    192.0f, 193.0f, 194.0f, 195.0f, 196.0f, 197.0f, 198.0f, 199.0f,
    200.0f, 201.0f, 202.0f, 203.0f, 204.0f, 205.0f, 206.0f, 207.0f,
    208.0f, 209.0f, 210.0f, 211.0f, 212.0f, 213.0f, 214.0f, 215.0f,
    216.0f, 217.0f, 218.0f, 219.0f, 220.0f, 221.0f, 222.0f, 223.0f,
    224.0f, 225.0f, 226.0f, 227.0f, 228.0f, 229.0f, 230.0f, 231.0f,
    232.0f, 233.0f, 234.0f, 235.0f, 236.0f, 237.0f, 238.0f, 239.0f,
    240.0f, 241.0f, 242.0f, 243.0f, 244.0f, 245.0f, 246.0f, 247.0f,
    248.0f, 249.0f, 250.0f, 251.0f, 252.0f, 253.0f, 254.0f, 255.0f
};
}

#define PHYS_U8_TO_REAL(x) float(k_phys_u8_to_real[(x)])


__forceinline void phys_bvh_long_ray_vm::add_cast_hit(unsigned int key, const unsigned int properties[1])
{
    phys_shape_buffer shape_scratch_buffer;
    const phys_shape *bounding_child_shape = m_shape_container->get_child_shape(key, shape_scratch_buffer);
    if (m_ray_input.m_ray_compound_filter)
    {
        if (!m_ray_input.m_ray_compound_filter->allows_collision(m_ray_input, *bounding_child_shape, *m_shape_container, key))
        {
            return;
        }
    }

    if (m_ray_output_ptr)
    {
        m_ray_output_ptr->change_key_level(1);
        phys_bool hit = bounding_child_shape->trace_ray(m_ray_input, *m_ray_output_ptr);
        m_ray_output_ptr->change_key_level(-1);
        if (hit)
        {
            m_obb_hit_found = hit;
            m_early_out_hit_frac = m_ray_output_ptr->m_hit_frac;
            m_ray_output_ptr->set_cached_key(key);
        }
    }
    else
    {
        phys_collision_proxy child_proxy(m_ray_proxy);
        child_proxy.set_proxy_shape(bounding_child_shape, key);
        bounding_child_shape->trace_ray_with_collector(m_ray_input, child_proxy, *m_collector);
        m_early_out_hit_frac = m_collector->m_early_out_hit_frac;
    }
}

// @address 0x68f220 ?query_ray_on_tree@phys_bvh_long_ray_vm@@IAAXPBUquery_int@1@PBEQAUquery_float@1@H@Z
void phys_bvh_long_ray_vm::query_ray_on_tree(const query_int *query, const unsigned char *PC, query_float *const float_query, int bvh_chunk_offset)
{
    if (!g_phys_bvh_long_ray_flying_colors_flag)
    {
        g_phys_bvh_long_ray_flying_colors_flag = process_flying_colors(4);
        if (!g_phys_bvh_long_ray_flying_colors_flag)
        {
            return;
        }
    }

    query_int scaled_query;
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
            hi = float(PC[1] * 2);
            lo = float(PC[2] * 2);
            from = float_query->m_ray_from(1) + float_query->m_ray_from(2);
            to = float_query->m_ray_to(1) + float_query->m_ray_to(2);
            goto split_common;
        case PHYS_BVH_SPLIT_YMZ:
            hi = float(PC[1] * 2 - 255);
            lo = float(PC[2] * 2 - 255);
            from = float_query->m_ray_from(1) - float_query->m_ray_from(2);
            to = float_query->m_ray_to(1) - float_query->m_ray_to(2);
            goto split_common;
        case PHYS_BVH_SPLIT_XZ:
            hi = float(PC[1] * 2);
            lo = float(PC[2] * 2);
            from = float_query->m_ray_from(0) + float_query->m_ray_from(2);
            to = float_query->m_ray_to(0) + float_query->m_ray_to(2);
            goto split_common;
        case PHYS_BVH_SPLIT_XMZ:
            hi = float(PC[1] * 2 - 255);
            lo = float(PC[2] * 2 - 255);
            from = float_query->m_ray_from(0) - float_query->m_ray_from(2);
            to = float_query->m_ray_to(0) - float_query->m_ray_to(2);
            goto split_common;
        case PHYS_BVH_SPLIT_XY:
            hi = float(PC[1] * 2);
            lo = float(PC[2] * 2);
            from = float_query->m_ray_from(0) + float_query->m_ray_from(1);
            to = float_query->m_ray_to(0) + float_query->m_ray_to(1);
            goto split_common;
        case PHYS_BVH_SPLIT_XMY:
            hi = float(PC[1] * 2 - 255);
            lo = float(PC[2] * 2 - 255);
            from = float_query->m_ray_from(0) - float_query->m_ray_from(1);
            to = float_query->m_ray_to(0) - float_query->m_ray_to(1);
            goto split_common;
        case PHYS_BVH_SPLIT_XYZ:
            hi = float(PC[1] * 3);
            lo = float(PC[2] * 3);
            from = float_query->m_ray_from(0) + float_query->m_ray_from(1) + float_query->m_ray_from(2);
            to = float_query->m_ray_to(0) + float_query->m_ray_to(1) + float_query->m_ray_to(2);
            goto split_common;
        case PHYS_BVH_SPLIT_XYMZ:
            hi = float(PC[1] * 3 - 255);
            lo = float(PC[2] * 3 - 255);
            from = float_query->m_ray_from(0) + float_query->m_ray_from(1) - float_query->m_ray_from(2);
            to = float_query->m_ray_to(0) + float_query->m_ray_to(1) - float_query->m_ray_to(2);
            goto split_common;
        case PHYS_BVH_SPLIT_XMYZ:
            hi = float(PC[1] * 3 - 255);
            lo = float(PC[2] * 3 - 255);
            from = float_query->m_ray_from(0) - float_query->m_ray_from(1) + float_query->m_ray_from(2);
            to = float_query->m_ray_to(0) - float_query->m_ray_to(1) + float_query->m_ray_to(2);
            goto split_common;
        case PHYS_BVH_SPLIT_XMYMZ:
            hi = float(PC[1] * 3 - 510);
            lo = float(PC[2] * 3 - 510);
            from = float_query->m_ray_from(0) - float_query->m_ray_from(1) - float_query->m_ray_from(2);
            to = float_query->m_ray_to(0) - float_query->m_ray_to(1) - float_query->m_ray_to(2);
            goto split_common;
        case PHYS_BVH_SINGLE_SPLIT_X:
        case PHYS_BVH_SINGLE_SPLIT_Y:
        case PHYS_BVH_SINGLE_SPLIT_Z:
            bvh_scale_index = command - PHYS_BVH_SINGLE_SPLIT_X;
            lo = PHYS_U8_TO_REAL(PC[1]);
            hi = lo + 1.0f;
            from = float_query->m_ray_from(bvh_scale_index);
            to = float_query->m_ray_to(bvh_scale_index);
            PC += 3;
            bvh_left_offset = 0;
            bvh_right_offset = PC[-1];
            goto split_compare;
        case PHYS_BVH_SPLIT_JUMP_X:
        case PHYS_BVH_SPLIT_JUMP_Y:
        case PHYS_BVH_SPLIT_JUMP_Z:
            bvh_scale_index = command - PHYS_BVH_SPLIT_JUMP_X;
            hi = PHYS_U8_TO_REAL(PC[1]);
            lo = PHYS_U8_TO_REAL(PC[2]);
            from = float_query->m_ray_from(bvh_scale_index);
            to = float_query->m_ray_to(bvh_scale_index);
            bvh_left_offset = (PC[3] << 8) + PC[4];
            PC += 7;
            bvh_right_offset = (PC[-2] << 8) + PC[-1];
            goto split_compare;
        case PHYS_BVH_SPLIT_X:
        case PHYS_BVH_SPLIT_Y:
        case PHYS_BVH_SPLIT_Z:
            bvh_scale_index = command - PHYS_BVH_SPLIT_X;
            hi = PHYS_U8_TO_REAL(PC[1]);
            lo = PHYS_U8_TO_REAL(PC[2]);
            from = float_query->m_ray_from(bvh_scale_index);
            to = float_query->m_ray_to(bvh_scale_index);
split_common:
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

                query_float new_query;
                new_query.m_ray_from = float_query->m_ray_from;
                new_query.m_ray_to = float_query->m_ray_to;

                if (from_minus_hi < to_hi)
                {
                    if (from_minus_hi * to_hi < 0.0f)
                    {
                        const float t = from_minus_hi / (from_minus_hi - to_hi);
                        new_query.m_ray_to.set_lerp4(float_query->m_ray_from, float_query->m_ray_to, phys_simd_scalar(t));
                    }
                    query_ray_on_tree(query, PC - bvh_right_offset + bvh_left_offset, &new_query, bvh_chunk_offset);
                    if (from_minus_lo * to_lo < 0.0f)
                    {
                        const float t = from_minus_lo / (from_minus_lo - to_lo);
                        float_query->m_ray_from.set_lerp4(float_query->m_ray_from, float_query->m_ray_to, phys_simd_scalar(t));
                    }
                    if (m_early_out_hit_frac < 1.0f)
                    {
                        phys_vec4 new_to;
                        new_to.set_lerp4(m_ray_input.m_ray_from, m_ray_input.m_ray_to, phys_simd_scalar(m_early_out_hit_frac));
                        float_query->m_ray_to.set_sub4(new_to, m_bvh_code->m_bvh_info.m_offset);
                        float_query->m_ray_to.mul4(phys_simd_scalar(query->m_scale));
                        float_query->m_ray_to.sub4(query->m_offset);
                        if (bvh_scale_index < 3 && float_query->m_ray_to(bvh_scale_index) < lo)
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
                        new_query.m_ray_to.set_lerp4(float_query->m_ray_from, float_query->m_ray_to, phys_simd_scalar(t));
                    }
                    query_ray_on_tree(query, PC, &new_query, bvh_chunk_offset);
                    if (from_minus_hi * to_hi < 0.0f)
                    {
                        const float t = from_minus_hi / (from_minus_hi - to_hi);
                        float_query->m_ray_from.set_lerp4(float_query->m_ray_from, float_query->m_ray_to, phys_simd_scalar(t));
                    }
                    if (m_early_out_hit_frac < 1.0f)
                    {
                        phys_vec4 new_to;
                        new_to.set_lerp4(m_ray_input.m_ray_from, m_ray_input.m_ray_to, phys_simd_scalar(m_early_out_hit_frac));
                        float_query->m_ray_to.set_sub4(new_to, m_bvh_code->m_bvh_info.m_offset);
                        float_query->m_ray_to.mul4(phys_simd_scalar(query->m_scale));
                        float_query->m_ray_to.sub4(query->m_offset);
                        if (bvh_scale_index < 3 && float_query->m_ray_to(bvh_scale_index) > hi)
                        {
                            return;
                        }
                    }
                    PC = PC - bvh_right_offset + bvh_left_offset;
                    continue;
                }
            }
        case PHYS_BVH_DOUBLE_CUT24_X:
        case PHYS_BVH_DOUBLE_CUT24_Y:
        case PHYS_BVH_DOUBLE_CUT24_Z:
            bvh_scale_index = command - PHYS_BVH_DOUBLE_CUT24_X;
            lo = float(read_u24(PC + 1)) * m_int_to_float_scale;
            hi = float(read_u24(PC + 4)) * m_int_to_float_scale;
            lo = lo * query->m_scale;
            hi = hi * query->m_scale;
            lo = lo - query->m_offset(bvh_scale_index);
            hi = hi - query->m_offset(bvh_scale_index);
            PC += 7;
            goto double_cut;
        case PHYS_BVH_DOUBLE_CUT_X:
        case PHYS_BVH_DOUBLE_CUT_Y:
        case PHYS_BVH_DOUBLE_CUT_Z:
            bvh_scale_index = command - PHYS_BVH_DOUBLE_CUT_X;
            lo = PHYS_U8_TO_REAL(PC[1]);
            hi = PHYS_U8_TO_REAL(PC[2]);
            PC += 3;
double_cut:
            {
                from = float_query->m_ray_from(bvh_scale_index);
                to = float_query->m_ray_to(bvh_scale_index);
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
                phys_vec4 ray_from = float_query->m_ray_from;
                phys_vec4 ray_to = float_query->m_ray_to;
                {
                    const float a = from - hi;
                    const float b = to - hi;
                    if (a * b < 0.0f)
                    {
                        const float t = a / (a - b);
                        (&float_query->m_ray_from)[1 - flip].set_lerp4(ray_from, ray_to, phys_simd_scalar(t));
                    }
                }
                {
                    const float a = from - lo;
                    const float b = to - lo;
                    if (a * b < 0.0f)
                    {
                        const float t = a / (a - b);
                        (&float_query->m_ray_from)[flip].set_lerp4(ray_from, ray_to, phys_simd_scalar(t));
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
                offset.set(PHYS_U8_TO_REAL(PC[1]), PHYS_U8_TO_REAL(PC[2]), PHYS_U8_TO_REAL(PC[3]), 0.0f);
                const float scale = float(1 << shift);
                float_query->m_ray_from.sub4(offset);
                float_query->m_ray_to.sub4(offset);
                float_query->m_ray_from.mul4(phys_simd_scalar(scale));
                float_query->m_ray_to.mul4(phys_simd_scalar(scale));
                scaled_query.m_offset.set_add4(query->m_offset, offset);
                scaled_query.m_offset.mul4(phys_simd_scalar(scale));
                scaled_query.m_bit_shift = query->m_bit_shift + shift;
                scaled_query.m_scale = query->m_scale * scale;
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
        case PHYS_BVH_TERM8:
            jump_offset_low = PC[1];
            goto add_terminal;
        case PHYS_BVH_TERM16:
            jump_offset_high = PC[1];
            jump_offset_high <<= 8;
            jump_offset_low = PC[2];
            jump_offset_low += jump_offset_high;
            goto add_terminal;
        case PHYS_BVH_TERM24:
            jump_offset_low = PC[1];
            jump_offset_low <<= 16;
            jump_offset_high = PC[2];
            jump_offset_high <<= 8;
            jump_offset_high += PC[3];
            jump_offset_low += jump_offset_high;
            goto add_terminal;
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
            goto add_terminal;
        case 0x30: case 0x31: case 0x32: case 0x33: case 0x34: case 0x35: case 0x36: case 0x37:
        case 0x38: case 0x39: case 0x3a: case 0x3b: case 0x3c: case 0x3d: case 0x3e: case 0x3f:
        case 0x40: case 0x41: case 0x42: case 0x43: case 0x44: case 0x45: case 0x46: case 0x47:
        case 0x48: case 0x49: case 0x4a: case 0x4b: case 0x4c: case 0x4d: case 0x4e: case 0x4f:
            jump_offset_low = command - PHYS_BVH_TERM4_0;
            {
add_terminal:
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
        default:
            {
                char assert_buf[512];
                phys_err_stream report_stream(assert_buf, sizeof(assert_buf));
                report_stream << "Unknown command - This mopp data has been corrupted (check for memory trashing), "
                     "or an hkMoppBvTreeShape has been pointed at invalid mopp data.\n";
                phys_singleton<phys_error>::s_instance->message(
                    phys_error::MESSAGE_ERROR, 0x1298fedd, assert_buf, ".\\collide\\mopp\\machine\\hkMoppLongRayVirtualMachine.cpp", 538);
                continue;
            }
        }
                const unsigned int v = scaled_query.m_properties[0];
                if (query != &scaled_query)
                {
                    scaled_query = *query;
                    query = &scaled_query;
                }
                scaled_query.m_properties[0] = v;
                continue;
    }
}

__forceinline void phys_bvh_long_ray_vm::query_long_ray_sub(const phys_bvh_bytecode *code, const phys_ray_input &input)
{
    m_bvh_code = code;
    const unsigned char *bvh_commands = &m_bvh_code->m_data[0];

    query_int query;
    query.m_offset.set_zero4();
    m_int_to_float_scale = 1.0f / m_bvh_code->m_bvh_info.get_offset_scale();
    query.m_scale = m_bvh_code->m_bvh_info.get_offset_scale() / 65536.0f;

    query_float float_query;
    float_query.m_ray_from.set_sub4(input.m_ray_from, code->m_bvh_info.m_offset);
    float_query.m_ray_to.set_sub4(input.m_ray_to, code->m_bvh_info.m_offset);
    float_query.m_ray_from.mul4(phys_simd_scalar(query.m_scale));
    float_query.m_ray_to.mul4(phys_simd_scalar(query.m_scale));

    m_ray_input = input;
    m_obb_hit_found = false;

    query.m_primitive_offset = 0;
    query.m_bit_shift = 0;
    for (int i = 0; i < 1; i++)
        query.m_properties[i] = 0;

    query_ray_on_tree(&query, bvh_commands, &float_query, 0);
}

// @address 0x690438 ?query_long_ray@phys_bvh_long_ray_vm@@QAAXPBVphys_shape_container@@PBVphys_bvh_bytecode@@ABUphys_ray_input@@ABVphys_collision_proxy@@AAVphys_ray_hit_collector@@@Z
void phys_bvh_long_ray_vm::query_long_ray(const phys_shape_container *collection, const phys_bvh_bytecode *code, const phys_ray_input &input,
    const phys_collision_proxy &body, phys_ray_hit_collector &collector)
{
    m_shape_container = collection;
    m_ray_output_ptr = 0;
    m_ray_proxy = &body;
    m_collector = &collector;
    m_early_out_hit_frac = collector.m_early_out_hit_frac;
    query_long_ray_sub(code, input);
}
// @address 0x690310 ?query_long_ray@phys_bvh_long_ray_vm@@QAA?AVphys_bool@@PBVphys_shape_container@@PBVphys_bvh_bytecode@@ABUphys_ray_input@@AAUphys_ray_output@@@Z
phys_bool phys_bvh_long_ray_vm::query_long_ray(
    const phys_shape_container *collection, const phys_bvh_bytecode *code, const phys_ray_input &input, phys_ray_output &ray_result)
{
    m_shape_container = collection;
    m_ray_output_ptr = &ray_result;
    m_early_out_hit_frac = 1.0f;
    query_long_ray_sub(code, input);
    return m_obb_hit_found;
}
