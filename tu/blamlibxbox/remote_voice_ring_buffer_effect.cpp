// blamlibxbox/remote_voice_ring_buffer_effect.obj: from the records 0x2f8c38 0x2f8d70 0x2f8e28 0x2f8e88 0x2f8e98 0x2f8f38
// @flags /TP /O2 /Oc
#include "memory/wrapped_arrays.h"
#include <stddef.h>
#include <string.h>
#include <xaudfx.h>
// @address 0x2f8c38 ?Process@c_remote_voice_ring_buffer_effect@@UAAJPAUIXAudioFrameBuffer@@0@Z
// @address 0x2f8d70 ?GetInfo@c_remote_voice_ring_buffer_effect@@UAAJPAUXAUDIOFXINFO@@@Z
// @address 0x2f8e28 ?SetParam@c_remote_voice_ring_buffer_effect@@UAAJEEPBTXAUDIOFXPARAM@@@Z
// @address 0x2f8e88 ?xaudio_fx_query_size@c_remote_voice_ring_buffer_effect@@SAJPBUXAUDIOFXINIT@@PAK@Z
// @address 0x2f8e98 ?xaudio_fx_create_mixbin_effect@c_remote_voice_ring_buffer_effect@@SAJPBUXAUDIOFXINIT@@PAUIXAudioBatchAllocator@@PAPAUIXAudioEffect@@@Z
// @address 0x2f8f38 ?xaudio_fx_create_output_effect@c_remote_voice_ring_buffer_effect@@SAJPBUXAUDIOFXINIT@@PAUIXAudioBatchAllocator@@PAPAUIXAudioEffect@@@Z

#include "sound/xenon/dsp/remote_voice_ring_buffer_effect.h"

long c_remote_voice_ring_buffer_effect::Process(IXAudioFrameBuffer *input, IXAudioFrameBuffer *output)
{
    XAUDIOFRAMEBUFDATA data;
    unsigned short sample_count = 256;
    long result = 0;
    XAudioFrameBuffer_GetProcessingData(input, &data);
    XAudioLock(XAUDIOLOCKTYPE_SPINLOCK | XAUDIOLOCKTYPE_LOCK);
    long index = m_index;
    XAudioLock(XAUDIOLOCKTYPE_SPINLOCK | XAUDIOLOCKTYPE_UNLOCK);
    if (m_ring && index != -1) {
        if (m_value == 0) {
            c_wrapped_array<float> raw_samples = make_wrapped_array(data.pSampleBuffer, sample_count);
            m_ring->accumulate_current_frame_output(index, raw_samples);
        } else if (m_value == 1) {
            const float *last_frame = m_ring->get_last_frame_output(index);
            if (last_frame) {
                c_wrapped_array<float> samples(data.pSampleBuffer, sample_count);
                for (long sample_index = 0; sample_index < sample_count; ++sample_index)
                    samples[sample_index] += last_frame[sample_index];
            }
        }
    }
    return result;
}

long c_remote_voice_ring_buffer_effect::GetInfo(XAUDIOFXINFO *info)
{
    int value = m_value;
    int effect_type;
    if (value == 0)
        effect_type = 2;
    else
        effect_type = (value != 1) + 1;
    info->DataFlow = (unsigned char)effect_type;
    info->TrailFrames = 0;
    return 0;
}

long c_remote_voice_ring_buffer_effect::SetParam(unsigned char id, unsigned char type, const XAUDIOFXPARAM *param)
{
    long result = 0;
    XAudioLock(XAUDIOLOCKTYPE_SPINLOCK | XAUDIOLOCKTYPE_LOCK);
    switch (id)
    {
    case 0:
        m_index = param->Flags;
        break;
    default:
        result = 0x80004005;
        break;
    }
    XAudioLock(XAUDIOLOCKTYPE_SPINLOCK | XAUDIOLOCKTYPE_UNLOCK);
    return result;
}

long c_remote_voice_ring_buffer_effect::xaudio_fx_query_size(const XAUDIOFXINIT *init, unsigned long *size)
{
    *size = 24;
    return 0;
}

long c_remote_voice_ring_buffer_effect::xaudio_fx_create_mixbin_effect(const XAUDIOFXINIT *init, IXAudioBatchAllocator *allocator, IXAudioEffect **effect)
{
    long result = 0;
    c_remote_voice_ring_buffer_effect *created = 0;
    created = new (allocator) c_remote_voice_ring_buffer_effect(_remote_voice_mixbin, init->pContext);
    created->Initialize(init, allocator);
    *effect = created;
    return result;
}

long c_remote_voice_ring_buffer_effect::xaudio_fx_create_output_effect(const XAUDIOFXINIT *init, IXAudioBatchAllocator *allocator, IXAudioEffect **effect)
{
    long result = 0;
    c_remote_voice_ring_buffer_effect *created = 0;
    created = new (allocator) c_remote_voice_ring_buffer_effect(_remote_voice_output, init->pContext);
    created->Initialize(init, allocator);
    *effect = created;
    return result;
}

unsigned long c_remote_voice_ring_buffer_effect::AddRef()
{
    return ++m_reference_count;
}

long c_remote_voice_ring_buffer_effect::GetContext(void **context)
{
    *context = m_context;
    return 0;
}

long c_remote_voice_ring_buffer_effect::GetParam(unsigned char id, unsigned char type, XAUDIOFXPARAM *param)
{
    XAUDIOFXPARAM value;
    value.Value = 0.0f;
    memset(&value.Data.BufferSize, 0, sizeof(value.Data.BufferSize));
    XAudioLock(XAUDIOLOCKTYPE_SPINLOCK | XAUDIOLOCKTYPE_LOCK);
    *param = value;
    XAudioLock(XAUDIOLOCKTYPE_SPINLOCK | XAUDIOLOCKTYPE_UNLOCK);
    return 0x80004005;
}

long c_remote_voice_ring_buffer_effect::Initialize(const XAUDIOFXINIT *, IXAudioBatchAllocator *)
{
    return 0;
}
