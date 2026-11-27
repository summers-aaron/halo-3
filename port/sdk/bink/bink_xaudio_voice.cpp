// port/sdk/bink (both hosts): the XAudio calls of the Bink sound driver (bink_xaudio.cpp, binkxa.obj), made with the
// SDK's own types (xaudio.h), which the driver's unit cannot include: include/bink/binkxa.h declares its own
// XAUDIOPACKETPOSITION and handle-typed XAudio prototypes for the decompiled half of binkxa.obj
// (tu/binkxenon/binkxa.cpp). A voice is an IXAudioSourceVoice pointer, the driver's SNDCHANNEL::voice handle.
//
// Units: the port's engine (port/sdk/audio/xaudio_engine.cpp) counts a PCM packet's positions in samples (frames of
// all channels); the driver's ring arithmetic is in bytes, as the retail driver reads them. The conversion is here.
#include <xtl.h>
#include <xaudio.h>

#include "bink_xaudio_voice.h"

extern "C" {

void *bink_xaudio_voice_create(unsigned rate, unsigned channels, unsigned bits)
{
    // retail Open (0x711e88): 16- or 8-bit PCM at the track's rate, room for a 12-entry channel map (two inputs to
    // the six mix bins), one packet (the looping ring)
    XAUDIOSOURCEVOICEINIT init;
    memset(&init, 0, sizeof(init));
    init.Format.SampleType = bits == 16 ? XAUDIOSAMPLETYPE_16BITPCM : XAUDIOSAMPLETYPE_8BITPCM;
    init.Format.ChannelCount = (XAUDIOCHANNEL)channels;
    init.Format.SampleRate = rate;
    init.MaxChannelMapEntryCount = 12;
    init.MaxPacketCount = 1;
    IXAudioSourceVoice *voice = 0;
    if (FAILED(XAudioCreateSourceVoice(&init, &voice)))
        return 0;
    return voice;
}

void bink_xaudio_voice_submit_ring(void *handle, void *buffer, unsigned bytes)
{
    // retail start_streaming (0x7118e8): the whole ring as one packet looping for ever, from its start
    IXAudioSourceVoice *voice = (IXAudioSourceVoice *)handle;
    XAudioSourceVoice_FlushSourceBuffers(voice);
    XAUDIOPACKET packet;
    memset(&packet, 0, sizeof(packet));
    packet.pBuffer = buffer;
    packet.BufferSize = bytes;
    packet.LoopCount = XAUDIOLOOPCOUNT_INFINITE;
    packet.LoopStart = 0;
    packet.LoopLength = 0; // to the end of the packet
    packet.pContext = (void *)-1;
    XAudioSourceVoice_SubmitPacket(voice, &packet, XAUDIOSUBMITPACKET_DISCONTINUITY);
    XAudioSourceVoice_SetPacketLoopCount(voice, XAUDIOLOOPCOUNT_INFINITE);
    XAUDIOPACKETPOSITION position;
    memset(&position, 0, sizeof(position));
    XAudioSourceVoice_SetPacketPosition(voice, &position);
}

unsigned bink_xaudio_voice_position(void *handle, unsigned block_align)
{
    XAUDIOPACKETPOSITION position;
    memset(&position, 0, sizeof(position));
    if (FAILED(XAudioSourceVoice_GetPacketPosition((IXAudioSourceVoice *)handle, &position)))
        return 0;
    return position.Position * block_align;
}

void bink_xaudio_voice_set_volume(void *handle, float volume)
{
    XAudioSourceVoice_SetVolume((IXAudioSourceVoice *)handle, volume);
}

void bink_xaudio_voice_set_pitch(void *handle, float octaves)
{
    XAudioSourceVoice_SetPitch((IXAudioSourceVoice *)handle, octaves);
}

void bink_xaudio_voice_set_output(void *handle, unsigned channels, const float *volumes)
{
    // retail set_rate_volume_and_pan (0x7113b8): one output, the mastering voice (a null destination), with a map
    // from every input channel to each of the six mix bins (FL FR FC LFE BL BR)
    XAUDIOCHANNELMAPENTRY entries[12];
    const unsigned count = channels * 6u <= 12u ? channels * 6u : 12u;
    for (unsigned i = 0; i < count; i++) {
        entries[i].InputChannel = (XAUDIOCHANNEL)(i / 6u);
        entries[i].OutputChannel = (XAUDIOCHANNEL)(i % 6u);
        entries[i].Volume = volumes[i];
    }
    XAUDIOCHANNELMAP map;
    map.EntryCount = (XAUDIOCHANNEL)count;
    map.paEntries = entries;
    XAUDIOVOICEOUTPUTENTRY entry;
    entry.pDestVoice = 0;
    entry.pChannelMap = &map;
    XAUDIOVOICEOUTPUT output;
    output.EntryCount = 1;
    output.paEntries = &entry;
    XAudioVoice_SetVoiceOutput((IXAudioVoice *)handle, &output);
}

} // extern "C"
