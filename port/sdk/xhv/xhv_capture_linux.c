// port/sdk/xhv (Linux host): the voice runtime's host services (xhv_host.h): the microphone, a thread, the clock.
//
// The microphone is recorded at 16 kHz mono 16-bit through the first backend that opens (each loaded at run time,
// RTLD_LOCAL, so a desktop lacking either library still runs):
//   pulse  libpulse-simple.so.0 (+ libpulse.so.0 for source enumeration): PulseAudio, or PipeWire's pulse server;
//   alsa   libasound.so.2: "default" or a specific capture PCM ("alsa:<name>"), 40 ms of buffer.
// Reads block on the device, which paces the capture thread; a read is one 20 ms frame.
#define _GNU_SOURCE
#include "xhv_host.h"
#include "../runtime/port_config.h"
#include "../runtime/port_runtime.h"

#include <dlfcn.h>
#include <errno.h>
#include <pthread.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>

static unsigned str_len(const char *s)
{
    unsigned n = 0;
    while (s != 0 && s[n] != 0)
        n++;
    return n;
}

static void str_copy(char *dst, unsigned cap, const char *src)
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

static int str_eq(const char *a, const char *b)
{
    if (a == 0 || b == 0)
        return a == b;
    while (*a && *a == *b) {
        a++;
        b++;
    }
    return *a == *b;
}

static int str_starts(const char *s, const char *prefix)
{
    if (s == 0 || prefix == 0)
        return 0;
    while (*prefix) {
        if (*s++ != *prefix++)
            return 0;
    }
    return 1;
}

static int str_ends(const char *s, const char *suffix)
{
    unsigned sl = str_len(s), su = str_len(suffix);
    if (sl < su)
        return 0;
    return str_eq(s + (sl - su), suffix);
}

static int is_default_device(const char *device_id)
{
    return device_id == 0 || device_id[0] == 0 || str_eq(device_id, "default");
}

const char *xhv_host_getenv(const char *name) { return getenv(name); }

void xhv_host_log(const char *text) { port_write_error(text); }

unsigned xhv_host_mic_device(char *out, unsigned capacity)
{
    return (unsigned)port_config_mic_device(out, (unsigned long)capacity);
}

unsigned xhv_host_time_ms(void)
{
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return (unsigned)((unsigned long long)t.tv_sec * 1000ull + (unsigned long long)t.tv_nsec / 1000000ull);
}

void xhv_host_sleep_ms(unsigned milliseconds)
{
    struct timespec t = {(time_t)(milliseconds / 1000u), (long)(milliseconds % 1000u) * 1000000L};
    while (nanosleep(&t, &t) != 0 && errno == EINTR)
        ;
}

// ---- the thread ----

static pthread_t g_thread;
static void (*g_thread_main)(void);

static void *thread_entry(void *parameter)
{
    (void)parameter;
    g_thread_main();
    return 0;
}

int xhv_host_thread_start(void (*main)(void))
{
    g_thread_main = main;
    return pthread_create(&g_thread, 0, thread_entry, 0) == 0;
}

void xhv_host_thread_join(void) { pthread_join(g_thread, 0); }

// ---- PulseAudio (simple API + libpulse introspection for source enumeration) ----

typedef struct {
    int format;
    unsigned rate;
    unsigned char channels;
} pa_sample_spec_t;
typedef struct {
    unsigned maxlength, tlength, prebuf, minreq, fragsize;
} pa_buffer_attr_t;
#define PA_SAMPLE_S16LE 3
#define PA_STREAM_RECORD 2

// Prefix of pa_source_info (pulse/introspect.h): stable across PulseAudio and PipeWire.
typedef struct {
    const char *name;
    uint32_t index;
    const char *description;
} pa_source_info_head_t;

#define PA_CONTEXT_READY 4
#define PA_CONTEXT_FAILED 5
#define PA_CONTEXT_TERMINATED 6
#define PA_OPERATION_RUNNING 0

static struct {
    void *library;
    void *stream;
    void *(*simple_new)(const char *, const char *, int, const char *, const char *, const pa_sample_spec_t *,
        const void *, const pa_buffer_attr_t *, int *);
    int (*simple_read)(void *, void *, size_t, int *);
    void (*simple_free)(void *);
} g_pulse;

static struct {
    void *library;
    int tried;
    void *(*mainloop_new)(void);
    void (*mainloop_free)(void *);
    void *(*mainloop_get_api)(void *);
    int (*mainloop_iterate)(void *, int, int *);
    void *(*context_new)(void *, const char *);
    void (*context_unref)(void *);
    int (*context_connect)(void *, const char *, int, const void *);
    void (*context_disconnect)(void *);
    int (*context_get_state)(const void *);
    void *(*context_get_source_info_list)(void *, void (*)(void *, const pa_source_info_head_t *, int, void *), void *);
    int (*operation_get_state)(const void *);
    void (*operation_unref)(void *);
} g_pa_enum;

typedef struct {
    xhv_capture_device *out;
    unsigned capacity;
    unsigned count;
    int done;
} pa_enum_ctx;

static void pa_source_cb(void *ctx, const pa_source_info_head_t *info, int eol, void *userdata)
{
    (void)ctx;
    pa_enum_ctx *state = (pa_enum_ctx *)userdata;
    if (eol != 0) {
        state->done = 1;
        return;
    }
    if (info == 0 || info->name == 0 || info->name[0] == 0)
        return;
    // Skip sink monitor/loopback sources (PulseAudio and PipeWire always suffix them ".monitor").
    if (str_ends(info->name, ".monitor"))
        return;
    if (state->count >= state->capacity)
        return;
    for (unsigned i = 0; i < state->count; i++) {
        if (str_eq(state->out[i].id, info->name))
            return;
    }
    xhv_capture_device *dev = &state->out[state->count++];
    str_copy(dev->id, sizeof(dev->id), info->name);
    const char *label = (info->description != 0 && info->description[0] != 0) ? info->description : info->name;
    str_copy(dev->name, sizeof(dev->name), label);
}

static unsigned enum_pulse(xhv_capture_device *out, unsigned capacity)
{
    if (!g_pa_enum.tried) {
        g_pa_enum.tried = 1;
        g_pa_enum.library = dlopen("libpulse.so.0", RTLD_NOW | RTLD_LOCAL);
        if (g_pa_enum.library != 0) {
            *(void **)&g_pa_enum.mainloop_new = dlsym(g_pa_enum.library, "pa_mainloop_new");
            *(void **)&g_pa_enum.mainloop_free = dlsym(g_pa_enum.library, "pa_mainloop_free");
            *(void **)&g_pa_enum.mainloop_get_api = dlsym(g_pa_enum.library, "pa_mainloop_get_api");
            *(void **)&g_pa_enum.mainloop_iterate = dlsym(g_pa_enum.library, "pa_mainloop_iterate");
            *(void **)&g_pa_enum.context_new = dlsym(g_pa_enum.library, "pa_context_new");
            *(void **)&g_pa_enum.context_unref = dlsym(g_pa_enum.library, "pa_context_unref");
            *(void **)&g_pa_enum.context_connect = dlsym(g_pa_enum.library, "pa_context_connect");
            *(void **)&g_pa_enum.context_disconnect = dlsym(g_pa_enum.library, "pa_context_disconnect");
            *(void **)&g_pa_enum.context_get_state = dlsym(g_pa_enum.library, "pa_context_get_state");
            *(void **)&g_pa_enum.context_get_source_info_list =
                dlsym(g_pa_enum.library, "pa_context_get_source_info_list");
            *(void **)&g_pa_enum.operation_get_state = dlsym(g_pa_enum.library, "pa_operation_get_state");
            *(void **)&g_pa_enum.operation_unref = dlsym(g_pa_enum.library, "pa_operation_unref");
        }
    }
    if (!g_pa_enum.mainloop_new || !g_pa_enum.mainloop_free || !g_pa_enum.mainloop_get_api ||
        !g_pa_enum.mainloop_iterate || !g_pa_enum.context_new || !g_pa_enum.context_unref ||
        !g_pa_enum.context_connect || !g_pa_enum.context_disconnect || !g_pa_enum.context_get_state ||
        !g_pa_enum.context_get_source_info_list || !g_pa_enum.operation_get_state || !g_pa_enum.operation_unref)
        return 0;

    void *ml = g_pa_enum.mainloop_new();
    if (ml == 0)
        return 0;
    void *api = g_pa_enum.mainloop_get_api(ml);
    void *ctx = api ? g_pa_enum.context_new(api, "Halo 3") : 0;
    if (ctx == 0) {
        g_pa_enum.mainloop_free(ml);
        return 0;
    }
    pa_enum_ctx state = {out, capacity, 0, 0};
    if (g_pa_enum.context_connect(ctx, 0, 0, 0) == 0) {
        unsigned start = xhv_host_time_ms();
        int ready = 0;
        while ((unsigned)(xhv_host_time_ms() - start) < 500u) {
            int s = g_pa_enum.context_get_state(ctx);
            if (s == PA_CONTEXT_READY) {
                ready = 1;
                break;
            }
            if (s == PA_CONTEXT_FAILED || s == PA_CONTEXT_TERMINATED)
                break;
            if (g_pa_enum.mainloop_iterate(ml, 0, 0) <= 0)
                xhv_host_sleep_ms(2);
        }
        if (ready) {
            void *op = g_pa_enum.context_get_source_info_list(ctx, pa_source_cb, &state);
            if (op != 0) {
                while (!state.done && (unsigned)(xhv_host_time_ms() - start) < 500u) {
                    int s = g_pa_enum.context_get_state(ctx);
                    if (s == PA_CONTEXT_FAILED || s == PA_CONTEXT_TERMINATED)
                        break;
                    if (g_pa_enum.operation_get_state(op) != PA_OPERATION_RUNNING)
                        break;
                    if (g_pa_enum.mainloop_iterate(ml, 0, 0) <= 0)
                        xhv_host_sleep_ms(2);
                }
                g_pa_enum.operation_unref(op);
            }
        }
        g_pa_enum.context_disconnect(ctx);
    }
    g_pa_enum.context_unref(ctx);
    g_pa_enum.mainloop_free(ml);
    return state.count;
}

static int open_pulse(const char *device_id)
{
    if (g_pulse.library == 0)
        g_pulse.library = dlopen("libpulse-simple.so.0", RTLD_NOW | RTLD_LOCAL);
    if (g_pulse.library == 0)
        return 0;
    *(void **)&g_pulse.simple_new = dlsym(g_pulse.library, "pa_simple_new");
    *(void **)&g_pulse.simple_read = dlsym(g_pulse.library, "pa_simple_read");
    *(void **)&g_pulse.simple_free = dlsym(g_pulse.library, "pa_simple_free");
    if (!g_pulse.simple_new || !g_pulse.simple_read || !g_pulse.simple_free)
        return 0;
    const char *dev = 0;
    if (!is_default_device(device_id)) {
        dev = str_starts(device_id, "pulse:") ? device_id + 6 : device_id;
        if (dev[0] == 0)
            dev = 0;
    }
    pa_sample_spec_t spec = {PA_SAMPLE_S16LE, 16000u, 1};
    // fragments of 20 ms (640 bytes), the rest the server's defaults
    pa_buffer_attr_t attr = {(unsigned)-1, (unsigned)-1, (unsigned)-1, (unsigned)-1, 640u};
    int error = 0;
    g_pulse.stream = g_pulse.simple_new(0, "Halo 3", PA_STREAM_RECORD, dev, "voice chat", &spec, 0, &attr, &error);
    return g_pulse.stream != 0;
}

// ---- ALSA ----

#define SND_PCM_STREAM_CAPTURE 1
#define SND_PCM_FORMAT_S16_LE 2
#define SND_PCM_ACCESS_RW_INTERLEAVED 3

static struct {
    void *library;
    void *pcm;
    int (*pcm_open)(void **, const char *, int, int);
    int (*pcm_set_params)(void *, int, int, unsigned, unsigned, int, unsigned);
    long (*pcm_readi)(void *, void *, unsigned long);
    int (*pcm_recover)(void *, int, int);
    int (*pcm_close)(void *);
    int (*device_name_hint)(int, const char *, void ***);
    char *(*device_name_get_hint)(const void *, const char *);
    int (*device_name_free_hint)(void **);
} g_alsa;

static int load_alsa(void)
{
    if (g_alsa.library == 0)
        g_alsa.library = dlopen("libasound.so.2", RTLD_NOW | RTLD_LOCAL);
    if (g_alsa.library == 0)
        return 0;
    *(void **)&g_alsa.pcm_open = dlsym(g_alsa.library, "snd_pcm_open");
    *(void **)&g_alsa.pcm_set_params = dlsym(g_alsa.library, "snd_pcm_set_params");
    *(void **)&g_alsa.pcm_readi = dlsym(g_alsa.library, "snd_pcm_readi");
    *(void **)&g_alsa.pcm_recover = dlsym(g_alsa.library, "snd_pcm_recover");
    *(void **)&g_alsa.pcm_close = dlsym(g_alsa.library, "snd_pcm_close");
    *(void **)&g_alsa.device_name_hint = dlsym(g_alsa.library, "snd_device_name_hint");
    *(void **)&g_alsa.device_name_get_hint = dlsym(g_alsa.library, "snd_device_name_get_hint");
    *(void **)&g_alsa.device_name_free_hint = dlsym(g_alsa.library, "snd_device_name_free_hint");
    return g_alsa.pcm_open && g_alsa.pcm_set_params && g_alsa.pcm_readi && g_alsa.pcm_recover && g_alsa.pcm_close;
}

static unsigned enum_alsa(xhv_capture_device *out, unsigned capacity)
{
    if (!load_alsa() || !g_alsa.device_name_hint || !g_alsa.device_name_get_hint || !g_alsa.device_name_free_hint)
        return 0;
    void **hints = 0;
    if (g_alsa.device_name_hint(-1, "pcm", &hints) != 0 || hints == 0)
        return 0;
    unsigned count = 0;
    for (void **h = hints; *h != 0 && count < capacity; h++) {
        char *name = g_alsa.device_name_get_hint(*h, "NAME");
        char *desc = g_alsa.device_name_get_hint(*h, "DESC");
        char *ioid = g_alsa.device_name_get_hint(*h, "IOID");
        int capture = (ioid == 0 || str_eq(ioid, "Input"));
        if (capture && name != 0 && name[0] != 0 && !str_eq(name, "null") && !str_eq(name, "default") &&
            !str_eq(name, "sysdefault") && !str_eq(name, "pulse") && !str_eq(name, "pipewire")) {
            char full_id[XHV_DEVICE_ID_MAX];
            str_copy(full_id, sizeof(full_id), "alsa:");
            unsigned p = 5;
            for (unsigned i = 0; name[i] != 0 && p + 1 < sizeof(full_id); i++)
                full_id[p++] = name[i];
            full_id[p] = 0;
            int dup = 0;
            for (unsigned i = 0; i < count; i++)
                if (str_eq(out[i].id, full_id))
                    dup = 1;
            if (!dup) {
                xhv_capture_device *dev = &out[count++];
                str_copy(dev->id, sizeof(dev->id), full_id);
                // First line of DESC, or fall back to the PCM name.
                if (desc != 0 && desc[0] != 0) {
                    unsigned k = 0;
                    for (; desc[k] != 0 && desc[k] != '\n' && desc[k] != '\r' && k + 1 < sizeof(dev->name); k++)
                        dev->name[k] = desc[k];
                    dev->name[k] = 0;
                } else {
                    str_copy(dev->name, sizeof(dev->name), name);
                }
            }
        }
        if (name)
            free(name);
        if (desc)
            free(desc);
        if (ioid)
            free(ioid);
    }
    g_alsa.device_name_free_hint(hints);
    return count;
}

static int open_alsa(const char *device_id)
{
    if (!load_alsa())
        return 0;
    const char *pcm_name = "default";
    if (!is_default_device(device_id)) {
        pcm_name = str_starts(device_id, "alsa:") ? device_id + 5 : device_id;
        if (pcm_name[0] == 0)
            pcm_name = "default";
    }
    if (g_alsa.pcm_open(&g_alsa.pcm, pcm_name, SND_PCM_STREAM_CAPTURE, 0) < 0)
        return 0;
    if (g_alsa.pcm_set_params(g_alsa.pcm, SND_PCM_FORMAT_S16_LE, SND_PCM_ACCESS_RW_INTERLEAVED, 1, 16000u, 1,
            40000u) < 0) {
        g_alsa.pcm_close(g_alsa.pcm);
        g_alsa.pcm = 0;
        return 0;
    }
    return 1;
}

unsigned xhv_host_capture_devices(xhv_capture_device *out, unsigned capacity)
{
    if (out == 0 || capacity == 0)
        return 0;
    if (capacity > XHV_MAX_DEVICES)
        capacity = XHV_MAX_DEVICES;
    unsigned count = enum_pulse(out, capacity);
    if (count > 0)
        return count;
    return enum_alsa(out, capacity);
}

static enum { k_none, k_pulse, k_alsa } g_backend;

int xhv_host_capture_open(const char *device_id, const char **backend)
{
    int explicit_dev = !is_default_device(device_id);
    if (explicit_dev && str_starts(device_id, "alsa:")) {
        if (open_alsa(device_id)) {
            g_backend = k_alsa;
            *backend = "alsa";
            return 1;
        }
        xhv_host_log("xhv: selected ALSA microphone unavailable; falling back to default\n");
    } else if (explicit_dev) {
        if (open_pulse(device_id)) {
            g_backend = k_pulse;
            *backend = "pulse";
            return 1;
        }
        if (open_alsa(device_id)) {
            g_backend = k_alsa;
            *backend = "alsa";
            return 1;
        }
        xhv_host_log("xhv: selected microphone unavailable; falling back to default\n");
    }
    if (open_pulse(0)) {
        g_backend = k_pulse;
        *backend = "pulse";
        return 1;
    }
    if (open_alsa(0)) {
        g_backend = k_alsa;
        *backend = "alsa";
        return 1;
    }
    xhv_host_log("xhv: no microphone (neither libpulse-simple.so.0 nor libasound.so.2 opened a capture stream)\n");
    return 0;
}

int xhv_host_capture_read(short *pcm, unsigned samples)
{
    if (g_backend == k_pulse) {
        int error = 0;
        return g_pulse.simple_read(g_pulse.stream, pcm, (size_t)samples * 2u, &error) == 0 ? 1 : -1;
    }
    if (g_backend == k_alsa) {
        unsigned done = 0;
        while (done < samples) {
            long n = g_alsa.pcm_readi(g_alsa.pcm, pcm + done, samples - done);
            if (n < 0) {
                if (g_alsa.pcm_recover(g_alsa.pcm, (int)n, 1) < 0)
                    return -1;
                continue;
            }
            done += (unsigned)n;
        }
        return 1;
    }
    return -1;
}

void xhv_host_capture_close(void)
{
    if (g_backend == k_pulse && g_pulse.stream != 0) {
        g_pulse.simple_free(g_pulse.stream);
        g_pulse.stream = 0;
    } else if (g_backend == k_alsa && g_alsa.pcm != 0) {
        g_alsa.pcm_close(g_alsa.pcm);
        g_alsa.pcm = 0;
    }
    g_backend = k_none;
}
