// Halo 3 PC port: clean-room Xbox 360 SDK headers (zero-XDK build).
// xaudio.h: the XAudio engine API (initialization, voice creation, engine callbacks, effect registration) and the
// voice interfaces, whose methods are inline forwarders to the C entry points.
#ifndef __XAUDIO_INCLUDED__
#define __XAUDIO_INCLUDED__
#include <xaudfx.h>

#pragma pack(push, 4)

// ---- engine ----

STDAPI XAudioInitialize(LPCXAUDIOENGINEINIT pInit);
STDAPI_(ULONG) XAudioShutDown(void);
STDAPI XAudioCreateSourceVoice(LPCXAUDIOSOURCEVOICEINIT pInit, IXAudioSourceVoice **ppVoice);
STDAPI XAudioCreateSubmixVoice(LPCXAUDIOSUBMIXVOICEINIT pInit, IXAudioSubmixVoice **ppVoice);
STDAPI XAudioGetMasteringVoice(IXAudioMasteringVoice **ppVoice);
STDAPI XAudioRegisterEngineCallback(XAUDIOENGINECALLBACKTYPE CallbackType, LPFNXAUDIOENGINECALLBACK pfnCallback,
    LPVOID pContext, LPXAUDIOCALLBACKHANDLE phCallbackHandle);
STDAPI XAudioUnregisterEngineCallback(XAUDIOCALLBACKHANDLE hCallbackHandle);
STDAPI XAudioRegisterEffects(LPCXAUDIOFXREGISTER pEffects);
STDAPI XAudioUnregisterEffects(LPCXAUDIOFXUNREGISTER pEffects);
STDAPI XAudioSynchStart(void);
STDAPI XAudioQueryPIXCounters(LPXAUDIOPIXCOUNTERS pCounters);

// ---- every voice ----

STDAPI_(ULONG) XAudioVoice_AddRef(IXAudioVoice *pVoice);
STDAPI_(ULONG) XAudioVoice_Release(IXAudioVoice *pVoice);
STDAPI XAudioVoice_GetEffectState(IXAudioVoice *pVoice, XAUDIOVOICEFXINDEX EffectIndex, LPXAUDIOVOICEFXSTATE pEffectState);
STDAPI XAudioVoice_SetEffectState(IXAudioVoice *pVoice, XAUDIOVOICEFXINDEX EffectIndex, XAUDIOVOICEFXSTATE EffectState);
STDAPI XAudioVoice_GetEffectParam(IXAudioVoice *pVoice, XAUDIOVOICEFXINDEX EffectIndex, XAUDIOFXPARAMID ParamId,
    XAUDIOFXPARAMTYPE ParamType, LPXAUDIOFXPARAM pParam);
STDAPI XAudioVoice_SetEffectParam(IXAudioVoice *pVoice, XAUDIOVOICEFXINDEX EffectIndex, XAUDIOFXPARAMID ParamId,
    XAUDIOFXPARAMTYPE ParamType, LPCXAUDIOFXPARAM pParam);
STDAPI XAudioVoice_SetVoiceOutput(IXAudioVoice *pVoice, LPCXAUDIOVOICEOUTPUT pVoiceOutput);
STDAPI XAudioVoice_SetVoiceOutputVolume(IXAudioVoice *pVoice, LPCXAUDIOVOICEOUTPUTVOLUME pVolume);
STDAPI XAudioVoice_GetVoiceContext(IXAudioVoice *pVoice, LPVOID *ppContext);
STDAPI XAudioVoice_GetVoiceState(IXAudioVoice *pVoice, LPXAUDIOSOURCESTATE pState);
STDAPI XAudioVoice_GetVoiceType(IXAudioVoice *pVoice, LPXAUDIOVOICETYPE pType);

#ifdef __cplusplus
struct __declspec(novtable) IXAudioVoice
{
    __forceinline ULONG AddRef(void)
    {
        return XAudioVoice_AddRef(this);
    }

    __forceinline ULONG Release(void)
    {
        return XAudioVoice_Release(this);
    }

    __forceinline HRESULT GetEffectState(XAUDIOVOICEFXINDEX EffectIndex, LPXAUDIOVOICEFXSTATE pEffectState)
    {
        return XAudioVoice_GetEffectState(this, EffectIndex, pEffectState);
    }

    __forceinline HRESULT SetEffectState(XAUDIOVOICEFXINDEX EffectIndex, XAUDIOVOICEFXSTATE EffectState)
    {
        return XAudioVoice_SetEffectState(this, EffectIndex, EffectState);
    }

    __forceinline HRESULT GetEffectParam(XAUDIOVOICEFXINDEX EffectIndex, XAUDIOFXPARAMID ParamId, XAUDIOFXPARAMTYPE ParamType,
        LPXAUDIOFXPARAM pParam)
    {
        return XAudioVoice_GetEffectParam(this, EffectIndex, ParamId, ParamType, pParam);
    }

    __forceinline HRESULT SetEffectParam(XAUDIOVOICEFXINDEX EffectIndex, XAUDIOFXPARAMID ParamId, XAUDIOFXPARAMTYPE ParamType,
        LPCXAUDIOFXPARAM pParam)
    {
        return XAudioVoice_SetEffectParam(this, EffectIndex, ParamId, ParamType, pParam);
    }

    __forceinline HRESULT SetVoiceOutput(LPCXAUDIOVOICEOUTPUT pVoiceOutput)
    {
        return XAudioVoice_SetVoiceOutput(this, pVoiceOutput);
    }

    __forceinline HRESULT SetVoiceOutputVolume(LPCXAUDIOVOICEOUTPUTVOLUME pVolume)
    {
        return XAudioVoice_SetVoiceOutputVolume(this, pVolume);
    }

    __forceinline HRESULT GetVoiceContext(LPVOID *ppContext)
    {
        return XAudioVoice_GetVoiceContext(this, ppContext);
    }

    __forceinline HRESULT GetVoiceState(LPXAUDIOSOURCESTATE pState)
    {
        return XAudioVoice_GetVoiceState(this, pState);
    }

    __forceinline HRESULT GetVoiceType(LPXAUDIOVOICETYPE pType)
    {
        return XAudioVoice_GetVoiceType(this, pType);
    }
};
#endif

// ---- source voices ----

STDAPI XAudioSourceVoice_GetVoiceFormat(IXAudioSourceVoice *pVoice, LPXAUDIOSOURCEFORMAT pFormat);
STDAPI XAudioSourceVoice_SubmitPacket(IXAudioSourceVoice *pVoice, LPCXAUDIOPACKET pPacket, XAUDIOSUBMITPACKET Flags);
STDAPI XAudioSourceVoice_FlushPackets(IXAudioSourceVoice *pVoice);
STDAPI XAudioSourceVoice_SubmitSourceBuffer(IXAudioSourceVoice *pVoice, LPCXAUDIOSOURCEBUFFER pSourceBuffer, XAUDIOSUBMITPACKET Flags);
STDAPI XAudioSourceVoice_FlushSourceBuffers(IXAudioSourceVoice *pVoice);
STDAPI XAudioSourceVoice_Start(IXAudioSourceVoice *pVoice, XAUDIOSTART Flags);
STDAPI XAudioSourceVoice_Stop(IXAudioSourceVoice *pVoice, XAUDIOSTOP Flags);
STDAPI XAudioSourceVoice_Discontinuity(IXAudioSourceVoice *pVoice);
STDAPI XAudioSourceVoice_GetPacketPosition(IXAudioSourceVoice *pVoice, LPXAUDIOPACKETPOSITION pPosition);
STDAPI XAudioSourceVoice_SetPacketPosition(IXAudioSourceVoice *pVoice, LPCXAUDIOPACKETPOSITION pPosition);
STDAPI XAudioSourceVoice_GetStreamPosition(IXAudioSourceVoice *pVoice, LPDWORD pPosition);
STDAPI XAudioSourceVoice_GetPacketLoopCount(IXAudioSourceVoice *pVoice, LPDWORD pLoopCount);
STDAPI XAudioSourceVoice_SetPacketLoopCount(IXAudioSourceVoice *pVoice, DWORD LoopCount);
STDAPI XAudioSourceVoice_GetPacketContext(IXAudioSourceVoice *pVoice, LPVOID *ppContext);
STDAPI XAudioSourceVoice_GetVolume(IXAudioSourceVoice *pVoice, LPXAUDIOVOLUME pVolume);
STDAPI XAudioSourceVoice_SetVolume(IXAudioSourceVoice *pVoice, XAUDIOVOLUME Volume);
STDAPI XAudioSourceVoice_GetPitch(IXAudioSourceVoice *pVoice, LPXAUDIOPITCH pPitch);
STDAPI XAudioSourceVoice_SetPitch(IXAudioSourceVoice *pVoice, XAUDIOPITCH Pitch);
STDAPI XAudioSourceVoice_GetFrequencyScale(IXAudioSourceVoice *pVoice, LPXAUDIOFREQSCALE pScale);
STDAPI XAudioSourceVoice_SetFrequencyScale(IXAudioSourceVoice *pVoice, XAUDIOFREQSCALE Scale);

#ifdef __cplusplus
struct __declspec(novtable) IXAudioSourceVoice : public IXAudioVoice
{
    __forceinline HRESULT GetVoiceFormat(LPXAUDIOSOURCEFORMAT pFormat)
    {
        return XAudioSourceVoice_GetVoiceFormat(this, pFormat);
    }

    __forceinline HRESULT SubmitPacket(LPCXAUDIOPACKET pPacket, XAUDIOSUBMITPACKET Flags)
    {
        return XAudioSourceVoice_SubmitPacket(this, pPacket, Flags);
    }

    __forceinline HRESULT FlushPackets(void)
    {
        return XAudioSourceVoice_FlushPackets(this);
    }

    __forceinline HRESULT SubmitSourceBuffer(LPCXAUDIOSOURCEBUFFER pSourceBuffer, XAUDIOSUBMITPACKET Flags)
    {
        return XAudioSourceVoice_SubmitSourceBuffer(this, pSourceBuffer, Flags);
    }

    __forceinline HRESULT FlushSourceBuffers(void)
    {
        return XAudioSourceVoice_FlushSourceBuffers(this);
    }

    __forceinline HRESULT Start(XAUDIOSTART Flags)
    {
        return XAudioSourceVoice_Start(this, Flags);
    }

    __forceinline HRESULT Stop(XAUDIOSTOP Flags)
    {
        return XAudioSourceVoice_Stop(this, Flags);
    }

    __forceinline HRESULT Discontinuity(void)
    {
        return XAudioSourceVoice_Discontinuity(this);
    }

    __forceinline HRESULT GetPacketPosition(LPXAUDIOPACKETPOSITION pPosition)
    {
        return XAudioSourceVoice_GetPacketPosition(this, pPosition);
    }

    __forceinline HRESULT SetPacketPosition(LPCXAUDIOPACKETPOSITION pPosition)
    {
        return XAudioSourceVoice_SetPacketPosition(this, pPosition);
    }

    __forceinline HRESULT GetStreamPosition(LPDWORD pPosition)
    {
        return XAudioSourceVoice_GetStreamPosition(this, pPosition);
    }

    __forceinline HRESULT GetPacketLoopCount(LPDWORD pLoopCount)
    {
        return XAudioSourceVoice_GetPacketLoopCount(this, pLoopCount);
    }

    __forceinline HRESULT SetPacketLoopCount(DWORD LoopCount)
    {
        return XAudioSourceVoice_SetPacketLoopCount(this, LoopCount);
    }

    __forceinline HRESULT GetPacketContext(LPVOID *ppContext)
    {
        return XAudioSourceVoice_GetPacketContext(this, ppContext);
    }

    __forceinline HRESULT GetVolume(LPXAUDIOVOLUME pVolume)
    {
        return XAudioSourceVoice_GetVolume(this, pVolume);
    }

    __forceinline HRESULT SetVolume(XAUDIOVOLUME Volume)
    {
        return XAudioSourceVoice_SetVolume(this, Volume);
    }

    __forceinline HRESULT GetPitch(LPXAUDIOPITCH pPitch)
    {
        return XAudioSourceVoice_GetPitch(this, pPitch);
    }

    __forceinline HRESULT SetPitch(XAUDIOPITCH Pitch)
    {
        return XAudioSourceVoice_SetPitch(this, Pitch);
    }

    __forceinline HRESULT GetFrequencyScale(LPXAUDIOFREQSCALE pScale)
    {
        return XAudioSourceVoice_GetFrequencyScale(this, pScale);
    }

    __forceinline HRESULT SetFrequencyScale(XAUDIOFREQSCALE Scale)
    {
        return XAudioSourceVoice_SetFrequencyScale(this, Scale);
    }
};
#endif

// ---- submix and mastering voices ----

STDAPI XAudioSubmixVoice_GetVoiceFormat(IXAudioSubmixVoice *pVoice, LPXAUDIOFORMAT pFormat);

#ifdef __cplusplus
struct __declspec(novtable) IXAudioSubmixVoice : public IXAudioVoice
{
    __forceinline HRESULT GetVoiceFormat(LPXAUDIOFORMAT pFormat)
    {
        return XAudioSubmixVoice_GetVoiceFormat(this, pFormat);
    }
};
#endif

STDAPI XAudioMasteringVoice_GetVoiceFormat(IXAudioMasteringVoice *pVoice, LPXAUDIOFORMAT pFormat);

#ifdef __cplusplus
struct __declspec(novtable) IXAudioMasteringVoice : public IXAudioVoice
{
    __forceinline HRESULT GetVoiceFormat(LPXAUDIOFORMAT pFormat)
    {
        return XAudioMasteringVoice_GetVoiceFormat(this, pFormat);
    }
};
#endif

#pragma pack(pop)

#endif // __XAUDIO_INCLUDED__
