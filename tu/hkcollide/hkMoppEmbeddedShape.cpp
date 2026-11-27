// hkcollide/hkMoppEmbeddedShape.obj: from the records 0x7057b0 0x7057c8 0x705860 0x705868 0x705870 0x7058b8 0x705920 0x7059a0 0x705a70 0x705ac0 0x705af0 0x705c90 0x705d08 0x705e10 0x705e28 0x705ec8 0x705f48 0x706150 0x7061c8 0x7062b8 0x706460 0x706418 0x706430 0x706588 0x706688 0x706788 0x706790 0x7067a8 0x7067b0
// @compiler xdk6130
// @flags /TP /O2 /Oc
// @address 0x7057b0 ?destroy_loaded_bvh_embedded_shape@@YAXPAX@Z
// @address 0x7057c8 ?gather_memory_stats@phys_bvh_embedded_shape@@UBAXPAVphys_statistics_collector@@@Z
// @address 0x705860 ?shape_query_sphere@phys_bvh_embedded_shape@@UBAXABVphys_sphere@@AAV?$phys_array@I@@@Z
// @address 0x705868 ?query_obb@phys_bvh_embedded_shape@@UBAXABVphys_transform@@ABVphys_vec4@@MAAV?$phys_array@I@@@Z
// @address 0x705870 ?get_child_shape_count@phys_bvh_embedded_shape@@UBAHXZ
// @address 0x7058b8 ?get_first_key@phys_bvh_embedded_shape@@UBAIXZ
// @address 0x705920 ?get_next_key@phys_bvh_embedded_shape@@UBAII@Z
// @address 0x7059a0 ?compute_edge_weld_info@phys_bvh_embedded_shape@@QAAXW4edge_weld_kind@phys_edge_weld_util@@@Z
// @address 0x705a70 ?get_original_key@phys_bvh_embedded_shape@@QBAII@Z
// @address 0x705ac0 ?initialize@phys_bvh_embedded_shape@@QAAXABUbytecode_info@phys_bvh_bytecode@@PBEI@Z
// @address 0x705e10 ?get_aabb_impl@phys_bvh_embedded_shape@@UBAXABVphys_transform@@MAAVphys_aabb@@@Z
// @address 0x706418 ?fixup_loaded_bvh_embedded_shape@@YAXPAX@Z
// @address 0x706430 ?get_bvh_embedded_shape_vtable@@YAPBXXZ
// @address 0x706688 ??$quick_sort_recursive@Uphys_bvh_reindexed_terminal@@V?$less@Uphys_bvh_reindexed_terminal@@@phys_algorithm@@@phys_algorithm@@YAXPAUphys_bvh_reindexed_terminal@@HHV?$less@Uphys_bvh_reindexed_terminal@@@0@@Z
// @address 0x706788 ?compute_dma_size@phys_bvh_embedded_shape@@UBAHH@Z
// @address 0x706790 ?get_shape_container@phys_bvh_embedded_shape@@UBAPBVphys_shape_container@@XZ
#include "phys/phys_base_types.h"
// @address 0x706150 ??0phys_bvh_embedded_shape@@QAA@Vphys_fixup_tag@@@Z
// @address 0x7067a8 ??_Ephys_bvh_embedded_shape@@WBA@AAPAXI@Z
// @address 0x7067b0 ??_Gphys_bvh_embedded_shape@@UAAPAXI@Z
#include <vectorintrinsics.h>
#include "physics/havok_halo_external.h"
#include "phys/phys_math.h"
#include "phys/phys_object.h"
#include "phys/phys_aabb.h"
#include "phys/phys_aabb_util.h"
#include "phys/phys_collide.h"
#include "phys/phys_statistics_collector.h"
#include "phys/phys_bvh_embedded_shape.h"
#include "phys/phys_bvh_long_ray_vm.h"
#include "phys/phys_bvh_obb_vm.h"
#include "phys/phys_algorithm_sort.h"
#include "phys/phys_edge_weld_util.h"
#include "phys/phys_mesh_edge_weld_util.h"
#include "phys/phys_mesh_material.h"
#include "phys/phys_triangle_shape.h"
#include "phys/phys_type_desc.h"
#include <stddef.h>
#include "phys/phys_thread_memory.h"
namespace {
// MOPP chunks encode offsets and terminal counts as big-endian byte pairs.
enum {
    bvh_chunk_bytes = 512,
    bvh_chunk_data_ofs_hi = 1,
    bvh_chunk_data_ofs_lo = 2,
    bvh_chunk_leaf_cnt_hi = 3,
    bvh_chunk_leaf_cnt_lo = 4
};
struct compressed_mesh {
    float m_x;
    float m_y;
    float m_soa_z;
    unsigned char m_vert_count;
    unsigned char m_triangle_count;
    unsigned char m_material_idx_count;
    float m_x_step;
    float m_y_step;
    float m_z_step;
    unsigned int pad2;
};
struct tri_vertex_indices { unsigned short m_vert_coords[3]; };
struct tri_indices { unsigned char m_vert_ids[3]; };
typedef unsigned short edge_weld_bits;
static __forceinline const void *embedded_chunk_data(const unsigned char *code, int chunk_idx) {
    const unsigned char *chunk = code + chunk_idx * bvh_chunk_bytes;
    int offset = (chunk[bvh_chunk_data_ofs_hi] << 8) | chunk[bvh_chunk_data_ofs_lo];
    return chunk + offset;
}
// The welding infos follow the vertices and triangle indices; the material indices follow the welding infos.
static __forceinline const edge_weld_bits *edge_weld_infos(const void *data) {
    const compressed_mesh &packed_mesh = *(const compressed_mesh *)data;
    const tri_vertex_indices *verts = (const tri_vertex_indices *)((const unsigned char *)data + sizeof(compressed_mesh));
    const tri_indices *mesh_tri_idx_list = (const tri_indices *)(verts + packed_mesh.m_vert_count);
    return (const edge_weld_bits *)(mesh_tri_idx_list + packed_mesh.m_triangle_count);
}
static __forceinline unsigned char tri_material_idx(int tri_idx, const void *data) {
    const compressed_mesh &packed_mesh = *(const compressed_mesh *)data;
    const unsigned char *materials = (const unsigned char *)(edge_weld_infos(data) + packed_mesh.m_triangle_count);
    return materials[tri_idx];
}
static __forceinline void set_edge_weld_info(int tri_idx, void *data, unsigned short edge_weld_info) {
    edge_weld_bits *weld_infos_rw = const_cast<edge_weld_bits *>(edge_weld_infos(data));
    weld_infos_rw[tri_idx] = edge_weld_info;
}
}
__forceinline bool operator<(const phys_bvh_reindexed_terminal &a, const phys_bvh_reindexed_terminal &b) {
    return a.m_reindexed_key < b.m_reindexed_key;
}

void destroy_loaded_bvh_embedded_shape(void *object) {
    ((phys_bvh_embedded_shape *)object)->~phys_bvh_embedded_shape();
}
void phys_bvh_embedded_shape::gather_memory_stats(phys_statistics_collector *collector) const {
    collector->begin_stats_object("MoppEmbeddedShape", phys_statistics_collector::stat_category_shared, this);
    collector->add_chunk_stats("Mopp", phys_statistics_collector::stat_category_shared, m_code_data, m_code_size, 0);
    collector->end_object();
}
void phys_bvh_embedded_shape::shape_query_sphere(const phys_sphere &, phys_array<unsigned int> &) const {}
void phys_bvh_embedded_shape::query_obb(const phys_transform &, const phys_vec4 &, float, phys_array<unsigned int> &) const {}
int phys_bvh_embedded_shape::get_child_shape_count() const {
    int list_shape_count = 0;
    int chunk_total = m_code_size / bvh_chunk_bytes;
    for (int chunk_idx = 0; chunk_idx < chunk_total; ++chunk_idx) {
        const unsigned char *chunk = m_code_data + chunk_idx * bvh_chunk_bytes;
        int terminal_total = (chunk[bvh_chunk_leaf_cnt_hi] << 8) | chunk[bvh_chunk_leaf_cnt_lo];
        list_shape_count += terminal_total;
    }
    return list_shape_count;
}
unsigned int phys_bvh_embedded_shape::get_first_key() const {
    int chunk_id = 0;
    int terminal = 0;
    const unsigned char *chunk = m_code_data + chunk_id * bvh_chunk_bytes;
    int terminal_total = (chunk[bvh_chunk_leaf_cnt_hi] << 8) | chunk[bvh_chunk_leaf_cnt_lo];
    int chunk_total = m_code_size / bvh_chunk_bytes;
    while (terminal >= terminal_total) {
        if (chunk_id++ == chunk_total) return ~0u;
        chunk = m_code_data + chunk_id * bvh_chunk_bytes;
        terminal_total = (chunk[bvh_chunk_leaf_cnt_hi] << 8) | chunk[bvh_chunk_leaf_cnt_lo];
    }
    return (chunk_id << 8) | terminal;
}
unsigned int phys_bvh_embedded_shape::get_next_key(unsigned int old_key) const {
    int chunk_id = old_key >> 8;
    int terminal = (old_key & 0xff) + 1;
    const unsigned char *chunk = m_code_data + chunk_id * bvh_chunk_bytes;
    int terminal_total = (chunk[bvh_chunk_leaf_cnt_hi] << 8) | chunk[bvh_chunk_leaf_cnt_lo];
    int chunk_total = m_code_size / bvh_chunk_bytes;
    while (terminal >= terminal_total) {
        terminal = 0;
        if (++chunk_id == chunk_total) return ~0u;
        chunk = m_code_data + chunk_id * bvh_chunk_bytes;
        terminal_total = (chunk[bvh_chunk_leaf_cnt_hi] << 8) | chunk[bvh_chunk_leaf_cnt_lo];
    }
    return (chunk_id << 8) | terminal;
}
void phys_bvh_embedded_shape::compute_edge_weld_info(phys_edge_weld_util::edge_weld_kind edge_weld_type) {
    m_edge_weld_type = (unsigned char)edge_weld_type;
    if (m_edge_weld_type == phys_edge_weld_util::EDGE_WELD_NONE) return;
    for (unsigned int key = get_first_key(); key != ~0u; key = get_next_key(key)) {
        unsigned short edge_weld_info = phys_mesh_edge_weld_util::compute_triangle_weld_info(key, this);
        int chunk_idx = key >> 8;
        int tri_idx = (unsigned char)key;
        void *data = (void *)embedded_chunk_data(m_code_data, chunk_idx);
        set_edge_weld_info(tri_idx, data, edge_weld_info);
    }
}
unsigned int phys_bvh_embedded_shape::get_original_key(unsigned int key) const {
    if (m_reindexed_terminals.getSize() == 0) return ~0u;
    for (int index = 0; index < m_reindexed_terminals.getSize(); ++index) {
        if (m_reindexed_terminals[index].m_reindexed_key == key)
            return m_reindexed_terminals[index].m_original_key;
    }
    return ~0u;
}
void phys_bvh_embedded_shape::initialize(const phys_bvh_bytecode::bytecode_info &info, const unsigned char *code, unsigned int size) {
    m_bvh_code = 0;
    m_code_info.m_offset = info.m_offset;
    m_code_data = code;
    m_code_size = size;
    m_user_data = 0;
    m_type = PHYS_SHAPE_BVH_EMBEDDED;
}
void phys_bvh_embedded_shape::get_aabb_impl(const phys_transform &transform, float tolerance, phys_aabb &out) const {
    phys_aabb_util::compute_bounds(transform, m_aabb_half_extents, m_aabb_center, tolerance, out);
}
void fixup_loaded_bvh_embedded_shape(void *object) {
    phys_fixup_tag flag;
    flag.m_fixup_finishing = 1;
    new (object) phys_bvh_embedded_shape(flag);
}
const void *get_bvh_embedded_shape_vtable() {
    __declspec(align(16)) char storage[sizeof(phys_bvh_embedded_shape)];
    phys_fixup_tag flag;
    new (storage) phys_bvh_embedded_shape(flag);
    return *(const void **)storage;
}

namespace phys_algorithm {
template void quick_sort_recursive<phys_bvh_reindexed_terminal, less<phys_bvh_reindexed_terminal> >(
    phys_bvh_reindexed_terminal *, int, int, less<phys_bvh_reindexed_terminal>);
}
phys_bvh_embedded_shape::phys_bvh_embedded_shape(phys_fixup_tag flag) : phys_bvh_shape(flag)
{
    m_type = PHYS_SHAPE_BVH_EMBEDDED;
    if (flag.m_fixup_finishing == 1) {
        m_code_info.m_offset = m_bvh_code->m_bvh_info.m_offset;
        m_code_data = m_bvh_code->m_data.begin();
        m_code_size = m_bvh_code->m_data.getSize();
    }
}
int phys_bvh_embedded_shape::compute_dma_size(int) const { return 128; }
const phys_shape_container *phys_bvh_embedded_shape::get_shape_container() const { return this; }
// @address 0x705c90 ?get_collision_filter_info@phys_bvh_embedded_shape@@UBAII@Z
unsigned int phys_bvh_embedded_shape::get_collision_filter_info(unsigned int key) const {
    // match: 0x705c90: volatile pointer load preserves memory access order
    const void *const volatile &material_table_base = m_material_base;
    if (material_table_base == 0) return 0;
    int chunk_idx = key >> 8;
    int tri_idx = key & 0xff;
    const void *data = embedded_chunk_data(m_code_data, chunk_idx);
    unsigned char tri_material = tri_material_idx(tri_idx, data);
    return ((const phys_mesh_material *)((const char *)m_material_base + m_material_stride * tri_material))->m_filter_info;
}
// @address 0x705ec8 ?trace_ray_with_collector@phys_bvh_embedded_shape@@UBAXABUphys_ray_input@@ABVphys_collision_proxy@@AAVphys_ray_hit_collector@@@Z
void phys_bvh_embedded_shape::trace_ray_with_collector(const phys_ray_input &input, const phys_collision_proxy &collision_proxy,
    phys_ray_hit_collector &collector) const {
    phys_bvh_long_ray_vm long_ray_vm;
    const int code_ptr_storage_bytes = 16 + sizeof(phys_bvh_bytecode);
    char code_ptr_storage[code_ptr_storage_bytes];
    phys_bvh_bytecode *aligned_code = (phys_bvh_bytecode *)(((size_t)code_ptr_storage + 15) & ~(size_t)15);
    phys_bvh_bytecode::bytecode_info &bvh_code_info = (phys_bvh_bytecode::bytecode_info &)m_code_info;
    aligned_code->m_bvh_info.m_offset = bvh_code_info.m_offset;
    aligned_code->m_data.set_owned_data((unsigned char *)m_code_data, m_code_size, m_code_size);
    phys_collision_proxy child_proxy(&collision_proxy);
    child_proxy.set_proxy_shape(this, 0);
    long_ray_vm.query_long_ray(this, aligned_code, input, child_proxy, collector);
}
// @address 0x705e28 ?trace_ray_impl@phys_bvh_embedded_shape@@UBA?AVphys_bool@@ABUphys_ray_input@@AAUphys_ray_output@@@Z
phys_bool phys_bvh_embedded_shape::trace_ray_impl(const phys_ray_input &input, phys_ray_output &output) const {
    char code_storage[60];
    phys_bvh_long_ray_vm vm;
    phys_bvh_bytecode *code = (phys_bvh_bytecode *)(((size_t)code_storage + 15) & ~(size_t)15);
    phys_bvh_bytecode::bytecode_info &bvh_code_info = (phys_bvh_bytecode::bytecode_info &)m_code_info;
    output.change_key_level(1);
    code->m_bvh_info.m_offset = bvh_code_info.m_offset;
    code->m_data.set_owned_data((unsigned char *)m_code_data, (int)m_code_size, (int)m_code_size);
    phys_bool hit = vm.query_long_ray(this, code, input, output);
    output.change_key_level(-1);
    if (hit) output.set_cached_key(0);
    return hit;
}
// @address 0x705af0 ?recompute_bounds_extents@phys_bvh_embedded_shape@@QAAXXZ
void phys_bvh_embedded_shape::recompute_bounds_extents() {
    phys_aabb aabb;
    aabb.m_min_limit.set_zero4();
    aabb.m_max_limit.set_zero4();
    unsigned int key = get_first_key();
    if (key != ~0u) {
        __declspec(align(16)) char buffer[512];
        const phys_shape *child = get_child_shape(key, buffer);
        child->get_aabb_impl(phys_transform::get_identity(), m_child_radius, aabb);
        phys_aabb child_aabb;
        do {
            child = get_child_shape(key, buffer);
            child->get_aabb_impl(phys_transform::get_identity(), m_child_radius, child_aabb);
            aabb.m_min_limit.set_min4(aabb.m_min_limit, child_aabb.m_min_limit);
            aabb.m_max_limit.set_max4(aabb.m_max_limit, child_aabb.m_max_limit);
            key = get_next_key(key);
        } while (key != ~0u);
    }
    m_aabb_half_extents.set_sub4(aabb.m_max_limit, aabb.m_min_limit);
    m_aabb_half_extents.mul4(phys_simd_scalar(0.5f));
    m_aabb_center.set_add4(aabb.m_min_limit, aabb.m_max_limit);
    m_aabb_center.mul4(phys_simd_scalar(0.5f));
}

// @address 0x706588 ?insert_at@?$phys_array@Uphys_bvh_reindexed_terminal@@@@QAAXHPBUphys_bvh_reindexed_terminal@@H@Z

// @address 0x705d08 ?query_aabb@phys_bvh_embedded_shape@@UBAXABVphys_aabb@@AAV?$phys_array@I@@@Z
void phys_bvh_embedded_shape::query_aabb(class phys_aabb const &aabb, class phys_array<unsigned int> &hits) const
{
    const int code_ptr_storage_bytes = 16 + sizeof(phys_bvh_bytecode);
    char code_ptr_storage[code_ptr_storage_bytes];
    phys_bvh_bytecode *aligned_code = (phys_bvh_bytecode *)(((size_t)code_ptr_storage + 15) & ~(size_t)15);
    phys_bvh_bytecode::bytecode_info &bvh_code_info = (phys_bvh_bytecode::bytecode_info &)m_code_info;
    aligned_code->m_bvh_info.m_offset = bvh_code_info.m_offset;
    aligned_code->m_data.set_owned_data((unsigned char *)m_code_data, m_code_size, m_code_size);

    phys_vec4 offset = bvh_code_info.m_offset;
    float width = 16777216.0f / bvh_code_info.m_offset(3);

    phys_aabb scratch_aabb;
    scratch_aabb.m_min_limit = offset;
    scratch_aabb.m_max_limit = offset;
    phys_vec4 w;
    w.set(width, width, width, 0.0f);
    scratch_aabb.m_max_limit.set_add4(scratch_aabb.m_max_limit, w);
    phys_aabb new_aabb = scratch_aabb;
    new_aabb.m_min_limit.set_max4(new_aabb.m_min_limit, aabb.m_min_limit);
    new_aabb.m_max_limit.set_min4(new_aabb.m_max_limit, aabb.m_max_limit);

    phys_bvh_obb_vm vm;
    vm.query_aabb(aligned_code, new_aabb, (phys_array<phys_bvh_primitive_info> *)&hits);
}
// @address 0x7062b8 ?query_aabb@phys_bvh_embedded_shape@@QBAIABVphys_aabb@@PAIH@Z
unsigned int phys_bvh_embedded_shape::query_aabb(const phys_aabb &aabb, unsigned int *hits, int max_num_keys) const
{
    const int code_ptr_storage_bytes = 16 + sizeof(phys_bvh_bytecode);
    char code_ptr_storage[code_ptr_storage_bytes];
    phys_bvh_bytecode *aligned_code = (phys_bvh_bytecode *)(((size_t)code_ptr_storage + 15) & ~(size_t)15);
    phys_bvh_bytecode::bytecode_info &bvh_code_info = (phys_bvh_bytecode::bytecode_info &)m_code_info;
    aligned_code->m_bvh_info.m_offset = bvh_code_info.m_offset;
    aligned_code->m_data.set_owned_data((unsigned char *)m_code_data, m_code_size, m_code_size);

    phys_vec4 offset = bvh_code_info.m_offset;
    float width = 16777216.0f / bvh_code_info.m_offset(3);

    phys_aabb scratch_aabb;
    scratch_aabb.m_min_limit = offset;
    scratch_aabb.m_max_limit = offset;
    phys_vec4 w;
    w.set(width, width, width, 0.0f);
    scratch_aabb.m_max_limit.set_add4(scratch_aabb.m_max_limit, w);
    phys_aabb new_aabb = scratch_aabb;
    new_aabb.m_min_limit.set_max4(new_aabb.m_min_limit, aabb.m_min_limit);
    new_aabb.m_max_limit.set_min4(new_aabb.m_max_limit, aabb.m_max_limit);

    phys_bvh_obb_vm obb_vm;
    phys_array<unsigned int> hit_keys;
    hit_keys.m_data = hits;
    hit_keys.m_size = 0;
    hit_keys.m_capacity_and_flags = max_num_keys | (int)0x80000000;
    obb_vm.query_aabb(aligned_code, new_aabb, (phys_array<phys_bvh_primitive_info> *)&hit_keys);
    unsigned int bvh_hit_count = obb_vm.get_hit_count();
    return bvh_hit_count;
}

namespace {
static __forceinline void build_tri_shape(phys_triangle_shape &tri_shape_out, int index, const void *data) {
    const compressed_mesh &packed_mesh = *(const compressed_mesh *)data;
    phys_vec4 offset; offset.set(packed_mesh.m_x, packed_mesh.m_y, packed_mesh.m_soa_z, 0.0f);
    phys_vec4 quant_step; quant_step.set(packed_mesh.m_x_step, packed_mesh.m_y_step, packed_mesh.m_z_step, 0.0f);
    const tri_vertex_indices *verts = (const tri_vertex_indices *)((const unsigned char *)data + sizeof(compressed_mesh));
    const tri_indices *mesh_tri_idx_list = (const tri_indices *)(verts + packed_mesh.m_vert_count);
    for (int i = 0; i < 3; ++i) {
        const tri_vertex_indices &byte_vertex = verts[mesh_tri_idx_list[index].m_vert_ids[i]];
        phys_vec4 quant_vert;
        quant_vert.set((float)byte_vertex.m_vert_coords[0], (float)byte_vertex.m_vert_coords[1], (float)byte_vertex.m_vert_coords[2], 0.0f);
        phys_vec4 dome_point_vtx;
        dome_point_vtx.m_simd_quad = __vmaddfp(quant_step.m_simd_quad, quant_vert.m_simd_quad, offset.m_simd_quad);
        tri_shape_out.set_triangle_vertex(i, dome_point_vtx);
    }
    const edge_weld_bits *weld_info_base = (const edge_weld_bits *)(mesh_tri_idx_list + packed_mesh.m_triangle_count);
    tri_shape_out.set_edge_weld_info(weld_info_base[index]);
}
}
// @address 0x705f48 ?get_child_shape@phys_bvh_embedded_shape@@UBAPBVphys_shape@@IAAY0CAA@D@Z
const phys_shape *phys_bvh_embedded_shape::get_child_shape(unsigned int key, char (&buffer)[512]) const {
    int chunk_id = key >> 8;
    int tri_idx = key & 0xff;
    const phys_bvh_embedded_shape *self = this;
    const void *data = embedded_chunk_data(self->m_code_data, chunk_id);
    phys_triangle_shape *triangle = new (buffer) phys_triangle_shape(phys_convex_shape_default_radius, 0, phys_edge_weld_util::EDGE_WELD_NONE);
    build_tri_shape(*triangle, tri_idx, data);
    triangle->set_shape_radius(self->m_child_radius);
    triangle->set_edge_weld_type((phys_edge_weld_util::edge_weld_kind)self->m_edge_weld_type);
    triangle->set_extrusion_vec(self->m_triangle_extrusion);
    return triangle;
}
// @address 0x7061c8 ??1phys_bvh_embedded_shape@@UAA@XZ
phys_bvh_embedded_shape::~phys_bvh_embedded_shape()
{
    m_bvh_code->object_remove_ref();
}
// @address 0x706460 ??0phys_bvh_embedded_shape@@QAA@PBVphys_bvh_bytecode@@PAV?$phys_array@Uphys_bvh_reindexed_terminal@@@@@Z
phys_bvh_embedded_shape::phys_bvh_embedded_shape(const phys_bvh_bytecode *code, phys_array<phys_bvh_reindexed_terminal> *reindexed_terminals) :
    phys_bvh_shape(PHYS_SHAPE_BVH_EMBEDDED), m_bvh_code((phys_bvh_bytecode *)code)
{
    m_bvh_code->add_object_reference();
    m_code_info.m_offset = code->m_bvh_info.m_offset;
    m_code_data = code->m_data.begin();
    m_code_size = code->m_data.getSize();
    m_material_base = 0;
    m_child_radius = phys_convex_shape_default_radius;
    m_edge_weld_type = phys_edge_weld_util::EDGE_WELD_NONE;
    m_triangle_extrusion.set_zero4();
    recompute_bounds_extents();
    if (reindexed_terminals) {
        m_reindexed_terminals.insert_at(0, reindexed_terminals->begin(), reindexed_terminals->getSize());
        phys_algorithm::quick_sort(m_reindexed_terminals.begin(), m_reindexed_terminals.getSize());
    }
}

const phys_type_desc phys_bvh_embedded_shape_type_desc = phys_type_desc(
    "hkMoppEmbeddedShape",
    fixup_loaded_bvh_embedded_shape,
    destroy_loaded_bvh_embedded_shape,
    get_bvh_embedded_shape_vtable());


