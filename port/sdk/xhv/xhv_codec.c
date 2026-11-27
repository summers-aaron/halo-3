// port/sdk/xhv (both hosts): the voice codec (interface, packet layout and bit budget: xhv_codec.h).
//
// Narrowband algebraic CELP, 20 ms frames of four 5 ms subframes, 151 payload bits a frame:
//   LSF          31  10 line spectral frequencies: residual of a first-order MA prediction from the last frame's
//                    quantized residual, each scalar quantized (k_lsf_bits / k_lsf_step around k_lsf_mean)
//   pitch lag    24  8 bits absolute in subframes 0 and 2 (half samples 20..84.5, whole samples 85..147),
//                    4 bits relative (-4..+3.5 around the previous subframe's) in subframes 1 and 3
//   innovation   68  4 signed pulses a subframe on the interleaved tracks 5k, 5k+1, 5k+2, 5k+3|5k+4 (13 + 4 bits)
//   gains        28  a subframe: pitch gain (3 bits, k_gp) and the innovation gain as a correction of its
//                    MA-predicted energy (4 bits, k_gc_db), searched jointly on the weighted error
// The analysis is the textbook one: an asymmetric 240-sample LPC window with 5 ms of look-ahead, lag-windowed
// autocorrelation and Levinson-Durbin, LSF interpolation per subframe, perceptual weighting A(z/0.94)/A(z/0.6),
// open-loop pitch on the weighted speech, closed-loop pitch and innovation search on the weighted target with the
// zero-input response removed. The tables are this codec's own, fitted on a speech corpus (port/GAME-CHANGES.md,
// XHV voice chat).
//
// No C library: the math (sqrt, log2, exp2, sin) is here, so the Windows runtime links nothing new and both hosts
// compute bit-identical values from the same packets.
#include "xhv_codec.h"

#include <stdint.h>

#define M XHV_CODEC_ORDER
#define L_FRAME XHV_CODEC_NB_FRAME
#define L_SUBFR XHV_CODEC_NB_SUBFRAME
#define PIT_MAX XHV_CODEC_PITCH_MAX
#define EXC_HIST XHV_CODEC_EXC_HISTORY
#define L_WINDOW 240
#define L_AHEAD 40
#define N_SUBFR 4
#define PAYLOAD_BITS 151
#define INTERP_TAPS 8 // half-sample interpolation: taps a side
#define RS_TAPS 79    // resampling low-pass (16 kHz)
#define H_MIN 40      // lags in half samples: 20.0
#define H_MAX 294     //                       147.0
#define H_FRAC_END 170 // half-sample resolution below 85 in absolute subframes

static const float k_gamma1 = 0.94f, k_gamma2 = 0.6f;         // perceptual weighting
static const float k_pf_gamma_n = 0.55f, k_pf_gamma_d = 0.7f;  // short-term postfilter
static const float k_sharp_min = 0.2f, k_sharp_max = 0.8f;
static const float k_mean_energy = 36.0f;                      // innovation energy predictor mean (dB)
static const float k_energy_pred[4] = {0.68f, 0.58f, 0.34f, 0.19f};
static const float k_lsf_ma = 0.65f;                            // LSF MA prediction from the last quantized residual
static const float k_lsf_gap = 0.0392f, k_lsf_low = 0.005f, k_lsf_high = 3.135f;

// ---- tables fitted on a speech corpus (LSF mean and MA-residual spread; Lloyd-Max pitch and innovation gains) -----

static const float k_lsf_mean[M] = {0.2220f, 0.3428f, 0.5178f, 0.9154f, 1.2232f, 1.5421f, 1.9190f, 2.1296f, 2.4958f,
                                    2.6787f};
static const unsigned char k_lsf_bits[M] = {2, 3, 3, 4, 4, 4, 3, 3, 3, 2};
static const float k_lsf_step[M] = {0.0695f, 0.0598f, 0.0993f, 0.0770f, 0.0784f, 0.0662f, 0.0930f, 0.0716f, 0.0569f,
                                    0.0775f};
static const float k_gp[8] = {0.0f, 0.225f, 0.406f, 0.573f, 0.732f, 0.876f, 1.014f, 1.178f};
static const float k_gc_db[16] = {-19.9f, -15.4f, -12.2f, -9.1f, -6.3f, -3.9f, -1.8f, 0.1f,
                                  1.85f, 3.65f, 5.45f, 7.4f, 9.5f, 11.9f, 15.5f, 21.5f};

// ---- math without libm ----------------------------------------------------------------------------------------------

static const double k_pi = 3.14159265358979323846;

typedef union {
    float f;
    uint32_t i;
} float_bits;

static float xv_sqrt(float x)
{
    if (!(x > 0.0f))
        return 0.0f;
    float_bits u;
    u.f = x;
    u.i = 0x1fbd1df5u + (u.i >> 1); // within 4%; three Newton steps reach float precision
    float y = u.f;
    y = 0.5f * (y + x / y);
    y = 0.5f * (y + x / y);
    y = 0.5f * (y + x / y);
    return y;
}

static float xv_log2(float x)
{
    if (!(x > 1e-30f))
        return -100.0f;
    float_bits u;
    u.f = x;
    int e = (int)((u.i >> 23) & 255u) - 127;
    u.i = (u.i & 0x007fffffu) | 0x3f800000u;
    float m = u.f;
    if (m > 1.41421356f) {
        m *= 0.5f;
        e++;
    }
    float s = (m - 1.0f) / (m + 1.0f), s2 = s * s; // |s| <= 0.172: the atanh series to s^11 is exact in float
    float ln = 2.0f * s * (1.0f + s2 * (1.0f / 3.0f + s2 * (1.0f / 5.0f + s2 * (1.0f / 7.0f + s2 * (1.0f / 9.0f)))));
    return (float)e + ln * 1.44269504f;
}

static float xv_exp2(float y)
{
    if (y < -126.0f)
        return 0.0f;
    if (y > 126.0f)
        y = 126.0f;
    int n = (int)y;
    if ((float)n > y)
        n--;
    float f = (y - (float)n) * 0.69314718f;
    float p = 1.0f + f * (1.0f + f * (0.5f + f * (1.0f / 6.0f + f * (1.0f / 24.0f + f * (1.0f / 120.0f +
        f * (1.0f / 720.0f + f * (1.0f / 5040.0f)))))));
    float_bits u;
    u.i = (uint32_t)(n + 127) << 23;
    return p * u.f;
}

static float db_to_amplitude(float db) { return xv_exp2(db * (3.32192809f / 20.0f)); }
static float energy_db(float energy) { return 3.01029996f * xv_log2(energy); } // 10 log10

// sin of any x: reduced to [-pi, pi], then the Taylor series to x^25 (|error| < 1e-15 there)
static double xv_sin(double x)
{
    double turns = x / (2.0 * k_pi);
    double whole = (double)(long long)(turns < 0.0 ? turns - 0.5 : turns + 0.5);
    x -= whole * 2.0 * k_pi;
    double term = x, sum = x, x2 = x * x;
    for (int n = 1; n <= 12; n++) {
        term *= -x2 / (double)((2 * n) * (2 * n + 1));
        sum += term;
    }
    return sum;
}

static double xv_cos(double x) { return xv_sin(x + k_pi / 2.0); }

// ---- tables built once (deterministic: every caller computes the same values) ---------------------------------------

static float g_window[L_WINDOW];
static float g_lag_window[M + 1];
static float g_interp[INTERP_TAPS]; // h(k + 1/2), k = 0..7: the half-sample interpolator, one side
static float g_resample[RS_TAPS];
static float g_hpf_b[3], g_hpf_a[3];
static float g_grid[101]; // cos of the LSF search grid
static volatile int g_tables_ready;

static void build_tables(void)
{
    if (g_tables_ready)
        return;
    // LPC window: a half Hamming window rising over 200 samples, a quarter cosine falling over the 40 ahead
    for (int n = 0; n < 200; n++)
        g_window[n] = (float)(0.54 - 0.46 * xv_cos(2.0 * k_pi * n / 399.0));
    for (int n = 0; n < 40; n++)
        g_window[200 + n] = (float)xv_cos(2.0 * k_pi * n / 159.0);
    // lag window: 60 Hz Gaussian, white-noise correction on r(0)
    g_lag_window[0] = 1.0001f;
    for (int k = 1; k <= M; k++) {
        double a = 2.0 * k_pi * 60.0 * k / 8000.0;
        g_lag_window[k] = (float)xv_exp2((float)(-0.5 * a * a * 1.4426950408889634));
    }
    // half-sample interpolation: Hamming-windowed sinc over 16 taps, normalized to unit DC gain
    double sum = 0.0, coefficient[INTERP_TAPS];
    for (int k = 0; k < INTERP_TAPS; k++) {
        double t = k + 0.5;
        double sinc = xv_sin(k_pi * t) / (k_pi * t);
        double window = 0.54 + 0.46 * xv_cos(k_pi * t / (INTERP_TAPS + 0.5));
        coefficient[k] = sinc * window;
        sum += 2.0 * coefficient[k];
    }
    for (int k = 0; k < INTERP_TAPS; k++)
        g_interp[k] = (float)(coefficient[k] / sum);
    // resampler: 79-tap Blackman-windowed sinc low-pass at 3.7 kHz (16 kHz), unit DC gain
    double total = 0.0, taps[RS_TAPS];
    for (int n = 0; n < RS_TAPS; n++) {
        double t = n - (RS_TAPS - 1) / 2.0, fc = 3700.0 / 16000.0;
        double sinc = t == 0.0 ? 2.0 * fc : xv_sin(2.0 * k_pi * fc * t) / (k_pi * t);
        double w = 0.42 - 0.5 * xv_cos(2.0 * k_pi * n / (RS_TAPS - 1)) + 0.08 * xv_cos(4.0 * k_pi * n / (RS_TAPS - 1));
        taps[n] = sinc * w;
        total += taps[n];
    }
    for (int n = 0; n < RS_TAPS; n++)
        g_resample[n] = (float)(taps[n] / total);
    // input high-pass: second-order Butterworth at 100 Hz (8 kHz)
    double w0 = k_pi * 100.0 / 8000.0, k = xv_sin(w0) / xv_cos(w0), r2 = 1.4142135623730951;
    double norm = 1.0 / (1.0 + r2 * k + k * k);
    g_hpf_b[0] = (float)norm;
    g_hpf_b[1] = (float)(-2.0 * norm);
    g_hpf_b[2] = (float)norm;
    g_hpf_a[1] = (float)(2.0 * (k * k - 1.0) * norm);
    g_hpf_a[2] = (float)((1.0 - r2 * k + k * k) * norm);
    for (int i = 0; i <= 100; i++)
        g_grid[i] = (float)xv_cos(k_pi * i / 100.0);
    g_tables_ready = 1;
}

// ---- LPC ------------------------------------------------------------------------------------------------------------

static void levinson(const float *r, float *a)
{
    float next[M + 1];
    a[0] = 1.0f;
    for (int i = 1; i <= M; i++)
        a[i] = 0.0f;
    float error = r[0];
    for (int i = 1; i <= M; i++) {
        if (error <= 1e-9f * r[0] || error <= 0.0f)
            break;
        float acc = r[i];
        for (int j = 1; j < i; j++)
            acc += a[j] * r[i - j];
        float k = -acc / error;
        if (k > 0.9999f)
            k = 0.9999f;
        if (k < -0.9999f)
            k = -0.9999f;
        for (int j = 1; j < i; j++)
            next[j] = a[j] + k * a[i - j];
        for (int j = 1; j < i; j++)
            a[j] = next[j];
        a[i] = k;
        error *= 1.0f - k * k;
    }
}

static void weight_lpc(const float *a, float gamma, float *ap)
{
    float g = 1.0f;
    for (int i = 0; i <= M; i++) {
        ap[i] = a[i] * g;
        g *= gamma;
    }
}

// y[n] = sum a[i] x[n - i], x with M samples of history before it
static void residual(const float *a, const float *x, float *y, int length)
{
    for (int n = 0; n < length; n++) {
        float s = x[n];
        for (int i = 1; i <= M; i++)
            s += a[i] * x[n - i];
        y[n] = s;
    }
}

// y[n] = x[n] - sum a[i] y[n - i]; memory holds the last M outputs, oldest first; update keeps the new ones
static void synthesis(const float *a, const float *x, float *y, int length, float *memory, int update)
{
    float buffer[M + 2 * L_FRAME];
    for (int i = 0; i < M; i++)
        buffer[i] = memory[i];
    float *out = buffer + M;
    for (int n = 0; n < length; n++) {
        float s = x[n];
        for (int i = 1; i <= M; i++)
            s -= a[i] * out[n - i];
        out[n] = s;
    }
    for (int n = 0; n < length; n++)
        y[n] = out[n];
    if (update)
        for (int i = 0; i < M; i++)
            memory[i] = out[length - M + i];
}

// Chebyshev evaluation of the sum/difference polynomial f (f[0] = 1, five more coefficients) at x = cos(w)
static float chebyshev(float x, const float *f)
{
    float b2 = 1.0f, b1 = 2.0f * x + f[1], b0;
    for (int i = 2; i < 5; i++) {
        b0 = 2.0f * x * b1 - b2 + f[i];
        b2 = b1;
        b1 = b0;
    }
    return x * b1 - b2 + 0.5f * f[5];
}

// LPC to LSF (radians): the roots of the sum and difference polynomials alternate along w = 0..pi; scanned on a
// 100-point grid, each sign change bisected 8 times and the root placed by linear interpolation. 0 if fewer than M
// roots were found (the caller keeps the last frame's).
static int lpc_to_lsf(const float *a, float *lsf)
{
    float f1[6], f2[6];
    f1[0] = 1.0f;
    f2[0] = 1.0f;
    for (int i = 0; i < 5; i++) {
        f1[i + 1] = a[i + 1] + a[M - i] - f1[i];
        f2[i + 1] = a[i + 1] - a[M - i] + f2[i];
    }
    int found = 0, j = 0;
    const float *f = f1;
    float w_low = 0.0f, y_low = chebyshev(g_grid[0], f);
    while (found < M && j < 100) {
        j++;
        float w_high = w_low, y_high = y_low;
        w_low = (float)(k_pi * j / 100.0);
        y_low = chebyshev(g_grid[j], f);
        if (y_low * y_high > 0.0f)
            continue;
        for (int i = 0; i < 8; i++) {
            float w_mid = 0.5f * (w_low + w_high), y_mid = chebyshev((float)xv_cos(w_mid), f);
            if (y_low * y_mid <= 0.0f) {
                y_high = y_mid;
                w_high = w_mid;
            } else {
                y_low = y_mid;
                w_low = w_mid;
            }
        }
        float root = y_high != y_low ? w_low - y_low * (w_high - w_low) / (y_high - y_low) : w_low;
        lsf[found++] = root;
        f = (found & 1) ? f2 : f1;
        w_low = root;
        y_low = chebyshev((float)xv_cos(root), f);
    }
    return found == M;
}

static void lsp_polynomial(const float *q, float *f) // q: the 5 cosines of one set, stride 2
{
    f[0] = 1.0f;
    f[1] = -2.0f * q[0];
    for (int i = 2; i <= 5; i++) {
        float b = -2.0f * q[2 * (i - 1)];
        f[i] = b * f[i - 1] + 2.0f * f[i - 2];
        for (int j = i - 1; j > 1; j--)
            f[j] += b * f[j - 1] + f[j - 2];
        f[1] += b;
    }
}

static void lsf_to_lpc(const float *lsf, float *a)
{
    float q[M], f1[6], f2[6];
    for (int i = 0; i < M; i++)
        q[i] = (float)xv_cos(lsf[i]);
    lsp_polynomial(&q[0], f1);
    lsp_polynomial(&q[1], f2);
    for (int i = 5; i > 0; i--) {
        f1[i] += f1[i - 1];
        f2[i] -= f2[i - 1];
    }
    a[0] = 1.0f;
    for (int i = 1, j = M; i <= 5; i++, j--) {
        a[i] = 0.5f * (f1[i] + f2[i]);
        a[j] = 0.5f * (f1[i] - f2[i]);
    }
}

// ordered, apart by at least the gap, inside (low, high)
static void lsf_stabilize(float *lsf)
{
    for (int pass = 0; pass < 2; pass++)
        for (int i = 0; i < M - 1; i++)
            if (lsf[i + 1] < lsf[i]) {
                float t = lsf[i];
                lsf[i] = lsf[i + 1];
                lsf[i + 1] = t;
            }
    if (lsf[0] < k_lsf_low)
        lsf[0] = k_lsf_low;
    for (int i = 0; i < M - 1; i++)
        if (lsf[i + 1] - lsf[i] < k_lsf_gap)
            lsf[i + 1] = lsf[i] + k_lsf_gap;
    if (lsf[M - 1] > k_lsf_high) {
        lsf[M - 1] = k_lsf_high;
        for (int i = M - 2; i >= 0; i--)
            if (lsf[i + 1] - lsf[i] < k_lsf_gap)
                lsf[i] = lsf[i + 1] - k_lsf_gap;
    }
}

static void lsf_dequantize(const unsigned *index, float *residual_memory, float *lsfq)
{
    for (int i = 0; i < M; i++) {
        int levels = 1 << k_lsf_bits[i];
        float r = ((float)index[i] - (float)levels * 0.5f + 0.5f) * k_lsf_step[i];
        lsfq[i] = k_lsf_mean[i] + k_lsf_ma * residual_memory[i] + r;
        residual_memory[i] = r;
    }
    lsf_stabilize(lsfq);
}

static void lsf_quantize(const float *lsf, float *residual_memory, float *lsfq, unsigned *index)
{
    for (int i = 0; i < M; i++) {
        int levels = 1 << k_lsf_bits[i];
        float r = lsf[i] - k_lsf_mean[i] - k_lsf_ma * residual_memory[i];
        float q = r / k_lsf_step[i] + (float)levels * 0.5f;
        int k = q < 0.0f ? 0 : (int)q;
        if (k > levels - 1)
            k = levels - 1;
        index[i] = (unsigned)k;
    }
    lsf_dequantize(index, residual_memory, lsfq);
}

// the four subframes' filters: weights 1/4, 1/2, 3/4, 1 of the new frame's LSF
static void interpolate_lpc(const float *lsf_old, const float *lsf_new, float *a_subframes)
{
    for (int s = 0; s < N_SUBFR; s++) {
        float f = (float)(s + 1) * 0.25f, lsf[M];
        for (int i = 0; i < M; i++)
            lsf[i] = (1.0f - f) * lsf_old[i] + f * lsf_new[i];
        lsf_to_lpc(lsf, a_subframes + s * (M + 1));
    }
}

// ---- pitch ----------------------------------------------------------------------------------------------------------

// The adaptive codebook vector at lag h/2 (h in half samples), written over exc[0..length) from the excitation before
// it; for lags under the length the vector repeats itself.
static void predict_long_term(float *exc, int h, int length)
{
    int t0 = h >> 1;
    if ((h & 1) == 0) {
        for (int n = 0; n < length; n++)
            exc[n] = exc[n - t0];
        return;
    }
    for (int n = 0; n < length; n++) {
        const float *x = &exc[n - t0]; // the value sought sits between x[-1] and x[0]
        float s = 0.0f;
        for (int k = 0; k < INTERP_TAPS; k++)
            s += g_interp[k] * (x[-1 - k] + x[k]);
        exc[n] = s;
    }
}

static void convolve(const float *x, const float *h, float *y, int length)
{
    for (int n = 0; n < length; n++) {
        float s = 0.0f;
        for (int i = 0; i <= n; i++)
            s += x[i] * h[n - i];
        y[n] = s;
    }
}

static float dot(const float *a, const float *b, int length)
{
    float s = 0.0f;
    for (int n = 0; n < length; n++)
        s += a[n] * b[n];
    return s;
}

// open-loop lag of 80 weighted samples (wsp has PIT_MAX of history before it), favouring short lags over multiples
static int open_loop_pitch(const float *wsp)
{
    static const int k_sections[3][2] = {{20, 39}, {40, 79}, {80, PIT_MAX}};
    float best[3];
    int lag[3];
    for (int s = 0; s < 3; s++) {
        best[s] = -1e30f;
        lag[s] = k_sections[s][0];
        for (int t = k_sections[s][0]; t <= k_sections[s][1]; t++) {
            float c = dot(wsp, wsp - t, 80);
            if (c > best[s]) {
                best[s] = c;
                lag[s] = t;
            }
        }
        float e = dot(wsp - lag[s], wsp - lag[s], 80);
        best[s] = best[s] / xv_sqrt(e > 1e-3f ? e : 1e-3f);
    }
    if (best[1] >= 0.85f * best[2]) {
        best[2] = best[1];
        lag[2] = lag[1];
    }
    if (best[0] >= 0.85f * best[2])
        lag[2] = lag[0];
    return lag[2];
}

static int clamp_int(int v, int lo, int hi) { return v < lo ? lo : v > hi ? hi : v; }

static unsigned lag_index_absolute(int h)
{
    return h < H_FRAC_END ? (unsigned)(h - H_MIN) : 130u + (unsigned)(h / 2 - 85);
}
static int lag_absolute(unsigned index)
{
    if (index > 192u)
        index = 192u;
    return index < 130u ? (int)index + H_MIN : 2 * ((int)index - 130 + 85);
}
static int lag_relative_base(int h_previous) { return clamp_int(h_previous - 8, H_MIN, H_MAX - 15); }

// ---- the algebraic codebook -----------------------------------------------------------------------------------------

static int pulse_position(int track, unsigned index)
{
    return track < 3 ? track + 5 * (int)index : 3 + 5 * (int)(index >> 1) + (int)(index & 1u);
}

// pulses (positions and signs) to the codevector, then pitch sharpening for lags under a subframe
static void build_code(const int *position, const int *sign, int t0, float sharp, float *code)
{
    for (int n = 0; n < L_SUBFR; n++)
        code[n] = 0.0f;
    for (int p = 0; p < 4; p++)
        code[position[p]] += sign[p] > 0 ? 1.0f : -1.0f;
    if (t0 < L_SUBFR)
        for (int n = t0; n < L_SUBFR; n++)
            code[n] += sharp * code[n - t0];
}

static float innovation_gain_prediction(const float *code, const float *past_energy)
{
    float energy = 0.01f + dot(code, code, L_SUBFR);
    float predicted = k_mean_energy;
    for (int i = 0; i < 4; i++)
        predicted += k_energy_pred[i] * past_energy[i];
    return db_to_amplitude(predicted - energy_db(energy / (float)L_SUBFR));
}

static void past_energy_push(float *past_energy, float db)
{
    for (int i = 3; i > 0; i--)
        past_energy[i] = past_energy[i - 1];
    past_energy[0] = db;
}

static float clamp_sharp(float gp) { return gp < k_sharp_min ? k_sharp_min : gp > k_sharp_max ? k_sharp_max : gp; }

// ---- bits -----------------------------------------------------------------------------------------------------------

typedef struct {
    unsigned char bit[PAYLOAD_BITS];
    int position;
} bit_buffer;

static void put_bits(bit_buffer *b, unsigned value, int count)
{
    for (int i = count - 1; i >= 0; i--)
        b->bit[b->position++] = (unsigned char)((value >> i) & 1u);
}

static unsigned get_bits(bit_buffer *b, int count)
{
    unsigned v = 0;
    for (int i = 0; i < count; i++)
        v = (v << 1) | b->bit[b->position++];
    return v;
}

static void pack_packets(const bit_buffer *b, unsigned seq, int reset, unsigned char *packets)
{
    unsigned char bits[2][80];
    bits[0][0] = 0;
    bits[1][0] = 1;
    for (int i = 0; i < 3; i++) {
        bits[0][1 + i] = (unsigned char)((seq >> (2 - i)) & 1u);
        bits[1][1 + i] = bits[0][1 + i];
    }
    bits[0][4] = (unsigned char)(reset != 0);
    for (int i = 0; i < 75; i++)
        bits[0][5 + i] = b->bit[i];
    for (int i = 0; i < 76; i++)
        bits[1][4 + i] = b->bit[75 + i];
    for (int p = 0; p < 2; p++)
        for (int byte = 0; byte < XHV_CODEC_PACKET_BYTES; byte++) {
            unsigned v = 0;
            for (int i = 0; i < 8; i++)
                v = (v << 1) | bits[p][byte * 8 + i];
            packets[p * XHV_CODEC_PACKET_BYTES + byte] = (unsigned char)v;
        }
}

static void unpack_packets(const unsigned char *packets, bit_buffer *b)
{
    unsigned char bits[2][80];
    for (int p = 0; p < 2; p++)
        for (int i = 0; i < 80; i++)
            bits[p][i] = (unsigned char)((packets[p * XHV_CODEC_PACKET_BYTES + i / 8] >> (7 - i % 8)) & 1u);
    for (int i = 0; i < 75; i++)
        b->bit[i] = bits[0][5 + i];
    for (int i = 0; i < 76; i++)
        b->bit[75 + i] = bits[1][4 + i];
    b->position = 0;
}

int xhv_codec_packet_info(const unsigned char *packet, unsigned *half, unsigned *seq, unsigned *reset)
{
    unsigned h = packet[0] >> 7;
    *half = h;
    *seq = (packet[0] >> 4) & 7u;
    *reset = h == 0 ? (packet[0] >> 3) & 1u : 0u;
    return 1;
}

// ---- resampling (16 kHz <-> 8 kHz) ----------------------------------------------------------------------------------

static void decimate(xhv_resampler *r, const short *in, float *out) // 320 in, 160 out
{
    float buffer[RS_TAPS - 1 + XHV_CODEC_FRAME_SAMPLES];
    for (int i = 0; i < RS_TAPS - 1; i++)
        buffer[i] = r->history[i];
    for (int i = 0; i < XHV_CODEC_FRAME_SAMPLES; i++)
        buffer[RS_TAPS - 1 + i] = (float)in[i];
    for (int k = 0; k < L_FRAME; k++) {
        const float *x = &buffer[RS_TAPS - 1 + 2 * k + 1];
        float s = 0.0f;
        for (int j = 0; j < RS_TAPS; j++)
            s += g_resample[j] * x[-j];
        out[k] = s;
    }
    for (int i = 0; i < RS_TAPS - 1; i++)
        r->history[i] = buffer[XHV_CODEC_FRAME_SAMPLES + i];
}

static short to_pcm(float v)
{
    if (v > 32767.0f)
        return 32767;
    if (v < -32768.0f)
        return -32768;
    return (short)(v < 0.0f ? v - 0.5f : v + 0.5f);
}

static void interpolate(xhv_resampler *r, const float *in, short *out) // 160 in, 320 out
{
    float buffer[RS_TAPS - 1 + XHV_CODEC_FRAME_SAMPLES];
    for (int i = 0; i < RS_TAPS - 1; i++)
        buffer[i] = r->history[i];
    for (int k = 0; k < L_FRAME; k++) {
        buffer[RS_TAPS - 1 + 2 * k] = 2.0f * in[k];
        buffer[RS_TAPS - 1 + 2 * k + 1] = 0.0f;
    }
    for (int n = 0; n < XHV_CODEC_FRAME_SAMPLES; n++) {
        const float *x = &buffer[RS_TAPS - 1 + n];
        float s = 0.0f;
        for (int j = (n & 1); j < RS_TAPS; j += 2) // the zero-stuffed half contributes nothing
            s += g_resample[j] * x[-j];
        out[n] = to_pcm(s);
    }
    for (int i = 0; i < RS_TAPS - 1; i++)
        r->history[i] = buffer[XHV_CODEC_FRAME_SAMPLES + i];
}

// ---- encoder --------------------------------------------------------------------------------------------------------

static void encoder_reset_shared(xhv_encoder *e)
{
    for (int i = 0; i < EXC_HIST + L_FRAME; i++)
        e->exc[i] = 0.0f;
    for (int i = 0; i < M; i++) {
        e->mem_syn[i] = 0.0f;
        e->mem_err[i] = 0.0f;
        e->mem_w0[i] = 0.0f;
        e->lsfq_old[i] = k_lsf_mean[i];
        e->lsf_residual[i] = 0.0f;
    }
    for (int i = 0; i < 4; i++)
        e->past_energy[i] = -14.0f;
    e->sharp = k_sharp_min;
    e->seq = 0;
    e->fresh = 1;
}

void xhv_encoder_init(xhv_encoder *e)
{
    build_tables();
    unsigned char *p = (unsigned char *)e;
    for (unsigned i = 0; i < sizeof(*e); i++)
        p[i] = 0;
    for (int i = 0; i < M; i++)
        e->lsf_old[i] = k_lsf_mean[i];
    encoder_reset_shared(e);
}

// The impulse response of the weighted synthesis filter Ap1 / (Aq Ap2), 40 samples.
static void weighted_impulse(const float *aq, const float *ap1, const float *ap2, float *h)
{
    float x[L_SUBFR], zero[M];
    for (int n = 0; n < L_SUBFR; n++)
        x[n] = n <= M ? ap1[n] : 0.0f;
    for (int i = 0; i < M; i++)
        zero[i] = 0.0f;
    synthesis(aq, x, h, L_SUBFR, zero, 0);
    for (int i = 0; i < M; i++)
        zero[i] = 0.0f;
    synthesis(ap2, h, h, L_SUBFR, zero, 0);
}

// The 4-pulse search: maximize (d.c)^2 / (c' Phi c) over the tracks, signs fixed by the backward-filtered target.
static void search_pulses(const float *target, const float *h, int *position, int *sign)
{
    float d[L_SUBFR], dd[L_SUBFR], s[L_SUBFR];
    float phi[L_SUBFR][L_SUBFR];
    for (int n = 0; n < L_SUBFR; n++) {
        float v = 0.0f;
        for (int i = n; i < L_SUBFR; i++)
            v += target[i] * h[i - n];
        d[n] = v;
        s[n] = v >= 0.0f ? 1.0f : -1.0f;
        dd[n] = v >= 0.0f ? v : -v;
    }
    // Phi(i, i - k) = sum_{t=0}^{39-i} h(t) h(t + k), built along the diagonals
    for (int k = 0; k < L_SUBFR; k++) {
        float acc = 0.0f;
        for (int t = 0; t + k < L_SUBFR; t++) {
            acc += h[t] * h[t + k];
            int i = L_SUBFR - 1 - t, j = i - k;
            phi[i][j] = acc * s[i] * s[j];
            phi[j][i] = phi[i][j];
        }
    }
    float best_c = 0.0f, best_e = 1.0f;
    int best[4] = {0, 1, 2, 3};
    for (unsigned i0 = 0; i0 < 8; i0++) {
        int p0 = pulse_position(0, i0);
        float c0 = dd[p0], e0 = phi[p0][p0];
        for (unsigned i1 = 0; i1 < 8; i1++) {
            int p1 = pulse_position(1, i1);
            float c1 = c0 + dd[p1], e1 = e0 + phi[p1][p1] + 2.0f * phi[p0][p1];
            for (unsigned i2 = 0; i2 < 8; i2++) {
                int p2 = pulse_position(2, i2);
                float c2 = c1 + dd[p2], e2 = e1 + phi[p2][p2] + 2.0f * (phi[p0][p2] + phi[p1][p2]);
                for (unsigned i3 = 0; i3 < 16; i3++) {
                    int p3 = pulse_position(3, i3);
                    float c3 = c2 + dd[p3];
                    float e3 = e2 + phi[p3][p3] + 2.0f * (phi[p0][p3] + phi[p1][p3] + phi[p2][p3]);
                    if (c3 * c3 * best_e > best_c * best_c * e3) {
                        best_c = c3;
                        best_e = e3;
                        best[0] = (int)i0;
                        best[1] = (int)i1;
                        best[2] = (int)i2;
                        best[3] = (int)i3;
                    }
                }
            }
        }
    }
    for (int t = 0; t < 4; t++) {
        position[t] = best[t];
        sign[t] = s[pulse_position(t, (unsigned)best[t])] > 0.0f ? 1 : -1;
    }
}

void xhv_encoder_encode(xhv_encoder *e, const short *pcm, int reset, unsigned char *packets)
{
    if (reset)
        encoder_reset_shared(e);
    // band-limit to 8 kHz, high-pass, append to the analysis buffer (the coded frame trails the input by 5 ms)
    float input[L_FRAME];
    decimate(&e->decimator, pcm, input);
    for (int n = 0; n < L_FRAME; n++) {
        float x = input[n];
        float y = g_hpf_b[0] * x + g_hpf_b[1] * e->hpf_x[0] + g_hpf_b[2] * e->hpf_x[1] - g_hpf_a[1] * e->hpf_y[0] -
                  g_hpf_a[2] * e->hpf_y[1];
        e->hpf_x[1] = e->hpf_x[0];
        e->hpf_x[0] = x;
        e->hpf_y[1] = e->hpf_y[0];
        e->hpf_y[0] = y;
        input[n] = y;
    }
    for (int i = 0; i < M + L_WINDOW - L_FRAME; i++)
        e->speech[i] = e->speech[i + L_FRAME];
    for (int n = 0; n < L_FRAME; n++)
        e->speech[M + L_WINDOW - L_FRAME + n] = input[n];
    const float *window = &e->speech[M];
    const float *speech = &e->speech[M + L_WINDOW - L_FRAME - L_AHEAD]; // the frame coded now, M of history before

    // LPC analysis
    float windowed[L_WINDOW], r[M + 1], a[M + 1], lsf[M];
    for (int n = 0; n < L_WINDOW; n++)
        windowed[n] = window[n] * g_window[n];
    for (int k = 0; k <= M; k++)
        r[k] = dot(windowed, windowed + k, L_WINDOW - k) * g_lag_window[k];
    if (r[0] < 1.0f)
        r[0] = 1.0f;
    levinson(r, a);
    if (!lpc_to_lsf(a, lsf))
        for (int i = 0; i < M; i++)
            lsf[i] = e->lsf_old[i];
    unsigned lsf_index[M];
    float lsfq[M];
    lsf_quantize(lsf, e->lsf_residual, lsfq, lsf_index);

    float a_unq[N_SUBFR * (M + 1)], a_q[N_SUBFR * (M + 1)];
    interpolate_lpc(e->lsf_old, lsf, a_unq);
    interpolate_lpc(e->lsfq_old, lsfq, a_q);

    // weighted speech of the frame, then the open-loop lag of each half
    float *wsp = &e->wsp[PIT_MAX];
    for (int s = 0; s < N_SUBFR; s++) {
        float ap1[M + 1], ap2[M + 1], tmp[L_SUBFR];
        weight_lpc(a_unq + s * (M + 1), k_gamma1, ap1);
        weight_lpc(a_unq + s * (M + 1), k_gamma2, ap2);
        residual(ap1, speech + s * L_SUBFR, tmp, L_SUBFR);
        synthesis(ap2, tmp, wsp + s * L_SUBFR, L_SUBFR, e->mem_w, 1);
    }
    int open_loop[2] = {open_loop_pitch(wsp), open_loop_pitch(wsp + 80)};

    bit_buffer bits;
    bits.position = 0;
    for (int i = 0; i < M; i++)
        put_bits(&bits, lsf_index[i], k_lsf_bits[i]);

    float *exc = &e->exc[EXC_HIST];
    int h_previous = 2 * open_loop[0];
    for (int s = 0; s < N_SUBFR; s++) {
        const float *aq = a_q + s * (M + 1);
        float ap1[M + 1], ap2[M + 1];
        weight_lpc(a_unq + s * (M + 1), k_gamma1, ap1);
        weight_lpc(a_unq + s * (M + 1), k_gamma2, ap2);
        const float *sp = speech + s * L_SUBFR;
        float *ex = exc + s * L_SUBFR;

        // target: the weighted error of the LP residual with the zero-input response removed
        float error[M + L_SUBFR], target[L_SUBFR], res[L_SUBFR], h[L_SUBFR];
        residual(aq, sp, res, L_SUBFR);
        for (int i = 0; i < M; i++)
            error[i] = e->mem_err[i];
        synthesis(aq, res, error + M, L_SUBFR, e->mem_err, 0);
        residual(ap1, error + M, target, L_SUBFR);
        synthesis(ap2, target, target, L_SUBFR, e->mem_w0, 0);
        weighted_impulse(aq, ap1, ap2, h);

        // closed-loop lag
        int h_lo, h_hi, absolute = (s & 1) == 0;
        if (absolute) {
            int centre = open_loop[s >> 1];
            h_lo = clamp_int(2 * (centre - 3), H_MIN, H_MAX);
            h_hi = clamp_int(2 * (centre + 3), H_MIN, H_MAX);
        } else {
            h_lo = lag_relative_base(h_previous);
            h_hi = h_lo + 15;
        }
        int h_best = h_lo;
        float score_best = -1.0f, y[L_SUBFR];
        for (int hl = h_lo; hl <= h_hi; hl++) {
            if (absolute && hl >= H_FRAC_END && (hl & 1))
                continue;
            predict_long_term(ex, hl, L_SUBFR);
            convolve(ex, h, y, L_SUBFR);
            float xy = dot(target, y, L_SUBFR), yy = dot(y, y, L_SUBFR);
            float score = yy > 1e-6f && xy > 0.0f ? xy * xy / yy : 0.0f;
            if (score > score_best) {
                score_best = score;
                h_best = hl;
            }
        }
        if (absolute)
            put_bits(&bits, lag_index_absolute(h_best), 8);
        else
            put_bits(&bits, (unsigned)(h_best - h_lo), 4);
        h_previous = h_best;
        int t0 = h_best >> 1;
        float v[L_SUBFR];
        predict_long_term(ex, h_best, L_SUBFR);
        for (int n = 0; n < L_SUBFR; n++)
            v[n] = ex[n];
        convolve(v, h, y, L_SUBFR);
        float xy = dot(target, y, L_SUBFR), yy = dot(y, y, L_SUBFR);
        float gp = yy > 1e-6f ? xy / yy : 0.0f;
        if (gp < 0.0f)
            gp = 0.0f;
        if (gp > 1.2f)
            gp = 1.2f;

        // innovation: search on the target less the pitch contribution, with the sharpened impulse response
        float target2[L_SUBFR], hs[L_SUBFR];
        for (int n = 0; n < L_SUBFR; n++) {
            target2[n] = target[n] - gp * y[n];
            hs[n] = h[n];
        }
        if (t0 < L_SUBFR)
            for (int n = t0; n < L_SUBFR; n++)
                hs[n] += e->sharp * hs[n - t0];
        int track_index[4], sign[4], position[4];
        search_pulses(target2, hs, track_index, sign);
        for (int t = 0; t < 4; t++)
            position[t] = pulse_position(t, (unsigned)track_index[t]);
        float code[L_SUBFR], z[L_SUBFR];
        build_code(position, sign, t0, e->sharp, code);
        convolve(code, h, z, L_SUBFR);
        unsigned sign_bits = 0;
        for (int t = 0; t < 4; t++) {
            put_bits(&bits, (unsigned)track_index[t], t < 3 ? 3 : 4);
            sign_bits = (sign_bits << 1) | (sign[t] > 0 ? 1u : 0u);
        }
        put_bits(&bits, sign_bits, 4);

        // gains: the joint search over the two codebooks
        float gcode0 = innovation_gain_prediction(code, e->past_energy);
        float xx_y = dot(target, y, L_SUBFR), yy_ = dot(y, y, L_SUBFR), xz = dot(target, z, L_SUBFR);
        float zz = dot(z, z, L_SUBFR), yz = dot(y, z, L_SUBFR);
        unsigned best_gp = 0, best_gc = 0;
        float best_error = 1e38f;
        for (unsigned i = 0; i < 8; i++) {
            float p = k_gp[i];
            for (unsigned j = 0; j < 16; j++) {
                float c = gcode0 * db_to_amplitude(k_gc_db[j]);
                float err = p * p * yy_ - 2.0f * p * xx_y + c * c * zz - 2.0f * c * xz + 2.0f * p * c * yz;
                if (err < best_error) {
                    best_error = err;
                    best_gp = i;
                    best_gc = j;
                }
            }
        }
#ifdef XHV_CODEC_TRAINING
        // the unquantized optimum of the joint gains, for fitting k_gp and k_gc_db (a training build only)
        {
            float det = yy_ * zz - yz * yz;
            if (det > 1e-6f * yy_ * zz && gcode0 > 0.0f) {
                float gp_opt = (xx_y * zz - xz * yz) / det, gc_opt = (xz * yy_ - xx_y * yz) / det;
                xhv_codec_training_gains(gp_opt, gc_opt / gcode0);
            }
        }
#endif
        put_bits(&bits, best_gp, 3);
        put_bits(&bits, best_gc, 4);
        float gp_q = k_gp[best_gp], gc_q = gcode0 * db_to_amplitude(k_gc_db[best_gc]);
        past_energy_push(e->past_energy, k_gc_db[best_gc]);
        e->sharp = clamp_sharp(gp_q);

        // excitation and memories, as the decoder will have them
        for (int n = 0; n < L_SUBFR; n++)
            ex[n] = gp_q * v[n] + gc_q * code[n];
        float synth[L_SUBFR];
        synthesis(aq, ex, synth, L_SUBFR, e->mem_syn, 1);
        for (int i = 0; i < M; i++) {
            int n = L_SUBFR - M + i;
            e->mem_err[i] = sp[n] - synth[n];
            e->mem_w0[i] = target[n] - gp_q * y[n] - gc_q * z[n];
        }
    }
    for (int i = 0; i < EXC_HIST; i++)
        e->exc[i] = e->exc[i + L_FRAME];
    for (int i = 0; i < PIT_MAX; i++)
        e->wsp[i] = e->wsp[i + L_FRAME];
    for (int i = 0; i < M; i++) {
        e->lsf_old[i] = lsf[i];
        e->lsfq_old[i] = lsfq[i];
    }
    pack_packets(&bits, e->seq, e->fresh, packets);
    e->seq = (e->seq + 1) & 7u;
    e->fresh = 0;
}

// ---- decoder --------------------------------------------------------------------------------------------------------

static void decoder_reset(xhv_decoder *d)
{
    int postfilter = d->postfilter;
    xhv_resampler keep = d->interpolator;
    unsigned char *p = (unsigned char *)d;
    for (unsigned i = 0; i < sizeof(*d); i++)
        p[i] = 0;
    d->interpolator = keep;
    d->postfilter = postfilter;
    for (int i = 0; i < M; i++)
        d->lsfq_old[i] = k_lsf_mean[i];
    for (int i = 0; i < 4; i++)
        d->past_energy[i] = -14.0f;
    d->sharp = k_sharp_min;
    d->pf_gain = 1.0f;
    d->last_lag = 40;
    d->noise = 12345u;
}

void xhv_decoder_init(xhv_decoder *d)
{
    build_tables();
    unsigned char *p = (unsigned char *)d;
    for (unsigned i = 0; i < sizeof(*d); i++)
        p[i] = 0;
    d->postfilter = 1;
    decoder_reset(d);
}

// Adaptive postfilter of a subframe: long-term (on the short-term residual), short-term A(z/gn)/A(z/gd) with tilt
// compensation, and gain control back to the synthesized speech's energy.
static void postfilter(xhv_decoder *d, const float *aq, int t0, const float *syn, float *out)
{
    float apn[M + 1], apd[M + 1], buffer[M + L_SUBFR];
    weight_lpc(aq, k_pf_gamma_n, apn);
    weight_lpc(aq, k_pf_gamma_d, apd);
    for (int i = 0; i < M; i++)
        buffer[i] = d->pf_syn_hist[i];
    for (int n = 0; n < L_SUBFR; n++)
        buffer[M + n] = syn[n];
    float *res = &d->pf_res[PIT_MAX + 2];
    residual(apn, buffer + M, res, L_SUBFR);

    // long-term: the best whole lag around the decoded one
    float lt[L_SUBFR];
    int best_t = 0;
    float best_c = 0.0f;
    for (int t = t0 - 1; t <= t0 + 1; t++) {
        if (t < 20 || t > PIT_MAX + 1)
            continue;
        float c = dot(res, res - t, L_SUBFR);
        if (c > best_c) {
            best_c = c;
            best_t = t;
        }
    }
    float gl = 0.0f;
    if (best_t != 0) {
        float en = dot(res - best_t, res - best_t, L_SUBFR), en0 = dot(res, res, L_SUBFR);
        if (en > 0.0f && best_c * best_c >= 0.5f * en * en0) {
            float g = best_c / en;
            gl = 0.5f * (g > 1.0f ? 1.0f : g);
        }
    }
    for (int n = 0; n < L_SUBFR; n++)
        lt[n] = gl > 0.0f ? (res[n] + gl * res[n - best_t]) / (1.0f + gl) : res[n];

    // tilt: from the first two autocorrelations of A(z/gn)/A(z/gd)'s impulse response
    float imp[22], x[22], zero[M];
    for (int n = 0; n < 22; n++)
        x[n] = n <= M ? apn[n] : 0.0f;
    for (int i = 0; i < M; i++)
        zero[i] = 0.0f;
    synthesis(apd, x, imp, 22, zero, 0);
    float r0 = dot(imp, imp, 22), r1 = dot(imp, imp + 1, 21);
    float k1 = r0 > 0.0f ? -r1 / r0 : 0.0f;
    float mu = k1 < 0.0f ? 0.8f * k1 : 0.0f;
    float tilted[L_SUBFR];
    for (int n = 0; n < L_SUBFR; n++) {
        tilted[n] = lt[n] + mu * d->pf_tilt_mem;
        d->pf_tilt_mem = lt[n];
    }
    synthesis(apd, tilted, out, L_SUBFR, d->pf_mem_stp, 1);

    float gin = dot(syn, syn, L_SUBFR), gout = dot(out, out, L_SUBFR);
    float g = gout > 1e-3f ? xv_sqrt(gin / gout) : 0.0f;
    for (int n = 0; n < L_SUBFR; n++) {
        d->pf_gain = 0.9f * d->pf_gain + 0.1f * g;
        out[n] *= d->pf_gain;
    }
    for (int i = 0; i < PIT_MAX + 2; i++)
        d->pf_res[i] = d->pf_res[i + L_SUBFR];
    for (int i = 0; i < M; i++)
        d->pf_syn_hist[i] = syn[L_SUBFR - M + i];
}

static void decoder_finish_frame(xhv_decoder *d, const float *synth, const float *a_q, const int *lags, float fade_from,
    float fade_to, short *pcm)
{
    float out[L_FRAME];
    for (int s = 0; s < N_SUBFR; s++) {
        if (d->postfilter)
            postfilter(d, a_q + s * (M + 1), lags[s], synth + s * L_SUBFR, out + s * L_SUBFR);
        else
            for (int n = 0; n < L_SUBFR; n++)
                out[s * L_SUBFR + n] = synth[s * L_SUBFR + n];
    }
    if (fade_from != 1.0f || fade_to != 1.0f)
        for (int n = 0; n < L_FRAME; n++)
            out[n] *= fade_from + (fade_to - fade_from) * (float)(n + 1) / (float)L_FRAME;
    for (int i = 0; i < EXC_HIST; i++)
        d->exc[i] = d->exc[i + L_FRAME];
    interpolate(&d->interpolator, out, pcm);
}

void xhv_decoder_decode(xhv_decoder *d, const unsigned char *packets, short *pcm)
{
    unsigned half, seq, reset;
    xhv_codec_packet_info(packets, &half, &seq, &reset);
    if (reset)
        decoder_reset(d);
    bit_buffer bits;
    unpack_packets(packets, &bits);
    unsigned lsf_index[M];
    for (int i = 0; i < M; i++)
        lsf_index[i] = get_bits(&bits, k_lsf_bits[i]);
    float lsfq[M], a_q[N_SUBFR * (M + 1)], synth[L_FRAME];
    lsf_dequantize(lsf_index, d->lsf_residual, lsfq);
    interpolate_lpc(d->lsfq_old, lsfq, a_q);

    float *exc = &d->exc[EXC_HIST];
    int h_previous = 0, lags[N_SUBFR];
    for (int s = 0; s < N_SUBFR; s++) {
        float *ex = exc + s * L_SUBFR;
        int hl = (s & 1) == 0 ? lag_absolute(get_bits(&bits, 8))
                              : lag_relative_base(h_previous) + (int)get_bits(&bits, 4);
        h_previous = hl;
        int t0 = hl >> 1;
        lags[s] = t0;
        int position[4], sign[4];
        for (int t = 0; t < 4; t++)
            position[t] = pulse_position(t, get_bits(&bits, t < 3 ? 3 : 4));
        unsigned sign_bits = get_bits(&bits, 4);
        for (int t = 0; t < 4; t++)
            sign[t] = (sign_bits >> (3 - t)) & 1u ? 1 : -1;
        unsigned i_gp = get_bits(&bits, 3), i_gc = get_bits(&bits, 4);
        float code[L_SUBFR];
        build_code(position, sign, t0, d->sharp, code);
        float gcode0 = innovation_gain_prediction(code, d->past_energy);
        float gp = k_gp[i_gp], gc = gcode0 * db_to_amplitude(k_gc_db[i_gc]);
        past_energy_push(d->past_energy, k_gc_db[i_gc]);
        d->sharp = clamp_sharp(gp);
        predict_long_term(ex, hl, L_SUBFR);
        for (int n = 0; n < L_SUBFR; n++)
            ex[n] = gp * ex[n] + gc * code[n];
        synthesis(a_q + s * (M + 1), ex, synth + s * L_SUBFR, L_SUBFR, d->mem_syn, 1);
        d->last_lag = t0;
        d->last_gp = gp;
        d->last_gc = gc;
    }
    for (int i = 0; i < M; i++)
        d->lsfq_old[i] = lsfq[i];
    d->erased = 0;
    decoder_finish_frame(d, synth, a_q, lags, 1.0f, 1.0f, pcm);
}

static float conceal_fade(unsigned erased) // 1 at the first lost frame's start, 0 after five (100 ms)
{
    return erased >= 5 ? 0.0f : 1.0f - (float)erased / 5.0f;
}

void xhv_decoder_conceal(xhv_decoder *d, short *pcm)
{
    d->erased++;
    float a_q[N_SUBFR * (M + 1)], synth[L_FRAME];
    interpolate_lpc(d->lsfq_old, d->lsfq_old, a_q);
    for (int i = 0; i < M; i++)
        d->lsf_residual[i] *= 0.5f;
    int voiced = d->last_gp >= 0.5f;
    float gp = voiced ? (d->last_gp > 0.9f ? 0.9f : d->last_gp) : 0.0f;
    float gc = voiced ? 0.0f : d->last_gc;
    float *exc = &d->exc[EXC_HIST];
    int lags[N_SUBFR];
    for (int s = 0; s < N_SUBFR; s++) {
        float *ex = exc + s * L_SUBFR, code[L_SUBFR];
        lags[s] = d->last_lag;
        predict_long_term(ex, 2 * d->last_lag, L_SUBFR);
        for (int n = 0; n < L_SUBFR; n++)
            code[n] = 0.0f;
        for (int p = 0; p < 4; p++) {
            d->noise = d->noise * 1664525u + 1013904223u;
            code[(d->noise >> 16) % L_SUBFR] += (d->noise & 0x8000u) ? 1.0f : -1.0f;
        }
        for (int n = 0; n < L_SUBFR; n++)
            ex[n] = gp * ex[n] + gc * code[n];
        synthesis(a_q + s * (M + 1), ex, synth + s * L_SUBFR, L_SUBFR, d->mem_syn, 1);
        gp *= 0.9f;
        gc *= 0.8f;
    }
    d->last_gp = voiced ? gp : 0.0f;
    d->last_gc = gc;
    d->sharp = clamp_sharp(gp);
    // the energy predictor forgets: the mean of its memory less 4 dB, as G.729's erasure rule
    float mean = 0.25f * (d->past_energy[0] + d->past_energy[1] + d->past_energy[2] + d->past_energy[3]) - 4.0f;
    past_energy_push(d->past_energy, mean < -14.0f ? -14.0f : mean);
    decoder_finish_frame(d, synth, a_q, lags, conceal_fade(d->erased - 1), conceal_fade(d->erased), pcm);
}
