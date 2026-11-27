// port/sdk/audio (Linux host): audio devices for audio_output.c (interface: audio_host.h).
//
// The game is a 32-bit process, and a 64-bit desktop may not have the 32-bit sound libraries, so each backend is
// optional and loaded at run time (RTLD_LOCAL: their symbols stay out of the game's namespace):
//   pulse  libpulse-simple.so.0: PulseAudio, or PipeWire's pulse server (the usual desktop path);
//   alsa   libasound.so.2, device "default" (PipeWire / Pulse ALSA plugins, or the card itself);
//   pipe   no 32-bit library at all: a pw-cat (else pacat) child process reads raw float frames from a pipe; any
//          64-bit PipeWire / PulseAudio client tool works, as the protocol stays inside it.
// Blocking writes pace the drain thread: pa_simple_write and snd_pcm_writei block on the device buffer; the pipe is
// non-blocking with poll(), so a child that dies is noticed (then the output falls back to the null sink).
#define _GNU_SOURCE
#include "port_audio.h"
#include "audio_host.h"
#include "../runtime/port_runtime.h"

#include <dlfcn.h>
#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <pthread.h>
#include <signal.h>
#include <spawn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

extern char **environ;

int PORT_MS_ABI port_audio_getenv(const char *name, char *value, unsigned capacity)
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

void PORT_MS_ABI port_audio_log(const char *text) { port_write_error(text); }

// ---- threads, time, files ----

static pthread_t g_drain_thread;
static void (*g_drain_main)(void);

static void *drain_entry(void *parameter)
{
    (void)parameter;
    // writes to a pipe whose reader died fail with EPIPE instead of a process-wide SIGPIPE
    sigset_t pipe_signal;
    sigemptyset(&pipe_signal);
    sigaddset(&pipe_signal, SIGPIPE);
    pthread_sigmask(SIG_BLOCK, &pipe_signal, 0);
    g_drain_main();
    return 0;
}

void port_audio_host_thread_start(void (*main)(void))
{
    g_drain_main = main;
    pthread_create(&g_drain_thread, 0, drain_entry, 0);
}

void port_audio_host_thread_join(void) { pthread_join(g_drain_thread, 0); }

void port_audio_host_sleep_us(unsigned microseconds)
{
    struct timespec t = {(time_t)(microseconds / 1000000u), (long)(microseconds % 1000000u) * 1000L};
    while (nanosleep(&t, &t) != 0 && errno == EINTR)
        ;
}

unsigned long long port_audio_host_time_us(void)
{
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return (unsigned long long)t.tv_sec * 1000000ull + (unsigned long long)t.tv_nsec / 1000ull;
}

void *port_audio_host_file_create(const char *path) { return fopen(path, "wb"); }

int port_audio_host_file_write(void *file, const void *data, unsigned bytes)
{
    return fwrite(data, 1, bytes, (FILE *)file) == bytes;
}

void port_audio_host_file_update_header(void *file, const void *header, unsigned bytes)
{
    fseek((FILE *)file, 0, SEEK_SET);
    fwrite(header, 1, bytes, (FILE *)file);
    fseek((FILE *)file, 0, SEEK_END);
    fflush((FILE *)file);
}

void port_audio_host_file_close(void *file) { fclose((FILE *)file); }

// ---- PulseAudio (simple API) ----

typedef struct {
    int format;
    unsigned rate;
    unsigned char channels;
} pa_sample_spec_t;
typedef struct {
    unsigned char channels;
    int map[32];
} pa_channel_map_t;
typedef struct {
    unsigned maxlength, tlength, prebuf, minreq, fragsize;
} pa_buffer_attr_t;
#define PA_SAMPLE_FLOAT32LE 5
#define PA_STREAM_PLAYBACK 1

static struct {
    void *library;
    void *stream;
    void *(*simple_new)(const char *, const char *, int, const char *, const char *, const pa_sample_spec_t *,
        const pa_channel_map_t *, const pa_buffer_attr_t *, int *);
    int (*simple_write)(void *, const void *, size_t, int *);
    void (*simple_free)(void *);
    unsigned channels;
} g_pulse;

static int pulse_write(const float *frames, unsigned count)
{
    int error = 0;
    return g_pulse.simple_write(g_pulse.stream, frames, (size_t)count * g_pulse.channels * 4, &error) == 0;
}

static void pulse_close(void)
{
    g_pulse.simple_free(g_pulse.stream);
    g_pulse.stream = 0;
}

static int open_pulse(unsigned channels, port_audio_sink *sink)
{
    if (g_pulse.library == 0)
        g_pulse.library = dlopen("libpulse-simple.so.0", RTLD_NOW | RTLD_LOCAL);
    if (g_pulse.library == 0)
        return 0;
    *(void **)&g_pulse.simple_new = dlsym(g_pulse.library, "pa_simple_new");
    *(void **)&g_pulse.simple_write = dlsym(g_pulse.library, "pa_simple_write");
    *(void **)&g_pulse.simple_free = dlsym(g_pulse.library, "pa_simple_free");
    if (!g_pulse.simple_new || !g_pulse.simple_write || !g_pulse.simple_free)
        return 0;
    pa_sample_spec_t spec = {PA_SAMPLE_FLOAT32LE, PORT_AUDIO_RATE, (unsigned char)channels};
    // FL FR FC LFE RL RR (pa_channel_position_t)
    pa_channel_map_t map = {6, {1, 2, 3, 7, 5, 6}};
    // 30 ms in the server, written 5.3 ms at a time
    unsigned frame_bytes = channels * 4;
    pa_buffer_attr_t attr = {(unsigned)-1, PORT_AUDIO_RATE * 30 / 1000 * frame_bytes, (unsigned)-1,
        256 * frame_bytes, (unsigned)-1};
    int error = 0;
    g_pulse.stream = g_pulse.simple_new(0, "Halo 3", PA_STREAM_PLAYBACK, 0, "game audio", &spec,
        channels == 6 ? &map : 0, &attr, &error);
    if (g_pulse.stream == 0)
        return 0;
    g_pulse.channels = channels;
    sink->name = "pulse";
    sink->channels = channels;
    sink->exact = 0;
    sink->write = pulse_write;
    sink->close = pulse_close;
    return 1;
}

// ---- ALSA ----

#define SND_PCM_STREAM_PLAYBACK 0
#define SND_PCM_FORMAT_FLOAT_LE 14
#define SND_PCM_ACCESS_RW_INTERLEAVED 3

static struct {
    void *library;
    void *pcm;
    int (*pcm_open)(void **, const char *, int, int);
    int (*pcm_set_params)(void *, int, int, unsigned, unsigned, int, unsigned);
    long (*pcm_writei)(void *, const void *, unsigned long);
    int (*pcm_recover)(void *, int, int);
    int (*pcm_close)(void *);
    unsigned channels;
} g_alsa;

static int alsa_write(const float *frames, unsigned count)
{
    while (count != 0) {
        long n = g_alsa.pcm_writei(g_alsa.pcm, frames, count);
        if (n < 0) {
            if (g_alsa.pcm_recover(g_alsa.pcm, (int)n, 1) < 0)
                return 0;
            continue;
        }
        frames += (unsigned long)n * g_alsa.channels;
        count -= (unsigned)n;
    }
    return 1;
}

static void alsa_close(void)
{
    g_alsa.pcm_close(g_alsa.pcm);
    g_alsa.pcm = 0;
}

static int open_alsa(unsigned channels, port_audio_sink *sink)
{
    if (g_alsa.library == 0)
        g_alsa.library = dlopen("libasound.so.2", RTLD_NOW | RTLD_LOCAL);
    if (g_alsa.library == 0)
        return 0;
    *(void **)&g_alsa.pcm_open = dlsym(g_alsa.library, "snd_pcm_open");
    *(void **)&g_alsa.pcm_set_params = dlsym(g_alsa.library, "snd_pcm_set_params");
    *(void **)&g_alsa.pcm_writei = dlsym(g_alsa.library, "snd_pcm_writei");
    *(void **)&g_alsa.pcm_recover = dlsym(g_alsa.library, "snd_pcm_recover");
    *(void **)&g_alsa.pcm_close = dlsym(g_alsa.library, "snd_pcm_close");
    if (!g_alsa.pcm_open || !g_alsa.pcm_set_params || !g_alsa.pcm_writei || !g_alsa.pcm_recover || !g_alsa.pcm_close)
        return 0;
    if (g_alsa.pcm_open(&g_alsa.pcm, "default", SND_PCM_STREAM_PLAYBACK, 0) < 0)
        return 0;
    // resampling allowed, 40 ms latency
    if (g_alsa.pcm_set_params(g_alsa.pcm, SND_PCM_FORMAT_FLOAT_LE, SND_PCM_ACCESS_RW_INTERLEAVED, channels,
            PORT_AUDIO_RATE, 1, 40000) < 0) {
        g_alsa.pcm_close(g_alsa.pcm);
        g_alsa.pcm = 0;
        return 0;
    }
    g_alsa.channels = channels;
    sink->name = "alsa";
    sink->channels = channels;
    sink->exact = 0;
    sink->write = alsa_write;
    sink->close = alsa_close;
    return 1;
}

// ---- pipe to pw-cat / pacat ----

static struct {
    int fd;
    pid_t child;
    unsigned channels;
} g_pipe = {-1, 0, 0};

static int pipe_child_alive(void)
{
    int status;
    return waitpid(g_pipe.child, &status, WNOHANG) == 0;
}

static int pipe_write(const float *frames, unsigned count)
{
    const unsigned char *bytes = (const unsigned char *)frames;
    size_t left = (size_t)count * g_pipe.channels * 4;
    while (left != 0) {
        ssize_t n = write(g_pipe.fd, bytes, left);
        if (n > 0) {
            bytes += n;
            left -= (size_t)n;
            continue;
        }
        if (n < 0 && errno != EAGAIN && errno != EINTR)
            return 0; // EPIPE: the player exited
        struct pollfd p = {g_pipe.fd, POLLOUT, 0};
        if (poll(&p, 1, 250) == 0 && !pipe_child_alive())
            return 0;
        if (p.revents & (POLLERR | POLLHUP))
            return 0;
    }
    return 1;
}

static void pipe_close(void)
{
    close(g_pipe.fd);
    g_pipe.fd = -1;
    int status;
    for (int i = 0; i < 100 && waitpid(g_pipe.child, &status, WNOHANG) == 0; i++)
        port_audio_host_sleep_us(10000);
    if (waitpid(g_pipe.child, &status, WNOHANG) == 0) {
        kill(g_pipe.child, SIGTERM);
        waitpid(g_pipe.child, &status, 0);
    }
}

static int open_pipe(unsigned channels, port_audio_sink *sink)
{
    static const char *const k_players[] = {"/usr/bin/pw-cat", "/usr/local/bin/pw-cat", "/usr/bin/pacat",
        "/usr/local/bin/pacat"};
    const char *player = 0;
    for (unsigned i = 0; i < sizeof(k_players) / sizeof(k_players[0]) && player == 0; i++)
        if (access(k_players[i], X_OK) == 0)
            player = k_players[i];
    if (player == 0)
        return 0;
    char channel_text[8], rate_text[16];
    snprintf(channel_text, sizeof(channel_text), "%u", channels);
    snprintf(rate_text, sizeof(rate_text), "%u", PORT_AUDIO_RATE);
    char channel_option[32], rate_option[32];
    snprintf(channel_option, sizeof(channel_option), "--channels=%u", channels);
    snprintf(rate_option, sizeof(rate_option), "--rate=%u", PORT_AUDIO_RATE);
    int is_pw = strstr(player, "pw-cat") != 0;
    char *pw_args[] = {(char *)"pw-cat", (char *)"--playback", (char *)"--raw", (char *)"--rate", rate_text,
        (char *)"--channels", channel_text, (char *)"--format", (char *)"f32", (char *)"--latency", (char *)"30ms",
        (char *)"--media-role", (char *)"Game", (char *)"-", 0};
    char *pa_args[] = {(char *)"pacat", (char *)"--playback", (char *)"--raw", rate_option, channel_option,
        (char *)"--format=float32le", (char *)"--latency-msec=30", (char *)"--client-name=Halo 3", 0};
    int fds[2];
    if (pipe2(fds, O_CLOEXEC) != 0)
        return 0;
    posix_spawn_file_actions_t actions;
    posix_spawn_file_actions_init(&actions);
    posix_spawn_file_actions_adddup2(&actions, fds[0], 0);
    pid_t child;
    int failed = posix_spawn(&child, player, &actions, 0, is_pw ? pw_args : pa_args, environ);
    posix_spawn_file_actions_destroy(&actions);
    close(fds[0]);
    if (failed != 0) {
        close(fds[1]);
        return 0;
    }
    fcntl(fds[1], F_SETPIPE_SZ, 16384); // ~21 ms of stereo in the pipe
    fcntl(fds[1], F_SETFL, fcntl(fds[1], F_GETFL) | O_NONBLOCK);
    g_pipe.fd = fds[1];
    g_pipe.child = child;
    g_pipe.channels = channels;
    port_audio_host_sleep_us(20000);
    if (!pipe_child_alive()) {
        close(g_pipe.fd);
        g_pipe.fd = -1;
        return 0;
    }
    sink->name = "pipe";
    sink->channels = channels;
    sink->exact = 0;
    sink->write = pipe_write;
    sink->close = pipe_close;
    return 1;
}

int port_audio_host_open(const char *forced, unsigned channels, port_audio_sink *sink)
{
    if (forced != 0) {
        if (strcmp(forced, "pulse") == 0)
            return open_pulse(channels, sink);
        if (strcmp(forced, "alsa") == 0)
            return open_alsa(channels, sink);
        if (strcmp(forced, "pipe") == 0)
            return open_pipe(channels, sink);
        port_audio_log("port: audio: unknown HALO3_AUDIO_BACKEND; trying them in order\n");
    }
    return open_pulse(channels, sink) || open_alsa(channels, sink) || open_pipe(channels, sink);
}
