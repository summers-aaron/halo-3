// binkxenon/binkxa.obj: from the records 0x711678 0x7118b0 0x711a18 0x711a90 0x711aa8 0x711ac0 0x711cb8 0x711d78 0x711e18 0x712148
// @flags /TP /O2 /Oc
#include "bink/radmem.h"
#include "bink/rad_timer.h"
#include <string.h>

#include "bink/binkxa.h"

// @address 0x711678 ?DS_clear@@YAXPAUSNDCHANNEL@@II@Z
void DS_clear(SNDCHANNEL *channel, unsigned int start, unsigned int length)
{
    unsigned int first_length = length;
    if (start + length > channel->buffer_size)
        first_length = channel->buffer_size - start;
    memset(channel->buffer + start, channel->silence, first_length);
    if (first_length < length)
        memset(channel->buffer, channel->silence, length - first_length);
}

// @address 0x7118b0 ?Lock@@YAHPAUBINKSND@@PAPAEPAI@Z
int Lock(BINKSND *sound, unsigned char **buffer, unsigned int *length)
{
    unsigned int available = sound->channel.locked_bytes;
    if (sound->channel.write_position + available > sound->channel.buffer_size)
        available = sound->channel.buffer_size - sound->channel.write_position;
    *buffer = sound->channel.buffer + sound->channel.write_position;
    *length = available;
    return 1;
}

// @address 0x711a18 ?Unlock@@YAHPAUBINKSND@@I@Z
int Unlock(BINKSND *sound, unsigned int byte_count)
{
    int write_position = byte_count + sound->channel.write_position;
    unsigned int buffer_size = sound->channel.buffer_size;
    sound->channel.write_position = write_position;
    if ((unsigned int)write_position >= buffer_size)
        sound->channel.write_position = write_position - buffer_size;
    int state_flags = sound->channel.paused;
    unsigned int *locked_bytes_address = &sound->channel.locked_bytes;
    // match: 0x711a18: volatile load keeps locked_bytes in r11 for the subtract and zero-compare (plain load colors it r10)
    unsigned int locked_bytes = *(volatile unsigned int *)(locked_bytes_address);
    unsigned int remaining_bytes = locked_bytes - byte_count;
    sound->channel.locked_bytes = remaining_bytes;
    if (state_flags == 0 && remaining_bytes == 0 && sound->channel.closing == 0)
        start_streaming(&sound->channel);
    return 1;
}

// @address 0x7113b8 ?set_rate_volume_and_pan@@YAXPAUBINKSND@@@Z
// @address 0x711e88 ?Open@@YAHPAUBINKSND@@IHHIPAUBINK@@@Z

// @address 0x711a90 ?Volume@@YAXPAUBINKSND@@H@Z
void Volume(BINKSND *sound, int volume)
{
    if (volume > 0x7fff)
        volume = 0x7fff;
    sound->volume = volume;
    set_rate_volume_and_pan(sound);
}

// @address 0x711aa8 ?Pan@@YAXPAUBINKSND@@H@Z
void Pan(BINKSND *sound, int pan)
{
    if (pan > 0x10000)
        pan = 0x10000;
    sound->pan = pan;
    set_rate_volume_and_pan(sound);
}

// @address 0x711ac0 ?MixBins@@YAXPAUBINKSND@@PAII@Z
void MixBins(BINKSND *sound, unsigned int *bins, unsigned int count)
{
    if (count > 12)
        count = 12;
    sound->mix_bin_count = count;
    sound->mix_bins_enabled = 1;
    memcpy(sound->mix_bins, bins, count * sizeof(unsigned int));
    set_rate_volume_and_pan(sound);
}

// @address 0x711cb8 ?SetOnOff@@YAHPAUBINKSND@@H@Z
int SetOnOff(BINKSND *sound, int on)
{
    if (on) {
        if (!sound->on) {
            unsigned int initial_bytes = sound->channel.initial_bytes;
            sound->on = 1;
            sound->channel.paused = 0;
            sound->channel.write_position = 0;
            sound->channel.locked_bytes = initial_bytes;
        }
    } else {
        if (sound->on) {
            SNDCHANNEL *channel = &sound->channel;
            XAUDIOPACKETPOSITION position;
            position.Position = 0;
            if (XAudioSourceVoice_Stop(channel->voice, 1) >= 0) {
                unsigned char state;
                do {
                    state = 0;
                    XAudioVoice_GetVoiceState(channel->voice.handle, &state);
                } while (state == 1);
            }
            XAudioSourceVoice_SetPacketPosition(channel->voice.handle, &position);
            DS_clear(channel, 0, sound->channel.buffer_size);
            sound->on = 0;
        }
    }
    return sound->on;
}

static __forceinline void binkxa_start(SNDCHANNEL *channel)
{
    XAudioSourceVoice_Start(channel->voice.handle, 0);
    channel->closing = 0;
}

static __forceinline void binkxa_stop(SNDCHANNEL *channel)
{
    XAudioSourceVoice_Stop(channel->voice, 1);
    channel->closing = 1;
}

// @address 0x711d78 ?Pause@@YAHPAUBINKSND@@H@Z
int Pause(BINKSND *sound, int pause)
{
    if (pause) {
        pause = 1;
        if (sound->channel.paused) {
            binkxa_stop(&sound->channel);
        }
    } else {
        pause = 0;
        if (!sound->channel.paused) {
            if (!sound->channel.locked_bytes)
                start_streaming(&sound->channel);
        } else {
            volatile unsigned int *start_time = &sound->channel.start_time;
            *start_time = RADTimerRead();
            binkxa_start(&sound->channel);
        }
    }
    sound->channel.closing = pause;
    SNDCHANNEL *channel = &sound->channel;
    return channel->closing;
}

// @address 0x11df2a8 g_bink_xaudio_voice_count

// @address 0x711e18 ?Close@@YAXPAUBINKSND@@@Z
void Close(BINKSND *sound)
{
    XAUDIOSOURCEVOICE *voice = &sound->channel.voice;
    if (voice->handle)
    {
        XAudioSourceVoice_Stop(*voice, 1);
        sound->channel.closing = 1;
        XAudioVoice_Release(voice->handle);
        void *buffer = sound->channel.buffer;
        voice->handle = 0;
        radfree(buffer);
        --g_bink_xaudio_voice_count;
    }
}

// @address 0x712148 BinkOpenXAudio
extern "C" BINKSNDOPEN BinkOpenXAudio(void)
{
    return Open;
}
