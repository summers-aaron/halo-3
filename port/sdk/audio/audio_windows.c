// port/sdk/audio (Windows host): audio devices for audio_output.c (interface: audio_host.h).
//
// winmm's waveOut, loaded at run time (winmm.dll is on every Windows and in Wine): the wave mapper, 32-bit float
// (WAVE_FORMAT_EXTENSIBLE, then WAVE_FORMAT_IEEE_FLOAT), else 16-bit PCM. Four 512-frame buffers circulate (43 ms
// queued); the drain thread waits on the driver's event for a buffer to come back, which paces it. Win32 itself is
// reached through host_ imports (port/sdk/kernel/win32_host.h): the plain names are the Xbox API the port defines.
#include "port_audio.h"
#include "audio_host.h"
#include "../kernel/win32_host.h"
#include "../runtime/port_runtime.h"

PORT_HOST_IMPORT(void *, LoadLibraryA, 4, (const char *name));
PORT_HOST_IMPORT(void *, GetProcAddress, 8, (void *module, const char *name));
PORT_HOST_IMPORT(void *, CreateThread, 24, (void *security, __SIZE_TYPE__ stack, unsigned long(__stdcall *start)(void *),
    void *parameter, unsigned long flags, unsigned long *id));
PORT_HOST_IMPORT(unsigned long, WaitForSingleObject, 8, (void *handle, unsigned long milliseconds));
PORT_HOST_IMPORT(int, CloseHandle, 4, (void *handle));
PORT_HOST_IMPORT(void, Sleep, 4, (unsigned long milliseconds));
PORT_HOST_IMPORT(void *, CreateEventA, 16, (void *security, int manual_reset, int initial_state, const char *name));
PORT_HOST_IMPORT(int, QueryPerformanceCounter, 4, (long long *count));
PORT_HOST_IMPORT(int, QueryPerformanceFrequency, 4, (long long *frequency));
PORT_HOST_IMPORT(void *, CreateFileA, 28, (const char *name, unsigned long access, unsigned long share, void *security,
    unsigned long disposition, unsigned long flags, void *template_file));
PORT_HOST_IMPORT(int, WriteFile, 20, (void *file, const void *data, unsigned long bytes, unsigned long *written,
    void *overlapped));
PORT_HOST_IMPORT(unsigned long, SetFilePointer, 16, (void *file, long distance, long *distance_high, unsigned long method));

__declspec(dllimport) char *__cdecl getenv(const char *name);

int port_audio_getenv(const char *name, char *value, unsigned capacity)
{
    const char *v = getenv(name);
    if (v == 0 || capacity == 0)
        return 0;
    unsigned i = 0;
    for (; v[i] != 0 && i + 1 < capacity; i++)
        value[i] = v[i];
    value[i] = 0;
    return 1;
}

void port_audio_log(const char *text) { port_write_error(text); }

// ---- threads, time, files ----

static void *g_drain_thread;
static void (*g_drain_main)(void);

static unsigned long __stdcall drain_entry(void *parameter)
{
    (void)parameter;
    g_drain_main();
    return 0;
}

void port_audio_host_thread_start(void (*main)(void))
{
    g_drain_main = main;
    g_drain_thread = host_CreateThread(0, 0, drain_entry, 0, 0, 0);
}

void port_audio_host_thread_join(void)
{
    host_WaitForSingleObject(g_drain_thread, 0xffffffffu);
    host_CloseHandle(g_drain_thread);
    g_drain_thread = 0;
}

void port_audio_host_sleep_us(unsigned microseconds) { host_Sleep(microseconds < 1000 ? 1 : microseconds / 1000); }

unsigned long long port_audio_host_time_us(void)
{
    long long count, frequency;
    host_QueryPerformanceCounter(&count);
    host_QueryPerformanceFrequency(&frequency);
    return (unsigned long long)(count / frequency) * 1000000ull +
           (unsigned long long)(count % frequency) * 1000000ull / (unsigned long long)frequency;
}

#define HOST_INVALID_HANDLE ((void *)(__INTPTR_TYPE__)-1)

void *port_audio_host_file_create(const char *path)
{
    void *file = host_CreateFileA(path, 0x40000000ul /* GENERIC_WRITE */, 0, 0, 2 /* CREATE_ALWAYS */, 0x80, 0);
    return file == HOST_INVALID_HANDLE ? 0 : file;
}

int port_audio_host_file_write(void *file, const void *data, unsigned bytes)
{
    unsigned long written = 0;
    return host_WriteFile(file, data, bytes, &written, 0) && written == bytes;
}

void port_audio_host_file_update_header(void *file, const void *header, unsigned bytes)
{
    host_SetFilePointer(file, 0, 0, 0 /* FILE_BEGIN */);
    port_audio_host_file_write(file, header, bytes);
    host_SetFilePointer(file, 0, 0, 2 /* FILE_END */);
}

void port_audio_host_file_close(void *file) { host_CloseHandle(file); }

// ---- waveOut ----

#pragma pack(push, 1)
typedef struct {
    unsigned short format_tag, channels;
    unsigned long samples_per_second, average_bytes_per_second;
    unsigned short block_align, bits_per_sample, extra_size;
} wave_format_ex;
typedef struct {
    wave_format_ex format;
    unsigned short valid_bits_per_sample;
    unsigned long channel_mask;
    unsigned char sub_format[16];
} wave_format_extensible;
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
typedef char wave_format_extensible_size[sizeof(wave_format_extensible) == 40 ? 1 : -1];
typedef char wave_header_size[sizeof(wave_header) == (sizeof(void *) == 8 ? 48 : 32) ? 1 : -1];

#define WAVE_MAPPER 0xffffffffu
#define CALLBACK_EVENT 0x00050000ul
#define WHDR_DONE 0x00000001ul
#define BUFFER_COUNT 4
#define BUFFER_FRAMES 512

// KSDATAFORMAT_SUBTYPE_IEEE_FLOAT and _PCM: {0000000x-0000-0010-8000-00aa00389b71}
static const unsigned char k_subtype_float[16] = {3, 0, 0, 0, 0, 0, 0x10, 0, 0x80, 0, 0, 0xaa, 0, 0x38, 0x9b, 0x71};

static struct {
    void *library;
    void *device;
    void *event;
    unsigned (__stdcall *out_open)(void **, unsigned, const wave_format_ex *, __UINTPTR_TYPE__, __UINTPTR_TYPE__, unsigned long);
    unsigned (__stdcall *out_prepare)(void *, wave_header *, unsigned);
    unsigned (__stdcall *out_unprepare)(void *, wave_header *, unsigned);
    unsigned (__stdcall *out_write)(void *, wave_header *, unsigned);
    unsigned (__stdcall *out_reset)(void *);
    unsigned (__stdcall *out_close)(void *);
    unsigned channels;
    int pcm16;
    wave_header headers[BUFFER_COUNT];
    int queued[BUFFER_COUNT];
    float pending[BUFFER_FRAMES * 6];
    unsigned pending_frames;
    unsigned next;
} g_wave;
static unsigned char g_wave_memory[BUFFER_COUNT][BUFFER_FRAMES * 6 * 4];

// Queues g_wave.pending into the next free buffer, waiting for one to come back if all are queued.
static int wave_submit(void)
{
    wave_header *h = &g_wave.headers[g_wave.next];
    while (g_wave.queued[g_wave.next] && !(h->flags & WHDR_DONE)) {
        if (host_WaitForSingleObject(g_wave.event, 500) != 0 && !(h->flags & WHDR_DONE))
            return 0; // the driver stopped returning buffers
    }
    if (g_wave.queued[g_wave.next])
        g_wave.out_unprepare(g_wave.device, h, sizeof(*h));
    unsigned samples = BUFFER_FRAMES * g_wave.channels;
    if (g_wave.pcm16) {
        short *out = (short *)g_wave_memory[g_wave.next];
        for (unsigned i = 0; i < samples; i++) {
            float v = g_wave.pending[i] * 32767.0f;
            out[i] = (short)(v > 32767.0f ? 32767 : v < -32768.0f ? -32768 : (int)v);
        }
    } else {
        __builtin_memcpy(g_wave_memory[g_wave.next], g_wave.pending, samples * 4);
    }
    __builtin_memset(h, 0, sizeof(*h));
    h->data = (char *)g_wave_memory[g_wave.next];
    h->buffer_length = samples * (g_wave.pcm16 ? 2u : 4u);
    if (g_wave.out_prepare(g_wave.device, h, sizeof(*h)) != 0 || g_wave.out_write(g_wave.device, h, sizeof(*h)) != 0)
        return 0;
    g_wave.queued[g_wave.next] = 1;
    g_wave.next = (g_wave.next + 1) % BUFFER_COUNT;
    return 1;
}

static int wave_write(const float *frames, unsigned count)
{
    while (count != 0) {
        unsigned take = BUFFER_FRAMES - g_wave.pending_frames;
        if (take > count)
            take = count;
        __builtin_memcpy(&g_wave.pending[g_wave.pending_frames * g_wave.channels], frames, take * g_wave.channels * 4);
        g_wave.pending_frames += take;
        frames += take * g_wave.channels;
        count -= take;
        if (g_wave.pending_frames == BUFFER_FRAMES) {
            g_wave.pending_frames = 0;
            if (!wave_submit())
                return 0;
        }
    }
    return 1;
}

static void wave_close(void)
{
    g_wave.out_reset(g_wave.device);
    for (int i = 0; i < BUFFER_COUNT; i++)
        if (g_wave.queued[i])
            g_wave.out_unprepare(g_wave.device, &g_wave.headers[i], sizeof(wave_header));
    g_wave.out_close(g_wave.device);
    g_wave.device = 0;
    host_CloseHandle(g_wave.event);
    g_wave.event = 0;
}

static int wave_try(unsigned channels, int extensible, int pcm16)
{
    wave_format_extensible f;
    __builtin_memset(&f, 0, sizeof(f));
    f.format.format_tag = extensible ? 0xfffe : pcm16 ? 1 : 3;
    f.format.channels = (unsigned short)channels;
    f.format.samples_per_second = PORT_AUDIO_RATE;
    f.format.bits_per_sample = pcm16 ? 16 : 32;
    f.format.block_align = (unsigned short)(channels * f.format.bits_per_sample / 8);
    f.format.average_bytes_per_second = PORT_AUDIO_RATE * f.format.block_align;
    if (extensible) {
        f.format.extra_size = 22;
        f.valid_bits_per_sample = 32;
        f.channel_mask = channels == 6 ? 0x3f : 0x3; // FL FR FC LFE BL BR / FL FR
        __builtin_memcpy(f.sub_format, k_subtype_float, 16);
    }
    return g_wave.out_open(&g_wave.device, WAVE_MAPPER, &f.format, (__UINTPTR_TYPE__)g_wave.event, 0, CALLBACK_EVENT) == 0;
}

static int open_winmm(unsigned channels, port_audio_sink *sink)
{
    if (g_wave.library == 0)
        g_wave.library = host_LoadLibraryA("winmm.dll");
    if (g_wave.library == 0)
        return 0;
    *(void **)&g_wave.out_open = host_GetProcAddress(g_wave.library, "waveOutOpen");
    *(void **)&g_wave.out_prepare = host_GetProcAddress(g_wave.library, "waveOutPrepareHeader");
    *(void **)&g_wave.out_unprepare = host_GetProcAddress(g_wave.library, "waveOutUnprepareHeader");
    *(void **)&g_wave.out_write = host_GetProcAddress(g_wave.library, "waveOutWrite");
    *(void **)&g_wave.out_reset = host_GetProcAddress(g_wave.library, "waveOutReset");
    *(void **)&g_wave.out_close = host_GetProcAddress(g_wave.library, "waveOutClose");
    if (!g_wave.out_open || !g_wave.out_prepare || !g_wave.out_unprepare || !g_wave.out_write || !g_wave.out_reset ||
        !g_wave.out_close)
        return 0;
    g_wave.event = host_CreateEventA(0, 0, 0, 0);
    if (g_wave.event == 0)
        return 0;
    g_wave.pcm16 = 0;
    int opened = wave_try(channels, 1, 0) || (channels == 2 && wave_try(2, 0, 0));
    if (!opened && channels == 2) {
        g_wave.pcm16 = 1;
        opened = wave_try(2, 0, 1);
    }
    if (!opened) {
        host_CloseHandle(g_wave.event);
        g_wave.event = 0;
        return 0;
    }
    g_wave.channels = channels;
    g_wave.next = 0;
    g_wave.pending_frames = 0;
    for (int i = 0; i < BUFFER_COUNT; i++)
        g_wave.queued[i] = 0;
    sink->name = "winmm";
    sink->channels = channels;
    sink->exact = 0;
    sink->write = wave_write;
    sink->close = wave_close;
    return 1;
}

int port_audio_host_open(const char *forced, unsigned channels, port_audio_sink *sink)
{
    if (forced != 0 && !(forced[0] == 'w' && forced[1] == 'i' && forced[2] == 'n'))
        port_audio_log("port: audio: unknown HALO3_AUDIO_BACKEND; using winmm\n");
    if (open_winmm(channels, sink))
        return 1;
    // a 5.1 request on a device that refuses it: stereo, folded down
    return channels == 6 && open_winmm(2, sink);
}
