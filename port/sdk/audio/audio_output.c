// port/sdk/audio (both hosts): the output ring, the drain thread and the sinks every host has (null, WAV file); the
// device backends are audio_linux.c and audio_windows.c (interface: port_audio.h, host side: audio_host.h).
//
// The engine writes the mastering voice's 6-channel frames (FL FR FC LFE BL BR). The drain converts them for the
// sink: 6 channels as they are, or the 5.1 to stereo fold-down XAudio2 and X3DAudio use for a stereo endpoint
// (L = FL + 0.7071 FC + 0.7071 BL, R = FR + 0.7071 FC + 0.7071 BR; LFE is not folded in), then a safety peak limiter
// (instant attack, 150 ms release, ceiling -0.2 dBFS). The game's own master limiter, the Waves L360 on its master
// submix (waves_bypass.c), already holds each channel at -1 dBFS; this one only catches what the fold-down adds.
// NaNs and infinities never reach the device (they become silence).
#include "port_audio.h"
#include "audio_host.h"

#define RING_FRAMES PORT_AUDIO_RING_FRAMES
#define MASTER_CHANNELS 6
#define CHUNK_FRAMES 256

static float g_ring[RING_FRAMES * MASTER_CHANNELS];
static unsigned g_ring_head; // frames written (monotonic; the engine thread only)
static unsigned g_ring_tail; // frames read (monotonic; the drain thread only)
static volatile int g_state; // 0 closed, 1 opening, 2 open
static volatile int g_stop;
static port_audio_sink g_sink;
static unsigned g_underruns;
static float g_limiter_envelope = 0.0f;

int PORT_MS_ABI port_audio_disabled(void)
{
    char value[8];
    return port_audio_getenv("HALO3_NO_AUDIO", value, sizeof(value)) && value[0] == '1';
}

unsigned PORT_MS_ABI port_audio_writable(void)
{
    unsigned head = __atomic_load_n(&g_ring_head, __ATOMIC_RELAXED);
    unsigned tail = __atomic_load_n(&g_ring_tail, __ATOMIC_ACQUIRE);
    return RING_FRAMES - (head - tail);
}

unsigned PORT_MS_ABI port_audio_write(const float *frames, unsigned count)
{
    unsigned room = port_audio_writable();
    if (count > room)
        count = room;
    unsigned head = __atomic_load_n(&g_ring_head, __ATOMIC_RELAXED);
    for (unsigned i = 0; i < count; i++) {
        float *slot = &g_ring[((head + i) % RING_FRAMES) * MASTER_CHANNELS];
        for (unsigned c = 0; c < MASTER_CHANNELS; c++)
            slot[c] = frames[i * MASTER_CHANNELS + c];
    }
    __atomic_store_n(&g_ring_head, head + count, __ATOMIC_RELEASE);
    return count;
}

static unsigned ring_available(void)
{
    unsigned head = __atomic_load_n(&g_ring_head, __ATOMIC_ACQUIRE);
    unsigned tail = __atomic_load_n(&g_ring_tail, __ATOMIC_RELAXED);
    return head - tail;
}

static int finite(float x) { return x == x && x <= 3.0e38f && x >= -3.0e38f; }

// Up to `count` ring frames converted for the sink into `out` (sink channels, interleaved). With `pad`, missing
// frames are silence (and counted as an underrun) so a device that is waiting for data gets `count` frames.
static unsigned ring_read(float *out, unsigned count, int pad)
{
    unsigned available = ring_available();
    unsigned take = available < count ? available : count;
    unsigned tail = __atomic_load_n(&g_ring_tail, __ATOMIC_RELAXED);
    const unsigned channels = g_sink.channels;
    // release coefficient for 150 ms at 48 kHz: exp(-1 / (0.15 * 48000))
    const float release = 0.999861f, ceiling = 0.977f;
    unsigned produced = pad ? count : take;
    for (unsigned i = 0; i < produced; i++) {
        float m[MASTER_CHANNELS] = {0, 0, 0, 0, 0, 0};
        if (i < take) {
            const float *slot = &g_ring[((tail + i) % RING_FRAMES) * MASTER_CHANNELS];
            for (unsigned c = 0; c < MASTER_CHANNELS; c++)
                m[c] = finite(slot[c]) ? slot[c] : 0.0f;
        }
        float o[MASTER_CHANNELS];
        if (channels == 2) {
            o[0] = m[0] + 0.70710678f * (m[2] + m[4]);
            o[1] = m[1] + 0.70710678f * (m[2] + m[5]);
        } else {
            for (unsigned c = 0; c < MASTER_CHANNELS; c++)
                o[c] = m[c];
        }
        float peak = 0.0f;
        for (unsigned c = 0; c < channels; c++) {
            float a = o[c] < 0 ? -o[c] : o[c];
            if (a > peak)
                peak = a;
        }
        g_limiter_envelope = peak > g_limiter_envelope ? peak : g_limiter_envelope * release;
        float gain = g_limiter_envelope > ceiling ? ceiling / g_limiter_envelope : 1.0f;
        for (unsigned c = 0; c < channels; c++) {
            float v = o[c] * gain;
            out[i * channels + c] = v > 1.0f ? 1.0f : v < -1.0f ? -1.0f : v;
        }
    }
    __atomic_store_n(&g_ring_tail, tail + take, __ATOMIC_RELEASE);
    if (pad && take < count)
        g_underruns++;
    return produced;
}

// ---- the wall clock: paces the sinks that stand in for a device (null, wavrt) ----

static unsigned long long g_clock_start_us, g_clock_frames;

static void clock_reset(void) { g_clock_frames = 0; }

static void clock_take(unsigned count)
{
    if (g_clock_frames == 0)
        g_clock_start_us = port_audio_host_time_us();
    g_clock_frames += count;
    unsigned long long due = g_clock_start_us + g_clock_frames * 1000000ull / PORT_AUDIO_RATE;
    unsigned long long now = port_audio_host_time_us();
    if (due > now)
        port_audio_host_sleep_us((unsigned)(due - now));
}

// ---- null sink: the wall clock takes the frames ----

static int null_write(const float *frames, unsigned count)
{
    (void)frames;
    clock_take(count);
    return 1;
}

static void null_close(void) {}

static void open_null(port_audio_sink *sink, unsigned channels)
{
    sink->name = "null";
    sink->channels = channels;
    sink->exact = 0;
    sink->write = null_write;
    sink->close = null_close;
    clock_reset();
}

// ---- WAV sinks: float WAV. wav: takes exactly what the engine renders, as fast as it renders it (tests);
// wavrt: is paced by the wall clock like a device, silence where the engine fell behind (captures of a run) ----

static void *g_wav_file;
static unsigned g_wav_data_bytes;

static void put_u32(unsigned char *p, unsigned v)
{
    p[0] = (unsigned char)v;
    p[1] = (unsigned char)(v >> 8);
    p[2] = (unsigned char)(v >> 16);
    p[3] = (unsigned char)(v >> 24);
}

static void wav_header(unsigned char *h, unsigned channels, unsigned data_bytes)
{
    static const unsigned char k_template[44] = {'R', 'I', 'F', 'F', 0, 0, 0, 0, 'W', 'A', 'V', 'E', 'f', 'm', 't', ' ', 16, 0,
        0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 32, 0, 'd', 'a', 't', 'a', 0, 0, 0, 0};
    for (int i = 0; i < 44; i++)
        h[i] = k_template[i];
    put_u32(h + 4, 36 + data_bytes);
    h[22] = (unsigned char)channels;
    put_u32(h + 24, PORT_AUDIO_RATE);
    put_u32(h + 28, PORT_AUDIO_RATE * channels * 4);
    h[32] = (unsigned char)(channels * 4);
    put_u32(h + 40, data_bytes);
}

static int wav_write(const float *frames, unsigned count)
{
    unsigned bytes = count * g_sink.channels * 4;
    if (!port_audio_host_file_write(g_wav_file, frames, bytes))
        return 0;
    g_wav_data_bytes += bytes;
    // once a second of audio the header catches up, so a capture of a run that is killed stays a valid WAV
    unsigned second = PORT_AUDIO_RATE * g_sink.channels * 4;
    if (g_wav_data_bytes / second != (g_wav_data_bytes - bytes) / second) {
        unsigned char header[44];
        wav_header(header, g_sink.channels, g_wav_data_bytes);
        port_audio_host_file_update_header(g_wav_file, header, sizeof(header));
    }
    return 1;
}

static void wav_close(void)
{
    unsigned char header[44];
    wav_header(header, g_sink.channels, g_wav_data_bytes);
    port_audio_host_file_update_header(g_wav_file, header, sizeof(header));
    port_audio_host_file_close(g_wav_file);
    g_wav_file = 0;
}

static int wavrt_write(const float *frames, unsigned count)
{
    if (!wav_write(frames, count))
        return 0;
    clock_take(count);
    return 1;
}

static int open_wav(port_audio_sink *sink, const char *path, unsigned channels, int realtime)
{
    g_wav_file = port_audio_host_file_create(path);
    if (g_wav_file == 0)
        return 0;
    unsigned char header[44];
    wav_header(header, channels, 0);
    if (!port_audio_host_file_write(g_wav_file, header, sizeof(header))) {
        port_audio_host_file_close(g_wav_file);
        g_wav_file = 0;
        return 0;
    }
    g_wav_data_bytes = 0;
    sink->name = realtime ? "wavrt" : "wav";
    sink->channels = channels;
    sink->exact = !realtime;
    sink->write = realtime ? wavrt_write : wav_write;
    clock_reset();
    sink->close = wav_close;
    return 1;
}

// ---- the drain ----

static void drain_main(void)
{
    static float buffer[CHUNK_FRAMES * MASTER_CHANNELS];
    while (!__atomic_load_n(&g_stop, __ATOMIC_ACQUIRE)) {
        if (g_sink.exact) {
            // a file takes exactly what was rendered
            unsigned n = ring_read(buffer, CHUNK_FRAMES, 0);
            if (n == 0) {
                port_audio_host_sleep_us(500);
                continue;
            }
            if (!g_sink.write(buffer, n))
                break;
            continue;
        }
        // a device (or the clock) takes a chunk every 5.3 ms whether or not the engine kept up
        ring_read(buffer, CHUNK_FRAMES, 1);
        if (!g_sink.write(buffer, CHUNK_FRAMES)) {
            port_audio_log("port: audio: the device stopped taking audio; continuing without sound\n");
            g_sink.close();
            open_null(&g_sink, g_sink.channels);
        }
    }
    if (g_sink.exact) {
        // flush what the engine rendered before the close
        unsigned n;
        while ((n = ring_read(buffer, CHUNK_FRAMES, 0)) != 0)
            if (!g_sink.write(buffer, n))
                break;
    }
}

unsigned PORT_MS_ABI port_audio_open(void)
{
    if (__atomic_load_n(&g_state, __ATOMIC_ACQUIRE) == 2)
        return g_sink.channels == 6 ? 6u : 2u;
    int expected = 0;
    if (!__atomic_compare_exchange_n(&g_state, &expected, 1, 0, __ATOMIC_ACQ_REL, __ATOMIC_ACQUIRE)) {
        while (__atomic_load_n(&g_state, __ATOMIC_ACQUIRE) != 2)
            port_audio_host_sleep_us(1000);
        return g_sink.channels == 6 ? 6u : 2u;
    }
    char text[512];
    unsigned channels = 2;
    if (port_audio_getenv("HALO3_AUDIO_CHANNELS", text, sizeof(text)) && text[0] == '6' && text[1] == 0)
        channels = 6;
    char backend[512];
    if (!port_audio_getenv("HALO3_AUDIO_BACKEND", backend, sizeof(backend)))
        backend[0] = 0;
    int opened = 0;
    if (port_audio_disabled()) {
        // HALO3_NO_AUDIO=1: no device; the clock still takes the frames of the voices the engine runs anyway
    } else if (backend[0] == 'w' && backend[1] == 'a' && backend[2] == 'v' && (backend[3] == ':' ||
                   (backend[3] == 'r' && backend[4] == 't' && backend[5] == ':'))) {
        int realtime = backend[3] == 'r';
        opened = open_wav(&g_sink, backend + (realtime ? 6 : 4), channels, realtime);
        if (!opened)
            port_audio_log("port: audio: cannot create the WAV file; using the null sink\n");
    } else if (!(backend[0] == 'n' && backend[1] == 'u' && backend[2] == 'l' && backend[3] == 'l')) {
        opened = port_audio_host_open(backend[0] ? backend : 0, channels, &g_sink);
        if (!opened)
            port_audio_log("port: audio: no audio device; continuing without sound\n");
    }
    if (!opened)
        open_null(&g_sink, channels);
    g_ring_head = g_ring_tail = 0;
    g_underruns = 0;
    g_limiter_envelope = 0.0f;
    g_stop = 0;
    port_audio_log("port: audio: output ");
    port_audio_log(g_sink.name);
    port_audio_log(g_sink.channels == 6 ? ", 5.1 at 48000 Hz\n" : ", stereo at 48000 Hz\n");
    port_audio_host_thread_start(drain_main);
    __atomic_store_n(&g_state, 2, __ATOMIC_RELEASE);
    return g_sink.channels == 6 ? 6u : 2u;
}

void PORT_MS_ABI port_audio_close(void)
{
    if (__atomic_load_n(&g_state, __ATOMIC_ACQUIRE) != 2)
        return;
    __atomic_store_n(&g_stop, 1, __ATOMIC_RELEASE);
    port_audio_host_thread_join();
    g_sink.close();
    __atomic_store_n(&g_state, 0, __ATOMIC_RELEASE);
}

const char * PORT_MS_ABI port_audio_backend(void) { return g_state == 2 ? g_sink.name : "closed"; }

unsigned PORT_MS_ABI port_audio_underruns(void) { return g_underruns; }
