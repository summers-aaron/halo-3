// port/sdk/runtime: self-contained RFC 1950 / RFC 1951 streaming z_inflate* and z_deflate* (Z_PREFIX spelling)
// matching include/zlib/zlib.h and honoring stream->zalloc / stream->zfree (blam_zlib_malloc / blam_zlib_free).
#include <stddef.h>
#include "zlib_heap.h"
#include "zlib/zlib.h"

namespace {

static void *z_alloc_mem(z_streamp strm, uInt items, uInt size)
{
    if (strm->zalloc)
        return strm->zalloc(strm->opaque, items, size);
    return RtlAllocateHeap(GetProcessHeap(), 0, (size_t)items * (size_t)size);
}

static void z_free_mem(z_streamp strm, void *ptr)
{
    if (!ptr)
        return;
    if (strm->zfree)
        strm->zfree(strm->opaque, ptr);
    else
        RtlFreeHeap(GetProcessHeap(), 0, ptr);
}

static uLong adler32_step(uLong adler, const Bytef *buf, uInt len)
{
    return z_adler32(adler, buf, len);
}

static const unsigned short k_lens[29] = {
    3,4,5,6,7,8,9,10,11,13,15,17,19,23,27,31,35,43,51,59,67,83,99,115,131,163,195,227,258
};
static const unsigned short k_lext[29] = {
    0,0,0,0,0,0,0,0,1,1,1,1,2,2,2,2,3,3,3,3,4,4,4,4,5,5,5,5,0
};
static const unsigned short k_dists[30] = {
    1,2,3,4,5,7,9,13,17,25,33,49,65,97,129,193,257,385,513,769,1025,1537,2049,3073,4097,6145,8193,12289,16385,24577
};
static const unsigned short k_dext[30] = {
    0,0,0,0,1,1,2,2,3,3,4,4,5,5,6,6,7,7,8,8,9,9,10,10,11,11,12,12,13,13
};

static constexpr unsigned k_window_size = 32768;

struct huff_table {
    unsigned short counts[16];
    unsigned short symbols[320];
};

static int build_huff(huff_table *h, const unsigned char *lengths, int num)
{
    for (int i = 0; i < 16; i++)
        h->counts[i] = 0;
    for (int i = 0; i < num; i++) {
        if (lengths[i] > 15)
            return -1;
        h->counts[lengths[i]]++;
    }
    if (h->counts[0] == num)
        return 0;

    int left = 1;
    for (int len = 1; len <= 15; len++) {
        left <<= 1;
        left -= h->counts[len];
        if (left < 0)
            return -1;
    }

    unsigned short offs[16];
    offs[1] = 0;
    for (int len = 1; len < 15; len++)
        offs[len + 1] = (unsigned short)(offs[len] + h->counts[len]);

    for (int sym = 0; sym < num; sym++) {
        if (lengths[sym] != 0)
            h->symbols[offs[lengths[sym]]++] = (unsigned short)sym;
    }
    return left;
}

// Inflate commits one header or symbol at a time. The largest transaction is a
// dynamic Huffman header, which fits in the bounded input staging area.
struct inflate_ctx {
    enum phase_type { header, block, stored, codes, trailer, done, bad } phase;
    bool wrap, final;
    unsigned window_limit, history, write_pos;
    unsigned run, distance, literal;
    unsigned bitbuf, bitcnt, staged_size;
    unsigned char staged[1024];
    unsigned char window[k_window_size];
    huff_table lengths, distances;
};

struct bit_cursor {
    z_streamp stream;
    inflate_ctx *ctx;
    unsigned pos, bitbuf, bitcnt;
};

static bool bc_bits(bit_cursor *bc, unsigned need, unsigned *out)
{
    while (bc->bitcnt < need) {
        const unsigned pos = bc->pos;
        unsigned byte;
        if (pos < bc->ctx->staged_size) {
            byte = bc->ctx->staged[pos];
        } else {
            const unsigned offset = pos - bc->ctx->staged_size;
            if (offset == bc->stream->avail_in)
                return false;
            byte = bc->stream->next_in[offset];
        }
        ++bc->pos;
        bc->bitbuf |= byte << bc->bitcnt;
        bc->bitcnt += 8;
    }
    *out = bc->bitbuf & ((1u << need) - 1u);
    bc->bitbuf >>= need;
    bc->bitcnt -= need;
    return true;
}

static void consume_input(z_streamp stream, unsigned size)
{
    if (size) {
        stream->next_in += size;
        stream->avail_in -= size;
        stream->total_in += size;
    }
}

static void commit_bits(const bit_cursor &bc)
{
    inflate_ctx *c = bc.ctx;
    if (bc.pos < c->staged_size) {
        c->staged_size -= bc.pos;
        __builtin_memmove(c->staged, c->staged + bc.pos, c->staged_size);
    } else {
        consume_input(bc.stream, bc.pos - c->staged_size);
        c->staged_size = 0;
    }
    c->bitbuf = bc.bitbuf;
    c->bitcnt = bc.bitcnt;
}

static int stage_bits(const bit_cursor &bc)
{
    inflate_ctx *c = bc.ctx;
    const unsigned bytes = bc.stream->avail_in;
    if (bytes > sizeof(c->staged) - c->staged_size)
        return Z_DATA_ERROR;
    if (bytes) {
        __builtin_memcpy(c->staged + c->staged_size, bc.stream->next_in, bytes);
        c->staged_size += bytes;
        consume_input(bc.stream, bytes);
    }
    return Z_OK;
}

static int bc_decode(bit_cursor *bc, const huff_table *h, int *sym_out)
{
    int code = 0, first = 0, index = 0;
    for (int len = 1; len <= 15; len++) {
        unsigned int bit = 0;
        if (!bc_bits(bc, 1, &bit))
            return 0;
        code |= (int)bit;
        int count = h->counts[len];
        if (code - count < first) {
            *sym_out = h->symbols[index + (code - first)];
            return 1;
        }
        index += count;
        first += count;
        first <<= 1;
        code <<= 1;
    }
    return -1;
}

static void fixed_tables(inflate_ctx *c)
{
    unsigned char lengths[288];
    for (int i = 0; i < 144; ++i) lengths[i] = 8;
    for (int i = 144; i < 256; ++i) lengths[i] = 9;
    for (int i = 256; i < 280; ++i) lengths[i] = 7;
    for (int i = 280; i < 288; ++i) lengths[i] = 8;
    build_huff(&c->lengths, lengths, 288);
    for (int i = 0; i < 32; ++i) lengths[i] = 5;
    build_huff(&c->distances, lengths, 32);
}

// Positive means a complete transaction, zero means more input, negative means bad data.
static int dynamic_tables(bit_cursor &bc, inflate_ctx *c)
{
    static const unsigned char k_order[19] = {
        16,17,18,0,8,7,9,6,10,5,11,4,12,3,13,2,14,1,15
    };
    unsigned nlen, ndist, ncode;
    if (!bc_bits(&bc, 5, &nlen) || !bc_bits(&bc, 5, &ndist) || !bc_bits(&bc, 4, &ncode))
        return 0;
    nlen += 257;
    ndist += 1;
    ncode += 4;
    if (nlen > 286 || ndist > 30)
        return -1;

    unsigned char lengths[320] = {};
    for (unsigned i = 0; i < ncode; ++i) {
        unsigned length;
        if (!bc_bits(&bc, 3, &length))
            return 0;
        lengths[k_order[i]] = (unsigned char)length;
    }
    huff_table code_lengths;
    if (build_huff(&code_lengths, lengths, 19) != 0)
        return -1;

    unsigned i = 0;
    while (i < nlen + ndist) {
        int symbol;
        const int result = bc_decode(&bc, &code_lengths, &symbol);
        if (result <= 0)
            return result;
        if (symbol < 16) {
            lengths[i++] = (unsigned char)symbol;
            continue;
        }
        if (symbol == 16 && i == 0)
            return -1;
        static const unsigned extra[] = { 2, 3, 7 };
        static const unsigned base[] = { 3, 3, 11 };
        unsigned repeat;
        if (!bc_bits(&bc, extra[symbol - 16], &repeat))
            return 0;
        repeat += base[symbol - 16];
        if (repeat > nlen + ndist - i)
            return -1;
        const unsigned char length = symbol == 16 ? lengths[i - 1] : 0;
        while (repeat--)
            lengths[i++] = length;
    }
    if (!lengths[256] || build_huff(&c->lengths, lengths, (int)nlen) < 0
        || build_huff(&c->distances, lengths + nlen, (int)ndist) < 0)
        return -1;
    return 1;
}

static void end_block(inflate_ctx *c)
{
    c->phase = inflate_ctx::block;
    if (c->final) {
        c->bitbuf = c->bitcnt = 0;
        c->phase = c->wrap ? inflate_ctx::trailer : inflate_ctx::done;
    }
}

static int inflate_block_header(bit_cursor &bc, inflate_ctx *c)
{
    unsigned final, type;
    if (!bc_bits(&bc, 1, &final) || !bc_bits(&bc, 2, &type))
        return 0;
    switch (type) {
    case 0: {
        bc.bitbuf = bc.bitcnt = 0;
        unsigned length, complement;
        if (!bc_bits(&bc, 16, &length) || !bc_bits(&bc, 16, &complement))
            return 0;
        if ((length ^ 0xffffu) != complement)
            return -1;
        c->run = length;
        break;
    }
    case 1:
        fixed_tables(c);
        break;
    case 2: {
        const int result = dynamic_tables(bc, c);
        if (result <= 0)
            return result;
        break;
    }
    default:
        return -1;
    }
    c->final = final != 0;
    commit_bits(bc);
    c->phase = type == 0 ? inflate_ctx::stored : inflate_ctx::codes;
    return 1;
}

static int inflate_symbol(bit_cursor &bc, inflate_ctx *c)
{
    int symbol;
    int result = bc_decode(&bc, &c->lengths, &symbol);
    if (result <= 0)
        return result;
    if (symbol < 256) {
        c->literal = (unsigned)symbol;
        c->distance = 0;
        c->run = 1;
    } else if (symbol > 256) {
        const int length_code = symbol - 257;
        if (length_code >= 29)
            return -1;
        unsigned length_extra, dist_extra;
        if (!bc_bits(&bc, k_lext[length_code], &length_extra))
            return 0;
        int dist_code;
        result = bc_decode(&bc, &c->distances, &dist_code);
        if (result <= 0)
            return result;
        if (dist_code >= 30)
            return -1;
        if (!bc_bits(&bc, k_dext[dist_code], &dist_extra))
            return 0;
        const unsigned distance = k_dists[dist_code] + dist_extra;
        if (distance > c->history || distance > c->window_limit)
            return -1;
        c->distance = distance;
        c->run = k_lens[length_code] + length_extra;
    }
    commit_bits(bc);
    if (symbol == 256)
        end_block(c);
    return 1;
}

static void inflate_byte(z_streamp strm, inflate_ctx *c, unsigned char byte)
{
    *strm->next_out++ = byte;
    --strm->avail_out;
    ++strm->total_out;
    c->window[c->write_pos++ & (k_window_size - 1)] = byte;
    if (c->history < k_window_size)
        ++c->history;
}

static int inflate_data(z_streamp strm, inflate_ctx *c, Bytef *output, unsigned output_size)
{
    for (;;) {
        bit_cursor bc = { strm, c, 0, c->bitbuf, c->bitcnt };
        switch (c->phase) {
        case inflate_ctx::header: {
            unsigned cmf, flg;
            if (!bc_bits(&bc, 8, &cmf) || !bc_bits(&bc, 8, &flg))
                return stage_bits(bc);
            const unsigned window_bits = (cmf >> 4) + 8;
            if ((cmf & 0xf) != Z_DEFLATED || window_bits > MAX_WBITS
                || (1u << window_bits) > c->window_limit || ((cmf << 8) + flg) % 31 != 0)
                return Z_DATA_ERROR;
            if (flg & 0x20)
                return Z_DATA_ERROR; // Preset dictionaries are unsupported.
            c->window_limit = 1u << window_bits;
            commit_bits(bc);
            c->phase = inflate_ctx::block;
            break;
        }
        case inflate_ctx::block: {
            const int result = inflate_block_header(bc, c);
            if (result <= 0)
                return result < 0 ? Z_DATA_ERROR : stage_bits(bc);
            break;
        }
        case inflate_ctx::stored:
            if (!c->run) {
                end_block(c);
                break;
            }
            if (!strm->avail_out)
                return Z_OK;
            while (c->run && strm->avail_out && strm->avail_in) {
                inflate_byte(strm, c, *strm->next_in);
                consume_input(strm, 1);
                --c->run;
            }
            if (c->run)
                return Z_OK;
            break;
        case inflate_ctx::codes:
            if (!c->run) {
                const int result = inflate_symbol(bc, c);
                if (result <= 0)
                    return result < 0 ? Z_DATA_ERROR : stage_bits(bc);
            }
            while (c->run && strm->avail_out) {
                const unsigned char byte = c->distance ? c->window[(c->write_pos - c->distance) & (k_window_size - 1)]
                                                       : (unsigned char)c->literal;
                inflate_byte(strm, c, byte);
                --c->run;
            }
            if (c->run)
                return Z_OK;
            break;
        case inflate_ctx::trailer: {
            unsigned high, low;
            if (!bc_bits(&bc, 16, &high) || !bc_bits(&bc, 16, &low))
                return stage_bits(bc);
            commit_bits(bc);
            const unsigned expected = ((high & 0xff) << 24) | ((high >> 8) << 16) | ((low & 0xff) << 8) | (low >> 8);
            const unsigned written = output_size - strm->avail_out;
            const uLong adler = written ? adler32_step(strm->adler, output, written) : strm->adler;
            if (expected != adler)
                return Z_DATA_ERROR;
            c->phase = inflate_ctx::done;
            break;
        }
        case inflate_ctx::done:
            return Z_STREAM_END;
        case inflate_ctx::bad:
            return Z_DATA_ERROR;
        }
    }
}

// Deflate stages a bounded block so it can choose fixed Huffman coding or
// stored bytes, and drains that block before accepting another one.
static constexpr unsigned k_block_size = 16384;
static constexpr unsigned k_hash_size = 8192;

struct deflate_ctx {
    bool wrap, header_sent, finished, sync_emitted;
    unsigned level, window_bits;
    unsigned input_size, output_pos, output_size;
    unsigned bitbuf, bitcnt;
    unsigned char input[k_block_size];
    unsigned char output[k_block_size * 9 / 8 + 16];
    unsigned short head[k_hash_size], previous[k_block_size];
};

static unsigned smaller(unsigned a, unsigned b)
{
    return a < b ? a : b;
}

static void write_bits(deflate_ctx *c, unsigned value, unsigned count)
{
    c->bitbuf |= value << c->bitcnt;
    c->bitcnt += count;
    while (c->bitcnt >= 8) {
        c->output[c->output_size++] = (unsigned char)c->bitbuf;
        c->bitbuf >>= 8;
        c->bitcnt -= 8;
    }
}

static void byte_align(deflate_ctx *c)
{
    if (c->bitcnt)
        write_bits(c, 0, 8 - c->bitcnt);
}

static unsigned reverse_bits(unsigned value, unsigned count)
{
    unsigned result = 0;
    while (count--) {
        result = (result << 1) | (value & 1);
        value >>= 1;
    }
    return result;
}

static void write_fixed_symbol(deflate_ctx *c, unsigned symbol)
{
    if (symbol < 144)
        write_bits(c, reverse_bits(0x30 + symbol, 8), 8);
    else if (symbol < 256)
        write_bits(c, reverse_bits(0x190 + symbol - 144, 9), 9);
    else if (symbol < 280)
        write_bits(c, reverse_bits(symbol - 256, 7), 7);
    else
        write_bits(c, reverse_bits(0xc0 + symbol - 280, 8), 8);
}

static void write_match(deflate_ctx *c, unsigned length, unsigned distance)
{
    unsigned len_code = 0, dist_code = 0;
    while (len_code < 28 && length >= k_lens[len_code + 1])
        ++len_code;
    while (dist_code < 29 && distance >= k_dists[dist_code + 1])
        ++dist_code;
    write_fixed_symbol(c, len_code + 257);
    write_bits(c, length - k_lens[len_code], k_lext[len_code]);
    write_bits(c, reverse_bits(dist_code, 5), 5);
    write_bits(c, distance - k_dists[dist_code], k_dext[dist_code]);
}

static unsigned index_match(deflate_ctx *c, unsigned pos)
{
    const unsigned char *bytes = c->input + pos;
    const unsigned hash = ((bytes[0] * 251u + bytes[1]) * 251u + bytes[2]) & (k_hash_size - 1);
    const unsigned previous = c->head[hash];
    c->previous[pos] = (unsigned short)previous;
    c->head[hash] = (unsigned short)(pos + 1);
    return previous;
}

static void write_fixed_block(deflate_ctx *c, bool final)
{
    __builtin_memset(c->head, 0, sizeof(c->head));
    write_bits(c, 2u | final, 3);
    const unsigned window = 1u << c->window_bits;
    unsigned pos = 0;
    while (pos + 3 <= c->input_size) {
        unsigned candidate = index_match(c, pos);
        const unsigned limit = smaller(c->input_size - pos, 258);
        unsigned length = 2, distance = 0;
        unsigned remaining = 4u << (c->level / 2);
        while (candidate && remaining--) {
            const unsigned from = candidate - 1;
            if (pos - from > window)
                break;
            unsigned match = 0;
            while (match < limit && c->input[from + match] == c->input[pos + match])
                ++match;
            if (match > length) {
                length = match;
                distance = pos - from;
                if (length == limit)
                    break;
            }
            candidate = c->previous[from];
        }
        if (distance) {
            write_match(c, length, distance);
            const unsigned end = pos + length;
            while (++pos < end && pos + 3 <= c->input_size)
                index_match(c, pos);
            pos = end;
        } else {
            write_fixed_symbol(c, c->input[pos++]);
        }
    }
    while (pos < c->input_size)
        write_fixed_symbol(c, c->input[pos++]);
    write_fixed_symbol(c, 256);
}

static void write_stored_block(deflate_ctx *c, bool final, unsigned length)
{
    write_bits(c, final, 3);
    byte_align(c);
    write_bits(c, length, 16);
    write_bits(c, length ^ 0xffffu, 16);
    __builtin_memcpy(c->output + c->output_size, c->input, length);
    c->output_size += length;
}

static void write_block(deflate_ctx *c, bool final)
{
    if (c->level) {
        const unsigned output_size = c->output_size;
        const unsigned bitbuf = c->bitbuf, bitcnt = c->bitcnt;
        write_fixed_block(c, final);
        const unsigned fixed_bits = (c->output_size - output_size) * 8 + c->bitcnt;
        const unsigned stored_bits = ((bitcnt + 3 + 7) & ~7u) + (4 + c->input_size) * 8;
        if (fixed_bits <= stored_bits) {
            c->input_size = 0;
            return;
        }
        c->output_size = output_size;
        c->bitbuf = bitbuf;
        c->bitcnt = bitcnt;
    }
    write_stored_block(c, final, c->input_size);
    c->input_size = 0;
}

static bool drain_deflate(z_streamp strm, deflate_ctx *c)
{
    const unsigned bytes = smaller(strm->avail_out, c->output_size - c->output_pos);
    if (bytes) {
        __builtin_memcpy(strm->next_out, c->output + c->output_pos, bytes);
        c->output_pos += bytes;
        strm->next_out += bytes;
        strm->avail_out -= bytes;
        strm->total_out += bytes;
    }
    if (c->output_pos != c->output_size)
        return false;
    c->output_pos = c->output_size = 0;
    return true;
}

static bool valid_buffers(z_streamp strm)
{
    return strm && strm->state && (!strm->avail_in || strm->next_in) && (!strm->avail_out || strm->next_out);
}

} // namespace

extern "C" {

int z_inflateInit2_(z_streamp strm, int window_bits, const char *version, int stream_size)
{
    (void)version;
    if (!strm || stream_size != (int)sizeof(z_stream))
        return Z_VERSION_ERROR;
    const bool wrap = window_bits >= 0;
    if (window_bits < -MAX_WBITS || window_bits > MAX_WBITS)
        return Z_STREAM_ERROR;
    const int bits = window_bits ? (wrap ? window_bits : -window_bits) : MAX_WBITS;
    if (bits < 8)
        return Z_STREAM_ERROR;
    inflate_ctx *c = (inflate_ctx *)z_alloc_mem(strm, 1, sizeof(inflate_ctx));
    if (!c)
        return Z_MEM_ERROR;
    __builtin_memset(c, 0, sizeof(*c));
    c->wrap = wrap;
    c->window_limit = 1u << bits;
    c->phase = wrap ? inflate_ctx::header : inflate_ctx::block;
    strm->state = (internal_state *)c;
    strm->total_in = strm->total_out = 0;
    strm->adler = 1;
    strm->msg = 0;
    return Z_OK;
}

int z_inflate(z_streamp strm, int flush)
{
    if (!valid_buffers(strm) || flush < Z_NO_FLUSH || flush > Z_FINISH)
        return Z_STREAM_ERROR;
    inflate_ctx *c = (inflate_ctx *)strm->state;
    Bytef *const output = strm->next_out;
    const unsigned available = strm->avail_out;
    const unsigned input_available = strm->avail_in;
    const int result = inflate_data(strm, c, output, available);
    const unsigned written = available - strm->avail_out;
    if (c->wrap && written)
        strm->adler = adler32_step(strm->adler, output, written);
    if (result == Z_DATA_ERROR)
        c->phase = inflate_ctx::bad;
    if (result == Z_OK && flush == Z_FINISH && !written && input_available == strm->avail_in)
        return Z_BUF_ERROR;
    return result;
}

int z_inflateEnd(z_streamp strm)
{
    if (!strm || !strm->state)
        return Z_STREAM_ERROR;
    z_free_mem(strm, strm->state);
    strm->state = 0;
    return Z_OK;
}

int z_deflateInit2_(z_streamp strm, int level, int method, int window_bits, int mem_level, int strategy,
    const char *version, int stream_size)
{
    (void)mem_level; (void)strategy; (void)version;
    if (!strm || stream_size != (int)sizeof(z_stream))
        return Z_VERSION_ERROR;
    if (method != Z_DEFLATED || level < Z_DEFAULT_COMPRESSION || level > Z_BEST_COMPRESSION
        || window_bits < -MAX_WBITS || window_bits > MAX_WBITS || (window_bits > -8 && window_bits < 8))
        return Z_STREAM_ERROR;
    deflate_ctx *c = (deflate_ctx *)z_alloc_mem(strm, 1, sizeof(deflate_ctx));
    if (!c)
        return Z_MEM_ERROR;
    __builtin_memset(c, 0, sizeof(*c));
    c->wrap = window_bits > 0;
    c->window_bits = c->wrap ? window_bits : -window_bits;
    c->level = level == Z_DEFAULT_COMPRESSION ? 6 : level;
    strm->state = (internal_state *)c;
    strm->total_in = strm->total_out = 0;
    strm->adler = 1;
    strm->msg = 0;
    return Z_OK;
}

int z_deflate(z_streamp strm, int flush)
{
    if (!valid_buffers(strm) || flush < Z_NO_FLUSH || flush > Z_FINISH)
        return Z_STREAM_ERROR;
    deflate_ctx *c = (deflate_ctx *)strm->state;
    if (c->finished && (flush != Z_FINISH || strm->avail_in))
        return Z_STREAM_ERROR;

    if (c->wrap && !c->header_sent) {
        const unsigned header = (((c->window_bits - 8) << 4) | Z_DEFLATED) << 8;
        const unsigned flags = (31 - header % 31) % 31;
        c->output[c->output_size++] = (unsigned char)(header >> 8);
        c->output[c->output_size++] = (unsigned char)flags;
        c->header_sent = true;
    }

    while (drain_deflate(strm, c)) {
        if (c->finished)
            return Z_STREAM_END;
        const unsigned bytes = smaller(strm->avail_in, k_block_size - c->input_size);
        if (bytes) {
            __builtin_memcpy(c->input + c->input_size, strm->next_in, bytes);
            if (c->wrap)
                strm->adler = adler32_step(strm->adler, strm->next_in, bytes);
            consume_input(strm, bytes);
            c->input_size += bytes;
            c->sync_emitted = false;
        }
        if (flush == Z_FINISH && !strm->avail_in) {
            write_block(c, true);
            byte_align(c);
            if (c->wrap) {
                for (int shift = 24; shift >= 0; shift -= 8)
                    write_bits(c, (unsigned)(strm->adler >> shift) & 0xff, 8);
            }
            c->finished = true;
        } else if (c->input_size == k_block_size) {
            write_block(c, false);
        } else if (flush != Z_NO_FLUSH && !c->sync_emitted) {
            if (c->input_size)
                write_block(c, false);
            write_stored_block(c, false, 0);
            c->sync_emitted = true;
        } else {
            break;
        }
    }
    return Z_OK;
}

int z_deflateEnd(z_streamp strm)
{
    if (!strm || !strm->state)
        return Z_STREAM_ERROR;
    z_free_mem(strm, strm->state);
    strm->state = 0;
    return Z_OK;
}

} // extern "C"
