// port/sdk/xhv (host code): what the voice runtime (xhv_voice.c) needs from the host, one implementation per host:
// xhv_capture_linux.c (PulseAudio / PipeWire through libpulse-simple.so.0, else ALSA through libasound.so.2, both
// loaded at run time) and xhv_capture_windows.c (winmm's waveIn, loaded at run time). No link-time dependencies.
#ifndef PORT_XHV_HOST_H
#define PORT_XHV_HOST_H

#ifdef __cplusplus
extern "C" {
#endif

// The value of an environment variable, or 0.
const char *xhv_host_getenv(const char *name);
// A line to the log (stderr); `text` ends with its newline.
void xhv_host_log(const char *text);
// A monotonic clock, in milliseconds.
unsigned xhv_host_time_ms(void);
void xhv_host_sleep_ms(unsigned milliseconds);

// The capture thread (one).
int xhv_host_thread_start(void (*main)(void));
void xhv_host_thread_join(void);

#define XHV_DEVICE_ID_MAX   256
#define XHV_DEVICE_NAME_MAX 64
#define XHV_MAX_DEVICES     16

typedef struct xhv_capture_device {
    char id[XHV_DEVICE_ID_MAX];     // stable identifier saved in halo3.ini
    char name[XHV_DEVICE_NAME_MAX]; // friendly display name for the Guide Settings page
} xhv_capture_device;

// Current configured microphone setting ("off", "default", or a capture device ID). Returns its length.
unsigned xhv_host_mic_device(char *out, unsigned capacity);

// Enumerates available host microphone capture devices (excluding loopback/monitor sources where identifiable).
// Returns the number of devices written (0..capacity). Safe to call whether or not capture is open.
unsigned xhv_host_capture_devices(xhv_capture_device *out, unsigned capacity);

// Opens the requested microphone (`device_id` NULL, "" or "default" = system default), 16 kHz mono 16-bit.
// Returns 1 and the backend's name ("pulse", "alsa", "winmm") when it opened; 0 when none opened (logged).
int xhv_host_capture_open(const char *device_id, const char **backend);
// Blocks until `samples` samples were read (the device paces the capture thread), or about 100 ms passed with none
// (returns 0: the caller checks whether to stop and calls again). Returns -1 when the device failed.
int xhv_host_capture_read(short *pcm, unsigned samples);
void xhv_host_capture_close(void);

#ifdef __cplusplus
}
#endif

#endif
