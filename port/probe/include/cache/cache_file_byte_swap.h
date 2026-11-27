#ifndef CACHE_CACHE_FILE_BYTE_SWAP_H
#define CACHE_CACHE_FILE_BYTE_SWAP_H

// Cache files are stored big-endian. The Xbox reads them in place; a little-endian host
// converts the header and the tag section once, where the raw file bytes become structures,
// and each resource page once, when it has been read and its hash checked.

struct s_cache_file_header;
class c_tag_resource_page_datum;

#if defined(_XBOX) && !defined(__clang__)

static __forceinline void cache_file_header_to_host(s_cache_file_header *)
{
}

static __forceinline void cache_file_header_to_disk(s_cache_file_header *)
{
}

static __forceinline bool cache_file_tags_to_host(const s_cache_file_header *, void *)
{
    return true;
}

static __forceinline void cache_file_resource_page_to_host(const s_cache_file_header *, const void *, c_tag_resource_page_datum *)
{
}

static __forceinline void cache_file_resource_page_moved(const void *, unsigned long)
{
}

static __forceinline long cache_file_checksum_to_host(long checksum)
{
    return checksum;
}

#else

#include "cache/tag_resource_page_table.h"
#include "cache/tag_resource_page_allocator.h"

struct s_port_cache_swap_stats;
extern "C" void port_cache_swap_header(void *header);
extern "C" int port_cache_swap_tags(const void *header, void *tag_buffer, int from_big_endian, s_port_cache_swap_stats *stats);

// The tag section and the two tags paged data needs (port/data/resource_swap.h).
struct s_port_resource_zone_view
{
    const unsigned char *tag_buffer;
    unsigned int tag_base_address;
    unsigned int tag_size;
    const unsigned char *zone;
    const unsigned char *layout;
};
struct s_port_resource_swap_stats;
extern "C" int port_resource_zone_view_init(const void *swapped_header_0x3000, const void *tag_buffer, s_port_resource_zone_view *out_view);
extern "C" int port_resource_swap_page(const s_port_resource_zone_view *view, unsigned int page_index, void *page_data,
    unsigned int page_size, int from_big_endian, s_port_resource_swap_stats *stats);
// Nonzero when the page's bytes are those of a page port_resource_swap_page converted to host order.
extern "C" int port_resource_page_is_host_order(const void *page_data, unsigned int page_size);
// The renderer's records about memory the pager just rewrote (port/sdk/d3d/vulkan_device.h): the page holds Xenos-order
// texels now, whatever the CPU or a Resolve left in that memory before.
extern "C" void port_vk_memory_streamed(const void *base, unsigned int size);

// The header swap is its own inverse, so one call converts in either direction.
static __forceinline void cache_file_header_to_host(s_cache_file_header *header)
{
    port_cache_swap_header(header);
}

static __forceinline void cache_file_header_to_disk(s_cache_file_header *header)
{
    port_cache_swap_header(header);
}

static __forceinline bool cache_file_tags_to_host(const s_cache_file_header *header, void *tag_buffer)
{
    return port_cache_swap_tags(header, tag_buffer, 1, 0) != 0;
}

// The page's layout index is the low half of its handle, the one the datum handler sizes it by.
static __forceinline void cache_file_resource_page_to_host(const s_cache_file_header *header, const void *tag_buffer,
    c_tag_resource_page_datum *page)
{
    s_port_resource_zone_view view;
    if (port_resource_zone_view_init(header, tag_buffer, &view))
    {
        port_resource_swap_page(&view, page->m_handle & 0xffff, (void *)page->m_page->buffer.m_address,
            page->m_datum_handler->get_page_size(page->m_handle), 1, 0);
    }
    port_vk_memory_streamed(page->m_page->buffer.m_address, page->m_datum_handler->get_page_size(page->m_handle));
}

// Compaction moved a ready page (lruv_move_tag_resource_page): its new memory holds the page's bytes now.
static __forceinline void cache_file_resource_page_moved(const void *data, unsigned long size)
{
    port_vk_memory_streamed(data, (unsigned int)size);
}

// The inverse of cache_file_resource_page_to_host: back to the order the page has on disc.
static __forceinline void cache_file_resource_page_to_big_endian(const s_cache_file_header *header, const void *tag_buffer,
    c_tag_resource_page_datum *page)
{
    s_port_resource_zone_view view;
    if (port_resource_zone_view_init(header, tag_buffer, &view))
    {
        port_resource_swap_page(&view, page->m_handle & 0xffff, (void *)page->m_page->buffer.m_address,
            page->m_datum_handler->get_page_size(page->m_handle), 0, 0);
    }
}

// The copy checksum XORs the file's 64-bit words as the host reads them; the XOR commutes with the
// byte order, so a little-endian sum is the big-endian one byte-reversed.
static __forceinline long cache_file_checksum_to_host(long checksum)
{
#if defined(__clang__) || defined(__GNUC__)
    return (long)__builtin_bswap32((unsigned long)checksum);
#else
    return (long)_byteswap_ulong((unsigned long)checksum);
#endif
}

#endif

#endif // CACHE_CACHE_FILE_BYTE_SWAP_H
