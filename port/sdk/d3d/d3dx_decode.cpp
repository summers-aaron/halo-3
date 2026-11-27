// port/sdk/d3d: JPEG and PNG decoding for the D3DX file calls (d3dx_decode.h). The JPEG decoder follows IJG libjpeg
// 6b (what d3dx9.lib links) wherever the output depends on it: jidctint.c's ISLOW IDCT, jdsample.c's fancy h2v1 / h2v2
// upsampling with replicated edges, jdcolor.c's fixed-point YCbCr tables, jdhuff.c / jdphuff.c's entropy decoding
// (baseline, extended and progressive Huffman, restart markers). Arithmetic coding, lossless and 12-bit JPEG are not
// read (D3DX's libjpeg was built without them either).
#include "d3dx_decode.h"

#include <stdlib.h>
#include <string.h>
#include "zlib/zlib.h"

namespace {

// ---- JPEG ----

const uint8_t k_zigzag[64 + 16] = {
    0, 1, 8, 16, 9, 2, 3, 10, 17, 24, 32, 25, 18, 11, 4, 5,
    12, 19, 26, 33, 40, 48, 41, 34, 27, 20, 13, 6, 7, 14, 21, 28,
    35, 42, 49, 56, 57, 50, 43, 36, 29, 22, 15, 23, 30, 37, 44, 51,
    58, 59, 52, 45, 38, 31, 39, 46, 53, 60, 61, 54, 47, 55, 62, 63,
    63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, // corrupt-data guard (jutils.c)
};

struct s_huffman {
    uint8_t present;
    uint8_t values[256];
    int32_t maxcode[18]; // per code length 1..16: the largest code of that length, -1 none (17: sentinel)
    int32_t valptr[17];  // index into values of the first code of that length, minus that code
};

struct s_component {
    uint32_t id, h, v, tq;
    uint32_t td, ta;        // the current scan's DC / AC table
    uint32_t blocks_w, blocks_h; // the block grid, padded to whole MCUs
    uint32_t width, height; // samples: ceil(image * h / hmax), ceil(image * v / vmax)
    int16_t *coefficients;  // blocks_w * blocks_h blocks of 64, natural order, quantized
    int32_t dc_predictor;
    uint8_t *samples;       // after the IDCT: blocks_w * 8 wide, blocks_h * 8 high
};

struct s_jpeg {
    const uint8_t *data;
    uint32_t size, pos;
    uint16_t quant[4][64]; // natural order
    s_huffman dc[4], ac[4];
    s_component comps[4];
    uint32_t ncomp, width, height, hmax, vmax, mcus_x, mcus_y;
    uint32_t progressive, restart_interval;
    int32_t adobe_transform; // -1: no Adobe APP14 marker
    uint32_t jfif;
    // entropy decoder
    uint32_t bits, bit_count;
    uint32_t marker;        // a marker the bit reader stopped at (0 none)
    uint32_t eob_run;
    int error;
};

uint32_t read_u8(s_jpeg *j)
{
    if (j->pos >= j->size) {
        j->error = 1;
        return 0;
    }
    return j->data[j->pos++];
}

uint32_t read_u16(s_jpeg *j)
{
    const uint32_t hi = read_u8(j);
    return hi << 8 | read_u8(j);
}

bool build_huffman(s_huffman *h, const uint8_t counts[16], const uint8_t *values, uint32_t total)
{
    memset(h, 0, sizeof(*h));
    if (total > 256)
        return false;
    memcpy(h->values, values, total);
    int32_t code = 0;
    uint32_t k = 0;
    for (uint32_t length = 1; length <= 16; length++) {
        h->valptr[length] = (int32_t)k - code;
        if (counts[length - 1] != 0) {
            code += counts[length - 1];
            k += counts[length - 1];
            h->maxcode[length] = code - 1;
        } else {
            h->maxcode[length] = -1;
        }
        if (code > (1 << length))
            return false;
        code <<= 1;
    }
    h->maxcode[17] = 0x7fffffff;
    h->present = 1;
    return true;
}

// The bit reader (jdhuff.c): bytes 0xFF 0x00 are a data 0xFF; any other marker ends the entropy-coded segment, after
// which zeros are read (libjpeg's behaviour on a short segment).
void fill_bits(s_jpeg *j)
{
    while (j->bit_count <= 24) {
        uint32_t byte = 0;
        if (j->marker == 0 && j->pos < j->size) {
            byte = j->data[j->pos];
            if (byte == 0xff) {
                uint32_t next = j->pos + 1 < j->size ? j->data[j->pos + 1] : 0xd9u;
                while (next == 0xff && j->pos + 2 < j->size) { // fill bytes
                    j->pos++;
                    next = j->data[j->pos + 1];
                }
                if (next == 0) {
                    j->pos += 2;
                } else {
                    j->marker = next;
                    byte = 0;
                }
            } else {
                j->pos++;
            }
        }
        j->bits |= byte << (24 - j->bit_count);
        j->bit_count += 8;
    }
}

uint32_t get_bits(s_jpeg *j, uint32_t n)
{
    if (n == 0)
        return 0;
    fill_bits(j);
    const uint32_t value = j->bits >> (32 - n);
    j->bits <<= n;
    j->bit_count -= n;
    return value;
}

uint32_t get_bit(s_jpeg *j)
{
    return get_bits(j, 1);
}

int32_t extend(uint32_t value, uint32_t n)
{
    return n == 0 ? 0 : value < (1u << (n - 1)) ? (int32_t)value - (int32_t)(1u << n) + 1 : (int32_t)value;
}

int32_t decode_huffman(s_jpeg *j, const s_huffman *h)
{
    if (!h->present) {
        j->error = 1;
        return 0;
    }
    int32_t code = (int32_t)get_bit(j);
    uint32_t length = 1;
    while (code > h->maxcode[length]) {
        code = code << 1 | (int32_t)get_bit(j);
        if (++length > 16) {
            j->error = 1; // a code no table has: libjpeg warns and uses 0
            return 0;
        }
    }
    return h->values[(h->valptr[length] + code) & 0xff];
}

void reset_entropy(s_jpeg *j)
{
    j->bits = 0;
    j->bit_count = 0;
    j->marker = 0;
    j->eob_run = 0;
    for (uint32_t c = 0; c < j->ncomp; c++)
        j->comps[c].dc_predictor = 0;
}

// One block of one scan (jdhuff.c decode_mcu, jdphuff.c decode_mcu_DC_first / _AC_first / _DC_refine / _AC_refine).
void decode_block(s_jpeg *j, s_component *c, int16_t *block, uint32_t ss, uint32_t se, uint32_t ah, uint32_t al)
{
    if (!j->progressive) {
        const uint32_t t = (uint32_t)decode_huffman(j, &j->dc[c->td]);
        c->dc_predictor += extend(get_bits(j, t), t);
        block[0] = (int16_t)c->dc_predictor;
        for (uint32_t k = 1; k < 64;) {
            const uint32_t rs = (uint32_t)decode_huffman(j, &j->ac[c->ta]);
            const uint32_t r = rs >> 4, s = rs & 15u;
            if (s == 0) {
                if (r != 15)
                    break;
                k += 16;
                continue;
            }
            k += r;
            if (k > 63)
                break;
            block[k_zigzag[k]] = (int16_t)extend(get_bits(j, s), s);
            k++;
        }
        return;
    }
    if (ss == 0) {
        if (ah == 0) {
            const uint32_t t = (uint32_t)decode_huffman(j, &j->dc[c->td]);
            c->dc_predictor += extend(get_bits(j, t), t);
            block[0] = (int16_t)(c->dc_predictor * (1 << al));
        } else if (get_bit(j)) {
            block[0] = (int16_t)(block[0] | (1 << al));
        }
        return;
    }
    if (ah == 0) {
        if (j->eob_run > 0) {
            j->eob_run--;
            return;
        }
        for (uint32_t k = ss; k <= se;) {
            const uint32_t rs = (uint32_t)decode_huffman(j, &j->ac[c->ta]);
            const uint32_t r = rs >> 4, s = rs & 15u;
            if (s == 0) {
                if (r < 15) {
                    j->eob_run = (1u << r) - 1u;
                    if (r != 0)
                        j->eob_run += get_bits(j, r);
                    break;
                }
                k += 16;
                continue;
            }
            k += r;
            if (k > 63)
                break;
            block[k_zigzag[k]] = (int16_t)(extend(get_bits(j, s), s) * (1 << al));
            k++;
        }
        return;
    }
    // AC refinement
    const int32_t p1 = 1 << al, m1 = -1 * (1 << al);
    uint32_t k = ss;
    if (j->eob_run == 0) {
        for (; k <= se; k++) {
            const uint32_t rs = (uint32_t)decode_huffman(j, &j->ac[c->ta]);
            int32_t r = (int32_t)(rs >> 4);
            int32_t s = (int32_t)(rs & 15u);
            if (s != 0) {
                s = get_bit(j) ? p1 : m1;
            } else if (r != 15) {
                j->eob_run = 1u << r;
                if (r != 0)
                    j->eob_run += get_bits(j, (uint32_t)r);
                break;
            }
            do {
                int16_t *coefficient = block + k_zigzag[k];
                if (*coefficient != 0) {
                    if (get_bit(j) && (*coefficient & p1) == 0)
                        *coefficient = (int16_t)(*coefficient >= 0 ? *coefficient + p1 : *coefficient + m1);
                } else if (--r < 0) {
                    break;
                }
                k++;
            } while (k <= se);
            if (s != 0 && k <= 63)
                block[k_zigzag[k]] = (int16_t)s;
        }
    }
    if (j->eob_run > 0) {
        for (; k <= se; k++) {
            int16_t *coefficient = block + k_zigzag[k];
            if (*coefficient != 0 && get_bit(j) && (*coefficient & p1) == 0)
                *coefficient = (int16_t)(*coefficient >= 0 ? *coefficient + p1 : *coefficient + m1);
        }
        j->eob_run--;
    }
}

// After a restart interval: the RSTn marker, then fresh predictors.
bool read_restart(s_jpeg *j)
{
    j->bits = 0;
    j->bit_count = 0;
    if (j->marker == 0) {
        // the bit reader has not reached the marker yet: skip to it
        while (j->pos + 1 < j->size && !(j->data[j->pos] == 0xff && j->data[j->pos + 1] >= 0xd0 && j->data[j->pos + 1] <= 0xd7))
            j->pos++;
        if (j->pos + 1 >= j->size)
            return false;
        j->marker = j->data[j->pos + 1];
    }
    if (j->marker < 0xd0 || j->marker > 0xd7)
        return false; // a missing restart marker: stop decoding this scan
    j->pos += 2;
    j->marker = 0;
    j->eob_run = 0;
    for (uint32_t c = 0; c < j->ncomp; c++)
        j->comps[c].dc_predictor = 0;
    return true;
}

void decode_scan(s_jpeg *j, s_component **scan, uint32_t ns, uint32_t ss, uint32_t se, uint32_t ah, uint32_t al)
{
    reset_entropy(j);
    uint32_t restarts_left = j->restart_interval;
    if (ns == 1) {
        // non-interleaved: every block the component's own size covers, one MCU each
        s_component *c = scan[0];
        const uint32_t bw = (c->width + 7) / 8, bh = (c->height + 7) / 8;
        for (uint32_t by = 0; by < bh && !j->error; by++) {
            for (uint32_t bx = 0; bx < bw && !j->error; bx++) {
                if (j->restart_interval != 0 && restarts_left-- == 0) {
                    if (!read_restart(j))
                        return;
                    restarts_left = j->restart_interval - 1;
                }
                decode_block(j, c, c->coefficients + ((size_t)by * c->blocks_w + bx) * 64, ss, se, ah, al);
            }
        }
        return;
    }
    for (uint32_t my = 0; my < j->mcus_y && !j->error; my++) {
        for (uint32_t mx = 0; mx < j->mcus_x && !j->error; mx++) {
            if (j->restart_interval != 0 && restarts_left-- == 0) {
                if (!read_restart(j))
                    return;
                restarts_left = j->restart_interval - 1;
            }
            for (uint32_t i = 0; i < ns; i++) {
                s_component *c = scan[i];
                for (uint32_t y = 0; y < c->v; y++)
                    for (uint32_t x = 0; x < c->h; x++) {
                        const uint32_t bx = mx * c->h + x, by = my * c->v + y;
                        decode_block(j, c, c->coefficients + ((size_t)by * c->blocks_w + bx) * 64, ss, se, ah, al);
                    }
            }
        }
    }
}

// jidctint.c jpeg_idct_islow: the dequantized block into 8x8 samples (+128, clamped).
void idct_islow(const int16_t *in, const uint16_t *quant, uint8_t *out, uint32_t stride)
{
    enum { CONST_BITS = 13, PASS1_BITS = 2 };
    const int32_t FIX_0_298631336 = 2446, FIX_0_390180644 = 3196, FIX_0_541196100 = 4433, FIX_0_765366865 = 6270,
                  FIX_0_899976223 = 7373, FIX_1_175875602 = 9633, FIX_1_501321110 = 12299, FIX_1_847759065 = 15137,
                  FIX_1_961570560 = 16069, FIX_2_053119869 = 16819, FIX_2_562915447 = 20995, FIX_3_072711026 = 25172;
#define PORT_DESCALE(x, n) (((x) + ((int32_t)1 << ((n) - 1))) >> (n))
    int32_t dq[64];
    for (int i = 0; i < 64; i++) {
        int32_t v = (int32_t)in[i] * (int32_t)quant[i];
        dq[i] = v < -16384 ? -16384 : v > 16383 ? 16383 : v; // valid 8-bit data stays within +-2^11
    }
    int32_t ws[64];
    for (int col = 0; col < 8; col++) {
        const int32_t *p = dq + col;
        if (p[8] == 0 && p[16] == 0 && p[24] == 0 && p[32] == 0 && p[40] == 0 && p[48] == 0 && p[56] == 0) {
            const int32_t dc = p[0] * (1 << PASS1_BITS);
            for (int r = 0; r < 8; r++)
                ws[r * 8 + col] = dc;
            continue;
        }
        int32_t z2 = p[16], z3 = p[48];
        int32_t z1 = (z2 + z3) * FIX_0_541196100;
        int32_t tmp2 = z1 + z3 * -FIX_1_847759065;
        int32_t tmp3 = z1 + z2 * FIX_0_765366865;
        int32_t tmp0 = (p[0] + p[32]) * (1 << CONST_BITS);
        int32_t tmp1 = (p[0] - p[32]) * (1 << CONST_BITS);
        const int32_t tmp10 = tmp0 + tmp3, tmp13 = tmp0 - tmp3, tmp11 = tmp1 + tmp2, tmp12 = tmp1 - tmp2;
        tmp0 = p[56];
        tmp1 = p[40];
        tmp2 = p[24];
        tmp3 = p[8];
        z1 = tmp0 + tmp3;
        z2 = tmp1 + tmp2;
        z3 = tmp0 + tmp2;
        int32_t z4 = tmp1 + tmp3;
        const int32_t z5 = (z3 + z4) * FIX_1_175875602;
        tmp0 *= FIX_0_298631336;
        tmp1 *= FIX_2_053119869;
        tmp2 *= FIX_3_072711026;
        tmp3 *= FIX_1_501321110;
        z1 *= -FIX_0_899976223;
        z2 *= -FIX_2_562915447;
        z3 *= -FIX_1_961570560;
        z4 *= -FIX_0_390180644;
        z3 += z5;
        z4 += z5;
        tmp0 += z1 + z3;
        tmp1 += z2 + z4;
        tmp2 += z2 + z3;
        tmp3 += z1 + z4;
        ws[0 * 8 + col] = PORT_DESCALE(tmp10 + tmp3, CONST_BITS - PASS1_BITS);
        ws[7 * 8 + col] = PORT_DESCALE(tmp10 - tmp3, CONST_BITS - PASS1_BITS);
        ws[1 * 8 + col] = PORT_DESCALE(tmp11 + tmp2, CONST_BITS - PASS1_BITS);
        ws[6 * 8 + col] = PORT_DESCALE(tmp11 - tmp2, CONST_BITS - PASS1_BITS);
        ws[2 * 8 + col] = PORT_DESCALE(tmp12 + tmp1, CONST_BITS - PASS1_BITS);
        ws[5 * 8 + col] = PORT_DESCALE(tmp12 - tmp1, CONST_BITS - PASS1_BITS);
        ws[3 * 8 + col] = PORT_DESCALE(tmp13 + tmp0, CONST_BITS - PASS1_BITS);
        ws[4 * 8 + col] = PORT_DESCALE(tmp13 - tmp0, CONST_BITS - PASS1_BITS);
    }
    for (int row = 0; row < 8; row++) {
        const int32_t *w = ws + row * 8;
        uint8_t *o = out + (size_t)row * stride;
        int32_t z2 = w[2], z3 = w[6];
        int32_t z1 = (z2 + z3) * FIX_0_541196100;
        int32_t tmp2 = z1 + z3 * -FIX_1_847759065;
        int32_t tmp3 = z1 + z2 * FIX_0_765366865;
        int32_t tmp0 = (w[0] + w[4]) * (1 << CONST_BITS);
        int32_t tmp1 = (w[0] - w[4]) * (1 << CONST_BITS);
        const int32_t tmp10 = tmp0 + tmp3, tmp13 = tmp0 - tmp3, tmp11 = tmp1 + tmp2, tmp12 = tmp1 - tmp2;
        tmp0 = w[7];
        tmp1 = w[5];
        tmp2 = w[3];
        tmp3 = w[1];
        z1 = tmp0 + tmp3;
        z2 = tmp1 + tmp2;
        z3 = tmp0 + tmp2;
        int32_t z4 = tmp1 + tmp3;
        const int32_t z5 = (z3 + z4) * FIX_1_175875602;
        tmp0 *= FIX_0_298631336;
        tmp1 *= FIX_2_053119869;
        tmp2 *= FIX_3_072711026;
        tmp3 *= FIX_1_501321110;
        z1 *= -FIX_0_899976223;
        z2 *= -FIX_2_562915447;
        z3 *= -FIX_1_961570560;
        z4 *= -FIX_0_390180644;
        z3 += z5;
        z4 += z5;
        tmp0 += z1 + z3;
        tmp1 += z2 + z4;
        tmp2 += z2 + z3;
        tmp3 += z1 + z4;
        const int32_t v[8] = {
            tmp10 + tmp3, tmp11 + tmp2, tmp12 + tmp1, tmp13 + tmp0,
            tmp13 - tmp0, tmp12 - tmp1, tmp11 - tmp2, tmp10 - tmp3,
        };
        for (int i = 0; i < 8; i++) {
            const int32_t s = PORT_DESCALE(v[i], CONST_BITS + PASS1_BITS + 3) + 128;
            o[i] = (uint8_t)(s < 0 ? 0 : s > 255 ? 255 : s);
        }
    }
#undef PORT_DESCALE
}

// jdsample.c: one component row group up to the image's full sampling. The full-size plane is
// width x height of the image; the component plane is c->blocks_w * 8 wide.
void upsample(const s_jpeg *j, const s_component *c, uint8_t *out)
{
    const uint32_t stride = c->blocks_w * 8u;
    const uint32_t hf = j->hmax / c->h, vf = j->vmax / c->v;
    const uint32_t cw = c->width, ch = c->height;
    const uint8_t *in = c->samples;
    if (hf == 1 && vf == 1) {
        for (uint32_t y = 0; y < j->height; y++)
            memcpy(out + (size_t)y * j->width, in + (size_t)y * stride, j->width);
        return;
    }
    if (hf == 2 && vf == 1 && cw >= 2) {
        for (uint32_t y = 0; y < j->height; y++) {
            const uint8_t *r = in + (size_t)y * stride;
            uint8_t *o = out + (size_t)y * j->width;
            // h2v1_fancy_upsample: 3/4 nearer + 1/4 further, edges replicated
            for (uint32_t x = 0; x < cw; x++) {
                const int32_t here = r[x] * 3;
                const int32_t left = x == 0 ? r[0] * 4 : here + r[x - 1] + 1;
                const int32_t right = x + 1 == cw ? r[x] * 4 : here + r[x + 1] + 2;
                if (2 * x < j->width)
                    o[2 * x] = (uint8_t)(x == 0 ? r[0] : left >> 2);
                if (2 * x + 1 < j->width)
                    o[2 * x + 1] = (uint8_t)(x + 1 == cw ? r[x] : right >> 2);
            }
        }
        return;
    }
    if (hf == 2 && vf == 2 && cw >= 2) {
        for (uint32_t y = 0; y < j->height; y++) {
            const uint32_t cy = y / 2;
            // the nearer row and the further one (above for even output rows, below for odd), edges replicated
            const uint32_t ny = (y & 1) == 0 ? (cy == 0 ? 0 : cy - 1) : (cy + 1 < ch ? cy + 1 : cy);
            const uint8_t *r0 = in + (size_t)cy * stride, *r1 = in + (size_t)ny * stride;
            uint8_t *o = out + (size_t)y * j->width;
            int32_t this_sum = r0[0] * 3 + r1[0];
            int32_t next_sum = r0[1] * 3 + r1[1];
            if (j->width > 0)
                o[0] = (uint8_t)((this_sum * 4 + 8) >> 4);
            if (j->width > 1)
                o[1] = (uint8_t)((this_sum * 3 + next_sum + 7) >> 4);
            int32_t last_sum = this_sum;
            this_sum = next_sum;
            for (uint32_t x = 1; x + 1 < cw; x++) {
                next_sum = r0[x + 1] * 3 + r1[x + 1];
                if (2 * x < j->width)
                    o[2 * x] = (uint8_t)((this_sum * 3 + last_sum + 8) >> 4);
                if (2 * x + 1 < j->width)
                    o[2 * x + 1] = (uint8_t)((this_sum * 3 + next_sum + 7) >> 4);
                last_sum = this_sum;
                this_sum = next_sum;
            }
            const uint32_t x = cw - 1;
            if (2 * x < j->width)
                o[2 * x] = (uint8_t)((this_sum * 3 + last_sum + 8) >> 4);
            if (2 * x + 1 < j->width)
                o[2 * x + 1] = (uint8_t)((this_sum * 4 + 7) >> 4);
        }
        return;
    }
    // any other factor: replication (int_upsample)
    for (uint32_t y = 0; y < j->height; y++) {
        const uint8_t *r = in + (size_t)(y / vf) * stride;
        uint8_t *o = out + (size_t)y * j->width;
        for (uint32_t x = 0; x < j->width; x++)
            o[x] = r[x / hf];
    }
}

uint8_t clamp_byte(int32_t v)
{
    return (uint8_t)(v < 0 ? 0 : v > 255 ? 255 : v);
}

bool jpeg_parse(s_jpeg *j, bool header_only)
{
    if (j->size < 4 || j->data[0] != 0xff || j->data[1] != 0xd8)
        return false;
    j->pos = 2;
    j->adobe_transform = -1;
    bool frame = false;
    for (;;) {
        // next marker (skipping fill bytes)
        uint32_t byte = read_u8(j);
        while (!j->error && byte != 0xff)
            byte = read_u8(j);
        uint32_t marker = read_u8(j);
        while (!j->error && marker == 0xff)
            marker = read_u8(j);
        if (j->error)
            return frame && !header_only ? true : false;
        if (marker == 0xd9) // EOI
            return frame;
        if (marker >= 0xd0 && marker <= 0xd7) // a stray RSTn
            continue;
        const uint32_t length = read_u16(j);
        if (j->error || length < 2 || j->pos + length - 2 > j->size)
            return false;
        const uint32_t end = j->pos + length - 2;
        switch (marker) {
        case 0xc0: case 0xc1: case 0xc2: {
            if (frame)
                return false;
            j->progressive = marker == 0xc2;
            if (read_u8(j) != 8)
                return false; // 12-bit
            j->height = read_u16(j);
            j->width = read_u16(j);
            j->ncomp = read_u8(j);
            if (j->width == 0 || j->height == 0 || (j->ncomp != 1 && j->ncomp != 3 && j->ncomp != 4) ||
                j->width > 16384 || j->height > 16384)
                return false;
            j->hmax = j->vmax = 1;
            for (uint32_t i = 0; i < j->ncomp; i++) {
                s_component &c = j->comps[i];
                c.id = read_u8(j);
                const uint32_t hv = read_u8(j);
                c.h = hv >> 4;
                c.v = hv & 15u;
                c.tq = read_u8(j);
                if (c.h == 0 || c.h > 4 || c.v == 0 || c.v > 4 || c.tq > 3)
                    return false;
                j->hmax = c.h > j->hmax ? c.h : j->hmax;
                j->vmax = c.v > j->vmax ? c.v : j->vmax;
            }
            frame = true;
            if (header_only)
                return !j->error;
            j->mcus_x = (j->width + 8 * j->hmax - 1) / (8 * j->hmax);
            j->mcus_y = (j->height + 8 * j->vmax - 1) / (8 * j->vmax);
            for (uint32_t i = 0; i < j->ncomp; i++) {
                s_component &c = j->comps[i];
                if (j->hmax % c.h != 0 || j->vmax % c.v != 0)
                    return false;
                c.blocks_w = j->mcus_x * c.h;
                c.blocks_h = j->mcus_y * c.v;
                c.width = (j->width * c.h + j->hmax - 1) / j->hmax;
                c.height = (j->height * c.v + j->vmax - 1) / j->vmax;
                c.coefficients = (int16_t *)calloc((size_t)c.blocks_w * c.blocks_h, 64 * sizeof(int16_t));
                if (c.coefficients == 0)
                    return false;
            }
            break;
        }
        case 0xc3: case 0xc5: case 0xc6: case 0xc7: case 0xc9: case 0xca: case 0xcb: case 0xcd: case 0xce: case 0xcf:
            return false; // lossless, hierarchical or arithmetic coded
        case 0xc4: // DHT
            while (j->pos < end && !j->error) {
                const uint32_t tc_th = read_u8(j);
                uint8_t counts[16];
                uint32_t total = 0;
                for (int i = 0; i < 16; i++) {
                    counts[i] = (uint8_t)read_u8(j);
                    total += counts[i];
                }
                if ((tc_th & 15u) > 3 || (tc_th >> 4) > 1 || total > 256 || j->pos + total > end)
                    return false;
                s_huffman *h = (tc_th >> 4) == 0 ? &j->dc[tc_th & 3u] : &j->ac[tc_th & 3u];
                if (!build_huffman(h, counts, j->data + j->pos, total))
                    return false;
                j->pos += total;
            }
            break;
        case 0xdb: // DQT
            while (j->pos < end && !j->error) {
                const uint32_t pq_tq = read_u8(j);
                if ((pq_tq & 15u) > 3)
                    return false;
                for (int i = 0; i < 64; i++)
                    j->quant[pq_tq & 3u][k_zigzag[i]] = (uint16_t)((pq_tq >> 4) != 0 ? read_u16(j) : read_u8(j));
            }
            break;
        case 0xdd: // DRI
            j->restart_interval = read_u16(j);
            break;
        case 0xe0: // APP0 JFIF
            if (length >= 7 && memcmp(j->data + j->pos, "JFIF\0", 5) == 0)
                j->jfif = 1;
            break;
        case 0xee: // APP14 Adobe
            if (length >= 14 && memcmp(j->data + j->pos, "Adobe", 5) == 0)
                j->adobe_transform = j->data[j->pos + 11];
            break;
        case 0xda: { // SOS
            if (!frame || header_only)
                return false;
            const uint32_t ns = read_u8(j);
            if (ns == 0 || ns > 4 || ns > j->ncomp)
                return false;
            s_component *scan[4] = {};
            for (uint32_t i = 0; i < ns; i++) {
                const uint32_t id = read_u8(j), tables = read_u8(j);
                for (uint32_t c = 0; c < j->ncomp; c++)
                    if (j->comps[c].id == id)
                        scan[i] = &j->comps[c];
                if (scan[i] == 0)
                    return false;
                scan[i]->td = (tables >> 4) & 3u;
                scan[i]->ta = tables & 3u;
            }
            const uint32_t ss = read_u8(j), se = read_u8(j), a = read_u8(j);
            const uint32_t ah = a >> 4, al = a & 15u;
            if (j->error || j->pos != end || ss > 63 || se > 63 || ss > se || al > 13)
                return false;
            if (!j->progressive && (ss != 0 || se != 63))
                return false;
            decode_scan(j, scan, ns, ss, j->progressive ? se : 63u, ah, al);
            if (j->error)
                return false;
            // continue at the marker that ended the entropy-coded segment
            if (j->marker != 0) {
                while (j->pos + 1 < j->size && !(j->data[j->pos] == 0xff && j->data[j->pos + 1] == j->marker))
                    j->pos++;
            }
            j->marker = 0;
            continue; // the segment had no length to skip
        }
        default:
            break;
        }
        if (j->error)
            return false;
        j->pos = end;
    }
}

void jpeg_free(s_jpeg *j)
{
    for (uint32_t c = 0; c < 4; c++) {
        free(j->comps[c].coefficients);
        free(j->comps[c].samples);
        j->comps[c].coefficients = 0;
        j->comps[c].samples = 0;
    }
}

bool jpeg_decode(const uint8_t *data, uint32_t size, s_port_image_info *info, uint32_t **out_pixels)
{
    s_jpeg *j = (s_jpeg *)calloc(1, sizeof(s_jpeg));
    if (j == 0)
        return false;
    j->data = data;
    j->size = size;
    bool ok = jpeg_parse(j, false);
    uint8_t *planes[4] = {};
    uint32_t *pixels = 0;
    if (ok) {
        // IDCT every block, then each component up to full size
        for (uint32_t c = 0; c < j->ncomp && ok; c++) {
            s_component &comp = j->comps[c];
            const uint32_t stride = comp.blocks_w * 8u;
            comp.samples = (uint8_t *)malloc((size_t)stride * comp.blocks_h * 8u);
            planes[c] = (uint8_t *)malloc((size_t)j->width * j->height);
            if (comp.samples == 0 || planes[c] == 0) {
                ok = false;
                break;
            }
            for (uint32_t by = 0; by < comp.blocks_h; by++)
                for (uint32_t bx = 0; bx < comp.blocks_w; bx++)
                    idct_islow(comp.coefficients + ((size_t)by * comp.blocks_w + bx) * 64, j->quant[comp.tq],
                               comp.samples + (size_t)by * 8u * stride + bx * 8u, stride);
            upsample(j, &comp, planes[c]);
        }
        pixels = ok ? (uint32_t *)malloc((size_t)j->width * j->height * 4u) : 0;
        ok = pixels != 0;
    }
    if (ok) {
        // jdcolor.c: YCbCr unless an Adobe marker says RGB (transform 0) or the component ids are 'R' 'G' 'B'
        const bool rgb = j->ncomp == 3 && (j->adobe_transform == 0 ||
                                           (j->adobe_transform < 0 && !j->jfif && j->comps[0].id == 'R' &&
                                            j->comps[1].id == 'G' && j->comps[2].id == 'B'));
        const bool ycck = j->ncomp == 4 && j->adobe_transform == 2;
        const int32_t k_one_half = 1 << 15;
        const size_t count = (size_t)j->width * j->height;
        for (size_t i = 0; i < count; i++) {
            uint32_t r, g, b;
            if (j->ncomp == 1) {
                r = g = b = planes[0][i];
            } else if (rgb) {
                r = planes[0][i];
                g = planes[1][i];
                b = planes[2][i];
            } else {
                const int32_t y = planes[0][i], cb = (int32_t)planes[1][i] - 128, cr = (int32_t)planes[2][i] - 128;
                int32_t rr = y + ((91881 * cr + k_one_half) >> 16);
                int32_t gg = y + ((-22554 * cb + k_one_half - 46802 * cr) >> 16);
                int32_t bb = y + ((116130 * cb + k_one_half) >> 16);
                if (j->ncomp == 4) {
                    // Adobe CMYK / YCCK, stored inverted: colour = C * K / 255
                    const int32_t k = planes[3][i];
                    if (!ycck) {
                        rr = planes[0][i];
                        gg = planes[1][i];
                        bb = planes[2][i];
                    } else {
                        rr = 255 - clamp_byte(rr);
                        gg = 255 - clamp_byte(gg);
                        bb = 255 - clamp_byte(bb);
                    }
                    rr = (rr * k + 127) / 255;
                    gg = (gg * k + 127) / 255;
                    bb = (bb * k + 127) / 255;
                }
                r = clamp_byte(rr);
                g = clamp_byte(gg);
                b = clamp_byte(bb);
            }
            pixels[i] = 0xff000000u | r << 16 | g << 8 | b;
        }
        info->file = k_port_image_file_jpeg;
        info->width = j->width;
        info->height = j->height;
        info->channels = j->ncomp == 1 ? 1u : 3u;
        *out_pixels = pixels;
    } else {
        free(pixels);
    }
    for (int c = 0; c < 4; c++)
        free(planes[c]);
    jpeg_free(j);
    free(j);
    return ok;
}

// ---- PNG ----

uint32_t be32(const uint8_t *p)
{
    return (uint32_t)p[0] << 24 | (uint32_t)p[1] << 16 | (uint32_t)p[2] << 8 | p[3];
}

const uint8_t k_png_signature[8] = { 137, 80, 78, 71, 13, 10, 26, 10 };

struct s_png {
    uint32_t width, height, depth, color, interlace;
    uint8_t palette[256][4];
    uint32_t palette_count;
    uint32_t has_key;       // tRNS for grey / truecolour: the transparent sample values
    uint32_t key[3];
    uint32_t has_alpha;
};

uint32_t png_channels(uint32_t color)
{
    switch (color) {
    case 0: return 1;
    case 2: return 3;
    case 3: return 1;
    case 4: return 2;
    case 6: return 4;
    default: return 0;
    }
}

bool png_header(const uint8_t *data, uint32_t size, s_png *png)
{
    if (size < 33 || memcmp(data, k_png_signature, 8) != 0 || be32(data + 8) != 13 || memcmp(data + 12, "IHDR", 4) != 0)
        return false;
    const uint8_t *h = data + 16;
    png->width = be32(h);
    png->height = be32(h + 4);
    png->depth = h[8];
    png->color = h[9];
    png->interlace = h[12];
    const uint32_t channels = png_channels(png->color);
    if (png->width == 0 || png->height == 0 || png->width > 16384 || png->height > 16384 || channels == 0 ||
        h[10] != 0 || h[11] != 0 || png->interlace > 1)
        return false;
    const uint32_t d = png->depth;
    const bool depth_ok = png->color == 0 ? (d == 1 || d == 2 || d == 4 || d == 8 || d == 16)
                          : png->color == 3 ? (d == 1 || d == 2 || d == 4 || d == 8)
                                            : (d == 8 || d == 16);
    return depth_ok;
}

uint8_t paeth(int32_t a, int32_t b, int32_t c)
{
    const int32_t p = a + b - c;
    const int32_t pa = p > a ? p - a : a - p, pb = p > b ? p - b : b - p, pc = p > c ? p - c : c - p;
    return (uint8_t)(pa <= pb && pa <= pc ? a : pb <= pc ? b : c);
}

// Unfilters one (sub)image of rows x row_bytes in place (each row led by its filter byte). bpp: bytes per pixel,
// at least 1.
bool png_unfilter(uint8_t *raw, uint32_t rows, uint32_t row_bytes, uint32_t bpp)
{
    const uint8_t *previous = 0;
    for (uint32_t y = 0; y < rows; y++) {
        uint8_t *row = raw + (size_t)y * (row_bytes + 1);
        const uint32_t filter = row[0];
        uint8_t *p = row + 1;
        for (uint32_t x = 0; x < row_bytes; x++) {
            const int32_t a = x >= bpp ? p[x - bpp] : 0;
            const int32_t b = previous != 0 ? previous[x] : 0;
            const int32_t c = previous != 0 && x >= bpp ? previous[x - bpp] : 0;
            switch (filter) {
            case 0: break;
            case 1: p[x] = (uint8_t)(p[x] + a); break;
            case 2: p[x] = (uint8_t)(p[x] + b); break;
            case 3: p[x] = (uint8_t)(p[x] + ((a + b) >> 1)); break;
            case 4: p[x] = (uint8_t)(p[x] + paeth(a, b, c)); break;
            default: return false;
            }
        }
        previous = p;
    }
    return true;
}

// The sample of channel `channel` of pixel x in an unfiltered row, scaled to 8 bits (16-bit: the high byte; low
// depths: replicated), and its raw value (for the tRNS key).
uint32_t png_sample(const s_png *png, const uint8_t *row, uint32_t x, uint32_t channel, uint32_t *raw)
{
    const uint32_t channels = png_channels(png->color);
    const uint32_t d = png->depth;
    if (d == 16) {
        const uint8_t *p = row + ((size_t)x * channels + channel) * 2;
        *raw = (uint32_t)p[0] << 8 | p[1];
        return p[0];
    }
    if (d == 8) {
        *raw = row[(size_t)x * channels + channel];
        return *raw;
    }
    const uint32_t bit = x * d;
    const uint32_t v = (row[bit / 8] >> (8 - d - (bit % 8))) & ((1u << d) - 1u);
    *raw = v;
    if (png->color == 3)
        return v;
    return d == 1 ? v * 255u : d == 2 ? v * 85u : v * 17u;
}

void png_store(const s_png *png, const uint8_t *row, uint32_t width, uint32_t *out, uint32_t out_step)
{
    for (uint32_t x = 0; x < width; x++) {
        uint32_t raw[4] = {}, s[4] = {};
        const uint32_t channels = png_channels(png->color);
        for (uint32_t c = 0; c < channels; c++)
            s[c] = png_sample(png, row, x, c, &raw[c]);
        uint32_t r, g, b, a = 255;
        switch (png->color) {
        case 0:
            r = g = b = s[0];
            if (png->has_key && raw[0] == png->key[0])
                a = 0;
            break;
        case 2:
            r = s[0]; g = s[1]; b = s[2];
            if (png->has_key && raw[0] == png->key[0] && raw[1] == png->key[1] && raw[2] == png->key[2])
                a = 0;
            break;
        case 3: {
            const uint8_t *e = raw[0] < png->palette_count ? png->palette[raw[0]] : png->palette[0];
            r = e[0]; g = e[1]; b = e[2]; a = e[3];
            break;
        }
        case 4:
            r = g = b = s[0];
            a = s[1];
            break;
        default:
            r = s[0]; g = s[1]; b = s[2]; a = s[3];
            break;
        }
        out[(size_t)x * out_step] = a << 24 | r << 16 | g << 8 | b;
    }
}

bool png_inflate(const uint8_t *in, uint32_t in_size, uint8_t *out, uint32_t out_size)
{
    z_stream stream;
    memset(&stream, 0, sizeof(stream));
    if (z_inflateInit2_(&stream, MAX_WBITS, ZLIB_VERSION, (int)sizeof(z_stream)) != Z_OK)
        return false;
    stream.next_in = (Bytef *)in;
    stream.avail_in = in_size;
    stream.next_out = out;
    stream.avail_out = out_size;
    int result = Z_OK;
    while (result == Z_OK && stream.avail_out != 0)
        result = z_inflate(&stream, Z_FINISH);
    const bool ok = stream.avail_out == 0 && (result == Z_OK || result == Z_STREAM_END || result == Z_BUF_ERROR);
    z_inflateEnd(&stream);
    return ok;
}

bool png_decode(const uint8_t *data, uint32_t size, s_port_image_info *info, uint32_t **out_pixels)
{
    s_png png;
    memset(&png, 0, sizeof(png));
    if (!png_header(data, size, &png))
        return false;
    for (int i = 0; i < 256; i++)
        png.palette[i][3] = 255;
    // gather the IDAT stream, the palette and the transparency
    uint8_t *compressed = (uint8_t *)malloc(size);
    if (compressed == 0)
        return false;
    uint32_t compressed_size = 0;
    bool ended = false;
    for (uint32_t pos = 8; pos + 12 <= size && !ended;) {
        const uint32_t length = be32(data + pos);
        const uint8_t *type = data + pos + 4, *body = data + pos + 8;
        if (length > size - pos - 12)
            break;
        if (memcmp(type, "PLTE", 4) == 0) {
            png.palette_count = length / 3 > 256 ? 256 : length / 3;
            for (uint32_t i = 0; i < png.palette_count; i++) {
                png.palette[i][0] = body[i * 3];
                png.palette[i][1] = body[i * 3 + 1];
                png.palette[i][2] = body[i * 3 + 2];
            }
        } else if (memcmp(type, "tRNS", 4) == 0) {
            png.has_alpha = 1;
            if (png.color == 3) {
                for (uint32_t i = 0; i < length && i < 256; i++)
                    png.palette[i][3] = body[i];
            } else if (png.color == 0 && length >= 2) {
                png.has_key = 1;
                png.key[0] = (uint32_t)body[0] << 8 | body[1];
            } else if (png.color == 2 && length >= 6) {
                png.has_key = 1;
                for (int c = 0; c < 3; c++)
                    png.key[c] = (uint32_t)body[c * 2] << 8 | body[c * 2 + 1];
            }
        } else if (memcmp(type, "IDAT", 4) == 0) {
            memcpy(compressed + compressed_size, body, length);
            compressed_size += length;
        } else if (memcmp(type, "IEND", 4) == 0) {
            ended = true;
        }
        pos += 12 + length;
    }
    if (png.color == 4 || png.color == 6)
        png.has_alpha = 1;
    const uint32_t channels = png_channels(png.color);
    const uint32_t bits_per_pixel = channels * png.depth;
    const uint32_t bpp = bits_per_pixel >= 8 ? bits_per_pixel / 8 : 1u;
    // the passes (one, or Adam7's seven): origin and step
    static const uint32_t k_adam7[7][4] = {
        { 0, 0, 8, 8 }, { 4, 0, 8, 8 }, { 0, 4, 4, 8 }, { 2, 0, 4, 4 }, { 0, 2, 2, 4 }, { 1, 0, 2, 2 }, { 0, 1, 1, 2 },
    };
    const uint32_t passes = png.interlace ? 7u : 1u;
    uint32_t pass_w[7], pass_h[7];
    size_t raw_size = 0;
    for (uint32_t p = 0; p < passes; p++) {
        const uint32_t *a = png.interlace ? k_adam7[p] : 0;
        pass_w[p] = a != 0 ? (png.width > a[0] ? (png.width - a[0] + a[2] - 1) / a[2] : 0) : png.width;
        pass_h[p] = a != 0 ? (png.height > a[1] ? (png.height - a[1] + a[3] - 1) / a[3] : 0) : png.height;
        if (pass_w[p] != 0 && pass_h[p] != 0)
            raw_size += (size_t)pass_h[p] * (((size_t)pass_w[p] * bits_per_pixel + 7) / 8 + 1);
    }
    uint8_t *raw = raw_size <= 0x7fffffffu ? (uint8_t *)malloc(raw_size) : 0;
    uint32_t *pixels = (uint32_t *)malloc((size_t)png.width * png.height * 4u);
    bool ok = raw != 0 && pixels != 0 && compressed_size != 0 &&
              png_inflate(compressed, compressed_size, raw, (uint32_t)raw_size);
    size_t offset = 0;
    for (uint32_t p = 0; p < passes && ok; p++) {
        if (pass_w[p] == 0 || pass_h[p] == 0)
            continue;
        const uint32_t row_bytes = (uint32_t)(((size_t)pass_w[p] * bits_per_pixel + 7) / 8);
        uint8_t *sub = raw + offset;
        ok = png_unfilter(sub, pass_h[p], row_bytes, bpp);
        const uint32_t *a = png.interlace ? k_adam7[p] : 0;
        for (uint32_t y = 0; y < pass_h[p] && ok; y++) {
            const uint8_t *row = sub + (size_t)y * (row_bytes + 1) + 1;
            const uint32_t oy = a != 0 ? a[1] + y * a[3] : y, ox = a != 0 ? a[0] : 0, step = a != 0 ? a[2] : 1;
            png_store(&png, row, pass_w[p], pixels + (size_t)oy * png.width + ox, step);
        }
        offset += (size_t)pass_h[p] * (row_bytes + 1);
    }
    free(compressed);
    free(raw);
    if (!ok) {
        free(pixels);
        return false;
    }
    info->file = k_port_image_file_png;
    info->width = png.width;
    info->height = png.height;
    info->channels = png.has_alpha ? 4u : (png.color == 0 ? 1u : 3u);
    *out_pixels = pixels;
    return true;
}

} // namespace

int port_image_get_info(const void *data, uint32_t size, s_port_image_info *out)
{
    const uint8_t *bytes = (const uint8_t *)data;
    if (data == 0 || out == 0)
        return 0;
    memset(out, 0, sizeof(*out));
    if (size >= 8 && memcmp(bytes, k_png_signature, 8) == 0) {
        s_png png;
        memset(&png, 0, sizeof(png));
        if (!png_header(bytes, size, &png))
            return 0;
        out->file = k_port_image_file_png;
        out->width = png.width;
        out->height = png.height;
        out->channels = png.color == 4 || png.color == 6 ? 4u : png.color == 0 ? 1u : 3u;
        // a tRNS chunk makes it 4 as well (before IDAT, by the PNG rules)
        for (uint32_t pos = 8; pos + 12 <= size;) {
            const uint32_t length = be32(bytes + pos);
            if (length > size - pos - 12 || memcmp(bytes + pos + 4, "IDAT", 4) == 0)
                break;
            if (memcmp(bytes + pos + 4, "tRNS", 4) == 0)
                out->channels = 4;
            pos += 12 + length;
        }
        return 1;
    }
    s_jpeg *j = (s_jpeg *)calloc(1, sizeof(s_jpeg));
    if (j == 0)
        return 0;
    j->data = bytes;
    j->size = size;
    const bool ok = jpeg_parse(j, true);
    if (ok) {
        out->file = k_port_image_file_jpeg;
        out->width = j->width;
        out->height = j->height;
        out->channels = j->ncomp == 1 ? 1u : 3u;
    }
    jpeg_free(j);
    free(j);
    return ok ? 1 : 0;
}

int port_image_decode(const void *data, uint32_t size, s_port_image_info *out_info, uint32_t **out_pixels)
{
    if (data == 0 || out_info == 0 || out_pixels == 0)
        return 0;
    *out_pixels = 0;
    memset(out_info, 0, sizeof(*out_info));
    const uint8_t *bytes = (const uint8_t *)data;
    if (size >= 8 && memcmp(bytes, k_png_signature, 8) == 0)
        return png_decode(bytes, size, out_info, out_pixels) ? 1 : 0;
    return jpeg_decode(bytes, size, out_info, out_pixels) ? 1 : 0;
}
