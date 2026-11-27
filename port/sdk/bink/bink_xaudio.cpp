// port/sdk/bink (both hosts): the rest of binkxa.obj, the Bink library's XAudio sound driver (the half the game has
// decompiled is tu/binkxenon/binkxa.cpp: DS_clear, Lock, Unlock, Volume, Pan, MixBins, SetOnOff, Pause, Close,
// BinkOpenXAudio). Built like the game, against the game's Bink headers, so the names and layouts are the game's;
// the XAudio calls go through bink_xaudio_voice.cpp (binkxa.h and the SDK's xaudio.h cannot share a unit).
//
// The driver, as the retail functions do it (addresses: metadata/judge-inputs/retail-names.tsv):
//   - Open (0x711e88) gives a track a PCM voice and a ring of 0.6 s of samples (rounded up to 1 KB), played as one
//     packet looping for ever, and fills the BINKSND function table. Bink (checksound, bink_read.cpp) writes the
//     ring through Lock / Unlock; the first time the free space (locked_bytes, at first the whole ring or the
//     movie's audio if shorter) is filled, Unlock calls start_streaming.
//   - start_streaming (0x7118e8) submits the ring, starts the voice and waits until the first 50 ms chunk has
//     played (restarting the voice if it has not moved in 50 ms).
//   - Ready (0x7116e0) is Bink's "how much may I write": the bytes from the writer to the play cursor less a 1 KB
//     guard, in 4-byte steps, polled from the voice at most every 12 ms. Under one chunk it lets Bink wait. When the
//     cursor has come round to the writer (90% of the ring free) or the voice was not polled for 540 ms, the track
//     dropped out: two chunks of silence go in ahead of the cursor (sound_dropped_out).
//   - set_rate_volume_and_pan (0x7113b8): XAudio volume (volume / 32768)^(5/3), pitch log2(rate / frequency), and
//     with mix bins set (MixBins / MixBinVolumes) one output to the mastering voice mapping each input channel to
//     the bins.
// SNDCHANNEL's unnamed words (include/bink/binkxa.h) as these functions use them: parameters[0..3] the chunk,
// parameters[4..7] the rate the ring plays at, reserved[0..3] the last play position, reserved[4..7] the
// 90% threshold, start_time the time of the last poll, reserved_time the 540 ms, paused 1 once streaming.
//
// Port differences, each for the port's engine or memory safety:
//   - the engine counts PCM positions in samples; bink_xaudio_voice_position returns bytes (see that file);
//   - MixBinVolumes keeps its bins and volumes in SNDCHANNEL::private_data (the packet the retail driver kept there
//     is copied by the engine). The retail function writes them at BINKSND + 344 / + 392, past the 380-byte entry
//     (bink_sound_entry), into the next track's function table. chunk_bytes and ring_rate live there too: on 64-bit,
//     SNDCHANNEL::buffer widens from 4 to 8 bytes (+4 alignment pad), which shifts SNDCHANNEL::parameters[0..7] onto
//     BINKSND's anonymous union fields volume/pan (offsets 108/112); keeping chunk_bytes and ring_rate in private_data
//     prevents Open / Volume / Pan from clobbering ring_rate with 0x7fff (which pitch-shifted Bink audio to ~0.7x);
//   - start_streaming's wait yields the processor between polls and gives up after a second without movement (a
//     voice the engine never renders would otherwise hang the sound thread).
#include <xtl.h>
#include <math.h>
#include <string.h>

#include "bink/binkread.h"
#include "bink/binkxa.h"
#include "bink/radmem.h"
#include "bink/rad_timer.h"
#include "bink_xaudio_voice.h"

// tu/binkxenon/binkxa.cpp
void DS_clear(SNDCHANNEL *channel, unsigned int start, unsigned int length);
int Lock(BINKSND *sound, unsigned char **buffer, unsigned int *length);
void Volume(BINKSND *sound, int volume);
void Pan(BINKSND *sound, int pan);
void MixBins(BINKSND *sound, unsigned int *bins, unsigned int count);
int SetOnOff(BINKSND *sound, int on);
int Pause(BINKSND *sound, int pause);
void Close(BINKSND *sound);
int Ready(BINKSND *sound);
void MixBinVolumes(BINKSND *sound, unsigned int *bins, int *volumes, unsigned int count);

// @address 0x11df2a8 g_bink_xaudio_voice_count
extern "C" int g_bink_xaudio_voice_count = 0;

namespace {

typedef char binksnd_fits_entry[(sizeof(BINKSND) <= sizeof(bink_sound_entry)) ? 1 : -1];
typedef char binksnd_on_offset[(offsetof(BINKSND, on) == offsetof(bink_sound_entry, on_off)) ? 1 : -1];
typedef char binksnd_channel_offset[(offsetof(BINKSND, channel) == offsetof(bink_sound_entry, sound_data)) ? 1 : -1];

const unsigned k_ring_ms = 600;        // the ring
const unsigned k_chunk_ms = 50;        // Bink's write unit and start_streaming's wait
const unsigned k_guard_bytes = 1024;   // kept free in front of the play cursor
const unsigned k_poll_ms = 12;         // Ready polls the voice at most this often
const unsigned k_dropout_ms = 540;     // a poll this late means the ring ran dry
const unsigned k_start_give_up_ms = 1000;

// Private driver state in SNDCHANNEL::private_data: MixBinVolumes' bins/volumes plus chunk_bytes and ring_rate
// (see the top of the file). All fields are <= 4-byte aligned because private_data sits at offset 4.
struct s_channel_private {
    unsigned char bins[12];
    float volumes[12];
    unsigned chunk_bytes;
    unsigned ring_rate;
};
typedef char channel_private_fit[(sizeof(s_channel_private) <= sizeof(((SNDCHANNEL *)0)->private_data)) ? 1 : -1];
typedef char channel_private_before_volume[
    (offsetof(BINKSND, channel) + offsetof(SNDCHANNEL, private_data) + sizeof(s_channel_private) <=
     offsetof(BINKSND, volume)) ? 1 : -1];
typedef char buffer_size_before_volume[
    (offsetof(BINKSND, channel) + offsetof(SNDCHANNEL, buffer_size) + sizeof(unsigned int) <=
     offsetof(BINKSND, volume)) ? 1 : -1];
typedef char pan_before_paused[
    (offsetof(BINKSND, pan) + sizeof(int) <=
     offsetof(BINKSND, channel) + offsetof(SNDCHANNEL, paused)) ? 1 : -1];
typedef char last_position_before_start_time[
    (offsetof(SNDCHANNEL, initial_bytes) + 3 * sizeof(unsigned) <= offsetof(SNDCHANNEL, start_time)) ? 1 : -1];

inline bink_sound_entry *entry_of(BINKSND *sound) { return (bink_sound_entry *)sound; }
inline void *voice_of(const SNDCHANNEL *channel) { return (void *)(size_t)channel->voice.handle; }
inline s_channel_private *channel_private(SNDCHANNEL *channel) { return (s_channel_private *)channel->private_data; }
inline unsigned &chunk_bytes(SNDCHANNEL *channel) { return channel_private(channel)->chunk_bytes; }
inline unsigned &ring_rate(SNDCHANNEL *channel) { return channel_private(channel)->ring_rate; }
inline unsigned &last_position(SNDCHANNEL *channel) { return *(&channel->initial_bytes + 1); }
inline unsigned &dropout_threshold(SNDCHANNEL *channel) { return *(&channel->initial_bytes + 2); }

unsigned block_align(BINKSND *sound)
{
    const bink_sound_entry *entry = entry_of(sound);
    return (unsigned)((entry->bits >> 3) * entry->channels);
}

// The voice's play position, read until two reads agree within 1 KB (as the retail Ready reads it).
unsigned poll_position(BINKSND *sound)
{
    SNDCHANNEL *channel = &sound->channel;
    const unsigned align = block_align(sound);
    for (;;) {
        const unsigned first = bink_xaudio_voice_position(voice_of(channel), align);
        bink_xaudio_voice_position(voice_of(channel), align);
        const unsigned last = bink_xaudio_voice_position(voice_of(channel), align);
        if (last - first < k_guard_bytes)
            return last;
    }
}

} // namespace

// @address 0x7113b8 ?set_rate_volume_and_pan@@YAXPAUBINKSND@@@Z
void set_rate_volume_and_pan(BINKSND *sound)
{
    SNDCHANNEL *channel = &sound->channel;
    bink_sound_entry *entry = entry_of(sound);
    void *voice = voice_of(channel);
    if (voice == 0)
        return;
    const int volume = sound->volume > 0x7fff ? 0x7fff : sound->volume;
    bink_xaudio_voice_set_volume(voice, (float)pow((double)((float)volume * (1.0f / 32768.0f)), 1.6666666269302368));
    const double ratio = (double)ring_rate(channel) / (double)entry->frequency;
    bink_xaudio_voice_set_pitch(voice, (float)(log(ratio) * 1.4426950408889634));
    if (!sound->mix_bins_enabled && !sound->reserved)
        return;
    const unsigned channels = (unsigned)entry->channels;
    float volumes[12];
    for (int i = 0; i < 12; i++)
        volumes[i] = 0.0f;
    if (sound->mix_bins_enabled) {
        sound->mix_bins_enabled = 0;
        if (sound->mix_bin_count == 0) {
            for (int i = 0; i < 12; i++)
                volumes[i] = 1.0f;
        } else {
            for (unsigned i = 0; i < sound->mix_bin_count; i++)
                if (sound->mix_bins[i] < 6)
                    volumes[(i & (channels - 1)) * 6 + sound->mix_bins[i]] = 1.0f;
        }
    }
    if (sound->reserved) {
        // MixBinVolumes: bins with their own volumes
        sound->reserved = 0;
        const s_channel_private *bins = channel_private(channel);
        for (unsigned i = 0; i < sound->reserved_mix && i < 12; i++)
            if (bins->bins[i] < 6)
                volumes[(i & (channels - 1)) * 6 + bins->bins[i]] = bins->volumes[i];
    }
    bink_xaudio_voice_set_output(voice, channels, volumes);
}

// @address 0x7116e0 ?Ready@@YAHPAUBINKSND@@@Z
int Ready(BINKSND *sound)
{
    SNDCHANNEL *channel = &sound->channel;
    if (channel->closing || !sound->on)
        return 0;
    if (!channel->paused)
        return 1; // not streaming yet: the ring is Bink's to fill
    unsigned position = last_position(channel);
    if (RADTimerRead() - channel->start_time > k_poll_ms)
        goto poll;
    for (;;) {
        {
            const unsigned write = channel->write_position;
            unsigned room = position < write ? channel->buffer_size - write + position : position - write;
            room = room < k_guard_bytes ? 0 : (room - k_guard_bytes) & ~3u;
            if (room < chunk_bytes(channel) && (channel->locked_bytes == 0 || room == 0))
                return 0;
            if (room < dropout_threshold(channel)) {
                channel->locked_bytes = room;
                return 1;
            }
        }
    dropped:
        {
            // the cursor came round to the writer: silence ahead of it, Bink writes on after that
            entry_of(sound)->sound_dropped_out = 1;
            const unsigned silence = chunk_bytes(channel) * 2;
            DS_clear(channel, channel->write_position, silence);
            unsigned write = channel->write_position + silence;
            if (write >= channel->buffer_size)
                write -= channel->buffer_size;
            channel->write_position = write;
        }
    poll:
        position = poll_position(sound);
        {
            const unsigned now = RADTimerRead();
            const unsigned elapsed = now - channel->start_time;
            last_position(channel) = position;
            channel->start_time = now;
            if (elapsed >= channel->reserved_time)
                goto dropped;
        }
    }
}

// @address 0x7118e8 ?start_streaming@@YAXPAUSNDCHANNEL@@@Z
void start_streaming(SNDCHANNEL *channel)
{
    BINKSND *sound = (BINKSND *)((char *)channel - offsetof(BINKSND, channel));
    channel->paused = 1;
    bink_xaudio_voice_submit_ring(voice_of(channel), channel->buffer, channel->buffer_size);
    XAudioSourceVoice_Start(channel->voice.handle, 0);
    channel->closing = 0;
    const unsigned align = block_align(sound), begun = RADTimerRead();
    unsigned started = begun;
    unsigned first = bink_xaudio_voice_position(voice_of(channel), align);
    int moved = 0;
    for (;;) {
        const unsigned position = bink_xaudio_voice_position(voice_of(channel), align);
        last_position(channel) = position;
        if (position != first) {
            moved = 1;
        } else if (!moved && RADTimerRead() - started > k_chunk_ms) {
            if (RADTimerRead() - begun > k_start_give_up_ms)
                break;
            XAudioSourceVoice_Start(channel->voice.handle, 0);
            channel->closing = 0;
            started = RADTimerRead();
            first = bink_xaudio_voice_position(voice_of(channel), align);
        }
        if (last_position(channel) > chunk_bytes(channel))
            break;
        Sleep(1);
    }
    channel->start_time = RADTimerRead();
}

// @address 0x711b18 ?MixBinVolumes@@YAXPAUBINKSND@@PAIPAHI@Z
void MixBinVolumes(BINKSND *sound, unsigned int *bins, int *volumes, unsigned int count)
{
    if (count > 12)
        count = 12;
    s_channel_private *stored = channel_private(&sound->channel);
    sound->reserved_mix = count;
    sound->reserved = 1;
    for (unsigned i = 0; i < count; i++) {
        stored->bins[i] = (unsigned char)(bins[i] < 255 ? bins[i] : 255);
        const int volume = volumes[i] > 0x7fff ? 0x7fff : volumes[i];
        stored->volumes[i] = (float)volume * (1.0f / 32768.0f);
    }
    set_rate_volume_and_pan(sound);
}

// @address 0x711e88 ?Open@@YAHPAUBINKSND@@IHHIPAUBINK@@@Z
int Open(BINKSND *sound, unsigned int frequency, int bits, int channels, unsigned int flags, BINK *bink)
{
    (void)flags;
    bink_sound_entry *entry = entry_of(sound);
    const bink_read_state *movie = (const bink_read_state *)bink;
    memset(sound, 0, sizeof(bink_sound_entry));
    entry->frequency = frequency;
    entry->bits = bits;
    entry->channels = channels;
    entry->sound_dropped_out = 0;
    if (frequency == 0 || (bits != 8 && bits != 16) || channels < 1 || channels > 2)
        return 0;
    SNDCHANNEL *channel = &sound->channel;
    void *voice = bink_xaudio_voice_create(frequency, (unsigned)channels, (unsigned)bits);
    if (voice == 0)
        return 0;
    channel->voice.handle = (unsigned int)(size_t)voice;
    ring_rate(channel) = frequency;
    entry->latency = 0;
    const unsigned bytes_per_second = (unsigned)(bits >> 3) * (unsigned)channels * frequency;
    channel->reserved_time = k_dropout_ms;
    channel->buffer_size = (bytes_per_second * k_ring_ms / 1000 + 1023) & ~1023u;
    chunk_bytes(channel) = (bytes_per_second * k_chunk_ms / 1000 + 255) & ~255u;
    dropout_threshold(channel) = channel->buffer_size * 90 / 100;
    // the movie's audio, if it is shorter than the ring
    unsigned total = chunk_bytes(channel);
    if (movie != 0 && movie->frame_rate != 0 && movie->frame_rate_divisor != 0)
        total = (unsigned)((unsigned __int64)movie->frame_count * movie->frame_rate_divisor * bytes_per_second /
                           movie->frame_rate);
    channel->buffer = (unsigned char *)radmalloc((int)channel->buffer_size, 0, 0, 0, 0, 0);
    if (channel->buffer == 0) {
        XAudioVoice_Release((int)channel->voice.handle);
        channel->voice.handle = 0;
        return 0;
    }
    entry->best_size_in_16 = chunk_bytes(channel) * (bits == 8 ? 2u : 1u);
    sound->mix_bin_count = 0;
    sound->reserved_mix = 0;
    sound->volume = 0x7fff;
    sound->pan = 0x7fff;
    channel->silence = bits == 16 ? 0 : (int)0x80808080u;
    set_rate_volume_and_pan(sound);
    DS_clear(channel, 0, channel->buffer_size);
    channel->write_position = 0;
    channel->paused = 0;
    channel->initial_bytes = channel->locked_bytes = total < channel->buffer_size ? total : channel->buffer_size;
    channel->closing = 0;
    sound->on = 1;
    entry->ready = (int (*)(bink_sound_entry *))Ready;
    entry->lock = (int (*)(bink_sound_entry *, unsigned char **, unsigned int *))Lock;
    entry->unlock = (int (*)(bink_sound_entry *, unsigned int))Unlock;
    entry->set_volume = (void (*)(bink_sound_entry *, int))Volume;
    entry->set_pan = (void (*)(bink_sound_entry *, int))Pan;
    entry->pause = (void (*)(bink_sound_entry *, int))Pause;
    entry->set_on_off = (int (*)(bink_sound_entry *, int))SetOnOff;
    entry->close = (void (*)(bink_sound_entry *))Close;
    entry->set_mix_bins = (void (*)(bink_sound_entry *, unsigned int *, int))MixBins;
    entry->set_mix_bin_volumes = (void (*)(bink_sound_entry *, unsigned int *, int *, unsigned int))MixBinVolumes;
    ++g_bink_xaudio_voice_count;
    return 1;
}
