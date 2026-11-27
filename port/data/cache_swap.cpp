#include "cache_swap.h"

#include "cache_swap_internal.h"

#include "data_crt.h"

// Private tag-load hooks in resource_swap.cpp.
void port_resource_invalidate_page_index(const void *tag_buffer);
void port_resource_register_page_index(const void *header, const void *tag_buffer);

#if defined(PORT_CACHE_SWAP_TRACE)
#include <stdio.h>
#include <stdlib.h>
#endif

namespace {

using namespace port_cache_swap;

#include "cache_swap_tables.inc"

static const uint32_t k_swap_max_depth = 64;

static inline uint16_t bswap16(uint16_t v) {
    return (uint16_t)((v >> 8) | (v << 8));
}

static inline uint32_t bswap32(uint32_t v) {
    return ((v >> 24) & 0x000000ffu) |
           ((v >>  8) & 0x0000ff00u) |
           ((v <<  8) & 0x00ff0000u) |
           ((v << 24) & 0xff000000u);
}

static inline uint64_t bswap64(uint64_t v) {
    return ((uint64_t)bswap32((uint32_t)(v & 0xffffffffu)) << 32) |
           (uint64_t)bswap32((uint32_t)(v >> 32));
}

static inline uint16_t load_u16(const void *p) {
    uint16_t v;
    memcpy(&v, p, sizeof(v));
    return v;
}

static inline void store_u16(void *p, uint16_t v) {
    memcpy(p, &v, sizeof(v));
}

static inline uint32_t load_u32(const void *p) {
    uint32_t v;
    memcpy(&v, p, sizeof(v));
    return v;
}

static inline void store_u32(void *p, uint32_t v) {
    memcpy(p, &v, sizeof(v));
}

static inline uint64_t load_u64(const void *p) {
    uint64_t v;
    memcpy(&v, p, sizeof(v));
    return v;
}

static inline void store_u64(void *p, uint64_t v) {
    memcpy(p, &v, sizeof(v));
}

static inline void swap16_n(uint8_t *p, uint32_t n) {
    for (uint32_t i = 0; i < n; ++i, p += 2) {
        store_u16(p, bswap16(load_u16(p)));
    }
}

static inline void swap32_n(uint8_t *p, uint32_t n) {
    for (uint32_t i = 0; i < n; ++i, p += 4) {
        store_u32(p, bswap32(load_u32(p)));
    }
}

static inline void swap64_n(uint8_t *p, uint32_t n) {
    for (uint32_t i = 0; i < n; ++i, p += 8) {
        store_u64(p, bswap64(load_u64(p)));
    }
}

static int find_group_struct_idx(uint32_t fourcc, uint32_t *out_idx) {
    size_t lo = 0;
    size_t hi = sizeof(k_tag_swap_groups) / sizeof(k_tag_swap_groups[0]);
    while (lo < hi) {
        size_t mid = lo + ((hi - lo) >> 1);
        uint32_t cur = k_tag_swap_groups[mid].fourcc;
        if (cur == fourcc) {
            *out_idx = k_tag_swap_groups[mid].struct_idx;
            return 1;
        }
        if (cur < fourcc) {
            lo = mid + 1;
        } else {
            hi = mid;
        }
    }
    return 0;
}

struct s_resource_fixup_pair {
    uint32_t block_offset;
    uint32_t value;
};

struct s_swap_ctx {
    uint8_t  *tag_base;
    uint32_t  va_base;
    uint32_t  va_size;
    uint8_t  *visited; /* 1 bit per byte of tag_base */
    int       from_be;
    uint32_t  cur_tag_index;
    uint32_t  cur_fourcc;
    s_port_cache_swap_stats *stats;

    /* Active when walking a slice inside zone->fixup_information */
    int       in_resource_slice;
    uint8_t  *res_base;
    uint32_t  res_size;
    uint8_t  *res_visited;
    const s_resource_fixup_pair *res_fixups;
    uint32_t  res_fixup_count;

    /* The first layout that converted each block's data (privatize_shared_block): offset, struct index + 1 pairs */
    uint32_t *owners;
    uint32_t  owner_count;
    uint32_t  owner_capacity;

    /* Structure path from the tag root to the field being walked (diagnostics only) */
    uint32_t  depth;
    uint32_t  path_struct[k_swap_max_depth];
    uint32_t  path_field[k_swap_max_depth];

#if defined(PORT_CACHE_SWAP_TRACE)
    uint32_t *touched;     /* per tag byte: 1 + index of the tag that swapped it in this pass */
    uint32_t *res_touched; /* same for the active resource slice */
    uint32_t  trace_double_swaps;
    uint32_t  watch_va;    /* PORT_CACHE_SWAP_WATCH=<hex va>: print every walk that swaps this address */
    uint32_t *covered;     /* per tag byte: 1 + index of the tag whose walk first reached it (coverage audit) */
    struct s_trace_suspect *suspects;
    uint32_t  suspect_count;
    uint32_t  suspect_capacity;
#endif
};

static void trace_path(const s_swap_ctx *ctx, const char *what, uint32_t value) {
#if defined(PORT_CACHE_SWAP_TRACE)
    char fcc[5] = {
        (char)(ctx->cur_fourcc >> 24), (char)(ctx->cur_fourcc >> 16),
        (char)(ctx->cur_fourcc >> 8), (char)ctx->cur_fourcc, 0 };
    fprintf(stderr, "cache_swap: %s tag=%u '%s' value=0x%08x%s path:",
            what, ctx->cur_tag_index, fcc, value, ctx->in_resource_slice ? " (resource)" : "");
    for (uint32_t d = 0; d < ctx->depth; ++d) {
        fprintf(stderr, " %s+0x%x", k_tag_swap_struct_names[ctx->path_struct[d]], ctx->path_field[d]);
    }
    fputc('\n', stderr);
#else
    (void)ctx; (void)what; (void)value;
#endif
}

static inline void record_bounds_error(s_swap_ctx *ctx, uint32_t bad_va) {
    if (ctx->stats) {
        if (ctx->stats->bounds_errors == 0) {
            ctx->stats->first_error_tag_index  = ctx->cur_tag_index;
            ctx->stats->first_error_fourcc     = ctx->cur_fourcc;
            ctx->stats->first_error_va         = bad_va;
            ctx->stats->first_error_struct_idx = ctx->depth ? ctx->path_struct[ctx->depth - 1] : 0xffffffffu;
            ctx->stats->first_error_field_off  = ctx->depth ? ctx->path_field[ctx->depth - 1] : 0xffffffffu;
        }
        ctx->stats->bounds_errors += 1;
    }
    trace_path(ctx, "bounds error", bad_va);
}

#if defined(PORT_CACHE_SWAP_TRACE)
/* Trace builds remember which tag swapped each tag/resource byte and report any byte swapped twice in one pass. */
static void trace_mark_swapped(s_swap_ctx *ctx, const uint8_t *p, uint32_t n) {
    uint32_t *map = NULL;
    uint32_t off = 0;
    if (ctx->in_resource_slice && p >= ctx->res_base && p < ctx->res_base + ctx->res_size) {
        map = ctx->res_touched;
        off = (uint32_t)(p - ctx->res_base);
    } else if (p >= ctx->tag_base && p < ctx->tag_base + ctx->va_size) {
        map = ctx->touched;
        off = (uint32_t)(p - ctx->tag_base);
    }
    if (!map) return;
    if (ctx->watch_va && map == ctx->touched) {
        uint32_t va = ctx->va_base + off;
        if (ctx->watch_va >= va && ctx->watch_va < va + n) trace_path(ctx, "watch", va);
    }
    for (uint32_t i = 0; i < n; ++i) {
        if (map[off + i]) {
            if (ctx->stats) ctx->stats->double_swaps += 1;
            if (ctx->trace_double_swaps < 32) {
                ctx->trace_double_swaps += 1;
                fprintf(stderr, "cache_swap: (first swapped while walking tag %u)\n", map[off + i] - 1u);
                trace_path(ctx, "double swap", (uint32_t)(ctx->va_base + (p - ctx->tag_base)));
            }
            break;
        }
    }
    for (uint32_t i = 0; i < n; ++i) map[off + i] = ctx->cur_tag_index + 1u;
}
#define TRACE_MARK_SWAPPED(ctx, p, n) trace_mark_swapped((ctx), (p), (n))

/* Plain 32-bit fields whose host value lies inside tag memory; matched against unreached ranges afterwards to name
 * schema fields that are really pointers. */
struct s_trace_suspect { uint32_t value; uint32_t struct_idx; uint32_t field_off; uint32_t tag_index; };

static void trace_plain_dwords(s_swap_ctx *ctx, const uint8_t *p, uint32_t n) {
    if (!ctx->covered || !ctx->from_be || ctx->in_resource_slice || !ctx->depth) return;
    for (uint32_t i = 0; i < n; ++i) {
        uint32_t v = load_u32(p + i * 4u);
        if (v < ctx->va_base || v - ctx->va_base >= ctx->va_size) continue;
        if (ctx->suspect_count == ctx->suspect_capacity) {
            uint32_t cap = ctx->suspect_capacity ? ctx->suspect_capacity * 2u : 4096u;
            s_trace_suspect *grown = (s_trace_suspect *)realloc(ctx->suspects, cap * sizeof(s_trace_suspect));
            if (!grown) return;
            ctx->suspects = grown;
            ctx->suspect_capacity = cap;
        }
        uint32_t level = ctx->depth - 1u;
        ctx->suspects[ctx->suspect_count++] = s_trace_suspect{
            v, ctx->path_struct[level], ctx->path_field[level] + i * 4u, ctx->cur_tag_index };
    }
}
#define TRACE_PLAIN_DWORDS(ctx, p, n) trace_plain_dwords((ctx), (p), (n))

/* Coverage audit: every byte reached by the walk (definitions, blocks, data, interop descriptors) records the tag
 * that reached it first. Unreached tag memory means a field the schema treats as padding or plain data really
 * points somewhere, which a BE -> LE -> BE round trip cannot detect because such data is swapped in neither pass. */
static void trace_cover_owner(s_swap_ctx *ctx, uint32_t off, uint32_t n, uint32_t owner) {
    if (!ctx->covered || off >= ctx->va_size) return;
    if (n > ctx->va_size - off) n = ctx->va_size - off;
    for (uint32_t i = 0; i < n; ++i) {
        if (!ctx->covered[off + i]) ctx->covered[off + i] = owner;
    }
}
#define TRACE_COVER(ctx, off, n) trace_cover_owner((ctx), (off), (n), (ctx)->cur_tag_index + 1u)
#define TRACE_COVER_HEADER(ctx, off, n) trace_cover_owner((ctx), (off), (n), 0xffffffffu)

static int trace_suspect_less(const void *a, const void *b) {
    uint32_t x = ((const s_trace_suspect *)a)->value, y = ((const s_trace_suspect *)b)->value;
    return x < y ? -1 : (x > y ? 1 : 0);
}

/* Groups unreached nonzero tag memory by the plain field that points into it ("unreferenced" when nothing does). */
static void trace_report_coverage(s_swap_ctx *ctx) {
    struct s_ref_total { uint32_t struct_idx; uint32_t field_off; uint64_t bytes; uint32_t runs; uint32_t example_va; };
    enum { k_max_totals = 512 };
    s_ref_total *totals = (s_ref_total *)calloc(k_max_totals, sizeof(s_ref_total));
    if (!totals) return;
    uint32_t total_count = 0;
    uint64_t reported = 0;
    qsort(ctx->suspects, ctx->suspect_count, sizeof(s_trace_suspect), trace_suspect_less);
    if (ctx->watch_va && ctx->watch_va - ctx->va_base < ctx->va_size) {
        fprintf(stderr, "cache_swap: watch 0x%08x reached by tag %d\n", ctx->watch_va, (int)ctx->covered[ctx->watch_va - ctx->va_base] - 1);
    }

    uint32_t i = 0;
    while (i < ctx->va_size) {
        if (ctx->covered[i]) { ++i; continue; }
        uint32_t start = i;
        while (i < ctx->va_size && !ctx->covered[i]) ++i;
        uint32_t len = i - start;
        /* gaps below 16 bytes are alignment between blocks and definitions; all-zero runs are padding */
        uint32_t nonzero = 0;
        for (uint32_t z = start; z < i; ++z) nonzero += ctx->tag_base[z] != 0;
        if (len < 16u || nonzero == 0) continue;
        reported += len;

        /* first plain field whose value points into [start, start + len) */
        uint32_t run_va = ctx->va_base + start;
        uint32_t lo = 0, hi = ctx->suspect_count;
        while (lo < hi) {
            uint32_t mid = (lo + hi) >> 1;
            if (ctx->suspects[mid].value < run_va) lo = mid + 1; else hi = mid;
        }
        uint32_t sidx = 0xffffffffu, foff = 0xffffffffu;
        if (lo < ctx->suspect_count && ctx->suspects[lo].value < run_va + len) {
            sidx = ctx->suspects[lo].struct_idx;
            foff = ctx->suspects[lo].field_off;
        }
        uint32_t k = 0;
        while (k < total_count && !(totals[k].struct_idx == sidx && totals[k].field_off == foff)) ++k;
        if (k == total_count && total_count < k_max_totals) {
            totals[total_count++] = s_ref_total{ sidx, foff, 0, 0, run_va };
        }
        if (k < total_count) { totals[k].bytes += len; totals[k].runs += 1; }
        if (ctx->watch_va >= run_va && ctx->watch_va - run_va < len) {
            for (uint32_t s = lo, shown = 0; s < ctx->suspect_count && ctx->suspects[s].value < run_va + len && shown < 24; ++s, ++shown) {
                fprintf(stderr, "cache_swap:   0x%08x from tag %u %s+0x%x\n", ctx->suspects[s].value, ctx->suspects[s].tag_index,
                        k_tag_swap_struct_names[ctx->suspects[s].struct_idx], ctx->suspects[s].field_off);
            }
        }
        if (ctx->stats) ctx->stats->uncovered_runs += 1;
    }
    if (ctx->stats) ctx->stats->uncovered_bytes = (uint32_t)reported;
    for (uint32_t k = 0; k < total_count; ++k) {
        if (totals[k].struct_idx == 0xffffffffu) {
            fprintf(stderr, "cache_swap: unreached %llu bytes in %u runs, unreferenced (e.g. va 0x%08x)\n",
                    (unsigned long long)totals[k].bytes, totals[k].runs, totals[k].example_va);
        } else {
            fprintf(stderr, "cache_swap: unreached %llu bytes in %u runs, pointed to by %s+0x%x (e.g. va 0x%08x)\n",
                    (unsigned long long)totals[k].bytes, totals[k].runs,
                    k_tag_swap_struct_names[totals[k].struct_idx], totals[k].field_off, totals[k].example_va);
        }
    }
    free(totals);
}
#else
#define TRACE_MARK_SWAPPED(ctx, p, n) ((void)(ctx), (void)(p), (void)(n))
#define TRACE_COVER(ctx, off, n) ((void)0)
#define TRACE_PLAIN_DWORDS(ctx, p, n) ((void)0)
#define TRACE_COVER_HEADER(ctx, off, n) ((void)0)
#endif

static inline void ctx_swap16(s_swap_ctx *ctx, uint8_t *p, uint32_t n) {
    TRACE_MARK_SWAPPED(ctx, p, n * 2u);
    swap16_n(p, n);
}

static inline void ctx_swap32(s_swap_ctx *ctx, uint8_t *p, uint32_t n) {
    TRACE_MARK_SWAPPED(ctx, p, n * 4u);
    swap32_n(p, n);
}

static inline void ctx_swap64(s_swap_ctx *ctx, uint8_t *p, uint32_t n) {
    TRACE_MARK_SWAPPED(ctx, p, n * 8u);
    swap64_n(p, n);
}

static inline int va_range_valid(const s_swap_ctx *ctx, uint32_t va, uint64_t byte_len, uint32_t *out_off) {
    if (va < ctx->va_base) return 0;
    uint64_t off = (uint64_t)(va - ctx->va_base);
    if (off + byte_len > (uint64_t)ctx->va_size) return 0;
    *out_off = (uint32_t)off;
    return 1;
}

static inline int test_and_mark_visited(uint8_t *bitmap, uint32_t byte_off) {
    uint32_t idx = byte_off >> 3;
    uint8_t  bit = (uint8_t)(1u << (byte_off & 7u));
    if (bitmap[idx] & bit) return 1;
    bitmap[idx] |= bit;
    return 0;
}


/* ---- block data the cache file shares between blocks of different element layouts ----
 *
 * The cache compiler stores identical block data once: a pixel / vertex shader's entry-point array of
 * { uint8 start, uint8 count } ({ 00 01 }) is the same two bytes as a render method's option index array of one int16 1.
 * The console reads both from the same big-endian bytes; a little-endian host cannot (the short wants 01 00, the
 * byte pair 00 01). The first walk to reach the data converts it for its own layout. A later block of another
 * layout over the same data gets a private copy converted for its layout, and its address moves there. Only leaf
 * layouts (bytes, 16/32/64-bit values, nested inline structures of those) take part; their conversion is its own
 * inverse, which brings the shared bytes back to the cache file's order for the copy. */

/* The swap width of every byte of one element of a leaf structure (1 = left as it is); 0 when the structure is
 * not a leaf (blocks, references, pointers, data) or larger than `capacity`. */
static uint32_t leaf_widths(uint32_t struct_idx, uint8_t *widths, uint32_t capacity, uint32_t depth) {
    const s_tag_swap_struct &st = k_tag_swap_structs[struct_idx];
    if (st.size > capacity || depth > 8) return 0;
    uint32_t at = 0;
    for (uint32_t oi = st.first_op; oi < st.first_op + st.op_count; ++oi) {
        const s_tag_swap_op &op = k_tag_swap_ops[oi];
        uint32_t width = 0, bytes = 0;
        switch (op.kind) {
        case k_op_skip: width = 1; bytes = op.count; break;
        case k_op_swap16: width = 2; bytes = op.count * 2u; break;
        case k_op_swap32: width = 4; bytes = op.count * 4u; break;
        case k_op_swap64: width = 8; bytes = op.count * 8u; break;
        case k_op_inline_struct: {
            const uint32_t size = k_tag_swap_structs[op.arg].size;
            for (uint32_t r = 0; r < op.count; ++r) {
                if (at + size > capacity || leaf_widths(op.arg, widths + at, capacity - at, depth + 1) != size) return 0;
                at += size;
            }
            continue;
        }
        default: return 0;
        }
        if (at + bytes > capacity) return 0;
        memset(widths + at, (int)width, bytes);
        at += bytes;
    }
    return at == st.size ? st.size : 0;
}

/* Swaps `count` elements of `size` bytes in place by a leaf layout's widths. */
static void apply_leaf_widths(uint8_t *data, uint32_t count, uint32_t size, const uint8_t *widths) {
    for (uint32_t e = 0; e < count; ++e, data += size) {
        for (uint32_t b = 0; b < size;) {
            const uint32_t w = widths[b] ? widths[b] : 1u;
            if (w == 2) store_u16(data + b, bswap16(load_u16(data + b)));
            else if (w == 4) store_u32(data + b, bswap32(load_u32(data + b)));
            else if (w == 8) store_u64(data + b, bswap64(load_u64(data + b)));
            b += w;
        }
    }
}

/* The private copies of the last pass over each tag buffer (freed when that buffer is converted again). */
struct s_private_copy {
    const void *tag_buffer;
    void *data;
    uint32_t va;          /* its address in tag space */
    uint32_t shared_va;   /* the shared data it copies (restored by the LE -> BE pass) */
};
#if defined(__x86_64__) || defined(_M_X64)
#ifdef _WIN32
extern "C" void *port_low4g_reserve_commit(size_t size);
extern "C" void port_low4g_release(void *base);
#else
#include <sys/mman.h>
#endif

struct s_private_arena {
    const void *tag_buffer;
    uint8_t *base;
    size_t size;
    size_t used;
    s_private_arena *next;
};

static s_private_arena *g_private_arenas;

static void *private_low32_alloc(const void *tag_buffer, uint32_t size) {
    const size_t aligned = (size ? ((size_t)size + 15u) & ~(size_t)15u : 16u);
    for (s_private_arena *a = g_private_arenas; a; a = a->next) {
        if (a->tag_buffer == tag_buffer && a->used + aligned <= a->size) {
            void *p = a->base + a->used;
            a->used += aligned;
            return p;
        }
    }
    const size_t k_slab = 1024u * 1024u;
    const size_t map_sz = aligned > k_slab ? ((aligned + 0xffffu) & ~(size_t)0xffffu) : k_slab;
#ifdef _WIN32
    void *mem = port_low4g_reserve_commit(map_sz);
    if (!mem) return 0;
#else
    void *mem = mmap(0, map_sz, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_32BIT, -1, 0);
    if (mem == MAP_FAILED) return 0;
#endif
    s_private_arena *a = (s_private_arena *)malloc(sizeof(*a));
    if (!a) {
#ifdef _WIN32
        port_low4g_release(mem);
#else
        munmap(mem, map_sz);
#endif
        return 0;
    }
    a->tag_buffer = tag_buffer;
    a->base = (uint8_t *)mem;
    a->size = map_sz;
    a->used = aligned;
    a->next = g_private_arenas;
    g_private_arenas = a;
    return mem;
}

static void private_low32_release(const void *tag_buffer) {
    s_private_arena **pp = &g_private_arenas;
    while (*pp) {
        s_private_arena *a = *pp;
        if (a->tag_buffer == tag_buffer) {
            *pp = a->next;
#ifdef _WIN32
            port_low4g_release(a->base);
#else
            munmap(a->base, a->size);
#endif
            free(a);
        } else {
            pp = &a->next;
        }
    }
}

static const char g_havok_unpack_arena_owner = 0;

extern "C" void *PORT_MS_ABI port_havok_unpack_alloc_64(size_t bytes) {
    return private_low32_alloc(&g_havok_unpack_arena_owner, bytes ? (uint32_t)bytes : 16u);
}
#endif

static s_private_copy *g_private_copies;
static uint32_t g_private_count, g_private_capacity;

static void release_private_copies(const void *tag_buffer) {
    uint32_t kept = 0;
    for (uint32_t i = 0; i < g_private_count; ++i) {
        if (g_private_copies[i].tag_buffer == tag_buffer) {
#if !(defined(__x86_64__) || defined(_M_X64))
            free(g_private_copies[i].data);
#endif
        } else {
            g_private_copies[kept++] = g_private_copies[i];
        }
    }
    g_private_count = kept;
#if defined(__x86_64__) || defined(_M_X64)
    private_low32_release(tag_buffer);
    private_low32_release(&g_havok_unpack_arena_owner);
#endif
}

static void *private_copy_alloc(const void *tag_buffer, uint32_t size, uint32_t va_base, uint32_t shared_va) {
    if (g_private_count == g_private_capacity) {
        const uint32_t capacity = g_private_capacity ? g_private_capacity * 2u : 256u;
        s_private_copy *grown = (s_private_copy *)realloc(g_private_copies, capacity * sizeof(*grown));
        if (!grown) return 0;
        g_private_copies = grown;
        g_private_capacity = capacity;
    }
#if defined(__x86_64__) || defined(_M_X64)
    void *data = private_low32_alloc(tag_buffer, size);
#else
    void *data = malloc(size ? size : 1u);
#endif
    if (!data) return 0;
    g_private_copies[g_private_count].tag_buffer = tag_buffer;
    g_private_copies[g_private_count].data = data;
    g_private_copies[g_private_count].va = va_base + (uint32_t)((const uint8_t *)data - (const uint8_t *)tag_buffer);
    g_private_copies[g_private_count].shared_va = shared_va;
    ++g_private_count;
    return data;
}

/* The shared data a private copy at tag address `va` replaced for this buffer's last BE -> LE pass; 0 if none. */
static uint32_t private_copy_shared_va(const void *tag_buffer, uint32_t va) {
    for (uint32_t i = 0; i < g_private_count; ++i)
        if (g_private_copies[i].tag_buffer == tag_buffer && g_private_copies[i].va == va) return g_private_copies[i].shared_va;
    return 0;
}

/* The first layout that converted each block's data: tag offset -> struct index + 1 (open addressing). */
static uint32_t *owner_slot(s_swap_ctx *ctx, uint32_t off, int insert) {
    if (insert && (ctx->owner_count + 1u) * 2u > ctx->owner_capacity) {
        const uint32_t capacity = ctx->owner_capacity ? ctx->owner_capacity * 2u : 4096u;
        uint32_t *table = (uint32_t *)calloc((size_t)capacity * 2u, sizeof(uint32_t));
        if (!table) return 0;
        for (uint32_t i = 0; i < ctx->owner_capacity; ++i) {
            if (ctx->owners[i * 2u + 1u] == 0) continue;
            uint32_t h = (ctx->owners[i * 2u] * 2654435761u) & (capacity - 1u);
            while (table[h * 2u + 1u] != 0) h = (h + 1u) & (capacity - 1u);
            table[h * 2u] = ctx->owners[i * 2u];
            table[h * 2u + 1u] = ctx->owners[i * 2u + 1u];
        }
        free(ctx->owners);
        ctx->owners = table;
        ctx->owner_capacity = capacity;
    }
    if (ctx->owner_capacity == 0) return 0;
    uint32_t h = (off * 2654435761u) & (ctx->owner_capacity - 1u);
    while (ctx->owners[h * 2u + 1u] != 0) {
        if (ctx->owners[h * 2u] == off) return &ctx->owners[h * 2u + 1u];
        h = (h + 1u) & (ctx->owner_capacity - 1u);
    }
    if (!insert) return 0;
    ctx->owners[h * 2u] = off;
    ctx->owner_count += 1;
    return &ctx->owners[h * 2u + 1u];
}

/* A block (header at `header`, host order) over data another layout already converted: when the two leaf layouts
 * differ, the block gets a private copy in its own layout. */
static void privatize_shared_block(s_swap_ctx *ctx, uint8_t *header, uint32_t off, uint32_t struct_idx, uint32_t count) {
    const uint32_t *owner = owner_slot(ctx, off, 0);
    if (!owner || *owner == 0 || *owner - 1u == struct_idx) return;
    const uint32_t owner_idx = *owner - 1u;
    const uint32_t size = k_tag_swap_structs[struct_idx].size;
    if (k_tag_swap_structs[owner_idx].size != size || size == 0 || size > 64u) return;
    uint8_t mine[64], theirs[64];
    if (leaf_widths(struct_idx, mine, sizeof(mine), 0) != size || leaf_widths(owner_idx, theirs, sizeof(theirs), 0) != size)
        return; /* not leaves: shared nested data is not expected (a trace build reports it as a double swap) */
    if (memcmp(mine, theirs, size) == 0) return; /* the same conversion: sharing is harmless */
    const uint32_t bytes = count * size;
    uint8_t *copy = (uint8_t *)private_copy_alloc(ctx->tag_base, bytes, ctx->va_base, ctx->va_base + off);
    if (!copy) return;
    memcpy(copy, ctx->tag_base + off, bytes);
    apply_leaf_widths(copy, count, size, theirs); /* back to the cache file's byte order */
    apply_leaf_widths(copy, count, size, mine);
    /* the block's address in tag space (the host maps tag space at tag_base) */
    store_u32(header + 4, ctx->va_base + (uint32_t)(copy - ctx->tag_base));
    if (ctx->stats) ctx->stats->private_blocks += 1;
}

static int lookup_resource_slice_target(const s_swap_ctx *ctx, const uint8_t *field_ptr, uint64_t byte_len, uint32_t *out_off) {
    uint32_t field_off = (uint32_t)(field_ptr - ctx->res_base);
    for (uint32_t i = 0; i < ctx->res_fixup_count; ++i) {
        if (ctx->res_fixups[i].block_offset == field_off) {
            uint32_t fval = ctx->res_fixups[i].value;
            uint32_t ftype = fval >> 29;
            uint32_t foff  = fval & 0x1fffffffu;
            /* _tag_resource_fixup_type_data (1): offset inside this resource's control_data slice */
            if (ftype == 1u) {
                if ((uint64_t)foff + byte_len > (uint64_t)ctx->res_size) {
                    return -1;
                }
                *out_off = foff;
                return 1;
            }
            /* Other fixup types (2 = primary paged data, 3 = optional paged data, 0 = literal) live outside fixup_information */
            return 0;
        }
    }
    return 0;
}

/*
 * Swaps a c_function_definition blob (tu/blamlibxbox/function_definitions.cpp):
 *   0x00..0x03: u8 type, u8 flags, u8 constant/color_count, u8 pad (preserved)
 *   0x04..0x1f: 7 x u32 (colors[4] or lower/upper, excluded_lower, excluded_upper, compact_data_size)
 *   0x20..    : compact_data ((flags & 1) ? 2 : 1 compact function blocks)
 */
static void swap_function_blob(uint8_t *blob, uint32_t size, int from_be) {
    if (size < 32u) return;

    uint8_t fn_type  = blob[0];
    uint8_t fn_flags = blob[1];

    if (from_be) {
        swap32_n(blob + 4, 7);
    }

    uint32_t compact_size = load_u32(blob + 28);
    uint32_t avail = size - 32u;
    if (compact_size > avail) compact_size = avail;

    uint8_t *cur = blob + 32;
    uint8_t *end = cur + compact_size;
    uint32_t fn_count = (fn_flags & 1u) ? 2u : 1u;

    for (uint32_t fi = 0; fi < fn_count && cur < end; ++fi) {
        switch (fn_type) {
        case 0: /* identity */
        case 1: /* constant */
            break;
        case 2: /* c_transition_function_compact: u8 m_type + pad[3], float m_lower, float m_upper (12B) */
            if ((size_t)(end - cur) >= 12u) {
                swap32_n(cur + 4, 2);
                cur += 12;
            } else { cur = end; }
            break;
        case 3: /* c_periodic_function_compact: u8 m_type + pad[3], 4 floats (20B) */
            if ((size_t)(end - cur) >= 20u) {
                swap32_n(cur + 4, 4);
                cur += 20;
            } else { cur = end; }
            break;
        case 4: /* c_linear_function_compact: 2 floats (8B) */
            if ((size_t)(end - cur) >= 8u) {
                swap32_n(cur, 2);
                cur += 8;
            } else { cur = end; }
            break;
        case 5: /* c_linear_key_function: 20 floats (80B) */
            if ((size_t)(end - cur) >= 80u) {
                swap32_n(cur, 20);
                cur += 80;
            } else { cur = end; }
            break;
        case 7: /* c_spline_function_compact: 4 floats (16B) */
            if ((size_t)(end - cur) >= 16u) {
                swap32_n(cur, 4);
                cur += 16;
            } else { cur = end; }
            break;
        case 8: /* c_multi_part_function_compact: long m_part_count + s_function_part[] */
            if ((size_t)(end - cur) >= 4u) {
                if (from_be) swap32_n(cur, 1);
                uint32_t part_count = load_u32(cur);
                if (!from_be) swap32_n(cur, 1);
                cur += 4;
                for (uint32_t pi = 0; pi < part_count && (size_t)(end - cur) >= 8u; ++pi) {
                    uint8_t ptype = cur[0];
                    uint32_t psize = (ptype == 4u) ? 16u : ((ptype == 7u) ? 24u : 36u);
                    if ((size_t)(end - cur) < psize) { cur = end; break; }
                    /* cur[0..3] = u8 type + pad[3]; cur[4..psize-1] = domain_max + compact floats */
                    swap32_n(cur + 4, (psize - 4u) >> 2);
                    cur += psize;
                }
            } else { cur = end; }
            break;
        case 9: /* c_exponent_function_compact: 3 floats (12B) */
            if ((size_t)(end - cur) >= 12u) {
                swap32_n(cur, 3);
                cur += 12;
            } else { cur = end; }
            break;
        case 10: /* c_spline2_function_compact: 7 floats (28B) */
            if ((size_t)(end - cur) >= 28u) {
                swap32_n(cur, 7);
                cur += 28;
            } else { cur = end; }
            break;
        default:
            cur = end;
            break;
        }
    }

    if (!from_be) {
        swap32_n(blob + 4, 7);
    }
}

static void swap_struct_instance(s_swap_ctx *ctx, uint8_t *base, uint32_t struct_idx);
static uint32_t swap_mopp_code_sequence(s_swap_ctx *ctx, uint8_t *blob, uint32_t size, int32_t max_count);

static void swap_struct_array(s_swap_ctx *ctx, uint8_t *base, uint32_t struct_idx, uint32_t count) {
    uint32_t elem_size = k_tag_swap_structs[struct_idx].size;
    /* Fast path for single-opcode primitive/padding arrays */
    if (k_tag_swap_structs[struct_idx].op_count == 1) {
        const s_tag_swap_op &op = k_tag_swap_ops[k_tag_swap_structs[struct_idx].first_op];
        if (op.kind == k_op_skip) return;
        if (op.kind == k_op_swap16) { ctx_swap16(ctx, base, op.count * count); return; }
        if (op.kind == k_op_swap32) { ctx_swap32(ctx, base, op.count * count); return; }
        if (op.kind == k_op_swap64) { ctx_swap64(ctx, base, op.count * count); return; }
    }
    uint8_t *p = base;
    for (uint32_t i = 0; i < count; ++i, p += elem_size) {
        swap_struct_instance(ctx, p, struct_idx);
    }
}

static void swap_struct_instance(s_swap_ctx *ctx, uint8_t *base, uint32_t struct_idx) {
    const s_tag_swap_struct &st = k_tag_swap_structs[struct_idx];
    uint8_t *p = base;
    uint32_t end_op = st.first_op + st.op_count;

    if (ctx->depth >= k_swap_max_depth) {
        record_bounds_error(ctx, struct_idx);
        return;
    }
    uint32_t level = ctx->depth++;
    ctx->path_struct[level] = struct_idx;

    for (uint32_t oi = st.first_op; oi < end_op; ++oi) {
        const s_tag_swap_op &op = k_tag_swap_ops[oi];
        ctx->path_field[level] = (uint32_t)(p - base);
        switch ((e_tag_swap_op_kind)op.kind) {
        case k_op_skip:
            p += op.count;
            break;
        case k_op_swap16:
            ctx_swap16(ctx, p, op.count);
            p += op.count * 2u;
            break;
        case k_op_swap32:
            ctx_swap32(ctx, p, op.count);
            TRACE_PLAIN_DWORDS(ctx, p, op.count);
            p += op.count * 4u;
            break;
        case k_op_swap64:
            ctx_swap64(ctx, p, op.count);
            p += op.count * 8u;
            break;
        case k_op_tag_ref:
            /* s_tag_reference (16B): group_tag, name, name_length, index */
            ctx_swap32(ctx, p, 4u * op.count);
            p += 16u * op.count;
            break;
        case k_op_tag_data: {
            /* s_tag_data (20B): size (+0), flags (+4), stream_position (+8), address (+12), definition (+16) */
            if (ctx->from_be) ctx_swap32(ctx, p, 5);
            int32_t  dsize = (int32_t)load_u32(p + 0);
            uint32_t daddr = load_u32(p + 12);
            if ((op.arg == 1u || op.arg == 2u) && dsize > 0) {
                uint8_t *blob = 0;
                if (ctx->in_resource_slice) {
                    uint32_t sub_off = 0;
                    int rc = lookup_resource_slice_target(ctx, p + 12, (uint64_t)(uint32_t)dsize, &sub_off);
                    if (rc == 1 && !test_and_mark_visited(ctx->res_visited, sub_off)) {
                        blob = ctx->res_base + sub_off;
                    } else if (rc < 0) {
                        record_bounds_error(ctx, sub_off);
                    }
                } else if (daddr != 0) {
                    uint32_t off = 0;
                    if (va_range_valid(ctx, daddr, (uint64_t)(uint32_t)dsize, &off)) {
                        TRACE_COVER(ctx, off, (uint32_t)dsize);
                        if (!test_and_mark_visited(ctx->visited, off)) blob = ctx->tag_base + off;
                    } else {
                        record_bounds_error(ctx, daddr);
                    }
                }
                if (blob && op.arg == 1u) {
                    swap_function_blob(blob, (uint32_t)dsize, ctx->from_be);
                    if (ctx->stats) ctx->stats->swapped_functions += 1;
                } else if (blob) {
                    const uint32_t codes = swap_mopp_code_sequence(ctx, blob, (uint32_t)dsize, 0x7fffffff);
                    if (ctx->stats) ctx->stats->swapped_mesh_mopps += codes;
                }
            } else if (!ctx->in_resource_slice && dsize > 0 && daddr != 0) {
                uint32_t off = 0;
                if (!va_range_valid(ctx, daddr, (uint64_t)(uint32_t)dsize, &off)) {
                    record_bounds_error(ctx, daddr);
                } else {
                    TRACE_COVER(ctx, off, (uint32_t)dsize);
                }
            }
            if (!ctx->from_be) ctx_swap32(ctx, p, 5);
            p += 20;
            break;
        }
        case k_op_tag_block: {
            /* s_tag_block (12B): count (+0), address (+4), definition (+8) */
            if (ctx->from_be) ctx_swap32(ctx, p, 3);
            int32_t  bcount = (int32_t)load_u32(p + 0);
            uint32_t baddr  = load_u32(p + 4);
            uint32_t esize  = k_tag_swap_structs[op.arg].size;
            if (bcount > 0 && esize > 0) {
                uint64_t byte_len = (uint64_t)(uint32_t)bcount * (uint64_t)esize;
                if (ctx->in_resource_slice) {
                    uint32_t sub_off = 0;
                    int rc = lookup_resource_slice_target(ctx, p + 4, byte_len, &sub_off);
                    if (rc == 1) {
                        if (!test_and_mark_visited(ctx->res_visited, sub_off)) {
                            swap_struct_array(ctx, ctx->res_base + sub_off, op.arg, (uint32_t)bcount);
                            if (ctx->stats) ctx->stats->swapped_blocks += 1;
                        }
                    } else if (rc < 0) {
                        record_bounds_error(ctx, sub_off);
                    }
                } else if (baddr != 0) {
                    uint32_t off = 0;
                    uint32_t shared_va = 0;
                    if (!ctx->from_be && g_private_count != 0 &&
                        (shared_va = private_copy_shared_va(ctx->tag_base, baddr)) != 0) {
                        /* a private copy (privatize_shared_block): the block points at the shared data again, which
                         * the layout that owns it converts */
                        store_u32(p + 4, shared_va);
                    } else if (va_range_valid(ctx, baddr, byte_len, &off)) {
                        TRACE_COVER(ctx, off, (uint32_t)byte_len);
                        if (!test_and_mark_visited(ctx->visited, off)) {
                            swap_struct_array(ctx, ctx->tag_base + off, op.arg, (uint32_t)bcount);
                            if (ctx->stats) ctx->stats->swapped_blocks += 1;
                            if (ctx->from_be) {
                                if (uint32_t *owner = owner_slot(ctx, off, 1)) *owner = op.arg + 1u;
                            }
                        } else if (ctx->from_be) {
                            privatize_shared_block(ctx, p, off, op.arg, (uint32_t)bcount);
                        }
                    } else {
                        record_bounds_error(ctx, baddr);
                    }
                }
            } else if (bcount < 0 || (bcount > 0 && esize == 0 && baddr != 0)) {
                /* negative count, or a block with data whose element layout is unknown (H3UnknownElement); count
                 * with a null address is an editor-only block stripped from the cache file */
                record_bounds_error(ctx, (uint32_t)bcount);
            }
            if (!ctx->from_be) ctx_swap32(ctx, p, 3);
            p += 12;
            break;
        }
        case k_op_tag_interop: {
            /* D3DStructure<T> / TagStructureReference<T> (12B): descriptor (+0), runtime_address (+4), definition_address (+8) */
            if (ctx->from_be) ctx_swap32(ctx, p, 3);
            uint32_t desc_va = load_u32(p + 0);
            uint32_t dsize   = k_tag_swap_structs[op.arg].size;
            if (dsize > 0) {
                if (ctx->in_resource_slice) {
                    uint32_t sub_off = 0;
                    int rc = lookup_resource_slice_target(ctx, p + 0, (uint64_t)dsize, &sub_off);
                    if (rc == 1) {
                        if (!test_and_mark_visited(ctx->res_visited, sub_off)) {
                            swap_struct_instance(ctx, ctx->res_base + sub_off, op.arg);
                            if (ctx->stats) ctx->stats->swapped_interops += 1;
                        }
                    } else if (rc < 0) {
                        record_bounds_error(ctx, sub_off);
                    }
                } else if (desc_va != 0) {
                    uint32_t off = 0;
                    if (va_range_valid(ctx, desc_va, (uint64_t)dsize, &off)) {
                        TRACE_COVER(ctx, off, dsize);
                        if (!test_and_mark_visited(ctx->visited, off)) {
                            swap_struct_instance(ctx, ctx->tag_base + off, op.arg);
                            if (ctx->stats) ctx->stats->swapped_interops += 1;
                        }
                    } else {
                        record_bounds_error(ctx, desc_va);
                    }
                }
            }
            if (!ctx->from_be) ctx_swap32(ctx, p, 3);
            p += 12;
            break;
        }
        case k_op_tag_pointer: {
            if (ctx->from_be) ctx_swap32(ctx, p, 1);
            uint32_t ptr_va = load_u32(p);
            uint32_t tsize  = k_tag_swap_structs[op.arg].size;
            if (tsize > 0) {
                if (ctx->in_resource_slice) {
                    uint32_t sub_off = 0;
                    int rc = lookup_resource_slice_target(ctx, p, (uint64_t)tsize, &sub_off);
                    if (rc == 1 && !test_and_mark_visited(ctx->res_visited, sub_off)) {
                        swap_struct_instance(ctx, ctx->res_base + sub_off, op.arg);
                    } else if (rc < 0) {
                        record_bounds_error(ctx, sub_off);
                    }
                } else if (ptr_va != 0) {
                    uint32_t off = 0;
                    if (va_range_valid(ctx, ptr_va, (uint64_t)tsize, &off)) {
                        TRACE_COVER(ctx, off, tsize);
                        if (!test_and_mark_visited(ctx->visited, off)) {
                            swap_struct_instance(ctx, ctx->tag_base + off, op.arg);
                        }
                    } else {
                        record_bounds_error(ctx, ptr_va);
                    }
                }
            }
            if (!ctx->from_be) ctx_swap32(ctx, p, 1);
            p += 4;
            break;
        }
        case k_op_hs_syntax_data: {
            /* hs_syntax_node: constant type (+2) and flags (+6), converted by this instance's earlier ops (host order
               when reading big-endian, big-endian when writing it). */
            uint16_t constant_type = load_u16(base + 2), flags = load_u16(base + 6);
            if (!ctx->from_be) {
                constant_type = bswap16(constant_type);
                flags = bswap16(flags);
            }
            const uint32_t width = hs_syntax_data_width(constant_type, flags);
            if (width == 4) ctx_swap32(ctx, p, 1);
            else if (width == 2) ctx_swap16(ctx, p, 1);
            p += 4;
            break;
        }
        case k_op_inline_struct: {
            uint32_t sub_size = k_tag_swap_structs[op.arg].size;
            for (uint32_t k = 0; k < op.count; ++k) {
                swap_struct_instance(ctx, p, op.arg);
                p += sub_size;
            }
            break;
        }
        }
    }
    ctx->depth = level;
}

/*
 * Swaps the headers of serialized hkMoppCodes packed back to back in `blob` (at most max_count of them): each is a
 * 0x30-byte HkpMoppCode header followed by its code bytes, the next at (m_data.m_size + 63) & ~15, the step the
 * game takes (physics_model_definitions.cpp, visibility_collection_structure.cpp mopp_code_tag_data_size). The
 * code bytes are a byte stream and stay as they are. Returns the number of codes swapped.
 */
static uint32_t swap_mopp_code_sequence(s_swap_ctx *ctx, uint8_t *blob, uint32_t size, int32_t max_count) {
    if (k_hkp_mopp_code_struct_idx == 0xffffffffu) return 0;
    uint32_t cur_off = 0, swapped = 0;
    for (int32_t i = 0; i < max_count && cur_off + 0x30u <= size; ++i) {
        uint8_t *hdr = blob + cur_off;
        if (ctx->from_be) {
            swap_struct_instance(ctx, hdr, k_hkp_mopp_code_struct_idx);
        }
        /* hkMoppCode::m_data.m_size sits at +0x24 inside HkpMoppCode (0x30B) */
        uint32_t code_bytes = load_u32(hdr + 0x24);
        if (!ctx->from_be) {
            swap_struct_instance(ctx, hdr, k_hkp_mopp_code_struct_idx);
        }
        swapped++;
        uint32_t step = (code_bytes + 63u) & ~15u;
        if (step < 0x30u || step > size - cur_off) break;
        cur_off += step;
    }
    return swapped;
}

/*
 * Swaps serialized hkMoppCode headers inside s_physics_model_definition::mopp_codes
 * (tu/blamlibxbox/physics_model_definitions.cpp:513-522, 601-615).
 * Host fields of `phmo_def` are already in host LE when called.
 */
static void swap_phmo_mopp_data(s_swap_ctx *ctx, uint8_t *phmo_def) {
    if (k_hkp_mopp_code_struct_idx == 0xffffffffu) return;

    uint32_t flags       = load_u32(phmo_def + 0x000);
    int32_t  mopp_count  = (int32_t)load_u32(phmo_def + 0x0f4); /* offsetof(mopps.count) == 0xf4 */
    int32_t  data_size   = (int32_t)load_u32(phmo_def + 0x100); /* offsetof(mopp_codes.size) == 0x100 */
    uint32_t data_va     = load_u32(phmo_def + 0x10c);          /* offsetof(mopp_codes.address) == 0x10c */

    if ((flags & 1u) || mopp_count <= 0 || data_size <= 0 || data_va == 0) return;

    uint32_t base_off = 0;
    if (!va_range_valid(ctx, data_va, (uint64_t)(uint32_t)data_size, &base_off)) {
        record_bounds_error(ctx, data_va);
        return;
    }
    if (test_and_mark_visited(ctx->visited, base_off)) return;

    const uint32_t codes = swap_mopp_code_sequence(ctx, ctx->tag_base + base_off, (uint32_t)data_size, mopp_count);
    if (ctx->stats) ctx->stats->swapped_phmo_mopps += codes;
}

/*
 * Finds the control-data root structure of a resource type from its s_tag_persistent_identifier
 * (zone->resource_types[i].identifier). Returns 0 for a type this table does not know.
 */
static int find_resource_root_struct(const uint8_t *identifier, uint32_t *out_struct_idx) {
    for (size_t i = 0; i < sizeof(k_tag_resource_swap_types) / sizeof(k_tag_resource_swap_types[0]); ++i) {
        const s_tag_resource_swap_type &type = k_tag_resource_swap_types[i];
        if (load_u32(identifier + 0x0) == type.guid[0] && load_u32(identifier + 0x4) == type.guid[1] &&
            load_u32(identifier + 0x8) == type.guid[2] && load_u32(identifier + 0xc) == type.guid[3])
        {
            *out_struct_idx = type.struct_idx;
            return 1;
        }
    }
    return 0;
}

/*
 * Swaps every resource's control data inside s_cache_file_resource_gestalt::fixup_information ('zone',
 * include/cache/cache_file_resource_gestalt.h). Each s_cache_file_tag_resource_data names its type
 * (resource_types index -> persistent identifier -> root structure), its control-data slice
 * [data_offset, data_offset + data_size), the root location and the fixups that turn slice offsets into pointers.
 * Fields of `zone_def` and its blocks are in host order when called (before the definition is swapped back on the
 * LE -> BE pass).
 */
static void swap_zone_fixup_information(s_swap_ctx *ctx, uint8_t *zone_def) {
    int32_t  res_type_count = (int32_t)load_u32(zone_def + 0x004); /* resource_types */
    uint32_t res_type_va    = load_u32(zone_def + 0x008);
    int32_t  res_count      = (int32_t)load_u32(zone_def + 0x058); /* resources */
    uint32_t res_va         = load_u32(zone_def + 0x05c);
    int32_t  fix_info_size  = (int32_t)load_u32(zone_def + 0x13c); /* fixup_information */
    uint32_t fix_info_va    = load_u32(zone_def + 0x148);

    if (res_count <= 0 || res_va == 0 || fix_info_size <= 0 || fix_info_va == 0) return;

    uint32_t res_arr_off = 0, fix_info_off = 0, res_type_off = 0;
    if (!va_range_valid(ctx, res_va, (uint64_t)(uint32_t)res_count * 0x40u, &res_arr_off)) {
        record_bounds_error(ctx, res_va);
        return;
    }
    if (!va_range_valid(ctx, fix_info_va, (uint64_t)(uint32_t)fix_info_size, &fix_info_off)) {
        record_bounds_error(ctx, fix_info_va);
        return;
    }
    if (res_type_count < 0 ||
        (res_type_count > 0 && !va_range_valid(ctx, res_type_va, (uint64_t)(uint32_t)res_type_count * 0x1cu, &res_type_off)))
    {
        record_bounds_error(ctx, res_type_va);
        return;
    }
    if (test_and_mark_visited(ctx->visited, fix_info_off)) return;

    uint8_t *fix_info_base = ctx->tag_base + fix_info_off;
    uint8_t *res_entries   = ctx->tag_base + res_arr_off;

    for (int32_t ri = 0; ri < res_count; ++ri) {
        uint8_t *entry = res_entries + (uint32_t)ri * 0x40u;
        int8_t   type_idx  = (int8_t)entry[0x12];                   /* page_index's neighbour: resource type index */
        int32_t  d_off     = (int32_t)load_u32(entry + 0x14);       /* data_offset */
        int32_t  d_size    = (int32_t)load_u32(entry + 0x18);       /* data_size */
        uint32_t loc_val   = load_u32(entry + 0x24);                /* location (root of the control data) */
        int32_t  fix_count = (int32_t)load_u32(entry + 0x28);       /* fixups */
        uint32_t fix_va    = load_u32(entry + 0x2c);

        if (type_idx < 0 || d_size <= 0) continue; /* no control data (e.g. sound_resource) */
        if (type_idx >= res_type_count || d_off < 0 ||
            (uint64_t)(uint32_t)d_off + (uint64_t)(uint32_t)d_size > (uint64_t)(uint32_t)fix_info_size)
        {
            record_bounds_error(ctx, (uint32_t)d_off);
            continue;
        }

        uint32_t root_struct_idx = 0;
        const uint8_t *type_def = ctx->tag_base + res_type_off + (uint32_t)type_idx * 0x1cu;
        if (!find_resource_root_struct(type_def, &root_struct_idx)) {
            /* control data of a resource type the generated tables do not describe */
            record_bounds_error(ctx, load_u32(type_def));
            continue;
        }

        /* the root must live in this resource's control data (fixup type 1) */
        uint32_t root_sub_off = loc_val & 0x1fffffffu;
        uint32_t root_sz = k_tag_swap_structs[root_struct_idx].size;
        if ((loc_val >> 29) != 1u || (uint64_t)root_sub_off + (uint64_t)root_sz > (uint64_t)(uint32_t)d_size) {
            record_bounds_error(ctx, loc_val);
            continue;
        }

        s_resource_fixup_pair *pairs = NULL;
        uint32_t pair_count = 0;
        if (fix_count > 0 && fix_va != 0) {
            uint32_t fix_arr_off = 0;
            if (!va_range_valid(ctx, fix_va, (uint64_t)(uint32_t)fix_count * 8u, &fix_arr_off)) {
                record_bounds_error(ctx, fix_va);
                continue;
            }
            pairs = (s_resource_fixup_pair *)malloc((size_t)(uint32_t)fix_count * sizeof(s_resource_fixup_pair));
            if (!pairs) {
                record_bounds_error(ctx, fix_va);
                continue;
            }
            const uint8_t *fp = ctx->tag_base + fix_arr_off;
            for (int32_t fi = 0; fi < fix_count; ++fi) {
                pairs[fi].block_offset = load_u32(fp + (uint32_t)fi * 8u + 0u);
                pairs[fi].value        = load_u32(fp + (uint32_t)fi * 8u + 4u);
            }
            pair_count = (uint32_t)fix_count;
        }

        uint8_t *res_bmap = (uint8_t *)calloc((((size_t)(uint32_t)d_size) + 7u) >> 3, 1u);
        if (!res_bmap) {
            free(pairs);
            record_bounds_error(ctx, (uint32_t)d_size);
            continue;
        }
        ctx->in_resource_slice = 1;
        ctx->res_base          = fix_info_base + (uint32_t)d_off;
        ctx->res_size          = (uint32_t)d_size;
        ctx->res_visited       = res_bmap;
        ctx->res_fixups        = pairs;
        ctx->res_fixup_count   = pair_count;
#if defined(PORT_CACHE_SWAP_TRACE)
        ctx->res_touched       = (uint32_t *)calloc((size_t)(uint32_t)d_size, sizeof(uint32_t));
#endif

        swap_struct_instance(ctx, ctx->res_base + root_sub_off, root_struct_idx);
        if (ctx->stats) ctx->stats->swapped_zone_resources += 1;

        ctx->in_resource_slice = 0;
        ctx->res_base          = NULL;
        ctx->res_size          = 0;
        ctx->res_visited       = NULL;
        ctx->res_fixups        = NULL;
        ctx->res_fixup_count   = 0;
#if defined(PORT_CACHE_SWAP_TRACE)
        free(ctx->res_touched);
        ctx->res_touched       = NULL;
#endif
        free(res_bmap);
        free(pairs);
    }
}

static void release_ctx(s_swap_ctx *ctx) {
    free(ctx->visited);
    free(ctx->owners);
#if defined(PORT_CACHE_SWAP_TRACE)
    free(ctx->touched);
    free(ctx->covered);
    free(ctx->suspects);
#endif
}

} /* namespace */

namespace port_cache_swap {

const s_tag_swap_tables &tables() {
    static const s_tag_swap_tables k_tables = {
        k_tag_swap_ops,
        k_tag_swap_structs,
        (uint32_t)(sizeof(k_tag_swap_structs) / sizeof(k_tag_swap_structs[0])),
        k_tag_resource_swap_types,
        (uint32_t)(sizeof(k_tag_resource_swap_types) / sizeof(k_tag_resource_swap_types[0])),
#if defined(PORT_CACHE_SWAP_TRACE)
        k_tag_swap_struct_names,
#else
        NULL,
#endif
    };
    return k_tables;
}

void swap_function_blob(uint8_t *blob, uint32_t size, int from_be) {
    ::swap_function_blob(blob, size, from_be);
}

uint32_t mopp_code_struct_index() {
    return k_hkp_mopp_code_struct_idx;
}

int find_group_struct(uint32_t fourcc, uint32_t *out_struct_idx) {
    return find_group_struct_idx(fourcc, out_struct_idx);
}

const s_tag_resource_swap_type *find_resource_type(const void *identifier) {
    uint32_t struct_idx = 0;
    if (!find_resource_root_struct((const uint8_t *)identifier, &struct_idx)) return NULL;
    for (size_t i = 0; i < sizeof(k_tag_resource_swap_types) / sizeof(k_tag_resource_swap_types[0]); ++i) {
        if (k_tag_resource_swap_types[i].struct_idx == struct_idx) return &k_tag_resource_swap_types[i];
    }
    return NULL;
}

} // namespace port_cache_swap

extern "C" void PORT_MS_ABI port_cache_swap_header(void *header_0x3000) {
    uint8_t *h = (uint8_t *)header_0x3000;
    if (!h) return;

    swap32_n(h + 0x000, 7);   /* header_signature .. total_tags_size */
    swap16_n(h + 0x13c, 2);   /* scenario_type, flags */
    swap64_n(h + 0x144, 1);   /* cached_map_date (FILETIME) */
    swap32_n(h + 0x14c, 7);   /* low_detail_texture_* + string_id_* */
    swap64_n(h + 0x16c, 1);   /* timestamp (a FILETIME as long[2]; CompareFileTime against level_timestamps) */
    swap64_n(h + 0x174, 3);   /* level_timestamps[3] (FILETIMEs) */
    swap32_n(h + 0x1ac, 1);   /* language */
    swap32_n(h + 0x2b0, 6);   /* minor_version .. cached_map_checksum */
    swap32_n(h + 0x2e8, 2);   /* tag_memory_base, xdk_version */
    swap32_n(h + 0x2f0, 13);  /* ranges[6] (12 longs) + checksum (1 long) */
    swap64_n(h + 0x328, 1);   /* timestamp328 */
    /* 0x330..0x36b: digest (60B SHA-1) - preserved */
    /* 0x36c..0x46b: signature (256B RSA-2048) - preserved */
    swap32_n(h + 0x46c, 16);  /* section_address_to_offsets[4] + sections[4] + trailing_fields[4] */
    /*
     * The tag resource file header that follows trailing_fields (s_cache_file_tag_resource_file_header,
     * tu/blamlibxbox/cache_file_tag_resource_runtime.cpp; the page table occupies trailing_fields + 0x28..0x2328): a
     * map reads it from the header of each shared map it streams from (game_state_xenon_try_and_get_data). Fixed
     * size, so the conversion stays an involution; zone_count is a byte.
     */
    swap16_n(h + 0x4ac, 2);   /* resource_count, page_count */
    swap32_n(h + 0x4b0, 5);   /* offset, key (s_tag_persistent_identifier) */
    for (uint32_t page = 0; page < 320u; ++page) {
        swap32_n(h + 0x4c4 + page * 28u, 2); /* s_cache_file_tag_resource_file_page: flags, size; 20-byte hash */
    }
    const uint32_t zone_count = h[0x27c4];
    for (uint32_t zone = 0; zone < zone_count && 0x27c8u + (zone + 1u) * 172u <= 0x2ffcu; ++zone) {
        swap32_n(h + 0x27c8 + zone * 172u, 43); /* s_resource_availability_zone: unknown0, resources[32], shared[10] */
    }
    swap32_n(h + 0x2ffc, 1);  /* footer_signature */
}

extern "C" int PORT_MS_ABI port_cache_swap_tags(
    const void *swapped_header_0x3000,
    void *tag_buffer,
    int from_big_endian,
    struct s_port_cache_swap_stats *out_stats)
{
    if (out_stats) {
        memset(out_stats, 0, sizeof(*out_stats));
    }
    if (!swapped_header_0x3000 || !tag_buffer) return 0;
    port_resource_invalidate_page_index(tag_buffer);

    const uint8_t *hdr = (const uint8_t *)swapped_header_0x3000;
    uint32_t tags_header_va  = load_u32(hdr + 0x010);
    uint32_t total_tags_size = load_u32(hdr + 0x018);
    uint32_t tag_memory_base = load_u32(hdr + 0x2e8);

    if (total_tags_size == 0) return 1;
    /* the copies the last conversion of this buffer made belong to the tags it held */
    if (from_big_endian) release_private_copies(tag_buffer);

    size_t visited_bytes = ((size_t)total_tags_size + 7u) >> 3;
    uint8_t *visited = (uint8_t *)calloc(visited_bytes ? visited_bytes : 1u, 1u);
    if (!visited) return 0;

    s_swap_ctx ctx;
    memset(&ctx, 0, sizeof(ctx));
    ctx.tag_base = (uint8_t *)tag_buffer;
    ctx.va_base  = tag_memory_base;
    ctx.va_size  = total_tags_size;
    ctx.visited  = visited;
#if defined(PORT_CACHE_SWAP_TRACE)
    ctx.touched  = (uint32_t *)calloc(total_tags_size, sizeof(uint32_t));
    if (const char *w = getenv("PORT_CACHE_SWAP_WATCH")) ctx.watch_va = (uint32_t)strtoul(w, NULL, 16);
    if (from_big_endian) ctx.covered = (uint32_t *)calloc(total_tags_size, sizeof(uint32_t));
#endif
    ctx.from_be  = from_big_endian ? 1 : 0;
    ctx.stats    = out_stats;

    uint32_t th_off = 0;
    if (!va_range_valid(&ctx, tags_header_va, 0x28u, &th_off)) {
        release_ctx(&ctx);
        return 0;
    }
    uint8_t *th = ctx.tag_base + th_off;
    test_and_mark_visited(visited, th_off);

    /* s_cache_file_tags_header (0x28 = 10 x u32) */
    if (ctx.from_be) {
        swap32_n(th, 10);
    }

    int32_t  group_count    = (int32_t)load_u32(th + 0x00);
    uint32_t groups_va      = load_u32(th + 0x04);
    int32_t  instance_count = (int32_t)load_u32(th + 0x08);
    uint32_t instances_va   = load_u32(th + 0x0c);
    int32_t  global_count   = (int32_t)load_u32(th + 0x10);
    uint32_t globals_va     = load_u32(th + 0x14);
    int32_t  fixup_count    = (int32_t)load_u32(th + 0x18);
    uint32_t fixups_va      = load_u32(th + 0x1c);
    uint32_t tags_sig       = load_u32(th + 0x24);

    if (tags_sig != 0x74616773u /* 'tags' */ || group_count < 0 || instance_count < 0) {
        if (!ctx.from_be) swap32_n(th, 10);
        release_ctx(&ctx);
        return 0;
    }

    if (out_stats) {
        out_stats->group_count    = (uint32_t)group_count;
        out_stats->instance_count = (uint32_t)instance_count;
    }

    uint32_t groups_off = 0, instances_off = 0, globals_off = 0, fixups_off = 0;
    if ((group_count > 0 && !va_range_valid(&ctx, groups_va, (uint64_t)(uint32_t)group_count * 16u, &groups_off)) ||
        (instance_count > 0 && !va_range_valid(&ctx, instances_va, (uint64_t)(uint32_t)instance_count * 8u, &instances_off)) ||
        (global_count > 0 && !va_range_valid(&ctx, globals_va, (uint64_t)(uint32_t)global_count * 8u, &globals_off)) ||
        (fixup_count > 0 && !va_range_valid(&ctx, fixups_va, (uint64_t)(uint32_t)fixup_count * 8u, &fixups_off)))
    {
        release_ctx(&ctx);
        return 0;
    }

    uint8_t *groups_ptr    = ctx.tag_base + groups_off;
    uint8_t *instances_ptr = ctx.tag_base + instances_off;
    TRACE_COVER_HEADER(&ctx, th_off, 0x28u);
    TRACE_COVER_HEADER(&ctx, groups_off, (uint32_t)group_count * 16u);
    TRACE_COVER_HEADER(&ctx, instances_off, (uint32_t)instance_count * 8u);
    TRACE_COVER_HEADER(&ctx, globals_off, (uint32_t)(global_count > 0 ? global_count : 0) * 8u);
    TRACE_COVER_HEADER(&ctx, fixups_off, (uint32_t)(fixup_count > 0 ? fixup_count : 0) * 8u);

    if (ctx.from_be) {
        /* cache_file_tag_group: 4 x u32 (group_tags[3], string_id name) */
        if (group_count > 0)    swap32_n(groups_ptr, (uint32_t)group_count * 4u);
        /* global_tag_index: 2 x u32 (group, index) */
        if (global_count > 0)   swap32_n(ctx.tag_base + globals_off, (uint32_t)global_count * 2u);
        /* s_cache_file_tag_interop_type_fixup: 2 x u32 (interop, type_index) */
        if (fixup_count > 0)    swap32_n(ctx.tag_base + fixups_off, (uint32_t)fixup_count * 2u);
        /* cache_file_tag_instance: short group_index, ushort salt, u32 definition */
        for (int32_t i = 0; i < instance_count; ++i) {
            uint8_t *inst = instances_ptr + (uint32_t)i * 8u;
            swap16_n(inst + 0, 2);
            swap32_n(inst + 4, 1);
        }
    }

    /* Walk every non-null tag instance! */
    for (int32_t i = 0; i < instance_count; ++i) {
        uint8_t *inst = instances_ptr + (uint32_t)i * 8u;
        int16_t  gidx   = (int16_t)load_u16(inst + 0);
        uint32_t def_va = load_u32(inst + 4);

        if (gidx < 0 || def_va == 0) {
            if (out_stats) out_stats->null_instances += 1;
            continue;
        }
        if (gidx >= group_count) {
            record_bounds_error(&ctx, (uint32_t)gidx);
            continue;
        }

        uint32_t fourcc = load_u32(groups_ptr + (uint32_t)gidx * 16u + 0u);
        ctx.cur_tag_index = (uint32_t)i;
        ctx.cur_fourcc    = fourcc;

        uint32_t sidx = 0;
        if (!find_group_struct_idx(fourcc, &sidx)) {
            if (out_stats) {
                if (out_stats->unknown_group_instances == 0) {
                    out_stats->unknown_group_fourcc = fourcc;
                }
                out_stats->unknown_group_instances += 1;
            }
            continue;
        }

        uint32_t root_size = k_tag_swap_structs[sidx].size;
        uint32_t def_off   = 0;
        if (!va_range_valid(&ctx, def_va, (uint64_t)root_size, &def_off)) {
            record_bounds_error(&ctx, def_va);
            continue;
        }

        TRACE_COVER(&ctx, def_off, root_size);
        if (!test_and_mark_visited(visited, def_off)) {
            uint8_t *def_ptr = ctx.tag_base + def_off;
            if (ctx.from_be) {
                swap_struct_instance(&ctx, def_ptr, sidx);
                if (fourcc == 0x70686d6fu /* 'phmo' */) {
                    swap_phmo_mopp_data(&ctx, def_ptr);
                } else if (fourcc == 0x7a6f6e65u /* 'zone' */) {
                    swap_zone_fixup_information(&ctx, def_ptr);
                }
            } else {
                /* Inverse order for LE -> BE round-trip: children that read host parent fields run BEFORE parent swaps back! */
                if (fourcc == 0x70686d6fu /* 'phmo' */) {
                    swap_phmo_mopp_data(&ctx, def_ptr);
                } else if (fourcc == 0x7a6f6e65u /* 'zone' */) {
                    swap_zone_fixup_information(&ctx, def_ptr);
                }
                swap_struct_instance(&ctx, def_ptr, sidx);
            }
            if (out_stats) out_stats->swapped_instances += 1;
        }
    }

    if (!ctx.from_be) {
        for (int32_t i = 0; i < instance_count; ++i) {
            uint8_t *inst = instances_ptr + (uint32_t)i * 8u;
            swap16_n(inst + 0, 2);
            swap32_n(inst + 4, 1);
        }
        if (fixup_count > 0)  swap32_n(ctx.tag_base + fixups_off, (uint32_t)fixup_count * 2u);
        if (global_count > 0) swap32_n(ctx.tag_base + globals_off, (uint32_t)global_count * 2u);
        if (group_count > 0)  swap32_n(groups_ptr, (uint32_t)group_count * 4u);
        swap32_n(th, 10);
    }

#if defined(PORT_CACHE_SWAP_TRACE)
    if (ctx.covered) trace_report_coverage(&ctx);
#endif
    release_ctx(&ctx);
    /* back in the cache file's order the blocks point at the shared data again: the copies are unused */
    if (!from_big_endian) release_private_copies(tag_buffer);
    if (out_stats && (out_stats->bounds_errors > 0 || out_stats->unknown_group_instances > 0)) {
        return 0;
    }
    if (from_big_endian) port_resource_register_page_index(swapped_header_0x3000, tag_buffer);
    return 1;
}
