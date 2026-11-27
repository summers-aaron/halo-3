#ifndef SOUND_XENON_DSP_REMOTE_VOICE_RING_BUFFER_EFFECT_H
#define SOUND_XENON_DSP_REMOTE_VOICE_RING_BUFFER_EFFECT_H

#include <stddef.h>
#include <xaudfx.h>
#include "sound/xenon/dsp/xaudio_output_ring_buffer.h"

enum e_remote_voice_ring_buffer_mode { _remote_voice_mixbin, _remote_voice_output };
class c_remote_voice_ring_buffer_effect : public IXAudioEffect
{
public:
    virtual unsigned long AddRef();
    virtual unsigned long Release();
    virtual long GetInfo(XAUDIOFXINFO *info);
    virtual long GetParam(unsigned char, unsigned char, XAUDIOFXPARAM *);
    virtual long SetParam(unsigned char id, unsigned char type, const XAUDIOFXPARAM *param);
    virtual long GetContext(void **);
    virtual long Process(IXAudioFrameBuffer *input, IXAudioFrameBuffer *output);
    virtual long Initialize(const XAUDIOFXINIT *, IXAudioBatchAllocator *);
    static long xaudio_fx_query_size(const XAUDIOFXINIT *init, unsigned long *size);
    static long xaudio_fx_create_mixbin_effect(const XAUDIOFXINIT *, IXAudioBatchAllocator *, IXAudioEffect **);
    static long xaudio_fx_create_output_effect(const XAUDIOFXINIT *, IXAudioBatchAllocator *, IXAudioEffect **);
    static __forceinline void *operator new(size_t size, IXAudioBatchAllocator *allocator)
    {
        return allocator->Alloc(size);
    }
    __forceinline c_remote_voice_ring_buffer_effect(e_remote_voice_ring_buffer_mode mode, void *context)
    {
        m_value = mode;
        m_context = context;
        m_reference_count = 1;
        m_ring = (c_xaudio_output_ring_buffer *)context;
        m_index = -1;
    }
    void *m_context;
    unsigned long m_reference_count;
    long m_index;
    c_xaudio_output_ring_buffer *m_ring;
    int m_value;
};

#endif // SOUND_XENON_DSP_REMOTE_VOICE_RING_BUFFER_EFFECT_H
