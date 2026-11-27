// Halo 3 PC port: clean-room Xbox 360 SDK headers (zero-XDK build).
// xaudfx.h: the XAudio effect interface (IXAudioEffect), the frame buffer an effect processes (IXAudioFrameBuffer)
// and the engine's default effect table.
#ifndef __XAUDFX_INCLUDED__
#define __XAUDFX_INCLUDED__
#include <xauddefs.h>

#pragma pack(push, 4)

// An effect in a voice's chain; Process reads the input frame buffer and writes the output one (the same buffer
// for in-place effects).
#undef INTERFACE
#define INTERFACE IXAudioEffect
DECLARE_INTERFACE_(IXAudioEffect, IXAudioRefCount)
{
    STDMETHOD_(ULONG, AddRef)(THIS) PURE;
    STDMETHOD_(ULONG, Release)(THIS) PURE;
    STDMETHOD(GetInfo)(THIS_ LPXAUDIOFXINFO pInfo) PURE;
    STDMETHOD(GetParam)(THIS_ XAUDIOFXPARAMID ParamId, XAUDIOFXPARAMTYPE ParamType, LPXAUDIOFXPARAM pParam) PURE;
    STDMETHOD(SetParam)(THIS_ XAUDIOFXPARAMID ParamId, XAUDIOFXPARAMTYPE ParamType, LPCXAUDIOFXPARAM pParam) PURE;
    STDMETHOD(GetContext)(THIS_ LPVOID *ppContext) PURE;
    STDMETHOD(Process)(THIS_ IXAudioFrameBuffer *pInputBuffer, IXAudioFrameBuffer *pOutputBuffer) PURE;
};

STDAPI XAudioFrameBuffer_GetProcessingData(IXAudioFrameBuffer *pFrameBuffer, LPXAUDIOFRAMEBUFDATA pProcessData);
STDAPI XAudioFrameBuffer_Unprepare(IXAudioFrameBuffer *pFrameBuffer);

#ifdef __cplusplus
// The frame buffer is an engine object; its methods are the C entry points.
struct IXAudioFrameBuffer
{
    __forceinline HRESULT GetProcessingData(LPXAUDIOFRAMEBUFDATA pProcessData)
    {
        return XAudioFrameBuffer_GetProcessingData(this, pProcessData);
    }

    __forceinline HRESULT Unprepare(void)
    {
        return XAudioFrameBuffer_Unprepare(this);
    }
};
#endif

EXTERN_C const XAUDIOFXTABLE XAudioDefaultEffectTable;

#pragma pack(pop)

#endif // __XAUDFX_INCLUDED__
