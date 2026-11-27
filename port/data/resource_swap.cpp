#include "resource_swap.h"

#include "cache_swap_internal.h"
#include "data_crt.h"
#include "geometry_swap.h"

#ifndef PORT_DATA_HAVE_LIBC_HEADERS
extern "C" void qsort(void *, size_t, size_t, int (*)(const void *, const void *));
#endif

using namespace port_cache_swap;

namespace {

// ---- byte helpers ----

inline uint16_t load_u16(const uint8_t *p) { uint16_t v; memcpy(&v, p, 2); return v; }
inline uint32_t load_u32(const uint8_t *p) { uint32_t v; memcpy(&v, p, 4); return v; }
inline void store_u16(uint8_t *p, uint16_t v) { memcpy(p, &v, 2); }
inline void store_u32(uint8_t *p, uint32_t v) { memcpy(p, &v, 4); }
inline uint32_t bswap32(uint32_t v) { return (v >> 24) | ((v >> 8) & 0xff00u) | ((v << 8) & 0xff0000u) | (v << 24); }

void swap16_n(uint8_t *p, uint32_t n)
{
    for (uint32_t i = 0; i < n; i++, p += 2) store_u16(p, (uint16_t)((load_u16(p) >> 8) | (load_u16(p) << 8)));
}
void swap32_n(uint8_t *p, uint32_t n)
{
    for (uint32_t i = 0; i < n; i++, p += 4) store_u32(p, bswap32(load_u32(p)));
}
void swap64_n(uint8_t *p, uint32_t n)
{
    for (uint32_t i = 0; i < n; i++, p += 8) {
        const uint32_t lo = load_u32(p), hi = load_u32(p + 4);
        store_u32(p, bswap32(hi));
        store_u32(p + 4, bswap32(lo));
    }
}

// Reads a 32-bit value of a field being converted: before the swap when reading disk order, after it otherwise.
uint32_t swap32_and_read(uint8_t *p, int from_be)
{
    if (from_be) swap32_n(p, 1);
    const uint32_t value = load_u32(p);
    if (!from_be) swap32_n(p, 1);
    return value;
}

void record_error(s_port_resource_swap_stats *stats, uint32_t resource, uint32_t kind, uint32_t value)
{
    if (!stats) return;
    const bool layout = kind != k_port_resource_swap_error_bounds;
    if (stats->bounds_errors + stats->layout_errors == 0) {
        stats->first_error_resource = resource;
        stats->first_error_kind = kind;
        stats->first_error_value = value;
    }
    if (layout) stats->layout_errors++;
    else stats->bounds_errors++;
}

// ---- the structures paged data is dispatched on, found from the generated resource roots ----

struct s_known_structs {
    int initialized;
    uint32_t vertex_buffer;    // VertexBufferDefinition (s_render_vertex_buffer_descriptor, 0x1c)
    uint32_t index_buffer;     // IndexBufferDefinition (s_render_index_buffer_descriptor, 0x18)
    uint32_t animation_member; // ModelAnimationTagResource::GroupMember (0x30)
};
s_known_structs g_known = { 0, 0xffffffffu, 0xffffffffu, 0xffffffffu };

const s_tag_resource_swap_type *find_type_by_name(const char *name)
{
    const s_tag_swap_tables &t = tables();
    for (uint32_t i = 0; i < t.resource_type_count; i++) {
        const char *a = t.resource_types[i].name, *b = name;
        while (*a && *a == *b) a++, b++;
        if (*a == *b) return &t.resource_types[i];
    }
    return NULL;
}

// The element structure of the n-th tag block op of a structure, 0xffffffff if there is none.
uint32_t nth_block_element(uint32_t struct_idx, uint32_t n)
{
    const s_tag_swap_tables &t = tables();
    const s_tag_swap_struct &s = t.structs[struct_idx];
    for (uint32_t i = 0; i < s.op_count; i++) {
        const s_tag_swap_op &op = t.ops[s.first_op + i];
        if (op.kind == k_op_tag_block && n-- == 0) return op.arg;
    }
    return 0xffffffffu;
}

// The descriptor structure of a D3DStructure<T> (a structure holding one interop op).
uint32_t interop_descriptor(uint32_t struct_idx)
{
    const s_tag_swap_tables &t = tables();
    if (struct_idx >= t.struct_count) return 0xffffffffu;
    const s_tag_swap_struct &s = t.structs[struct_idx];
    for (uint32_t i = 0; i < s.op_count; i++) {
        const s_tag_swap_op &op = t.ops[s.first_op + i];
        if (op.kind == k_op_tag_interop) return op.arg;
    }
    return 0xffffffffu;
}

void initialize_known_structs()
{
    if (g_known.initialized) return;
    const s_tag_swap_tables &t = tables();
    // RenderGeometryApiResourceDefinition: Textures, InterleavedTextures, VertexBuffers, IndexBuffers, each a block of
    // D3DStructure<definition>.
    if (const s_tag_resource_swap_type *geometry = find_type_by_name("render_geometry_api_resource_definition")) {
        const uint32_t vb = interop_descriptor(nth_block_element(geometry->struct_idx, 2));
        const uint32_t ib = interop_descriptor(nth_block_element(geometry->struct_idx, 3));
        if (vb < t.struct_count && t.structs[vb].size == 0x1c) g_known.vertex_buffer = vb;
        if (ib < t.struct_count && t.structs[ib].size == 0x18) g_known.index_buffer = ib;
    }
    // ModelAnimationTagResource: GroupMembers.
    if (const s_tag_resource_swap_type *animation = find_type_by_name("model_animation_tag_resource")) {
        const uint32_t member = nth_block_element(animation->struct_idx, 0);
        if (member < t.struct_count && t.structs[member].size == 0x30) g_known.animation_member = member;
    }
    g_known.initialized = 1;
}

// ---- one resource ----

struct s_fixup {
    uint32_t offset; // in the control data
    uint32_t value;  // type << 29 | offset
};

void sort_fixups(s_fixup *f, uint32_t n)
{
    bool sorted = true;
    for (uint32_t i = 1; i < n && sorted; i++) sorted = f[i - 1].offset <= f[i].offset;
    if (sorted) return;
    // Shell sort (Ciura gaps): no C library qsort on the Windows build (data_crt.h).
    static const uint32_t k_gaps[] = { 701, 301, 132, 57, 23, 10, 4, 1 };
    for (uint32_t g = 0; g < sizeof(k_gaps) / sizeof(k_gaps[0]); g++) {
        const uint32_t gap = k_gaps[g];
        for (uint32_t i = gap; i < n; i++) {
            const s_fixup value = f[i];
            uint32_t j = i;
            for (; j >= gap && f[j - gap].offset > value.offset; j -= gap) f[j] = f[j - gap];
            f[j] = value;
        }
    }
}

// A paged buffer: [data, data + size), its visited bitmap indexed from visited_origin (several buffers may share a
// page and its bitmap).
struct s_paged_buffer {
    uint8_t *data;
    uint32_t size;
    uint8_t *visited;
    uint32_t visited_origin;
};

struct s_resource_walk {
    const s_tag_swap_tables *t;
    const uint8_t *control;
    uint32_t control_size;
    const s_fixup *fixups;
    uint32_t fixup_count;
    uint8_t *control_walked; // bitmap over the control data: arrays already walked
    s_paged_buffer paged[2]; // [0]: required data (type 2), [1]: optional data (type 3)
    int from_be;
    uint32_t resource_index;
    uint32_t depth;
    s_port_resource_swap_stats *stats;
};

inline int test_and_mark(uint8_t *bitmap, uint32_t bit)
{
    const uint8_t mask = (uint8_t)(1u << (bit & 7u));
    if (bitmap[bit >> 3] & mask) return 1;
    bitmap[bit >> 3] |= mask;
    return 0;
}

enum e_target { k_target_none, k_target_control, k_target_paged, k_target_absent, k_target_error };

// Where the reference at control field `field` points, for `length` bytes.
e_target resolve(s_resource_walk *w, const uint8_t *field, uint64_t length, uint32_t *out_which, uint32_t *out_offset,
                 bool required = true)
{
    const uint32_t offset = (uint32_t)(field - w->control);
    uint32_t lo = 0, hi = w->fixup_count;
    while (lo < hi) {
        const uint32_t mid = (lo + hi) >> 1;
        if (w->fixups[mid].offset < offset) lo = mid + 1;
        else hi = mid;
    }
    if (lo == w->fixup_count || w->fixups[lo].offset != offset) {
        if (!required) return k_target_none; // a null descriptor or pointer
        record_error(w->stats, w->resource_index, k_port_resource_swap_error_fixup, offset);
        return k_target_error;
    }
    const uint32_t type = w->fixups[lo].value >> 29, target = w->fixups[lo].value & 0x1fffffffu;
    *out_offset = target;
    if (type == 1) {
        if ((uint64_t)target + length > w->control_size) {
            record_error(w->stats, w->resource_index, k_port_resource_swap_error_bounds, target);
            return k_target_error;
        }
        return k_target_control;
    }
    if (type == 2 || type == 3) {
        const s_paged_buffer &buffer = w->paged[type - 2];
        *out_which = type - 2;
        if (!buffer.data) {
            if (w->stats) w->stats->absent_buffers++;
            return k_target_absent;
        }
        if ((uint64_t)target + length > buffer.size) {
            record_error(w->stats, w->resource_index, k_port_resource_swap_error_bounds, target);
            return k_target_error;
        }
        return k_target_paged;
    }
    record_error(w->stats, w->resource_index, k_port_resource_swap_error_fixup, w->fixups[lo].value);
    return k_target_error;
}

// Paged structures hold no references of their own (fixups only patch control data): every reference field must be
// empty. Its words are converted as plain values.
void swap_flat_struct(s_resource_walk *w, uint8_t *p, uint32_t struct_idx, uint32_t depth);

void swap_flat_array(s_resource_walk *w, uint8_t *p, uint32_t struct_idx, uint32_t count, uint32_t depth)
{
    const s_tag_swap_struct &s = w->t->structs[struct_idx];
    if (s.op_count == 1) {
        const s_tag_swap_op &op = w->t->ops[s.first_op];
        switch (op.kind) {
        case k_op_skip: return;
        case k_op_swap16: swap16_n(p, op.count * count); return;
        case k_op_swap32: swap32_n(p, op.count * count); return;
        case k_op_swap64: swap64_n(p, op.count * count); return;
        default: break;
        }
    }
    for (uint32_t i = 0; i < count; i++, p += s.size) swap_flat_struct(w, p, struct_idx, depth);
}

void nested_reference(s_resource_walk *w, uint32_t value)
{
    if (value) record_error(w->stats, w->resource_index, k_port_resource_swap_error_nested_reference, value);
}

void swap_flat_struct(s_resource_walk *w, uint8_t *p, uint32_t struct_idx, uint32_t depth)
{
    if (depth > 64) {
        record_error(w->stats, w->resource_index, k_port_resource_swap_error_nested_reference, struct_idx);
        return;
    }
    uint8_t *const base = p;
    const s_tag_swap_struct &s = w->t->structs[struct_idx];
    for (uint32_t i = 0; i < s.op_count; i++) {
        const s_tag_swap_op &op = w->t->ops[s.first_op + i];
        switch (op.kind) {
        case k_op_skip: p += op.count; break;
        case k_op_swap16: swap16_n(p, op.count); p += op.count * 2u; break;
        case k_op_swap32: swap32_n(p, op.count); p += op.count * 4u; break;
        case k_op_swap64: swap64_n(p, op.count); p += op.count * 8u; break;
        case k_op_tag_ref: swap32_n(p, 4u * op.count); p += 16u * op.count; break;
        case k_op_tag_data:
            nested_reference(w, swap32_and_read(p, w->from_be)); // size
            swap32_n(p + 4, 4);
            p += 20;
            break;
        case k_op_tag_block:
            nested_reference(w, swap32_and_read(p, w->from_be)); // count
            swap32_n(p + 4, 2);
            p += 12;
            break;
        case k_op_tag_interop:
            nested_reference(w, swap32_and_read(p, w->from_be)); // descriptor
            swap32_n(p + 4, 2);
            p += 12;
            break;
        case k_op_tag_pointer:
            nested_reference(w, swap32_and_read(p, w->from_be));
            p += 4;
            break;
        case k_op_hs_syntax_data: {
            // hs_syntax_node: constant type (+2) and flags (+6), already converted by this instance's earlier ops
            uint16_t constant_type = load_u16(base + 2), flags = load_u16(base + 6);
            if (!w->from_be) {
                constant_type = (uint16_t)(constant_type >> 8 | constant_type << 8);
                flags = (uint16_t)(flags >> 8 | flags << 8);
            }
            const uint32_t width = hs_syntax_data_width(constant_type, flags);
            if (width == 4) swap32_n(p, 1);
            else if (width == 2) swap16_n(p, 1);
            p += 4;
            break;
        }
        case k_op_inline_struct:
            for (uint32_t k = 0; k < op.count; k++, p += w->t->structs[op.arg].size)
                swap_flat_struct(w, p, op.arg, depth + 1);
            break;
        }
    }
}

uint8_t *claim_paged(s_resource_walk *w, uint32_t which, uint32_t offset)
{
    s_paged_buffer &buffer = w->paged[which];
    if (test_and_mark(buffer.visited, buffer.visited_origin + offset)) return NULL;
    return buffer.data + offset;
}

void convert_paged_data(s_resource_walk *w, const uint8_t *owner, uint32_t owner_idx, uint32_t kind, uint8_t *data,
                        uint32_t size)
{
    s_port_resource_swap_stats *stats = w->stats;
    if (kind == 1) {
        swap_function_blob(data, size, w->from_be);
        if (stats) stats->functions++;
    } else if (kind == 2) {
        // serialized hkMoppCodes back to back: 0x30-byte headers, code bytes, next at (m_data.m_size + 63) & ~15
        // (cache_swap.cpp swap_mopp_code_sequence)
        const uint32_t mopp = mopp_code_struct_index();
        for (uint32_t offset = 0; mopp != 0xffffffffu && offset + 0x30u <= size;) {
            uint8_t *header = data + offset;
            if (w->from_be) swap_flat_struct(w, header, mopp, 0);
            const uint32_t code_bytes = load_u32(header + 0x24);
            if (!w->from_be) swap_flat_struct(w, header, mopp, 0);
            if (stats) stats->mopp_codes++;
            const uint32_t step = (code_bytes + 63u) & ~15u;
            if (step < 0x30u || step > size - offset) break;
            offset += step;
        }
    } else if (owner_idx == g_known.vertex_buffer) {
        // s_render_vertex_buffer_descriptor: vertex_count, declaration, stride, vertices
        const uint32_t count = load_u32(owner), declaration = load_u16(owner + 4), stride = load_u16(owner + 6);
        const uint32_t result = port_geometry_swap_vertices(declaration, stride, count, data, size);
        if (result != k_port_geometry_ok)
            record_error(stats, w->resource_index, k_port_resource_swap_error_vertex_declaration,
                         declaration | result << 16);
        else if (stats) {
            stats->vertex_buffers++;
            stats->vertices += count;
        }
    } else if (owner_idx == g_known.index_buffer) {
        // s_render_index_buffer_descriptor: 16-bit indices (tag_d3d_index_buffer_fixup: D3DFMT_INDEX16, size >> 1)
        if (size & 1u) record_error(stats, w->resource_index, k_port_resource_swap_error_index_size, size);
        port_geometry_swap_indices(2, size >> 1, data);
        if (stats) {
            stats->index_buffers++;
            stats->indices += size >> 1;
        }
    } else if (owner_idx == g_known.animation_member) {
        const uint32_t errors = stats ? stats->bounds_errors + stats->layout_errors : 0;
        if (!port_resource_swap_animation_data(owner, data, size, w->from_be, stats) && stats && errors == 0)
            stats->first_error_resource = w->resource_index; // the member recorded the error without it
    } else if (stats) {
        stats->opaque_data++;
        stats->opaque_bytes += size;
    }
}

void walk_control_struct(s_resource_walk *w, const uint8_t *base, uint32_t struct_idx);

void walk_control_array(s_resource_walk *w, const uint8_t *base, uint32_t struct_idx, uint32_t count)
{
    const s_tag_swap_struct &s = w->t->structs[struct_idx];
    for (uint32_t i = 0; i < count; i++) walk_control_struct(w, base + (size_t)i * s.size, struct_idx);
}

void walk_control_struct(s_resource_walk *w, const uint8_t *base, uint32_t struct_idx)
{
    if (w->depth >= 64) {
        record_error(w->stats, w->resource_index, k_port_resource_swap_error_nested_reference, struct_idx);
        return;
    }
    w->depth++;
    const s_tag_swap_struct &s = w->t->structs[struct_idx];
    const uint8_t *p = base;
    for (uint32_t i = 0; i < s.op_count; i++) {
        const s_tag_swap_op &op = w->t->ops[s.first_op + i];
        uint32_t which = 0, offset = 0;
        switch (op.kind) {
        case k_op_skip: p += op.count; break;
        case k_op_swap16: p += op.count * 2u; break;
        case k_op_swap32: p += op.count * 4u; break;
        case k_op_swap64: p += op.count * 8u; break;
        case k_op_tag_ref: p += 16u * op.count; break;
        case k_op_hs_syntax_data: p += 4; break;
        case k_op_tag_data: {
            // s_tag_data: size (+0), flags, stream position, address (+12), definition
            const int32_t size = (int32_t)load_u32(p);
            if (size > 0) {
                const e_target target = resolve(w, p + 12, (uint32_t)size, &which, &offset);
                if (target == k_target_paged) {
                    if (uint8_t *data = claim_paged(w, which, offset))
                        convert_paged_data(w, base, struct_idx, op.arg, data, (uint32_t)size);
                }
                // control data (type 1) was converted with the tag section
            }
            p += 20;
            break;
        }
        case k_op_tag_block: {
            // s_tag_block: count (+0), address (+4), definition
            const int32_t count = (int32_t)load_u32(p);
            const uint32_t element_size = w->t->structs[op.arg].size;
            if (count > 0 && element_size) {
                const uint64_t length = (uint64_t)(uint32_t)count * element_size;
                const e_target target = resolve(w, p + 4, length, &which, &offset);
                if (target == k_target_control) {
                    if (!test_and_mark(w->control_walked, offset))
                        walk_control_array(w, w->control + offset, op.arg, (uint32_t)count);
                } else if (target == k_target_paged) {
                    if (uint8_t *data = claim_paged(w, which, offset)) {
                        swap_flat_array(w, data, op.arg, (uint32_t)count, 0);
                        if (w->stats) {
                            w->stats->paged_blocks++;
                            w->stats->paged_elements += (uint32_t)count;
                        }
                    }
                }
            } else if (count < 0) {
                record_error(w->stats, w->resource_index, k_port_resource_swap_error_bounds, (uint32_t)count);
            }
            p += 12;
            break;
        }
        case k_op_tag_interop:
        case k_op_tag_pointer: {
            // D3DStructure<T>: descriptor (+0), runtime address, definition address; or a plain pointer. The field
            // itself holds nothing before the fixups run: a reference exists exactly when a fixup names it.
            const uint32_t size = w->t->structs[op.arg].size;
            if (size) {
                const e_target target = resolve(w, p, size, &which, &offset, false);
                if (target == k_target_control) {
                    if (!test_and_mark(w->control_walked, offset)) walk_control_struct(w, w->control + offset, op.arg);
                } else if (target == k_target_paged) {
                    if (uint8_t *data = claim_paged(w, which, offset)) swap_flat_struct(w, data, op.arg, 0);
                }
            }
            p += op.kind == k_op_tag_pointer ? 4 : 12;
            break;
        }
        case k_op_inline_struct:
            for (uint32_t k = 0; k < op.count; k++, p += w->t->structs[op.arg].size) walk_control_struct(w, p, op.arg);
            break;
        }
    }
    w->depth--;
}

// ---- zone access ----

const uint8_t *view_at(const s_port_resource_zone_view *view, uint32_t address, uint64_t length)
{
    const uint32_t offset = address - view->tag_base_address;
    if (address < view->tag_base_address || (uint64_t)offset + length > view->tag_size) return NULL;
    return view->tag_buffer + offset;
}

// zone->resources[index] (s_cache_file_tag_resource_data, 0x40 bytes).
const uint8_t *zone_resource(const s_port_resource_zone_view *view, uint32_t index)
{
    const uint32_t count = load_u32(view->zone + 0x58);
    if (index >= count) return NULL;
    return view_at(view, load_u32(view->zone + 0x5c) + index * 0x40u, 0x40);
}

int swap_resource(const s_port_resource_zone_view *view, uint32_t resource_index, const s_paged_buffer paged[2],
                  int from_be, s_port_resource_swap_stats *stats)
{
    initialize_known_structs();
    const uint8_t *entry = zone_resource(view, resource_index);
    if (!entry) {
        record_error(stats, resource_index, k_port_resource_swap_error_bounds, resource_index);
        return 0;
    }
    const int32_t type_index = (int8_t)entry[0x12];
    const int32_t data_offset = (int32_t)load_u32(entry + 0x14), data_size = (int32_t)load_u32(entry + 0x18);
    if (type_index < 0 || data_size <= 0) return 1; // no control data (sound): its pages are plain data
    const uint32_t before = stats ? stats->bounds_errors + stats->layout_errors : 0;

    const uint8_t *type = (uint32_t)type_index < load_u32(view->zone + 0x04)
                              ? view_at(view, load_u32(view->zone + 0x08) + (uint32_t)type_index * 0x1cu, 0x1c)
                              : NULL;
    const s_tag_resource_swap_type *swap_type = type ? find_resource_type(type) : NULL;
    if (!swap_type) {
        record_error(stats, resource_index, k_port_resource_swap_error_unknown_type, type ? load_u32(type) : 0);
        return 0;
    }
    // control data: zone->fixup_information (size +0x13c, address +0x148) + data_offset
    const uint32_t fixup_information_size = load_u32(view->zone + 0x13c);
    const uint8_t *fixup_information = view_at(view, load_u32(view->zone + 0x148), fixup_information_size);
    const uint32_t root = load_u32(entry + 0x24);
    if (!fixup_information || data_offset < 0 ||
        (uint64_t)(uint32_t)data_offset + (uint32_t)data_size > fixup_information_size || (root >> 29) != 1 ||
        (uint64_t)(root & 0x1fffffffu) + tables().structs[swap_type->struct_idx].size > (uint32_t)data_size) {
        record_error(stats, resource_index, k_port_resource_swap_error_bounds, (uint32_t)data_offset);
        return 0;
    }

    const uint32_t fixup_count = load_u32(entry + 0x28);
    const uint8_t *fixup_data = fixup_count ? view_at(view, load_u32(entry + 0x2c), (uint64_t)fixup_count * 8u) : NULL;
    if (fixup_count && !fixup_data) {
        record_error(stats, resource_index, k_port_resource_swap_error_bounds, load_u32(entry + 0x2c));
        return 0;
    }
    s_fixup *fixups = fixup_count ? (s_fixup *)malloc(fixup_count * sizeof(s_fixup)) : NULL;
    uint8_t *control_walked = (uint8_t *)calloc(((uint32_t)data_size + 7u) >> 3, 1);
    if ((fixup_count && !fixups) || !control_walked) {
        free(fixups);
        free(control_walked);
        record_error(stats, resource_index, k_port_resource_swap_error_bounds, (uint32_t)data_size);
        return 0;
    }
    memcpy(fixups, fixup_data, fixup_count * sizeof(s_fixup));
    sort_fixups(fixups, fixup_count);

    s_resource_walk w;
    w.t = &tables();
    w.control = fixup_information + data_offset;
    w.control_size = (uint32_t)data_size;
    w.fixups = fixups;
    w.fixup_count = fixup_count;
    w.control_walked = control_walked;
    w.paged[0] = paged[0];
    w.paged[1] = paged[1];
    w.from_be = from_be;
    w.resource_index = resource_index;
    w.depth = 0;
    w.stats = stats;
    walk_control_struct(&w, w.control + (root & 0x1fffffffu), swap_type->struct_idx);
    if (stats) stats->resources++;

    free(fixups);
    free(control_walked);
    return !stats || stats->bounds_errors + stats->layout_errors == before;
}

// ---- the page -> resource index for port_resource_swap_page ----

struct s_page_entry {
    uint32_t page;
    uint32_t resource;
    uint32_t which; // 0: the resource's primary (required) page, 1: its secondary (optional) page
};

struct s_page_index {
    s_port_resource_zone_view view;
    uint32_t count;
    s_page_entry *entries; // sorted by page, resource, which

    void invalidate(const void *tag_buffer)
    {
        if (view.tag_buffer == tag_buffer) {
            free(entries);
            *this = {};
        }
    }
};
// The game publishes one tag buffer after conversion and drains its readers before reuse.
s_page_index g_loaded_index = {};
uint32_t g_loaded_tags_header_va;
s_page_index g_fallback_index = {};
uint64_t g_fallback_fingerprint;
int g_pages_swapped = 0;

int compare_page_entries(const void *left, const void *right)
{
    const s_page_entry &a = *(const s_page_entry *)left, &b = *(const s_page_entry *)right;
    if (a.page != b.page) return a.page < b.page ? -1 : 1;
    if (a.resource != b.resource) return a.resource < b.resource ? -1 : 1;
    return (int)a.which - (int)b.which;
}

// A resource's segment (layout_table.segments, 0x10 bytes): primary page, secondary page, primary offset, secondary
// offset, primary size index, secondary size index.
const uint8_t *resource_segment(const s_port_resource_zone_view *view, const uint8_t *entry)
{
    const int32_t segment = (int16_t)load_u16(entry + 0x22);
    if (segment < 0 || !view->layout || (uint32_t)segment >= load_u32(view->layout + 0x30)) return NULL;
    return view_at(view, load_u32(view->layout + 0x34) + (uint32_t)segment * 0x10u, 0x10);
}

// What the index is built from: each resource's flags, size and segment, and the segments' pages. Every map's tags
// load at the same base address, so the next map's zone tag can sit where the last one's did (main menu -> Arrival):
// the address alone does not tell the maps apart, and a stale index converts a page by another map's resources.
uint64_t page_index_fingerprint(const s_port_resource_zone_view *view)
{
    uint64_t hash = 0xcbf29ce484222325ull;
    const auto mix = [&hash](uint32_t value) {
        for (int b = 0; b < 4; b++) {
            hash ^= (value >> (b * 8)) & 0xffu;
            hash *= 0x100000001b3ull;
        }
    };
    const uint32_t resource_count = load_u32(view->zone + 0x58);
    mix(resource_count);
    for (uint32_t r = 0; r < resource_count; r++) {
        const uint8_t *entry = zone_resource(view, r);
        if (!entry) { mix(0xffffffffu); continue; }
        mix(entry[0x12]);
        mix(load_u32(entry + 0x18));
        const uint8_t *segment = resource_segment(view, entry);
        mix(segment ? load_u32(segment) : 0xffffffffu); // primary and secondary page
    }
    return hash;
}

bool same_view(const s_port_resource_zone_view &a, const s_port_resource_zone_view &b)
{
    return a.tag_buffer == b.tag_buffer && a.tag_base_address == b.tag_base_address && a.tag_size == b.tag_size &&
           a.zone == b.zone && a.layout == b.layout;
}

bool build_page_index(const s_port_resource_zone_view *view, s_page_index *index)
{
    const uint32_t resource_count = load_u32(view->zone + 0x58);
    s_page_entry *entries = (s_page_entry *)malloc((size_t)resource_count * 2u * sizeof(s_page_entry) + 1u);
    if (!entries) return false;
    uint32_t count = 0;
    for (uint32_t r = 0; r < resource_count; r++) {
        const uint8_t *entry = zone_resource(view, r);
        const uint8_t *segment = entry ? resource_segment(view, entry) : NULL;
        if (!segment || (int8_t)entry[0x12] < 0 || (int32_t)load_u32(entry + 0x18) <= 0) continue;
        const int32_t primary = (int16_t)load_u16(segment), secondary = (int16_t)load_u16(segment + 2);
        if (primary >= 0) entries[count++] = s_page_entry{ (uint32_t)primary, r, 0 };
        if (secondary >= 0) entries[count++] = s_page_entry{ (uint32_t)secondary, r, 1 };
    }
    qsort(entries, count, sizeof(*entries), compare_page_entries);
    free(index->entries);
    *index = { *view, count, entries };
    return true;
}

const s_page_index *page_index_for_view(const s_port_resource_zone_view *view)
{
    if (same_view(g_loaded_index.view, *view)) return &g_loaded_index;
    const uint64_t fingerprint = page_index_fingerprint(view);
    if (fingerprint != g_fallback_fingerprint || !same_view(g_fallback_index.view, *view)) {
        if (!build_page_index(view, &g_fallback_index)) return NULL;
        g_fallback_fingerprint = fingerprint;
    }
    return &g_fallback_index;
}

} // namespace

// ---- animation data ----

namespace {

struct s_animation_section {
    uint8_t *data;
    uint32_t size;
    int from_be;
    s_port_resource_swap_stats *stats;
    int ok;
};

// Converts `count` units of `unit` bytes at section offset `offset`.
void section_swap(s_animation_section *s, uint32_t offset, uint64_t count, uint32_t unit)
{
    const uint64_t bytes = count * unit;
    if ((uint64_t)offset + bytes > s->size) {
        record_error(s->stats, 0xffffffffu, k_port_resource_swap_error_bounds, offset);
        s->ok = 0;
        return;
    }
    if (unit == 2) swap16_n(s->data + offset, (uint32_t)count);
    else if (unit == 4) swap32_n(s->data + offset, (uint32_t)count);
}

// Reads the 32-bit header word at `offset` of a section being converted (its words are converted in one pass).
uint32_t section_word(const s_animation_section *s, uint32_t offset)
{
    const uint32_t value = load_u32(s->data + offset);
    return s->from_be ? bswap32(value) : value;
}

// The largest first + count of a keyframe channel table (count in bits 0..11, first key in bits 12..31), read before
// the table is converted. Zero (and an error) if the table is out of bounds.
uint32_t keyframe_total(s_animation_section *s, uint32_t offset, uint32_t channels)
{
    if ((uint64_t)offset + (uint64_t)channels * 4u > s->size) {
        record_error(s->stats, 0xffffffffu, k_port_resource_swap_error_bounds, offset);
        s->ok = 0;
        return 0;
    }
    uint32_t total = 0;
    for (uint32_t c = 0; c < channels; c++) {
        const uint32_t info = section_word(s, offset + c * 4u);
        const uint32_t end = (info >> 12) + (info & 0xfffu);
        if (end > total) total = end;
    }
    return total;
}

/*
 * One codec section (tu/blamlibxbox/animation_codecs.cpp, animation_codec_types.h). The 12-byte
 * s_animation_codec_header (codec type, rotation / translation / scale node counts: bytes; error percentage and
 * playback rate: floats) is followed by:
 *   1 uncompressed static, 2 uncompressed animated, 3 8-byte quantized rotations, 8 blend screen
 *     (s_fullframe_codec_header / s_static_codec_header): translation offset, scale offset, rotation / translation /
 *     scale strides; rotations from +0x20: four 16-bit components per key (1, 3) or a float quaternion (2, 8);
 *     translations three floats, scales one float. Static data holds one key per node, the others frame_count.
 *   4..7 byte / word keyframes, plain and reverse (s_keyframe_codec_header): eight offsets, 4 bytes of padding, the
 *     rotation channel table from +0x30 and the translation and scale tables at their offsets (one word per node:
 *     key count | first key << 12); key times of 8 bits (4, 6) or 16 (5, 7); rotation keys of four 16-bit
 *     components, translation keys of three floats, scale keys one float.
 */
void swap_codec(s_animation_section *s)
{
    if (s->size < 12) {
        record_error(s->stats, 0xffffffffu, k_port_resource_swap_error_codec, s->size);
        s->ok = 0;
        return;
    }
    const uint32_t codec = s->data[0];
    const uint32_t rotations = s->data[1], translations = s->data[2], scales = s->data[3];
    swap32_n(s->data + 4, 2); // error percentage, playback rate
    switch (codec) {
    case 1: case 2: case 3: case 8: {
        if (s->size < 0x20) break;
        const uint32_t translation_offset = section_word(s, 0x0c), scale_offset = section_word(s, 0x10);
        uint32_t rotation_stride = section_word(s, 0x14), translation_stride = section_word(s, 0x18),
                 scale_stride = section_word(s, 0x1c);
        swap32_n(s->data + 0x0c, 5);
        const uint32_t rotation_size = codec == 1 || codec == 3 ? 8u : 16u;
        if (codec == 1) { // one key per node: the strides are unused
            rotation_stride = rotation_size;
            translation_stride = 12;
            scale_stride = 4;
        }
        if (rotation_size == 8) section_swap(s, 0x20, (uint64_t)rotations * rotation_stride / 2u, 2);
        else section_swap(s, 0x20, (uint64_t)rotations * rotation_stride / 4u, 4);
        section_swap(s, translation_offset, (uint64_t)translations * translation_stride / 4u, 4);
        section_swap(s, scale_offset, (uint64_t)scales * scale_stride / 4u, 4);
        if (s->stats) s->stats->animation_codecs++;
        return;
    }
    case 4: case 5: case 6: case 7: {
        if (s->size < 0x30 + rotations * 4u) break;
        uint32_t offsets[8];
        for (uint32_t i = 0; i < 8; i++) offsets[i] = section_word(s, 0x0c + i * 4u);
        swap32_n(s->data + 0x0c, 8); // the offsets; +0x2c is padding
        // offsets: translation channels, scale channels, rotation / translation / scale times, rotation /
        // translation / scale values
        const uint32_t rotation_keys = keyframe_total(s, 0x30, rotations);
        const uint32_t translation_keys = keyframe_total(s, offsets[0], translations);
        const uint32_t scale_keys = keyframe_total(s, offsets[1], scales);
        if (!s->ok) return;
        section_swap(s, 0x30, rotations, 4);
        section_swap(s, offsets[0], translations, 4);
        section_swap(s, offsets[1], scales, 4);
        const uint32_t time_size = codec == 4 || codec == 6 ? 1u : 2u;
        section_swap(s, offsets[2], rotation_keys, time_size);
        section_swap(s, offsets[3], translation_keys, time_size);
        section_swap(s, offsets[4], scale_keys, time_size);
        section_swap(s, offsets[5], (uint64_t)rotation_keys * 4u, 2);
        section_swap(s, offsets[6], (uint64_t)translation_keys * 3u, 4);
        section_swap(s, offsets[7], scale_keys, 4);
        if (s->stats) s->stats->animation_codecs++;
        return;
    }
    default:
        break;
    }
    record_error(s->stats, 0xffffffffu, k_port_resource_swap_error_codec, codec | s->size << 8);
    s->ok = 0;
}

} // namespace

extern "C" int port_resource_swap_animation_data(const void *member_, void *data_, uint32_t size, int from_be,
                                                 s_port_resource_swap_stats *stats)
{
    // ModelAnimationTagResource::GroupMember (0x30): name, checksum, frame count (+8), node count (+0xa), movement data
    // type (+0xb), c_animation_data_sizes (+0xc: static node flags, animated node flags (bytes), movement, pill
    // offset, default (static) data (shorts), uncompressed, compressed data (longs)), animation data (+0x1c).
    const uint8_t *member = (const uint8_t *)member_;
    uint8_t *data = (uint8_t *)data_;
    const uint32_t static_flags = member[0x0c], animated_flags = member[0x0d];
    const uint32_t movement = load_u16(member + 0x0e), pill = load_u16(member + 0x10);
    const uint32_t default_size = load_u16(member + 0x12);
    const uint32_t uncompressed = load_u32(member + 0x14), compressed = load_u32(member + 0x18);
    const uint64_t total = (uint64_t)default_size + compressed + static_flags + animated_flags + movement + pill +
                           uncompressed;
    if (stats) stats->animation_members++;
    if (total > size || uncompressed != 0 || ((static_flags | animated_flags | movement | pill) & 3u)) {
        record_error(stats, 0xffffffffu, k_port_resource_swap_error_animation_sizes, (uint32_t)total);
        return 0;
    }
    int ok = 1;
    // c_animation_data_sizes order: default (static) codec data, compressed (animated) codec data, static node
    // flags, animated node flags, movement data, pill offset data
    uint32_t offset = 0;
    const uint32_t codec_sizes[2] = { default_size, compressed };
    for (uint32_t c = 0; c < 2; c++) {
        if (codec_sizes[c]) {
            s_animation_section section = { data + offset, codec_sizes[c], from_be, stats, 1 };
            swap_codec(&section);
            ok &= section.ok;
        }
        offset += codec_sizes[c];
    }
    // node flags (32-bit words, three components), movement data (floats per frame: dx dy [dz] [dyaw]), pill offsets
    // (a real_point3d per frame): all 32-bit
    swap32_n(data + offset, (static_flags + animated_flags + movement + pill) / 4u);
    return ok;
}

extern "C" int PORT_MS_ABI port_resource_zone_view_init(const void *swapped_header, const void *tag_buffer,
                                            s_port_resource_zone_view *out)
{
    const uint8_t *header = (const uint8_t *)swapped_header;
    memset(out, 0, sizeof(*out));
    out->tag_buffer = (const uint8_t *)tag_buffer;
    out->tag_base_address = load_u32(header + 0x2e8); // tag_memory_base
    out->tag_size = load_u32(header + 0x18);          // total_tags_size
    const uint32_t tags_header_va = load_u32(header + 0x10);
    const s_port_resource_zone_view &loaded = g_loaded_index.view;
    if (loaded.tag_buffer == tag_buffer && loaded.tag_base_address == out->tag_base_address &&
        loaded.tag_size == out->tag_size && g_loaded_tags_header_va == tags_header_va && loaded.zone) {
        *out = loaded;
        return 1;
    }
    // tags header (s_cache_file_header::tags_header): group count / address, instance count / address, global count /
    // address
    const uint8_t *tags = view_at(out, tags_header_va, 0x18);
    if (!tags) return 0;
    const uint32_t group_count = load_u32(tags + 0x00), instance_count = load_u32(tags + 0x08);
    const uint8_t *groups = view_at(out, load_u32(tags + 0x04), (uint64_t)group_count * 16u);
    const uint8_t *instances = view_at(out, load_u32(tags + 0x0c), (uint64_t)instance_count * 8u);
    if (!groups || !instances) return 0;
    for (uint32_t i = 0; i < instance_count && !(out->zone && out->layout); i++) {
        const int32_t group = (int16_t)load_u16(instances + i * 8u);
        if (group < 0 || (uint32_t)group >= group_count) continue;
        const uint32_t fourcc = load_u32(groups + (uint32_t)group * 16u);
        const uint32_t address = load_u32(instances + i * 8u + 4u);
        if (fourcc == 0x7a6f6e65u && !out->zone) out->zone = view_at(out, address, 0x214);       // 'zone'
        else if (fourcc == 0x706c6179u && !out->layout) out->layout = view_at(out, address, 0x3c); // 'play'
    }
    if (!out->zone) return 0;
    // the layout table is the 'play' tag's in retail maps; zone's own copy (+0x1c) is empty there
    if (!out->layout || load_u32(out->layout + 0x18) == 0) out->layout = out->zone + 0x1c;
    return 1;
}

// A conversion starts a new generation even when the map, tag base and zone address are identical.
void port_resource_invalidate_page_index(const void *tag_buffer)
{
    g_loaded_index.invalidate(tag_buffer);
    g_fallback_index.invalidate(tag_buffer);
}

void port_resource_register_page_index(const void *header, const void *tag_buffer)
{
    s_port_resource_zone_view view;
    if (!port_resource_zone_view_init(header, tag_buffer, &view)) return;
    const uint32_t resources = load_u32(view.zone + 0x58);
    if (resources && !view_at(&view, load_u32(view.zone + 0x5c), (uint64_t)resources * 0x40u)) return;
    if (build_page_index(&view, &g_loaded_index))
        g_loaded_tags_header_va = load_u32((const uint8_t *)header + 0x10);
}

extern "C" int port_resource_swap_paged_data(const s_port_resource_zone_view *view, uint32_t resource_index,
                                             void *cache_data, uint32_t cache_size, void *optional_data,
                                             uint32_t optional_size, int from_be, s_port_resource_swap_stats *stats)
{
    s_paged_buffer paged[2] = {
        { (uint8_t *)cache_data, cache_size, NULL, 0 },
        { (uint8_t *)optional_data, optional_size, NULL, 0 },
    };
    for (int i = 0; i < 2; i++) {
        if (!paged[i].data) continue;
        paged[i].visited = (uint8_t *)calloc(((size_t)paged[i].size + 8u) >> 3, 1);
        if (!paged[i].visited) {
            free(paged[0].visited);
            record_error(stats, resource_index, k_port_resource_swap_error_bounds, paged[i].size);
            return 0;
        }
    }
    const int result = swap_resource(view, resource_index, paged, from_be, stats);
    free(paged[0].visited);
    free(paged[1].visited);
    return result;
}

extern "C" int PORT_MS_ABI port_resource_swap_page(const s_port_resource_zone_view *view, uint32_t page_index, void *page_data,
                                       uint32_t page_size, int from_be, s_port_resource_swap_stats *stats)
{
    g_pages_swapped = 1;
    const s_page_index *index = page_index_for_view(view);
    if (!index) return 0;
    // first entry of this page
    uint32_t lo = 0, hi = index->count;
    while (lo < hi) {
        const uint32_t mid = (lo + hi) >> 1;
        if (index->entries[mid].page < page_index) lo = mid + 1;
        else hi = mid;
    }
    if (lo == index->count || index->entries[lo].page != page_index) return 1;
    uint8_t *visited = (uint8_t *)calloc(((size_t)page_size + 8u) >> 3, 1);
    if (!visited) return 0;
    int ok = 1;
    uint8_t *page = (uint8_t *)page_data;
    for (uint32_t i = lo; i < index->count && index->entries[i].page == page_index;) {
        const uint32_t resource = index->entries[i].resource;
        s_paged_buffer paged[2] = { { NULL, 0, visited, 0 }, { NULL, 0, visited, 0 } };
        const uint8_t *segment = resource_segment(view, zone_resource(view, resource));
        for (; i < index->count && index->entries[i].page == page_index &&
               index->entries[i].resource == resource;
             i++) {
            const uint32_t which = index->entries[i].which;
            const int32_t offset = (int32_t)load_u32(segment + 4u + which * 4u);
            if (offset < 0 || (uint32_t)offset > page_size) {
                record_error(stats, resource, k_port_resource_swap_error_bounds, (uint32_t)offset);
                ok = 0;
                continue;
            }
            paged[which].data = page + offset;
            paged[which].size = page_size - (uint32_t)offset;
            paged[which].visited_origin = (uint32_t)offset;
        }
        ok &= swap_resource(view, resource, paged, from_be, stats);
    }
    free(visited);
    // what a later map's page table takes over need not be converted again (port_resource_page_is_host_order)
    if (from_be) port_resource_page_mark_host_order(page_data, page_size);
    return ok;
}

extern "C" int PORT_MS_ABI port_resource_paged_data_is_host_order(void)
{
    return g_pages_swapped;
}

// ---- pages already in host order ----
//
// A page is converted once, when it becomes ready. One the loader keeps resident across a map change (precached
// during the lobby countdown, or shared with the last map) is registered again by the next map's page table, and
// its hash cannot always tell the two orders apart (the hash of the 8 MB first page of 005_intro matches it in
// either order). So each conversion records the converted bytes by a 64-bit content hash, and a page whose bytes
// hash to a recorded value is known to be in host order already. Content, not address: the page table moves pages
// when it compacts its storage, and a recycled address may hold a fresh read.

namespace {

struct s_host_order_pages {
    uint64_t *slots; // open addressing; 0: empty
    uint32_t capacity, count;
    volatile long lock;
};
s_host_order_pages g_host_order_pages = { NULL, 0, 0, 0 };

uint64_t page_content_hash(const void *data, uint32_t size)
{
    const uint8_t *p = (const uint8_t *)data;
    uint64_t hash = 0x9e3779b97f4a7c15ull ^ size;
    uint32_t i = 0;
    for (; i + 8u <= size; i += 8u) {
        uint64_t word;
        memcpy(&word, p + i, 8);
        hash = (hash ^ word) * 0xff51afd7ed558ccdull;
        hash ^= hash >> 32;
    }
    for (; i < size; i++)
        hash = (hash ^ p[i]) * 0x100000001b3ull;
    hash ^= hash >> 29;
    return hash != 0 ? hash : 1u;
}

void host_order_lock()
{
    while (__atomic_exchange_n(&g_host_order_pages.lock, 1, __ATOMIC_ACQUIRE) != 0) {
    }
}

void host_order_unlock()
{
    __atomic_store_n(&g_host_order_pages.lock, 0, __ATOMIC_RELEASE);
}

bool host_order_insert(uint64_t *slots, uint32_t capacity, uint64_t hash)
{
    for (uint32_t i = (uint32_t)hash & (capacity - 1u);; i = (i + 1u) & (capacity - 1u)) {
        if (slots[i] == hash) return false;
        if (slots[i] == 0) { slots[i] = hash; return true; }
    }
}

} // namespace

extern "C" void port_resource_page_mark_host_order(const void *page_data, uint32_t page_size)
{
    if (!page_data || page_size == 0) return;
    const uint64_t hash = page_content_hash(page_data, page_size);
    host_order_lock();
    s_host_order_pages &set = g_host_order_pages;
    if ((set.count + 1u) * 2u > set.capacity) {
        const uint32_t capacity = set.capacity ? set.capacity * 2u : 4096u;
        uint64_t *slots = (uint64_t *)calloc(capacity, sizeof(uint64_t));
        if (!slots) { host_order_unlock(); return; }
        for (uint32_t i = 0; i < set.capacity; i++)
            if (set.slots[i]) host_order_insert(slots, capacity, set.slots[i]);
        free(set.slots);
        set.slots = slots;
        set.capacity = capacity;
    }
    if (host_order_insert(set.slots, set.capacity, hash)) set.count++;
    host_order_unlock();
}

extern "C" int PORT_MS_ABI port_resource_page_is_host_order(const void *page_data, uint32_t page_size)
{
    if (!page_data || page_size == 0) return 0;
    const uint64_t hash = page_content_hash(page_data, page_size);
    host_order_lock();
    const s_host_order_pages &set = g_host_order_pages;
    int found = 0;
    if (set.capacity) {
        for (uint32_t i = (uint32_t)hash & (set.capacity - 1u); set.slots[i] != 0; i = (i + 1u) & (set.capacity - 1u))
            if (set.slots[i] == hash) { found = 1; break; }
    }
    host_order_unlock();
    return found;
}
