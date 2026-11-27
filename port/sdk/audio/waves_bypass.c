// port/sdk/audio (both hosts): stand-ins for the Waves DSP modules (interface: port_audio.h).
//
// sound_xaudio_xenon_initialize_dsp_manager loads four Xbox DLLs from the disc (game:\WavesLibDLL.dll,
// game:\WaveShell-Xbox.dll, game:\waves\l360.dll, game:\waves\q10.dll: PowerPC code), checks each one's version
// (export 1), takes 19 exports of WaveShell by ordinal, and hands WaveShell its allocator (export 14). The DSP
// effects the game builds on them (state_variable_filter_effect.cpp: the master "L360,L3M6" surround limiter and the
// "Q10,EQ1M" / "Q10,EQ4M" equalizers of the obstruction, occlusion and radio voices) then create a filter state
// (export 2), check its version (export 1: voice_filter_validate_version), load its preset from the game's own XEX
// section (export 11 registers it) and run it every frame (export 13), with mode and frequency setters (7, 6).
//
// No PowerPC code runs on a PC, so each module here is a table of host functions with those signatures, and a filter
// state runs a host implementation of its plug-in configured from the same preset the Xbox module reads. The presets
// are Waves "PresetXMLTree" documents (XEX resources; the game passes the text to export 11); their
// <Parameters Type="RealWorld"> list holds the settings in display units:
//   Q10 (EQ1M: band 1 only, EQ4M: bands 1-4; mono): per band [on, type, gain dB, frequency Hz, Q] x 10 bands, a second
//     channel's 10 bands, then the globals, [102] the output gain in dB. Types: 0 bell (peaking), 3 high cut
//     (low-pass), 4 low cut (high-pass); no preset the game ships uses another. The retail presets:
//       obstr_qn (the six obstruction voices): high cut 1000 Hz, output -6 dB
//       occl_qn  (the occlusion voice, into both reverbs): high cut 500 Hz, output -10 dB
//       radio_qn (the radio voice): low cut 1037 Hz, bell +3 dB at 2327 Hz (Q 2.7), high cut 5301 Hz, output 0 dB
//     The bell is the RBJ peaking biquad at the preset's Q; the cut filters are 12 dB/octave Butterworth biquads (the
//     presets set their Q control to its maximum, 100, which in Q10 shapes the cut filters' slope; it is not taken
//     as a resonance).
//   L360 (L3M6: the master voice, 6 channels FL FR FC LFE BL BR): [1..4] the thresholds and [5..8] the output
//     ceilings of its four channel groups (front pair, center, LFE, surround pair), in dB; mix_l360: -6 / -1 for all.
//     As in the Waves L-series, a signal under the threshold is raised by (ceiling - threshold) (+5 dB) and peaks are
//     held at the ceiling (-1 dBFS) by a linked look-ahead brickwall limiter (1.5 ms look-ahead, 100 ms release),
//     so the master voice is delayed by the look-ahead.
// A state with no preset (or one this file cannot read) passes its audio through. Versions are the retail ones the
// game compares against (k_waves_library_versions at 0x8202b040 and the constants in voice_filter_validate_version).
//
// Export ordinals and the cells they land in (sound_xaudio_xenon_import_waves_procedures: export n goes to the slot
// imports[n - 1] names; slot k is the view's procedures[k], bss 0x828c43dc + 4 k):
//   1 -> 3  get_version (also g_voice_queue_get_effect_version)    2 -> 1  new_filter_state (svf_new_filter_state_procedure)
//   6 -> 6  set_frequency (svf_set_frequency_proc)                  7 -> 7  set_mode (svf_set_mode_proc)
//  11 -> 16 register_filter_state (svf_register_filter_state_proc) 13 -> 18 process (g_state_variable_filter_proc)
//  14 -> 0  set_memory_procedures (g_xaudio_waves_set_memory_procedures)
// The other twelve exports have no caller in the game; they are present (the import loop requires all 19) and do
// nothing.
#include "port_audio.h"
#include "../runtime/port_runtime.h"

#define WAVES_MODULE_MAGIC 0x57415645u // 'WAVE'
#define WAVES_STATE_MAGIC 0x57535446u  // 'WSTF'
#define WAVES_MAX_STATES 64

typedef struct waves_module {
    unsigned magic;
    unsigned index;
    unsigned int version, platform;
} waves_module;

// WavesLibDLL, WaveShell-Xbox, l360, q10 (k_waves_library_versions)
static waves_module g_modules[4] = {
    {WAVES_MODULE_MAGIC, 0, 0x00000301u, 0x6c770004u},
    {WAVES_MODULE_MAGIC, 1, 0x00000301u, 0x73780004u},
    {WAVES_MODULE_MAGIC, 2, 0x00010301u, 0x534c0005u},
    {WAVES_MODULE_MAGIC, 3, 0x00000301u, 0x6e510004u},
};

#define WAVES_MAX_CHANNELS 6
#define WAVES_MAX_BANDS 4
#define WAVES_LOOKAHEAD 72 // 1.5 ms at 48 kHz

enum { k_waves_kind_pass, k_waves_kind_eq, k_waves_kind_limiter };

// A biquad (direct form I coefficients, normalized by a0) and its history per channel.
typedef struct waves_biquad {
    float b0, b1, b2, a1, a2;
    float x1[WAVES_MAX_CHANNELS], x2[WAVES_MAX_CHANNELS], y1[WAVES_MAX_CHANNELS], y2[WAVES_MAX_CHANNELS];
} waves_biquad;

typedef struct waves_state {
    unsigned magic;
    unsigned int version, platform;
    unsigned int sample_rate;
    unsigned int mode;
    double frequency;
    const void *preset;
    int kind;      // k_waves_kind_*: what the preset configured
    int max_bands; // Q10: the component's band count (EQ1M 1, EQ4M 4)
    // Q10
    int band_count;
    waves_biquad bands[WAVES_MAX_BANDS];
    float output_gain;
    // L360
    float limiter_gain[WAVES_MAX_CHANNELS];    // ceiling - threshold, as a factor
    float limiter_ceiling[WAVES_MAX_CHANNELS]; // the output ceiling, as a factor
    float limiter_release;                     // per-sample recovery coefficient of the gain
    float limiter_hold;                        // the gain the look-ahead minimum last settled on
    float limiter_required[WAVES_LOOKAHEAD];   // the gain each sample in the look-ahead needs (ring)
    float limiter_smoothed[WAVES_LOOKAHEAD];   // the held gains averaged over the look-ahead (ring)
    float limiter_smoothed_sum;
    float limiter_delay[WAVES_MAX_CHANNELS][WAVES_LOOKAHEAD];
    int limiter_at;
    unsigned long long limiter_sample_index;
    unsigned long long limiter_deque_idx[128];
    float limiter_deque_val[128];
    unsigned int limiter_deque_head;
    unsigned int limiter_deque_tail;
} waves_state;

static waves_state g_states[WAVES_MAX_STATES];
static volatile int g_state_count;
static void *(PORT_MS_ABI *g_allocate)(int size);
static void (PORT_MS_ABI *g_free)(void *pointer);
static void *(PORT_MS_ABI *g_reallocate)(void *pointer, int size);

static int name_is(const char *name, const char *suffix)
{
    // the game's paths are "game:\..." with either case on the file name; compare the file name only
    const char *file = name;
    for (const char *p = name; *p; p++)
        if (*p == '\\' || *p == '/' || *p == ':')
            file = p + 1;
    for (; *file && *suffix; file++, suffix++) {
        char a = *file >= 'A' && *file <= 'Z' ? (char)(*file - 'A' + 'a') : *file;
        if (a != *suffix)
            return 0;
    }
    return *file == 0 && *suffix == 0;
}

static int starts_with(const char *text, const char *prefix)
{
    while (*prefix)
        if (*text++ != *prefix++)
            return 0;
    return 1;
}

// export 1 of each module: (0, &version, &platform) for the module, (state, ...) for a filter state
#define DEFINE_GET_VERSION(n) \
    static PORT_MS_ABI int waves_get_version_##n(void *object, unsigned int *version, unsigned int *platform) \
    { \
        const waves_state *state = (const waves_state *)object; \
        if (state != 0 && state->magic == WAVES_STATE_MAGIC) { \
            *version = state->version; \
            *platform = state->platform; \
            return 1; \
        } \
        if (object != 0) \
            return 0; \
        *version = g_modules[n].version; \
        *platform = g_modules[n].platform; \
        return 1; \
    }
DEFINE_GET_VERSION(0)
DEFINE_GET_VERSION(1)
DEFINE_GET_VERSION(2)
DEFINE_GET_VERSION(3)

typedef struct waves_channel {
    float *samples;
    int stride;
    int reserved[3];
} waves_channel;

// ---- the plug-ins ----
// Host code without a C math library on every target: the few transcendental values the filter designs need, computed
// once when a preset is registered.

static double waves_exp(double x)
{
    // x = k ln 2 + r, |r| <= ln 2 / 2: e^x = 2^k e^r
    const double ln2 = 0.69314718055994530942;
    int k = (int)(x / ln2 + (x < 0.0 ? -0.5 : 0.5));
    double r = x - k * ln2, term = 1.0, sum = 1.0;
    for (int n = 1; n < 20; n++) {
        term *= r / n;
        sum += term;
    }
    for (; k > 0; k--)
        sum *= 2.0;
    for (; k < 0; k++)
        sum *= 0.5;
    return sum;
}

static double waves_db_to_gain(double db) { return waves_exp(db * 0.11512925464970228420); } // 10^(db / 20)

// sine and cosine of x in [0, pi] (filter angles 2 pi f / rate)
static void waves_sincos(double x, double *s, double *c)
{
    double x2 = x * x, ts = x, tc = 1.0, sum_s = x, sum_c = 1.0;
    for (int n = 1; n < 14; n++) {
        ts *= -x2 / ((2 * n) * (2 * n + 1));
        tc *= -x2 / ((2 * n - 1) * (2 * n));
        sum_s += ts;
        sum_c += tc;
    }
    *s = sum_s;
    *c = sum_c;
}

// The numbers of a preset's <Parameters Type="RealWorld"> list ('*', a setting with no display value, reads as 0).
// Returns the count read, 0 when the document has no such list.
static int waves_preset_values(const char *text, double *values, int capacity)
{
    static const char k_key[] = "Type=\"RealWorld\">";
    const char *p = 0;
    for (const char *t = text; t != 0 && *t && t < text + 65536; t++) {
        int i = 0;
        while (k_key[i] && t[i] == k_key[i])
            i++;
        if (k_key[i] == 0) {
            p = t + i;
            break;
        }
    }
    if (p == 0)
        return 0;
    int count = 0;
    while (*p && *p != '<' && count < capacity) {
        if (*p == ' ' || *p == '\t' || *p == '\r' || *p == '\n') {
            p++;
            continue;
        }
        if (*p == '*') {
            values[count++] = 0.0;
            p++;
            continue;
        }
        double sign = 1.0, value = 0.0, scale = 1.0;
        if (*p == '-' || *p == '+')
            sign = *p++ == '-' ? -1.0 : 1.0;
        int digits = 0;
        for (; *p >= '0' && *p <= '9'; p++, digits++)
            value = value * 10.0 + (*p - '0');
        if (*p == '.')
            for (p++; *p >= '0' && *p <= '9'; p++, digits++)
                value += (*p - '0') * (scale *= 0.1);
        if (digits == 0)
            return 0; // not a number list
        values[count++] = sign * value;
    }
    return count;
}

static void waves_biquad_design(waves_biquad *q, int type, double gain_db, double frequency, double bandwidth, double rate)
{
    double s, c;
    waves_sincos(6.28318530717958647692 * frequency / rate, &s, &c);
    double b0, b1, b2, a0, a1, a2;
    if (type == 0) {
        // bell: RBJ peaking EQ
        double a = waves_exp(gain_db * 0.05756462732485114210); // 10^(gain / 40)
        double alpha = s / (2.0 * bandwidth);
        b0 = 1.0 + alpha * a, b1 = -2.0 * c, b2 = 1.0 - alpha * a;
        a0 = 1.0 + alpha / a, a1 = -2.0 * c, a2 = 1.0 - alpha / a;
    } else {
        // high cut (low-pass) / low cut (high-pass): second-order Butterworth
        double alpha = s * 0.70710678118654752440; // s / (2 Q), Q = 1 / sqrt 2
        double k = type == 3 ? (1.0 - c) * 0.5 : (1.0 + c) * 0.5;
        b0 = k, b1 = type == 3 ? 2.0 * k : -2.0 * k, b2 = k;
        a0 = 1.0 + alpha, a1 = -2.0 * c, a2 = 1.0 - alpha;
    }
    q->b0 = (float)(b0 / a0), q->b1 = (float)(b1 / a0), q->b2 = (float)(b2 / a0);
    q->a1 = (float)(a1 / a0), q->a2 = (float)(a2 / a0);
    for (int ch = 0; ch < WAVES_MAX_CHANNELS; ch++)
        q->x1[ch] = q->x2[ch] = q->y1[ch] = q->y2[ch] = 0.0f;
}

// Q10: the component's bands that are on, then the output gain.
static int waves_configure_q10(waves_state *state, const double *v, int count)
{
    if (count < 103)
        return 0;
    double rate = state->sample_rate ? (double)state->sample_rate : (double)PORT_AUDIO_RATE;
    state->band_count = 0;
    for (int b = 0; b < state->max_bands && b < 10; b++) {
        const double *band = v + 5 * b;
        int type = (int)band[1];
        if (band[0] == 0.0)
            continue;
        if ((type != 0 && type != 3 && type != 4) || !(band[3] > 10.0 && band[3] < rate * 0.49) || !(band[4] > 0.0))
            return 0; // a setting this stand-in does not implement: pass the audio through rather than guess
        waves_biquad_design(&state->bands[state->band_count++], type, band[2], band[3], band[4], rate);
    }
    state->output_gain = (float)waves_db_to_gain(v[102]);
    return 1;
}

// L360: per channel group (front pair, center, LFE, surround pair) the make-up gain and the ceiling.
static int waves_configure_l360(waves_state *state, const double *v, int count)
{
    static const int k_group[WAVES_MAX_CHANNELS] = {0, 0, 1, 2, 3, 3};
    if (count < 9)
        return 0;
    double rate = state->sample_rate ? (double)state->sample_rate : (double)PORT_AUDIO_RATE;
    for (int c = 0; c < WAVES_MAX_CHANNELS; c++) {
        double threshold = v[1 + k_group[c]], ceiling = v[5 + k_group[c]];
        if (!(threshold <= 0.0 && threshold >= -30.0 && ceiling <= 0.0 && ceiling >= -30.0))
            return 0;
        state->limiter_gain[c] = (float)waves_db_to_gain(ceiling - threshold);
        state->limiter_ceiling[c] = (float)waves_db_to_gain(ceiling);
    }
    state->limiter_release = (float)waves_exp(-1.0 / (0.1 * rate)); // 100 ms
    state->limiter_hold = 1.0f;
    for (int i = 0; i < WAVES_LOOKAHEAD; i++) {
        state->limiter_required[i] = state->limiter_smoothed[i] = 1.0f;
        for (int c = 0; c < WAVES_MAX_CHANNELS; c++)
            state->limiter_delay[c][i] = 0.0f;
    }
    state->limiter_smoothed_sum = (float)WAVES_LOOKAHEAD;
    state->limiter_at = 0;
    state->limiter_sample_index = 0;
    state->limiter_deque_head = 0;
    state->limiter_deque_tail = 0;
    return 1;
}

static float waves_flush(float x) { return x > -1.0e-20f && x < 1.0e-20f ? 0.0f : x; }

static void waves_run_eq(waves_state *state, int channels, int samples, waves_channel *io)
{
    for (int c = 0; c < channels && c < WAVES_MAX_CHANNELS; c++) {
        float *x = io[c].samples;
        int stride = io[c].stride ? io[c].stride : 1;
        if (x == 0)
            continue;
        for (int b = 0; b < state->band_count; b++) {
            waves_biquad *q = &state->bands[b];
            float x1 = q->x1[c], x2 = q->x2[c], y1 = q->y1[c], y2 = q->y2[c];
            for (int i = 0; i < samples; i++) {
                float in = x[i * stride];
                float y = q->b0 * in + q->b1 * x1 + q->b2 * x2 - q->a1 * y1 - q->a2 * y2;
                x2 = x1, x1 = in, y2 = y1, y1 = waves_flush(y);
                x[i * stride] = y1;
            }
            q->x1[c] = x1, q->x2[c] = x2, q->y1[c] = y1, q->y2[c] = y2;
        }
        for (int i = 0; i < samples; i++)
            x[i * stride] *= state->output_gain;
    }
}

// A linked look-ahead brickwall limiter: each sample's required gain (the most any channel can take, after its make-up
// gain, under its ceiling) enters a WAVES_LOOKAHEAD window; the window's minimum, released exponentially, is averaged
// over another WAVES_LOOKAHEAD samples, and the audio, delayed WAVES_LOOKAHEAD - 1 samples, takes that gain: at a peak
// every averaged value is at most the gain the peak needs, so no sample exceeds its ceiling, with no step in the gain.
static void waves_run_limiter(waves_state *state, int channels, int samples, waves_channel *io)
{
    if (channels > WAVES_MAX_CHANNELS)
        channels = WAVES_MAX_CHANNELS;
    unsigned int head = state->limiter_deque_head;
    unsigned int tail = state->limiter_deque_tail;
    unsigned long long sample_idx = state->limiter_sample_index;
    for (int i = 0; i < samples; i++) {
        int at = state->limiter_at;
        float need = 1.0f;
        for (int c = 0; c < channels; c++) {
            if (io[c].samples == 0)
                continue;
            float x = io[c].samples[i * (io[c].stride ? io[c].stride : 1)];
            x = x == x ? x : 0.0f;
            state->limiter_delay[c][at] = x;
            float level = (x < 0.0f ? -x : x) * state->limiter_gain[c];
            if (level > state->limiter_ceiling[c] && state->limiter_ceiling[c] / level < need)
                need = state->limiter_ceiling[c] / level;
        }
        state->limiter_required[at] = need;
        unsigned long long idx = sample_idx++;
        while (head != tail && idx - state->limiter_deque_idx[head] >= (unsigned long long)WAVES_LOOKAHEAD)
            head = (head + 1u) & 127u;
        while (head != tail && state->limiter_deque_val[(tail - 1u) & 127u] >= need)
            tail = (tail - 1u) & 127u;
        state->limiter_deque_idx[tail] = idx;
        state->limiter_deque_val[tail] = need;
        tail = (tail + 1u) & 127u;
        float minimum = state->limiter_deque_val[head];
        float hold = state->limiter_hold;
        hold = minimum < hold ? minimum : minimum + (hold - minimum) * state->limiter_release;
        state->limiter_hold = hold;
        state->limiter_smoothed_sum += hold - state->limiter_smoothed[at];
        state->limiter_smoothed[at] = hold;
        float gain = state->limiter_smoothed_sum * (1.0f / WAVES_LOOKAHEAD);
        int out = at + 1 == WAVES_LOOKAHEAD ? 0 : at + 1; // the sample WAVES_LOOKAHEAD - 1 ago
        for (int c = 0; c < channels; c++) {
            if (io[c].samples == 0)
                continue;
            float y = state->limiter_delay[c][out] * state->limiter_gain[c] * gain, ceiling = state->limiter_ceiling[c];
            io[c].samples[i * (io[c].stride ? io[c].stride : 1)] = y > ceiling ? ceiling : y < -ceiling ? -ceiling : y;
        }
        state->limiter_at = out;
    }
    state->limiter_deque_head = head;
    state->limiter_deque_tail = tail;
    state->limiter_sample_index = sample_idx;
    // the running sum drifts with rounding: rebuild it once a frame
    float sum = 0.0f;
    for (int k = 0; k < WAVES_LOOKAHEAD; k++)
        sum += state->limiter_smoothed[k];
    state->limiter_smoothed_sum = sum;
}

// export 2: a filter state for a plug-in ("L360,L3M6", "Q10,EQ1M", "Q10,EQ4M")
static PORT_MS_ABI void *waves_new_filter_state(const char *name, unsigned int sample_rate, unsigned int flags)
{
    (void)flags;
    if (name == 0)
        return 0;
    int index = __atomic_fetch_add(&g_state_count, 1, __ATOMIC_ACQ_REL);
    if (index >= WAVES_MAX_STATES)
        return 0;
    waves_state *state = &g_states[index];
    const waves_module *module = starts_with(name, "L360") ? &g_modules[2] : &g_modules[3];
    state->version = module->version;
    state->platform = module->platform;
    state->sample_rate = sample_rate;
    state->mode = 0;
    state->frequency = 500.0;
    state->preset = 0;
    state->kind = k_waves_kind_pass;
    state->max_bands = starts_with(name, "Q10,EQ4") ? 4 : 1;
    state->magic = WAVES_STATE_MAGIC;
    return state;
}

// export 6 / 7
static PORT_MS_ABI void waves_set_frequency(void *object, unsigned int mode, double frequency)
{
    waves_state *state = (waves_state *)object;
    if (state != 0 && state->magic == WAVES_STATE_MAGIC) {
        state->mode = mode;
        state->frequency = frequency;
    }
}

static PORT_MS_ABI void waves_set_mode(void *object, unsigned int mode, double *frequency)
{
    waves_state *state = (waves_state *)object;
    if (state != 0 && state->magic == WAVES_STATE_MAGIC) {
        state->mode = mode;
        if (frequency != 0)
            state->frequency = *frequency;
    }
}

// export 11: the preset from the game's XEX section ("mix_l360", "obstr_qn", "occl_qn", "radio_qn")
static PORT_MS_ABI int waves_register_filter_state(void *object, const void *preset, unsigned int flags)
{
    (void)flags;
    waves_state *state = (waves_state *)object;
    if (state == 0 || state->magic != WAVES_STATE_MAGIC)
        return 0;
    state->preset = preset;
    double values[128];
    int count = preset != 0 ? waves_preset_values((const char *)preset, values, 128) : 0;
    state->kind = k_waves_kind_pass;
    if (state->platform == g_modules[2].platform) {
        if (waves_configure_l360(state, values, count))
            state->kind = k_waves_kind_limiter;
    } else if (waves_configure_q10(state, values, count)) {
        state->kind = k_waves_kind_eq;
    }
    if (state->kind == k_waves_kind_pass)
        port_write_error("port: audio: a Waves preset this stand-in cannot read; its effect passes the audio through\n");
    return 1;
}

// export 13: process `in_samples` samples of `in_channels` channels into the output channels (the game passes the
// same channels as both: in place).
static PORT_MS_ABI void waves_process(void *object, int in_channels, int in_samples, waves_channel *in, int out_channels,
    int out_samples, void *output, waves_channel *out, int flags)
{
    (void)output; (void)flags;
    waves_state *state = (waves_state *)object;
    if (in == 0 || out == 0)
        return;
    int channels = in_channels < out_channels ? in_channels : out_channels;
    int samples = in_samples < out_samples ? in_samples : out_samples;
    if (in != out) {
        for (int c = 0; c < channels; c++) {
            if (in[c].samples == out[c].samples || in[c].samples == 0 || out[c].samples == 0)
                continue;
            int is = in[c].stride ? in[c].stride : 1, os = out[c].stride ? out[c].stride : 1;
            for (int i = 0; i < samples; i++)
                out[c].samples[i * os] = in[c].samples[i * is];
        }
    }
    if (state == 0 || state->magic != WAVES_STATE_MAGIC)
        return;
    if (state->kind == k_waves_kind_eq)
        waves_run_eq(state, channels, samples, out);
    else if (state->kind == k_waves_kind_limiter)
        waves_run_limiter(state, channels, samples, out);
}

// export 14
static PORT_MS_ABI void waves_set_memory_procedures(void *(PORT_MS_ABI *allocate)(int size), void (PORT_MS_ABI *release)(void *pointer),
    void *(PORT_MS_ABI *reallocate)(void *pointer, int size))
{
    g_allocate = allocate;
    g_free = release;
    g_reallocate = reallocate;
}

// the exports with no caller in the game
static PORT_MS_ABI int waves_unused(void) { return 0; }

static void *const k_wave_shell_exports[19] = {
    (void *)waves_get_version_1,          // 1
    (void *)waves_new_filter_state,       // 2
    (void *)waves_unused,                 // 3
    (void *)waves_unused,                 // 4
    (void *)waves_unused,                 // 5
    (void *)waves_set_frequency,          // 6
    (void *)waves_set_mode,               // 7
    (void *)waves_unused,                 // 8
    (void *)waves_unused,                 // 9
    (void *)waves_unused,                 // 10
    (void *)waves_register_filter_state,  // 11
    (void *)waves_unused,                 // 12
    (void *)waves_process,                // 13
    (void *)waves_set_memory_procedures,  // 14
    (void *)waves_unused,                 // 15
    (void *)waves_unused,                 // 16
    (void *)waves_unused,                 // 17
    (void *)waves_unused,                 // 18
    (void *)waves_unused,                 // 19
};

void *port_waves_load_library(const char *name)
{
    if (name == 0)
        return 0;
    if (name_is(name, "waveslibdll.dll"))
        return &g_modules[0];
    if (name_is(name, "waveshell-xbox.dll"))
        return &g_modules[1];
    if (name_is(name, "l360.dll"))
        return &g_modules[2];
    if (name_is(name, "q10.dll"))
        return &g_modules[3];
    return 0;
}

void *port_waves_get_procedure(void *handle, const char *name)
{
    const waves_module *module = (const waves_module *)handle;
    if (module == 0 || module->magic != WAVES_MODULE_MAGIC || module->index > 3)
        return 0;
    __UINTPTR_TYPE__ ordinal = (__UINTPTR_TYPE__)name;
    if (ordinal == 0 || ordinal >= 0x10000)
        return 0; // the game imports by ordinal only
    if (module->index == 1)
        return ordinal <= 19 ? k_wave_shell_exports[ordinal - 1] : 0;
    // the other modules: only their version export is used
    if (ordinal != 1)
        return 0;
    switch (module->index) {
    case 0: return (void *)waves_get_version_0;
    case 2: return (void *)waves_get_version_2;
    default: return (void *)waves_get_version_3;
    }
}
