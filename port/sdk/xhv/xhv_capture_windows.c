// port/sdk/xhv (Windows host): the voice runtime's host services (xhv_host.h): the microphone, a thread, the clock.
//
// The microphone is winmm's waveIn on either the wave mapper (system default recording device) or a specific
// enumerated recording device (which Windows resamples to 16 kHz mono 16-bit), loaded at run time like waveOut
// (audio_windows.c). Four 20 ms buffers circulate; the capture thread waits on the driver's event for one to come
// back, which paces it. Win32 itself is reached through host_ imports (port/sdk/kernel/win32_host.h): the plain names
// are the Xbox API the port defines.
#include "xhv_host.h"
#include "../kernel/win32_host.h"
#include "../runtime/port_config.h"
#include "../runtime/port_runtime.h"

PORT_HOST_IMPORT(void *, LoadLibraryA, 4, (const char *name));
PORT_HOST_IMPORT(void *, GetProcAddress, 8, (void *module, const char *name));
PORT_HOST_IMPORT(void *, CreateThread, 24, (void *security, __SIZE_TYPE__ stack,
    unsigned long(__stdcall *start)(void *), void *parameter, unsigned long flags, unsigned long *id));
PORT_HOST_IMPORT(unsigned long, WaitForSingleObject, 8, (void *handle, unsigned long milliseconds));
PORT_HOST_IMPORT(int, CloseHandle, 4, (void *handle));
PORT_HOST_IMPORT(void, Sleep, 4, (unsigned long milliseconds));
PORT_HOST_IMPORT(void *, CreateEventA, 16, (void *security, int manual_reset, int initial_state, const char *name));
PORT_HOST_IMPORT(int, QueryPerformanceCounter, 4, (long long *count));
PORT_HOST_IMPORT(int, QueryPerformanceFrequency, 4, (long long *frequency));

__declspec(dllimport) char *__cdecl getenv(const char *name);

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

static int is_default_device(const char *device_id)
{
    return device_id == 0 || device_id[0] == 0 || str_eq(device_id, "default");
}

static void append_text(char *dst, unsigned cap, const char *src)
{
    unsigned n = 0;
    while (n < cap && dst[n] != 0)
        n++;
    unsigned i = 0;
    while (src[i] != 0 && n + 1 < cap)
        dst[n++] = src[i++];
    if (cap > 0)
        dst[n] = 0;
}

static void append_uint(char *dst, unsigned cap, unsigned v)
{
    char digits[12];
    int n = 0;
    do {
        digits[n++] = (char)('0' + v % 10u);
        v /= 10u;
    } while (v != 0);
    unsigned at = 0;
    while (at < cap && dst[at] != 0)
        at++;
    while (n > 0 && at + 1 < cap)
        dst[at++] = digits[--n];
    if (cap > 0)
        dst[at] = 0;
}

const char *xhv_host_getenv(const char *name) { return getenv(name); }

void xhv_host_log(const char *text) { port_write_error(text); }

unsigned xhv_host_mic_device(char *out, unsigned capacity)
{
    return (unsigned)port_config_mic_device(out, (unsigned long)capacity);
}

unsigned xhv_host_time_ms(void)
{
    long long count, frequency;
    host_QueryPerformanceCounter(&count);
    host_QueryPerformanceFrequency(&frequency);
    return (unsigned)((unsigned long long)(count / frequency) * 1000ull +
                      (unsigned long long)(count % frequency) * 1000ull / (unsigned long long)frequency);
}

void xhv_host_sleep_ms(unsigned milliseconds) { host_Sleep(milliseconds); }

// ---- the thread ----

static void *g_thread;
static void (*g_thread_main)(void);

static unsigned long __stdcall thread_entry(void *parameter)
{
    (void)parameter;
    g_thread_main();
    return 0;
}

int xhv_host_thread_start(void (*main)(void))
{
    g_thread_main = main;
    g_thread = host_CreateThread(0, 0, thread_entry, 0, 0, 0);
    return g_thread != 0;
}

void xhv_host_thread_join(void)
{
    host_WaitForSingleObject(g_thread, 0xffffffffu);
    host_CloseHandle(g_thread);
    g_thread = 0;
}

// ---- waveIn ----

#pragma pack(push, 1)
typedef struct {
    unsigned short format_tag, channels;
    unsigned long samples_per_second, average_bytes_per_second;
    unsigned short block_align, bits_per_sample, extra_size;
} wave_format_ex;
typedef struct {
    unsigned short mid, pid;
    unsigned int driver_version;
    char pname[32];
    unsigned long formats;
    unsigned short channels;
    unsigned short reserved1;
} wave_in_caps_a;
#pragma pack(pop)
typedef struct wave_header {
    char *data;
    unsigned long buffer_length, bytes_recorded;
    __UINTPTR_TYPE__ user;
    unsigned long flags, loops;
    struct wave_header *next;
    __UINTPTR_TYPE__ reserved;
} wave_header;
typedef char wave_format_ex_size[sizeof(wave_format_ex) == 18 ? 1 : -1];
typedef char wave_in_caps_a_size[sizeof(wave_in_caps_a) == 48 ? 1 : -1];
typedef char wave_header_size[sizeof(wave_header) == (sizeof(void *) == 8 ? 48 : 32) ? 1 : -1];

#define WAVE_MAPPER 0xffffffffu
#define WAVE_FORMAT_PCM 1
#define CALLBACK_EVENT 0x00050000ul
#define WHDR_DONE 0x00000001ul
#define BUFFER_COUNT 4
#define BUFFER_SAMPLES 320

static struct {
    void *library;
    void *device;
    void *event;
    wave_header headers[BUFFER_COUNT];
    short buffers[BUFFER_COUNT][BUFFER_SAMPLES];
    unsigned next;           // the buffer that comes back next
    unsigned consumed;       // samples of it already handed out
    unsigned (__stdcall *get_num_devs)(void);
    unsigned (__stdcall *get_dev_caps)(__UINTPTR_TYPE__, wave_in_caps_a *, unsigned);
    unsigned (__stdcall *open)(void **, unsigned, const wave_format_ex *, __UINTPTR_TYPE__, __UINTPTR_TYPE__, unsigned long);
    unsigned (__stdcall *prepare)(void *, wave_header *, unsigned);
    unsigned (__stdcall *unprepare)(void *, wave_header *, unsigned);
    unsigned (__stdcall *add_buffer)(void *, wave_header *, unsigned);
    unsigned (__stdcall *start)(void *);
    unsigned (__stdcall *reset)(void *);
    unsigned (__stdcall *close)(void *);
} g_wave;

static int load_winmm(void)
{
    if (g_wave.library == 0)
        g_wave.library = host_LoadLibraryA("winmm.dll");
    if (g_wave.library == 0)
        return 0;
    *(void **)&g_wave.get_num_devs = host_GetProcAddress(g_wave.library, "waveInGetNumDevs");
    *(void **)&g_wave.get_dev_caps = host_GetProcAddress(g_wave.library, "waveInGetDevCapsA");
    *(void **)&g_wave.open = host_GetProcAddress(g_wave.library, "waveInOpen");
    *(void **)&g_wave.prepare = host_GetProcAddress(g_wave.library, "waveInPrepareHeader");
    *(void **)&g_wave.unprepare = host_GetProcAddress(g_wave.library, "waveInUnprepareHeader");
    *(void **)&g_wave.add_buffer = host_GetProcAddress(g_wave.library, "waveInAddBuffer");
    *(void **)&g_wave.start = host_GetProcAddress(g_wave.library, "waveInStart");
    *(void **)&g_wave.reset = host_GetProcAddress(g_wave.library, "waveInReset");
    *(void **)&g_wave.close = host_GetProcAddress(g_wave.library, "waveInClose");
    return g_wave.open && g_wave.prepare && g_wave.unprepare && g_wave.add_buffer && g_wave.start && g_wave.reset &&
           g_wave.close;
}

unsigned xhv_host_capture_devices(xhv_capture_device *out, unsigned capacity)
{
    if (out == 0 || capacity == 0)
        return 0;
    if (capacity > XHV_MAX_DEVICES)
        capacity = XHV_MAX_DEVICES;
    if (!load_winmm() || !g_wave.get_num_devs || !g_wave.get_dev_caps)
        return 0;
    unsigned total = g_wave.get_num_devs();
    unsigned count = 0;
    for (unsigned i = 0; i < total && count < capacity; i++) {
        wave_in_caps_a caps;
        __builtin_memset(&caps, 0, sizeof(caps));
        if (g_wave.get_dev_caps((__UINTPTR_TYPE__)i, &caps, sizeof(caps)) != 0)
            continue;
        caps.pname[sizeof(caps.pname) - 1] = 0;
        if (caps.pname[0] == 0)
            continue;
        // Disambiguate if two devices share the same 31-character waveIn prefix.
        unsigned dup = 0;
        for (unsigned j = 0; j < count; j++) {
            if (str_eq(out[j].id, caps.pname) || str_eq(out[j].name, caps.pname))
                dup++;
        }
        xhv_capture_device *dev = &out[count++];
        if (dup == 0) {
            str_copy(dev->id, sizeof(dev->id), caps.pname);
            str_copy(dev->name, sizeof(dev->name), caps.pname);
        } else {
            dev->id[0] = 0;
            append_text(dev->id, sizeof(dev->id), "winmm:");
            append_uint(dev->id, sizeof(dev->id), i);
            append_text(dev->id, sizeof(dev->id), ":");
            append_text(dev->id, sizeof(dev->id), caps.pname);
            str_copy(dev->name, sizeof(dev->name), caps.pname);
            append_text(dev->name, sizeof(dev->name), " (");
            append_uint(dev->name, sizeof(dev->name), dup + 1u);
            append_text(dev->name, sizeof(dev->name), ")");
        }
    }
    return count;
}

static unsigned resolve_winmm_device_id(const char *device_id, int *matched)
{
    *matched = 0;
    if (is_default_device(device_id) || !g_wave.get_num_devs || !g_wave.get_dev_caps)
        return WAVE_MAPPER;
    unsigned total = g_wave.get_num_devs();
    // Explicit "winmm:<index>:<name>" disambiguator
    if (str_starts(device_id, "winmm:")) {
        const char *p = device_id + 6;
        unsigned idx = 0;
        int digits = 0;
        while (*p >= '0' && *p <= '9') {
            idx = idx * 10u + (unsigned)(*p - '0');
            p++;
            digits = 1;
        }
        if (digits && idx < total) {
            if (*p == ':' && p[1] != 0) {
                wave_in_caps_a caps;
                __builtin_memset(&caps, 0, sizeof(caps));
                if (g_wave.get_dev_caps((__UINTPTR_TYPE__)idx, &caps, sizeof(caps)) == 0) {
                    caps.pname[sizeof(caps.pname) - 1] = 0;
                    if (str_eq(caps.pname, p + 1)) {
                        *matched = 1;
                        return idx;
                    }
                }
            } else {
                *matched = 1;
                return idx;
            }
        }
    }
    // Exact name match
    for (unsigned i = 0; i < total; i++) {
        wave_in_caps_a caps;
        __builtin_memset(&caps, 0, sizeof(caps));
        if (g_wave.get_dev_caps((__UINTPTR_TYPE__)i, &caps, sizeof(caps)) != 0)
            continue;
        caps.pname[sizeof(caps.pname) - 1] = 0;
        if (str_eq(caps.pname, device_id)) {
            *matched = 1;
            return i;
        }
    }
    return WAVE_MAPPER;
}

int xhv_host_capture_open(const char *device_id, const char **backend)
{
    if (!load_winmm()) {
        xhv_host_log("xhv: no microphone (winmm.dll did not load or lacks waveIn)\n");
        return 0;
    }
    g_wave.event = host_CreateEventA(0, 0, 0, 0);
    if (g_wave.event == 0)
        return 0;
    wave_format_ex format = {WAVE_FORMAT_PCM, 1, 16000ul, 32000ul, 2, 16, 0};
    int explicit_dev = !is_default_device(device_id);
    int matched = 0;
    unsigned target = resolve_winmm_device_id(device_id, &matched);
    unsigned open_err = 1;
    if (explicit_dev && matched)
        open_err = g_wave.open(&g_wave.device, target, &format, (__UINTPTR_TYPE__)g_wave.event, 0, CALLBACK_EVENT);
    if (open_err != 0) {
        if (explicit_dev)
            xhv_host_log("xhv: selected microphone unavailable; falling back to default\n");
        open_err = g_wave.open(&g_wave.device, WAVE_MAPPER, &format, (__UINTPTR_TYPE__)g_wave.event, 0, CALLBACK_EVENT);
    }
    if (open_err != 0) {
        xhv_host_log("xhv: no microphone (waveInOpen failed: no recording device)\n");
        host_CloseHandle(g_wave.event);
        g_wave.event = 0;
        g_wave.device = 0;
        return 0;
    }
    for (unsigned i = 0; i < BUFFER_COUNT; i++) {
        wave_header *h = &g_wave.headers[i];
        h->data = (char *)g_wave.buffers[i];
        h->buffer_length = sizeof(g_wave.buffers[i]);
        h->bytes_recorded = 0;
        h->flags = 0;
        g_wave.prepare(g_wave.device, h, sizeof(*h));
        g_wave.add_buffer(g_wave.device, h, sizeof(*h));
    }
    g_wave.next = 0;
    g_wave.consumed = 0;
    g_wave.start(g_wave.device);
    *backend = "winmm";
    return 1;
}

int xhv_host_capture_read(short *pcm, unsigned samples)
{
    unsigned done = 0;
    while (done < samples) {
        wave_header *h = &g_wave.headers[g_wave.next];
        volatile unsigned long *flags = &h->flags; // the driver sets WHDR_DONE from its own thread
        if (!(*flags & WHDR_DONE)) {
            if (host_WaitForSingleObject(g_wave.event, 100) == 0 || (*flags & WHDR_DONE))
                continue;
            if (done == 0)
                return 0; // nothing within 100 ms: the caller checks whether to stop
            while (done < samples) // the device stalled mid-frame: the rest is silence, never a hang
                pcm[done++] = 0;
            return 1;
        }
        unsigned available = (unsigned)(h->bytes_recorded / 2u);
        while (done < samples && g_wave.consumed < available)
            pcm[done++] = g_wave.buffers[g_wave.next][g_wave.consumed++];
        if (g_wave.consumed >= available) {
            h->flags &= ~WHDR_DONE;
            h->bytes_recorded = 0;
            g_wave.add_buffer(g_wave.device, h, sizeof(*h));
            g_wave.next = (g_wave.next + 1u) % BUFFER_COUNT;
            g_wave.consumed = 0;
        }
    }
    return 1;
}

void xhv_host_capture_close(void)
{
    if (g_wave.device == 0)
        return;
    g_wave.reset(g_wave.device);
    for (unsigned i = 0; i < BUFFER_COUNT; i++)
        g_wave.unprepare(g_wave.device, &g_wave.headers[i], sizeof(g_wave.headers[i]));
    g_wave.close(g_wave.device);
    g_wave.device = 0;
    host_CloseHandle(g_wave.event);
    g_wave.event = 0;
}
