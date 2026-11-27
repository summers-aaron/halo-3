#ifndef PORT_DATA_CACHE_SWAP_INTERNAL_H
#define PORT_DATA_CACHE_SWAP_INTERNAL_H

/*
 * Interface inside port/data between cache_swap.cpp, which owns the layout tables port/tools/mapswap generates
 * (cache_swap_tables.inc, compiled once), and the other byte-order converters that walk the same layouts
 * (resource_swap.cpp for paged resource data, tmp/ tests). Not for game code: the game calls cache_swap.h and
 * resource_swap.h.
 *
 * A structure is a list of ops (s_tag_swap_struct::first_op .. + op_count) over its bytes in order; see the op kinds.
 */

#include <stddef.h>
#include <stdint.h>

namespace port_cache_swap {

enum e_tag_swap_op_kind : uint8_t {
    k_op_skip          = 1,  /* count bytes left as they are */
    k_op_swap16        = 2,  /* count 16-bit values */
    k_op_swap32        = 3,  /* count 32-bit values */
    k_op_swap64        = 4,  /* count 64-bit values */
    k_op_tag_ref       = 5,  /* count s_tag_reference (16 bytes: group, name, name_length, index) */
    k_op_tag_data      = 6,  /* s_tag_data (20 bytes); arg=0: plain byte[], arg=1: c_function_definition */
    k_op_tag_block     = 7,  /* s_tag_block (12 bytes); arg=element struct idx */
    k_op_tag_interop   = 8,  /* D3DStructure<T> / TagStructureReference<T> (12 bytes); arg=descriptor struct idx */
    k_op_tag_pointer   = 9,  /* 32-bit pointer; arg=target struct idx */
    k_op_inline_struct = 10, /* arg=struct idx, count=repeat */
    k_op_hs_syntax_data = 11, /* the 4-byte value of an hs_syntax_node (+0x10); width from hs_syntax_data_width */
};

struct s_tag_swap_op {
    uint8_t  kind;
    uint32_t count;
    uint32_t arg;
};

struct s_tag_swap_struct {
    uint32_t size;
    uint32_t first_op;
    uint32_t op_count;
};

struct s_tag_swap_group {
    uint32_t fourcc;
    uint32_t struct_idx;
};

struct s_tag_resource_swap_type {
    uint32_t    guid[4];    /* s_tag_persistent_identifier of the resource type */
    const char *name;
    uint32_t    struct_idx; /* root structure of its control data */
};

struct s_tag_swap_tables {
    const s_tag_swap_op *ops;
    const s_tag_swap_struct *structs;
    uint32_t struct_count;
    const s_tag_resource_swap_type *resource_types;
    uint32_t resource_type_count;
    const char *const *struct_names; /* TagTool type name per struct (PORT_CACHE_SWAP_TRACE builds), else NULL */
};

/* The generated tables. */
const s_tag_swap_tables &tables();

/* Converts a serialized c_function_definition in place (involution; from_be selects the read order). */
void swap_function_blob(uint8_t *blob, uint32_t size, int from_be);

/* Structure index of TagTool.Havok.HkpMoppCode (a serialized hkMoppCode header), or 0xffffffff. */
uint32_t mopp_code_struct_index();

/* Root structure of a tag group (its definition), by group fourcc. Returns 0 for a group the tables do not know. */
int find_group_struct(uint32_t fourcc, uint32_t *out_struct_idx);

/*
 * Bytes of an hs_syntax_node value (+0x10) in big-endian order: the game reads it through a union at offset 0
 * (include/hs/hs.h, hs_value_types.h), long / float / pointer (4), short (2) or bool (1), so a conversion reverses
 * only that many bytes. constant_type (+2) and flags (+6) in host order. A node that is not a constant (flags bit 0
 * clear: a call, whose value is its first argument node) or that refers to a global or parameter (bit 2) holds a
 * long index. A constant is as wide as its type (Halo 3 retail e_hs_type, 83 types): booleans 1; shorts, scripts,
 * trigger volumes, cutscene data, AI command lists / scripts / behaviors / orders, starting profiles,
 * conversations, zone sets, designer zones, the enumerations (game difficulty .. character physics, 50..64) and
 * object names (72..78: hs_cast reads short_value) 2; everything else 4 (objects 65..71 too: hs_cast passes
 * value on as a long).
 */
static inline uint32_t hs_syntax_data_width(uint16_t constant_type, uint16_t flags)
{
    static const uint8_t k_widths[83] = {
        4, 4, 4, 4, 4, 1, 4, 2, 4, 4, // unparsed, special form, function name, passthrough, void, boolean, real, short, long, string
        2, 4, 4, 2, 2, 2, 2, 2, 4, 4, // script, string id, unit seat mapping, trigger volume, cutscene flag / camera point /
                                      // title / recording, device group, ai
        2, 2, 2, 2, 4, 2, 2, 2, 2, 4, // ai command list / script, behavior, orders, ai line, starting profile, conversation,
                                      // zone set, designer zone, point reference
        4, 4, 4, 4, 4, 4, 4, 4, 4, 4, // style, object list, folder, sound .. object definition
        4, 4, 4, 4, 4, 4, 4, 4, 4, 4, // bitmap .. any tag not resolving
        2, 2, 2, 2, 2, 2, 2, 2, 2, 2, // game difficulty .. voice mask
        2, 2, 2, 2, 2, 4, 4, 4, 4, 4, // subtitle setting .. character physics, object, unit, vehicle, weapon, device
        4, 4, 2, 2, 2, 2, 2, 2, 2, 4, // scenery, effect scenery, object .. effect scenery names, cinematic lightprobe
        4, 4, 4,                      // animation budget, looping sound budget, sound budget references
    };
    if (!(flags & 1u) || (flags & 4u)) return 4;
    return constant_type < sizeof(k_widths) ? k_widths[constant_type] : 4;
}

/* Resource type of a zone->resource_types entry's s_tag_persistent_identifier (host order), or NULL. */
const s_tag_resource_swap_type *find_resource_type(const void *identifier);

} // namespace port_cache_swap

#endif /* PORT_DATA_CACHE_SWAP_INTERNAL_H */
