#ifndef PORT_DATA_RESOURCE_SWAP_H
#define PORT_DATA_RESOURCE_SWAP_H

/*
 * Byte order of Halo 3's paged resource data.
 *
 * A Gen3 resource ('zone' -> resources[i], s_cache_file_tag_resource_data) is control data plus up to two paged
 * buffers. The control data lives in zone->fixup_information and is converted with the tag section
 * (port_cache_swap_tags). Its fixups (s_tag_resource_fixup_location: control offset -> value, the top three bits the
 * type) turn block, data and descriptor addresses into pointers when the resource is made available
 * (tag_resource_fixup_cache_data_references): type 1 into the control data itself, type 2 into the required
 * ("cache") data and type 3 into the optional data, which are segments of resource pages
 * ('play' layout_table: segments -> pages). Those paged buffers are big-endian and are converted here:
 *
 *   - tag blocks paged by type 2 / 3 fixups (structure_bsp_tag_resources: collision BSP nodes, planes, leaves,
 *     surfaces, edges, vertices; instanced geometry; ...): element by element with the layouts port/tools/mapswap
 *     generates for cache_swap (port/data/cache_swap_internal.h);
 *   - render geometry vertex and index buffers (render_geometry_api_resource_definition): per vertex element by the
 *     buffer's declaration, indices as 16-bit values (port/data/geometry_swap.h);
 *   - animation data (model_animation_tag_resource group members): codec headers and frames of every codec of the
 *     retail game (tu/blamlibxbox/animation_codecs.cpp: uncompressed static, 8-byte quantized rotations, byte / word
 *     keyframes, blend screen), node flags, movement and pill offset data;
 *   - c_function_definition data, as in tags.
 * Left as they are, on purpose: bitmap pixel data (port/data/texture_convert converts it from its Xenos layout on
 * upload), sound data (XMA) and other plain byte data (Havok MOPP byte code).
 *
 * Conversion is in place and an involution; from_big_endian selects which order values are read in (counts,
 * offsets, channel tables), so 1 converts disk data to host order and 0 converts it back.
 *
 * The game's hook (port/GAME-CHANGES.md, E1) is port_resource_swap_page, once per page when it has been read,
 * decompressed and hash-checked (tag_resource_page_table_set_page_ready), before any resource on it is fixed up.
 *
 * Host C/C++ with C linkage, no C++ runtime (port/data).
 */

#include <stddef.h>
#include <stdint.h>

#ifndef PORT_MS_ABI
#include "../sdk/runtime/port_abi.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* Where the tag section is and the two tags paged data needs; pointers inside the tag section are addresses based at
 * tag_base_address (s_cache_file_header::tag_memory_base for a loaded map: the section's own address in the game). */
struct s_port_resource_zone_view {
    const uint8_t *tag_buffer;   /* the tag section, host order (port_cache_swap_tags) */
    uint32_t tag_base_address;   /* address tag_buffer[0] has in the section's pointers */
    uint32_t tag_size;
    const uint8_t *zone;         /* the 'zone' tag (s_cache_file_resource_gestalt) */
    const uint8_t *layout;       /* the resource layout table: of the 'play' tag in retail maps, else zone's own */
};

/* Finds 'zone' and 'play' through the tag section header. `swapped_header_0x3000` and the tags must be in host order.
 * Returns 0 if the map has no zone tag. */
int PORT_MS_ABI port_resource_zone_view_init(const void *swapped_header_0x3000, const void *tag_buffer,
                                             struct s_port_resource_zone_view *out_view);

struct s_port_resource_swap_stats {
    uint32_t resources;              /* resources with paged data converted */
    uint32_t paged_blocks;           /* paged tag block arrays */
    uint32_t paged_elements;         /* their elements */
    uint32_t vertex_buffers;
    uint32_t vertices;
    uint32_t index_buffers;
    uint32_t indices;
    uint32_t animation_members;      /* animation data blobs */
    uint32_t animation_codecs;       /* codec sections in them (static + animated) */
    uint32_t functions;              /* c_function_definition data */
    uint32_t mopp_codes;             /* serialized hkMoppCode headers (render geometry per-mesh MOPP data) */
    uint32_t opaque_data;            /* paged data left as bytes (texture pixels, ...) */
    uint32_t opaque_bytes;
    uint32_t absent_buffers;         /* references into a buffer not supplied (other page / optional not loaded) */
    uint32_t bounds_errors;          /* references or offsets outside their buffer */
    uint32_t layout_errors;          /* data whose layout is unknown: vertex declaration, codec, sizes that do not add
                                        up, non-empty references inside paged elements */
    uint32_t first_error_resource;
    uint32_t first_error_kind;       /* e_port_resource_swap_error */
    uint32_t first_error_value;
};

enum e_port_resource_swap_error {
    k_port_resource_swap_error_none = 0,
    k_port_resource_swap_error_bounds = 1,
    k_port_resource_swap_error_fixup = 2,          /* reference without a fixup / of an unexpected type */
    k_port_resource_swap_error_unknown_type = 3,   /* resource type the generated tables do not describe */
    k_port_resource_swap_error_vertex_declaration = 4,
    k_port_resource_swap_error_codec = 5,
    k_port_resource_swap_error_animation_sizes = 6,
    k_port_resource_swap_error_nested_reference = 7,
    k_port_resource_swap_error_index_size = 8,
};

/*
 * Converts the paged data of zone->resources[resource_index]: everything its control data references in
 * `cache_data` (required data, type 2 fixups) and `optional_data` (type 3). Either buffer may be NULL (not loaded,
 * or converted separately); references into it are then skipped. Sizes bound every reference. Ranges referenced
 * twice are converted once. Returns 1 if nothing was out of bounds or unknown.
 */
int port_resource_swap_paged_data(const struct s_port_resource_zone_view *view, uint32_t resource_index,
                                  void *cache_data, uint32_t cache_size, void *optional_data, uint32_t optional_size,
                                  int from_big_endian, struct s_port_resource_swap_stats *stats);

/*
 * Converts everything resources keep on one resource page (`page_index` in the layout table's pages): each
 * resource whose segment's primary page it is (its required data) or secondary page (its optional data).
 * `page_data` holds the page's uncompressed bytes. The page -> resource index is built on first use and kept for the
 * zone tag it was built for (its address and its resources: the next map's zone tag can load at the same address).
 * Returns 1 if nothing was out of bounds or unknown.
 */
int PORT_MS_ABI port_resource_swap_page(const struct s_port_resource_zone_view *view, uint32_t page_index, void *page_data,
                                        uint32_t page_size, int from_big_endian, struct s_port_resource_swap_stats *stats);

/* Converts one animation data blob of a model_animation_tag_resource group member (`member` points at the 0x30-byte
 * group member in host order: frame count, node count, movement type, packed data sizes). Returns 1 if its layout
 * was understood and in bounds. */
int port_resource_swap_animation_data(const void *member, void *data, uint32_t size, int from_big_endian,
                                      struct s_port_resource_swap_stats *stats);

/* Nonzero once port_resource_swap_page has run: paged data reaches the game in host order (GAME-CHANGES E1). */
int PORT_MS_ABI port_resource_paged_data_is_host_order(void);

/*
 * Pages already converted (GAME-CHANGES E1). port_resource_page_mark_host_order records a page's bytes (a 64-bit
 * content hash) once it is in host order; port_resource_page_is_host_order tells whether a page holds such bytes, so
 * a page the next map's page table takes over from the last map (precached during the lobby, or shared) is not
 * converted a second time. The page hash cannot tell: some pages match it in either order.
 */
void port_resource_page_mark_host_order(const void *page_data, uint32_t page_size);
int PORT_MS_ABI port_resource_page_is_host_order(const void *page_data, uint32_t page_size);

#ifdef __cplusplus
}
#endif

#endif /* PORT_DATA_RESOURCE_SWAP_H */
