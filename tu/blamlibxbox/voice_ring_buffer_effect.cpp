// blamlibxbox/voice_ring_buffer_effect.obj: from the records 0x28a348 0x28a538 0x28a588 0x28a5c8 0x28a5d8 0x28a670 0x2f81b0 0x2f8dc0 0x6cea98
// @flags /TP /O2
#include <xaudfx.h>
#include <stddef.h>
#include "memory/wrapped_arrays.h"
#include <string.h>

#include "sound/xenon/dsp/voice_ring_buffer_effect.h"
#include "sound/xenon/dsp/sound_xaudio_globals.h"


// @address 0x8c4280 g_sound_xaudio_global_gain

long c_voice_headset_ring_buffer_effect::Process(IXAudioFrameBuffer *input, IXAudioFrameBuffer *output)
{
    XAUDIOFRAMEBUFDATA data;
    unsigned short sample_count = 256;
    long result = 0;
    XAudioFrameBuffer_GetProcessingData(input, &data);
    if (m_ring)
    {
        if (m_value == 0)
        {
            for (long channel = 0; channel < data.Format.ChannelCount; ++channel)
                m_ring->accumulate_current_frame_output(channel, make_wrapped_array(data.pSampleBuffer + channel * sample_count, sample_count));
            XAudioFrameBuffer_Unprepare(input);
        }
        else if (m_value == 1)
        {
            for (long channel = 0; channel < data.Format.ChannelCount; ++channel)
            {
                const float *last_frame = m_ring->get_last_frame_output(channel);
                c_wrapped_array<float> samples(data.pSampleBuffer + channel * sample_count, sample_count);
                if (last_frame)
                {
                    for (long i = 0; i < sample_count; ++i)
                        samples[i] += g_sound_xaudio_global_gain * samples[i] + last_frame[i];
                }
                else
                {
                    for (long i = 0; i < sample_count; ++i)
                        samples[i] += (g_sound_xaudio_global_gain + 1.0f);
                }
            }
        }
    }
    return result;
}

long c_voice_headset_ring_buffer_effect::GetInfo(XAUDIOFXINFO *info)
{
    int value = m_value;
    int result;
    if (value == 0)
        result = 2;
    else
        result = (value != 1) + 1;
    info->DataFlow = (unsigned char)result;
    info->TrailFrames = 0;
    return 0;
}

// 0x28a588
long c_voice_headset_ring_buffer_effect::SetParam(unsigned char parameter, unsigned char type, const XAUDIOFXPARAM *value)
{
    long result = 0;
    XAudioLock(0x10001);
    switch (parameter)
    {
    default:
        result = 0x80004005;
        break;
    }
    XAudioLock(0x10003);
    return result;
}

long c_voice_headset_ring_buffer_effect::xaudio_fx_query_size(const XAUDIOFXINIT *init, unsigned long *size)
{
    *size = 20;
    return 0;
}

// @address 0x28a5d8 ?xaudio_fx_create_mixbin_effect@c_voice_headset_ring_buffer_effect@@SAJPBUXAUDIOFXINIT@@PAUIXAudioBatchAllocator@@PAPAUIXAudioEffect@@@Z
long c_voice_headset_ring_buffer_effect::xaudio_fx_create_mixbin_effect(const XAUDIOFXINIT *init, IXAudioBatchAllocator *allocator, IXAudioEffect **effect)
{
    long result = 0;
    c_voice_headset_ring_buffer_effect *created = 0;
    created = new (allocator) c_voice_headset_ring_buffer_effect(_voice_headset_mixbin, init->pContext);
    created->Initialize(init, allocator);
    *effect = created;
    return result;
}
// @address 0x28a670 ?xaudio_fx_create_output_effect@c_voice_headset_ring_buffer_effect@@SAJPBUXAUDIOFXINIT@@PAUIXAudioBatchAllocator@@PAPAUIXAudioEffect@@@Z
long c_voice_headset_ring_buffer_effect::xaudio_fx_create_output_effect(const XAUDIOFXINIT *init, IXAudioBatchAllocator *allocator, IXAudioEffect **effect)
{
    long result = 0;
    c_voice_headset_ring_buffer_effect *created = 0;
    created = new (allocator) c_voice_headset_ring_buffer_effect(_voice_headset_output, init->pContext);
    created->Initialize(init, allocator);
    *effect = created;
    return result;
}

// Returns the context pointer stored at 0x4.
long c_voice_headset_ring_buffer_effect::GetContext(void **context)
{
    *context = m_context;
    return 0;
}

long c_voice_headset_ring_buffer_effect::GetParam(unsigned char a, unsigned char b, XAUDIOFXPARAM *p)
{
    XAUDIOFXPARAM local;
    local.Value = 0.0f;
    memset(&local.Data.BufferSize, 0, sizeof(local.Data.BufferSize));
    XAudioLock(0x10001);
    *p = local;
    XAudioLock(0x10003);
    return 0x80004005;
}

// Increments the count at 0x8 and returns it.
unsigned long c_voice_headset_ring_buffer_effect::AddRef(void)
{
    return ++m_ref_count;
}

// Decrements the count at 0x8 and returns it; ICF folds this body with the remote voice effect's Release (0x28a708).
unsigned long c_voice_headset_ring_buffer_effect::Release(void)
{
    return --m_ref_count;
}
