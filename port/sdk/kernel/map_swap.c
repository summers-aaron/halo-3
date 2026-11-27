// port/sdk/kernel (both hosts): cache-file byte order at the SDK boundary (see map_swap.h).
#include "map_swap.h"
#include "../../data/cache_swap.h"
#include "../runtime/port_runtime.h"

extern char *getenv(const char *name);

// s_cache_file_header (include/cache/cache_file_header.h) offsets this layer reads.
#define HEADER_TAGS_HEADER      0x010u // TAG_POINTER(s_cache_file_tags_header)
#define HEADER_TOTAL_TAGS_SIZE  0x018u
#define HEADER_TAG_MEMORY_BASE  0x2e8u
#define HEADER_RANGES           0x2f0u // cache_file_allocation_range ranges[6]: start, size
#define HEADER_FOOTER_SIGNATURE 0x2ffcu
#define TAGS_HEADER_SIGNATURE   0x024u // s_cache_file_tags_header::signature

static unsigned long load32(const void *p)
{
    unsigned long v;
    __builtin_memcpy(&v, p, 4);
    return v;
}

static int bytes_are(const void *p, const char text[4])
{
    const unsigned char *b = (const unsigned char *)p;
    return b[0] == (unsigned char)text[0] && b[1] == (unsigned char)text[1] && b[2] == (unsigned char)text[2]
        && b[3] == (unsigned char)text[3];
}

// 'head' ... 'foot' as the disc stores them (big-endian).
static int is_disk_header(const void *p)
{
    return bytes_are(p, "head") && bytes_are((const char *)p + HEADER_FOOTER_SIGNATURE, "foot");
}

// The same signatures as host values (k_cache_file_header_signature, k_cache_file_footer_signature).
static int is_host_header(const void *p)
{
    return load32(p) == 0x68656164ul && load32((const char *)p + HEADER_FOOTER_SIGNATURE) == 0x666f6f74ul;
}

static int g_enabled = -1;

static int enabled(void)
{
    if (g_enabled < 0) {
        /* Game-side D1-D3 hooks (include/cache/cache_file_byte_swap.h) are now active in tu/;
         * keep SDK-boundary auto-swap opt-in via HALO3_AUTO_MAP_SWAP=1 for standalone SDK tests. */
        const char *on = getenv("HALO3_AUTO_MAP_SWAP");
        g_enabled = (on != 0 && on[0] == '1');
    }
    return g_enabled;
}

// The map whose header scenario_tags_load hashed last, and how much of its tag section has been hashed since.
static struct {
    int active;
    unsigned long tag_base;      // tag_memory_base: where the game loaded the section (it must, D3)
    unsigned long sections_size; // sections 1 and 2: ranges[0..2] + ranges[3..5]
    unsigned long hashed;        // contiguous bytes from tag_base hashed so far
    unsigned char header[PORT_MAP_SWAP_HEADER_SIZE]; // host order
} g_map;

void port_map_swap_after_read(void *buffer, unsigned long requested, unsigned long done, unsigned long long offset)
{
    if (requested == PORT_MAP_SWAP_HEADER_SIZE && done == PORT_MAP_SWAP_HEADER_SIZE && offset == 0 && buffer != 0
        && enabled() && is_disk_header(buffer))
        port_cache_swap_header(buffer);
}

const void *port_map_swap_before_write(const void *buffer, unsigned long bytes, unsigned long long offset,
    void *scratch)
{
    if (bytes != PORT_MAP_SWAP_HEADER_SIZE || offset != 0 || buffer == 0 || !enabled() || !is_host_header(buffer))
        return buffer;
    __builtin_memcpy(scratch, buffer, PORT_MAP_SWAP_HEADER_SIZE);
    port_cache_swap_header(scratch);
    return scratch;
}

const void *port_map_swap_before_hash(const void *data, unsigned long size, void *scratch)
{
    if (data == 0 || !enabled())
        return data;
    if (size == PORT_MAP_SWAP_HEADER_SIZE && is_host_header(data)) {
        // the header as signed: disk order
        __builtin_memcpy(g_map.header, data, PORT_MAP_SWAP_HEADER_SIZE);
        __builtin_memcpy(scratch, data, PORT_MAP_SWAP_HEADER_SIZE);
        port_cache_swap_header(scratch);
        unsigned long sections = 0;
        for (unsigned long i = 0; i < 6; i++)
            sections += load32(g_map.header + HEADER_RANGES + i * 8u + 4u);
        g_map.tag_base = load32(g_map.header + HEADER_TAG_MEMORY_BASE);
        g_map.sections_size = sections;
        g_map.hashed = 0;
        g_map.active = sections != 0 && sections <= load32(g_map.header + HEADER_TOTAL_TAGS_SIZE);
        return scratch;
    }
    // scenario_tags_load hashes sections 1 and 2 in 1 MB chunks straight from the loaded tag section
    if (g_map.active && (__UINTPTR_TYPE__)(const char *)data == (__UINTPTR_TYPE__)(g_map.tag_base + g_map.hashed))
        g_map.hashed += size;
    return data;
}

int port_map_swap_after_verify(int verified)
{
    if (!verified || !g_map.active || g_map.hashed < g_map.sections_size)
        return verified;
    g_map.active = 0;
    // the verified map's whole tag section was hashed: convert it unless it already is
    const unsigned long tags_header = load32(g_map.header + HEADER_TAGS_HEADER);
    const unsigned long total = load32(g_map.header + HEADER_TOTAL_TAGS_SIZE);
    if (tags_header < g_map.tag_base || tags_header - g_map.tag_base > total - 0x28u)
        return verified;
    const void *signature = (const void *)(__UINTPTR_TYPE__)(tags_header + TAGS_HEADER_SIGNATURE);
    if (!bytes_are(signature, "tags"))
        return verified; // host order already (GAME-CHANGES D3, or converted before)
    struct s_port_cache_swap_stats stats;
    if (!port_cache_swap_tags(g_map.header, (void *)(__UINTPTR_TYPE__)g_map.tag_base, 1, &stats)) {
        port_write_error("map swap: the tag section did not convert cleanly; the map is rejected\n");
        return 0;
    }
    return verified;
}
