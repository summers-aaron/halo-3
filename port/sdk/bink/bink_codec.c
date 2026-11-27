// port/sdk/bink (both hosts): Bink 1 container, video and audio decoding (interface and file layout: bink_codec.h).
//
// The video decoder follows FFmpeg n6.1 libavcodec/bink.c (bink_decode_plane and its bundle readers) and
// binkdsp.c (the Bink IDCT, scale_block, add_pixels8) for revisions 'f' .. 'k', function by function with FFmpeg's
// names, so it can be checked against it line by line and decodes bit-exactly the same planes; the audio decoder
// follows binkaudio.c (decode_block and the packet loop) for the DCT and RDFT variants. Rewritten without
// libavcodec's infrastructure: a plain LSB-first bit reader, flat lookup tables for the 16 trees, a radix-2 complex
// FFT for the transforms, and table-driven math, so neither libm nor an allocator is linked. Where this differs
// from FFmpeg (only for corrupt data), the comment says so:
//   - bundles keep one chunk of values at a time (a chunk is read only when the last is used up, so it starts the
//     buffer over), which needs buffers of one chunk's size instead of a whole plane's;
//   - a bundle read past its values returns 0 instead of reading on through FFmpeg's plane-sized buffer;
//   - a 16x16 block that would cross the plane's last block column or row is clipped to the plane (FFmpeg writes it
//     into its frame padding; the visible pixels are the same).
//
// FFmpeg is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either version 2.1 of the License, or (at your option) any
// later version. The original copyright lines:
//   bink.c, binkdsp.c: Copyright (c) 2009 Konstantin Shishkov, 2011 Peter Ross <pross@xvid.org>
//   binkaudio.c: Copyright (c) 2007-2011 Peter Ross (pross@xvid.org), 2009 Daniel Verkamp (daniel@drv.nu)
// This file is distributed under the same license (LGPL 2.1 or later).
#include "bink_codec.h"
#include "bink_tables.h"

#include <stdint.h>
#include <stddef.h>
// No C library headers (the Windows runtime builds without them): the compiler's own memset / memcpy.
#define memset __builtin_memset
#define memcpy __builtin_memcpy

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

static uint32_t rd32(const uint8_t *p) { return (uint32_t)p[0] | (uint32_t)p[1] << 8 | (uint32_t)p[2] << 16 | (uint32_t)p[3] << 24; }
static uint32_t rd16(const uint8_t *p) { return (uint32_t)p[0] | (uint32_t)p[1] << 8; }

// ---- container -----------------------------------------------------------------------------------------------------

int PORT_MS_ABI bink_codec_parse_header(const void *data, unsigned size, bink_codec_header *header)
{
    const uint8_t *p = (const uint8_t *)data;
    memset(header, 0, sizeof(*header));
    if (size < 44 || p[0] != 'B' || p[1] != 'I' || p[2] != 'K')
        return 0;
    const unsigned revision = p[3];
    if (revision != 'f' && revision != 'g' && revision != 'h' && revision != 'i' && revision != 'k')
        return 0;
    header->revision = revision;
    header->file_size = rd32(p + 4) + 8;
    header->frame_count = rd32(p + 8);
    header->largest_frame = rd32(p + 12);
    header->width = rd32(p + 20);
    header->height = rd32(p + 24);
    header->rate = rd32(p + 28);
    header->rate_divisor = rd32(p + 32);
    header->video_flags = rd32(p + 36);
    header->track_count = rd32(p + 40);
    unsigned offset = 44;
    if (revision == 'k')
        offset += 4; // a field of the newer files
    if (header->frame_count == 0 || header->frame_count > 1000000 || header->width == 0 || header->width > 7680 ||
        header->height == 0 || header->height > 4800 || header->rate == 0 || header->rate_divisor == 0 ||
        header->track_count > BINK_CODEC_MAX_TRACKS || header->largest_frame > header->file_size)
        return 0;
    header->tracks_offset = offset;
    header->index_offset = offset + header->track_count * 12;
    header->header_size = header->index_offset + 4 * (header->frame_count + 1);
    return size >= offset;
}

void PORT_MS_ABI bink_codec_track(const void *header_bytes, const bink_codec_header *header, unsigned track, unsigned *max_bytes,
    unsigned *sample_rate, unsigned *flags, unsigned *id)
{
    const uint8_t *p = (const uint8_t *)header_bytes + header->tracks_offset;
    const unsigned count = header->track_count;
    if (max_bytes)
        *max_bytes = rd32(p + 4 * track);
    if (sample_rate)
        *sample_rate = rd16(p + 4 * count + 4 * track);
    if (flags)
        *flags = rd16(p + 4 * count + 4 * track + 2);
    if (id)
        *id = rd32(p + 8 * count + 4 * track);
}

int PORT_MS_ABI bink_codec_frame_extent(const void *index, unsigned frame_count, unsigned frame, unsigned *offset, unsigned *size,
    int *key)
{
    const uint8_t *p = (const uint8_t *)index;
    if (frame >= frame_count)
        return 0;
    const uint32_t entry = rd32(p + 4 * frame), next = rd32(p + 4 * (frame + 1));
    const uint32_t start = entry & ~1u, end = next & ~1u;
    if (end <= start)
        return 0;
    *offset = start;
    *size = end - start;
    if (key)
        *key = frame == 0 || (entry & 1) != 0;
    return 1;
}

int PORT_MS_ABI bink_codec_frame_part(const void *frame, unsigned frame_size, unsigned track_count, unsigned track,
    const void **part, unsigned *part_size)
{
    const uint8_t *p = (const uint8_t *)frame;
    unsigned at = 0;
    for (unsigned t = 0; t < track_count; t++) {
        if (frame_size - at < 4)
            return 0;
        const uint32_t bytes = rd32(p + at);
        if (bytes > frame_size - at - 4)
            return 0;
        if (t == track) {
            *part = p + at + 4;
            *part_size = bytes;
            return 1;
        }
        at += 4 + bytes;
    }
    if (track != track_count)
        return 0;
    *part = p + at;
    *part_size = frame_size - at;
    return 1;
}

// ---- bit reader (BITSTREAM_READER_LE: bits are taken from the least significant end of little-endian bytes) --------

typedef struct bitreader {
    const uint8_t *data;
    uint32_t bytes;
    uint32_t bits;
    uint32_t pos;
} bitreader;

static void br_init(bitreader *b, const void *data, uint32_t bytes)
{
    b->data = (const uint8_t *)data;
    b->bytes = bytes;
    b->bits = bytes * 8u;
    b->pos = 0;
}

// The next 57 or more bits; past the end of the data, zeros (as FFmpeg's padded packets read).
static inline uint64_t br_window(const bitreader *b)
{
    const uint32_t byte = b->pos >> 3;
    uint64_t v = 0;
    if (byte + 8u <= b->bytes && byte + 8u > byte) {
        memcpy(&v, b->data + byte, 8); // the hosts are little-endian
    } else {
        for (uint32_t i = 0; i < 8u && byte + i < b->bytes; i++)
            v |= (uint64_t)b->data[byte + i] << (8 * i);
    }
    return v >> (b->pos & 7);
}

static inline uint32_t get_bits(bitreader *b, int n)
{
    if (n <= 0)
        return 0;
    const uint32_t v = (uint32_t)(br_window(b) & ((n >= 32) ? 0xffffffffull : ((1ull << n) - 1)));
    b->pos += (uint32_t)n;
    return v;
}

static inline uint32_t get_bits1(bitreader *b) { return get_bits(b, 1); }
static inline void skip_bits(bitreader *b, uint32_t n) { b->pos += n; }
static inline int64_t get_bits_left(const bitreader *b) { return (int64_t)b->bits - (int64_t)b->pos; }
static inline uint32_t get_bits_count(const bitreader *b) { return b->pos; }

static int av_log2(uint32_t v)
{
    int n = 0;
    while (v > 1) {
        v >>= 1;
        n++;
    }
    return n;
}

// ---- video ---------------------------------------------------------------------------------------------------------

enum Sources {
    BINK_SRC_BLOCK_TYPES = 0, // 8x8 block types
    BINK_SRC_SUB_BLOCK_TYPES, // 16x16 block types (a subset of 8x8 block types)
    BINK_SRC_COLORS,          // pixel values used for different block types
    BINK_SRC_PATTERN,         // 8-bit values for 2-colour pattern fill
    BINK_SRC_X_OFF,           // X components of motion value
    BINK_SRC_Y_OFF,           // Y components of motion value
    BINK_SRC_INTRA_DC,        // DC values for intrablocks with DCT
    BINK_SRC_INTER_DC,        // DC values for interblocks with DCT
    BINK_SRC_RUN,             // run lengths for special fill block
    BINK_NB_SRC
};

enum BlockTypes {
    SKIP_BLOCK = 0, // skipped block
    SCALED_BLOCK,   // block has size 16x16
    MOTION_BLOCK,   // block is copied from previous frame with some offset
    RUN_BLOCK,      // block is composed from runs of colours with custom scan order
    RESIDUE_BLOCK,  // motion block with some difference added
    INTRA_BLOCK,    // intra DCT block
    FILL_BLOCK,     // block is filled with single colour
    INTER_BLOCK,    // motion block with DCT applied to the difference
    PATTERN_BLOCK,  // block is filled with two colours following custom pattern
    RAW_BLOCK,      // uncoded 8x8 block
};

typedef struct vlc_entry {
    uint8_t sym, len;
} vlc_entry;

typedef struct Tree {
    int vlc_num;      // tree number (in bink_tree_*)
    uint8_t syms[16]; // leaf value to symbol mapping
} Tree;

typedef struct Bundle {
    int len;           // length of number of entries to decode (in bits)
    Tree tree;         // Huffman tree-related data
    uint8_t *data;     // buffer for decoded symbols
    uint8_t *data_end; // buffer end
    uint8_t *cur_dec;  // pointer to the not yet decoded part of the buffer
    uint8_t *cur_ptr;  // pointer to the data that is not read from buffer yet
} Bundle;

#define BUNDLE_GUARD 128u

struct bink_video {
    unsigned width, height;
    int version;
    int has_alpha;
    int swap_planes;
    Bundle bundle[BINK_NB_SRC];
    Tree col_high[16]; // trees for decoding high nibble in "colours" data type
    int col_lastval;   // value of last decoded high nibble in "colours" data type
    uint8_t vlc_bits[16];
    vlc_entry vlc[16][128];
    unsigned bundle_capacity[BINK_NB_SRC];
    uint8_t storage[]; // the bundle buffers
};

// The lengths init_lengths gives the bundles of a plane `width` pixels wide with bw 8x8 blocks a row.
static void bundle_lengths(int lengths[BINK_NB_SRC], int width, int bw)
{
    width = (width + 7) & ~7;
    lengths[BINK_SRC_BLOCK_TYPES] = av_log2((width >> 3) + 511) + 1;
    lengths[BINK_SRC_SUB_BLOCK_TYPES] = av_log2((width >> 4) + 511) + 1;
    lengths[BINK_SRC_COLORS] = av_log2(bw * 64 + 511) + 1;
    lengths[BINK_SRC_INTRA_DC] = lengths[BINK_SRC_INTER_DC] = lengths[BINK_SRC_X_OFF] = lengths[BINK_SRC_Y_OFF] =
        av_log2((width >> 3) + 511) + 1;
    lengths[BINK_SRC_PATTERN] = av_log2((bw << 3) + 511) + 1;
    lengths[BINK_SRC_RUN] = av_log2(bw * 48 + 511) + 1;
}

// One chunk of a bundle is at most 2^len - 1 values (two bytes each for the DCs); the luma plane's lengths are the
// largest. Sizes rounded to 16 bytes.
static void bundle_capacities(unsigned capacity[BINK_NB_SRC], unsigned width)
{
    int lengths[BINK_NB_SRC];
    bundle_lengths(lengths, (int)MAX(width, 8u), (int)((width + 7) >> 3));
    for (int i = 0; i < BINK_NB_SRC; i++)
        capacity[i] = ((2u << lengths[i]) + 15u) & ~15u;
}

unsigned PORT_MS_ABI bink_video_memory(unsigned width, unsigned height)
{
    (void)height;
    unsigned capacity[BINK_NB_SRC], total = (unsigned)sizeof(struct bink_video) + 16u;
    bundle_capacities(capacity, width);
    for (int i = 0; i < BINK_NB_SRC; i++)
        total += capacity[i] + BUNDLE_GUARD;
    return total;
}

static void init_lengths(struct bink_video *c, int width, int bw)
{
    int lengths[BINK_NB_SRC];
    bundle_lengths(lengths, width, bw);
    for (int i = 0; i < BINK_NB_SRC; i++)
        c->bundle[i].len = lengths[i];
}

static void bink_init_vlcs(struct bink_video *c)
{
    for (int t = 0; t < 16; t++) {
        const int maxbits = bink_tree_lens[t][15];
        c->vlc_bits[t] = (uint8_t)maxbits;
        for (int s = 0; s < 16; s++) {
            const int len = bink_tree_lens[t][s], code = bink_tree_bits[t][s];
            for (int k = 0; k < (1 << (maxbits - len)); k++) {
                vlc_entry *e = &c->vlc[t][code | (k << len)];
                e->sym = (uint8_t)s;
                e->len = (uint8_t)len;
            }
        }
    }
}

struct bink_video * PORT_MS_ABI bink_video_create(void *memory, unsigned width, unsigned height, unsigned revision,
    unsigned video_flags)
{
    struct bink_video *c = (struct bink_video *)(((uintptr_t)memory + 15u) & ~(uintptr_t)15u);
    memset(c, 0, sizeof(*c));
    c->width = width;
    c->height = height;
    c->version = (int)revision;
    c->has_alpha = (video_flags & BINK_CODEC_FLAG_ALPHA) != 0;
    c->swap_planes = c->version >= 'h';
    bink_init_vlcs(c);
    bundle_capacities(c->bundle_capacity, width);
    uint8_t *at = c->storage;
    for (int i = 0; i < BINK_NB_SRC; i++) {
        c->bundle[i].data = at;
        c->bundle[i].data_end = at + c->bundle_capacity[i];
        memset(at, 0, c->bundle_capacity[i] + BUNDLE_GUARD);
        at += c->bundle_capacity[i] + BUNDLE_GUARD;
    }
    return c;
}

static inline int get_vlc(bitreader *gb, const struct bink_video *c, int vlc_num)
{
    const int bits = c->vlc_bits[vlc_num];
    const vlc_entry e = c->vlc[vlc_num][(uint32_t)br_window(gb) & ((1u << bits) - 1u)];
    gb->pos += e.len;
    return e.sym;
}

#define GET_HUFF(gb, tree) (tree).syms[get_vlc(gb, c, (tree).vlc_num)]

// Merge two consequent lists of equal size depending on bits read.
static void merge(bitreader *gb, uint8_t *dst, uint8_t *src, int size)
{
    uint8_t *src2 = src + size;
    int size2 = size;
    do {
        if (!get_bits1(gb)) {
            *dst++ = *src++;
            size--;
        } else {
            *dst++ = *src2++;
            size2--;
        }
    } while (size && size2);
    while (size--)
        *dst++ = *src++;
    while (size2--)
        *dst++ = *src2++;
}

// Read information about Huffman tree used to decode data.
static int read_tree(bitreader *gb, Tree *tree)
{
    uint8_t tmp1[16] = { 0 }, tmp2[16], *in = tmp1, *out = tmp2;
    int i, t, len;
    if (get_bits_left(gb) < 4)
        return -1;
    tree->vlc_num = (int)get_bits(gb, 4);
    if (!tree->vlc_num) {
        for (i = 0; i < 16; i++)
            tree->syms[i] = (uint8_t)i;
        return 0;
    }
    if (get_bits1(gb)) {
        len = (int)get_bits(gb, 3);
        for (i = 0; i <= len; i++) {
            tree->syms[i] = (uint8_t)get_bits(gb, 4);
            tmp1[tree->syms[i]] = 1;
        }
        for (i = 0; i < 16 && len < 16 - 1; i++)
            if (!tmp1[i])
                tree->syms[++len] = (uint8_t)i;
    } else {
        len = (int)get_bits(gb, 2);
        for (i = 0; i < 16; i++)
            in[i] = (uint8_t)i;
        for (i = 0; i <= len; i++) {
            int size = 1 << i;
            for (t = 0; t < 16; t += size << 1)
                merge(gb, out + t, in + t, size);
            uint8_t *swap = in;
            in = out;
            out = swap;
        }
        memcpy(tree->syms, in, 16);
    }
    return 0;
}

// Prepare bundle for decoding data.
static int read_bundle(bitreader *gb, struct bink_video *c, int bundle_num)
{
    if (bundle_num == BINK_SRC_COLORS) {
        for (int i = 0; i < 16; i++)
            if (read_tree(gb, &c->col_high[i]) < 0)
                return -1;
        c->col_lastval = 0;
    }
    if (bundle_num != BINK_SRC_INTRA_DC && bundle_num != BINK_SRC_INTER_DC)
        if (read_tree(gb, &c->bundle[bundle_num].tree) < 0)
            return -1;
    c->bundle[bundle_num].cur_dec = c->bundle[bundle_num].cur_ptr = c->bundle[bundle_num].data;
    return 0;
}

// The common check before decoding bundle data (FFmpeg's CHECK_READ_VAL); then the used-up chunk's buffer is
// started over (see the top of the file).
#define CHECK_READ_VAL(gb, b, t)                      \
    if (!b->cur_dec || (b->cur_dec > b->cur_ptr))     \
        return 0;                                     \
    t = (int)get_bits(gb, b->len);                    \
    if (!t) {                                         \
        b->cur_dec = NULL;                            \
        return 0;                                     \
    }                                                 \
    b->cur_dec = b->cur_ptr = b->data;

static int read_runs(struct bink_video *c, bitreader *gb, Bundle *b)
{
    int t, v;
    const uint8_t *dec_end;
    CHECK_READ_VAL(gb, b, t);
    dec_end = b->cur_dec + t;
    if (dec_end > b->data_end)
        return -1;
    if (get_bits_left(gb) < 1)
        return -1;
    if (get_bits1(gb)) {
        v = (int)get_bits(gb, 4);
        memset(b->cur_dec, v, t);
        b->cur_dec += t;
    } else {
        while (b->cur_dec < dec_end)
            *b->cur_dec++ = GET_HUFF(gb, b->tree);
    }
    return 0;
}

static int read_motion_values(struct bink_video *c, bitreader *gb, Bundle *b)
{
    int t, sign, v;
    const uint8_t *dec_end;
    CHECK_READ_VAL(gb, b, t);
    dec_end = b->cur_dec + t;
    if (dec_end > b->data_end)
        return -1;
    if (get_bits_left(gb) < 1)
        return -1;
    if (get_bits1(gb)) {
        v = (int)get_bits(gb, 4);
        if (v) {
            sign = -(int)get_bits1(gb);
            v = (v ^ sign) - sign;
        }
        memset(b->cur_dec, v, t);
        b->cur_dec += t;
    } else {
        while (b->cur_dec < dec_end) {
            v = GET_HUFF(gb, b->tree);
            if (v) {
                sign = -(int)get_bits1(gb);
                v = (v ^ sign) - sign;
            }
            *b->cur_dec++ = (uint8_t)v;
        }
    }
    return 0;
}

static const uint8_t bink_rlelens[4] = { 4, 8, 12, 32 };

static int read_block_types(struct bink_video *c, bitreader *gb, Bundle *b)
{
    int t, v;
    int last = 0;
    const uint8_t *dec_end;
    CHECK_READ_VAL(gb, b, t);
    if (c->version == 'k') {
        t ^= 0xBB;
        if (t == 0) {
            b->cur_dec = NULL;
            return 0;
        }
    }
    dec_end = b->cur_dec + t;
    if (dec_end > b->data_end)
        return -1;
    if (get_bits_left(gb) < 1)
        return -1;
    if (get_bits1(gb)) {
        v = (int)get_bits(gb, 4);
        memset(b->cur_dec, v, t);
        b->cur_dec += t;
    } else {
        while (b->cur_dec < dec_end) {
            v = GET_HUFF(gb, b->tree);
            if (v < 12) {
                last = v;
                *b->cur_dec++ = (uint8_t)v;
            } else {
                int run = bink_rlelens[v - 12];
                if (dec_end - b->cur_dec < run)
                    return -1;
                memset(b->cur_dec, last, run);
                b->cur_dec += run;
            }
        }
    }
    return 0;
}

static int read_patterns(struct bink_video *c, bitreader *gb, Bundle *b)
{
    int t, v;
    const uint8_t *dec_end;
    CHECK_READ_VAL(gb, b, t);
    dec_end = b->cur_dec + t;
    if (dec_end > b->data_end)
        return -1;
    while (b->cur_dec < dec_end) {
        if (get_bits_left(gb) < 2)
            return -1;
        v = GET_HUFF(gb, b->tree);
        v |= GET_HUFF(gb, b->tree) << 4;
        *b->cur_dec++ = (uint8_t)v;
    }
    return 0;
}

static int read_colors(struct bink_video *c, bitreader *gb, Bundle *b)
{
    int t, sign, v;
    const uint8_t *dec_end;
    CHECK_READ_VAL(gb, b, t);
    dec_end = b->cur_dec + t;
    if (dec_end > b->data_end)
        return -1;
    if (get_bits1(gb)) {
        c->col_lastval = GET_HUFF(gb, c->col_high[c->col_lastval]);
        v = GET_HUFF(gb, b->tree);
        v = (c->col_lastval << 4) | v;
        if (c->version < 'i') {
            sign = ((int8_t)v) >> 7;
            v = ((v & 0x7F) ^ sign) - sign;
            v += 0x80;
        }
        memset(b->cur_dec, v, t);
        b->cur_dec += t;
    } else {
        while (b->cur_dec < dec_end) {
            if (get_bits_left(gb) < 2)
                return -1;
            c->col_lastval = GET_HUFF(gb, c->col_high[c->col_lastval]);
            v = GET_HUFF(gb, b->tree);
            v = (c->col_lastval << 4) | v;
            if (c->version < 'i') {
                sign = ((int8_t)v) >> 7;
                v = ((v & 0x7F) ^ sign) - sign;
                v += 0x80;
            }
            *b->cur_dec++ = (uint8_t)v;
        }
    }
    return 0;
}

// number of bits used to store first DC value in bundle
#define DC_START_BITS 11

static int read_dcs(bitreader *gb, Bundle *b, int start_bits, int has_sign)
{
    int i, j, len, len2, bsize, sign, v, v2;
    CHECK_READ_VAL(gb, b, len);
    uint8_t *dst = b->cur_dec;
    const uint8_t *dst_end = b->data_end;
    v = (int)get_bits(gb, start_bits - has_sign);
    if (v && has_sign) {
        sign = -(int)get_bits1(gb);
        v = (v ^ sign) - sign;
    }
    if (dst_end - dst < 2)
        return -1;
    int16_t value = (int16_t)v;
    memcpy(dst, &value, 2);
    dst += 2;
    len--;
    for (i = 0; i < len; i += 8) {
        len2 = MIN(len - i, 8);
        if (dst_end - dst < 2 * len2)
            return -1;
        bsize = (int)get_bits(gb, 4);
        if (bsize) {
            for (j = 0; j < len2; j++) {
                v2 = (int)get_bits(gb, bsize);
                if (v2) {
                    sign = -(int)get_bits1(gb);
                    v2 = (v2 ^ sign) - sign;
                }
                v += v2;
                value = (int16_t)v;
                memcpy(dst, &value, 2);
                dst += 2;
                if (v < -32768 || v > 32767)
                    return -1;
            }
        } else {
            for (j = 0; j < len2; j++) {
                value = (int16_t)v;
                memcpy(dst, &value, 2);
                dst += 2;
            }
        }
    }
    b->cur_dec = dst;
    return 0;
}

// A value of a bundle; past its decoded values (corrupt data only), 0.
static inline int get_value(struct bink_video *c, int bundle)
{
    Bundle *b = &c->bundle[bundle];
    if (b->cur_ptr + 2 > b->data_end + BUNDLE_GUARD)
        return 0;
    if (bundle < BINK_SRC_X_OFF || bundle == BINK_SRC_RUN)
        return *b->cur_ptr++;
    if (bundle == BINK_SRC_X_OFF || bundle == BINK_SRC_Y_OFF)
        return (int8_t)*b->cur_ptr++;
    int16_t v;
    memcpy(&v, b->cur_ptr, 2);
    b->cur_ptr += 2;
    return v;
}

// Read 8x8 block of DCT coefficients.
static int read_dct_coeffs(bitreader *gb, int32_t block[64], const uint8_t *scan, int *coef_count_, int coef_idx[64],
    int q)
{
    int coef_list[128];
    int mode_list[128];
    int i, t, bits, ccoef, mode, sign;
    int list_start = 64, list_end = 64, list_pos;
    int coef_count = 0;
    int quant_idx;

    if (get_bits_left(gb) < 4)
        return -1;

    coef_list[list_end] = 4;  mode_list[list_end++] = 0;
    coef_list[list_end] = 24; mode_list[list_end++] = 0;
    coef_list[list_end] = 44; mode_list[list_end++] = 0;
    coef_list[list_end] = 1;  mode_list[list_end++] = 3;
    coef_list[list_end] = 2;  mode_list[list_end++] = 3;
    coef_list[list_end] = 3;  mode_list[list_end++] = 3;

    for (bits = (int)get_bits(gb, 4) - 1; bits >= 0; bits--) {
        list_pos = list_start;
        while (list_pos < list_end) {
            if (!(mode_list[list_pos] | coef_list[list_pos]) || !get_bits1(gb)) {
                list_pos++;
                continue;
            }
            ccoef = coef_list[list_pos];
            mode = mode_list[list_pos];
            switch (mode) {
            case 0:
                coef_list[list_pos] = ccoef + 4;
                mode_list[list_pos] = 1;
                // fall through
            case 2:
                if (mode == 2) {
                    coef_list[list_pos] = 0;
                    mode_list[list_pos++] = 0;
                }
                for (i = 0; i < 4; i++, ccoef++) {
                    if (get_bits1(gb)) {
                        coef_list[--list_start] = ccoef;
                        mode_list[list_start] = 3;
                    } else {
                        if (!bits) {
                            t = 1 - (int)(get_bits1(gb) << 1);
                        } else {
                            t = (int)(get_bits(gb, bits) | 1u << bits);
                            sign = -(int)get_bits1(gb);
                            t = (t ^ sign) - sign;
                        }
                        block[scan[ccoef]] = t;
                        coef_idx[coef_count++] = ccoef;
                    }
                }
                break;
            case 1:
                mode_list[list_pos] = 2;
                for (i = 0; i < 3; i++) {
                    ccoef += 4;
                    coef_list[list_end] = ccoef;
                    mode_list[list_end++] = 2;
                }
                break;
            case 3:
                if (!bits) {
                    t = 1 - (int)(get_bits1(gb) << 1);
                } else {
                    t = (int)(get_bits(gb, bits) | 1u << bits);
                    sign = -(int)get_bits1(gb);
                    t = (t ^ sign) - sign;
                }
                block[scan[ccoef]] = t;
                coef_idx[coef_count++] = ccoef;
                coef_list[list_pos] = 0;
                mode_list[list_pos++] = 0;
                break;
            }
            // a corrupt block can push the lists past their ends
            if (list_start < 1 || list_end > 124)
                return -1;
        }
    }

    if (q == -1) {
        quant_idx = (int)get_bits(gb, 4);
    } else {
        quant_idx = q;
        if ((unsigned)quant_idx > 15u)
            return -1;
    }
    *coef_count_ = coef_count;
    return quant_idx;
}

static void unquantize_dct_coeffs(int32_t block[64], const int32_t quant[64], int coef_count, int coef_idx[64],
    const uint8_t *scan)
{
    block[0] = (int)((uint32_t)block[0] * (uint32_t)quant[0]) >> 11;
    for (int i = 0; i < coef_count; i++) {
        int idx = coef_idx[i];
        block[scan[idx]] = (int)((uint32_t)block[scan[idx]] * (uint32_t)quant[idx]) >> 11;
    }
}

// Read 8x8 block with residue after motion compensation.
static int read_residue(bitreader *gb, int16_t block[64], int masks_count)
{
    int coef_list[128];
    int mode_list[128];
    int i, sign, mask, ccoef, mode;
    int list_start = 64, list_end = 64, list_pos;
    int nz_coeff[64];
    int nz_coeff_count = 0;

    coef_list[list_end] = 4;  mode_list[list_end++] = 0;
    coef_list[list_end] = 24; mode_list[list_end++] = 0;
    coef_list[list_end] = 44; mode_list[list_end++] = 0;
    coef_list[list_end] = 0;  mode_list[list_end++] = 2;

    for (mask = 1 << get_bits(gb, 3); mask; mask >>= 1) {
        for (i = 0; i < nz_coeff_count; i++) {
            if (!get_bits1(gb))
                continue;
            if (block[nz_coeff[i]] < 0)
                block[nz_coeff[i]] -= mask;
            else
                block[nz_coeff[i]] += mask;
            masks_count--;
            if (masks_count < 0)
                return 0;
        }
        list_pos = list_start;
        while (list_pos < list_end) {
            if (!(coef_list[list_pos] | mode_list[list_pos]) || !get_bits1(gb)) {
                list_pos++;
                continue;
            }
            ccoef = coef_list[list_pos];
            mode = mode_list[list_pos];
            switch (mode) {
            case 0:
                coef_list[list_pos] = ccoef + 4;
                mode_list[list_pos] = 1;
                // fall through
            case 2:
                if (mode == 2) {
                    coef_list[list_pos] = 0;
                    mode_list[list_pos++] = 0;
                }
                for (i = 0; i < 4; i++, ccoef++) {
                    if (get_bits1(gb)) {
                        coef_list[--list_start] = ccoef;
                        mode_list[list_start] = 3;
                    } else {
                        if (nz_coeff_count >= 64)
                            return -1;
                        nz_coeff[nz_coeff_count++] = bink_scan[ccoef];
                        sign = -(int)get_bits1(gb);
                        block[bink_scan[ccoef]] = (int16_t)((mask ^ sign) - sign);
                        masks_count--;
                        if (masks_count < 0)
                            return 0;
                    }
                }
                break;
            case 1:
                mode_list[list_pos] = 2;
                for (i = 0; i < 3; i++) {
                    ccoef += 4;
                    coef_list[list_end] = ccoef;
                    mode_list[list_end++] = 2;
                }
                break;
            case 3:
                if (nz_coeff_count >= 64)
                    return -1;
                nz_coeff[nz_coeff_count++] = bink_scan[ccoef];
                sign = -(int)get_bits1(gb);
                block[bink_scan[ccoef]] = (int16_t)((mask ^ sign) - sign);
                coef_list[list_pos] = 0;
                mode_list[list_pos++] = 0;
                masks_count--;
                if (masks_count < 0)
                    return 0;
                break;
            }
            if (list_start < 1 || list_end > 124)
                return -1;
        }
    }
    return 0;
}

// ---- binkdsp.c ----

#define A1 2896 /* (1/sqrt(2))<<12 */
#define A2 2217
#define A3 3784
#define A4 -5352

#define MUL(X, Y) ((int)((unsigned)(X) * (Y)) >> 11)

#define IDCT_TRANSFORM(dest, s0, s1, s2, s3, s4, s5, s6, s7, d0, d1, d2, d3, d4, d5, d6, d7, munge, src) \
    {                                                                                                   \
        const int a0 = (src)[s0] + (src)[s4];                                                           \
        const int a1 = (src)[s0] - (src)[s4];                                                           \
        const int a2 = (src)[s2] + (src)[s6];                                                           \
        const int a3 = MUL(A1, (src)[s2] - (src)[s6]);                                                  \
        const int a4 = (src)[s5] + (src)[s3];                                                           \
        const int a5 = (src)[s5] - (src)[s3];                                                           \
        const int a6 = (src)[s1] + (src)[s7];                                                           \
        const int a7 = (src)[s1] - (src)[s7];                                                           \
        const int b0 = a4 + a6;                                                                         \
        const int b1 = MUL(A3, a5 + a7);                                                                \
        const int b2 = MUL(A4, a5) - b0 + b1;                                                           \
        const int b3 = MUL(A1, a6 - a4) - b2;                                                           \
        const int b4 = MUL(A2, a7) + b3 - b1;                                                           \
        (dest)[d0] = munge(a0 + a2 + b0);                                                               \
        (dest)[d1] = munge(a1 + a3 - a2 + b2);                                                          \
        (dest)[d2] = munge(a1 - a3 + a2 + b3);                                                          \
        (dest)[d3] = munge(a0 - a2 - b4);                                                               \
        (dest)[d4] = munge(a0 - a2 + b4);                                                               \
        (dest)[d5] = munge(a1 - a3 + a2 - b3);                                                          \
        (dest)[d6] = munge(a1 + a3 - a2 - b2);                                                          \
        (dest)[d7] = munge(a0 + a2 - b0);                                                               \
    }

#define MUNGE_NONE(x) (x)
#define IDCT_COL(dest, src) IDCT_TRANSFORM(dest, 0, 8, 16, 24, 32, 40, 48, 56, 0, 8, 16, 24, 32, 40, 48, 56, MUNGE_NONE, src)

#define MUNGE_ROW(x) (((x) + 0x7F) >> 8)
#define IDCT_ROW(dest, src) IDCT_TRANSFORM(dest, 0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7, MUNGE_ROW, src)

static inline void bink_idct_col(int *dest, const int32_t *src)
{
    if ((src[8] | src[16] | src[24] | src[32] | src[40] | src[48] | src[56]) == 0) {
        dest[0] = dest[8] = dest[16] = dest[24] = dest[32] = dest[40] = dest[48] = dest[56] = src[0];
    } else {
        IDCT_COL(dest, src);
    }
}

static void bink_idct_c(int32_t *block)
{
    int temp[64];
    for (int i = 0; i < 8; i++)
        bink_idct_col(&temp[i], &block[i]);
    for (int i = 0; i < 8; i++) {
        IDCT_ROW((&block[8 * i]), (&temp[8 * i]));
    }
}

static void bink_idct_add_c(uint8_t *dest, int linesize, int32_t *block)
{
    bink_idct_c(block);
    for (int i = 0; i < 8; i++, dest += linesize, block += 8)
        for (int j = 0; j < 8; j++)
            dest[j] += block[j];
}

static void bink_idct_put_c(uint8_t *dest, int linesize, int32_t *block)
{
    int temp[64];
    for (int i = 0; i < 8; i++)
        bink_idct_col(&temp[i], &block[i]);
    for (int i = 0; i < 8; i++) {
        IDCT_ROW((&dest[i * linesize]), (&temp[8 * i]));
    }
}

// A 16x16 block from 8x8 source pixels (each doubled), clipped to `w` x `h` pixels.
static void scale_block_c(const uint8_t src[64], uint8_t *dst, int linesize, int w, int h)
{
    for (int j = 0; j < 16 && j < h; j++) {
        const uint8_t *s = src + (j >> 1) * 8;
        uint8_t *d = dst + j * linesize;
        for (int i = 0; i < 16 && i < w; i++)
            d[i] = s[i >> 1];
    }
}

static void add_pixels8_c(uint8_t *pixels, int16_t *block, int line_size)
{
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++)
            pixels[j] = (uint8_t)(pixels[j] + block[j]);
        pixels += line_size;
        block += 8;
    }
}

static void put_pixels8x8(uint8_t *dst, const uint8_t *src, int stride)
{
    if (dst == src)
        return;
    for (int i = 0; i < 8; i++)
        memcpy(dst + i * stride, src + i * stride, 8);
}

static void fill_block(uint8_t *dst, int value, int stride, int w, int h)
{
    for (int i = 0; i < h; i++)
        memset(dst + i * stride, value, w);
}

static int bink_put_pixels(struct bink_video *c, uint8_t *dst, const uint8_t *ref_start, intptr_t at, int stride,
    intptr_t ref_end)
{
    const int xoff = get_value(c, BINK_SRC_X_OFF);
    const int yoff = get_value(c, BINK_SRC_Y_OFF);
    const intptr_t ref = at + xoff + (intptr_t)yoff * stride;
    if (ref < 0 || ref > ref_end)
        return -1;
    put_pixels8x8(dst, ref_start + ref, stride);
    return 0;
}

// One plane of a frame: `frame` / `last` the plane in the frame being decoded and in the one before (last NULL:
// none, so `frame` is its own reference).
static int bink_decode_plane(struct bink_video *c, bitreader *gb, uint8_t *frame, const uint8_t *last, int stride,
    int is_chroma)
{
    int blk, ret;
    int i, j, bx, by;
    uint8_t *dst;
    int v, col[2];
    const uint8_t *scan;
    int16_t block[64];
    uint8_t ublock[64];
    int32_t dctblock[64];
    int coordmap[64], quant_idx, coef_count, coef_idx[64];

    const int bw = is_chroma ? (int)(c->width + 15) >> 4 : (int)(c->width + 7) >> 3;
    const int bh = is_chroma ? (int)(c->height + 15) >> 4 : (int)(c->height + 7) >> 3;
    const int width = (int)c->width >> is_chroma;
    const int height = (int)c->height >> is_chroma;

    if (c->version == 'k' && get_bits1(gb)) {
        int fill = (int)get_bits(gb, 8);
        for (i = 0; i < height; i++)
            memset(frame + i * stride, fill, width);
        goto end;
    }

    init_lengths(c, MAX(width, 8), bw);
    for (i = 0; i < BINK_NB_SRC; i++)
        if (read_bundle(gb, c, i) < 0)
            return -1;

    const uint8_t *ref_start = last ? last : frame;
    const intptr_t ref_end = ((intptr_t)bw - 1 + (intptr_t)stride * (bh - 1)) * 8;

    for (i = 0; i < 64; i++)
        coordmap[i] = (i & 7) + (i >> 3) * stride;

    for (by = 0; by < bh; by++) {
        if ((ret = read_block_types(c, gb, &c->bundle[BINK_SRC_BLOCK_TYPES])) < 0)
            return ret;
        if ((ret = read_block_types(c, gb, &c->bundle[BINK_SRC_SUB_BLOCK_TYPES])) < 0)
            return ret;
        if ((ret = read_colors(c, gb, &c->bundle[BINK_SRC_COLORS])) < 0)
            return ret;
        if ((ret = read_patterns(c, gb, &c->bundle[BINK_SRC_PATTERN])) < 0)
            return ret;
        if ((ret = read_motion_values(c, gb, &c->bundle[BINK_SRC_X_OFF])) < 0)
            return ret;
        if ((ret = read_motion_values(c, gb, &c->bundle[BINK_SRC_Y_OFF])) < 0)
            return ret;
        if ((ret = read_dcs(gb, &c->bundle[BINK_SRC_INTRA_DC], DC_START_BITS, 0)) < 0)
            return ret;
        if ((ret = read_dcs(gb, &c->bundle[BINK_SRC_INTER_DC], DC_START_BITS, 1)) < 0)
            return ret;
        if ((ret = read_runs(c, gb, &c->bundle[BINK_SRC_RUN])) < 0)
            return ret;

        dst = frame + 8 * by * stride;
        intptr_t at = (intptr_t)8 * by * stride; // the block's offset in the reference plane
        for (bx = 0; bx < bw; bx++, dst += 8, at += 8) {
            blk = get_value(c, BINK_SRC_BLOCK_TYPES);
            // 16x16 block type on odd line means part of the already decoded block, so skip it
            if (((by & 1) || (bx & 1)) && blk == SCALED_BLOCK) {
                bx++;
                dst += 8;
                at += 8;
                continue;
            }
            switch (blk) {
            case SKIP_BLOCK:
                put_pixels8x8(dst, ref_start + at, stride);
                break;
            case SCALED_BLOCK: {
                const int w = MIN(16, (bw - bx) * 8), h = MIN(16, (bh - by) * 8);
                blk = get_value(c, BINK_SRC_SUB_BLOCK_TYPES);
                switch (blk) {
                case RUN_BLOCK:
                    if (get_bits_left(gb) < 4)
                        return -1;
                    scan = bink_patterns[get_bits(gb, 4)];
                    i = 0;
                    do {
                        int run = get_value(c, BINK_SRC_RUN) + 1;
                        i += run;
                        if (i > 64)
                            return -1;
                        if (get_bits1(gb)) {
                            v = get_value(c, BINK_SRC_COLORS);
                            for (j = 0; j < run; j++)
                                ublock[*scan++] = (uint8_t)v;
                        } else {
                            for (j = 0; j < run; j++)
                                ublock[*scan++] = (uint8_t)get_value(c, BINK_SRC_COLORS);
                        }
                    } while (i < 63);
                    if (i == 63)
                        ublock[*scan++] = (uint8_t)get_value(c, BINK_SRC_COLORS);
                    break;
                case INTRA_BLOCK:
                    memset(dctblock, 0, sizeof(*dctblock) * 64);
                    dctblock[0] = get_value(c, BINK_SRC_INTRA_DC);
                    if ((quant_idx = read_dct_coeffs(gb, dctblock, bink_scan, &coef_count, coef_idx, -1)) < 0)
                        return quant_idx;
                    unquantize_dct_coeffs(dctblock, bink_intra_quant[quant_idx], coef_count, coef_idx, bink_scan);
                    bink_idct_put_c(ublock, 8, dctblock);
                    break;
                case FILL_BLOCK:
                    v = get_value(c, BINK_SRC_COLORS);
                    fill_block(dst, v, stride, w, h);
                    break;
                case PATTERN_BLOCK:
                    for (i = 0; i < 2; i++)
                        col[i] = get_value(c, BINK_SRC_COLORS);
                    for (j = 0; j < 8; j++) {
                        v = get_value(c, BINK_SRC_PATTERN);
                        for (i = 0; i < 8; i++, v >>= 1)
                            ublock[i + j * 8] = (uint8_t)col[v & 1];
                    }
                    break;
                case RAW_BLOCK:
                    for (j = 0; j < 8; j++)
                        for (i = 0; i < 8; i++)
                            ublock[i + j * 8] = (uint8_t)get_value(c, BINK_SRC_COLORS);
                    break;
                default:
                    return -1;
                }
                if (blk != FILL_BLOCK)
                    scale_block_c(ublock, dst, stride, w, h);
                bx++;
                dst += 8;
                at += 8;
                break;
            }
            case MOTION_BLOCK:
                if (bink_put_pixels(c, dst, ref_start, at, stride, ref_end) < 0)
                    return -1;
                break;
            case RUN_BLOCK:
                scan = bink_patterns[get_bits(gb, 4)];
                i = 0;
                do {
                    int run = get_value(c, BINK_SRC_RUN) + 1;
                    i += run;
                    if (i > 64)
                        return -1;
                    if (get_bits1(gb)) {
                        v = get_value(c, BINK_SRC_COLORS);
                        for (j = 0; j < run; j++)
                            dst[coordmap[*scan++]] = (uint8_t)v;
                    } else {
                        for (j = 0; j < run; j++)
                            dst[coordmap[*scan++]] = (uint8_t)get_value(c, BINK_SRC_COLORS);
                    }
                } while (i < 63);
                if (i == 63)
                    dst[coordmap[*scan++]] = (uint8_t)get_value(c, BINK_SRC_COLORS);
                break;
            case RESIDUE_BLOCK:
                if (bink_put_pixels(c, dst, ref_start, at, stride, ref_end) < 0)
                    return -1;
                memset(block, 0, sizeof(block));
                v = (int)get_bits(gb, 7);
                if (read_residue(gb, block, v) < 0)
                    return -1;
                add_pixels8_c(dst, block, stride);
                break;
            case INTRA_BLOCK:
                memset(dctblock, 0, sizeof(*dctblock) * 64);
                dctblock[0] = get_value(c, BINK_SRC_INTRA_DC);
                if ((quant_idx = read_dct_coeffs(gb, dctblock, bink_scan, &coef_count, coef_idx, -1)) < 0)
                    return quant_idx;
                unquantize_dct_coeffs(dctblock, bink_intra_quant[quant_idx], coef_count, coef_idx, bink_scan);
                bink_idct_put_c(dst, stride, dctblock);
                break;
            case FILL_BLOCK:
                v = get_value(c, BINK_SRC_COLORS);
                fill_block(dst, v, stride, 8, 8);
                break;
            case INTER_BLOCK:
                if (bink_put_pixels(c, dst, ref_start, at, stride, ref_end) < 0)
                    return -1;
                memset(dctblock, 0, sizeof(*dctblock) * 64);
                dctblock[0] = get_value(c, BINK_SRC_INTER_DC);
                if ((quant_idx = read_dct_coeffs(gb, dctblock, bink_scan, &coef_count, coef_idx, -1)) < 0)
                    return quant_idx;
                unquantize_dct_coeffs(dctblock, bink_inter_quant[quant_idx], coef_count, coef_idx, bink_scan);
                bink_idct_add_c(dst, stride, dctblock);
                break;
            case PATTERN_BLOCK:
                for (i = 0; i < 2; i++)
                    col[i] = get_value(c, BINK_SRC_COLORS);
                for (i = 0; i < 8; i++) {
                    v = get_value(c, BINK_SRC_PATTERN);
                    for (j = 0; j < 8; j++, v >>= 1)
                        dst[i * stride + j] = (uint8_t)col[v & 1];
                }
                break;
            case RAW_BLOCK: {
                Bundle *colors = &c->bundle[BINK_SRC_COLORS];
                if (colors->cur_ptr + 64 > colors->data_end + BUNDLE_GUARD)
                    return -1;
                for (i = 0; i < 8; i++)
                    memcpy(dst + i * stride, colors->cur_ptr + i * 8, 8);
                colors->cur_ptr += 64;
                break;
            }
            default:
                return -1;
            }
        }
    }

end:
    if (get_bits_count(gb) & 0x1F) // next plane data starts at 32-bit boundary
        skip_bits(gb, 32 - (get_bits_count(gb) & 0x1F));
    return 0;
}

int PORT_MS_ABI bink_video_decode(struct bink_video *c, const void *packet, unsigned size, const bink_video_planes *out,
    const bink_video_planes *previous, int alpha)
{
    bitreader gb;
    const uint32_t bits_count = size * 8u;
    br_init(&gb, packet, size);
    if (c->has_alpha) {
        if (c->version >= 'i') {
            // the bytes from here to the colour planes (the alpha plane's packet and its padding)
            const uint32_t skip = get_bits(&gb, 32);
            if (!alpha || out->data[3] == 0) {
                skip_bits(&gb, skip * 8u - 32u);
                goto colour;
            }
        } else if (!alpha || out->data[3] == 0) {
            return -1; // the older revisions say nothing of where the alpha plane ends
        }
        if (bink_decode_plane(c, &gb, out->data[3], previous ? previous->data[3] : 0, (int)out->pitch[3], 0) < 0)
            return -1;
    }
colour:
    if (c->version >= 'i')
        skip_bits(&gb, 32);
    for (int plane = 0; plane < 3; plane++) {
        const int plane_idx = (!plane || !c->swap_planes) ? plane : (plane ^ 3);
        if (bink_decode_plane(c, &gb, out->data[plane_idx], previous ? previous->data[plane_idx] : 0,
                (int)out->pitch[plane_idx], !!plane) < 0)
            return -1;
        if (get_bits_count(&gb) >= bits_count)
            break;
    }
    return 0;
}

// ---- audio ---------------------------------------------------------------------------------------------------------

#define MAX_CHANNELS 2
#define FFT_MAX 4096

// ff_wma_critical_freqs
static const uint16_t wma_critical_freqs[25] = {
    100, 200, 300, 400, 510, 630, 770, 920, 1080, 1270, 1480, 1720, 2000, 2320, 2700, 3150, 3700, 4400, 5300, 6400,
    7700, 9500, 12000, 15500, 24500,
};

static const uint8_t rle_length_tab[16] = { 2, 3, 4, 5, 6, 8, 9, 10, 11, 12, 13, 14, 15, 16, 32, 64 };

struct bink_audio {
    bitreader gb;
    int active;     // a packet is being read
    int use_dct;
    int channels;   // channels a block codes (1 for RDFT: its samples are interleaved)
    int out_channels;
    int first;
    int frame_len;   // transform size (samples)
    int overlap_len; // overlap size (samples)
    int num_bands;
    unsigned bands[26];
    float root;
    float quant_table[96];
    float previous[MAX_CHANNELS][FFT_MAX / 16]; // coeffs from previous audio block
    // the transform: a complex FFT of fft_n points (DCT: the 2N-point FFT of the coefficients turned by
    // exp(i pi n / 2N); RDFT: the N-point inverse DFT of the Hermitian spectrum)
    int fft_n, fft_bits;
    float scale;
    float tw_re[FFT_MAX / 2], tw_im[FFT_MAX / 2]; // exp(+2 pi i k / fft_n)
    float pre_re[FFT_MAX / 2], pre_im[FFT_MAX / 2]; // DCT: exp(i pi n / 2N)
    float work_re[FFT_MAX], work_im[FFT_MAX];
    float coeffs[FFT_MAX + 2];
    float out[MAX_CHANNELS][FFT_MAX];
};

// ---- math without libm (only used to build tables) ----

static const double k_pi = 3.14159265358979323846;

static double math_sin(double x)
{
    while (x > k_pi)
        x -= 2.0 * k_pi;
    while (x < -k_pi)
        x += 2.0 * k_pi;
    double term = x, sum = x, x2 = x * x;
    for (int n = 1; n < 30; n++) {
        term *= -x2 / ((2.0 * n) * (2.0 * n + 1.0));
        sum += term;
    }
    return sum;
}

static double math_cos(double x) { return math_sin(x + k_pi / 2.0); }

static double math_sqrt(double x)
{
    if (x <= 0.0)
        return 0.0;
    double r = x > 1.0 ? x : 1.0;
    for (int i = 0; i < 200; i++) {
        const double next = 0.5 * (r + x / r);
        if (next == r)
            break;
        r = next;
    }
    return r;
}

static double math_exp(double x)
{
    // exp(x) = exp(x / 2^k)^(2^k), the reduced argument's Taylor series to full precision
    int k = 0;
    while (x > 0.5 || x < -0.5) {
        x *= 0.5;
        k++;
    }
    double term = 1.0, sum = 1.0;
    for (int n = 1; n < 30; n++) {
        term *= x / n;
        sum += term;
    }
    while (k-- > 0)
        sum *= sum;
    return sum;
}

// m * 2^e for e in the normal range (ldexpf of get_float)
static float scale_pow2(uint32_t m, int e)
{
    const uint32_t bits = (uint32_t)(127 + e) << 23;
    float p;
    memcpy(&p, &bits, 4);
    return (float)m * p;
}

unsigned PORT_MS_ABI bink_audio_memory(void) { return (unsigned)sizeof(struct bink_audio) + 16u; }

struct bink_audio * PORT_MS_ABI bink_audio_create(void *memory, unsigned sample_rate, unsigned flags, unsigned revision)
{
    struct bink_audio *s = (struct bink_audio *)(((uintptr_t)memory + 15u) & ~(uintptr_t)15u);
    memset(s, 0, sizeof(*s));
    (void)revision; // 'b' (version_b) is not played
    const int channels = (flags & BINK_CODEC_AUDIO_STEREO) ? 2 : 1;
    int frame_len_bits;
    if (sample_rate < 22050)
        frame_len_bits = 9;
    else if (sample_rate < 44100)
        frame_len_bits = 10;
    else
        frame_len_bits = 11;
    s->use_dct = (flags & BINK_CODEC_AUDIO_DCT) != 0;
    s->out_channels = channels;
    if (!s->use_dct) {
        // audio is already interleaved for the RDFT format variant
        sample_rate *= (unsigned)channels;
        s->channels = 1;
        frame_len_bits += av_log2((uint32_t)channels);
    } else {
        s->channels = channels;
    }
    s->frame_len = 1 << frame_len_bits;
    s->overlap_len = s->frame_len / 16;
    const unsigned sample_rate_half = (sample_rate + 1u) / 2u;
    if (!s->use_dct)
        s->root = (float)(2.0 / (math_sqrt((double)s->frame_len) * 32768.0));
    else
        s->root = (float)((double)s->frame_len / (math_sqrt((double)s->frame_len) * 32768.0));
    for (int i = 0; i < 96; i++) {
        // constant is result of 0.066399999/log10(M_E)
        s->quant_table[i] = (float)math_exp((double)((float)i * 0.15289164787221953823f)) * s->root;
    }
    // calculate number of bands
    for (s->num_bands = 1; s->num_bands < 25; s->num_bands++)
        if (sample_rate_half <= wma_critical_freqs[s->num_bands - 1])
            break;
    // populate bands data
    s->bands[0] = 2;
    for (int i = 1; i < s->num_bands; i++)
        s->bands[i] = (wma_critical_freqs[i - 1] * (unsigned)s->frame_len / sample_rate_half) & ~1u;
    s->bands[s->num_bands] = (unsigned)s->frame_len;
    s->first = 1;

    // the transform tables
    if (s->use_dct) {
        // FFmpeg: av_tx DCT-III (inverse DCT) of frame_len points, scale 1 / frame_len on its definition, which is
        // twice the plain DCT-III below (c[0] / 2 + sum): test_bink fits the gain to exactly 2.0
        s->fft_n = 2 * s->frame_len;
        s->scale = 2.0f / (float)s->frame_len;
        const int n = s->frame_len;
        for (int i = 0; i < n; i++) {
            const double a = k_pi * i / (2.0 * n);
            s->pre_re[i] = (float)math_cos(a);
            s->pre_im[i] = (float)math_sin(a);
        }
    } else {
        // FFmpeg: av_tx RDFT, inverse (complex to real), frame_len points with scale 0.5
        s->fft_n = s->frame_len;
        s->scale = 0.5f;
    }
    s->fft_bits = av_log2((uint32_t)s->fft_n);
    for (int k = 0; k < s->fft_n / 2; k++) {
        const double a = 2.0 * k_pi * k / s->fft_n;
        s->tw_re[k] = (float)math_cos(a);
        s->tw_im[k] = (float)math_sin(a);
    }
    return s;
}

unsigned PORT_MS_ABI bink_audio_channels(const struct bink_audio *s) { return (unsigned)s->out_channels; }

unsigned PORT_MS_ABI bink_audio_block_frames(const struct bink_audio *s)
{
    return (unsigned)((s->frame_len - s->overlap_len) / (s->use_dct ? 1 : s->out_channels));
}

void PORT_MS_ABI bink_audio_reset(struct bink_audio *s)
{
    s->first = 1;
    s->active = 0;
}

// In place: x[k] = sum_n x[n] exp(+2 pi i n k / fft_n), unnormalized.
static void fft_inverse(struct bink_audio *s)
{
    const int n = s->fft_n, bits = s->fft_bits;
    float *re = s->work_re, *im = s->work_im;
    for (int i = 0; i < n; i++) {
        unsigned r = 0;
        for (int b = 0; b < bits; b++)
            r |= (((unsigned)i >> b) & 1u) << (bits - 1 - b);
        if ((int)r > i) {
            float t = re[i];
            re[i] = re[r];
            re[r] = t;
            t = im[i];
            im[i] = im[r];
            im[r] = t;
        }
    }
    for (int len = 2; len <= n; len <<= 1) {
        const int half = len >> 1, step = n / len;
        for (int i = 0; i < n; i += len) {
            for (int j = 0; j < half; j++) {
                const float wr = s->tw_re[j * step], wi = s->tw_im[j * step];
                const int a = i + j, b = a + half;
                const float br = re[b] * wr - im[b] * wi, bi = re[b] * wi + im[b] * wr;
                re[b] = re[a] - br;
                im[b] = im[a] - bi;
                re[a] += br;
                im[a] += bi;
            }
        }
    }
}

// The DCT-III of frame_len coefficients: out[k] = scale * (c[0] / 2 + sum_n>0 c[n] cos(pi n (2k + 1) / 2N)).
static void transform_dct(struct bink_audio *s, const float *c, float *out)
{
    const int n = s->frame_len;
    for (int i = 0; i < n; i++) {
        s->work_re[i] = c[i] * s->pre_re[i];
        s->work_im[i] = c[i] * s->pre_im[i];
    }
    s->work_re[0] *= 0.5f;
    for (int i = n; i < s->fft_n; i++)
        s->work_re[i] = s->work_im[i] = 0.0f;
    fft_inverse(s);
    for (int k = 0; k < n; k++)
        out[k] = s->work_re[k] * s->scale;
}

// The inverse real DFT of the packed spectrum c (c[0] DC, c[N] / c[N + 1] Nyquist, (c[2k], c[2k + 1]) bin k):
// out[n] = scale * sum over the whole Hermitian spectrum.
static void transform_rdft(struct bink_audio *s, const float *c, float *out)
{
    const int n = s->frame_len;
    s->work_re[0] = c[0];
    s->work_im[0] = 0.0f;
    s->work_re[n / 2] = c[n];
    s->work_im[n / 2] = 0.0f;
    for (int k = 1; k < n / 2; k++) {
        s->work_re[k] = c[2 * k];
        s->work_im[k] = c[2 * k + 1];
        s->work_re[n - k] = c[2 * k];
        s->work_im[n - k] = -c[2 * k + 1];
    }
    fft_inverse(s);
    for (int k = 0; k < n; k++)
        out[k] = s->work_re[k] * s->scale;
}

static float get_float(bitreader *gb)
{
    int power = (int)get_bits(gb, 5);
    float f = scale_pow2(get_bits(gb, 23), power - 23);
    if (get_bits1(gb))
        f = -f;
    return f;
}

// Decode Bink Audio block (FFmpeg's decode_block); the overlap is applied to s->out.
static int decode_block(struct bink_audio *s, int use_dct, int channels)
{
    int ch, i, j, k;
    float q, quant[25];
    int width, coeff;
    bitreader *gb = &s->gb;
    float *coeffs = s->coeffs;

    if (use_dct)
        skip_bits(gb, 2);

    for (ch = 0; ch < channels; ch++) {
        if (get_bits_left(gb) < 58)
            return -1;
        coeffs[0] = get_float(gb) * s->root;
        coeffs[1] = get_float(gb) * s->root;

        if (get_bits_left(gb) < s->num_bands * 8)
            return -1;
        for (i = 0; i < s->num_bands; i++) {
            int value = (int)get_bits(gb, 8);
            quant[i] = s->quant_table[MIN(value, 95)];
        }

        k = 0;
        q = quant[0];

        // parse coefficients
        i = 2;
        while (i < s->frame_len) {
            int v = (int)get_bits1(gb);
            if (v) {
                v = (int)get_bits(gb, 4);
                j = i + rle_length_tab[v] * 8;
            } else {
                j = i + 8;
            }
            j = MIN(j, s->frame_len);

            width = (int)get_bits(gb, 4);
            if (width == 0) {
                memset(coeffs + i, 0, (size_t)(j - i) * sizeof(*coeffs));
                i = j;
                while (s->bands[k] < (unsigned)i)
                    q = quant[k++];
            } else {
                while (i < j) {
                    if (s->bands[k] == (unsigned)i)
                        q = quant[k++];
                    coeff = (int)get_bits(gb, width);
                    if (coeff) {
                        int sign = (int)get_bits1(gb);
                        if (sign)
                            coeffs[i] = -q * (float)coeff;
                        else
                            coeffs[i] = q * (float)coeff;
                    } else {
                        coeffs[i] = 0.0f;
                    }
                    i++;
                }
            }
            if (get_bits_left(gb) < 0)
                return -1;
        }

        if (use_dct) {
            coeffs[0] /= 0.5f;
            transform_dct(s, coeffs, s->out[ch]);
        } else {
            for (i = 2; i < s->frame_len; i += 2)
                coeffs[i + 1] *= -1;
            coeffs[s->frame_len + 0] = coeffs[1];
            coeffs[s->frame_len + 1] = coeffs[1] = 0;
            transform_rdft(s, coeffs, s->out[ch]);
        }
    }

    for (ch = 0; ch < channels; ch++) {
        int count = s->overlap_len * channels;
        if (!s->first) {
            j = ch;
            for (i = 0; i < s->overlap_len; i++, j += channels)
                s->out[ch][i] = (s->previous[ch][i] * (float)(count - j) + s->out[ch][i] * (float)j) / (float)count;
        }
        memcpy(s->previous[ch], &s->out[ch][s->frame_len - s->overlap_len],
            (size_t)s->overlap_len * sizeof(*s->previous[ch]));
    }
    s->first = 0;
    return 0;
}

int PORT_MS_ABI bink_audio_begin(struct bink_audio *s, const void *packet, unsigned size)
{
    s->active = 0;
    if (size < 4)
        return -1;
    br_init(&s->gb, packet, size);
    const int reported = (int)get_bits(&s->gb, 32); // skip reported size
    s->active = get_bits_left(&s->gb) > 0;
    return reported < 0 ? 0 : reported;
}

int PORT_MS_ABI bink_audio_next_block(struct bink_audio *s, float *out)
{
    if (!s->active)
        return 0;
    if (decode_block(s, s->use_dct, s->channels) < 0) {
        s->active = 0;
        return -1;
    }
    // get_bits_align32
    const uint32_t n = (0u - get_bits_count(&s->gb)) & 31u;
    if (n)
        skip_bits(&s->gb, n);
    if (get_bits_left(&s->gb) <= 0)
        s->active = 0;
    const int samples = s->frame_len - s->overlap_len;
    if (s->use_dct) {
        for (int i = 0; i < samples; i++)
            for (int ch = 0; ch < s->channels; ch++)
                out[i * s->channels + ch] = s->out[ch][i];
        return samples;
    }
    for (int i = 0; i < samples; i++)
        out[i] = s->out[0][i];
    return samples / s->out_channels;
}
