// Halo 3 PC port: clean-room Xbox 360 SDK headers (zero-XDK build).
// pix.h: PIX profiling event/marker hooks (compiled out unless a debug/profile build enables USE_PIX).
#ifndef _PIX_H_
#define _PIX_H_

#if ((defined(_DEBUG) || DBG || (defined(PROFILE) && !defined(FASTCAP)) || defined(PROFILE_BUILD)) && !defined(i386))
#define USE_PIX
#endif

#define MAX_PIX_STRING_LENGTH 32

#ifdef __cplusplus
extern "C" {
#endif

void WINAPI PIXAddNamedCounter(float value, const char *szName, ...);

#if defined(USE_PIX)
void WINAPI PIXBeginNamedEvent_Copy_NoVarArgs(DWORD Color, const char *pFormat);
void WINAPI PIXBeginNamedEvent_Copy_VarArgs(DWORD Color, const char *pFormat, ...);
void WINAPI PIXBeginNamedEvent_Copy_Float(DWORD Color, const char *pFormat, float Float);
void WINAPI PIXBeginNamedEvent_Copy_Double(DWORD Color, const char *pFormat, double Double);
void WINAPI PIXBeginNamedEvent_Copy_Int(DWORD Color, const char *pFormat, int Int);
void WINAPI PIXBeginNamedEvent_Copy_Int64(DWORD Color, const char *pFormat, INT64 Int64);
#if defined(__cplusplus) && !defined(PIX_NO_OVERLOAD)
// One-argument formats are routed to the typed entry points (integers narrower than 64 bits travel as int).
extern "C++" {
__forceinline void WINAPI PIXBeginNamedEvent_Copy_VarArgs(DWORD Color, const char *pFormat, float v) { PIXBeginNamedEvent_Copy_Float(Color, pFormat, v); }
__forceinline void WINAPI PIXBeginNamedEvent_Copy_VarArgs(DWORD Color, const char *pFormat, double v) { PIXBeginNamedEvent_Copy_Double(Color, pFormat, v); }
__forceinline void WINAPI PIXBeginNamedEvent_Copy_VarArgs(DWORD Color, const char *pFormat, char v) { PIXBeginNamedEvent_Copy_Int(Color, pFormat, (int)v); }
__forceinline void WINAPI PIXBeginNamedEvent_Copy_VarArgs(DWORD Color, const char *pFormat, short v) { PIXBeginNamedEvent_Copy_Int(Color, pFormat, (int)v); }
__forceinline void WINAPI PIXBeginNamedEvent_Copy_VarArgs(DWORD Color, const char *pFormat, int v) { PIXBeginNamedEvent_Copy_Int(Color, pFormat, v); }
__forceinline void WINAPI PIXBeginNamedEvent_Copy_VarArgs(DWORD Color, const char *pFormat, long v) { PIXBeginNamedEvent_Copy_Int(Color, pFormat, (int)v); }
__forceinline void WINAPI PIXBeginNamedEvent_Copy_VarArgs(DWORD Color, const char *pFormat, INT64 v) { PIXBeginNamedEvent_Copy_Int64(Color, pFormat, v); }
__forceinline void WINAPI PIXBeginNamedEvent_Copy_VarArgs(DWORD Color, const char *pFormat, unsigned char v) { PIXBeginNamedEvent_Copy_Int(Color, pFormat, (int)v); }
__forceinline void WINAPI PIXBeginNamedEvent_Copy_VarArgs(DWORD Color, const char *pFormat, unsigned short v) { PIXBeginNamedEvent_Copy_Int(Color, pFormat, (int)v); }
__forceinline void WINAPI PIXBeginNamedEvent_Copy_VarArgs(DWORD Color, const char *pFormat, UINT v) { PIXBeginNamedEvent_Copy_Int(Color, pFormat, (int)v); }
__forceinline void WINAPI PIXBeginNamedEvent_Copy_VarArgs(DWORD Color, const char *pFormat, ULONG v) { PIXBeginNamedEvent_Copy_Int(Color, pFormat, (int)v); }
__forceinline void WINAPI PIXBeginNamedEvent_Copy_VarArgs(DWORD Color, const char *pFormat, UINT64 v) { PIXBeginNamedEvent_Copy_Int64(Color, pFormat, (INT64)v); }
}
#endif
#define PIXHasVarArgs(szString, ...) ("" #__VA_ARGS__[0] != 0)
#define PIXBeginNamedEvent(Color, pFormat, ...)                                                    \
    (PIXHasVarArgs(pFormat, __VA_ARGS__) ? (PIXBeginNamedEvent_Copy_VarArgs(Color, pFormat, __VA_ARGS__), 0) \
                                         : (PIXBeginNamedEvent_Copy_NoVarArgs(Color, pFormat), 0))
void WINAPI PIXEndNamedEvent();
void WINAPI PIXSetMarker(DWORD Color, const char *szName, ...);
void WINAPI PIXNameThread(const char *szName);
#else
// Compiled out: the event/marker "calls" short-circuit to nothing.
#define PIXBeginNamedEvent 0 &&
#define PIXEndNamedEvent()
#define PIXSetMarker 0 &&
#define PIXNameThread 0 &&
#endif

#ifdef __cplusplus
}
#endif

#endif // _PIX_H_
