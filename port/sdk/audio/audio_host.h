// port/sdk/audio (both hosts): what audio_output.c needs from the host (audio_linux.c, audio_windows.c).
#ifndef PORT_AUDIO_HOST_H
#define PORT_AUDIO_HOST_H

// A sink takes interleaved float frames of `channels` channels. write blocks until the device has room (that is the
// pacing) and returns 0 if the device failed. exact: a file, which takes only rendered frames (no padding).
typedef struct port_audio_sink {
    const char *name;
    unsigned channels;
    int exact;
    int (*write)(const float *frames, unsigned count);
    void (*close)(void);
} port_audio_sink;

// Opens a device sink: the backend named (`forced`: "pulse", "alsa", "pipe", "winmm"), or the first that works.
int port_audio_host_open(const char *forced, unsigned channels, port_audio_sink *sink);

void port_audio_host_thread_start(void (*main)(void));
void port_audio_host_thread_join(void);
void port_audio_host_sleep_us(unsigned microseconds);
unsigned long long port_audio_host_time_us(void);

void *port_audio_host_file_create(const char *path);
int port_audio_host_file_write(void *file, const void *data, unsigned bytes);
// writes HEADER over the start of the file, then puts the file position back at its end and flushes, so the file is
// complete on disk up to here even if the process is killed before the close
void port_audio_host_file_update_header(void *file, const void *header, unsigned bytes);
void port_audio_host_file_close(void *file);

#endif
