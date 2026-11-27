// port/sdk/audio (both hosts): the host audio output under the XAudio engine (xaudio_engine.cpp), and the Waves DSP
// module stand-ins the game loads through LoadLibraryA (waves_bypass.c, called by port/sdk/kernel/xapi_system.c).
// Host code (port/sdk/audio/audio_*.c, waves_bypass.c); the interface is ints, floats and pointers only, so the
// MS-ABI engine and the host runtime agree on it on both targets.
//
// Output model: the engine renders XAudio frames (256 samples at 48 kHz) into a lock-free single-producer /
// single-consumer ring of interleaved float frames; a host thread drains the ring into the device. The device's own
// blocking write paces the drain, the ring's free space paces the engine, so voices play in real time with a fixed,
// small latency (ring PORT_AUDIO_RING_FRAMES plus the device buffer). With no device (none found, or
// HALO3_AUDIO_BACKEND=null) a null sink drains the ring at the wall-clock rate, so the game's timing is unchanged.
//
// Backends, in order (HALO3_AUDIO_BACKEND forces one): Linux: PulseAudio / PipeWire-pulse through
// libpulse-simple.so.0, ALSA through libasound.so.2 (both dlopened, 32-bit), then a pw-cat / pacat child process fed
// through a pipe (no 32-bit library needed), then null. Windows: winmm waveOut (float, else 16-bit PCM), then null.
// HALO3_AUDIO_BACKEND=wav:PATH renders as fast as the engine can into a WAV file (tests); wavrt:PATH writes the WAV
// at the wall-clock rate, like a device, with silence where the engine fell behind (captures of a run). Channels: stereo unless
// HALO3_AUDIO_CHANNELS=6 asks the host for 5.1 (FL FR FC LFE BL BR, the Xbox order).
#ifndef PORT_AUDIO_H
#define PORT_AUDIO_H

#ifdef __cplusplus
extern "C" {
#endif

#define PORT_AUDIO_RATE 48000u
#define PORT_AUDIO_RING_FRAMES 2048u // 42.7 ms: the engine renders at most this far ahead of the device

// 1 when HALO3_NO_AUDIO=1: XAudioInitialize declines, as on a console with audio disabled.
int PORT_MS_ABI port_audio_disabled(void);
// Opens the output (idempotent). Returns the channel count frames must have (2 or 6); never fails: without a device
// the null sink takes the frames.
unsigned PORT_MS_ABI port_audio_open(void);
// Frames the ring can take now.
unsigned PORT_MS_ABI port_audio_writable(void);
// Appends interleaved frames (at most port_audio_writable()); returns the count taken.
unsigned PORT_MS_ABI port_audio_write(const float *frames, unsigned count);
// Stops the drain thread and closes the device (any frames still queued are dropped).
void PORT_MS_ABI port_audio_close(void);
// "pulse", "alsa", "pipe", "winmm", "wav", "null": for the log.
const char * PORT_MS_ABI port_audio_backend(void);
// Frames the device asked for that the ring could not supply (heard as silence), since open.
unsigned PORT_MS_ABI port_audio_underruns(void);

// ---- Waves DSP modules (WavesLibDLL, WaveShell-Xbox, l360, q10: PowerPC DLLs on the disc) ----
// A module handle for one of the four names the game loads, or 0 for any other name. The modules export, by
// ordinal, the functions sound_xaudio_xenon.cpp and state_variable_filter_effect.cpp import; here they run host
// implementations of the plug-ins (Q10 equalizer, L360 limiter) on the presets the game registers, and report the
// versions the game checks.
void *port_waves_load_library(const char *name);
// The export of a module from port_waves_load_library by ordinal (a name pointer below 0x10000), or 0.
void *port_waves_get_procedure(void *module, const char *name);

// ---- host services the engine uses (audio_linux.c / audio_windows.c) ----
// Copies the value of an environment variable (truncated to capacity - 1); returns 0 if it is not set.
int PORT_MS_ABI port_audio_getenv(const char *name, char *value, unsigned capacity);
// Writes a line to the log (stderr).
void PORT_MS_ABI port_audio_log(const char *text);

// ---- HALO3_AUDIO_TRACE=1 (xaudio_engine.cpp: "audio: " lines of what every voice plays) ----
// 1 when the trace is on (after the engine started); the trace's clock, milliseconds of audio the engine rendered, so
// other code (a diagnostic build of the game) can add lines that join the engine's through port_audio_log.
int PORT_MS_ABI port_audio_trace_enabled(void);
unsigned PORT_MS_ABI port_audio_trace_time_ms(void);

#ifdef __cplusplus
}
#endif

#endif
