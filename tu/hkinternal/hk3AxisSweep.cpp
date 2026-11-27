// hkinternal/hk3AxisSweep.obj: from the records 0x67bb38 0x67bb70 0x67bb98 0x67bbc0 0x67bbe0 0x67bce8 0x67bda8 0x67be30 0x67be38 0x67bea8 0x67bf68 0x67bf70 0x67c1a8 0x67c248 0x67c450 0x67c460 0x67c480 0x67c5f0 0x67c658 0x67c6c0 0x67c790 0x67c878 0x67c958 0x67ca90 0x67ccb8 0x67d098 0x67d5a0 0x67d650 0x67d668 0x67dd28 0x67df20 0x67e120 0x67ef08 0x67fb68 0x680808 0x680e00 0x681370 0x6816b0 0x681b50 0x681ba0 0x681cb0 0x6822d0 0x682780 0x682e70 0x682ff0 0x6830f0
// @compiler xdk5632
// @flags /TP /O2 /Oc
// @address 0x67bb38 ?mem_zero16@@YAXPAXH@Z
// @address 0x67bb70 ?static_flip_bit@@YAXPAIH@Z
// @address 0x67bb98 ?static_clear_bit@@YAXPAIH@Z
// @address 0x67bbc0 ?static_is_bit_set@@YAHPAIH@Z
// @address 0x67bce8 ?fix_nodes_after_delete@phys_axis_sweep@@IAAXPAVphys_sweep_node@1@HAAV21@@Z
// @address 0x67be38 ?find@phys_sweep_axis@phys_axis_sweep@@QBAPBVphys_sweep_endpoint@2@PBV32@0G@Z
// @address 0x67bf68 ?get_original_aabb@phys_axis_sweep@@QBAABVphys_aabb@@XZ
// @address 0x67c1a8 ?remove@phys_sweep_axis@phys_axis_sweep@@QAAXHH@Z
// @address 0x67c248 ?set_bits_from_x_interval@phys_axis_sweep@@IBAXHHABVphys_sweep_node@1@GPAI@Z
// @address 0x67c450 ?get_object_count@phys_axis_sweep@@UAAHXZ
// @address 0x67c460 ?get_aabb_cache_size@phys_axis_sweep@@UBAHXZ
// @address 0x67c5f0 ?begin_overlap@phys_axis_sweep@@SIXAAVphys_sweep_node@1@0AAV?$phys_array@Vphys_broadphase_handle_pair@@@@@Z
// @address 0x67c658 ?end_overlap@phys_axis_sweep@@SIXAAVphys_sweep_node@1@0AAV?$phys_array@Vphys_broadphase_handle_pair@@@@@Z
// @address 0x67c6c0 ?begin_overlap_check_marker@phys_axis_sweep@@SIXPAVphys_sweep_marker@1@AAVphys_sweep_node@1@H1AAV?$phys_array@Vphys_broadphase_handle_pair@@@@@Z
// @address 0x67c958 ?insert@phys_sweep_axis@phys_axis_sweep@@QAAXPAVphys_sweep_node@2@HGGAAG1@Z
// @address 0x67ca90 ?requery_single_object@phys_axis_sweep@@UBAXPBVphys_broadphase_handle@@AAV?$phys_array@Vphys_broadphase_handle_pair@@@@@Z
// @address 0x67d5a0 ?get_all_aabbs@phys_axis_sweep@@UAAXAAV?$phys_array@Vphys_aabb@@@@@Z
// @address 0x67d650 ?get_handle_aabb@phys_axis_sweep@@UBAXPBVphys_broadphase_handle@@AAVphys_aabb@@@Z
// @address 0x681b50 ?phys_axis_sweep16_create_broadphase@@YAPAVphys_broadphase@@ABVphys_vec4@@0H@Z
// match: 0x680808: trivial vector storage preserves the calibration AABB scalar-store/splat dependencies.
#define PHYS_DISABLE_VEC4_CONSTRUCTORS
#include "cseries/crt_xdk.h"
// @address 0x6830f0 ??_Gphys_axis_sweep@@UAAPAXI@Z
#include "phys/phys_base_types.h"
#include "phys/phys_broadphase_cast_collector.h"
#include "phys/phys_memory_class.h"
#include <vectorintrinsics.h>
#include <ppcintrinsics.h>
#include <math.h>
#include "phys/phys_math.h"

#include "phys/phys_axis_sweep_constants.h"
// @address 0xf00 k_phys_axis_sweep_negate_mask
#include "phys/phys_axis_sweep.h"
#include "phys/phys_statistics_collector.h"
#include "phys/phys_local_buffer.h"
#include "phys/phys_thread_memory_allocator.h"
#include "phys/phys_algorithm_sort.h"
#include "phys/phys_int_union64.h"
#include "phys/phys_collide.h"
#include "phys/phys_algorithm.h"

// @address 0x726470 ?s_axis_max_value@phys_axis_sweep@@2PAMA
__declspec(align(16)) float phys_axis_sweep::s_axis_max_value[4] = { 65532.0f, 65532.0f, 65532.0f, 65532.0f };
const int phys_axis_sweep::phys_sweep_node::s_memberOffsets[6] = { 8, 10, 0, 4, 2, 6 };

struct phys_axis_sweep_overlaps : public phys_array<const phys_axis_sweep::phys_sweep_node *> {
    const phys_axis_sweep::phys_sweep_node **m_local_memory;

    __forceinline phys_axis_sweep_overlaps(int size) {
        const phys_axis_sweep::phys_sweep_node **local_memory =
            (const phys_axis_sweep::phys_sweep_node **)phys_alloc_stack<unsigned int>(size);
        m_data = local_memory;
        m_local_memory = local_memory;
        m_capacity_and_flags = size | PHYS_ARRAY_DONT_DEALLOCATE_FLAG;
    }

    __forceinline ~phys_axis_sweep_overlaps() {
        phys_free_stack((unsigned int *)m_local_memory);
    }
};


void mem_zero16(void *dest, int byte_size)
{
    byte_size >>= 4;
    phys_vec4 val;
    val.set_zero4();
    phys_vec4 *d = (phys_vec4 *)dest;
    while (byte_size >= 0) {
        *(d++) = val;
        byte_size--;
    }
}

__forceinline void static_flip_bit(unsigned int *bits, int index)
{
    int pos = index >> 5;
    int bit = 1 << (index & 0x1f);
    bits[pos] ^= bit;
}

__forceinline void static_clear_bit(unsigned int *bits, int index)
{
    int pos = index >> 5;
    int bit = 1 << (index & 0x1f);
    bits[pos] &= ~bit;
}

int static_is_bit_set(unsigned int *bits, int index)
{
    return bits[index >> 5] & (1 << (index & 31));
}

const phys_axis_sweep::phys_sweep_endpoint *phys_axis_sweep::phys_sweep_axis::find(const phys_sweep_endpoint *first,
    const phys_sweep_endpoint *last, unsigned short key) const
{
    int range_bytes = phys_get_byte_offset_int(first, last);
    while ((range_bytes & -4) > 64) {
        const phys_sweep_endpoint *middle = phys_add_byte_offset_const(first, (range_bytes >> 3) * sizeof(phys_sweep_endpoint));
        if (middle->m_value < key)
            first = middle;
        else
            last = middle;
        range_bytes = phys_get_byte_offset_int(first, last);
    }
    while (first->m_value < key)
        first++;
    return first;
}

const phys_aabb &phys_axis_sweep::get_original_aabb() const
{
    return m_original_aabb;
}

void phys_axis_sweep::phys_sweep_axis::remove(int sweep_min_index, int sweep_max_index)
{
    phys_sweep_endpoint *current = &m_sweep_endpoints[sweep_min_index];
    phys_sweep_endpoint *end = &m_sweep_endpoints[sweep_max_index] - 1;
    while (current < end) {
        *current = current[1];
        current++;
    }
    end = &m_sweep_endpoints[m_sweep_endpoints.getSize() - 2];
    m_sweep_endpoints.set_size_unchecked(m_sweep_endpoints.getSize() - 2);
    while (current < end) {
        *current = current[2];
        current++;
    }
}

// @address 0x67bbe0 ?fix_nodes_after_insert@phys_axis_sweep@@IAAXPAVphys_sweep_node@1@HAAV21@@Z
void phys_axis_sweep::fix_nodes_after_insert(phys_sweep_node *nodes, int sweep_node_count, phys_sweep_node &new_sweep_node)
{
    __declspec(align(16)) phys_sweep_node min_node;
    __declspec(align(16)) phys_sweep_node max_node;

    min_node.lower_x = new_sweep_node.lower_x - 1;
    min_node.max_x_val = new_sweep_node.lower_x - 1;
    min_node.min_y = new_sweep_node.min_y - 1;
    min_node.max_y = new_sweep_node.min_y - 1;
    min_node.lower_z = new_sweep_node.lower_z - 1;
    min_node.max_z_val = new_sweep_node.lower_z - 1;

    max_node.lower_x = new_sweep_node.max_x_val - 2;
    max_node.max_x_val = new_sweep_node.max_x_val - 2;
    max_node.min_y = new_sweep_node.max_y - 2;
    max_node.max_y = new_sweep_node.max_y - 2;
    max_node.lower_z = new_sweep_node.max_z_val - 2;
    max_node.max_z_val = new_sweep_node.max_z_val - 2;

#ifdef HALO3_64BIT
    phys_sweep_node *nodes_end = nodes + sweep_node_count;
    for (; nodes < nodes_end; nodes++) {
        nodes->lower_x = (unsigned short)(nodes->lower_x + (nodes->lower_x > min_node.lower_x) + (nodes->lower_x > max_node.lower_x));
        nodes->max_x_val = (unsigned short)(nodes->max_x_val + (nodes->max_x_val > min_node.max_x_val) + (nodes->max_x_val > max_node.max_x_val));
        nodes->min_y = (unsigned short)(nodes->min_y + (nodes->min_y > min_node.min_y) + (nodes->min_y > max_node.min_y));
        nodes->max_y = (unsigned short)(nodes->max_y + (nodes->max_y > min_node.max_y) + (nodes->max_y > max_node.max_y));
        nodes->lower_z = (unsigned short)(nodes->lower_z + (nodes->lower_z > min_node.lower_z) + (nodes->lower_z > max_node.lower_z));
        nodes->max_z_val = (unsigned short)(nodes->max_z_val + (nodes->max_z_val > min_node.max_z_val) + (nodes->max_z_val > max_node.max_z_val));
    }
#else
    __declspec(align(16)) unsigned int keep_ptr_mask_words[4] = { 0xffffffff, 0xffffffff, 0xffffffff, 0x00000000 };
    __vector4 keep_ptr_mask = *(__vector4 *)keep_ptr_mask_words;
    __vector4 mi = __lvx(&min_node, 0);
    __vector4 ma = __lvx(&max_node, 0);

    phys_sweep_node *nodes_end = nodes + sweep_node_count;
    for (; nodes < nodes_end; nodes++) {
        __vector4 m = *reinterpret_cast<__vector4 *>(nodes);
        __vector4 min_cmp_mask = __vcmpgtuh(m, mi);
        __vector4 max_cmp_mask = __vcmpgtuh(m, ma);
        __vector4 neg_add_vec = __vadduhm(min_cmp_mask, max_cmp_mask);
        __vector4 sub_vec = __vand(neg_add_vec, keep_ptr_mask);
        *reinterpret_cast<__vector4 *>(nodes) = __vsubuhm(m, sub_vec);
    }
#endif
}

void phys_axis_sweep::fix_nodes_after_delete(phys_sweep_node *nodes, int sweep_node_count, phys_sweep_node &old_node)
{
    __declspec(align(16)) phys_sweep_node min_node;
    __declspec(align(16)) phys_sweep_node max_node;

    min_node.lower_x = old_node.lower_x;
    min_node.max_x_val = old_node.lower_x;
    min_node.min_y = old_node.min_y;
    min_node.max_y = old_node.min_y;
    min_node.lower_z = old_node.lower_z;
    min_node.max_z_val = old_node.lower_z;
    min_node.m_handle = (phys_broadphase_handle *)~(unsigned long)0;

    max_node.lower_x = old_node.max_x_val;
    max_node.max_x_val = old_node.max_x_val;
    max_node.min_y = old_node.max_y;
    max_node.max_y = old_node.max_y;
    max_node.lower_z = old_node.max_z_val;
    max_node.max_z_val = old_node.max_z_val;
    max_node.m_handle = (phys_broadphase_handle *)~(unsigned long)0;

#ifdef HALO3_64BIT
    phys_sweep_node *nodes_end = nodes + sweep_node_count;
    for (; nodes < nodes_end; nodes++) {
        nodes->lower_x = (unsigned short)(nodes->lower_x - (nodes->lower_x > min_node.lower_x) - (nodes->lower_x > max_node.lower_x));
        nodes->max_x_val = (unsigned short)(nodes->max_x_val - (nodes->max_x_val > min_node.max_x_val) - (nodes->max_x_val > max_node.max_x_val));
        nodes->min_y = (unsigned short)(nodes->min_y - (nodes->min_y > min_node.min_y) - (nodes->min_y > max_node.min_y));
        nodes->max_y = (unsigned short)(nodes->max_y - (nodes->max_y > min_node.max_y) - (nodes->max_y > max_node.max_y));
        nodes->lower_z = (unsigned short)(nodes->lower_z - (nodes->lower_z > min_node.lower_z) - (nodes->lower_z > max_node.lower_z));
        nodes->max_z_val = (unsigned short)(nodes->max_z_val - (nodes->max_z_val > min_node.max_z_val) - (nodes->max_z_val > max_node.max_z_val));
    }
#else
    __declspec(align(16)) unsigned int keep_ptr_mask_words[4] = { 0xffffffff, 0xffffffff, 0xffffffff, 0x00000000 };
    __vector4 keep_ptr_mask = *(__vector4 *)keep_ptr_mask_words;
    __vector4 mi = __lvx(&min_node, 0);
    __vector4 ma = __lvx(&max_node, 0);

    phys_sweep_node *nodes_end = nodes + sweep_node_count;
    for (; nodes < nodes_end; nodes++) {
        __vector4 m = *reinterpret_cast<__vector4 *>(nodes);
        __vector4 min_cmp_mask = __vcmpgtuh(m, mi);
        __vector4 max_cmp_mask = __vcmpgtuh(m, ma);
        __vector4 neg_add_vec = __vadduhm(min_cmp_mask, max_cmp_mask);
        __vector4 add_vec = __vand(neg_add_vec, keep_ptr_mask);
        *reinterpret_cast<__vector4 *>(nodes) = __vadduhm(m, add_vec);
    }
#endif
}

namespace {
static __forceinline void clear_bitfield(unsigned int *bits, int bytes)
{
    phys_vec4 zero;
    zero.set_zero4();
    phys_vec4 *output = (phys_vec4 *)bits;
    for (int i = bytes >> 4; i >= 0; i--)
        *output++ = zero;
}
}

void phys_axis_sweep::set_bits_from_x_interval(int sweep_node_count, int sweep_x_value, const phys_sweep_node &node,
    unsigned short sweep_node_idx, unsigned int *bits) const
{
    int sweep_min_index = node.lower_x;
    int sweep_max_index = node.max_x_val;
    clear_bitfield(bits, sweep_node_count >> 3);
    const phys_sweep_endpoint *current = &m_axis[0].m_sweep_endpoints[1];
    if (m_sweep_marker_count != 0) {
        int marker_idx = sweep_x_value >> (16 - m_log2_marker_count);
        if (marker_idx > 0) {
            const phys_sweep_marker *marker = &m_markers[marker_idx - 1];
            static_flip_bit(bits, marker->m_node_idx);
            const unsigned short *overlapping = marker->m_overlapping_objects.begin();
            for (int i = marker->m_overlapping_objects.getSize() - 1; i >= 0; i--) {
                if (*overlapping != sweep_node_idx)
                    static_flip_bit(bits, *overlapping);
                overlapping++;
            }
            const phys_sweep_node *marker_node = &m_nodes[marker->m_node_idx];
            const phys_sweep_endpoint *marker_cur = &m_axis[0].m_sweep_endpoints[marker_node->lower_x + 1];
            const phys_sweep_endpoint *marker_end = &m_axis[0].m_sweep_endpoints[marker_node->max_x_val];
            while (marker_cur < marker_end) {
                if (!marker_cur->is_max_point())
                    static_clear_bit(bits, marker_cur->m_node_idx);
                marker_cur++;
            }
            current = &m_axis[0].m_sweep_endpoints[marker_node->lower_x + 1];
        }
    }
    const phys_sweep_endpoint *end = &m_axis[0].m_sweep_endpoints[sweep_min_index];
    while (current < end) {
        static_flip_bit(bits, current->m_node_idx);
        current++;
    }
    const phys_sweep_endpoint *interval_cur = current + 1;
    const phys_sweep_endpoint *interval_end = &m_axis[0].m_sweep_endpoints[sweep_max_index];
    while (interval_cur < interval_end) {
        if (!interval_cur->is_max_point())
            static_flip_bit(bits, interval_cur->m_node_idx);
        interval_cur++;
    }
}

int phys_axis_sweep::get_object_count()
{
    return m_nodes.getSize() - 1;
}

int phys_axis_sweep::get_aabb_cache_size() const
{
    int sweep_node_count = m_nodes.getSize() - m_sweep_marker_count;
    int endpoint_cnt = sweep_node_count << 1;
    int aabb_cache_size = 36;
    aabb_cache_size += endpoint_cnt * 12;
    return aabb_cache_size;
}

void __fastcall phys_axis_sweep::begin_overlap(phys_sweep_node &sweep_node_a, phys_sweep_node &sweep_node_b, phys_array<phys_broadphase_handle_pair> &pairs)
{
    phys_broadphase_handle_pair &pair = pairs.expand_one();
    pair.m_a = sweep_node_a.m_handle;
    pair.m_b = sweep_node_b.m_handle;
}

void __fastcall phys_axis_sweep::end_overlap(phys_sweep_node &sweep_node_a, phys_sweep_node &sweep_node_b, phys_array<phys_broadphase_handle_pair> &pairs)
{
    phys_broadphase_handle_pair &pair = pairs.expand_one();
    pair.m_a = sweep_node_a.m_handle;
    pair.m_b = sweep_node_b.m_handle;
}

void __fastcall phys_axis_sweep::begin_overlap_check_marker(
    phys_sweep_marker *markers, phys_sweep_node &sweep_node_a, int sweep_node_idx, phys_sweep_node &sweep_node_b,
        phys_array<phys_broadphase_handle_pair> &pairs)
{
    if (!sweep_node_b.is_marker()) {
        phys_broadphase_handle_pair &pair = pairs.expand_one();
        pair.m_a = sweep_node_a.m_handle;
        pair.m_b = sweep_node_b.m_handle;
    } else {
        phys_sweep_marker *marker = sweep_node_b.get_marker(markers);
        unsigned short index = (unsigned short)sweep_node_idx;
        marker->m_overlapping_objects.array_push_back(index);
    }
}

void phys_axis_sweep::phys_sweep_axis::insert(
    phys_sweep_node *nodes, int sweep_node_idx, unsigned short sweep_min_value, unsigned short sweep_max_value,
        unsigned short &sweep_min_index, unsigned short &sweep_max_index)
{
    int array_new_size = m_sweep_endpoints.getSize() + 2;
    m_sweep_endpoints.set_array_size(array_new_size);
    phys_sweep_endpoint *current = &m_sweep_endpoints[array_new_size - 3];
    while (sweep_max_value <= current->m_value) {
        current[2] = *current;
        current--;
    }
    current[2].m_node_idx = (unsigned short)sweep_node_idx;
    current[2].m_value = sweep_max_value;
    sweep_max_index = (unsigned short)((current - &m_sweep_endpoints[0]) + 2);
    while (sweep_min_value < current->m_value) {
        current[1] = *current;
        current--;
    }
    current[1].m_value = sweep_min_value;
    current[1].m_node_idx = (unsigned short)sweep_node_idx;
    sweep_min_index = (unsigned short)((current - &m_sweep_endpoints[0]) + 1);
}

void phys_axis_sweep::requery_single_object(const phys_broadphase_handle *handle, phys_array<phys_broadphase_handle_pair> &pairs) const
{
    int sweep_node_count = m_nodes.getSize();
    int fill_word_count = (sweep_node_count >> 5) + 8;
    unsigned int *allocated_bits = phys_alloc_stack<unsigned int>(fill_word_count);
    unsigned int *bits = allocated_bits;
    int sweep_node_idx = handle->m_handle_id;
    const phys_sweep_node &query_node = m_nodes[sweep_node_idx];
    set_bits_from_x_interval(sweep_node_count, m_axis[0].m_sweep_endpoints[query_node.lower_x].m_value, query_node, (unsigned short)sweep_node_idx, bits);
    phys_broadphase_handle_pair pair;
    pair.m_a = const_cast<phys_broadphase_handle *>(handle);
    const phys_sweep_node *nodes = &m_nodes[0];
    unsigned int *end = bits + (m_nodes.getSize() >> 5) + 1;
    for (; bits < end; nodes += 32, bits++) {
        const phys_sweep_node *node = nodes;
        unsigned int mask = *bits;
        while (mask != 0) {
            if ((unsigned char)mask == 0) {
                node += 8;
                mask >>= 8;
            } else {
                if ((mask & 1) && !node->yz_disjoint(query_node) && !node->is_marker()) {
                    pair.m_b = (phys_broadphase_handle *)node->m_handle;
                    pairs.array_push_back(pair);
                }
                node++;
                mask >>= 1;
            }
        }
    }
    phys_free_stack(allocated_bits);
}

void phys_axis_sweep::get_all_aabbs(phys_array<phys_aabb> &aabb_list)
{
    aabb_list.reserve_exactly(m_nodes.getSize() - m_sweep_marker_count);
    aabb_list.set_size_unchecked(m_nodes.getSize() - m_sweep_marker_count);
    int aabb_count = 0;
    for (int i = 0; i < m_nodes.getSize(); i++) {
        phys_sweep_node &node = m_nodes[i];
        if (!node.is_marker()) {
            get_aabb_from_node(node, aabb_list[aabb_count]);
            aabb_count++;
        }
    }
}

void phys_axis_sweep::get_handle_aabb(const phys_broadphase_handle *handle, phys_aabb &aabb) const
{
    const phys_sweep_node *nodes = &m_nodes[0];
    int sweep_node_idx = handle->m_handle_id;
    const phys_sweep_node *node = nodes + sweep_node_idx;
    get_aabb_from_node(*node, aabb);
}


// @address 0x67dd28 ?gather_memory_stats@phys_axis_sweep@@UBAXPAVphys_statistics_collector@@@Z
void phys_axis_sweep::gather_memory_stats(phys_statistics_collector *collector) const
{
  collector->begin_stats_object("3AxisSweep", phys_statistics_collector::stat_category_engine, this);
  collector->add_array_stats("Nodes", phys_statistics_collector::stat_category_engine, m_nodes);
  collector->add_array_stats("Axis", phys_statistics_collector::stat_category_engine, m_axis[0].m_sweep_endpoints);
  collector->add_array_stats("Axis", phys_statistics_collector::stat_category_engine, m_axis[1].m_sweep_endpoints);
  collector->add_array_stats("Axis", phys_statistics_collector::stat_category_engine, m_axis[2].m_sweep_endpoints);
  if (m_sweep_marker_count) {
    collector->add_chunk_stats("Markers", phys_statistics_collector::stat_category_engine, m_markers,
                        m_sweep_marker_count * sizeof(phys_axis_sweep::phys_sweep_marker), 0);
  }
  for (int i = 0; i < m_sweep_marker_count; i++) {
    collector->add_array_stats("Markers", phys_statistics_collector::stat_category_runtime, m_markers[i].m_overlapping_objects);
  }
  collector->end_object();
}

phys_broadphase *phys_axis_sweep16_create_broadphase(const phys_vec4 &min, const phys_vec4 &max, int sweep_marker_count)
{
    return new phys_axis_sweep(min, max, sweep_marker_count);
}

// @address 0x682ff0 ??$quick_sort_recursive@Uvalue_int_pair@phys_axis_sweep@@V?$less@Uvalue_int_pair@phys_axis_sweep@@@phys_algorithm@@@phys_algorithm@@YAXPAUvalue_int_pair@phys_axis_sweep@@HHV?$less@Uvalue_int_pair@phys_axis_sweep@@@0@@Z
template void phys_algorithm::quick_sort_recursive<
    phys_axis_sweep::value_int_pair, phys_algorithm::less<phys_axis_sweep::value_int_pair> >(
        phys_axis_sweep::value_int_pair *, int, int, phys_algorithm::less<phys_axis_sweep::value_int_pair>);

// @address 0x67bea8 ?shift_broadphase@phys_axis_sweep@@UAAXABVphys_vec4@@AAV2@AAV?$phys_array@PAVphys_broadphase_handle@@@@@Z
void phys_axis_sweep::shift_broadphase(const phys_vec4 &shift_distance, phys_vec4 &effective_shift_out,
    phys_array<phys_broadphase_handle*> &new_collision_pairs)
{
    phys_vec4 object_shift_vec;
    object_shift_vec.m_simd_quad = __vxor(shift_distance.m_simd_quad, k_phys_axis_sweep_negate_mask);
    phys_vec4 effective_object_shift;
    shift_all_objects(object_shift_vec, effective_object_shift, new_collision_pairs);
    m_offset_low.add4(effective_object_shift);
    phys_vec4 rounding;
    rounding.set(1.0f / m_scale(0), 1.0f / m_scale(1), 1.0f / m_scale(2), 0.0f);
    m_offset_high.set_add4(m_offset_low, rounding);
    effective_shift_out.m_simd_quad = __vxor(effective_object_shift.m_simd_quad, k_phys_axis_sweep_negate_mask);
}

// @address 0x67c878 ?remove_node_batch@phys_sweep_axis@phys_axis_sweep@@QAAXPAVphys_sweep_node@2@HABV?$phys_fixed_array@H@@@Z
void phys_axis_sweep::phys_sweep_axis::remove_node_batch(phys_sweep_node *nodes, int axis, const phys_fixed_array<int> &node_relocations)
{
    const phys_sweep_endpoint *endpoints_end = m_sweep_endpoints.end();
    phys_sweep_endpoint *write_ep = m_sweep_endpoints.begin();
    int write_idx = 0;
    for (const phys_sweep_endpoint *read_ep = m_sweep_endpoints.begin(); read_ep < endpoints_end; read_ep++) {
        int sweep_node_idx = read_ep->m_node_idx;
        int newIndex = node_relocations[sweep_node_idx];
        if (newIndex < 0)
            continue;
        *write_ep = *read_ep;
        write_ep->m_node_idx = (unsigned short)newIndex;
        nodes[newIndex].set_elem(axis, write_ep->is_max_point(), write_idx);
        write_ep++;
        write_idx++;
    }
    m_sweep_endpoints.set_array_size(write_idx);
}

// @address 0x67c480 ?get_aabb_from_node@phys_axis_sweep@@IBAXABVphys_sweep_node@1@AAVphys_aabb@@@Z
void phys_axis_sweep::get_aabb_from_node(const phys_sweep_node &node, phys_aabb &aabb) const
{
    phys_vec4 aabb_mins_vec;
    aabb_mins_vec.set(m_axis[0].m_sweep_endpoints[node.lower_x].m_value * (1.0f / m_scale(0)),
             m_axis[1].m_sweep_endpoints[node.min_y].m_value * (1.0f / m_scale(1)),
             m_axis[2].m_sweep_endpoints[node.lower_z].m_value * (1.0f / m_scale(2)), 0.0f);
    aabb_mins_vec.sub4(m_offset_low);
    phys_vec4 aabb_max_corner;
    aabb_max_corner.set(m_axis[0].m_sweep_endpoints[node.max_x_val].m_value * (1.0f / m_scale(0)),
             m_axis[1].m_sweep_endpoints[node.max_y].m_value * (1.0f / m_scale(1)),
             m_axis[2].m_sweep_endpoints[node.max_z_val].m_value * (1.0f / m_scale(2)), 0.0f);
    aabb_max_corner.sub4(m_offset_low);
    aabb.m_min_limit = aabb_mins_vec;
    aabb.m_max_limit = aabb_max_corner;
}

// @address 0x67df20 ?shift_all_objects@phys_axis_sweep@@UAAXABVphys_vec4@@AAV2@AAV?$phys_array@PAVphys_broadphase_handle@@@@@Z
void phys_axis_sweep::shift_all_objects(const phys_vec4 &shift_distance, phys_vec4 &effective_shift_out,
    phys_array<phys_broadphase_handle*> &new_collision_pairs)
{
    phys_vec4 real_shift_vec = shift_distance;
    int int_shift[3];
    for (int i = 0; i < 3; i++) {
        int_shift[i] = (int)(real_shift_vec(i) * m_scale(i)) & ~1;
        real_shift_vec(i) = int_shift[i] / m_scale(i);
    }
    effective_shift_out = real_shift_vec;
    for (int a = 0; a < 3; a++) {
        phys_sweep_axis &axis = m_axis[a];
        int shift = int_shift[a];
        for (int i = 1; i < axis.m_sweep_endpoints.getSize() - 1; i++) {
            phys_sweep_endpoint &ep = axis.m_sweep_endpoints[i];
            int value = ep.m_value;
            int queue_new_value;
            if (value < 2 || value >= 0xfffc)
                continue;
            queue_new_value = value + shift;
            if (queue_new_value < 0) {
                queue_new_value = value & 1;
                new_collision_pairs.array_push_back(m_nodes[ep.m_node_idx].m_handle);
            } else if (queue_new_value >= 0xfffc) {
                queue_new_value = (value & 1) | 0xfffc;
                new_collision_pairs.array_push_back(m_nodes[ep.m_node_idx].m_handle);
            }
            ep.m_value = (unsigned short)queue_new_value;
        }
    }
}

// @address 0x67bf70 ?merge_endpoint_batch@phys_sweep_axis@phys_axis_sweep@@QAAXPAVphys_sweep_node@2@HHHPAVphys_sweep_endpoint@2@@Z
void phys_axis_sweep::phys_sweep_axis::merge_endpoint_batch(phys_sweep_node *nodes, int new_endpoints_idx, int sweep_new_count,
    int axis, phys_sweep_endpoint *scratch)
{
    phys_sweep_endpoint *old_ep_list = scratch;
    for (int i = 0; i < new_endpoints_idx; i++)
        old_ep_list[i] = m_sweep_endpoints[i];

    phys_sweep_endpoint *merged = &m_sweep_endpoints[1];
    const phys_sweep_endpoint *merge_old_ep = &old_ep_list[1];
    const phys_sweep_endpoint *merge_new_ep = &m_sweep_endpoints[new_endpoints_idx];

    const phys_sweep_endpoint *old_eps_end = &old_ep_list[0] + new_endpoints_idx - 1;
    const phys_sweep_endpoint *new_eps_end = &m_sweep_endpoints[0] + (new_endpoints_idx + sweep_new_count);

    while (merge_old_ep->m_value < merge_new_ep->m_value) {
        merge_old_ep++;
        merged++;
    }

    if (merge_old_ep < old_eps_end && merge_new_ep < new_eps_end) {
        while (1) {
            if (merge_old_ep->m_value < merge_new_ep->m_value) {
                *merged = *merge_old_ep;
                nodes[merged->m_node_idx].set_elem(axis, merged->is_max_point(), merged - &m_sweep_endpoints[0]);
                merge_old_ep++;
                merged++;
                if (merge_old_ep >= old_eps_end)
                    break;
            } else {
                *merged = *merge_new_ep;
                nodes[merged->m_node_idx].set_elem(axis, merged->is_max_point(), merged - &m_sweep_endpoints[0]);
                merge_new_ep++;
                merged++;
                if (merge_new_ep >= new_eps_end)
                    break;
            }
        }
    }

    while (merge_new_ep < new_eps_end) {
        *merged = *merge_new_ep;
        nodes[merged->m_node_idx].set_elem(axis, merged->is_max_point(), merged - &m_sweep_endpoints[0]);
        merge_new_ep++;
        merged++;
    }

    while (merge_old_ep <= old_eps_end) {
        *merged = *merge_old_ep;
        nodes[merged->m_node_idx].set_elem(axis, merged->is_max_point(), merged - &m_sweep_endpoints[0]);
        merge_old_ep++;
        merged++;
    }
}

// @address 0x681370 ?compute_bounds_cache_internal@phys_axis_sweep@@IBAXABV?$phys_array@PBVphys_sweep_node@phys_axis_sweep@@@@PAD@Z
void phys_axis_sweep::compute_bounds_cache_internal(const phys_array<const phys_sweep_node *> &overlap_nodes, char *bounds_cache_out) const
{
    phys_sweep_axis *axis = reinterpret_cast<phys_sweep_axis *>(bounds_cache_out);
    {
        phys_sweep_endpoint *ep = reinterpret_cast<phys_sweep_endpoint *>(axis + 3);
        int size = overlap_nodes.getSize() * 2 + 2;
        new (axis + 0) phys_sweep_axis(ep + 0 * size, size);
        new (axis + 1) phys_sweep_axis(ep + 1 * size, size);
        new (axis + 2) phys_sweep_axis(ep + 2 * size, size);
    }

    phys_sweep_endpoint ep;
    ep.m_value = 0;
    ep.m_node_idx = 0;

    axis[0].m_sweep_endpoints.push_back_unchecked(ep);
    axis[1].m_sweep_endpoints.push_back_unchecked(ep);
    axis[2].m_sweep_endpoints.push_back_unchecked(ep);

    phys_sweep_node **p = const_cast<phys_sweep_node **>(overlap_nodes.begin());

    for (int i = overlap_nodes.getSize() - 1; i >= 0; i--) {
        axis[0].m_sweep_endpoints.push_back_unchecked(m_axis[0].m_sweep_endpoints[(*p)->get_min_endpoint(0)]);
        axis[0].m_sweep_endpoints.push_back_unchecked(m_axis[0].m_sweep_endpoints[(*p)->get_max_endpoint(0)]);
        axis[1].m_sweep_endpoints.push_back_unchecked(m_axis[1].m_sweep_endpoints[(*p)->get_min_endpoint(1)]);
        axis[1].m_sweep_endpoints.push_back_unchecked(m_axis[1].m_sweep_endpoints[(*p)->get_max_endpoint(1)]);
        axis[2].m_sweep_endpoints.push_back_unchecked(m_axis[2].m_sweep_endpoints[(*p)->get_min_endpoint(2)]);
        axis[2].m_sweep_endpoints.push_back_unchecked(m_axis[2].m_sweep_endpoints[(*p)->get_max_endpoint(2)]);
        p++;
    }

    for (int i = 0; i < 3; i++) {
        int alloc_count = axis[i].m_sweep_endpoints.getSize() - 1;
        phys_algorithm::quick_sort(reinterpret_cast<phys_axis_sweep::value_int_pair *>(axis[i].m_sweep_endpoints.begin() + 1), alloc_count);
    }

    ep.m_value = 0xfffc;
    ep.m_node_idx = 0;
    axis[0].m_sweep_endpoints.push_back_unchecked(ep);
    axis[1].m_sweep_endpoints.push_back_unchecked(ep);
    axis[2].m_sweep_endpoints.push_back_unchecked(ep);
}

// @address 0x682e70 ?compute_bounds_cache@phys_axis_sweep@@UBAXABV?$phys_array@PAVphys_collider@@@@PAD@Z
void phys_axis_sweep::compute_bounds_cache(const phys_array<phys_collider *> &colliders, char *bounds_cache_out) const
{
    phys_axis_sweep_overlaps overlap_nodes(colliders.getSize());
    for (int i = 0; i < colliders.getSize(); i++) {
        overlap_nodes.push_back_unchecked(&m_nodes[colliders[i]->m_broadphase_handle.m_handle_id]);
    }
    compute_bounds_cache_internal(overlap_nodes, bounds_cache_out);
}


// @address 0x67c790 ?end_overlap_check_marker@phys_axis_sweep@@SIXPAVphys_sweep_marker@1@AAVphys_sweep_node@1@H1AAV?$phys_array@Vphys_broadphase_handle_pair@@@@@Z
void __fastcall phys_axis_sweep::end_overlap_check_marker(
    phys_sweep_marker *markers, phys_sweep_node &sweep_node_a, int sweep_node_idx, phys_sweep_node &sweep_node_b,
        phys_array<phys_broadphase_handle_pair> &pairs)
{
    if (!sweep_node_b.is_marker()) {
        phys_broadphase_handle_pair &pair = pairs.expand_one();
        pair.m_a = sweep_node_a.m_handle;
        pair.m_b = sweep_node_b.m_handle;
    } else {
        phys_sweep_marker *marker = sweep_node_b.get_marker(markers);
        int index = (&marker->m_overlapping_objects)->array_index_of((unsigned short)sweep_node_idx);
        (&marker->m_overlapping_objects)->remove_at(index);
    }
}

// @address 0x681ba0 ??1phys_axis_sweep@@UAA@XZ
phys_axis_sweep::~phys_axis_sweep()
{
    for (int i = 0; i < m_sweep_marker_count; i++) {
        m_markers[i].~phys_sweep_marker();
    }
    phys_free_memory<phys_sweep_marker>(m_markers);
}
// @address 0xf40 ?phys_axis_sweep_min_val@?1??convert_vector_to_int@phys_axis_sweep@@QBAXABVphys_vec4@@PAI@Z@4QBMB
// @address 0xf40 ?phys_axis_sweep_min_val@?1??convert_aabb_to_int@phys_axis_sweep@@QBAXABVphys_aabb@@AAUphys_sweep_aabb@2@@Z@4QBMB
// @address 0x176f0 k_phys_axis_sweep_conversion_bias
// @rdata 0x820176f0..0x82017700
const __vector4 k_phys_axis_sweep_conversion_bias = { 65536.0f, 65536.0f, 65536.0f, 65536.0f };
// @rdata-end
// @address 0x17710 k_phys_axis_sweep_select_masks
// @address 0x17740 k_phys_axis_sweep_endpoint_mask
// @address 0x17750 k_phys_axis_sweep_component_indices
// @rdata 0x82017710..0x82017760
__declspec(align(16)) const unsigned int k_phys_axis_sweep_select_masks[3][4] = {
    { 0xffffffff, 0, 0, 0 },
    { 0, 0xffffffff, 0, 0 },
    { 0, 0, 0xffffffff, 0 },
};
__declspec(align(16)) const unsigned int k_phys_axis_sweep_endpoint_mask[4] = { 0x0000ffff, 0, 0, 0 };
__declspec(align(16)) const unsigned int k_phys_axis_sweep_component_indices[4] = { 0, 1, 2, 0 };
// @rdata-end
__forceinline void phys_axis_sweep::query_single_aabb_add(phys_broadphase_handle *object, int new_node_index, const unsigned int *bit_field,
    phys_sweep_node &ref_node, phys_array<phys_broadphase_handle_pair> &node_pairs_out) const
{
    phys_broadphase_handle_pair pair;
    pair.m_a = object;
    const phys_sweep_node *node = &m_nodes[0];
    const unsigned int *end = bit_field + (m_nodes.getSize() >> 5) + 1;
    for (; bit_field < end; node += 32, bit_field++) {
        const phys_sweep_node *n = node;
        unsigned int mask = *bit_field;
        while (mask != 0) {
            if ((unsigned char)mask == 0) {
                n += 8;
                mask >>= 8;
            } else {
                if ((mask & 1) && !n->yz_disjoint(ref_node)) {
                    if (!n->is_marker()) {
                        pair.m_b = n->m_handle;
                        node_pairs_out.array_push_back(pair);
                    } else {
                        n->get_marker(m_markers)->m_overlapping_objects.array_push_back((unsigned short)new_node_index);
                    }
                }
                n++;
                mask >>= 1;
            }
        }
    }
}
static __forceinline void phys_axis_sweep_to_u16_clipped(
    const phys_vec4 &in, const phys_vec4 &offset, const phys_vec4 &scale, const phys_vec4 &min, const phys_vec4 &max, phys_int_union64 &out)
{
    phys_vec4 x;
    x.set_add4(in, offset);
    x.m_simd_quad = __vmulfp(x.m_simd_quad, scale.m_simd_quad);
    x.m_simd_quad = __vminfp(x.m_simd_quad, max.m_simd_quad);
    x.m_simd_quad = __vmaxfp(x.m_simd_quad, min.m_simd_quad);
    x.m_simd_quad = __vaddfp(x.m_simd_quad, k_phys_axis_sweep_conversion_bias);
    union
    {
        __vector4 q;
        unsigned int i[4];
    } u;
    u.q = x.m_simd_quad;
    out.i16_lanes[0] = (short)(u.i[0] >> 7);
    out.i16_lanes[1] = (short)(u.i[1] >> 7);
    out.i16_lanes[2] = (short)(u.i[2] >> 7);
    out.i16_lanes[3] = (short)(u.i[3] >> 7);
}
__forceinline void phys_axis_sweep::convert_aabb_to_int(const phys_aabb &aabb, phys_sweep_aabb &aabb_out) const
{
    __declspec(align(16)) static const float phys_axis_sweep_min_val[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
    __declspec(align(16)) phys_int_union64 mi;
    __declspec(align(16)) phys_int_union64 ma;
    phys_axis_sweep_to_u16_clipped(aabb.m_min_limit, m_offset_low, m_scale, 
        (const phys_vec4 &)phys_axis_sweep_min_val,
        (const phys_vec4 &)s_axis_max_value, mi);
    phys_axis_sweep_to_u16_clipped(aabb.m_max_limit, m_offset_high, m_scale, 
        (const phys_vec4 &)phys_axis_sweep_min_val,
        (const phys_vec4 &)s_axis_max_value, ma);
    aabb_out.m_min_limit[0] = mi.as_u16[0] & 0xfffe;
    aabb_out.m_min_limit[1] = mi.as_u16[1] & 0xfffe;
    aabb_out.m_min_limit[2] = mi.as_u16[2] & 0xfffe;
    aabb_out.m_max_limit[0] = ma.as_u16[0] | 1;
    aabb_out.m_max_limit[1] = ma.as_u16[1] | 1;
    aabb_out.m_max_limit[2] = ma.as_u16[2] | 1;
}
// @address 0x67ccb8 ?add_broadphase_object@phys_axis_sweep@@UAAXPAVphys_broadphase_handle@@ABVphys_aabb@@AAV?$phys_array@Vphys_broadphase_handle_pair@@@@@Z
void phys_axis_sweep::add_broadphase_object(class phys_broadphase_handle *object, class phys_aabb const &aabb_in,
    class phys_array<class phys_broadphase_handle_pair> &new_pairs)
{
    phys_axis_sweep::phys_sweep_aabb aabb;
    convert_aabb_to_int(aabb_in, aabb);

    int sweep_node_idx = m_nodes.getSize();
    phys_sweep_node &node = m_nodes.expand_one();
    phys_sweep_node *nodes = &m_nodes[0];

    m_axis[0].insert(nodes, sweep_node_idx, (unsigned short)aabb.m_min_limit[0], (unsigned short)aabb.m_max_limit[0], node.lower_x, node.max_x_val);
    m_axis[1].insert(nodes, sweep_node_idx, (unsigned short)aabb.m_min_limit[1], (unsigned short)aabb.m_max_limit[1], node.min_y, node.max_y);
    m_axis[2].insert(nodes, sweep_node_idx, (unsigned short)aabb.m_min_limit[2], (unsigned short)aabb.m_max_limit[2], node.lower_z, node.max_z_val);
    fix_nodes_after_insert(nodes, sweep_node_idx, node);

    node.m_handle = object;
    object->m_handle_id = sweep_node_idx;
    {
        int sweep_node_count = m_nodes.getSize();
        unsigned int *allocated_bits = phys_alloc_stack<unsigned int>((sweep_node_count >> 5) + 8);
        unsigned int *bit_field = allocated_bits;
        set_bits_from_x_interval(sweep_node_count, aabb.m_min_limit[0], node, (unsigned short)sweep_node_idx, bit_field);
        query_single_aabb_add(object, sweep_node_idx, bit_field, node, new_pairs);
        phys_free_stack(allocated_bits);
    }
}

__forceinline void phys_axis_sweep::query_single_aabb_remove(phys_broadphase_handle *object, int new_node_index, const unsigned int *bit_field,
    phys_sweep_node &ref_node, phys_array<phys_broadphase_handle_pair> &node_pairs_out) const
{
    phys_broadphase_handle_pair pair;
    pair.m_a = object;
    const phys_sweep_node *node = &m_nodes[0];
    const unsigned int *end = bit_field + (m_nodes.getSize() >> 5) + 1;
    for (; bit_field < end; node += 32, bit_field++) {
        const phys_sweep_node *n = node;
        unsigned int mask = *bit_field;
        while (mask != 0) {
            if ((unsigned char)mask == 0) {
                n += 8;
                mask >>= 8;
            } else {
                if ((mask & 1) && !n->yz_disjoint(ref_node)) {
                    if (!n->is_marker()) {
                        pair.m_b = n->m_handle;
                        node_pairs_out.array_push_back(pair);
                    } else {
                        phys_sweep_marker *m = n->get_marker(m_markers);
                        int i = m->m_overlapping_objects.array_index_of((unsigned short)new_node_index);
                        m->m_overlapping_objects.remove_at(i);
                    }
                }
                n++;
                mask >>= 1;
            }
        }
    }
}

template <int axis_idx, int marker>
static __forceinline void phys_axis_sweep_update_axis(phys_axis_sweep *sweep, phys_axis_sweep::phys_sweep_node *nodes, phys_axis_sweep::phys_sweep_node &node,
    unsigned int sweep_node_idx, unsigned int new_min_bound, unsigned int new_max_bound, phys_array<phys_broadphase_handle_pair> &new_pairs_out,
    phys_array<phys_broadphase_handle_pair> &deleted_pairs_out)
{
    phys_axis_sweep::phys_sweep_axis &axis = sweep->m_axis[axis_idx];
    {
        unsigned int min_ep_idx = node.get_min_endpoint(axis_idx);
        phys_axis_sweep::phys_sweep_endpoint *ep = &axis.m_sweep_endpoints[min_ep_idx];
        unsigned int other_bound;
        while (new_min_bound < (other_bound = ep[-1].m_value)) {
            unsigned int other_node_idx = ep[-1].m_node_idx;
            phys_axis_sweep::phys_sweep_node &other_node_ref = nodes[other_node_idx];
            ep[0].m_value = (unsigned short)other_bound;
            ep[0].m_node_idx = (unsigned short)other_node_idx;
            if (phys_axis_sweep::phys_sweep_endpoint::is_max_point(other_bound)) {
                int is_disjoint;
                if (axis_idx == 0) { is_disjoint = node.yz_disjoint(other_node_ref); other_node_ref.max_x_val = (unsigned short)min_ep_idx; }
                else if (axis_idx == 1) { is_disjoint = node.xz_disjoint(other_node_ref); other_node_ref.max_y = (unsigned short)min_ep_idx; }
                else { is_disjoint = node.xy_disjoint(other_node_ref); other_node_ref.max_z_val = (unsigned short)min_ep_idx; }
                if (!is_disjoint) {
                    if (marker == phys_axis_sweep::PHYS_SWEEP_NO_MARKER)
                        phys_axis_sweep::begin_overlap(node, other_node_ref, new_pairs_out);
                    else
                        phys_axis_sweep::begin_overlap_check_marker(sweep->m_markers, node, sweep_node_idx, other_node_ref, new_pairs_out);
                }
            } else {
                other_node_ref.get_min_endpoint(axis_idx) = (unsigned short)min_ep_idx;
            }
            ep--;
            min_ep_idx--;
            ep->m_node_idx = (unsigned short)sweep_node_idx;
        }
        node.get_min_endpoint(axis_idx) = (unsigned short)min_ep_idx;
        ep->m_value = (unsigned short)new_min_bound;
    }
    {
        phys_axis_sweep::phys_sweep_endpoint *max_ep = &axis.m_sweep_endpoints[node.get_max_endpoint(axis_idx)];
        int i = 0;
        unsigned int other_bound;
        while (new_max_bound > (other_bound = max_ep[1].m_value)) {
            max_ep++;
            i++;
            phys_axis_sweep::phys_sweep_node &other_node_ref = nodes[max_ep->m_node_idx];
            max_ep[-1] = max_ep[0];
            max_ep->m_node_idx = (unsigned short)sweep_node_idx;
            if (!phys_axis_sweep::phys_sweep_endpoint::is_max_point(other_bound)) {
                int is_disjoint;
                if (axis_idx == 0) { is_disjoint = node.yz_disjoint(other_node_ref); other_node_ref.lower_x--; }
                else if (axis_idx == 1) { is_disjoint = node.xz_disjoint(other_node_ref); other_node_ref.min_y--; }
                else { is_disjoint = node.xy_disjoint(other_node_ref); other_node_ref.lower_z--; }
                if (!is_disjoint) {
                    if (marker == phys_axis_sweep::PHYS_SWEEP_NO_MARKER)
                        phys_axis_sweep::begin_overlap(node, other_node_ref, new_pairs_out);
                    else
                        phys_axis_sweep::begin_overlap_check_marker(sweep->m_markers, node, sweep_node_idx, other_node_ref, new_pairs_out);
                }
            } else {
                other_node_ref.get_max_endpoint(axis_idx)--;
            }
        }
        node.get_max_endpoint(axis_idx) += i;
    }
    {
        phys_axis_sweep::phys_sweep_endpoint *max_ep = &axis.m_sweep_endpoints[node.get_max_endpoint(axis_idx)];
        int i = 0;
        unsigned int other_bound;
        while (new_max_bound < (other_bound = max_ep[-1].m_value)) {
            max_ep--;
            i--;
            phys_axis_sweep::phys_sweep_node &other_node_ref = nodes[max_ep->m_node_idx];
            max_ep[1] = max_ep[0];
            max_ep->m_node_idx = (unsigned short)sweep_node_idx;
            if (!phys_axis_sweep::phys_sweep_endpoint::is_max_point(other_bound)) {
                int is_disjoint;
                if (axis_idx == 0) { is_disjoint = node.yz_disjoint(other_node_ref); other_node_ref.lower_x++; }
                else if (axis_idx == 1) { is_disjoint = node.xz_disjoint(other_node_ref); other_node_ref.min_y++; }
                else { is_disjoint = node.xy_disjoint(other_node_ref); other_node_ref.lower_z++; }
                if (!is_disjoint) {
                    if (marker == phys_axis_sweep::PHYS_SWEEP_NO_MARKER)
                        phys_axis_sweep::end_overlap(node, other_node_ref, deleted_pairs_out);
                    else
                        phys_axis_sweep::end_overlap_check_marker(sweep->m_markers, node, sweep_node_idx, other_node_ref, deleted_pairs_out);
                }
            } else {
                other_node_ref.get_max_endpoint(axis_idx)++;
            }
        }
        node.get_max_endpoint(axis_idx) += i;
        max_ep->m_value = (unsigned short)new_max_bound;
    }
    {
        phys_axis_sweep::phys_sweep_endpoint *ep = &axis.m_sweep_endpoints[node.get_min_endpoint(axis_idx)];
        int i = 0;
        unsigned int other_bound;
        while (new_min_bound > (other_bound = ep[1].m_value)) {
            ep++;
            i++;
            phys_axis_sweep::phys_sweep_node &other_node_ref = nodes[ep->m_node_idx];
            ep[-1] = ep[0];
            ep->m_node_idx = (unsigned short)sweep_node_idx;
            if (phys_axis_sweep::phys_sweep_endpoint::is_max_point(other_bound)) {
                int is_disjoint;
                if (axis_idx == 0) { is_disjoint = node.yz_disjoint(other_node_ref); other_node_ref.max_x_val--; }
                else if (axis_idx == 1) { is_disjoint = node.xz_disjoint(other_node_ref); other_node_ref.max_y--; }
                else { is_disjoint = node.xy_disjoint(other_node_ref); other_node_ref.max_z_val--; }
                if (!is_disjoint) {
                    if (marker == phys_axis_sweep::PHYS_SWEEP_NO_MARKER)
                        phys_axis_sweep::end_overlap(node, other_node_ref, deleted_pairs_out);
                    else
                        phys_axis_sweep::end_overlap_check_marker(sweep->m_markers, node, sweep_node_idx, other_node_ref, deleted_pairs_out);
                }
            } else {
                other_node_ref.get_min_endpoint(axis_idx)--;
            }
        }
        node.get_min_endpoint(axis_idx) += i;
        ep->m_value = (unsigned short)new_min_bound;
    }
}

static __forceinline void phys_axis_sweep_update_aabb(phys_axis_sweep *sweep, phys_broadphase_handle *object, const phys_axis_sweep::phys_sweep_aabb &aabb,
    phys_array<phys_broadphase_handle_pair> &new_pairs, phys_array<phys_broadphase_handle_pair> &sweep_removed_pairs)
{
    phys_axis_sweep::phys_sweep_node *nodes = &sweep->m_nodes[0];
    unsigned int sweep_node_idx = object->m_handle_id;
    phys_axis_sweep::phys_sweep_node &node = nodes[sweep_node_idx];
    phys_axis_sweep_update_axis<0, phys_axis_sweep::PHYS_SWEEP_USE_MARKER>(sweep, nodes, node, sweep_node_idx, aabb.m_min_limit[0],
        aabb.m_max_limit[0], new_pairs, sweep_removed_pairs);
    phys_axis_sweep_update_axis<1, phys_axis_sweep::PHYS_SWEEP_NO_MARKER>(sweep, nodes, node, sweep_node_idx, aabb.m_min_limit[1],
        aabb.m_max_limit[1], new_pairs, sweep_removed_pairs);
    phys_axis_sweep_update_axis<2, phys_axis_sweep::PHYS_SWEEP_NO_MARKER>(sweep, nodes, node, sweep_node_idx, aabb.m_min_limit[2],
        aabb.m_max_limit[2], new_pairs, sweep_removed_pairs);
}

// @address 0x67e120 ?update_aabbs@phys_axis_sweep@@UAAXQAPAVphys_broadphase_handle@@PBVphys_aabb@@HAAV?$phys_array@Vphys_broadphase_handle_pair@@@@2@Z
void phys_axis_sweep::update_aabbs(phys_broadphase_handle *objects[], const phys_aabb *aabb_list, int sweep_object_count,
    phys_array<phys_broadphase_handle_pair> &sweep_added_pairs, phys_array<phys_broadphase_handle_pair> &erased_sweep_pairs)
{
    int alloc_count = sweep_object_count;
    const phys_aabb *cur_aabb = aabb_list;
    phys_broadphase_handle **objects_end = objects + alloc_count;
    for (phys_broadphase_handle **o = &objects[0]; o < objects_end; o++) {
        phys_axis_sweep::phys_sweep_aabb aabb;
        convert_aabb_to_int(*cur_aabb, aabb);
        phys_axis_sweep_update_aabb(this, *o, aabb, sweep_added_pairs, erased_sweep_pairs);
        cur_aabb++;
    }
}

// @address 0x67d098 ?remove_broadphase_object@phys_axis_sweep@@UAAXPAVphys_broadphase_handle@@AAV?$phys_array@Vphys_broadphase_handle_pair@@@@@Z
void phys_axis_sweep::remove_broadphase_object(phys_broadphase_handle *object, phys_array<phys_broadphase_handle_pair> &removed_pairs_out)
{
    unsigned int sweep_node_count = m_nodes.getSize();
    const unsigned int sweep_node_idx = object->m_handle_id;
    phys_sweep_node &node = m_nodes[sweep_node_idx];
    {
        unsigned int *bit_field = phys_alloc_stack<unsigned int>((sweep_node_count >> 5) + 8);
        set_bits_from_x_interval(sweep_node_count, m_axis[0].m_sweep_endpoints[node.lower_x].m_value, node, (unsigned short)sweep_node_idx, bit_field);
        query_single_aabb_remove(object, sweep_node_idx, bit_field, node, removed_pairs_out);
        phys_free_stack(bit_field);
    }

    phys_sweep_node *nodes = &m_nodes[0];
    m_axis[0].remove(node.lower_x, node.max_x_val);
    m_axis[1].remove(node.min_y, node.max_y);
    m_axis[2].remove(node.lower_z, node.max_z_val);
    fix_nodes_after_delete(nodes, sweep_node_count, node);

    if (sweep_node_idx < sweep_node_count - 1) {
        node = m_nodes[sweep_node_count - 1];
        m_axis[0].m_sweep_endpoints[node.lower_x].m_node_idx = (unsigned short)sweep_node_idx;
        m_axis[0].m_sweep_endpoints[node.max_x_val].m_node_idx = (unsigned short)sweep_node_idx;
        if (!node.is_marker()) {
            m_axis[1].m_sweep_endpoints[node.min_y].m_node_idx = (unsigned short)sweep_node_idx;
            m_axis[1].m_sweep_endpoints[node.max_y].m_node_idx = (unsigned short)sweep_node_idx;
            m_axis[2].m_sweep_endpoints[node.lower_z].m_node_idx = (unsigned short)sweep_node_idx;
            m_axis[2].m_sweep_endpoints[node.max_z_val].m_node_idx = (unsigned short)sweep_node_idx;
            node.m_handle->m_handle_id = sweep_node_idx;
        } else {
            node.get_marker(m_markers)->m_node_idx = (unsigned short)sweep_node_idx;
        }

        if (get_marker_count() && !node.is_marker()) {
            int min_marker_pos = m_axis[0].m_sweep_endpoints[node.lower_x].m_value >> (16 - m_log2_marker_count);
            if (min_marker_pos > 0) {
                phys_sweep_node &marker_node = m_nodes[m_markers[min_marker_pos - 1].m_node_idx];
                if (marker_node.max_x_val > node.lower_x)
                    min_marker_pos--;
            }
            int max_marker_pos = (m_axis[0].m_sweep_endpoints[node.max_x_val].m_value >> (16 - m_log2_marker_count)) - 1;
            for (int m = min_marker_pos; m <= max_marker_pos; m++) {
                phys_sweep_marker &marker = m_markers[m];
                int i = marker.m_overlapping_objects.array_index_of((unsigned short)(sweep_node_count - 1));
                marker.m_overlapping_objects[i] = (unsigned short)sweep_node_idx;
            }
        }
    }
    m_nodes.set_array_size(sweep_node_count - 1);
}

__forceinline void phys_axis_sweep::query_single_aabb_sub(phys_broadphase_handle *object, const unsigned int *bit_field, phys_sweep_node &ref_node,
    phys_sweep_query_kind type, phys_array<phys_broadphase_handle_pair> *node_pairs_out, phys_array<const phys_sweep_node *> *nodes_out) const
{
    phys_broadphase_handle_pair pair;
    pair.m_a = object;
    const phys_sweep_node *node = &m_nodes[0];
    const unsigned int *end = bit_field + (m_nodes.getSize() >> 5) + 1;
    for (; bit_field < end; node += 32, bit_field++) {
        unsigned int mask = bit_field[0];
        if (mask == 0)
            continue;
        const phys_sweep_node *n = node;
        do {
            if (mask & 0xf) {
                if ((mask & 1) && !n[0].yz_disjoint(ref_node) && !n[0].is_marker()) {
                    if (type == PHYS_SWEEP_REPORT_HANDLES) {
                        pair.m_b = n[0].m_handle;
                        node_pairs_out->array_push_back(pair);
                    } else {
                        nodes_out->push_back_unchecked(n + 0);
                    }
                }
                if ((mask & 2) && !n[1].yz_disjoint(ref_node) && !n[1].is_marker()) {
                    if (type == PHYS_SWEEP_REPORT_HANDLES) {
                        pair.m_b = n[1].m_handle;
                        node_pairs_out->array_push_back(pair);
                    } else {
                        nodes_out->push_back_unchecked(n + 1);
                    }
                }
                if ((mask & 4) && !n[2].yz_disjoint(ref_node) && !n[2].is_marker()) {
                    if (type == PHYS_SWEEP_REPORT_HANDLES) {
                        pair.m_b = n[2].m_handle;
                        node_pairs_out->array_push_back(pair);
                    } else {
                        nodes_out->push_back_unchecked(n + 2);
                    }
                }
                if ((mask & 8) && !n[3].yz_disjoint(ref_node) && !n[3].is_marker()) {
                    if (type == PHYS_SWEEP_REPORT_HANDLES) {
                        pair.m_b = n[3].m_handle;
                        node_pairs_out->array_push_back(pair);
                    } else {
                        nodes_out->push_back_unchecked(n + 3);
                    }
                }
            }
            n += 4;
            mask = mask >> 4;
        } while (mask);
    }
}

__forceinline void phys_axis_sweep::query_single_bounds(
    const phys_aabb &aabb_in, phys_sweep_query_kind type, phys_array<phys_broadphase_handle_pair> *node_pairs_out,
        phys_array<const phys_sweep_node *> *nodes_out) const
{
    phys_broadphase_handle *object = 0;
    int sweep_node_count = m_nodes.getSize();
    int bitfield_size = (sweep_node_count >> 5) + 8;
    unsigned int *bit_field = phys_alloc_stack<unsigned int>(bitfield_size);
    clear_bitfield(bit_field, sweep_node_count >> 3);

    phys_axis_sweep::phys_sweep_aabb aabb;
    convert_aabb_to_int(aabb_in, aabb);

    {
        const phys_sweep_endpoint *ep = &m_axis[0].m_sweep_endpoints[1];
        if (get_marker_count()) {
            int min_x_quant = aabb.m_min_limit[0];
            int pos = min_x_quant >> (16 - m_log2_marker_count);
            if (pos > 0) {
                phys_sweep_marker *m = &m_markers[pos - 1];
                {
                    static_flip_bit(bit_field, m->m_node_idx);
                    unsigned short *o = m->m_overlapping_objects.begin();
                    for (int k = m->m_overlapping_objects.getSize() - 1; k >= 0; k--) {
                        static_flip_bit(bit_field, *o);
                        o++;
                    }
                    const phys_sweep_node &marker_node = m_nodes[m->m_node_idx];
                    ep = &m_axis[0].m_sweep_endpoints[marker_node.lower_x + 1];
                    {
                        const phys_sweep_endpoint *end = &m_axis[0].m_sweep_endpoints[marker_node.max_x_val];
                        while (ep < end) {
                            if (!ep->is_max_point())
                                static_clear_bit(bit_field, ep->m_node_idx);
                            ep++;
                        }
                    }
                    ep = &m_axis[0].m_sweep_endpoints[marker_node.lower_x + 1];
                }
            }
        }
        while (ep->m_value < aabb.m_min_limit[0]) {
            static_flip_bit(bit_field, ep->m_node_idx);
            ep++;
        }
        while (ep->m_value < aabb.m_max_limit[0]) {
            if (!ep->is_max_point())
                static_flip_bit(bit_field, ep->m_node_idx);
            ep++;
        }
    }

    phys_sweep_node ref_node;
    {
        const phys_sweep_axis &axis = m_axis[1];
        const phys_sweep_endpoint *sp = &axis.m_sweep_endpoints[0];
        ref_node.min_y = (unsigned short)(axis.find(sp + 1, axis.m_sweep_endpoints.end() - 2, (unsigned short)aabb.m_min_limit[1]) - sp);
        ref_node.max_y = (unsigned short)(axis.find(sp + 1, axis.m_sweep_endpoints.end() - 2, (unsigned short)aabb.m_max_limit[1]) - (sp + 1));
    }
    {
        const phys_sweep_axis &axis = m_axis[2];
        const phys_sweep_endpoint *sp = &axis.m_sweep_endpoints[0];
        ref_node.lower_z = (unsigned short)(axis.find(sp + 1, axis.m_sweep_endpoints.end() - 2, (unsigned short)aabb.m_min_limit[2]) - sp);
        ref_node.max_z_val = (unsigned short)(axis.find(sp + 1, axis.m_sweep_endpoints.end() - 2, (unsigned short)aabb.m_max_limit[2]) - (sp + 1));
    }

    query_single_aabb_sub(object, bit_field, ref_node, type, node_pairs_out, nodes_out);
    phys_free_stack(bit_field);
}

// @address 0x67d668 ?query_single_aabb@phys_axis_sweep@@UBAXABVphys_aabb@@AAV?$phys_array@Vphys_broadphase_handle_pair@@@@@Z
void phys_axis_sweep::query_single_aabb(const phys_aabb &aabb_in, phys_array<phys_broadphase_handle_pair> &node_pairs_out) const
{
    query_single_bounds(aabb_in, PHYS_SWEEP_REPORT_HANDLES, &node_pairs_out, 0);
}

// @address 0x682780 ?compute_bounds_cache@phys_axis_sweep@@UBAXABVphys_aabb@@PAD@Z
void phys_axis_sweep::compute_bounds_cache(const phys_aabb &aabb, char *bounds_cache_out) const
{
    phys_axis_sweep_overlaps overlap_nodes(m_nodes.getSize());
    query_single_bounds(aabb, PHYS_SWEEP_REPORT_NODES, 0, &overlap_nodes);
    compute_bounds_cache_internal(overlap_nodes, bounds_cache_out);
}

// @address 0x67bda8 ?get_extents@phys_axis_sweep@@UBAXAAVphys_vec4@@0@Z
void phys_axis_sweep::get_extents(class phys_vec4 &p0, class phys_vec4 &p1) const
{
    p0.m_simd_quad = __vxor(m_offset_low.m_simd_quad, k_phys_axis_sweep_negate_mask);
    p1 = m_scale;
    p1(3) = 1.0f;
    p1.set_reciprocal4(p1);
    p1.mul4(phys_simd_scalar(65532.0f));
    p1.add4(p0);
}

// @address 0x67be30 ?check_sweep_consistency@phys_axis_sweep@@QAAXXZ
void phys_axis_sweep::check_sweep_consistency()
{
}


// @address 0x6816b0 ?defragment_sweep@phys_axis_sweep@@UAAXXZ
void phys_axis_sweep::defragment_sweep()
{
    int size = m_nodes.getSize();
    phys_local_buffer<phys_sweep_node> old_node_buf(size);
    phys_local_buffer<value_int_pair> new_to_old_map(size);
    phys_local_buffer<int> old_to_new_map(size);

    {
        for (int i = 0; i < m_nodes.getSize(); i++) {
            old_node_buf[i] = m_nodes[i];
            new_to_old_map[i].m_value = m_axis[0].m_sweep_endpoints[m_nodes[i].lower_x].m_value;
            new_to_old_map[i].m_old_idx = (unsigned short)i;
        }
    }

    phys_algorithm::quick_sort(&new_to_old_map[1], size - 1);

    {
        for (int i = 0; i < m_nodes.getSize(); i++) {
            old_to_new_map[new_to_old_map[i].m_old_idx] = i;
            m_nodes[i] = old_node_buf[new_to_old_map[i].m_old_idx];
        }
    }

    {
        for (int i = 1; i < m_nodes.getSize(); i++) {
            phys_sweep_node &node = m_nodes[i];
            if (!node.is_marker())
                m_nodes[i].m_handle->m_handle_id = i;
            else
                node.get_marker(m_markers)->m_node_idx = (unsigned short)i;
        }
    }

    {
        for (int i = 0; i < get_marker_count(); i++) {
            phys_sweep_marker &marker = m_markers[i];
            for (int j = marker.m_overlapping_objects.getSize() - 1; j >= 0; j--)
                marker.m_overlapping_objects[j] = (unsigned short)old_to_new_map[marker.m_overlapping_objects[j]];
        }
    }

    {
        for (int i = 0; i < 3; i++) {
            phys_sweep_axis &axis = m_axis[i];
            phys_sweep_endpoint *ep = axis.m_sweep_endpoints.begin();
            for (int j = 0; j < axis.m_sweep_endpoints.getSize(); j++)
                ep[j].m_node_idx = (unsigned short)old_to_new_map[ep[j].m_node_idx];
        }
    }
}

static __forceinline void phys_axis_sweep_quick_sort_endpoints(phys_axis_sweep::phys_sweep_endpoint *sort_items, int n)
{
    phys_algorithm::quick_sort(reinterpret_cast<phys_axis_sweep::value_int_pair *>(sort_items), n);
}

// @address 0x681cb0 ?add_object_batch@phys_axis_sweep@@UAAXAAV?$phys_array@PAVphys_broadphase_handle@@@@AAV?$phys_array@Vphys_aabb@@@@AAV?$phys_array@Vphys_broadphase_handle_pair@@@@@Z
void phys_axis_sweep::add_object_batch(phys_array<phys_broadphase_handle*> &add_object_list, phys_array<phys_aabb> &add_aabb_list,
    phys_array<phys_broadphase_handle_pair> &new_pairs)
{
    if (add_aabb_list.getSize() < 1)
        return;

    int old_node_cnt = m_nodes.getSize();
    int new_node_cnt = add_object_list.getSize();
    m_nodes.expand_by(new_node_cnt);

    int old_endpoint_cnts[3];
    old_endpoint_cnts[0] = m_axis[0].m_sweep_endpoints.getSize();
    old_endpoint_cnts[1] = m_axis[1].m_sweep_endpoints.getSize();
    old_endpoint_cnts[2] = m_axis[2].m_sweep_endpoints.getSize();

    phys_sweep_endpoint *new_eps_by_axis[3];
    new_eps_by_axis[0] = m_axis[0].m_sweep_endpoints.expand_by(new_node_cnt * 2);
    new_eps_by_axis[1] = m_axis[1].m_sweep_endpoints.expand_by(new_node_cnt * 2);
    new_eps_by_axis[2] = m_axis[2].m_sweep_endpoints.expand_by(new_node_cnt * 2);

    for (int i = 0; i < new_node_cnt; i++) {
        phys_axis_sweep::phys_sweep_aabb aabb;
        convert_aabb_to_int(add_aabb_list[i], aabb);
        int sweep_node_idx = old_node_cnt + i;
        phys_sweep_node &node = m_nodes[sweep_node_idx];
        phys_broadphase_handle *object = add_object_list[i];
        node.m_handle = object;
        object->m_handle_id = sweep_node_idx;

        new_eps_by_axis[0][i + i].m_value = (unsigned short)aabb.m_min_limit[0];
        new_eps_by_axis[0][i + i].m_node_idx = (unsigned short)sweep_node_idx;
        new_eps_by_axis[0][i + i + 1].m_value = (unsigned short)aabb.m_max_limit[0];
        new_eps_by_axis[0][i + i + 1].m_node_idx = (unsigned short)sweep_node_idx;

        new_eps_by_axis[1][i + i].m_value = (unsigned short)aabb.m_min_limit[1];
        new_eps_by_axis[1][i + i].m_node_idx = (unsigned short)sweep_node_idx;
        new_eps_by_axis[1][i + i + 1].m_value = (unsigned short)aabb.m_max_limit[1];
        new_eps_by_axis[1][i + i + 1].m_node_idx = (unsigned short)sweep_node_idx;

        new_eps_by_axis[2][i + i].m_value = (unsigned short)aabb.m_min_limit[2];
        new_eps_by_axis[2][i + i].m_node_idx = (unsigned short)sweep_node_idx;
        new_eps_by_axis[2][i + i + 1].m_value = (unsigned short)aabb.m_max_limit[2];
        new_eps_by_axis[2][i + i + 1].m_node_idx = (unsigned short)sweep_node_idx;
    }

    phys_sweep_node *nodes = &m_nodes[0];
    {
        for (int a = 0; a < 3; a++)
            phys_axis_sweep_quick_sort_endpoints(new_eps_by_axis[a], new_node_cnt * 2);
    }
    phys_sweep_endpoint *temp_endpoints = phys_alloc_stack<phys_sweep_endpoint>(old_endpoint_cnts[0]);
    {
        for (int a = 0; a < 3; a++)
            m_axis[a].merge_endpoint_batch(nodes, old_endpoint_cnts[a], new_node_cnt * 2, a, temp_endpoints);
    }
    phys_free_stack(temp_endpoints);

    if (get_marker_count()) {
        int delta = new_node_cnt * 2;
        for (int m = 0; m < get_marker_count(); m++) {
            phys_sweep_marker &marker = m_markers[m];
            phys_sweep_node &n = m_nodes[marker.m_node_idx];
            n.max_y = (unsigned short)(n.max_y + delta);
            n.max_z_val = (unsigned short)(n.max_z_val + delta);
        }
    }

    {
        int sweep_node_count = m_nodes.getSize();
        int atom_result_bytes = sweep_node_count >> 3;
        int word_cnt = (sweep_node_count >> 5) + 8;
        unsigned int *query_node_bits = phys_alloc_stack<unsigned int>(word_cnt);
        mem_zero16(query_node_bits, atom_result_bytes);
        for (int i = 0; i < new_node_cnt; i++)
            static_flip_bit(query_node_bits, old_node_cnt + i);
        query_batch_aabb_sub(query_node_bits, new_pairs, true);
        phys_free_stack(query_node_bits);
    }
}

// @address 0x6822d0 ?remove_broadphase_object_batch@phys_axis_sweep@@UAAXAAV?$phys_array@PAVphys_broadphase_handle@@@@AAV?$phys_array@Vphys_broadphase_handle_pair@@@@@Z
void phys_axis_sweep::remove_broadphase_object_batch(phys_array<phys_broadphase_handle*> &remove_handle_list,
    phys_array<phys_broadphase_handle_pair> &removed_pairs_out)
{
    int remove_count = remove_handle_list.getSize();
    int sweep_node_count = m_nodes.getSize();
    {
        int bit_cnt = sweep_node_count;
        int atom_result_bytes = bit_cnt >> 3;
        int word_cnt = (bit_cnt >> 5) + 8;
        unsigned int *query_node_bits = phys_alloc_stack<unsigned int>(word_cnt);
        mem_zero16(query_node_bits, atom_result_bytes);
        for (int i = 0; i < remove_count; i++) {
            phys_broadphase_handle *object = remove_handle_list[i];
            static_flip_bit(query_node_bits, object->m_handle_id);
        }
        query_batch_aabb_sub(query_node_bits, removed_pairs_out, false);
        phys_free_stack(query_node_bits);
    }

    phys_broadphase_handle missing_handle;
    missing_handle.m_handle_id = 0;
    {
        for (int i = 0; i < remove_count; i++) {
            int sweep_node_idx = remove_handle_list[i]->m_handle_id;
            m_nodes[sweep_node_idx].m_handle = &missing_handle;
            remove_handle_list[i]->m_handle_id = 0;
        }
    }

    phys_local_buffer<int> node_relocations(sweep_node_count);
    int last_element_idx = sweep_node_count - 1;
    int i = 0;
    while (i <= last_element_idx) {
        if (m_nodes[last_element_idx].m_handle == &missing_handle) {
            node_relocations[last_element_idx] = -1;
            last_element_idx--;
            continue;
        }
        phys_sweep_node &node = m_nodes[i];
        if (node.m_handle == &missing_handle) {
            node = m_nodes[last_element_idx];
            node_relocations[last_element_idx] = i;
            node_relocations[i] = -1;
            if (!node.is_marker())
                node.m_handle->m_handle_id = i;
            else
                node.get_marker(m_markers)->m_node_idx = (unsigned short)i;
            last_element_idx--;
        } else {
            node_relocations[i] = i;
        }
        i++;
    }
    sweep_node_count = last_element_idx + 1;
    m_nodes.set_array_size(sweep_node_count);

    m_axis[0].remove_node_batch(&m_nodes[0], 0, node_relocations);
    m_axis[1].remove_node_batch(&m_nodes[0], 1, node_relocations);
    m_axis[2].remove_node_batch(&m_nodes[0], 2, node_relocations);

    if (get_marker_count()) {
        int delta = remove_count * 2;
        for (int m = 0; m < get_marker_count(); m++) {
            phys_sweep_marker &marker = m_markers[m];
            phys_sweep_node &node = m_nodes[marker.m_node_idx];
            node.max_y = (unsigned short)(node.max_y - delta);
            node.max_z_val = (unsigned short)(node.max_z_val - delta);
            for (int j = 0; j < marker.m_overlapping_objects.getSize(); j++)
                marker.m_overlapping_objects[j] = (unsigned short)node_relocations[marker.m_overlapping_objects[j]];
        }
    }
}


__forceinline void phys_axis_sweep::add_node_pair(const phys_sweep_node *n0, const phys_sweep_node *n1,
    phys_array<phys_broadphase_handle_pair> &batch_pairs_out, phys_bool pair_add_mode) const
{
    if (n0->yz_disjoint(*n1))
        return;
    if (!n1->is_marker()) {
        if (!n0->is_marker()) {
            phys_broadphase_handle_pair &pair = batch_pairs_out.expand_one();
            pair.m_a = n0->m_handle;
            pair.m_b = n1->m_handle;
            return;
        }
        phys_algorithm::swap(n0, n1);
    }
    int i0 = n0 - m_nodes.begin();
    phys_sweep_marker *m = n1->get_marker(m_markers);
    if (pair_add_mode) {
        m->m_overlapping_objects.array_push_back((unsigned short)i0);
    } else {
        int i = m->m_overlapping_objects.array_index_of((unsigned short)i0);
        m->m_overlapping_objects.remove_at(i);
    }
}

// @address 0x680e00 ?query_batch_aabb_sub@phys_axis_sweep@@IBAXPAIAAV?$phys_array@Vphys_broadphase_handle_pair@@@@Vphys_bool@@@Z
void phys_axis_sweep::query_batch_aabb_sub(unsigned int *query_node_bits, phys_array<phys_broadphase_handle_pair> &batch_pairs_out,
    phys_bool pair_add_mode) const
{
    phys_inline_array<unsigned short, 256> open_query_nodes;
    phys_inline_array<unsigned short, 256> open_other_nodes;

    const phys_sweep_axis &axis = m_axis[0];

    const phys_sweep_endpoint *endpoints_end = &axis.m_sweep_endpoints[axis.m_sweep_endpoints.getSize() - 1];
    for (const phys_sweep_endpoint *ep = &axis.m_sweep_endpoints[1]; ep < endpoints_end; ep++) {
        int sweep_node_idx = ep->m_node_idx;
        int is_query_node = static_is_bit_set(query_node_bits, sweep_node_idx);

        if (ep->is_max_point()) {
            phys_inline_array<unsigned short, 256> &array = is_query_node ? open_query_nodes : open_other_nodes;
            int index = array.array_index_of((unsigned short)sweep_node_idx);
            array.remove_at(index);
            continue;
        }

        const phys_sweep_node *node = &m_nodes[sweep_node_idx];
        {
            for (int open_query_i = 0; open_query_i < open_query_nodes.getSize(); open_query_i++) {
                unsigned short open_query_idx = open_query_nodes[open_query_i];
                const phys_sweep_node *open_query_node = &m_nodes[open_query_idx];
                {
                    add_node_pair(open_query_node, node, batch_pairs_out, pair_add_mode);
                }
            }
        }

        if (is_query_node) {
            for (int open_other_i = 0; open_other_i < open_other_nodes.getSize(); open_other_i++) {
                unsigned short open_other_idx = open_other_nodes[open_other_i];
                const phys_sweep_node *open_other_node = &m_nodes[open_other_idx];
                {
                    add_node_pair(open_other_node, node, batch_pairs_out, pair_add_mode);
                }
            }
            open_query_nodes.array_push_back((unsigned short)sweep_node_idx);
        } else {
            open_other_nodes.array_push_back((unsigned short)sweep_node_idx);
        }
    }
}

__forceinline void phys_axis_sweep::convert_vector_to_int(const phys_vec4 &vec, unsigned int *out_ints) const
{
    __declspec(align(16)) static const float phys_axis_sweep_min_val[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
    __declspec(align(16)) phys_int_union64 mi;
    phys_axis_sweep_to_u16_clipped(vec, m_offset_low, m_scale, 
        (const phys_vec4 &)phys_axis_sweep_min_val,
        (const phys_vec4 &)s_axis_max_value, mi);
    out_ints[0] = mi.as_u16[0];
    out_ints[1] = mi.as_u16[1];
    out_ints[2] = mi.as_u16[2];
    out_ints[3] = mi.as_u16[3];
}
static __forceinline void phys_axis_sweep_compute_cur_dist(
    int component, phys_vec4 &axis_cur_dist, const phys_axis_sweep::phys_sweep_endpoint *ep, phys_vec4 &math_inv_scale, phys_vec4 &inv_offset)
{
#if !defined(_XBOX) || defined(__clang__)
    axis_cur_dist(component) = (float)ep->m_value * math_inv_scale(component) - inv_offset(component);
#else
    __vector4 val = __vor(__lvlx(ep, -2), __lvrx(ep, 16 - 2));
    val = __vand(val, __lvx(k_phys_axis_sweep_endpoint_mask, 0));
    val = __vcfsx(val, 0);
    val = __vspltw(val, 0);
    val = __vsubfp(__vmulfp(val, math_inv_scale.m_simd_quad), inv_offset.m_simd_quad);
    axis_cur_dist.m_simd_quad = __vsel(axis_cur_dist.m_simd_quad, val, __lvx(k_phys_axis_sweep_select_masks[component], 0));
#endif
}
static __forceinline int phys_axis_sweep_min_component(const phys_vec4 &axis_cur_dist, float &min)
{
    __vector4 v = axis_cur_dist.m_simd_quad;
    __vector4 i = __lvx(k_phys_axis_sweep_component_indices, 0);
    __vector4 rot1_vec = __vpermwi(v, VPERMWI_CONST(1, 2, 0, 0));
    __vector4 idx_rot1 = __vpermwi(i, VPERMWI_CONST(1, 2, 0, 0));
    __vector4 rot2_vec = __vpermwi(v, VPERMWI_CONST(2, 0, 1, 0));
    __vector4 idx_rot2 = __vpermwi(i, VPERMWI_CONST(2, 0, 1, 0));

    __vector4 sel_vr1_cmp = __vcmpgefp(v, rot1_vec);
    __vector4 sel_vr1 = __vsel(v, rot1_vec, sel_vr1_cmp);
    __vector4 idx_sel1 = __vsel(i, idx_rot1, sel_vr1_cmp);

    __vector4 sel_vr1_vr2_cmp = __vcmpgefp(sel_vr1, rot2_vec);
    __vector4 sel_vr1_vr2 = __vsel(sel_vr1, rot2_vec, sel_vr1_vr2_cmp);
    __vector4 idx_sel2 = __vsel(idx_sel1, idx_rot2, sel_vr1_vr2_cmp);

    sel_vr1_vr2 = __vspltw(sel_vr1_vr2, 0);
    idx_sel2 = __vspltw(idx_sel2, 0);

    __stvewx(sel_vr1_vr2, &min, 0);

    int sweep_min_index;
    __stvewx(idx_sel2, &sweep_min_index, 0);
    return sweep_min_index;
}
// @address 0x67ef08 ?trace_ray@phys_axis_sweep@@UBAXABUphys_ray_cast_input@phys_broadphase@@PAVphys_broadphase_cast_collector@@H@Z
void phys_axis_sweep::trace_ray(const phys_ray_cast_input &ray_input, phys_broadphase_cast_collector *cast_collector_base, int collector_stride) const
{
    int inertia_vertex_stride = collector_stride;
    __declspec(align(16)) unsigned int aabb[4];
    {
        convert_vector_to_int(ray_input.m_ray_from, aabb);
    }

    phys_local_buffer<unsigned char> scratch_bytes(m_nodes.getSize() + 16);
    {
        mem_zero16(scratch_bytes.begin(), m_nodes.getSize());
    }

    const phys_sweep_endpoint *start_ep_by_axis[3];

    const phys_sweep_axis *axis_cache = (ray_input.m_aabb_cache_info) ? reinterpret_cast<const phys_sweep_axis *>(ray_input.m_aabb_cache_info) : &m_axis[0];
    {
        unsigned char *bit_field = scratch_bytes.begin();
        int mask = 0x11;
        for (int i = 0; i < 3; i++) {
            const phys_sweep_axis &axis = axis_cache[i];

            const phys_sweep_endpoint *p;
            const phys_sweep_endpoint *mid_endpoint = &axis.m_sweep_endpoints[axis.m_sweep_endpoints.getSize() >> 1];
            if (aabb[i] < mid_endpoint->m_value) {
                const phys_sweep_endpoint *endpoints_tail = axis.m_sweep_endpoints.begin() + (axis.m_sweep_endpoints.getSize() - 4);

                for (p = &axis.m_sweep_endpoints[1]; p < endpoints_tail && p[3].m_value <= aabb[i]; p += 4) {
                    bit_field[p[0].m_node_idx] ^= mask;
                    bit_field[p[1].m_node_idx] ^= mask;
                    bit_field[p[2].m_node_idx] ^= mask;
                    bit_field[p[3].m_node_idx] ^= mask;
                }

                for (; p->m_value <= aabb[i]; p++) {
                    bit_field[p->m_node_idx] ^= mask;
                }
            } else {
                const phys_sweep_endpoint *endpoints_tail = axis.m_sweep_endpoints.begin() + 4;

                for (p = &axis.m_sweep_endpoints[axis.m_sweep_endpoints.getSize() - 2]; p >= endpoints_tail && p[-3].m_value > aabb[i]; p -= 4) {
                    bit_field[p[0].m_node_idx] ^= mask;
                    bit_field[p[-1].m_node_idx] ^= mask;
                    bit_field[p[-2].m_node_idx] ^= mask;
                    bit_field[p[-3].m_node_idx] ^= mask;
                }

                for (; p->m_value > aabb[i]; p--) {
                    bit_field[p->m_node_idx] ^= mask;
                }
                p++;
            }

            start_ep_by_axis[i] = p;
            mask = mask << 1;
        }
    }

    phys_local_buffer<float> min_hit_fracs(ray_input.m_cast_count);
    {
        for (int x = 0; x < ray_input.m_cast_count; x++) {
            min_hit_fracs[x] = 1.0f;
        }
    }

    {
        const phys_sweep_node *node = &m_nodes[0];
        int sweep_node_count = m_nodes.getSize();

        const int *bit_words = reinterpret_cast<const int *>(scratch_bytes.begin());
        const int *end = bit_words + (sweep_node_count >> 2) + 1;
        for (; bit_words < end;) {
            if (((bit_words[0] + 0x01010101) & 0x08080808) == 0) {
                if (((bit_words[1] + 0x01010101) & 0x08080808) == 0) {
                    if (((bit_words[2] + 0x01010101) & 0x08080808) == 0) {
                        node += 12;
                        bit_words += 3;
                        continue;
                    }
                    node += 8;
                    bit_words += 2;
                    continue;
                }
                node += 4;
                bit_words++;
                continue;
            }

            const unsigned char *bit_bytes = reinterpret_cast<const unsigned char *>(bit_words);
#define DO_HIT(hit_node) { \
                phys_broadphase_cast_collector *collector = cast_collector_base; \
                for (int x = 0; x < ray_input.m_cast_count; x++) { \
                    min_hit_fracs[x] = phys_math::min_of_two(min_hit_fracs[x], collector->add_broadphase_handle(hit_node.m_handle, x)); \
                    collector = phys_add_byte_offset(collector, inertia_vertex_stride); \
                } \
            };
            if (bit_bytes[0] == 0x77 && !node[0].is_marker()) DO_HIT(node[0]);
            if (bit_bytes[1] == 0x77 && !node[1].is_marker()) DO_HIT(node[1]);
            if (bit_bytes[2] == 0x77 && !node[2].is_marker()) DO_HIT(node[2]);
            if (bit_bytes[3] == 0x77 && !node[3].is_marker()) DO_HIT(node[3]);
            node += 4;
            bit_words++;
#undef DO_HIT
        }
        scratch_bytes.begin()[0] = 0x88;
    }

    phys_broadphase_cast_collector *collector = cast_collector_base;
    int sweep_node_count = m_nodes.getSize();
    for (int broadphase_cast_index = 0; broadphase_cast_index < ray_input.m_cast_count; collector = phys_add_byte_offset(collector,
        inertia_vertex_stride), broadphase_cast_index++) {
        float min_hit_frac = min_hit_fracs[broadphase_cast_index];

        const phys_sweep_endpoint *ep[3];
        ep[0] = start_ep_by_axis[0];
        ep[1] = start_ep_by_axis[1];
        ep[2] = start_ep_by_axis[2];

        phys_vec4 math_inv_scale;
        phys_vec4 inv_offset;
        int direction[3];
        {
            phys_vec4 to = *phys_add_byte_offset_const(ray_input.m_to_base, ray_input.m_to_stride * broadphase_cast_index);
            phys_vec4 dir;
            dir.set_sub4(to, ray_input.m_ray_from);
            for (int i = 0; i < 3; i++) {
                float delta = m_scale(i) * dir(i);
                float abs_delta = phys_math::fabs(delta);
                float ray_start_quant = (ray_input.m_ray_from(i) + m_offset_low(i)) * m_scale(i);
                float ray_end_quant = (to(i) + m_offset_low(i)) * m_scale(i);
                if (abs_delta < 1.192092896e-07f * ray_start_quant || abs_delta < 1.192092896e-07f * ray_end_quant) {
                    math_inv_scale(i) = 0.0f;
                    inv_offset(i) = -2.0f;
                    continue;
                }
                float inv_delta = 1.0f / delta;
                direction[i] = sizeof(phys_sweep_endpoint);
                if (delta < 0.0f) {
                    direction[i] = -(int)sizeof(phys_sweep_endpoint);
                    ep[i]--;
                }
                math_inv_scale(i) = inv_delta;
                float corrected_pos = ray_start_quant - m_int_to_float_floor_fix;
                float offset = corrected_pos * inv_delta;
                inv_offset(i) = offset;
            }
        }

        phys_vec4 axis_cur_dist;
        axis_cur_dist(0) = ep[0]->m_value * math_inv_scale(0) - inv_offset(0);
        axis_cur_dist(1) = ep[1]->m_value * math_inv_scale(1) - inv_offset(1);
        axis_cur_dist(2) = ep[2]->m_value * math_inv_scale(2) - inv_offset(2);

        unsigned char *bit_field = scratch_bytes.begin();

        while (1) {
            float min_component;
            int component = phys_axis_sweep_min_component(axis_cur_dist, min_component);

            if (min_component > min_hit_frac) {
                break;
            }

            while (1) {
                {
                    const int ep_node_idx = ep[component]->m_node_idx;
                    bit_field[ep_node_idx] ^= 1 << (component + 4);

                    if (bit_field[ep_node_idx] >= 0x70) {
                        if (ep_node_idx == 0) {
                            axis_cur_dist(component) = 2.0f;
                            break;
                        }

                        const phys_sweep_node &node = m_nodes[ep_node_idx];
                        if (!node.is_marker()) {
                            min_hit_frac = phys_math::min_of_two(min_hit_frac, collector->add_broadphase_handle(node.m_handle, broadphase_cast_index));
                        }
                    }
                }

                {
                    unsigned short last_ep_value = ep[component]->m_value;
                    ep[component] = phys_add_byte_offset_const<phys_sweep_endpoint>(ep[component], direction[component]);
                    unsigned short queue_new_value = ep[component]->m_value;
                    if (last_ep_value == queue_new_value) {
                        continue;
                    }
                    phys_axis_sweep_compute_cur_dist(component, axis_cur_dist, ep[component], math_inv_scale, inv_offset);
                    break;
                }
            }
        }
        if (broadphase_cast_index < ray_input.m_cast_count - 1) {
            int *bit_words = reinterpret_cast<int *>(scratch_bytes.begin());
            const int *end = bit_words + (sweep_node_count >> 2) + 1;
            int mask = 0x0f0f0f0f;
            for (; bit_words < end;) {
                int a = bit_words[0];
                int b = bit_words[1];
                a &= mask;
                b &= mask;
                a |= a << 4;
                b |= b << 4;
                bit_words[0] = a;
                bit_words[1] = b;
                bit_words += 2;
            }
        }
    }
}


// @address 0x67fb68 ?cast_bounds@phys_axis_sweep@@UBAXABUphys_aabb_cast_input@phys_broadphase@@AAVphys_broadphase_cast_collector@@@Z
void phys_axis_sweep::cast_bounds(const phys_aabb_cast_input &input, phys_broadphase_cast_collector &collector) const
{
    __declspec(align(16)) unsigned int aabb_min[4];
    __declspec(align(16)) unsigned int aabb_max[4];

    phys_vec4 ray_from_min;
    ray_from_min.set_sub4(input.m_ray_from, input.m_half_extents);
    phys_vec4 ray_from_max;
    ray_from_max.set_add4(input.m_ray_from, input.m_half_extents);
    {
        convert_vector_to_int(ray_from_min, aabb_min);
        convert_vector_to_int(ray_from_max, aabb_max);
    }

    phys_local_buffer<unsigned char> scratch_bytes(m_nodes.getSize() + 16);
    {
        mem_zero16(scratch_bytes.begin(), m_nodes.getSize());
    }

    const phys_sweep_endpoint *start_ep_min[3];
    const phys_sweep_endpoint *start_ep_max[3];
    const phys_sweep_axis *axis_cache = (input.m_aabb_cache_info) ? reinterpret_cast<const phys_sweep_axis *>(input.m_aabb_cache_info) : &m_axis[0];
    {
        unsigned char *bit_field = scratch_bytes.begin();
        int mask = 1;
        for (int i = 0; i < 3; i++) {
            const phys_sweep_axis &axis = axis_cache[i];

            const phys_sweep_endpoint *p;
            if (aabb_min[i] < (&axis.m_sweep_endpoints[axis.m_sweep_endpoints.getSize() >> 1])->m_value) {
                const phys_sweep_endpoint *endpoints_tail = axis.m_sweep_endpoints.begin() + (axis.m_sweep_endpoints.getSize() - 4);

                for (p = &axis.m_sweep_endpoints[1]; p < endpoints_tail && p[3].m_value <= aabb_min[i]; p += 4) {
                    bit_field[p[0].m_node_idx] ^= mask;
                    bit_field[p[1].m_node_idx] ^= mask;
                    bit_field[p[2].m_node_idx] ^= mask;
                    bit_field[p[3].m_node_idx] ^= mask;
                }

                for (; p->m_value <= aabb_min[i]; p++) {
                    bit_field[p->m_node_idx] ^= mask;
                }
                start_ep_min[i] = p;

                for (; p->m_value <= aabb_max[i]; p++) {
                    bit_field[p->m_node_idx] ^= mask & (p->is_max_point() - 1);
                }
                start_ep_max[i] = p;
            } else {
                const phys_sweep_endpoint *endpoints_tail = axis.m_sweep_endpoints.begin() + 4;

                for (p = &axis.m_sweep_endpoints[axis.m_sweep_endpoints.getSize() - 2]; p >= endpoints_tail && p[-3].m_value > aabb_max[i]; p -= 4) {
                    bit_field[p[0].m_node_idx] ^= mask;
                    bit_field[p[-1].m_node_idx] ^= mask;
                    bit_field[p[-2].m_node_idx] ^= mask;
                    bit_field[p[-3].m_node_idx] ^= mask;
                }

                for (; p->m_value > aabb_max[i]; p--) {
                    bit_field[p->m_node_idx] ^= mask;
                }
                start_ep_max[i] = p + 1;

                for (; p->m_value > aabb_min[i]; p--) {
                    bit_field[p->m_node_idx] ^= mask & -p->is_max_point();
                }
                start_ep_min[i] = p + 1;
            }
            mask = mask << 1;
        }
    }

    float min_hit_frac = 1.0f;
    {
        const phys_sweep_node *node = &m_nodes[0];
        int sweep_node_count = m_nodes.getSize();

        unsigned char *bit_field = scratch_bytes.begin();
        const int *bit_words = reinterpret_cast<const int *>(bit_field);
        const int *end = bit_words + (sweep_node_count >> 2) + 1;
        for (; bit_words < end; node += 4, bit_words++) {
            if (((bit_words[0] + 0x01010101) & 0x08080808) == 0)
                continue;

            const unsigned char *bit_bytes = reinterpret_cast<const unsigned char *>(bit_words);
            if (bit_bytes[0] == 7 && !node[0].is_marker())
                min_hit_frac = phys_math::min_of_two(min_hit_frac, collector.add_broadphase_handle(node[0].m_handle, 0));
            if (bit_bytes[1] == 7 && !node[1].is_marker())
                min_hit_frac = phys_math::min_of_two(min_hit_frac, collector.add_broadphase_handle(node[1].m_handle, 0));
            if (bit_bytes[2] == 7 && !node[2].is_marker())
                min_hit_frac = phys_math::min_of_two(min_hit_frac, collector.add_broadphase_handle(node[2].m_handle, 0));
            if (bit_bytes[3] == 7 && !node[3].is_marker())
                min_hit_frac = phys_math::min_of_two(min_hit_frac, collector.add_broadphase_handle(node[3].m_handle, 0));
        }
    }

    phys_vec4 math_inv_scale;
    phys_vec4 inv_offset_min;
    phys_vec4 inv_offset_max;
    int direction[3];
    int forward_flags[3];
    int reverse_flags[3];
    {
        phys_vec4 dir;
        dir.set_sub4(input.m_ray_to, input.m_ray_from);
        for (int i = 0; i < 3; i++) {
            float delta = m_scale(i) * dir(i);
            float abs_delta = phys_math::fabs(delta);
            if (delta > 0.0f) {
                direction[i] = sizeof(phys_sweep_endpoint);
                reverse_flags[i] = 0;
                forward_flags[i] = 1;
            } else {
                direction[i] = -(int)sizeof(phys_sweep_endpoint);
                phys_algorithm::swap(start_ep_min[i], start_ep_max[i]);
                phys_algorithm::swap(ray_from_min(i), ray_from_max(i));
                phys_algorithm::swap(aabb_min[i], aabb_max[i]);
                start_ep_min[i]--;
                start_ep_max[i]--;
                reverse_flags[i] = 1;
                forward_flags[i] = 0;
            }

            float ray_start_quant = (ray_from_min(i) + m_offset_low(i)) * m_scale(i);
            float ray_end_quant = (input.m_ray_to(i) + m_offset_low(i)) * m_scale(i);
            if (abs_delta < 1.192092896e-07f * ray_start_quant || abs_delta < 1.192092896e-07f * ray_end_quant) {
                math_inv_scale(i) = 0.0f;
                inv_offset_min(i) = -2.0f;
                inv_offset_max(i) = -2.0f;
                continue;
            }
            float inv_delta = 1.0f / delta;
            math_inv_scale(i) = inv_delta;
            {
                float position = (ray_from_min(i) + m_offset_low(i)) * m_scale(i);
                float corrected_pos = position - m_int_to_float_floor_fix;
                float offset = corrected_pos * inv_delta;
                inv_offset_min(i) = offset;
            }
            {
                float position = (ray_from_max(i) + m_offset_low(i)) * m_scale(i);
                float corrected_pos = position - m_int_to_float_floor_fix;
                float offset = corrected_pos * inv_delta;
                inv_offset_max(i) = offset;
            }
        }
    }

    phys_vec4 cur_distance_min;
    phys_vec4 cur_distance_max;
    cur_distance_min(0) = start_ep_min[0]->m_value * math_inv_scale(0) - inv_offset_min(0);
    cur_distance_min(1) = start_ep_min[1]->m_value * math_inv_scale(1) - inv_offset_min(1);
    cur_distance_min(2) = start_ep_min[2]->m_value * math_inv_scale(2) - inv_offset_min(2);
    cur_distance_max(0) = start_ep_max[0]->m_value * math_inv_scale(0) - inv_offset_max(0);
    cur_distance_max(1) = start_ep_max[1]->m_value * math_inv_scale(1) - inv_offset_max(1);
    cur_distance_max(2) = start_ep_max[2]->m_value * math_inv_scale(2) - inv_offset_max(2);

    unsigned char *bit_field = scratch_bytes.begin();
    bit_field[0] = 8;

    float unused_min_value;
    int min_component_idx = phys_axis_sweep_min_component(cur_distance_min, unused_min_value);
    int max_component_idx = phys_axis_sweep_min_component(cur_distance_max, unused_min_value);

    while (1) {
        if (cur_distance_max(max_component_idx) < cur_distance_min(min_component_idx)) {
            if (cur_distance_max(max_component_idx) > min_hit_frac) {
                break;
            }

            while (1) {
                const int ep_node_idx = start_ep_max[max_component_idx]->m_node_idx;
                int flip = (forward_flags[max_component_idx] ^ start_ep_max[max_component_idx]->is_max_point()) << max_component_idx;
                bit_field[ep_node_idx] ^= flip;

                if (bit_field[ep_node_idx] >= 7 && flip) {
                    if (ep_node_idx == 0) {
                        cur_distance_max(max_component_idx) = 2.0f;
                        float padding_words;
                        max_component_idx = phys_axis_sweep_min_component(cur_distance_max, padding_words);
                        break;
                    }

                    const phys_sweep_node &node = m_nodes[ep_node_idx];
                    if (!node.is_marker()) {
                        min_hit_frac = phys_math::min_of_two(min_hit_frac, collector.add_broadphase_handle(node.m_handle, 0));
                    }
                }

                unsigned short last_ep_value = start_ep_max[max_component_idx]->m_value;
                start_ep_max[max_component_idx] = phys_add_byte_offset_const<phys_sweep_endpoint>(start_ep_max[max_component_idx],
                    direction[max_component_idx]);
                unsigned short queue_new_value = start_ep_max[max_component_idx]->m_value;
                if (last_ep_value == queue_new_value) {
                    continue;
                }
                phys_axis_sweep_compute_cur_dist(max_component_idx, cur_distance_max, start_ep_max[max_component_idx], math_inv_scale, inv_offset_max);
                float padding_words;
                max_component_idx = phys_axis_sweep_min_component(cur_distance_max, padding_words);
                break;
            }
        } else {
            if (cur_distance_min(min_component_idx) > min_hit_frac) {
                break;
            }

            while (1) {
                const int ep_node_idx = start_ep_min[min_component_idx]->m_node_idx;
                bit_field[ep_node_idx] ^= (reverse_flags[min_component_idx] ^ start_ep_min[min_component_idx]->is_max_point()) << min_component_idx;

                if (bit_field[ep_node_idx] > 8) {
                    cur_distance_min(min_component_idx) = 2.0f;
                    float padding_words;
                    min_component_idx = phys_axis_sweep_min_component(cur_distance_min, padding_words);
                    break;
                }

                unsigned short last_ep_value = start_ep_min[min_component_idx]->m_value;
                start_ep_min[min_component_idx] = phys_add_byte_offset_const<phys_sweep_endpoint>(start_ep_min[min_component_idx],
                    direction[min_component_idx]);
                unsigned short queue_new_value = start_ep_min[min_component_idx]->m_value;
                if (last_ep_value == queue_new_value) {
                    continue;
                }
                phys_axis_sweep_compute_cur_dist(min_component_idx, cur_distance_min, start_ep_min[min_component_idx], math_inv_scale, inv_offset_min);
                float padding_words;
                min_component_idx = phys_axis_sweep_min_component(cur_distance_min, padding_words);
                break;
            }
        }
    }
}





__forceinline void *phys_axis_sweep::phys_sweep_marker::operator new(size_t, void *p) { return p; }
__forceinline phys_axis_sweep::phys_sweep_marker::phys_sweep_marker() {}

// @address 0x680808 ??0phys_axis_sweep@@QAA@ABVphys_vec4@@0H@Z
phys_axis_sweep::phys_axis_sweep(const phys_vec4 &world_min, const phys_vec4 &world_max, int sweep_marker_count)
    : m_original_aabb(world_min, world_max)
{
    if (!sweep_marker_count) {
        sweep_marker_count++;
    }
    int log2_marker_cnt = -1;
    int x = sweep_marker_count;
    while (x > 0) {
        log2_marker_cnt++;
        x = x >> 1;
    }

    const int estimated_max_objects = 255;
    m_nodes.reserve(estimated_max_objects);

    m_offset_low.set_zero4();
    m_offset_high.set_zero4();
    m_scale.flags_set_all(1.0f);
    {
        float l = 10.0f;
        float h = 11.0f;
        phys_aabb aabb;
        phys_axis_sweep::phys_sweep_aabb int_aabb;
        for (int i = 0; i < 23; i++) {
            float m = (l + h) * 0.5f;
            aabb.m_min_limit.flags_set_all(m);
            aabb.m_max_limit.flags_set_all(m + 1.0f);
            convert_aabb_to_int(aabb, int_aabb);
            if (int_aabb.m_max_limit[0] < 12) {
                l = m;
            } else {
                h = m;
            }
        }
        m_int_to_float_floor_fix = (l + h) * 0.5f - 11.0f;
    }

    {
        phys_vec4 span;
        span.set_sub4(world_max, world_min);
        phys_vec4 inv_span_vec;
        inv_span_vec.set(1.0f / span(0), 1.0f / span(1), 1.0f / span(2), 0.0f);
        m_scale.flags_set_all(65532.0f);
        m_scale.mul4(inv_span_vec);
        m_offset_low.m_simd_quad = __vxor(world_min.m_simd_quad, k_phys_axis_sweep_negate_mask);
        phys_vec4 rounding;
        rounding.set_mul4(phys_simd_scalar(1.0f / 65532.0f), span);
        m_offset_high.set_add4(m_offset_low, rounding);
        m_scale(3) = 0.0f;
        m_offset_low(3) = 0.0f;
        m_offset_high(3) = 0.0f;
    }

    phys_sweep_node &sentinel_node = m_nodes.expand_one();
    sentinel_node.min_y = sentinel_node.lower_z = 0;
    sentinel_node.lower_x = 0;
    {
        const int max_endpoints = 512;
        const int marker_cnt = (1 << log2_marker_cnt) - 1;
        m_axis[0].m_sweep_endpoints.reserve(max_endpoints + 2 * marker_cnt);
        m_axis[1].m_sweep_endpoints.reserve(max_endpoints);
        m_axis[2].m_sweep_endpoints.reserve(max_endpoints);

        phys_sweep_endpoint ep;
        ep.m_node_idx = 0;
        ep.m_value = 0;
        m_axis[0].m_sweep_endpoints.push_back_unchecked(ep);
        m_axis[1].m_sweep_endpoints.push_back_unchecked(ep);
        m_axis[2].m_sweep_endpoints.push_back_unchecked(ep);
    }

    {
        m_log2_marker_count = log2_marker_cnt;
        m_sweep_marker_count = (1 << m_log2_marker_count) - 1;
        m_markers = 0;
        if (m_sweep_marker_count) {
            m_markers = phys_alloc<phys_sweep_marker>(m_sweep_marker_count, PHYS_MEM_BROADPHASE);
        }
        for (int i = 0; i < m_sweep_marker_count; i++) {
            phys_sweep_marker &marker = *new (&m_markers[i]) phys_sweep_marker();
            phys_sweep_endpoint ep;
            ep.m_node_idx = (unsigned short)m_nodes.getSize();
            ep.m_value = (unsigned short)((i + 1) << (16 - m_log2_marker_count));
            marker.m_node_idx = ep.m_node_idx;
            marker.m_value = ep.m_value;

            phys_sweep_node &node = m_nodes.expand_one();
            node.lower_x = (unsigned short)m_axis[0].m_sweep_endpoints.getSize();
            m_axis[0].m_sweep_endpoints.push_back_unchecked(ep);
            ep.m_value |= 1;
            node.max_x_val = (unsigned short)m_axis[0].m_sweep_endpoints.getSize();
            m_axis[0].m_sweep_endpoints.push_back_unchecked(ep);
            node.m_handle = (phys_broadphase_handle *)(1 | (i * sizeof(phys_sweep_marker)));
            node.min_y = node.lower_z = 0;
            node.max_y = node.max_z_val = 1;
        }
    }

    {
        sentinel_node.max_x_val = (unsigned short)m_axis[0].m_sweep_endpoints.getSize();
        sentinel_node.max_y = (unsigned short)m_axis[1].m_sweep_endpoints.getSize();
        sentinel_node.max_z_val = (unsigned short)m_axis[2].m_sweep_endpoints.getSize();
        phys_sweep_endpoint ep;
        ep.m_node_idx = 0;
        ep.m_value = 0xfffd;
        m_axis[0].m_sweep_endpoints.push_back_unchecked(ep);
        m_axis[1].m_sweep_endpoints.push_back_unchecked(ep);
        m_axis[2].m_sweep_endpoints.push_back_unchecked(ep);
    }
}


