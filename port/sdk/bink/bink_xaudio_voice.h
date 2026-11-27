// port/sdk/bink (both hosts): the Bink sound driver's XAudio calls (bink_xaudio_voice.cpp), in plain types for the
// driver's unit (bink_xaudio.cpp), which cannot include the SDK's xaudio.h next to include/bink/binkxa.h.
#ifndef PORT_BINK_XAUDIO_VOICE_H
#define PORT_BINK_XAUDIO_VOICE_H

#ifdef __cplusplus
extern "C" {
#endif

// A PCM source voice (rate, 1 or 2 channels, 16 or 8 bits) with the default output, or 0.
void *bink_xaudio_voice_create(unsigned rate, unsigned channels, unsigned bits);
// Flushes the voice and queues the ring [buffer, buffer + bytes) as one packet looping for ever from its start.
void bink_xaudio_voice_submit_ring(void *voice, void *buffer, unsigned bytes);
// The byte offset in the ring the voice plays next (block_align: bytes of one sample of all channels).
unsigned bink_xaudio_voice_position(void *voice, unsigned block_align);
void bink_xaudio_voice_set_volume(void *voice, float volume);
void bink_xaudio_voice_set_pitch(void *voice, float octaves);
// One output, the mastering voice: volumes[c * 6 + bin] from input channel c (channels <= 2) to each mix bin.
void bink_xaudio_voice_set_output(void *voice, unsigned channels, const float *volumes);

#ifdef __cplusplus
}
#endif

#endif
