#ifndef CSERIES_CRT_XDK_H
#define CSERIES_CRT_XDK_H

#include <stddef.h>
#include <stdarg.h>
#include <string.h>
extern "C" errno_t strncpy_s(char *destination, size_t destination_size, const char *source, size_t count);
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <ctype.h>
#include <wchar.h>
#include <ppcintrinsics.h>
#include "cseries/rotate_intrinsics.h"

// PowerPC atomic primitives (XDK winbase.h declarations).
extern "C" {
    long _InterlockedIncrement(long volatile *lpAddend);
    long _InterlockedDecrement(long volatile *lpAddend);
    long _InterlockedExchange(long volatile *Target, long Value);
    long _InterlockedExchangeAdd(long volatile *Addend, long Value);
    long _InterlockedCompareExchange(long volatile *Destination, long Exchange, long Comperand);
}

// CRT time (time.h has the same declaration; a unit that defines its own `time` cannot include time.h, whose
// time.inl defines `time` static inline).
extern "C" __int64 __cdecl _time64(__int64 *time);

// Lightweight XDK xapilib & xboxkrnl declarations (omitted when the SDK headers already declare them).
#if !defined(_XTL_) && !defined(_WINBASE_)
typedef struct _RTL_CRITICAL_SECTION {
    long SynchronizationRaw[7];
} RTL_CRITICAL_SECTION;
struct _SECURITY_ATTRIBUTES;
union _LARGE_INTEGER;

extern "C" {
    // xapilib memory (0x817b0 = XMemCpy, 0x800c8 = XMemSet; distinct from LIBCMT memcpy 0x58e090 / memset 0x58e5c0)
    void *XMemCpy(void *pDest, const void *pSrc, size_t count);
    void *XMemSet(void *pDest, int c, size_t count);

    // xboxkrnl TLS & Critical Sections
    unsigned long TlsAlloc(void);
    int TlsFree(unsigned long dwTlsIndex);
    void *TlsGetValue(unsigned long dwTlsIndex);
    int TlsSetValue(unsigned long dwTlsIndex, void *lpTlsValue);
    void RtlInitializeCriticalSection(_RTL_CRITICAL_SECTION *cs);
    void RtlInitializeCriticalSectionAndSpinCount(_RTL_CRITICAL_SECTION *cs, unsigned long spinCount);
    void RtlEnterCriticalSection(_RTL_CRITICAL_SECTION *cs);
    void RtlLeaveCriticalSection(_RTL_CRITICAL_SECTION *cs);
    unsigned long RtlTryEnterCriticalSection(_RTL_CRITICAL_SECTION *cs);

    // xapilib timing, threading, synchronization, heap, and locale
    unsigned long XShowSigninUI(unsigned long paneCount, unsigned long flags);
    unsigned long __stdcall GetTickCount(void);
    int QueryPerformanceCounter(_LARGE_INTEGER *lpPerformanceCount);
    int QueryPerformanceFrequency(_LARGE_INTEGER *lpFrequency);
    unsigned long GetCurrentThreadId(void);
    unsigned long GetLastError(void);
    unsigned long WaitForSingleObject(void *hHandle, unsigned long dwMilliseconds);
    unsigned long WaitForSingleObjectEx(void *hHandle, unsigned long dwMilliseconds, int bAlertable);
    unsigned long WaitForMultipleObjectsEx(unsigned long nCount, void *const *lpHandles, int bWaitAll, unsigned long dwMilliseconds, int bAlertable);
    void *CreateEventA(_SECURITY_ATTRIBUTES *attr, int manualReset, int initialState, const char *name);
    int SetEvent(void *hEvent);
    int ResetEvent(void *hEvent);
    void *CreateMutexA(_SECURITY_ATTRIBUTES *attr, int initialOwner, const char *name);
    int ReleaseMutex(void *hMutex);
    void *CreateSemaphoreA(_SECURITY_ATTRIBUTES *attr, long initCount, long maxCount, const char *name);
    int ReleaseSemaphore(void *hSemaphore, long lReleaseCount, long *lpPreviousCount);
    int CloseHandle(void *hObject);
    int GetDiskFreeSpaceExA(const char *directory, void *available, void *total, void *free);
    void *CreateFileA(const char *name, unsigned long access, unsigned long share, void *security,
        unsigned long disposition, unsigned long attributes, void *template_file);
    unsigned long XFlushUtilityDrive(void);
    void Sleep(unsigned long dwMilliseconds);
    int SwitchToThread(void);
    int SetThreadPriority(void *thread_handle, int priority);
    void *GetProcessHeap(void);
    void *RtlAllocateHeap(void *hHeap, unsigned long dwFlags, size_t dwBytes);
    unsigned char RtlFreeHeap(void *hHeap, unsigned long dwFlags, void *lpMem);
    void *VirtualAlloc(void *lpAddress, size_t dwSize, unsigned long flAllocationType, unsigned long flProtect);
    int VirtualFree(void *lpAddress, size_t dwSize, unsigned long dwFreeType);
    unsigned long XTLGetLanguage(void);
    unsigned long XGetLocale(void);
    unsigned long XUserGetName(unsigned long user_index, char *name, unsigned long maximum_count);
}

// xboxkrnl's exported kernel version (an imported data object)
typedef struct _XBOX_KRNL_VERSION {
    unsigned short Major;
    unsigned short Minor;
    unsigned short Build;
    unsigned short Qfe;
} XBOX_KRNL_VERSION;

extern "C" __declspec(dllimport) XBOX_KRNL_VERSION XboxKrnlVersion;
#endif // !_XTL_ && !_WINBASE_

#endif // CSERIES_CRT_XDK_H
