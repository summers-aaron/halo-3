#ifndef SOUND_XENON_DSP_VOICE_RING_BUFFER_EFFECT_H
#define SOUND_XENON_DSP_VOICE_RING_BUFFER_EFFECT_H

#include <stddef.h>
#include <xaudfx.h>
#include "sound/xenon/dsp/xaudio_output_ring_buffer.h"

enum e_voice_headset_ring_buffer_mode { _voice_headset_mixbin, _voice_headset_output };
class c_voice_headset_ring_buffer_effect : public IXAudioEffect
{
public:
    virtual unsigned long AddRef(void);
    virtual unsigned long Release(void);
    virtual long GetInfo(XAUDIOFXINFO *info);
    virtual long GetParam(unsigned char a, unsigned char b, XAUDIOFXPARAM *p);
    virtual long SetParam(unsigned char a, unsigned char b, const XAUDIOFXPARAM *p);
    virtual long GetContext(void **context);
    virtual long Process(IXAudioFrameBuffer *input, IXAudioFrameBuffer *output);
    virtual long Initialize(const XAUDIOFXINIT *init, IXAudioBatchAllocator *allocator);
    static long xaudio_fx_query_size(const XAUDIOFXINIT *init, unsigned long *size);
    static long xaudio_fx_create_mixbin_effect(const XAUDIOFXINIT *, IXAudioBatchAllocator *, IXAudioEffect **);
    static long xaudio_fx_create_output_effect(const XAUDIOFXINIT *, IXAudioBatchAllocator *, IXAudioEffect **);
    static __forceinline void *operator new(size_t size, IXAudioBatchAllocator *allocator) { return allocator->Alloc(size); }
    __forceinline c_voice_headset_ring_buffer_effect(e_voice_headset_ring_buffer_mode mode, void *context)
    {
        m_value = mode;
        m_context = context;
        m_ref_count = 1;
        m_ring = (c_xaudio_output_ring_buffer *)context;
    }
    void *m_context;
    unsigned long m_ref_count;
    c_xaudio_output_ring_buffer *m_ring;
    int m_value;
};

#if defined(_M_PPCBE) || defined(_XBOX)
typedef char voice_headset_ring_buffer_effect_size[(sizeof(void *) != 4 || (sizeof(c_voice_headset_ring_buffer_effect) == 20)) ? 1 : -1];
#endif

#endif
