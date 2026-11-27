// port/sdk/audio (both hosts): XMA decoder (interface and model: xma_decoder.h).
//
// The frame decode is a C port of FFmpeg 4.4 libavcodec/wmaprodec.c as its XMAFRAMES path uses it (decode flags
// 0x10d6, 16-bit scale, 512 samples a frame, 1-2 channels), with the parts of wma.c (run/level decode, large
// values), mdct_template.c (imdct_half), fft and float_dsp (the sine-window overlap) it needs, rewritten without
// libavcodec's infrastructure: a plain MSB-first bit reader, two-level Huffman lookup tables built from the same
// code/length tables (wmapro_tables.h, verbatim), a radix-2 complex FFT, and table-driven math so neither libm nor
// an allocator is linked. Decisions and their order follow wmaprodec.c function by function (names kept), so it
// can be checked against it line by line.
//
// FFmpeg is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either version 2.1 of the License, or (at your option) any
// later version. The original copyright lines of wmaprodec.c:
//   Copyright (c) 2007 Baptiste Coudurier, Benjamin Larsson, Ulion
//   Copyright (c) 2008 - 2009 Sascha Sommer, Benjamin Larsson
// This file is distributed under the same license (LGPL 2.1 or later).
#include "xma_decoder.h"
#include "wmapro_tables.h"

#include <stdint.h>
// No C library headers (the Windows runtime builds without them): the compiler's own memset / memcpy.
#define memset __builtin_memset
#define memcpy __builtin_memcpy

#define MAX_SUBFRAMES 32
#define MAX_BANDS 29
#define XMA_CHANNELS 2
#define BLOCK_MIN_BITS 6
#define BLOCK_SIZES 8 // WMAPRO_BLOCK_MAX_BITS (13) - WMAPRO_BLOCK_MIN_BITS + 1
#define SAMPLES_PER_FRAME 512
#define LOG2_FRAME_SIZE 15 // av_log2(block_align 2048) + 4
#define DECODE_FLAGS 0x10d6
#define BITS_PER_SAMPLE 16
#define CHANNEL_OUT_SIZE 1024 // out[]: the overlap half (256), the frame (512), and room for a full block past it

// ---- math without libm --------------------------------------------------------------------------------------------

static const double k_pi = 3.14159265358979323846;

// sin and cos of x in [-pi, pi] (callers reduce): Taylor series to 1e-16, only used to build tables.
static double xma_sin(double x)
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

static double xma_cos(double x) { return xma_sin(x + k_pi / 2.0); }

// 10^(e / 20) for the dequantizer: exact decade powers times a table of 20ths, as ff_exp10(exp / 20.0).
static float g_db_steps[20];
static float db_to_gain(int e)
{
    int decade = e >= 0 ? e / 20 : -((-e + 19) / 20);
    int step = e - decade * 20;
    double gain = g_db_steps[step];
    if (decade >= 0) {
        for (int i = 0; i < decade && i < 60; i++)
            gain *= 10.0;
    } else {
        for (int i = 0; i < -decade && i < 60; i++)
            gain *= 0.1;
    }
    return (float)gain;
}

static float int_bits_to_float(uint32_t bits)
{
    float f;
    memcpy(&f, &bits, 4);
    return f;
}

static uint32_t float_to_int_bits(float f)
{
    uint32_t bits;
    memcpy(&bits, &f, 4);
    return bits;
}

static int av_log2(unsigned v)
{
    int n = 0;
    while (v > 1) {
        v >>= 1;
        n++;
    }
    return n;
}

// ---- bit reader (FFmpeg GetBitContext semantics: MSB first, reads past the end see the zero padding) --------------

typedef struct bit_reader {
    const uint8_t *buffer;
    int size_in_bits;
    int index;
    int buffer_bytes; // readable bytes (size plus padding)
} bit_reader;

static void init_get_bits(bit_reader *b, const uint8_t *buffer, int bits, int readable_bytes)
{
    b->buffer = buffer;
    b->size_in_bits = bits;
    b->index = 0;
    b->buffer_bytes = readable_bytes;
}

static uint32_t show_bits32(const bit_reader *b)
{
    int byte = b->index >> 3;
    uint64_t window = 0;
    if (byte >= 0 && byte + 5 <= b->buffer_bytes) {
        // all five bytes readable (every read but those at the very end of the buffer): the same window from one
        // big-endian word and the fifth byte, instead of five bounds-checked byte loads per read
        uint32_t word;
        memcpy(&word, b->buffer + byte, 4);
        window = (uint64_t)__builtin_bswap32(word) << 8 | b->buffer[byte + 4];
        return (uint32_t)(window >> (8 - (b->index & 7)));
    }
    for (int i = 0; i < 5; i++) {
        int at = byte + i;
        window = (window << 8) | (at >= 0 && at < b->buffer_bytes ? b->buffer[at] : 0);
    }
    return (uint32_t)(window >> (8 - (b->index & 7)));
}

static unsigned show_bits(const bit_reader *b, int n) { return n == 0 ? 0 : show_bits32(b) >> (32 - n); }
static void skip_bits(bit_reader *b, int n) { b->index += n; }
static unsigned get_bits(bit_reader *b, int n)
{
    unsigned v = show_bits(b, n);
    b->index += n;
    return v;
}
static unsigned get_bits1(bit_reader *b) { return get_bits(b, 1); }
static unsigned get_bitsz(bit_reader *b, int n) { return n ? get_bits(b, n) : 0; }
static int get_sbits(bit_reader *b, int n)
{
    int v = (int)get_bits(b, n);
    return (v ^ (1 << (n - 1))) - (1 << (n - 1));
}
static int get_bits_count(const bit_reader *b) { return b->index; }
static int get_bits_left(const bit_reader *b) { return b->size_in_bits - b->index; }

// ---- Huffman tables -----------------------------------------------------------------------------------------------
//
// INIT_VLC_STATIC's tables, rebuilt: every code of up to VLC_FAST_BITS bits resolves through one lookup of the
// next VLC_FAST_BITS bits; longer codes (all rarer) through a per-length sorted list. get_vlc returns -1 for a
// pattern that is no code (FFmpeg's tables also return -1 there).

#define VLC_FAST_BITS 10
#define VLC_MAX_SYMBOLS 272
#define VLC_MAX_LENGTH 32

typedef struct vlc_table {
    uint16_t fast_symbol[1 << VLC_FAST_BITS];
    uint8_t fast_length[1 << VLC_FAST_BITS];
    int max_length;
    uint16_t first[VLC_MAX_LENGTH + 2]; // index into long_codes of the first code of each length
    uint32_t long_codes[VLC_MAX_SYMBOLS];
    uint16_t long_symbols[VLC_MAX_SYMBOLS];
} vlc_table;

static void vlc_build(vlc_table *t, int count, const uint8_t *lengths, const void *codes, int code_size)
{
    memset(t, 0, sizeof(*t));
    int index = 0;
    for (int length = 1; length <= VLC_MAX_LENGTH; length++) {
        t->first[length] = (uint16_t)index;
        // codes of this length in ascending order (insertion sort: the tables are small)
        for (int s = 0; s < count; s++) {
            if (lengths[s] != length)
                continue;
            uint32_t code = code_size == 2 ? ((const uint16_t *)codes)[s] : ((const uint32_t *)codes)[s];
            if (length > t->max_length)
                t->max_length = length;
            if (length <= VLC_FAST_BITS) {
                uint32_t base = code << (VLC_FAST_BITS - length);
                for (uint32_t fill = 0; fill < (1u << (VLC_FAST_BITS - length)); fill++) {
                    t->fast_symbol[base + fill] = (uint16_t)s;
                    t->fast_length[base + fill] = (uint8_t)length;
                }
            } else {
                int at = index++;
                while (at > t->first[length] && t->long_codes[at - 1] > code) {
                    t->long_codes[at] = t->long_codes[at - 1];
                    t->long_symbols[at] = t->long_symbols[at - 1];
                    at--;
                }
                t->long_codes[at] = code;
                t->long_symbols[at] = (uint16_t)s;
            }
        }
    }
    t->first[VLC_MAX_LENGTH + 1] = (uint16_t)index;
}

static int get_vlc(bit_reader *b, const vlc_table *t)
{
    uint32_t bits = show_bits32(b);
    uint32_t fast = bits >> (32 - VLC_FAST_BITS);
    if (t->fast_length[fast]) {
        b->index += t->fast_length[fast];
        return t->fast_symbol[fast];
    }
    for (int length = VLC_FAST_BITS + 1; length <= t->max_length; length++) {
        uint32_t code = bits >> (32 - length);
        int lo = t->first[length], hi = t->first[length + 1] - 1;
        while (lo <= hi) {
            int mid = (lo + hi) >> 1;
            if (t->long_codes[mid] == code) {
                b->index += length;
                return t->long_symbols[mid];
            }
            if (t->long_codes[mid] < code)
                lo = mid + 1;
            else
                hi = mid - 1;
        }
    }
    return -1;
}

// ---- shared tables (built once) -----------------------------------------------------------------------------------

#define MDCT_MIN_BITS 7 // WMAPRO_BLOCK_MIN_BITS + 1: the MDCT of a 64-sample block
#define MDCT_SIZES 4    // XMA blocks are 64 (never: 4 subframes at most) .. 512 samples; 7..10 bits

typedef struct mdct_table {
    int bits;
    float tcos[1024 / 4];
    float tsin[1024 / 4];
    float twiddle_re[1024 / 8]; // FFT of n/4 points: exp(+2 pi i k / (n/4)), k < n/8
    float twiddle_im[1024 / 8];
    uint16_t bit_reverse[1024 / 4];
} mdct_table;

static vlc_table g_sf_vlc, g_sf_rl_vlc, g_vec4_vlc, g_vec2_vlc, g_vec1_vlc, g_coef_vlc[2];
static float g_sin64[33];
static float g_windows_storage[64 + 128 + 256 + 512 + 1024];
static const float *g_windows[BLOCK_SIZES]; // sine windows of 2^(i + 6) points
static mdct_table g_mdct[MDCT_SIZES];
static volatile int g_tables_state; // 0 none, 1 building, 2 ready

#ifdef XMA_DECODER_TEST_COSFAST
// Test only: the parabolic cosine approximation the xenia / rexglue FFmpeg fork builds its MDCT tables with
// (mdct_template.c cosfast, ~1e-3 error), so the A/B test against that fork can compare everything else exactly.
// Builds without it use the exact cosine, as upstream FFmpeg does.
static double floor_d(double x) { long long i = (long long)x; return (double)i > x ? (double)(i - 1) : (double)i; }
static double fabs_d(double x) { return x < 0 ? -x : x; }
static double table_cos(double x)
{
    x *= 1.0 / (2.0 * k_pi);
    x -= 0.25 + floor_d(x + 0.25);
    x *= 16.0 * (fabs_d(x) - 0.5);
    x += 0.225 * x * (fabs_d(x) - 1.0);
    return x;
}
#else
#define table_cos xma_cos
#endif

static void mdct_build(mdct_table *m, int bits, double scale)
{
    int n = 1 << bits, n4 = n >> 2, fft_bits = bits - 2;
    m->bits = bits;
    double theta = 1.0 / 8.0;
    double root = 1.0;
    // sqrt(fabs(scale)) by Newton's method
    for (int i = 0; i < 60; i++)
        root = 0.5 * (root + scale / root);
    for (int i = 0; i < n4; i++) {
        double alpha = 2.0 * k_pi * (i + theta) / n;
        m->tcos[i] = (float)(-table_cos(alpha) * root);
        m->tsin[i] = (float)(-xma_sin(alpha) * root);
    }
    for (int k = 0; k < n4 / 2; k++) {
        double angle = 2.0 * k_pi * k / n4;
        m->twiddle_re[k] = (float)xma_cos(angle);
        m->twiddle_im[k] = (float)xma_sin(angle);
    }
    for (int i = 0; i < n4; i++) {
        unsigned r = 0;
        for (int b = 0; b < fft_bits; b++)
            r |= ((i >> b) & 1u) << (fft_bits - 1 - b);
        m->bit_reverse[i] = (uint16_t)r;
    }
}

static void build_tables(void)
{
    vlc_build(&g_sf_vlc, HUFF_SCALE_SIZE, scale_huffbits, scale_huffcodes, 2);
    vlc_build(&g_sf_rl_vlc, HUFF_SCALE_RL_SIZE, scale_rl_huffbits, scale_rl_huffcodes, 4);
    vlc_build(&g_coef_vlc[0], HUFF_COEF0_SIZE, coef0_huffbits, coef0_huffcodes, 4);
    vlc_build(&g_coef_vlc[1], HUFF_COEF1_SIZE, coef1_huffbits, coef1_huffcodes, 4);
    vlc_build(&g_vec4_vlc, HUFF_VEC4_SIZE, vec4_huffbits, vec4_huffcodes, 2);
    vlc_build(&g_vec2_vlc, HUFF_VEC2_SIZE, vec2_huffbits, vec2_huffcodes, 2);
    vlc_build(&g_vec1_vlc, HUFF_VEC1_SIZE, vec1_huffbits, vec1_huffcodes, 2);
    for (int i = 0; i < 33; i++)
        g_sin64[i] = (float)xma_sin(i * k_pi / 64.0);
    // ff_init_ff_sine_windows: window[i] = sin((i + 0.5) * pi / (2 n)) for a window of n points
    float *storage = g_windows_storage;
    for (int i = 0; i < 5; i++) {
        int n = 64 << i;
        for (int k = 0; k < n; k++)
            storage[k] = (float)xma_sin((k + 0.5) * (k_pi / (2.0 * n)));
        g_windows[i] = storage;
        storage += n;
    }
    // ff_mdct_init(&mdct_ctx[i], WMAPRO_BLOCK_MIN_BITS + 1 + i, 1, 1.0 / (1 << (WMAPRO_BLOCK_MIN_BITS + i - 1)) /
    // (1ll << (bits_per_sample - 1)))
    for (int i = 0; i < MDCT_SIZES; i++)
        mdct_build(&g_mdct[i], MDCT_MIN_BITS + i,
            1.0 / (double)(1 << (BLOCK_MIN_BITS + i - 1)) / (double)(1ll << (BITS_PER_SAMPLE - 1)));
    double step = 1.0, tenth_root = 1.0;
    // 10^(1/20): Newton on x^20 = 10
    for (int i = 0; i < 200; i++) {
        double p = 1.0;
        for (int k = 0; k < 19; k++)
            p *= tenth_root;
        tenth_root -= (p * tenth_root - 10.0) / (20.0 * p);
    }
    for (int i = 0; i < 20; i++) {
        g_db_steps[i] = (float)step;
        step *= tenth_root;
    }
}

static void ensure_tables(void)
{
    if (__atomic_load_n(&g_tables_state, __ATOMIC_ACQUIRE) == 2)
        return;
    int expected = 0;
    if (__atomic_compare_exchange_n(&g_tables_state, &expected, 1, 0, __ATOMIC_ACQ_REL, __ATOMIC_ACQUIRE)) {
        build_tables();
        __atomic_store_n(&g_tables_state, 2, __ATOMIC_RELEASE);
        return;
    }
    while (__atomic_load_n(&g_tables_state, __ATOMIC_ACQUIRE) != 2)
        ;
}

// ---- transforms ---------------------------------------------------------------------------------------------------

// In-place complex FFT of 2^bits points with exp(+2 pi i n k / N) (FFmpeg's inverse transform), unscaled.
static void fft_inverse(const mdct_table *m, float *z, int bits)
{
    int n = 1 << bits;
    for (int i = 0; i < n; i++) {
        int j = m->bit_reverse[i];
        if (j > i) {
            float re = z[2 * i], im = z[2 * i + 1];
            z[2 * i] = z[2 * j];
            z[2 * i + 1] = z[2 * j + 1];
            z[2 * j] = re;
            z[2 * j + 1] = im;
        }
    }
    for (int size = 2; size <= n; size <<= 1) {
        int half = size >> 1, stride = n / size;
        for (int start = 0; start < n; start += size) {
            for (int k = 0; k < half; k++) {
                float wr = m->twiddle_re[k * stride], wi = m->twiddle_im[k * stride];
                float *a = z + 2 * (start + k), *b = z + 2 * (start + k + half);
                float br = b[0] * wr - b[1] * wi;
                float bi = b[0] * wi + b[1] * wr;
                b[0] = a[0] - br;
                b[1] = a[1] - bi;
                a[0] += br;
                a[1] += bi;
            }
        }
    }
}

#define CMUL(dre, dim, are, aim, bre, bim) \
    do { \
        (dre) = (are) * (bre) - (aim) * (bim); \
        (dim) = (are) * (bim) + (aim) * (bre); \
    } while (0)

// ff_imdct_half_c: the middle half (n/2 samples) of the IMDCT of n/2 coefficients.
static void imdct_half(const mdct_table *m, float *output, const float *input)
{
    int n = 1 << m->bits, n2 = n >> 1, n4 = n >> 2, n8 = n >> 3;
    float *z = output; // n4 complex values
    const float *in1 = input, *in2 = input + n2 - 1;
    for (int k = 0; k < n4; k++) {
        CMUL(z[2 * k], z[2 * k + 1], *in2, *in1, m->tcos[k], m->tsin[k]);
        in1 += 2;
        in2 -= 2;
    }
    fft_inverse(m, z, m->bits - 2);
    for (int k = 0; k < n8; k++) {
        float r0, i0, r1, i1;
        float *a = z + 2 * (n8 - k - 1), *b = z + 2 * (n8 + k);
        CMUL(r0, i1, a[1], a[0], m->tsin[n8 - k - 1], m->tcos[n8 - k - 1]);
        CMUL(r1, i0, b[1], b[0], m->tsin[n8 + k], m->tcos[n8 + k]);
        a[0] = r0;
        a[1] = i0;
        b[0] = r1;
        b[1] = i1;
    }
}

// float_dsp vector_fmul_window_c
static void vector_fmul_window(float *dst, const float *src0, const float *src1, const float *win, int len)
{
    dst += len;
    win += len;
    src0 += len;
    for (int i = -len, j = len - 1; i < 0; i++, j--) {
        float s0 = src0[i], s1 = src1[j], wi = win[i], wj = win[j];
        dst[i] = s0 * wj - s1 * wi;
        dst[j] = s0 * wi + s1 * wj;
    }
}

// ---- decoder state ------------------------------------------------------------------------------------------------

typedef struct channel_ctx {
    int16_t prev_block_len;
    uint8_t transmit_coefs;
    uint8_t num_subframes;
    uint16_t subframe_len[MAX_SUBFRAMES];
    uint16_t subframe_offset[MAX_SUBFRAMES];
    uint8_t cur_subframe;
    uint16_t decoded_samples;
    uint8_t grouped;
    int quant_step;
    int8_t reuse_sf;
    int8_t scale_factor_step;
    int max_scale_factor;
    int saved_scale_factors[2][MAX_BANDS];
    int8_t scale_factor_idx;
    int *scale_factors;
    uint8_t table_idx;
    float *coeffs;
    uint16_t num_vec_coeffs;
    float out[CHANNEL_OUT_SIZE];
} channel_ctx;

typedef struct channel_group {
    uint8_t num_channels;
    int8_t transform;
    int8_t transform_band[MAX_BANDS];
    float decorrelation_matrix[XMA_CHANNELS * XMA_CHANNELS];
    float *channel_data[XMA_CHANNELS];
} channel_group;

struct xma_decoder {
    unsigned sample_rate;
    int8_t nb_channels;
    uint16_t samples_per_frame;
    uint8_t max_num_subframes;
    uint8_t subframe_len_bits;
    uint8_t max_subframe_len_bit;
    uint16_t min_samples_per_subframe;
    int8_t num_sfb[BLOCK_SIZES];
    int16_t sfb_offsets[BLOCK_SIZES][MAX_BANDS];
    int8_t sf_offsets[BLOCK_SIZES][BLOCK_SIZES][MAX_BANDS];
    int16_t subwoofer_cutoffs[BLOCK_SIZES];
    uint8_t len_prefix;
    uint8_t dynamic_range_compression;
    int8_t lfe_channel;

    bit_reader gb;
    int num_saved_bits;
    int frame_offset;
    int subframe_offset;
    uint8_t packet_loss;
    uint8_t drc_gain;
    int8_t parsed_all_subframes;
    int16_t subframe_len;
    int8_t channels_for_cur_subframe;
    int8_t channel_indexes_for_cur_subframe[XMA_CHANNELS];
    int8_t num_bands;
    int8_t transmit_num_vec_coeffs;
    int16_t *cur_sfb_offsets;
    uint8_t table_idx;
    int8_t esc_len;
    uint8_t num_chgroups;
    channel_group chgroup[XMA_CHANNELS];
    float tmp[SAMPLES_PER_FRAME];
    channel_ctx channel[XMA_CHANNELS];
};

unsigned PORT_MS_ABI xma_decoder_size(void) { return (unsigned)sizeof(xma_decoder); }

static int get_rate(unsigned sample_rate)
{
    if (sample_rate > 44100)
        return 48000;
    if (sample_rate > 32000)
        return 44100;
    if (sample_rate > 24000)
        return 32000;
    return 24000;
}

void PORT_MS_ABI xma_decoder_reset(xma_decoder *s)
{
    for (int i = 0; i < s->nb_channels; i++) {
        memset(s->channel[i].out, 0, sizeof(s->channel[i].out));
        s->channel[i].prev_block_len = (int16_t)s->samples_per_frame;
        s->channel[i].reuse_sf = 0;
        s->channel[i].scale_factor_idx = 0;
        memset(s->channel[i].saved_scale_factors, 0, sizeof(s->channel[i].saved_scale_factors));
    }
    s->packet_loss = 0;
}

// decode_init for an XMA stream
int PORT_MS_ABI xma_decoder_init(xma_decoder *s, unsigned sample_rate, unsigned channels)
{
    if (s == 0 || sample_rate == 0 || channels < 1 || channels > XMA_CHANNELS)
        return 0;
    ensure_tables();
    memset(s, 0, sizeof(*s));
    s->sample_rate = sample_rate;
    s->nb_channels = (int8_t)channels;
    s->len_prefix = (DECODE_FLAGS & 0x40) != 0;
    s->samples_per_frame = SAMPLES_PER_FRAME;
    int log2_max_num_subframes = (DECODE_FLAGS & 0x38) >> 3;
    s->max_num_subframes = (uint8_t)(1 << log2_max_num_subframes);
    if (s->max_num_subframes == 16 || s->max_num_subframes == 4)
        s->max_subframe_len_bit = 1;
    s->subframe_len_bits = (uint8_t)(av_log2(log2_max_num_subframes) + 1);
    int num_possible_block_sizes = log2_max_num_subframes + 1;
    s->min_samples_per_subframe = (uint16_t)(s->samples_per_frame / s->max_num_subframes);
    s->dynamic_range_compression = (DECODE_FLAGS & 0x80) != 0;
    s->lfe_channel = -1; // channel_mask 0 for XMA

    int rate = get_rate(sample_rate);
    for (int i = 0; i < num_possible_block_sizes; i++) {
        int subframe_len = s->samples_per_frame >> i;
        int band = 1;
        s->sfb_offsets[i][0] = 0;
        for (int x = 0; x < MAX_BANDS - 1 && s->sfb_offsets[i][band - 1] < subframe_len; x++) {
            int offset = (subframe_len * 2 * critical_freq[x]) / rate + 2;
            offset &= ~3;
            if (offset > s->sfb_offsets[i][band - 1])
                s->sfb_offsets[i][band++] = (int16_t)offset;
            if (offset >= subframe_len)
                break;
        }
        s->sfb_offsets[i][band - 1] = (int16_t)subframe_len;
        s->num_sfb[i] = (int8_t)(band - 1);
        if (s->num_sfb[i] <= 0)
            return 0;
    }
    for (int i = 0; i < num_possible_block_sizes; i++) {
        for (int b = 0; b < s->num_sfb[i]; b++) {
            int offset = ((s->sfb_offsets[i][b] + s->sfb_offsets[i][b + 1] - 1) << i) >> 1;
            for (int x = 0; x < num_possible_block_sizes; x++) {
                int v = 0;
                while (s->sfb_offsets[x][v + 1] << x < offset) {
                    v++;
                    if (v >= MAX_BANDS - 1)
                        return 0;
                }
                s->sf_offsets[i][x][b] = (int8_t)v;
            }
        }
    }
    for (int i = 0; i < num_possible_block_sizes; i++) {
        int block_size = s->samples_per_frame >> i;
        // (440 * block_size + 3LL * (sample_rate >> 1) - 1) / sample_rate: fits 32 bits for any XMA rate
        int cutoff = (int)((440u * (unsigned)block_size + 3u * (sample_rate >> 1) - 1u) / sample_rate);
        s->subwoofer_cutoffs[i] = (int16_t)(cutoff < 4 ? 4 : cutoff > block_size ? block_size : cutoff);
    }
    xma_decoder_reset(s);
    return 1;
}

static int decode_subframe_length(xma_decoder *s, int offset)
{
    int frame_len_shift = 0;
    if (offset == s->samples_per_frame - s->min_samples_per_subframe)
        return s->min_samples_per_subframe;
    if (get_bits_left(&s->gb) < 1)
        return -1;
    if (s->max_subframe_len_bit) {
        if (get_bits1(&s->gb))
            frame_len_shift = 1 + get_bits(&s->gb, s->subframe_len_bits - 1);
    } else {
        frame_len_shift = get_bits(&s->gb, s->subframe_len_bits);
    }
    int subframe_len = s->samples_per_frame >> frame_len_shift;
    if (subframe_len < s->min_samples_per_subframe || subframe_len > s->samples_per_frame)
        return -1;
    return subframe_len;
}

static int decode_tilehdr(xma_decoder *s)
{
    uint16_t num_samples[XMA_CHANNELS] = {0};
    uint8_t contains_subframe[XMA_CHANNELS];
    int channels_for_cur_subframe = s->nb_channels;
    int fixed_channel_layout = 0;
    int min_channel_len = 0;

    for (int c = 0; c < s->nb_channels; c++)
        s->channel[c].num_subframes = 0;
    if (s->max_num_subframes == 1 || get_bits1(&s->gb))
        fixed_channel_layout = 1;
    do {
        for (int c = 0; c < s->nb_channels; c++) {
            if (num_samples[c] == min_channel_len) {
                if (fixed_channel_layout || channels_for_cur_subframe == 1 ||
                    (min_channel_len == s->samples_per_frame - s->min_samples_per_subframe))
                    contains_subframe[c] = 1;
                else
                    contains_subframe[c] = (uint8_t)get_bits1(&s->gb);
            } else {
                contains_subframe[c] = 0;
            }
        }
        int subframe_len = decode_subframe_length(s, min_channel_len);
        if (subframe_len <= 0)
            return -1;
        min_channel_len += subframe_len;
        for (int c = 0; c < s->nb_channels; c++) {
            channel_ctx *chan = &s->channel[c];
            if (contains_subframe[c]) {
                if (chan->num_subframes >= MAX_SUBFRAMES)
                    return -1;
                chan->subframe_len[chan->num_subframes] = (uint16_t)subframe_len;
                num_samples[c] += (uint16_t)subframe_len;
                ++chan->num_subframes;
                if (num_samples[c] > s->samples_per_frame)
                    return -1;
            } else if (num_samples[c] <= min_channel_len) {
                if (num_samples[c] < min_channel_len) {
                    channels_for_cur_subframe = 0;
                    min_channel_len = num_samples[c];
                }
                ++channels_for_cur_subframe;
            }
        }
    } while (min_channel_len < s->samples_per_frame);
    for (int c = 0; c < s->nb_channels; c++) {
        int offset = 0;
        for (int i = 0; i < s->channel[c].num_subframes; i++) {
            s->channel[c].subframe_offset[i] = (uint16_t)offset;
            offset += s->channel[c].subframe_len[i];
        }
    }
    return 0;
}

// decode_decorrelation_matrix is only reached for groups of more than two channels; an XMA stream has two at most.

static int decode_channel_transform(xma_decoder *s)
{
    s->num_chgroups = 0;
    if (s->nb_channels > 1) {
        int remaining_channels = s->channels_for_cur_subframe;
        if (get_bits1(&s->gb))
            return -1; // "Channel transform bit": unsupported, as in FFmpeg
        for (s->num_chgroups = 0; remaining_channels && s->num_chgroups < s->channels_for_cur_subframe;
             s->num_chgroups++) {
            channel_group *chgroup = &s->chgroup[s->num_chgroups];
            float **channel_data = chgroup->channel_data;
            chgroup->num_channels = 0;
            chgroup->transform = 0;
            if (remaining_channels > 2) {
                return -1; // unreachable for two channels
            } else {
                chgroup->num_channels = (uint8_t)remaining_channels;
                for (int i = 0; i < s->channels_for_cur_subframe; i++) {
                    int channel_idx = s->channel_indexes_for_cur_subframe[i];
                    if (!s->channel[channel_idx].grouped)
                        *channel_data++ = s->channel[channel_idx].coeffs;
                    s->channel[channel_idx].grouped = 1;
                }
            }
            if (chgroup->num_channels == 2) {
                if (get_bits1(&s->gb)) {
                    if (get_bits1(&s->gb))
                        return -1; // "Unknown channel transform type"
                } else {
                    chgroup->transform = 1;
                    if (s->nb_channels == 2) {
                        chgroup->decorrelation_matrix[0] = 1.0f;
                        chgroup->decorrelation_matrix[1] = -1.0f;
                        chgroup->decorrelation_matrix[2] = 1.0f;
                        chgroup->decorrelation_matrix[3] = 1.0f;
                    } else {
                        chgroup->decorrelation_matrix[0] = 0.70703125f;
                        chgroup->decorrelation_matrix[1] = -0.70703125f;
                        chgroup->decorrelation_matrix[2] = 0.70703125f;
                        chgroup->decorrelation_matrix[3] = 0.70703125f;
                    }
                }
            }
            if (chgroup->transform) {
                if (!get_bits1(&s->gb)) {
                    for (int i = 0; i < s->num_bands; i++)
                        chgroup->transform_band[i] = (int8_t)get_bits1(&s->gb);
                } else {
                    memset(chgroup->transform_band, 1, (size_t)s->num_bands);
                }
            }
            remaining_channels -= chgroup->num_channels;
        }
    }
    return 0;
}

static unsigned get_large_val(bit_reader *gb)
{
    int n_bits = 8;
    if (get_bits1(gb)) {
        n_bits += 8;
        if (get_bits1(gb)) {
            n_bits += 8;
            if (get_bits1(gb))
                n_bits += 7;
        }
    }
    return get_bits(gb, n_bits);
}

// ff_wma_run_level_decode, version 1 (WMA Pro), on float coefficients
static int run_level_decode(bit_reader *gb, const vlc_table *vlc, const float *level_table, const uint16_t *run_table,
    float *ptr, int offset, int num_coefs, int block_len, int frame_len_bits)
{
    const uint32_t *ilvl = (const uint32_t *)level_table;
    const unsigned coef_mask = (unsigned)block_len - 1;
    for (; offset < num_coefs; offset++) {
        int code = get_vlc(gb, vlc);
        if (code > 1) {
            offset += run_table[code];
            uint32_t sign = get_bits1(gb) - 1;
            ptr[offset & coef_mask] = int_bits_to_float(ilvl[code] ^ (sign & 0x80000000u));
        } else if (code == 1) {
            break; // EOB
        } else if (code == 0) {
            int level = (int)get_large_val(gb);
            if (get_bits1(gb)) {
                if (get_bits1(gb)) {
                    if (get_bits1(gb))
                        return -1; // broken escape sequence
                    offset += get_bits(gb, frame_len_bits) + 4;
                } else {
                    offset += get_bits(gb, 2) + 1;
                }
            }
            int sign = (int)get_bits1(gb) - 1;
            ptr[offset & coef_mask] = (float)((level ^ sign) - sign);
        } else {
            return -1; // no such code
        }
    }
    if (offset > num_coefs)
        return -1;
    return 0;
}

static int decode_coeffs(xma_decoder *s, int c)
{
    static const uint32_t fval_tab[16] = {
        0x00000000, 0x3f800000, 0x40000000, 0x40400000, 0x40800000, 0x40a00000, 0x40c00000, 0x40e00000,
        0x41000000, 0x41100000, 0x41200000, 0x41300000, 0x41400000, 0x41500000, 0x41600000, 0x41700000,
    };
    channel_ctx *ci = &s->channel[c];
    int rl_mode = 0, cur_coeff = 0, num_zeros = 0;
    int vlctable = (int)get_bits1(&s->gb);
    const vlc_table *vlc = &g_coef_vlc[vlctable];
    const uint16_t *run = vlctable ? coef1_run : coef0_run;
    const float *level = vlctable ? coef1_level : coef0_level;

    while ((s->transmit_num_vec_coeffs || !rl_mode) && (cur_coeff + 3 < ci->num_vec_coeffs)) {
        uint32_t vals[4];
        int idx = get_vlc(&s->gb, &g_vec4_vlc);
        if (idx < 0)
            return -1;
        if (idx == HUFF_VEC4_SIZE - 1) {
            for (int i = 0; i < 4; i += 2) {
                idx = get_vlc(&s->gb, &g_vec2_vlc);
                if (idx < 0)
                    return -1;
                if (idx == HUFF_VEC2_SIZE - 1) {
                    int v0 = get_vlc(&s->gb, &g_vec1_vlc);
                    if (v0 < 0)
                        return -1;
                    uint32_t u0 = (uint32_t)v0;
                    if (v0 == HUFF_VEC1_SIZE - 1)
                        u0 += get_large_val(&s->gb);
                    int v1 = get_vlc(&s->gb, &g_vec1_vlc);
                    if (v1 < 0)
                        return -1;
                    uint32_t u1 = (uint32_t)v1;
                    if (v1 == HUFF_VEC1_SIZE - 1)
                        u1 += get_large_val(&s->gb);
                    vals[i] = float_to_int_bits((float)u0);
                    vals[i + 1] = float_to_int_bits((float)u1);
                } else {
                    vals[i] = fval_tab[symbol_to_vec2[idx] >> 4];
                    vals[i + 1] = fval_tab[symbol_to_vec2[idx] & 0xF];
                }
            }
        } else {
            vals[0] = fval_tab[symbol_to_vec4[idx] >> 12];
            vals[1] = fval_tab[(symbol_to_vec4[idx] >> 8) & 0xF];
            vals[2] = fval_tab[(symbol_to_vec4[idx] >> 4) & 0xF];
            vals[3] = fval_tab[symbol_to_vec4[idx] & 0xF];
        }
        for (int i = 0; i < 4; i++) {
            if (vals[i]) {
                uint32_t sign = get_bits1(&s->gb) - 1;
                ci->coeffs[cur_coeff] = int_bits_to_float(vals[i] ^ sign << 31);
                num_zeros = 0;
            } else {
                ci->coeffs[cur_coeff] = 0;
                rl_mode |= (++num_zeros > s->subframe_len >> 8);
            }
            ++cur_coeff;
        }
    }
    if (cur_coeff < s->subframe_len) {
        memset(&ci->coeffs[cur_coeff], 0, sizeof(*ci->coeffs) * (size_t)(s->subframe_len - cur_coeff));
        if (run_level_decode(&s->gb, vlc, level, run, ci->coeffs, cur_coeff, s->subframe_len, s->subframe_len,
                s->esc_len))
            return -1;
    }
    return 0;
}

static int decode_scale_factors(xma_decoder *s)
{
    for (int i = 0; i < s->channels_for_cur_subframe; i++) {
        int c = s->channel_indexes_for_cur_subframe[i];
        channel_ctx *ch = &s->channel[c];
        ch->scale_factors = ch->saved_scale_factors[!ch->scale_factor_idx];
        int *sf_end = ch->scale_factors + s->num_bands;
        if (ch->reuse_sf) {
            const int8_t *sf_offsets = s->sf_offsets[s->table_idx][ch->table_idx];
            for (int b = 0; b < s->num_bands; b++)
                ch->scale_factors[b] = ch->saved_scale_factors[ch->scale_factor_idx][*sf_offsets++];
        }
        if (!ch->cur_subframe || get_bits1(&s->gb)) {
            if (!ch->reuse_sf) {
                ch->scale_factor_step = (int8_t)(get_bits(&s->gb, 2) + 1);
                int val = 45 / ch->scale_factor_step;
                for (int *sf = ch->scale_factors; sf < sf_end; sf++) {
                    int code = get_vlc(&s->gb, &g_sf_vlc);
                    if (code < 0)
                        return -1;
                    val += code - 60;
                    *sf = val;
                }
            } else {
                for (int b = 0; b < s->num_bands; b++) {
                    int skip, val, sign;
                    int idx = get_vlc(&s->gb, &g_sf_rl_vlc);
                    if (idx < 0)
                        return -1;
                    if (!idx) {
                        uint32_t code = get_bits(&s->gb, 14);
                        val = (int)(code >> 6);
                        sign = (int)(code & 1) - 1;
                        skip = (int)(code & 0x3f) >> 1;
                    } else if (idx == 1) {
                        break;
                    } else {
                        skip = scale_rl_run[idx];
                        val = scale_rl_level[idx];
                        sign = (int)get_bits1(&s->gb) - 1;
                    }
                    b += skip;
                    if (b >= s->num_bands)
                        return -1;
                    ch->scale_factors[b] += (val ^ sign) - sign;
                }
            }
            ch->scale_factor_idx = (int8_t)!ch->scale_factor_idx;
            ch->table_idx = s->table_idx;
            ch->reuse_sf = 1;
        }
        ch->max_scale_factor = ch->scale_factors[0];
        for (int *sf = ch->scale_factors + 1; sf < sf_end; sf++)
            if (*sf > ch->max_scale_factor)
                ch->max_scale_factor = *sf;
    }
    return 0;
}

static void inverse_channel_transform(xma_decoder *s)
{
    for (int i = 0; i < s->num_chgroups; i++) {
        if (!s->chgroup[i].transform)
            continue;
        float data[XMA_CHANNELS];
        const int num_channels = s->chgroup[i].num_channels;
        float **ch_data = s->chgroup[i].channel_data;
        float **ch_end = ch_data + num_channels;
        const int8_t *tb = s->chgroup[i].transform_band;
        for (int16_t *sfb = s->cur_sfb_offsets; sfb < s->cur_sfb_offsets + s->num_bands; sfb++) {
            if (*tb++ == 1) {
                int end = sfb[1] < s->subframe_len ? sfb[1] : s->subframe_len;
                for (int y = sfb[0]; y < end; y++) {
                    const float *mat = s->chgroup[i].decorrelation_matrix;
                    const float *data_end = data + num_channels;
                    float *data_ptr = data;
                    for (float **ch = ch_data; ch < ch_end; ch++)
                        *data_ptr++ = (*ch)[y];
                    for (float **ch = ch_data; ch < ch_end; ch++) {
                        float sum = 0;
                        data_ptr = data;
                        while (data_ptr < data_end)
                            sum += *data_ptr++ * *mat++;
                        (*ch)[y] = sum;
                    }
                }
            } else if (s->nb_channels == 2) {
                int end = sfb[1] < s->subframe_len ? sfb[1] : s->subframe_len;
                int len = end - sfb[0];
                for (int k = 0; k < len; k++) {
                    ch_data[0][sfb[0] + k] *= 181.0f / 128;
                    ch_data[1][sfb[0] + k] *= 181.0f / 128;
                }
            }
        }
    }
}

static void wmapro_window(xma_decoder *s)
{
    for (int i = 0; i < s->channels_for_cur_subframe; i++) {
        int c = s->channel_indexes_for_cur_subframe[i];
        int winlen = s->channel[c].prev_block_len;
        float *start = s->channel[c].coeffs - (winlen >> 1);
        if (s->subframe_len < winlen) {
            start += (winlen - s->subframe_len) >> 1;
            winlen = s->subframe_len;
        }
        const float *window = g_windows[av_log2((unsigned)winlen) - BLOCK_MIN_BITS];
        winlen >>= 1;
        vector_fmul_window(start, start, start + winlen, window, winlen);
        s->channel[c].prev_block_len = s->subframe_len;
    }
}

static int decode_subframe(xma_decoder *s)
{
    int offset = s->samples_per_frame;
    int subframe_len = s->samples_per_frame;
    int total_samples = s->samples_per_frame * s->nb_channels;
    int transmit_coeffs = 0;

    s->subframe_offset = get_bits_count(&s->gb);
    for (int i = 0; i < s->nb_channels; i++) {
        s->channel[i].grouped = 0;
        if (offset > s->channel[i].decoded_samples) {
            offset = s->channel[i].decoded_samples;
            subframe_len = s->channel[i].subframe_len[s->channel[i].cur_subframe];
        }
    }
    s->channels_for_cur_subframe = 0;
    for (int i = 0; i < s->nb_channels; i++) {
        const int cur_subframe = s->channel[i].cur_subframe;
        total_samples -= s->channel[i].decoded_samples;
        if (offset == s->channel[i].decoded_samples && subframe_len == s->channel[i].subframe_len[cur_subframe]) {
            total_samples -= s->channel[i].subframe_len[cur_subframe];
            s->channel[i].decoded_samples += s->channel[i].subframe_len[cur_subframe];
            s->channel_indexes_for_cur_subframe[s->channels_for_cur_subframe] = (int8_t)i;
            ++s->channels_for_cur_subframe;
        }
    }
    if (!total_samples)
        s->parsed_all_subframes = 1;
    if (subframe_len < 64 || subframe_len > s->samples_per_frame)
        return -1;

    s->table_idx = (uint8_t)av_log2((unsigned)(s->samples_per_frame / subframe_len));
    s->num_bands = s->num_sfb[s->table_idx];
    s->cur_sfb_offsets = s->sfb_offsets[s->table_idx];
    int cur_subwoofer_cutoff = s->subwoofer_cutoffs[s->table_idx];

    offset += s->samples_per_frame >> 1;
    for (int i = 0; i < s->channels_for_cur_subframe; i++) {
        int c = s->channel_indexes_for_cur_subframe[i];
        s->channel[c].coeffs = &s->channel[c].out[offset];
    }
    s->subframe_len = (int16_t)subframe_len;
    s->esc_len = (int8_t)(av_log2((unsigned)(s->subframe_len - 1)) + 1);

    if (get_bits1(&s->gb)) {
        int num_fill_bits = (int)get_bits(&s->gb, 2);
        if (!num_fill_bits) {
            int len = (int)get_bits(&s->gb, 4);
            num_fill_bits = (int)get_bitsz(&s->gb, len) + 1;
        }
        if (num_fill_bits >= 0) {
            if (get_bits_count(&s->gb) + num_fill_bits > s->num_saved_bits)
                return -1;
            skip_bits(&s->gb, num_fill_bits);
        }
    }
    if (get_bits1(&s->gb))
        return -1; // reserved bit
    if (decode_channel_transform(s) < 0)
        return -1;
    for (int i = 0; i < s->channels_for_cur_subframe; i++) {
        int c = s->channel_indexes_for_cur_subframe[i];
        if ((s->channel[c].transmit_coefs = (uint8_t)get_bits1(&s->gb)))
            transmit_coeffs = 1;
    }
    if (transmit_coeffs) {
        int quant_step = 90 * BITS_PER_SAMPLE >> 4;
        if ((s->transmit_num_vec_coeffs = (int8_t)get_bits1(&s->gb))) {
            int num_bits = av_log2((unsigned)((s->subframe_len + 3) / 4)) + 1;
            for (int i = 0; i < s->channels_for_cur_subframe; i++) {
                int c = s->channel_indexes_for_cur_subframe[i];
                int num_vec_coeffs = (int)get_bits(&s->gb, num_bits) << 2;
                if (num_vec_coeffs > s->subframe_len)
                    return -1;
                s->channel[c].num_vec_coeffs = (uint16_t)num_vec_coeffs;
            }
        } else {
            for (int i = 0; i < s->channels_for_cur_subframe; i++) {
                int c = s->channel_indexes_for_cur_subframe[i];
                s->channel[c].num_vec_coeffs = (uint16_t)s->subframe_len;
            }
        }
        int step = get_sbits(&s->gb, 6);
        quant_step += step;
        if (step == -32 || step == 31) {
            const int sign = (step == 31) - 1;
            int quant = 0;
            while (get_bits_count(&s->gb) + 5 < s->num_saved_bits && (step = (int)get_bits(&s->gb, 5)) == 31)
                quant += 31;
            quant_step += ((quant + step) ^ sign) - sign;
        }
        if (s->channels_for_cur_subframe == 1) {
            s->channel[s->channel_indexes_for_cur_subframe[0]].quant_step = quant_step;
        } else {
            int modifier_len = (int)get_bits(&s->gb, 3);
            for (int i = 0; i < s->channels_for_cur_subframe; i++) {
                int c = s->channel_indexes_for_cur_subframe[i];
                s->channel[c].quant_step = quant_step;
                if (get_bits1(&s->gb)) {
                    if (modifier_len)
                        s->channel[c].quant_step += (int)get_bits(&s->gb, modifier_len) + 1;
                    else
                        ++s->channel[c].quant_step;
                }
            }
        }
        if (decode_scale_factors(s) < 0)
            return -1;
    }

    for (int i = 0; i < s->channels_for_cur_subframe; i++) {
        int c = s->channel_indexes_for_cur_subframe[i];
        if (s->channel[c].transmit_coefs && get_bits_count(&s->gb) < s->num_saved_bits) {
            decode_coeffs(s, c); // FFmpeg ignores the result: a bad run leaves what was decoded
        } else {
            memset(s->channel[c].coeffs, 0, sizeof(*s->channel[c].coeffs) * (size_t)subframe_len);
        }
    }

    if (transmit_coeffs) {
        // mdct_ctx[av_log2(subframe_len) - WMAPRO_BLOCK_MIN_BITS]: g_mdct[i] is the MDCT of 2^(7 + i) points
        const mdct_table *mdct = &g_mdct[av_log2((unsigned)subframe_len) - BLOCK_MIN_BITS];
        inverse_channel_transform(s);
        for (int i = 0; i < s->channels_for_cur_subframe; i++) {
            int c = s->channel_indexes_for_cur_subframe[i];
            const int *sf = s->channel[c].scale_factors;
            if (c == s->lfe_channel)
                memset(&s->tmp[cur_subwoofer_cutoff], 0, sizeof(*s->tmp) * (size_t)(subframe_len - cur_subwoofer_cutoff));
            for (int b = 0; b < s->num_bands; b++) {
                const int end = s->cur_sfb_offsets[b + 1] < s->subframe_len ? s->cur_sfb_offsets[b + 1] : s->subframe_len;
                const int exp = s->channel[c].quant_step - (s->channel[c].max_scale_factor - *sf++) *
                                                              s->channel[c].scale_factor_step;
                const float quant = db_to_gain(exp);
                for (int k = s->cur_sfb_offsets[b]; k < end; k++)
                    s->tmp[k] = s->channel[c].coeffs[k] * quant;
            }
            imdct_half(mdct, s->channel[c].coeffs, s->tmp);
        }
    }

    wmapro_window(s);

    for (int i = 0; i < s->channels_for_cur_subframe; i++) {
        int c = s->channel_indexes_for_cur_subframe[i];
        if (s->channel[c].cur_subframe >= s->channel[c].num_subframes)
            return -1;
        ++s->channel[c].cur_subframe;
    }
    return 0;
}

// decode_frame: returns 1 with the frame's samples in out, 0 on a bitstream error before any output.
static int decode_frame(xma_decoder *s, float *out)
{
    bit_reader *gb = &s->gb;
    int len = 0;
    if (s->len_prefix)
        len = (int)get_bits(gb, LOG2_FRAME_SIZE);
    if (decode_tilehdr(s)) {
        s->packet_loss = 1;
        return 0;
    }
    if (s->nb_channels > 1 && get_bits1(gb)) {
        if (get_bits1(gb)) {
            for (int i = 0; i < s->nb_channels * s->nb_channels; i++)
                skip_bits(gb, 4);
        }
    }
    if (s->dynamic_range_compression)
        s->drc_gain = (uint8_t)get_bits(gb, 8);
    if (get_bits1(gb)) {
        if (get_bits1(gb))
            skip_bits(gb, av_log2(s->samples_per_frame * 2));
        if (get_bits1(gb))
            skip_bits(gb, av_log2(s->samples_per_frame * 2));
    }
    s->parsed_all_subframes = 0;
    for (int i = 0; i < s->nb_channels; i++) {
        s->channel[i].decoded_samples = 0;
        s->channel[i].cur_subframe = 0;
        s->channel[i].reuse_sf = 0;
    }
    while (!s->parsed_all_subframes) {
        if (decode_subframe(s) < 0) {
            s->packet_loss = 1;
            return 0;
        }
    }
    for (int i = 0; i < s->nb_channels; i++)
        memcpy(out + i * SAMPLES_PER_FRAME, s->channel[i].out, SAMPLES_PER_FRAME * sizeof(float));
    for (int i = 0; i < s->nb_channels; i++)
        memcpy(&s->channel[i].out[0], &s->channel[i].out[s->samples_per_frame],
            s->samples_per_frame * sizeof(float) >> 1);
    // The length check FFmpeg makes next only logs; the XMAFRAMES caller keeps the samples either way.
    (void)len;
    return 1;
}

int PORT_MS_ABI xma_decoder_decode_frame(xma_decoder *s, const unsigned char *frame, unsigned frame_bits, float *out)
{
    if (frame_bits < LOG2_FRAME_SIZE + 1 || frame_bits > XMA_MAX_FRAME_BITS) {
        memset(out, 0, sizeof(float) * SAMPLES_PER_FRAME * (size_t)s->nb_channels);
        xma_decoder_reset(s);
        return 0;
    }
    // save_bits(s, gb, frame_len, 0) with the frame at bit 0: frame_offset 0
    s->frame_offset = 0;
    s->num_saved_bits = (int)frame_bits;
    init_get_bits(&s->gb, frame, (int)frame_bits, XMA_FRAME_BUFFER_BYTES);
    s->packet_loss = 0;
    if (!decode_frame(s, out)) {
        memset(out, 0, sizeof(float) * SAMPLES_PER_FRAME * (size_t)s->nb_channels);
        xma_decoder_reset(s);
        return 0;
    }
    return 1;
}

void PORT_MS_ABI xma_decoder_flush(xma_decoder *s, float *out)
{
    for (int i = 0; i < s->nb_channels; i++) {
        memset(out + i * SAMPLES_PER_FRAME, 0, SAMPLES_PER_FRAME * sizeof(float));
        memcpy(out + i * SAMPLES_PER_FRAME, s->channel[i].out, SAMPLES_PER_FRAME * sizeof(float) >> 1);
    }
    xma_decoder_reset(s);
}

// ---- packet cursor ------------------------------------------------------------------------------------------------

static const unsigned char *packet_at(const xma_cursor *c, unsigned packet) { return c->data + packet * XMA_PACKET_BYTES; }

// Bit offset in the packet of the first frame that starts in it (past the 32-bit header), or 0 if none does.
static unsigned packet_first_frame(const xma_cursor *c, unsigned packet)
{
    const unsigned char *p = packet_at(c, packet);
    unsigned offset = ((p[0] & 3u) << 13) | ((unsigned)p[1] << 5) | (p[2] >> 3);
    offset += XMA_PACKET_HEADER_BITS;
    return offset < XMA_PACKET_BITS ? offset : 0;
}

static unsigned next_stream_packet(const xma_cursor *c, unsigned packet)
{
    unsigned skip = packet_at(c, packet)[3];
    return skip == 0xff ? c->packet_count : packet + 1 + skip;
}

// Moves to the first frame start at or after `packet`. Returns 0 when there is none.
static int cursor_enter_packet(xma_cursor *c, unsigned packet)
{
    while (packet < c->packet_count) {
        unsigned first = packet_first_frame(c, packet);
        if (first != 0) {
            c->packet = packet;
            c->bit = first;
            return 1;
        }
        packet = next_stream_packet(c, packet);
    }
    c->done = 1;
    return 0;
}

void PORT_MS_ABI xma_cursor_init(xma_cursor *c, const void *data, unsigned bytes, unsigned stream)
{
    memset(c, 0, sizeof(*c));
    c->data = (const unsigned char *)data;
    c->packet_count = data != 0 ? bytes / XMA_PACKET_BYTES : 0;
    cursor_enter_packet(c, stream);
}

int PORT_MS_ABI xma_cursor_seek_bits(xma_cursor *c, unsigned bit_offset)
{
    unsigned packet = bit_offset / XMA_PACKET_BITS, bit = bit_offset % XMA_PACKET_BITS;
    if (packet >= c->packet_count || bit < XMA_PACKET_HEADER_BITS)
        return 0;
    c->packet = packet;
    c->bit = bit;
    c->done = 0;
    return 1;
}

unsigned PORT_MS_ABI xma_cursor_tell_bits(const xma_cursor *c) { return c->packet * XMA_PACKET_BITS + c->bit; }

// Reads `count` (<= 32) bits from a stream position, crossing into the stream's next packets. Returns 0 if the
// buffer ends first.
static int stream_read(const xma_cursor *c, unsigned *packet, unsigned *bit, unsigned count, unsigned *value)
{
    unsigned v = 0;
    while (count != 0) {
        if (*bit >= XMA_PACKET_BITS) {
            *packet = next_stream_packet(c, *packet);
            *bit = XMA_PACKET_HEADER_BITS;
        }
        if (*packet >= c->packet_count)
            return 0;
        const unsigned char *p = packet_at(c, *packet);
        unsigned take = XMA_PACKET_BITS - *bit;
        if (take > count)
            take = count;
        if (take > 8 - (*bit & 7))
            take = 8 - (*bit & 7);
        unsigned byte = p[*bit >> 3];
        unsigned bits = (byte >> (8 - (*bit & 7) - take)) & ((1u << take) - 1);
        v = (v << take) | bits;
        *bit += take;
        count -= take;
    }
    *value = v;
    return 1;
}

// Moves a stream position on by `count` bits, crossing into the stream's next packets. Returns 0 past the buffer.
static int stream_advance(const xma_cursor *c, unsigned *packet, unsigned *bit, unsigned count)
{
    while (count != 0) {
        if (*bit >= XMA_PACKET_BITS) {
            *packet = next_stream_packet(c, *packet);
            *bit = XMA_PACKET_HEADER_BITS;
        }
        if (*packet >= c->packet_count)
            return 0;
        unsigned take = XMA_PACKET_BITS - *bit;
        if (take > count)
            take = count;
        *bit += take;
        count -= take;
    }
    return 1;
}

// The next frame: its length, and (with `frame`) its bits copied to frame[]. Walks without copying when frame is 0.
static unsigned cursor_frame(xma_cursor *c, unsigned char *frame)
{
    for (int guard = 0; guard < 4096 && !c->done; guard++) {
        if (c->packet >= c->packet_count) {
            c->done = 1;
            break;
        }
        if (c->bit >= XMA_PACKET_BITS) {
            if (!cursor_enter_packet(c, next_stream_packet(c, c->packet)))
                break;
            continue;
        }
        unsigned packet = c->packet, bit = c->bit, length = 0;
        if (!stream_read(c, &packet, &bit, LOG2_FRAME_SIZE, &length)) {
            c->done = 1;
            break;
        }
        if (length == XMA_MAX_FRAME_BITS || length <= LOG2_FRAME_SIZE) {
            // no further frame in this packet (padding to its end)
            if (!cursor_enter_packet(c, next_stream_packet(c, c->packet)))
                break;
            continue;
        }
        unsigned start_packet = c->packet, trailer = 0;
        packet = c->packet;
        bit = c->bit;
        if (frame != 0) {
            // copy the frame: its length field and the rest
            memset(frame, 0, XMA_FRAME_BUFFER_BYTES);
            unsigned copied = 0;
            int complete = 1;
            while (copied < length) {
                unsigned chunk = length - copied >= 8 ? 8 : length - copied, value = 0;
                if (!stream_read(c, &packet, &bit, chunk, &value)) {
                    complete = 0;
                    break;
                }
                unsigned at = copied >> 3, shift = copied & 7;
                unsigned left_aligned = value << (8 - chunk); // chunk bits at the top of a byte
                frame[at] |= (unsigned char)(left_aligned >> shift);
                if (shift != 0)
                    frame[at + 1] |= (unsigned char)(left_aligned << (8 - shift));
                copied += chunk;
            }
            if (!complete) {
                c->done = 1; // the frame runs past the buffer: the stream ends with the last whole frame
                break;
            }
            trailer = (frame[(length - 1) >> 3] >> (7 - ((length - 1) & 7))) & 1;
        } else {
            if (!stream_advance(c, &packet, &bit, length - 1) || !stream_read(c, &packet, &bit, 1, &trailer)) {
                c->done = 1;
                break;
            }
        }
        if (packet != start_packet) {
            // a frame that crosses into another packet is the last to start in its first one; the next starts where
            // that packet's header says (right after this frame)
            unsigned first = packet < c->packet_count ? packet_first_frame(c, packet) : 0;
            c->packet = packet;
            c->bit = first != 0 ? first : bit;
            if (first == 0)
                cursor_enter_packet(c, next_stream_packet(c, packet)); // sets done at the buffer's end
        } else if (trailer == 0 || bit + LOG2_FRAME_SIZE > XMA_PACKET_BITS) {
            // the last frame in its packet
            if (!cursor_enter_packet(c, next_stream_packet(c, packet)))
                c->done = 1;
        } else {
            c->bit = bit;
        }
        c->frame_index++;
        return length;
    }
    c->done = 1;
    return 0;
}

unsigned PORT_MS_ABI xma_cursor_next_frame(xma_cursor *c, unsigned char *frame) { return frame != 0 ? cursor_frame(c, frame) : 0; }

unsigned PORT_MS_ABI xma_cursor_skip_frame(xma_cursor *c) { return cursor_frame(c, 0); }
