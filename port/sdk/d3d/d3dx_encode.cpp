// port/sdk/d3d: JPEG and PNG encoding for D3DXSaveSurfaceToFileInMemory (d3dx_encode.h). The JPEG writer follows IJG
// libjpeg 6b (what d3dx9.lib links; the retail writer sets in_color_space = JCS_RGB, calls jpeg_set_defaults and
// changes nothing else) wherever the output depends on it: jccolor.c's fixed-point RGB -> YCbCr, jcsample.c's h2v2
// averaging (bias 1, 2, 1, 2, ...) after jcprepct.c's edge replication, jfdctint.c's ISLOW FDCT, jcdctmgr.c's
// rounded quantization, jccoefct.c's dummy blocks past the image's edges, jchuff.c's encoder, jcmarker.c's markers.
// The tables are libjpeg's (checked against d3dx9.lib's copies in the retail image: std_luminance_quant_tbl at
// 0x8200dc60, std_chrominance_quant_tbl 0x8200db60, the Huffman bits / huffval arrays 0x8200dd60..0x8200df0c). The PNG
// writer keeps alpha only when asked, picks each row's filter by libpng's minimum-sum-of-absolute-differences
// heuristic and deflates with hash-chained LZ77 (lazy matching) into stored, fixed or dynamic Huffman blocks,
// whichever is smallest.
#include "d3dx_encode.h"

#include <stdlib.h>
#include <string.h>

namespace {

// ---- output ----

struct s_output {
    uint8_t *data;
    uint32_t size, capacity;
    int error;
};

bool reserve(s_output *out, uint32_t more)
{
    if (out->error)
        return false;
    if (more <= out->capacity - out->size)
        return true;
    uint64_t want = (uint64_t)out->size + more;
    uint64_t capacity = out->capacity != 0 ? out->capacity : 4096u;
    while (capacity < want)
        capacity *= 2u;
    if (capacity > 0xffffffffu) {
        out->error = 1;
        return false;
    }
    uint8_t *data = (uint8_t *)malloc((size_t)capacity);
    if (data == 0) {
        out->error = 1;
        return false;
    }
    if (out->size != 0)
        memcpy(data, out->data, out->size);
    free(out->data);
    out->data = data;
    out->capacity = (uint32_t)capacity;
    return true;
}

void put_u8(s_output *out, uint32_t value)
{
    if (reserve(out, 1))
        out->data[out->size++] = (uint8_t)value;
}

void put_u16be(s_output *out, uint32_t value)
{
    put_u8(out, value >> 8);
    put_u8(out, value);
}

void put_u32be(s_output *out, uint32_t value)
{
    put_u16be(out, value >> 16);
    put_u16be(out, value);
}

void put_bytes(s_output *out, const void *data, uint32_t size)
{
    if (size != 0 && reserve(out, size)) {
        memcpy(out->data + out->size, data, size);
        out->size += size;
    }
}

uint32_t channel(uint32_t pixel, uint32_t shift)
{
    return (pixel >> shift) & 0xffu;
}

// ---- JPEG ----

// jcparam.c: the Annex K.1 / K.2 quantization tables, in natural (row-major) order.
const uint8_t k_luminance_quant[64] = {
    16, 11, 10, 16, 24, 40, 51, 61,
    12, 12, 14, 19, 26, 58, 60, 55,
    14, 13, 16, 24, 40, 57, 69, 56,
    14, 17, 22, 29, 51, 87, 80, 62,
    18, 22, 37, 56, 68, 109, 103, 77,
    24, 35, 55, 64, 81, 104, 113, 92,
    49, 64, 78, 87, 103, 121, 120, 101,
    72, 92, 95, 98, 112, 100, 103, 99,
};
const uint8_t k_chrominance_quant[64] = {
    17, 18, 24, 47, 99, 99, 99, 99,
    18, 21, 26, 66, 99, 99, 99, 99,
    24, 26, 56, 99, 99, 99, 99, 99,
    47, 66, 99, 99, 99, 99, 99, 99,
    99, 99, 99, 99, 99, 99, 99, 99,
    99, 99, 99, 99, 99, 99, 99, 99,
    99, 99, 99, 99, 99, 99, 99, 99,
    99, 99, 99, 99, 99, 99, 99, 99,
};

// jutils.c: the zigzag order (the k-th coefficient of a scan is natural index k_zigzag[k]).
const uint8_t k_zigzag[64] = {
    0, 1, 8, 16, 9, 2, 3, 10, 17, 24, 32, 25, 18, 11, 4, 5,
    12, 19, 26, 33, 40, 48, 41, 34, 27, 20, 13, 6, 7, 14, 21, 28,
    35, 42, 49, 56, 57, 50, 43, 36, 29, 22, 15, 23, 30, 37, 44, 51,
    58, 59, 52, 45, 38, 31, 39, 46, 53, 60, 61, 54, 47, 55, 62, 63,
};

// jcparam.c: the Annex K.3 Huffman tables (code counts per length 1..16, then the values).
const uint8_t k_dc_luminance_counts[16] = { 0, 1, 5, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0 };
const uint8_t k_dc_chrominance_counts[16] = { 0, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0 };
const uint8_t k_dc_values[12] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
const uint8_t k_ac_luminance_counts[16] = { 0, 2, 1, 3, 3, 2, 4, 3, 5, 5, 4, 4, 0, 0, 1, 0x7d };
const uint8_t k_ac_luminance_values[162] = {
    0x01, 0x02, 0x03, 0x00, 0x04, 0x11, 0x05, 0x12, 0x21, 0x31, 0x41, 0x06, 0x13, 0x51, 0x61, 0x07,
    0x22, 0x71, 0x14, 0x32, 0x81, 0x91, 0xa1, 0x08, 0x23, 0x42, 0xb1, 0xc1, 0x15, 0x52, 0xd1, 0xf0,
    0x24, 0x33, 0x62, 0x72, 0x82, 0x09, 0x0a, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x25, 0x26, 0x27, 0x28,
    0x29, 0x2a, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49,
    0x4a, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5a, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69,
    0x6a, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89,
    0x8a, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9a, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7,
    0xa8, 0xa9, 0xaa, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xc2, 0xc3, 0xc4, 0xc5,
    0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xe1, 0xe2,
    0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8,
    0xf9, 0xfa,
};
const uint8_t k_ac_chrominance_counts[16] = { 0, 2, 1, 2, 4, 4, 3, 4, 7, 5, 4, 4, 0, 1, 2, 0x77 };
const uint8_t k_ac_chrominance_values[162] = {
    0x00, 0x01, 0x02, 0x03, 0x11, 0x04, 0x05, 0x21, 0x31, 0x06, 0x12, 0x41, 0x51, 0x07, 0x61, 0x71,
    0x13, 0x22, 0x32, 0x81, 0x08, 0x14, 0x42, 0x91, 0xa1, 0xb1, 0xc1, 0x09, 0x23, 0x33, 0x52, 0xf0,
    0x15, 0x62, 0x72, 0xd1, 0x0a, 0x16, 0x24, 0x34, 0xe1, 0x25, 0xf1, 0x17, 0x18, 0x19, 0x1a, 0x26,
    0x27, 0x28, 0x29, 0x2a, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48,
    0x49, 0x4a, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5a, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
    0x69, 0x6a, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
    0x88, 0x89, 0x8a, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9a, 0xa2, 0xa3, 0xa4, 0xa5,
    0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xc2, 0xc3,
    0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda,
    0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8,
    0xf9, 0xfa,
};

struct s_huffman_table {
    const uint8_t *counts; // 16
    const uint8_t *values;
    uint32_t total;
};

const s_huffman_table k_huffman_tables[4] = { // DC 0, DC 1, AC 0, AC 1
    { k_dc_luminance_counts, k_dc_values, 12 },
    { k_dc_chrominance_counts, k_dc_values, 12 },
    { k_ac_luminance_counts, k_ac_luminance_values, 162 },
    { k_ac_chrominance_counts, k_ac_chrominance_values, 162 },
};

struct s_huffman_codes {
    uint16_t code[256];
    uint8_t size[256]; // 0: no code
};

// jchuff.c jpeg_make_c_derived_tbl: the canonical codes, by value.
void build_huffman_codes(const s_huffman_table &table, s_huffman_codes *codes)
{
    memset(codes, 0, sizeof(*codes));
    uint32_t code = 0, p = 0;
    for (uint32_t length = 1; length <= 16; length++) {
        for (uint32_t i = 0; i < table.counts[length - 1]; i++, p++) {
            codes->code[table.values[p]] = (uint16_t)code++;
            codes->size[table.values[p]] = (uint8_t)length;
        }
        code <<= 1;
    }
}

// jchuff.c emit_bits / flush_bits: the bits MSB first, a 0 stuffed after every 0xff byte.
struct s_jpeg_bits {
    s_output *out;
    uint32_t buffer; // the pending bits, left-aligned at bit 23
    uint32_t count;
};

void emit_bits(s_jpeg_bits *bits, uint32_t code, uint32_t size)
{
    uint32_t buffer = code & ((1u << size) - 1u);
    bits->count += size;
    buffer <<= 24u - bits->count;
    buffer |= bits->buffer;
    while (bits->count >= 8u) {
        const uint32_t c = (buffer >> 16) & 0xffu;
        put_u8(bits->out, c);
        if (c == 0xffu)
            put_u8(bits->out, 0);
        buffer <<= 8;
        bits->count -= 8u;
    }
    bits->buffer = buffer;
}

void flush_bits(s_jpeg_bits *bits)
{
    emit_bits(bits, 0x7f, 7); // fill any partial byte with ones
    bits->buffer = 0;
    bits->count = 0;
}

// jfdctint.c: the slow-but-accurate integer FDCT (Loeffler, Ligtenberg and Moschytz), 13-bit constants, 2 extra bits
// of precision between the passes. The output is scaled up by 8 (jcdctmgr.c divides by 8 * the quantizer).
#define FIX_0_298631336 2446
#define FIX_0_390180644 3196
#define FIX_0_541196100 4433
#define FIX_0_765366865 6270
#define FIX_0_899976223 7373
#define FIX_1_175875602 9633
#define FIX_1_501321110 12299
#define FIX_1_847759065 15137
#define FIX_1_961570560 16069
#define FIX_2_053119869 16819
#define FIX_2_562915447 20995
#define FIX_3_072711026 25172

int32_t descale(int32_t x, int n)
{
    return (x + (1 << (n - 1))) >> n;
}

void forward_dct(int32_t data[64])
{
    const int const_bits = 13, pass1_bits = 2;
    for (int pass = 0; pass < 2; pass++) {
        // pass 0: rows (stride 1, step 8); pass 1: columns (stride 8, step 1)
        const int stride = pass == 0 ? 1 : 8, step = pass == 0 ? 8 : 1;
        for (int line = 0; line < 8; line++) {
            int32_t *d = data + line * step;
            const int32_t tmp0 = d[0 * stride] + d[7 * stride], tmp7 = d[0 * stride] - d[7 * stride];
            const int32_t tmp1 = d[1 * stride] + d[6 * stride], tmp6 = d[1 * stride] - d[6 * stride];
            const int32_t tmp2 = d[2 * stride] + d[5 * stride], tmp5 = d[2 * stride] - d[5 * stride];
            const int32_t tmp3 = d[3 * stride] + d[4 * stride], tmp4 = d[3 * stride] - d[4 * stride];
            // even part
            const int32_t tmp10 = tmp0 + tmp3, tmp13 = tmp0 - tmp3, tmp11 = tmp1 + tmp2, tmp12 = tmp1 - tmp2;
            int32_t z1 = (tmp12 + tmp13) * FIX_0_541196100;
            if (pass == 0) {
                d[0 * stride] = (tmp10 + tmp11) * (1 << pass1_bits);
                d[4 * stride] = (tmp10 - tmp11) * (1 << pass1_bits);
            } else {
                d[0 * stride] = descale(tmp10 + tmp11, pass1_bits);
                d[4 * stride] = descale(tmp10 - tmp11, pass1_bits);
            }
            const int shift = pass == 0 ? const_bits - pass1_bits : const_bits + pass1_bits;
            d[2 * stride] = descale(z1 + tmp13 * FIX_0_765366865, shift);
            d[6 * stride] = descale(z1 - tmp12 * FIX_1_847759065, shift);
            // odd part
            z1 = tmp4 + tmp7;
            int32_t z2 = tmp5 + tmp6, z3 = tmp4 + tmp6, z4 = tmp5 + tmp7;
            const int32_t z5 = (z3 + z4) * FIX_1_175875602;
            const int32_t t4 = tmp4 * FIX_0_298631336, t5 = tmp5 * FIX_2_053119869;
            const int32_t t6 = tmp6 * FIX_3_072711026, t7 = tmp7 * FIX_1_501321110;
            z1 *= -FIX_0_899976223;
            z2 *= -FIX_2_562915447;
            z3 = z3 * -FIX_1_961570560 + z5;
            z4 = z4 * -FIX_0_390180644 + z5;
            d[7 * stride] = descale(t4 + z1 + z3, shift);
            d[5 * stride] = descale(t5 + z2 + z4, shift);
            d[3 * stride] = descale(t6 + z2 + z3, shift);
            d[1 * stride] = descale(t7 + z1 + z4, shift);
        }
    }
}

// A colour component's samples, padded as libjpeg pads them before the DCT: blocks_w * 8 by blocks_h * 8.
struct s_jpeg_plane {
    uint8_t *samples;
    uint32_t blocks_w, blocks_h, stride;
};

struct s_jpeg_component {
    uint32_t id, h, v, table; // table: quantization and Huffman table index
    s_jpeg_plane plane;
    int32_t last_dc;
};

// jcdctmgr.c forward_DCT (ISLOW): level shift, FDCT, divide by 8 * the quantizer with rounding toward the nearer value.
void encode_block_coefficients(const s_jpeg_plane &plane, uint32_t bx, uint32_t by, const uint32_t divisors[64],
                               int32_t out[64])
{
    int32_t data[64];
    for (uint32_t y = 0; y < 8; y++) {
        const uint8_t *row = plane.samples + (size_t)(by * 8u + y) * plane.stride + bx * 8u;
        for (uint32_t x = 0; x < 8; x++)
            data[y * 8u + x] = (int32_t)row[x] - 128;
    }
    forward_dct(data);
    for (uint32_t i = 0; i < 64; i++) {
        const int32_t q = (int32_t)divisors[i];
        int32_t t = data[i];
        if (t < 0) {
            t = -t + (q >> 1);
            t = t >= q ? t / q : 0;
            t = -t;
        } else {
            t += q >> 1;
            t = t >= q ? t / q : 0;
        }
        out[i] = t;
    }
}

// jchuff.c encode_one_block.
void encode_block_huffman(s_jpeg_bits *bits, const int32_t block[64], int32_t *last_dc, const s_huffman_codes &dc,
                          const s_huffman_codes &ac)
{
    int32_t temp = block[0] - *last_dc, temp2 = temp;
    *last_dc = block[0];
    if (temp < 0) {
        temp = -temp;
        temp2--;
    }
    uint32_t nbits = 0;
    while (temp != 0) {
        nbits++;
        temp >>= 1;
    }
    emit_bits(bits, dc.code[nbits], dc.size[nbits]);
    if (nbits != 0)
        emit_bits(bits, (uint32_t)temp2, nbits);
    uint32_t run = 0;
    for (uint32_t k = 1; k < 64; k++) {
        temp = block[k_zigzag[k]];
        if (temp == 0) {
            run++;
            continue;
        }
        while (run > 15) {
            emit_bits(bits, ac.code[0xf0], ac.size[0xf0]);
            run -= 16;
        }
        temp2 = temp;
        if (temp < 0) {
            temp = -temp;
            temp2--;
        }
        nbits = 1;
        while ((temp >>= 1) != 0)
            nbits++;
        const uint32_t symbol = (run << 4) + nbits;
        emit_bits(bits, ac.code[symbol], ac.size[symbol]);
        emit_bits(bits, (uint32_t)temp2, nbits);
        run = 0;
    }
    if (run > 0)
        emit_bits(bits, ac.code[0], ac.size[0]);
}

void put_marker(s_output *out, uint32_t marker)
{
    put_u8(out, 0xff);
    put_u8(out, marker);
}

int encode_jpeg(const uint32_t *pixels, uint32_t width, uint32_t height, uint32_t pitch, uint32_t quality,
                s_output *out)
{
    if (width > 65500u || height > 65500u) // JPEG_MAX_DIMENSION
        return 0;
    // jcparam.c jpeg_set_quality / jpeg_add_quant_table, forcing baseline (8-bit) quantizers
    quality = quality == 0 ? PORT_IMAGE_JPEG_DEFAULT_QUALITY : quality > 100u ? 100u : quality;
    const uint32_t scale = quality < 50u ? 5000u / quality : 200u - quality * 2u;
    uint32_t quant[2][64], divisors[2][64];
    for (uint32_t i = 0; i < 64; i++) {
        const uint8_t base[2] = { k_luminance_quant[i], k_chrominance_quant[i] };
        for (uint32_t t = 0; t < 2; t++) {
            uint32_t q = (base[t] * scale + 50u) / 100u;
            q = q == 0 ? 1u : q > 255u ? 255u : q;
            quant[t][i] = q;
            divisors[t][i] = q << 3;
        }
    }
    s_huffman_codes codes[4];
    for (uint32_t t = 0; t < 4; t++)
        build_huffman_codes(k_huffman_tables[t], &codes[t]);

    // jpeg_set_colorspace(JCS_YCbCr): Y 2x2 with table 0, Cb and Cr 1x1 with table 1
    s_jpeg_component comps[3] = {
        { 1, 2, 2, 0, { 0, 0, 0, 0 }, 0 },
        { 2, 1, 1, 1, { 0, 0, 0, 0 }, 0 },
        { 3, 1, 1, 1, { 0, 0, 0, 0 }, 0 },
    };
    const uint32_t mcus_x = (width + 15u) / 16u, mcus_y = (height + 15u) / 16u;
    for (uint32_t c = 0; c < 3; c++) {
        s_jpeg_plane &plane = comps[c].plane;
        // jdiv_round_up(jdiv_round_up(image * h, hmax), 8)
        plane.blocks_w = ((width * comps[c].h + 1u) / 2u + 7u) / 8u;
        plane.blocks_h = ((height * comps[c].v + 1u) / 2u + 7u) / 8u;
        plane.stride = plane.blocks_w * 8u;
        plane.samples = (uint8_t *)malloc((size_t)plane.stride * plane.blocks_h * 8u);
    }
    // full-resolution Cb and Cr rows, two at a time (the h2v2 downsampler's input), padded on the right to the
    // downsampled width * 2 by repeating the last column (jcsample.c expand_right_edge)
    const uint32_t chroma_input_w = comps[1].plane.stride * 2u;
    uint8_t *chroma_rows = (uint8_t *)malloc((size_t)chroma_input_w * 4u);
    int result = 0;
    if (comps[0].plane.samples == 0 || comps[1].plane.samples == 0 || comps[2].plane.samples == 0 ||
        chroma_rows == 0)
        goto done;

    {
        // jccolor.c rgb_ycc_convert: 16-bit fixed point (FIX(x) = x * 65536 rounded), Cb / Cr offset by 128 with
        // the rounding term one less so the result stays within 0..255
        const int32_t one_half = 1 << 15, cbcr_offset = 128 << 16;
        const uint32_t chroma_rows_real = (height + 1u) / 2u;
        for (uint32_t pair = 0; pair < chroma_rows_real; pair++) {
            for (uint32_t r = 0; r < 2; r++) {
                // jcprepct.c expand_bottom_edge: an odd last row is repeated to complete the pair
                const uint32_t y = pair * 2u + r < height ? pair * 2u + r : height - 1u;
                const uint32_t *src = pixels + (size_t)y * pitch;
                uint8_t *luma = comps[0].plane.samples + (size_t)(pair * 2u + r) * comps[0].plane.stride;
                uint8_t *cb = chroma_rows + (size_t)r * chroma_input_w;
                uint8_t *cr = chroma_rows + (size_t)(2u + r) * chroma_input_w;
                for (uint32_t x = 0; x < width; x++) {
                    const int32_t red = (int32_t)channel(src[x], 16), green = (int32_t)channel(src[x], 8),
                                  blue = (int32_t)channel(src[x], 0);
                    luma[x] = (uint8_t)((19595 * red + 38470 * green + 7471 * blue + one_half) >> 16);
                    cb[x] = (uint8_t)((-11059 * red - 21709 * green + 32768 * blue + cbcr_offset + one_half - 1) >> 16);
                    cr[x] = (uint8_t)((32768 * red - 27439 * green - 5329 * blue + cbcr_offset + one_half - 1) >> 16);
                }
                for (uint32_t x = width; x < comps[0].plane.stride; x++)
                    luma[x] = luma[width - 1u];
                for (uint32_t x = width; x < chroma_input_w; x++) {
                    cb[x] = cb[width - 1u];
                    cr[x] = cr[width - 1u];
                }
            }
            // jcsample.c h2v2_downsample
            for (uint32_t c = 1; c < 3; c++) {
                const uint8_t *in0 = chroma_rows + (size_t)(c == 1 ? 0u : 2u) * chroma_input_w;
                const uint8_t *in1 = in0 + chroma_input_w;
                uint8_t *dst = comps[c].plane.samples + (size_t)pair * comps[c].plane.stride;
                uint32_t bias = 1;
                for (uint32_t x = 0; x < comps[c].plane.stride; x++) {
                    dst[x] = (uint8_t)((in0[2u * x] + in0[2u * x + 1u] + in1[2u * x] + in1[2u * x + 1u] + bias) >> 2);
                    bias ^= 3u;
                }
            }
        }
        // jcprepct.c: below the image, the last row produced is repeated to fill the blocks
        for (uint32_t c = 0; c < 3; c++) {
            const s_jpeg_plane &plane = comps[c].plane;
            const uint32_t rows = c == 0 ? chroma_rows_real * 2u : chroma_rows_real;
            for (uint32_t y = rows; y < plane.blocks_h * 8u; y++)
                memcpy(plane.samples + (size_t)y * plane.stride, plane.samples + (size_t)(rows - 1u) * plane.stride,
                       plane.stride);
        }
        // a luma row past an odd height is the last row again (already, as the pair's second row)
    }

    // jcmarker.c: SOI, JFIF APP0, DQT 0 and 1, SOF0, DHT DC 0 / AC 0 / DC 1 / AC 1, SOS
    put_marker(out, 0xd8);
    put_marker(out, 0xe0);
    put_u16be(out, 16);
    put_bytes(out, "JFIF", 5);
    put_u8(out, 1); // version 1.01
    put_u8(out, 1);
    put_u8(out, 0); // density unit: none (aspect ratio 1:1)
    put_u16be(out, 1);
    put_u16be(out, 1);
    put_u8(out, 0); // no thumbnail
    put_u8(out, 0);
    for (uint32_t t = 0; t < 2; t++) {
        put_marker(out, 0xdb);
        put_u16be(out, 64 + 1 + 2);
        put_u8(out, t); // 8-bit precision, table t
        for (uint32_t k = 0; k < 64; k++)
            put_u8(out, quant[t][k_zigzag[k]]);
    }
    put_marker(out, 0xc0);
    put_u16be(out, 3 * 3 + 2 + 5 + 1);
    put_u8(out, 8);
    put_u16be(out, height);
    put_u16be(out, width);
    put_u8(out, 3);
    for (uint32_t c = 0; c < 3; c++) {
        put_u8(out, comps[c].id);
        put_u8(out, comps[c].h << 4 | comps[c].v);
        put_u8(out, comps[c].table);
    }
    for (uint32_t t = 0; t < 2; t++) {
        for (uint32_t ac = 0; ac < 2; ac++) {
            const s_huffman_table &table = k_huffman_tables[ac * 2u + t];
            put_marker(out, 0xc4);
            put_u16be(out, 2 + 1 + 16 + table.total);
            put_u8(out, t | ac << 4);
            put_bytes(out, table.counts, 16);
            put_bytes(out, table.values, table.total);
        }
    }
    put_marker(out, 0xda);
    put_u16be(out, 2 * 3 + 2 + 1 + 3);
    put_u8(out, 3);
    for (uint32_t c = 0; c < 3; c++) {
        put_u8(out, comps[c].id);
        put_u8(out, comps[c].table << 4 | comps[c].table);
    }
    put_u8(out, 0);  // Ss
    put_u8(out, 63); // Se
    put_u8(out, 0);  // Ah, Al

    {
        // jccoefct.c compress_data: the MCUs in order, each component's blocks row by row within the MCU; a block
        // past the component's last block column or row is a dummy, all zero but for the DC of the block before it
        s_jpeg_bits bits = { out, 0, 0 };
        int32_t block[64];
        for (uint32_t my = 0; my < mcus_y && !out->error; my++) {
            for (uint32_t mx = 0; mx < mcus_x; mx++) {
                for (uint32_t c = 0; c < 3; c++) {
                    s_jpeg_component &comp = comps[c];
                    int32_t previous_dc = 0;
                    for (uint32_t yi = 0; yi < comp.v; yi++) {
                        for (uint32_t xi = 0; xi < comp.h; xi++) {
                            const uint32_t bx = mx * comp.h + xi, by = my * comp.v + yi;
                            if (bx < comp.plane.blocks_w && by < comp.plane.blocks_h) {
                                encode_block_coefficients(comp.plane, bx, by, divisors[comp.table], block);
                            } else {
                                memset(block, 0, sizeof(block));
                                block[0] = previous_dc;
                            }
                            previous_dc = block[0];
                            encode_block_huffman(&bits, block, &comp.last_dc, codes[comp.table],
                                                 codes[2u + comp.table]);
                        }
                    }
                }
            }
        }
        flush_bits(&bits);
    }
    put_marker(out, 0xd9);
    result = !out->error;

done:
    for (uint32_t c = 0; c < 3; c++)
        free(comps[c].plane.samples);
    free(chroma_rows);
    return result;
}

// ---- deflate (RFC 1951) in a zlib stream (RFC 1950) ----

const uint32_t k_window = 32768u, k_window_mask = k_window - 1u;
const uint32_t k_hash_bits = 15u, k_hash_size = 1u << k_hash_bits;
const uint32_t k_min_match = 3u, k_max_match = 258u;
const uint32_t k_max_chain = 128u, k_nice_match = 128u, k_lazy_match = 32u;
const uint32_t k_too_far = 4096u; // a 3-byte match further back costs more than three literals (zlib's TOO_FAR)
const uint32_t k_block_tokens = 1u << 16;

// RFC 1951 3.2.5: the length codes 257..285 and distance codes 0..29 (base, extra bits).
struct s_code_range {
    uint16_t base;
    uint8_t extra;
};

void length_codes(s_code_range out[29])
{
    uint32_t base = 3;
    for (uint32_t code = 0; code < 28; code++) {
        out[code].extra = (uint8_t)(code < 8 ? 0 : (code - 4) / 4);
        out[code].base = (uint16_t)base;
        base += 1u << out[code].extra;
    }
    out[28].base = 258; // 285: 258 alone (284 stops at 257)
    out[28].extra = 0;
}

void distance_codes(s_code_range out[30])
{
    uint32_t base = 1;
    for (uint32_t code = 0; code < 30; code++) {
        out[code].extra = (uint8_t)(code < 4 ? 0 : (code - 2) / 2);
        out[code].base = (uint16_t)base;
        base += 1u << out[code].extra;
    }
}

struct s_token {
    uint16_t value;    // a literal byte, or a match length (3..258)
    uint16_t distance; // 0: literal
};

struct s_deflate {
    s_output *out;
    uint32_t bit_buffer, bit_count;
    s_code_range lengths[29], distances[30];
    uint8_t length_code[k_max_match + 1]; // match length -> code index 0..28
    uint8_t distance_code_low[256];       // distance 1..256 -> code, by distance - 1
    uint8_t distance_code_high[256];      // distance 257..32768 -> code, by (distance - 1) >> 7
};

void put_bits(s_deflate *d, uint32_t value, uint32_t count)
{
    d->bit_buffer |= value << d->bit_count;
    d->bit_count += count;
    while (d->bit_count >= 8u) {
        put_u8(d->out, d->bit_buffer & 0xffu);
        d->bit_buffer >>= 8;
        d->bit_count -= 8u;
    }
}

void align_bits(s_deflate *d)
{
    if (d->bit_count != 0)
        put_bits(d, 0, 8u - d->bit_count);
}

uint32_t distance_code(const s_deflate *d, uint32_t distance)
{
    return distance <= 256u ? d->distance_code_low[distance - 1u] : d->distance_code_high[(distance - 1u) >> 7];
}

// Huffman code lengths for `count` symbols, none longer than `limit`: an optimal tree (two-queue construction over
// the symbols sorted by frequency), then lengths over the limit folded back as miniz does
// (tdefl_huffman_enforce_max_code_size), the shortest lengths going to the most frequent symbols. At least two
// symbols are given lengths, so every code is complete.
void build_code_lengths(const uint32_t *frequencies, uint32_t count, uint32_t limit, uint8_t *lengths)
{
    uint16_t symbols[288];
    uint32_t weight[2 * 288], parent[2 * 288];
    uint32_t used = 0;
    memset(lengths, 0, count);
    for (uint32_t s = 0; s < count; s++)
        if (frequencies[s] != 0)
            symbols[used++] = (uint16_t)s;
    if (used < 2) { // complete the code with a second (unused) symbol
        const uint16_t other = used == 1 && symbols[0] == 0 ? 1 : 0;
        if (used == 0)
            symbols[used++] = 1;
        symbols[used++] = other;
    }
    // ascending frequency, then ascending symbol (insertion sort: at most 288)
    for (uint32_t i = 1; i < used; i++) {
        const uint16_t s = symbols[i];
        uint32_t j = i;
        while (j > 0 && (frequencies[symbols[j - 1]] > frequencies[s] ||
                         (frequencies[symbols[j - 1]] == frequencies[s] && symbols[j - 1] > s))) {
            symbols[j] = symbols[j - 1];
            j--;
        }
        symbols[j] = s;
    }
    // nodes 0..used-1 are the leaves in that order; internal nodes follow, made in nondecreasing weight
    for (uint32_t i = 0; i < used; i++)
        weight[i] = frequencies[symbols[i]];
    uint32_t leaf = 0, internal = used, nodes = used;
    for (uint32_t made = 0; made + 1 < used; made++) {
        uint32_t pick[2];
        for (uint32_t k = 0; k < 2; k++) {
            if (leaf < used && (internal >= nodes || weight[leaf] <= weight[internal]))
                pick[k] = leaf++;
            else
                pick[k] = internal++;
        }
        weight[nodes] = weight[pick[0]] + weight[pick[1]];
        parent[pick[0]] = parent[pick[1]] = nodes;
        nodes++;
    }
    // depths: the root is the last node; parents always come after their children
    uint32_t depth[2 * 288];
    depth[nodes - 1u] = 0;
    for (uint32_t n = nodes - 1u; n-- > 0;)
        depth[n] = depth[parent[n]] + 1u;
    uint32_t histogram[33];
    memset(histogram, 0, sizeof(histogram));
    for (uint32_t i = 0; i < used; i++)
        histogram[depth[i] > 32u ? 32u : depth[i]]++;
    for (uint32_t length = limit + 1u; length <= 32u; length++) {
        histogram[limit] += histogram[length];
        histogram[length] = 0;
    }
    uint32_t total = 0;
    for (uint32_t length = limit; length > 0; length--)
        total += histogram[length] << (limit - length);
    while (total != 1u << limit) {
        histogram[limit]--;
        for (uint32_t length = limit - 1u; length > 0; length--) {
            if (histogram[length] != 0) {
                histogram[length]--;
                histogram[length + 1u] += 2u;
                break;
            }
        }
        total--;
    }
    // the least frequent symbols (first in `symbols`) take the longest codes
    uint32_t next = 0;
    for (uint32_t length = limit; length > 0; length--)
        for (uint32_t k = 0; k < histogram[length]; k++)
            lengths[symbols[next++]] = (uint8_t)length;
}

// RFC 1951 3.2.2: canonical codes from lengths, bit-reversed (deflate sends Huffman codes MSB first into an LSB-first
// stream).
void build_codes(const uint8_t *lengths, uint32_t count, uint16_t *codes)
{
    uint32_t histogram[16], next[16];
    memset(histogram, 0, sizeof(histogram));
    for (uint32_t s = 0; s < count; s++)
        histogram[lengths[s]]++;
    histogram[0] = 0;
    uint32_t code = 0;
    for (uint32_t length = 1; length < 16; length++) {
        code = (code + histogram[length - 1u]) << 1;
        next[length] = code;
    }
    for (uint32_t s = 0; s < count; s++) {
        const uint32_t length = lengths[s];
        if (length == 0) {
            codes[s] = 0;
            continue;
        }
        uint32_t c = next[length]++, reversed = 0;
        for (uint32_t b = 0; b < length; b++) {
            reversed = reversed << 1 | (c & 1u);
            c >>= 1;
        }
        codes[s] = (uint16_t)reversed;
    }
}

// The code length alphabet's run-length coding of HLIT + HDIST lengths (RFC 1951 3.2.7): symbol | extra << 8.
uint32_t run_length_code_lengths(const uint8_t *lengths, uint32_t count, uint16_t *out)
{
    uint32_t n = 0;
    for (uint32_t i = 0; i < count;) {
        const uint8_t value = lengths[i];
        uint32_t run = 1;
        while (i + run < count && lengths[i + run] == value)
            run++;
        i += run;
        if (value == 0) {
            while (run >= 11u) {
                const uint32_t r = run > 138u ? 138u : run;
                out[n++] = (uint16_t)(18u | (r - 11u) << 8);
                run -= r;
            }
            if (run >= 3u) {
                out[n++] = (uint16_t)(17u | (run - 3u) << 8);
                run = 0;
            }
        } else {
            out[n++] = value;
            run--;
            while (run >= 3u) {
                const uint32_t r = run > 6u ? 6u : run;
                out[n++] = (uint16_t)(16u | (r - 3u) << 8);
                run -= r;
            }
        }
        while (run-- > 0)
            out[n++] = value;
    }
    return n;
}

const uint8_t k_code_length_order[19] = { 16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15 };

void put_tokens(s_deflate *d, const s_token *tokens, uint32_t count, const uint8_t *litlen_lengths,
                const uint16_t *litlen_codes, const uint8_t *distance_lengths, const uint16_t *distance_codes_)
{
    for (uint32_t i = 0; i < count; i++) {
        const s_token &t = tokens[i];
        if (t.distance == 0) {
            put_bits(d, litlen_codes[t.value], litlen_lengths[t.value]);
            continue;
        }
        const uint32_t lc = d->length_code[t.value];
        put_bits(d, litlen_codes[257u + lc], litlen_lengths[257u + lc]);
        put_bits(d, t.value - d->lengths[lc].base, d->lengths[lc].extra);
        const uint32_t dc = distance_code(d, t.distance);
        put_bits(d, distance_codes_[dc], distance_lengths[dc]);
        put_bits(d, t.distance - d->distances[dc].base, d->distances[dc].extra);
    }
    put_bits(d, litlen_codes[256], litlen_lengths[256]);
}

// One block of tokens covering input[start, end): stored, fixed or dynamic Huffman, the smallest.
void flush_block(s_deflate *d, const uint8_t *input, uint32_t start, uint32_t end, const s_token *tokens,
                 uint32_t count, bool final)
{
    uint32_t litlen_frequencies[286], distance_frequencies[30];
    memset(litlen_frequencies, 0, sizeof(litlen_frequencies));
    memset(distance_frequencies, 0, sizeof(distance_frequencies));
    for (uint32_t i = 0; i < count; i++) {
        if (tokens[i].distance == 0) {
            litlen_frequencies[tokens[i].value]++;
        } else {
            litlen_frequencies[257u + d->length_code[tokens[i].value]]++;
            distance_frequencies[distance_code(d, tokens[i].distance)]++;
        }
    }
    litlen_frequencies[256] = 1;

    // dynamic tables
    uint8_t litlen_lengths[286], distance_lengths[30], all_lengths[286 + 30];
    build_code_lengths(litlen_frequencies, 286, 15, litlen_lengths);
    build_code_lengths(distance_frequencies, 30, 15, distance_lengths);
    uint32_t hlit = 286, hdist = 30;
    while (hlit > 257u && litlen_lengths[hlit - 1u] == 0)
        hlit--;
    while (hdist > 1u && distance_lengths[hdist - 1u] == 0)
        hdist--;
    memcpy(all_lengths, litlen_lengths, hlit);
    memcpy(all_lengths + hlit, distance_lengths, hdist);
    uint16_t runs[286 + 30];
    const uint32_t run_count = run_length_code_lengths(all_lengths, hlit + hdist, runs);
    uint32_t code_length_frequencies[19];
    memset(code_length_frequencies, 0, sizeof(code_length_frequencies));
    for (uint32_t i = 0; i < run_count; i++)
        code_length_frequencies[runs[i] & 0xffu]++;
    uint8_t code_length_lengths[19];
    build_code_lengths(code_length_frequencies, 19, 7, code_length_lengths);
    uint32_t hclen = 19;
    while (hclen > 4u && code_length_lengths[k_code_length_order[hclen - 1u]] == 0)
        hclen--;

    // the cost of each kind, in bits
    uint64_t dynamic_bits = 3u + 5u + 5u + 4u + 3u * hclen, fixed_bits = 3u;
    for (uint32_t i = 0; i < run_count; i++) {
        const uint32_t symbol = runs[i] & 0xffu;
        const uint32_t extra = symbol == 16u ? 2u : symbol == 17u ? 3u : symbol == 18u ? 7u : 0u;
        dynamic_bits += code_length_lengths[symbol] + extra;
    }
    for (uint32_t s = 0; s < 286; s++) {
        const uint32_t extra = s >= 257u ? d->lengths[s - 257u].extra : 0u;
        const uint32_t fixed = s < 144u ? 8u : s < 256u ? 9u : s < 280u ? 7u : 8u;
        dynamic_bits += (uint64_t)litlen_frequencies[s] * (litlen_lengths[s] + extra);
        fixed_bits += (uint64_t)litlen_frequencies[s] * (fixed + extra);
    }
    for (uint32_t s = 0; s < 30; s++) {
        dynamic_bits += (uint64_t)distance_frequencies[s] * (distance_lengths[s] + d->distances[s].extra);
        fixed_bits += (uint64_t)distance_frequencies[s] * (5u + d->distances[s].extra);
    }
    const uint32_t raw = end - start;
    const uint64_t stored_bits = ((uint64_t)raw + 5u * ((raw + 65534u) / 65535u + (raw == 0))) * 8u + 7u;

    if (stored_bits <= dynamic_bits && stored_bits <= fixed_bits) {
        uint32_t at = start;
        do {
            const uint32_t chunk = end - at > 65535u ? 65535u : end - at;
            const bool last = at + chunk == end;
            put_bits(d, final && last ? 1u : 0u, 3); // BTYPE 00
            align_bits(d);
            put_u8(d->out, chunk & 0xffu);
            put_u8(d->out, chunk >> 8);
            put_u8(d->out, ~chunk & 0xffu);
            put_u8(d->out, (~chunk >> 8) & 0xffu);
            put_bytes(d->out, input + at, chunk);
            at += chunk;
        } while (at < end);
        return;
    }
    if (fixed_bits <= dynamic_bits) {
        uint8_t fixed_litlen[288], fixed_distance[30];
        uint16_t litlen_codes[288], distance_codes_[30];
        for (uint32_t s = 0; s < 288; s++)
            fixed_litlen[s] = (uint8_t)(s < 144u ? 8u : s < 256u ? 9u : s < 280u ? 7u : 8u);
        memset(fixed_distance, 5, sizeof(fixed_distance));
        build_codes(fixed_litlen, 288, litlen_codes);
        build_codes(fixed_distance, 30, distance_codes_);
        put_bits(d, (final ? 1u : 0u) | 1u << 1, 3); // BTYPE 01
        put_tokens(d, tokens, count, fixed_litlen, litlen_codes, fixed_distance, distance_codes_);
        return;
    }
    uint16_t litlen_codes[286], distance_codes_[30], code_length_codes[19];
    build_codes(litlen_lengths, 286, litlen_codes);
    build_codes(distance_lengths, 30, distance_codes_);
    build_codes(code_length_lengths, 19, code_length_codes);
    put_bits(d, (final ? 1u : 0u) | 2u << 1, 3); // BTYPE 10
    put_bits(d, hlit - 257u, 5);
    put_bits(d, hdist - 1u, 5);
    put_bits(d, hclen - 4u, 4);
    for (uint32_t i = 0; i < hclen; i++)
        put_bits(d, code_length_lengths[k_code_length_order[i]], 3);
    for (uint32_t i = 0; i < run_count; i++) {
        const uint32_t symbol = runs[i] & 0xffu, extra = runs[i] >> 8;
        put_bits(d, code_length_codes[symbol], code_length_lengths[symbol]);
        if (symbol == 16u)
            put_bits(d, extra, 2);
        else if (symbol == 17u)
            put_bits(d, extra, 3);
        else if (symbol == 18u)
            put_bits(d, extra, 7);
    }
    put_tokens(d, tokens, count, litlen_lengths, litlen_codes, distance_lengths, distance_codes_);
}

uint32_t hash3(const uint8_t *p)
{
    return ((uint32_t)p[0] << 10 ^ (uint32_t)p[1] << 5 ^ (uint32_t)p[2]) & (k_hash_size - 1u);
}

struct s_matcher {
    const uint8_t *input;
    uint32_t size;
    int32_t *head; // k_hash_size: the latest position with that hash, -1 none
    int32_t *prev; // k_window: the previous position with the same hash, by position & mask
};

void insert_position(s_matcher *m, uint32_t pos)
{
    if (pos + k_min_match > m->size)
        return;
    const uint32_t h = hash3(m->input + pos);
    m->prev[pos & k_window_mask] = m->head[h];
    m->head[h] = (int32_t)pos;
}

// The longest earlier match for input[pos..] (length 0 when shorter than 3, or a far 3-byte match).
uint32_t longest_match(const s_matcher *m, uint32_t pos, uint32_t *distance)
{
    if (pos + k_min_match > m->size)
        return 0;
    const uint32_t limit = m->size - pos < k_max_match ? m->size - pos : k_max_match;
    const uint8_t *here = m->input + pos;
    int32_t candidate = m->head[hash3(here)];
    uint32_t best = 0, chain = k_max_chain;
    while (candidate >= 0 && chain-- > 0) {
        const uint32_t cand = (uint32_t)candidate;
        if (pos - cand > k_window)
            break;
        const uint8_t *there = m->input + cand;
        if (there[best] == here[best] && there[0] == here[0]) {
            uint32_t length = 0;
            while (length < limit && there[length] == here[length])
                length++;
            if (length > best) {
                best = length;
                *distance = pos - cand;
                if (length >= k_nice_match || length == limit)
                    break;
            }
        }
        const int32_t next = m->prev[cand & k_window_mask];
        if (next >= candidate) // the slot was reused by a newer position: the chain ends here
            break;
        candidate = next;
    }
    if (best < k_min_match || (best == k_min_match && *distance > k_too_far))
        return 0;
    return best;
}

int deflate_zlib(const uint8_t *input, uint32_t size, s_output *out)
{
    s_deflate *d = (s_deflate *)malloc(sizeof(s_deflate));
    s_matcher m = { input, size, (int32_t *)malloc(k_hash_size * sizeof(int32_t)),
                    (int32_t *)malloc(k_window * sizeof(int32_t)) };
    s_token *tokens = (s_token *)malloc(k_block_tokens * sizeof(s_token));
    int result = 0;
    if (d == 0 || m.head == 0 || m.prev == 0 || tokens == 0)
        goto done;
    memset(d, 0, sizeof(*d));
    d->out = out;
    length_codes(d->lengths);
    distance_codes(d->distances);
    for (uint32_t code = 0; code < 29; code++) {
        const uint32_t last = code == 27 ? 257u : d->lengths[code].base + (1u << d->lengths[code].extra) - 1u;
        for (uint32_t length = d->lengths[code].base; length <= last && length <= k_max_match; length++)
            d->length_code[length] = (uint8_t)code;
    }
    for (uint32_t code = 0; code < 30; code++) {
        const uint32_t first = d->distances[code].base, last = first + (1u << d->distances[code].extra) - 1u;
        for (uint32_t distance = first; distance <= last; distance++) {
            if (distance <= 256u)
                d->distance_code_low[distance - 1u] = (uint8_t)code;
            else
                d->distance_code_high[(distance - 1u) >> 7] = (uint8_t)code;
        }
    }
    memset(m.head, 0xff, k_hash_size * sizeof(int32_t));
    memset(m.prev, 0xff, k_window * sizeof(int32_t));

    {
        // zlib header: deflate, 32K window, default compression level, no dictionary
        put_u8(out, 0x78);
        put_u8(out, 0x9c);
        uint32_t count = 0, block_start = 0, pos = 0;
        // lazy matching: a match found at pos is kept back one byte to see whether pos + 1 starts a longer one
        bool pending = false;
        uint32_t pending_length = 0, pending_distance = 0;
        while (pos < size || pending) {
            uint32_t distance = 0;
            const uint32_t length = pos < size ? longest_match(&m, pos, &distance) : 0;
            if (pending) {
                if (length > pending_length) {
                    tokens[count].value = input[pos - 1u];
                    tokens[count].distance = 0;
                    count++;
                    pending_length = length;
                    pending_distance = distance;
                    insert_position(&m, pos);
                    pos++;
                } else {
                    tokens[count].value = (uint16_t)pending_length;
                    tokens[count].distance = (uint16_t)pending_distance;
                    count++;
                    // the match starts at pos - 1 (already in the table); the rest of it goes in now
                    const uint32_t match_end = pos - 1u + pending_length;
                    for (uint32_t p = pos; p < match_end; p++)
                        insert_position(&m, p);
                    pos = match_end;
                    pending = false;
                }
            } else if (length >= k_lazy_match) {
                tokens[count].value = (uint16_t)length;
                tokens[count].distance = (uint16_t)distance;
                count++;
                for (uint32_t p = pos; p < pos + length; p++)
                    insert_position(&m, p);
                pos += length;
            } else if (length != 0) {
                pending = true;
                pending_length = length;
                pending_distance = distance;
                insert_position(&m, pos);
                pos++;
            } else {
                tokens[count].value = input[pos];
                tokens[count].distance = 0;
                count++;
                insert_position(&m, pos);
                pos++;
            }
            // each step adds at most one token; a full block ends where the tokens do (before a pending match)
            if (count == k_block_tokens) {
                const uint32_t covered = pending ? pos - 1u : pos;
                flush_block(d, input, block_start, covered, tokens, count, false);
                block_start = covered;
                count = 0;
            }
        }
        flush_block(d, input, block_start, size, tokens, count, true);
        align_bits(d);
        // Adler-32 of the uncompressed data
        uint32_t a = 1, b = 0;
        for (uint32_t i = 0; i < size;) {
            const uint32_t chunk = size - i < 5552u ? size - i : 5552u;
            for (uint32_t k = 0; k < chunk; k++) {
                a += input[i + k];
                b += a;
            }
            a %= 65521u;
            b %= 65521u;
            i += chunk;
        }
        put_u32be(out, b << 16 | a);
        result = !out->error;
    }

done:
    free(d);
    free(m.head);
    free(m.prev);
    free(tokens);
    return result;
}

// ---- PNG ----

uint32_t crc32_update(const uint32_t table[256], uint32_t crc, const uint8_t *data, uint32_t size)
{
    for (uint32_t i = 0; i < size; i++)
        crc = table[(crc ^ data[i]) & 0xffu] ^ (crc >> 8);
    return crc;
}

void put_chunk(s_output *out, const uint32_t crc_table[256], const char type[4], const uint8_t *data, uint32_t size)
{
    put_u32be(out, size);
    put_bytes(out, type, 4);
    put_bytes(out, data, size);
    uint32_t crc = crc32_update(crc_table, 0xffffffffu, (const uint8_t *)type, 4);
    crc = crc32_update(crc_table, crc, data, size);
    put_u32be(out, crc ^ 0xffffffffu);
}

uint32_t paeth(uint32_t a, uint32_t b, uint32_t c)
{
    const int32_t p = (int32_t)a + (int32_t)b - (int32_t)c;
    const int32_t pa = p > (int32_t)a ? p - (int32_t)a : (int32_t)a - p;
    const int32_t pb = p > (int32_t)b ? p - (int32_t)b : (int32_t)b - p;
    const int32_t pc = p > (int32_t)c ? p - (int32_t)c : (int32_t)c - p;
    return pa <= pb && pa <= pc ? a : pb <= pc ? b : c;
}

int encode_png(const uint32_t *pixels, uint32_t width, uint32_t height, uint32_t pitch, uint32_t channels,
               s_output *out)
{
    const uint64_t row_bytes = (uint64_t)width * channels;
    const uint64_t filtered_size = (row_bytes + 1u) * height;
    if (width > 0x7fffffffu || height > 0x7fffffffu || filtered_size > 0x7fffffffu || row_bytes > 0x7fffffffu / 5u)
        return 0;
    const uint32_t stride = (uint32_t)row_bytes;
    uint8_t *filtered = (uint8_t *)malloc((size_t)filtered_size);
    uint8_t *rows = (uint8_t *)malloc((size_t)stride * 2u);       // the previous and current raw rows
    uint8_t *candidates = (uint8_t *)malloc((size_t)stride * 5u); // the row under each filter
    s_output zlib = { 0, 0, 0, 0 };
    int result = 0;
    if (filtered == 0 || rows == 0 || candidates == 0)
        goto done;
    {
        uint8_t *above = rows + stride, *raw = rows;
        memset(above, 0, stride); // the row above the first is zero
        for (uint32_t y = 0; y < height; y++) {
            if (y != 0) {
                uint8_t *t = above;
                above = raw;
                raw = t;
            }
            const uint32_t *src = pixels + (size_t)y * pitch;
            for (uint32_t x = 0; x < width; x++) {
                uint8_t *p = raw + (size_t)x * channels;
                p[0] = (uint8_t)channel(src[x], 16);
                p[1] = (uint8_t)channel(src[x], 8);
                p[2] = (uint8_t)channel(src[x], 0);
                if (channels == 4)
                    p[3] = (uint8_t)channel(src[x], 24);
            }
            // libpng's heuristic: the filter whose output bytes, read as signed, have the smallest absolute sum
            uint32_t best = 0;
            uint64_t best_sum = ~(uint64_t)0;
            for (uint32_t f = 0; f < 5; f++) {
                uint8_t *dst = candidates + (size_t)f * stride;
                uint64_t sum = 0;
                for (uint32_t i = 0; i < stride; i++) {
                    const uint32_t a = i >= channels ? raw[i - channels] : 0u, b = above[i],
                                   c = i >= channels ? above[i - channels] : 0u;
                    const uint32_t predictor =
                        f == 0 ? 0u : f == 1 ? a : f == 2 ? b : f == 3 ? (a + b) >> 1 : paeth(a, b, c);
                    dst[i] = (uint8_t)(raw[i] - predictor);
                    sum += dst[i] < 128u ? dst[i] : 256u - dst[i];
                }
                if (sum < best_sum) {
                    best_sum = sum;
                    best = f;
                }
            }
            uint8_t *line = filtered + (size_t)y * (stride + 1u);
            line[0] = (uint8_t)best;
            memcpy(line + 1, candidates + (size_t)best * stride, stride);
        }
        if (!deflate_zlib(filtered, (uint32_t)filtered_size, &zlib))
            goto done;

        uint32_t crc_table[256];
        for (uint32_t n = 0; n < 256; n++) {
            uint32_t c = n;
            for (int k = 0; k < 8; k++)
                c = c & 1u ? 0xedb88320u ^ (c >> 1) : c >> 1;
            crc_table[n] = c;
        }
        static const uint8_t k_signature[8] = { 137, 80, 78, 71, 13, 10, 26, 10 };
        put_bytes(out, k_signature, 8);
        uint8_t header[13];
        const uint32_t fields[2] = { width, height };
        for (uint32_t i = 0; i < 2; i++)
            for (uint32_t b = 0; b < 4; b++)
                header[i * 4u + b] = (uint8_t)(fields[i] >> (24u - 8u * b));
        header[8] = 8;                          // bit depth
        header[9] = channels == 4 ? 6u : 2u;    // colour type: RGBA / RGB
        header[10] = header[11] = header[12] = 0; // deflate, adaptive filtering, no interlace
        put_chunk(out, crc_table, "IHDR", header, 13);
        put_chunk(out, crc_table, "IDAT", zlib.data, zlib.size);
        put_chunk(out, crc_table, "IEND", 0, 0);
        result = !out->error;
    }

done:
    free(filtered);
    free(rows);
    free(candidates);
    free(zlib.data);
    return result;
}

} // namespace

int port_image_encode(uint32_t file, const uint32_t *pixels, uint32_t width, uint32_t height, uint32_t pitch,
                      uint32_t channels, uint32_t quality, uint8_t **out_data, uint32_t *out_size)
{
    if (out_data != 0)
        *out_data = 0;
    if (out_size != 0)
        *out_size = 0;
    if (pixels == 0 || out_data == 0 || out_size == 0 || width == 0 || height == 0 || pitch < width ||
        (channels != 3 && channels != 4))
        return 0;
    s_output out = { 0, 0, 0, 0 };
    int ok = 0;
    if (file == k_port_image_file_jpeg)
        ok = encode_jpeg(pixels, width, height, pitch, quality, &out);
    else if (file == k_port_image_file_png)
        ok = encode_png(pixels, width, height, pitch, channels, &out);
    if (!ok || out.error) {
        free(out.data);
        return 0;
    }
    *out_data = out.data;
    *out_size = out.size;
    return 1;
}
