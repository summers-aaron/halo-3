// port/sdk/xhv (both hosts): the voice runtime (interface and threads: xhv_voice.h; host services: xhv_host.h).
//
// Local side (the capture thread): 20 ms frames of 16 kHz PCM from the microphone (or the test voice) through a
// voice activity detector -- frame level against a tracked noise floor, two loud frames to start, 12 dB over the
// floor to start and 6 dB to stay, 300 ms of hangover -- so silence is never sent. A talk spurt starts with the
// 60 ms before its onset (so the first syllable is not clipped) and a codec reset; while it lasts each frame becomes
// two chat packets in the user's queue, which GetLocalChatData drains.
//
// Remote side (the XAudio thread, one call per 20 ms the talker's voice plays): packets queued by the game's thread
// are paired into frames by their header (half, seq) and held in a jitter buffer. A spurt starts playing once 60 ms
// is buffered (20 ms more after a spurt that ran dry, up to 120 ms; 20 ms less after 4 that did not), frames play in
// seq order, a frame that is missing or late is concealed (the decoder's extrapolation, faded to silence over
// 100 ms), frames that arrive after their turn are dropped, and a buffer that holds more than it needs (its low point
// over a second above 40 ms, or 240 ms at any time) is decoded ahead without playing to bring the delay back.
#include "xhv_voice.h"
#include "xhv_host.h"
#include "xhv_codec.h"

#include <stdint.h>

#define FRAME XHV_VOICE_FRAME_SAMPLES
#define PACKET XHV_VOICE_PACKET_BYTES
#define LOCAL_RING 64     // packets (640 ms)
#define LOCAL_KEEP 16     // packets a read keeps at most (160 ms)
#define REMOTE_RING 64    // packets
#define JITTER_FRAMES 16  // frames (320 ms)
#define PREROLL_FRAMES 3
#define VAD_ONSET 2
#define VAD_HANGOVER 15
#define CONCEAL_LIMIT 5   // frames concealed before a talker goes idle (100 ms, the decoder's fade)
#define PREBUFFER_MIN 3
#define PREBUFFER_MAX 6
#define CLEAN_SPURTS 4    // spurts without an underrun before the pre-buffer shrinks again
#define SLACK_WINDOW 50   // renders (1 s) over which the buffer's lowest depth is watched
#define SLACK_FRAMES 2    // frames the buffer may hold at its lowest; more is latency to give back (40 ms)
#define LATENCY_HARD 12   // frames beyond which frames are decoded ahead at once (240 ms)
#define TALK_HOLD 10      // frames a decoded frame keeps the talker "talking" (200 ms)

static unsigned atomic_load(const volatile unsigned *p) { return __atomic_load_n(p, __ATOMIC_ACQUIRE); }
static void atomic_store(volatile unsigned *p, unsigned v) { __atomic_store_n(p, v, __ATOMIC_RELEASE); }

static void copy_bytes(unsigned char *to, const unsigned char *from, unsigned count)
{
    for (unsigned i = 0; i < count; i++)
        to[i] = from[i];
}

// ---- log lines without a C library ----------------------------------------------------------------------------------

typedef struct {
    char text[256]; // the longest, remote_trace with every count at 10 digits, is 243 characters
    unsigned length;
} line;

static void line_add(line *l, const char *s)
{
    while (*s && l->length + 2 < sizeof(l->text))
        l->text[l->length++] = *s++;
}

static void line_number(line *l, unsigned v)
{
    char digits[12];
    int n = 0;
    do {
        digits[n++] = (char)('0' + v % 10u);
        v /= 10u;
    } while (v != 0);
    while (n > 0 && l->length + 2 < sizeof(l->text))
        l->text[l->length++] = digits[--n];
}

static void line_end(line *l)
{
    l->text[l->length++] = '\n';
    l->text[l->length] = 0;
    xhv_host_log(l->text);
}

static int g_trace;

// ---- environment ----------------------------------------------------------------------------------------------------

static int text_equals(const char *a, const char *b)
{
    while (*a && *a == *b) {
        a++;
        b++;
    }
    return *a == *b;
}

static int env_is(const char *name, const char *value)
{
    const char *v = xhv_host_getenv(name);
    return v != 0 && text_equals(v, value);
}

// Whether a microphone may be opened: never when the run's audio goes nowhere or to a file (headless and test runs).
static int host_audio_allowed(void)
{
    if (env_is("HALO3_NO_AUDIO", "1"))
        return 0;
    const char *backend = xhv_host_getenv("HALO3_AUDIO_BACKEND");
    if (backend == 0 || *backend == 0)
        return 1;
    return text_equals(backend, "pulse") || text_equals(backend, "alsa") || text_equals(backend, "pipe") ||
           text_equals(backend, "winmm");
}

// ---- the test voice: a glottal pulse train through three formant resonators, syllables and pauses ------------------

static double tv_sin(double x) // |x| <= 2 pi is enough here
{
    const double pi = 3.14159265358979323846;
    while (x > pi)
        x -= 2.0 * pi;
    while (x < -pi)
        x += 2.0 * pi;
    double term = x, sum = x;
    for (int n = 1; n <= 12; n++) {
        term *= -x * x / (double)((2 * n) * (2 * n + 1));
        sum += term;
    }
    return sum;
}

static double tv_exp(double x) // x in [-1, 0]
{
    double term = 1.0, sum = 1.0;
    for (int n = 1; n <= 20; n++) {
        term *= x / n;
        sum += term;
    }
    return sum;
}

#define TV_VOWELS 5
static const float k_formants[TV_VOWELS][3] = {
    {730.0f, 1090.0f, 2440.0f}, {270.0f, 2290.0f, 3010.0f}, {300.0f, 870.0f, 2240.0f}, {530.0f, 1840.0f, 2480.0f},
    {570.0f, 840.0f, 2410.0f}};

static struct {
    float b1[TV_VOWELS][3], b2[TV_VOWELS][3], gain[TV_VOWELS][3];
    float y1[3], y2[3];
    double phase;
    unsigned sample;
    unsigned noise;
    float glottal; // the pulse train, low-passed (a crude glottal shape)
} g_tone;

static void tone_init(void)
{
    unsigned char *tp = (unsigned char *)&g_tone;
    for (unsigned i = 0; i < sizeof(g_tone); i++)
        tp[i] = 0;
    const double pi = 3.14159265358979323846;
    for (int v = 0; v < TV_VOWELS; v++)
        for (int f = 0; f < 3; f++) {
            double bandwidth = 60.0 + 30.0 * f;
            double r = tv_exp(-pi * bandwidth / XHV_VOICE_RATE);
            double theta = 2.0 * pi * k_formants[v][f] / XHV_VOICE_RATE;
            g_tone.b1[v][f] = (float)(2.0 * r * tv_sin(theta + pi / 2.0));
            g_tone.b2[v][f] = (float)(-r * r);
            g_tone.gain[v][f] = (float)(1.0 - r);
        }
    g_tone.noise = 22222u;
}

// 3 s cycles: 2 s of ten syllables (each a vowel with a raised-cosine envelope), then 1 s of near silence; the pitch
// glides around 120 Hz, and every other cycle around 210 Hz.
static void tone_frame(short *pcm)
{
    const double pi = 3.14159265358979323846;
    for (int n = 0; n < FRAME; n++) {
        unsigned t = g_tone.sample++;
        unsigned cycle = t / (3u * XHV_VOICE_RATE), in_cycle = t % (3u * XHV_VOICE_RATE);
        double seconds = (double)t / XHV_VOICE_RATE;
        g_tone.noise = g_tone.noise * 1664525u + 1013904223u;
        float noise = (float)((int)(g_tone.noise >> 16) - 32768) / 32768.0f * 8.0f; // -72 dBFS
        float out = noise;
        if (in_cycle < 2u * XHV_VOICE_RATE) {
            unsigned syllable = in_cycle / (XHV_VOICE_RATE / 5u), position = in_cycle % (XHV_VOICE_RATE / 5u);
            double envelope = 0.5 - 0.5 * tv_sin(2.0 * pi * position / (XHV_VOICE_RATE / 5.0) + pi / 2.0);
            double vibrato = 0.7 * seconds;
            vibrato -= (double)(long)vibrato;
            double f0 = ((cycle & 1u) ? 210.0 : 120.0) * (1.0 + 0.08 * tv_sin(2.0 * pi * vibrato));
            g_tone.phase += f0 / XHV_VOICE_RATE;
            float pulse = 0.0f;
            if (g_tone.phase >= 1.0) {
                g_tone.phase -= 1.0;
                pulse = 1.0f;
            }
            g_tone.glottal = 0.9f * g_tone.glottal + pulse;
            int v = (int)((syllable + cycle * 3u) % TV_VOWELS);
            float voiced = 0.0f;
            for (int f = 0; f < 3; f++) {
                float y = g_tone.gain[v][f] * g_tone.glottal + g_tone.b1[v][f] * g_tone.y1[f] +
                          g_tone.b2[v][f] * g_tone.y2[f];
                g_tone.y2[f] = g_tone.y1[f];
                g_tone.y1[f] = y;
                voiced += y * (f == 0 ? 1.0f : f == 1 ? 0.6f : 0.35f);
            }
            out += (float)envelope * voiced * 9000.0f;
        }
        pcm[n] = (short)(out > 32767.0f ? 32767 : out < -32768.0f ? -32768 : (int)out);
    }
}

// ---- local talkers --------------------------------------------------------------------------------------------------

typedef struct {
    unsigned char packets[LOCAL_RING][PACKET];
    volatile unsigned write, read; // write: the capture thread's; read: the game thread's
    volatile unsigned enabled;     // the game thread's
    volatile unsigned talking;     // the capture thread's
} local_talker;

static local_talker g_local[XHV_VOICE_USERS];

enum { k_source_none, k_source_microphone, k_source_test_voice };

static volatile char g_capture_lock;
static void capture_lock(void)
{
    while (__atomic_test_and_set(&g_capture_lock, __ATOMIC_ACQUIRE))
        __builtin_ia32_pause();
}
static void capture_unlock(void) { __atomic_clear(&g_capture_lock, __ATOMIC_RELEASE); }

static void copy_text(char *dst, unsigned cap, const char *src)
{
    if (cap == 0)
        return;
    unsigned i = 0;
    if (src != 0) {
        for (; src[i] != 0 && i + 1 < cap; i++)
            dst[i] = src[i];
    }
    dst[i] = 0;
}

static struct {
    int opened, running, source;
    volatile unsigned stop, failed;
    volatile unsigned headset_mask;
    char device_id[XHV_VOICE_DEVICE_ID_MAX];
    xhv_encoder encoder;
    // the detector (capture thread)
    float dc_x, dc_y, floor_db;
    int onset, hangover, active, spurt_frames;
    unsigned enabled_seen;
    short history[PREROLL_FRAMES][FRAME];
    unsigned history_count;
    unsigned spurts;
    volatile unsigned packets_sent;
} g_capture;

// 10 log10 of the mean square, within 0.1 dB: log2 from the float's exponent and a quadratic in the mantissa
static float level_db(const float *x, int count)
{
    float sum = 1.0f;
    for (int i = 0; i < count; i++)
        sum += x[i] * x[i];
    sum /= (float)count;
    union {
        float f;
        uint32_t i;
    } u;
    u.f = sum;
    float e = (float)((int)((u.i >> 23) & 255u) - 127);
    u.i = (u.i & 0x007fffffu) | 0x3f800000u;
    float m = u.f; // [1, 2)
    float log2m = (m - 1.0f) * (1.3465f - 0.3465f * (m - 1.0f));
    return 3.0103f * (e + log2m);
}

static void local_push_frame(local_talker *talker, const short *pcm, int reset)
{
    unsigned char packets[XHV_CODEC_FRAME_BYTES];
    xhv_encoder_encode(&g_capture.encoder, pcm, reset, packets);
    unsigned write = talker->write, read = atomic_load(&talker->read);
    if (write - read + 2u > LOCAL_RING)
        return; // the game has not polled for 640 ms: this frame is lost, the queue keeps what it has
    copy_bytes(talker->packets[write % LOCAL_RING], packets, PACKET);
    copy_bytes(talker->packets[(write + 1u) % LOCAL_RING], packets + PACKET, PACKET);
    atomic_store(&talker->write, write + 2u);
    atomic_store(&g_capture.packets_sent, g_capture.packets_sent + 2u);
}

// One 20 ms frame of user 0's source (the capture thread).
static void local_process(const short *pcm)
{
    local_talker *talker = &g_local[0];
    float x[FRAME];
    for (int n = 0; n < FRAME; n++) {
        float in = (float)pcm[n];
        g_capture.dc_y = in - g_capture.dc_x + 0.995f * g_capture.dc_y;
        g_capture.dc_x = in;
        x[n] = g_capture.dc_y;
    }
    float level = level_db(x, FRAME);
    if (level < g_capture.floor_db)
        g_capture.floor_db = 0.5f * g_capture.floor_db + 0.5f * level;
    else
        g_capture.floor_db += 0.05f; // 2.5 dB a second: a steady noise becomes the floor within seconds
    if (g_capture.floor_db < 20.0f)
        g_capture.floor_db = 20.0f;
    int loud = level > g_capture.floor_db + 12.0f && level > 45.0f;
    int voiced = level > g_capture.floor_db + 6.0f && level > 40.0f;
    int was_active = g_capture.active;
    if (g_capture.active) {
        if (voiced) {
            g_capture.hangover = VAD_HANGOVER;
        } else if (--g_capture.hangover <= 0) {
            g_capture.active = 0;
        }
    } else {
        g_capture.onset = loud ? g_capture.onset + 1 : 0;
        if (g_capture.onset >= VAD_ONSET) {
            g_capture.active = 1;
            g_capture.hangover = VAD_HANGOVER;
            g_capture.onset = 0;
        }
    }
    atomic_store(&talker->talking, (unsigned)g_capture.active);

    unsigned enabled = atomic_load(&talker->enabled);
    if (g_capture.active && enabled) {
        if (!was_active || !g_capture.enabled_seen) {
            // a new spurt: the pre-roll first, the codec reset on its first frame
            int reset = 1;
            for (unsigned h = 0; h < g_capture.history_count; h++) {
                local_push_frame(talker, g_capture.history[h], reset);
                reset = 0;
            }
            local_push_frame(talker, pcm, reset);
            g_capture.spurts++;
            g_capture.spurt_frames = 0;
            if (g_trace) {
                line l = {{0}, 0};
                line_add(&l, "xhv: local talker 0 spurt ");
                line_number(&l, g_capture.spurts);
                line_add(&l, " starts (floor ");
                line_number(&l, (unsigned)g_capture.floor_db);
                line_add(&l, " dB, level ");
                line_number(&l, (unsigned)level);
                line_add(&l, " dB)");
                line_end(&l);
            }
        } else {
            local_push_frame(talker, pcm, 0);
        }
        g_capture.spurt_frames++;
    } else if (was_active && g_capture.enabled_seen && g_trace) {
        line l = {{0}, 0};
        line_add(&l, "xhv: local talker 0 spurt ");
        line_number(&l, g_capture.spurts);
        line_add(&l, " ends after ");
        line_number(&l, (unsigned)g_capture.spurt_frames);
        line_add(&l, " frames (");
        line_number(&l, g_capture.packets_sent);
        line_add(&l, " packets queued in all)");
        line_end(&l);
    }
    g_capture.enabled_seen = g_capture.active && enabled;

    // the pre-roll: the last frames before this one
    if (g_capture.history_count == PREROLL_FRAMES) {
        for (unsigned h = 0; h + 1 < PREROLL_FRAMES; h++)
            for (int n = 0; n < FRAME; n++)
                g_capture.history[h][n] = g_capture.history[h + 1][n];
        g_capture.history_count--;
    }
    for (int n = 0; n < FRAME; n++)
        g_capture.history[g_capture.history_count][n] = pcm[n];
    g_capture.history_count++;
}

static void capture_main(void)
{
    short pcm[FRAME];
    unsigned next = xhv_host_time_ms();
    while (!atomic_load(&g_capture.stop)) {
        if (g_capture.source == k_source_microphone) {
            int result = xhv_host_capture_read(pcm, FRAME);
            if (result < 0) {
                xhv_host_log("xhv: the microphone failed; voice capture stops\n");
                atomic_store(&g_local[0].talking, 0u);
                atomic_store(&g_capture.failed, 1u);
                return;
            }
            if (result == 0)
                continue;
        } else {
            // the test voice runs at the wall clock's pace, as a device would
            unsigned now = xhv_host_time_ms();
            if ((int)(next - now) > 0) {
                xhv_host_sleep_ms(next - now);
                continue;
            }
            next += 20u;
            if ((int)(now - next) > 200)
                next = now; // fell far behind (a suspended process): no burst of stale frames
            tone_frame(pcm);
        }
        local_process(pcm);
    }
}

static void stop_capture_locked(void)
{
    if (g_capture.running) {
        atomic_store(&g_capture.stop, 1u);
        xhv_host_thread_join();
        g_capture.running = 0;
    }
    if (g_capture.source == k_source_microphone)
        xhv_host_capture_close();
    g_capture.source = k_source_none;
    g_capture.device_id[0] = 0;
    atomic_store(&g_capture.headset_mask, 0u);
    atomic_store(&g_capture.failed, 0u);
    atomic_store(&g_local[0].talking, 0u);
}

static void apply_source_locked(int initial)
{
    int desired_source = k_source_none;
    char desired_id[XHV_VOICE_DEVICE_ID_MAX];
    desired_id[0] = 0;

    if (env_is("HALO3_VOICE", "0")) {
        if (initial || g_capture.source != k_source_none)
            xhv_host_log("xhv: voice off (HALO3_VOICE=0)\n");
    } else if (env_is("HALO3_VOICE_TEST_TONE", "1")) {
        desired_source = k_source_test_voice;
    } else {
        char configured[XHV_VOICE_DEVICE_ID_MAX];
        configured[0] = 0;
        xhv_host_mic_device(configured, sizeof(configured));
        if ((configured[0] == 0 || text_equals(configured, "off")) && env_is("HALO3_VOICE", "1"))
            copy_text(configured, sizeof(configured), "default");
        if (configured[0] != 0 && !text_equals(configured, "off")) {
            if (!host_audio_allowed()) {
                if (initial || g_capture.source != k_source_none)
                    xhv_host_log("xhv: microphone ignored: the audio backend is null / a file (no microphone)\n");
            } else {
                desired_source = k_source_microphone;
                copy_text(desired_id, sizeof(desired_id), configured);
            }
        }
    }

    if (!initial && !atomic_load(&g_capture.failed) && g_capture.source == desired_source &&
        text_equals(g_capture.device_id, desired_id))
        return;

    stop_capture_locked();

    atomic_store(&g_capture.stop, 0u);
    atomic_store(&g_capture.failed, 0u);
    xhv_encoder_init(&g_capture.encoder); // also builds the codec's tables before any other thread uses them
    g_capture.dc_x = 0.0f;
    g_capture.dc_y = 0.0f;
    g_capture.floor_db = 50.0f;
    g_capture.onset = 0;
    g_capture.hangover = 0;
    g_capture.active = 0;
    g_capture.spurt_frames = 0;
    g_capture.enabled_seen = 0;
    g_capture.history_count = 0;

    if (desired_source == k_source_test_voice) {
        tone_init();
        g_capture.source = k_source_test_voice;
        xhv_host_log("xhv: player 1's headset speaks the test voice (HALO3_VOICE_TEST_TONE=1)\n");
    } else if (desired_source == k_source_microphone) {
        const char *backend = 0;
        if (xhv_host_capture_open(desired_id, &backend)) {
            g_capture.source = k_source_microphone;
            copy_text(g_capture.device_id, sizeof(g_capture.device_id), desired_id);
            line l = {{0}, 0};
            if (text_equals(desired_id, "default")) {
                line_add(&l, "xhv: player 1's headset is the default microphone (");
            } else {
                line_add(&l, "xhv: player 1's headset is microphone [");
                line_add(&l, desired_id);
                line_add(&l, "] (");
            }
            line_add(&l, backend);
            line_add(&l, ", 16 kHz)");
            line_end(&l);
        }
    }

    if (g_capture.source != k_source_none) {
        if (xhv_host_thread_start(capture_main)) {
            g_capture.running = 1;
            atomic_store(&g_capture.headset_mask, 1u);
        } else {
            xhv_host_log("xhv: no capture thread; voice off\n");
            if (g_capture.source == k_source_microphone)
                xhv_host_capture_close();
            g_capture.source = k_source_none;
            g_capture.device_id[0] = 0;
        }
    }
}

unsigned PORT_MS_ABI xhv_voice_devices(xhv_voice_device *out, unsigned capacity)
{
    if (out == 0 || capacity == 0 || !host_audio_allowed())
        return 0;
    xhv_capture_device found[XHV_MAX_DEVICES];
    unsigned limit = capacity > XHV_MAX_DEVICES ? XHV_MAX_DEVICES : capacity;
    unsigned count = xhv_host_capture_devices(found, limit);
    if (count > limit)
        count = limit;
    for (unsigned i = 0; i < count; i++) {
        copy_text(out[i].id, sizeof(out[i].id), found[i].id);
        copy_text(out[i].name, sizeof(out[i].name), found[i].name);
    }
    return count;
}

unsigned PORT_MS_ABI xhv_voice_open(void)
{
    capture_lock();
    if (g_capture.opened) {
        unsigned mask = atomic_load(&g_capture.headset_mask);
        capture_unlock();
        return mask;
    }
    g_capture.opened = 1;
    g_trace = env_is("HALO3_VOICE_TRACE", "1");
    apply_source_locked(1);
    unsigned mask = atomic_load(&g_capture.headset_mask);
    capture_unlock();
    return mask;
}

unsigned PORT_MS_ABI xhv_voice_refresh(void)
{
    capture_lock();
    if (!g_capture.opened) {
        capture_unlock();
        return 0;
    }
    apply_source_locked(0);
    unsigned mask = atomic_load(&g_capture.headset_mask);
    capture_unlock();
    return mask;
}

void PORT_MS_ABI xhv_voice_close(void)
{
    capture_lock();
    if (!g_capture.opened) {
        capture_unlock();
        return;
    }
    stop_capture_locked();
    unsigned char *p = (unsigned char *)&g_capture;
    for (unsigned i = 0; i < sizeof(g_capture); i++)
        p[i] = 0;
    for (unsigned u = 0; u < XHV_VOICE_USERS; u++) {
        atomic_store(&g_local[u].talking, 0u);
        atomic_store(&g_local[u].enabled, 0u);
        atomic_store(&g_local[u].read, atomic_load(&g_local[u].write));
    }
    capture_unlock();
}

unsigned PORT_MS_ABI xhv_voice_headset_mask(void) { return atomic_load(&g_capture.headset_mask); }

unsigned PORT_MS_ABI xhv_voice_local_packets_sent(void) { return atomic_load(&g_capture.packets_sent); }

void PORT_MS_ABI xhv_voice_local_enable(unsigned user, int enabled)
{
    if (user < XHV_VOICE_USERS)
        atomic_store(&g_local[user].enabled, enabled ? 1u : 0u);
}

int PORT_MS_ABI xhv_voice_local_talking(unsigned user)
{
    return user < XHV_VOICE_USERS && atomic_load(&g_local[user].talking) != 0;
}

unsigned PORT_MS_ABI xhv_voice_local_packets(unsigned user)
{
    if (user >= XHV_VOICE_USERS)
        return 0;
    local_talker *talker = &g_local[user];
    return atomic_load(&talker->write) - talker->read;
}

unsigned PORT_MS_ABI xhv_voice_local_read(unsigned user, unsigned char *data, unsigned max_packets)
{
    if (user >= XHV_VOICE_USERS)
        return 0;
    local_talker *talker = &g_local[user];
    unsigned write = atomic_load(&talker->write), read = talker->read;
    if (write - read > LOCAL_KEEP)
        read = write - LOCAL_KEEP; // whole frames: both counts are even
    unsigned count = 0;
    while (count < max_packets && read != write) {
        copy_bytes(data + count * PACKET, talker->packets[read % LOCAL_RING], PACKET);
        read++;
        count++;
    }
    atomic_store(&talker->read, read);
    return count;
}

// ---- remote talkers -------------------------------------------------------------------------------------------------

typedef struct {
    // the game thread's
    unsigned char ring[REMOTE_RING][PACKET];
    volatile unsigned write, read;
    volatile unsigned last_arrival; // ms
    volatile unsigned open;
    // the renderer's
    volatile unsigned talking, last_render; // ms
    xhv_decoder decoder;
    unsigned char pending[PACKET];
    int have_pending;
    unsigned char frames[JITTER_FRAMES][XHV_CODEC_FRAME_BYTES];
    unsigned frame_head, frame_count;
    int playing;
    unsigned expected, concealed, waited, prebuffer, hold;
    int underrun; // a frame of this spurt came after its turn was concealed
    unsigned clean_spurts; // spurts in a row without one
    unsigned window_renders, window_low; // the slack watch
    unsigned spurt_start_ms, spurt_renders; // the trace's playback rate
    unsigned decoded, concealed_total, unpaired, overflowed, skipped, late, lead_in; // the trace's counts
    volatile unsigned packets_received, decoded_total;
} remote_talker;

static remote_talker g_remote[XHV_VOICE_TALKERS];

void PORT_MS_ABI xhv_voice_remote_open(unsigned slot)
{
    if (slot >= XHV_VOICE_TALKERS)
        return;
    remote_talker *t = &g_remote[slot];
    unsigned char *p = (unsigned char *)t;
    for (unsigned i = 0; i < sizeof(*t); i++)
        p[i] = 0;
    xhv_decoder_init(&t->decoder);
    t->prebuffer = PREBUFFER_MIN;
    atomic_store(&t->open, 1u);
}

void PORT_MS_ABI xhv_voice_remote_close(unsigned slot)
{
    if (slot >= XHV_VOICE_TALKERS)
        return;
    atomic_store(&g_remote[slot].open, 0u);
    atomic_store(&g_remote[slot].talking, 0u);
}

unsigned PORT_MS_ABI xhv_voice_remote_submit(unsigned slot, const unsigned char *data, unsigned packets)
{
    if (slot >= XHV_VOICE_TALKERS || !atomic_load(&g_remote[slot].open))
        return 0;
    remote_talker *t = &g_remote[slot];
    unsigned write = t->write, accepted = 0;
    for (unsigned i = 0; i < packets; i++) {
        if (write - atomic_load(&t->read) >= REMOTE_RING)
            break;
        copy_bytes(t->ring[write % REMOTE_RING], data + i * PACKET, PACKET);
        write++;
        accepted++;
    }
    atomic_store(&t->write, write);
    if (accepted != 0)
        atomic_store(&t->packets_received, t->packets_received + accepted);
    atomic_store(&t->last_arrival, xhv_host_time_ms() | 1u); // never 0, which means "never"
    return packets;
}

static void remote_trace(unsigned slot, const char *what, const remote_talker *t)
{
    line l = {{0}, 0};
    line_add(&l, "xhv: remote talker ");
    line_number(&l, slot);
    line_add(&l, what);
    line_add(&l, " (decoded ");
    line_number(&l, t->decoded);
    line_add(&l, ", concealed ");
    line_number(&l, t->concealed_total);
    line_add(&l, ", unpaired ");
    line_number(&l, t->unpaired);
    line_add(&l, ", late ");
    line_number(&l, t->late);
    line_add(&l, ", overflowed ");
    line_number(&l, t->overflowed);
    line_add(&l, ", lead-in decoded ahead ");
    line_number(&l, t->lead_in);
    line_add(&l, ", trimmed ");
    line_number(&l, t->skipped);
    line_add(&l, ")");
    if (t->spurt_renders != 0) {
        line_add(&l, ", this spurt ");
        line_number(&l, t->spurt_renders * 20u);
        line_add(&l, " ms played in ");
        line_number(&l, xhv_host_time_ms() - t->spurt_start_ms);
        line_add(&l, " ms");
    }
    line_end(&l);
}

static unsigned frame_seq(const unsigned char *frame);
static int frame_starts_spurt(const unsigned char *frame);

// packets from the game's thread into frames
static void remote_assemble(remote_talker *t)
{
    unsigned write = atomic_load(&t->write), read = t->read;
    for (; read != write; read++) {
        const unsigned char *packet = t->ring[read % REMOTE_RING];
        unsigned half, seq, reset;
        xhv_codec_packet_info(packet, &half, &seq, &reset);
        if (half == 0) {
            if (t->have_pending)
                t->unpaired++; // its second half never came
            copy_bytes(t->pending, packet, PACKET);
            t->have_pending = 1;
            continue;
        }
        unsigned pending_half = 0, pending_seq = 0, pending_reset = 0;
        if (t->have_pending)
            xhv_codec_packet_info(t->pending, &pending_half, &pending_seq, &pending_reset);
        if (!t->have_pending || pending_seq != seq) {
            t->unpaired++;
            continue;
        }
        t->have_pending = 0;
        if (t->frame_count == JITTER_FRAMES) { // full: the oldest goes
            t->frame_head = (t->frame_head + 1u) % JITTER_FRAMES;
            t->frame_count--;
            t->overflowed++;
        }
        unsigned position = t->frame_count;
        unsigned char *frame = t->frames[(t->frame_head + position) % JITTER_FRAMES];
        copy_bytes(frame, t->pending, PACKET);
        copy_bytes(frame + PACKET, packet, PACKET);
        t->frame_count++;
        // a frame that overtook others on the way (up to 3 frames) moves back to its place, within its spurt
        while (position > 0 && !pending_reset) {
            unsigned char *previous = t->frames[(t->frame_head + position - 1u) % JITTER_FRAMES];
            unsigned ahead = (frame_seq(previous) - seq) & 7u;
            if (ahead == 0u || ahead > 3u || frame_starts_spurt(previous))
                break;
            unsigned char swap[XHV_CODEC_FRAME_BYTES];
            copy_bytes(swap, previous, XHV_CODEC_FRAME_BYTES);
            copy_bytes(previous, frame, XHV_CODEC_FRAME_BYTES);
            copy_bytes(frame, swap, XHV_CODEC_FRAME_BYTES);
            frame = previous;
            position--;
        }
    }
    atomic_store(&t->read, read);
}

static unsigned frame_seq(const unsigned char *frame)
{
    unsigned half, seq, reset;
    xhv_codec_packet_info(frame, &half, &seq, &reset);
    return seq;
}

static int frame_starts_spurt(const unsigned char *frame)
{
    unsigned half, seq, reset;
    xhv_codec_packet_info(frame, &half, &seq, &reset);
    return reset != 0;
}

static void frame_pop(remote_talker *t)
{
    t->frame_head = (t->frame_head + 1u) % JITTER_FRAMES;
    t->frame_count--;
}

int PORT_MS_ABI xhv_voice_remote_render(unsigned slot, short *pcm)
{
    for (int n = 0; n < FRAME; n++)
        pcm[n] = 0;
    if (slot >= XHV_VOICE_TALKERS || !atomic_load(&g_remote[slot].open))
        return 0;
    remote_talker *t = &g_remote[slot];
    atomic_store(&t->last_render, xhv_host_time_ms() | 1u); // never 0, which means "never"
    remote_assemble(t);
    if (t->hold > 0)
        t->hold--;
    int voice = 0;
    if (!t->playing) {
        if (t->frame_count == 0) {
            t->waited = 0;
        } else if (t->frame_count >= t->prebuffer || ++t->waited >= t->prebuffer) {
            // a spurt starts (or a talker joins one under way): from a clean decoder
            int postfilter = t->decoder.postfilter;
            xhv_decoder_init(&t->decoder);
            t->decoder.postfilter = postfilter;
            t->playing = 1;
            t->spurt_start_ms = xhv_host_time_ms();
            t->spurt_renders = 0;
            t->window_renders = 0;
            t->window_low = ~0u;
            t->expected = frame_seq(t->frames[t->frame_head]);
            t->concealed = 0;
            t->waited = 0;
            // A spurt's first frames come in one burst with its pre-roll (the 60 ms before the onset): what is beyond
            // the pre-buffer is decoded without playing, oldest first -- the quiet lead-in primes the decoder instead
            // of adding its length to the delay for the whole spurt.
            while (t->frame_count > t->prebuffer) {
                short discard[FRAME];
                xhv_decoder_decode(&t->decoder, t->frames[t->frame_head], discard);
                t->expected = (frame_seq(t->frames[t->frame_head]) + 1u) & 7u;
                frame_pop(t);
                t->lead_in++;
            }
            if (g_trace)
                remote_trace(slot, " starts playing", t);
        }
    }
    if (t->playing) {
        t->spurt_renders++;
        // Latency the network no longer needs is given back: when the buffer never fell under SLACK_FRAMES for a
        // second, one frame is decoded ahead (its state kept, its sound not played); a buffer past LATENCY_HARD is
        // cut back at once. Arrivals come in bursts (the game sends voice with its packets, 10 to 30 a second), so
        // the depth right after a burst says nothing; its low point over time does.
        if (t->frame_count < t->window_low)
            t->window_low = t->frame_count;
        unsigned ahead = 0;
        if (++t->window_renders >= SLACK_WINDOW) {
            if (t->window_low != ~0u && t->window_low > SLACK_FRAMES)
                ahead = 1;
            t->window_renders = 0;
            t->window_low = ~0u;
        }
        if (t->frame_count > LATENCY_HARD)
            ahead = t->frame_count - LATENCY_HARD;
        for (; ahead > 0 && t->frame_count > 0; ahead--) {
            short discard[FRAME];
            xhv_decoder_decode(&t->decoder, t->frames[t->frame_head], discard);
            t->expected = (frame_seq(t->frames[t->frame_head]) + 1u) & 7u;
            frame_pop(t);
            t->skipped++;
        }
        for (;;) {
            if (t->frame_count == 0) {
                if (t->concealed < CONCEAL_LIMIT) { // late or lost: extrapolate, fading
                    xhv_decoder_conceal(&t->decoder, pcm);
                    t->concealed++;
                    t->concealed_total++;
                    voice = 1;
                } else {
                    // the spurt is over: the next starts with 20 ms more in hand after an underrun, with 20 ms
                    // less after CLEAN_SPURTS without one (the network's jitter is bursty: one quiet spurt proves
                    // little)
                    t->clean_spurts = t->underrun ? 0u : t->clean_spurts + 1u;
                    if (t->underrun && t->prebuffer < PREBUFFER_MAX)
                        t->prebuffer++;
                    else if (t->clean_spurts >= CLEAN_SPURTS && t->prebuffer > PREBUFFER_MIN) {
                        t->prebuffer--;
                        t->clean_spurts = 0;
                    }
                    t->underrun = 0;
                    t->playing = 0;
                    if (g_trace)
                        remote_trace(slot, " stops", t);
                }
                break;
            }
            unsigned gap = (frame_seq(t->frames[t->frame_head]) - t->expected) & 7u;
            int spurt = frame_starts_spurt(t->frames[t->frame_head]); // a new spurt plays at once, whatever its seq
            if (!spurt && gap >= 5u) { // it is behind: its turn was concealed already
                frame_pop(t);
                t->late++;
                continue;
            }
            if (spurt || gap == 0u || gap <= t->concealed || gap == 4u) {
                if (!spurt && gap == 0u && t->concealed > 0)
                    t->underrun = 1; // late, not lost: its turn was concealed while it was on the way
                xhv_decoder_decode(&t->decoder, t->frames[t->frame_head], pcm);
                t->expected = (frame_seq(t->frames[t->frame_head]) + 1u) & 7u;
                frame_pop(t);
                t->concealed = 0;
                t->decoded++;
                atomic_store(&t->decoded_total, t->decoded);
                t->hold = TALK_HOLD;
                voice = 1;
            } else { // frames before this one are missing: conceal one, the next call looks again
                xhv_decoder_conceal(&t->decoder, pcm);
                t->concealed++;
                t->concealed_total++;
                voice = 1;
            }
            break;
        }
    }
    atomic_store(&t->talking, t->hold > 0 ? 1u : 0u);
    return voice;
}

int PORT_MS_ABI xhv_voice_remote_talking(unsigned slot)
{
    if (slot >= XHV_VOICE_TALKERS || !atomic_load(&g_remote[slot].open))
        return 0;
    remote_talker *t = &g_remote[slot];
    unsigned now = xhv_host_time_ms(), rendered = atomic_load(&t->last_render);
    if (rendered != 0 && (int)(now - rendered) < 200) // signed: the other thread may have stamped it after `now`
        return atomic_load(&t->talking) != 0;
    unsigned arrival = atomic_load(&t->last_arrival); // nothing plays the talker: arrivals are all there is
    return arrival != 0 && (int)(now - arrival) < 200;
}

unsigned PORT_MS_ABI xhv_voice_remote_frames_decoded(unsigned slot)
{
    if (slot >= XHV_VOICE_TALKERS || !atomic_load(&g_remote[slot].open))
        return 0;
    return atomic_load(&g_remote[slot].decoded_total);
}

unsigned PORT_MS_ABI xhv_voice_remote_packets_received(unsigned slot)
{
    if (slot >= XHV_VOICE_TALKERS || !atomic_load(&g_remote[slot].open))
        return 0;
    return atomic_load(&g_remote[slot].packets_received);
}
