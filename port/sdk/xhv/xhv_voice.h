// port/sdk/xhv (host code): the voice runtime under the XHV engine (xhv_engine.cpp): the local talker's microphone
// (or a synthetic test voice) through voice activity detection and the codec (xhv_codec.h) into chat packets, and
// each remote talker's packets through a jitter buffer and the decoder into 16 kHz PCM. Host C (xhv_voice.c), so its
// interface is ints and pointers only, the same on both ABIs.
//
// Threads: a capture thread of the runtime's own reads the source and encodes; the game's thread queues and takes
// packets; the XAudio engine's thread renders remote talkers. Every queue between two of them is single-producer,
// single-consumer and lock-free.
//
// Environment (read once, by xhv_voice_open):
//   HALO3_VOICE=1             the host's default microphone is player 1's headset (opt-in: the port never opens a
//                             microphone unless asked). Never with HALO3_AUDIO_BACKEND=null, wav:, wavrt: or
//                             HALO3_NO_AUDIO=1: headless and file-backed runs do not touch host audio devices.
//   HALO3_VOICE=0             no voice at all (also overrides HALO3_VOICE_TEST_TONE).
//   HALO3_VOICE_TEST_TONE=1   player 1's headset speaks a synthetic voice (talk spurts and pauses) instead of a
//                             microphone, with any audio backend: for tests and headless runs.
//   HALO3_VOICE_TRACE=1       "xhv: " log lines for talk spurts sent and played.
#ifndef PORT_XHV_VOICE_H
#define PORT_XHV_VOICE_H

#include "../runtime/port_abi.h"

#ifdef __cplusplus
extern "C" {
#endif

#define XHV_VOICE_USERS 4    // XHV_MAX_LOCAL_TALKERS
#define XHV_VOICE_TALKERS 30 // XHV_MAX_REMOTE_TALKERS
#define XHV_VOICE_PACKET_BYTES 10
#define XHV_VOICE_RATE 16000
#define XHV_VOICE_FRAME_SAMPLES 320 // 20 ms
#define XHV_VOICE_DEVICE_ID_MAX 256
#define XHV_VOICE_DEVICE_NAME_MAX 64
#define XHV_VOICE_MAX_DEVICES 16

typedef struct xhv_voice_device {
    char id[XHV_VOICE_DEVICE_ID_MAX];
    char name[XHV_VOICE_DEVICE_NAME_MAX];
} xhv_voice_device;

// Enumerates host microphone capture devices available for selection in Settings. Returns count written (0..capacity).
unsigned PORT_MS_ABI xhv_voice_devices(xhv_voice_device *out, unsigned capacity);

// Starts the voice source the environment / halo3.ini asks for (idempotent; one source per process). Returns the mask
// of local users with a headset (bit 0 when a source runs, else 0).
unsigned PORT_MS_ABI xhv_voice_open(void);
// Re-applies the configured microphone setting live (stops/switches/opens capture if changed). Returns the new mask.
unsigned PORT_MS_ABI xhv_voice_refresh(void);
// Stops the capture thread and closes the device.
void PORT_MS_ABI xhv_voice_close(void);
unsigned PORT_MS_ABI xhv_voice_headset_mask(void);

// Whether `user` takes part in chat (registered, chat mode started): only then are its talk spurts packetized.
void PORT_MS_ABI xhv_voice_local_enable(unsigned user, int enabled);
// The voice activity detector's state for `user` (with its hangover): the HUD's speaker icon.
int PORT_MS_ABI xhv_voice_local_talking(unsigned user);
// Packets queued for `user`.
unsigned PORT_MS_ABI xhv_voice_local_packets(unsigned user);
// Moves up to `max_packets` of `user`'s oldest packets (XHV_VOICE_PACKET_BYTES each) to `data`; returns the count.
// A queue the game has not drained for long is cut to its newest 16 packets first (160 ms).
unsigned PORT_MS_ABI xhv_voice_local_read(unsigned user, unsigned char *data, unsigned max_packets);
// Total local chat packets encoded since the current voice source opened (diagnostics / tests).
unsigned PORT_MS_ABI xhv_voice_local_packets_sent(void);

// Remote talkers, by slot (0..XHV_VOICE_TALKERS-1, the engine's). open before the first submit or render; close
// only once nothing renders the slot any more.
void PORT_MS_ABI xhv_voice_remote_open(unsigned slot);
void PORT_MS_ABI xhv_voice_remote_close(unsigned slot);
// Queues `packets` packets (XHV_VOICE_PACKET_BYTES each) that arrived for the talker; returns how many were taken
// (all of them: a full queue drops what does not fit, as stale voice is worth nothing).
unsigned PORT_MS_ABI xhv_voice_remote_submit(unsigned slot, const unsigned char *data, unsigned packets);
// The talker's next 20 ms (XHV_VOICE_FRAME_SAMPLES at XHV_VOICE_RATE): decoded voice, concealment of a lost frame
// fading out, or silence. Returns 1 when the frame holds voice. Called at the playback rate (the XAudio thread).
int PORT_MS_ABI xhv_voice_remote_render(unsigned slot, short *pcm);
// Whether the talker is being heard (voice decoded within the last 200 ms; when nothing renders the slot, packets
// arrived within the last 200 ms).
int PORT_MS_ABI xhv_voice_remote_talking(unsigned slot);
// Total frames decoded or packets received for `slot` (diagnostics / tests).
unsigned PORT_MS_ABI xhv_voice_remote_frames_decoded(unsigned slot);
unsigned PORT_MS_ABI xhv_voice_remote_packets_received(unsigned slot);

#ifdef __cplusplus
}
#endif

#endif
