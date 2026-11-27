// Halo 3 PC port: clean-room Xbox 360 SDK headers (zero-XDK build).
// basetsd.h: fixed-width and pointer-width integer typedefs of the Win32 API (32-bit target).
#ifndef _BASETSD_H_
#define _BASETSD_H_
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef signed char INT8, *PINT8;
typedef signed short INT16, *PINT16;
typedef signed int INT32, *PINT32;
typedef signed __int64 INT64, *PINT64;
typedef unsigned char UINT8, *PUINT8;
typedef unsigned short UINT16, *PUINT16;
typedef unsigned int UINT32, *PUINT32;
typedef unsigned __int64 UINT64, *PUINT64;
typedef signed int LONG32, *PLONG32;
typedef unsigned int ULONG32, *PULONG32;
typedef unsigned int DWORD32, *PDWORD32;

// The x86 "__w64" marker is never used: the build defines no x86 machine macros.
#ifndef _W64
#define _W64
#endif

#if defined(_WIN64)
typedef __int64 INT_PTR, *PINT_PTR;
typedef unsigned __int64 UINT_PTR, *PUINT_PTR;
typedef __int64 LONG_PTR, *PLONG_PTR;
typedef unsigned __int64 ULONG_PTR, *PULONG_PTR;
#define __int3264 __int64

typedef int HALF_PTR, *PHALF_PTR;
typedef unsigned int UHALF_PTR, *PUHALF_PTR;
typedef __int64 SHANDLE_PTR;
typedef unsigned __int64 HANDLE_PTR;
#define ADDRESS_TAG_BIT 0x40000000000ULL
#else
typedef int INT_PTR, *PINT_PTR;
typedef unsigned int UINT_PTR, *PUINT_PTR;
typedef long LONG_PTR, *PLONG_PTR;
typedef unsigned long ULONG_PTR, *PULONG_PTR;
#define __int3264 __int32

typedef short HALF_PTR, *PHALF_PTR;
typedef unsigned short UHALF_PTR, *PUHALF_PTR;
typedef long SHANDLE_PTR;
typedef unsigned long HANDLE_PTR;
#define ADDRESS_TAG_BIT 0x80000000UL
#endif

#define HandleToULong(h) ((ULONG)(ULONG_PTR)(h))
#define HandleToLong(h) ((LONG)(LONG_PTR)(h))
#define ULongToHandle(ul) ((HANDLE)(ULONG_PTR)(ul))
#define LongToHandle(h) ((HANDLE)(LONG_PTR)(h))
#define PtrToUlong(p) ((ULONG)(ULONG_PTR)(p))
#define PtrToLong(p) ((LONG)(LONG_PTR)(p))
#define PtrToUint(p) ((UINT)(UINT_PTR)(p))
#define PtrToInt(p) ((INT)(INT_PTR)(p))
#define PtrToUshort(p) ((unsigned short)(ULONG_PTR)(p))
#define PtrToShort(p) ((short)(LONG_PTR)(p))
#define IntToPtr(i) ((VOID *)(INT_PTR)((int)i))
#define UIntToPtr(ui) ((VOID *)(UINT_PTR)((unsigned int)ui))
#define LongToPtr(l) ((VOID *)(LONG_PTR)((long)l))
#define ULongToPtr(ul) ((VOID *)(ULONG_PTR)((unsigned long)ul))
#define HandleToUlong(h) HandleToULong(h)
#define UlongToHandle(ul) ULongToHandle(ul)
#define UlongToPtr(ul) ULongToPtr(ul)
#define UintToPtr(ui) UIntToPtr(ui)

#define MAXUINT_PTR (~((UINT_PTR)0))
#define MAXINT_PTR ((INT_PTR)(MAXUINT_PTR >> 1))
#define MININT_PTR (~MAXINT_PTR)
#define MAXULONG_PTR (~((ULONG_PTR)0))
#define MAXLONG_PTR ((LONG_PTR)(MAXULONG_PTR >> 1))
#define MINLONG_PTR (~MAXLONG_PTR)
#define MAXUHALF_PTR ((UHALF_PTR)~0)
#define MAXHALF_PTR ((HALF_PTR)(MAXUHALF_PTR >> 1))
#define MINHALF_PTR (~MAXHALF_PTR)

typedef ULONG_PTR SIZE_T, *PSIZE_T;
typedef LONG_PTR SSIZE_T, *PSSIZE_T;
typedef ULONG_PTR DWORD_PTR, *PDWORD_PTR;
typedef __int64 LONG64, *PLONG64;
typedef unsigned __int64 ULONG64, *PULONG64;
typedef unsigned __int64 DWORD64, *PDWORD64;
typedef ULONG_PTR KAFFINITY, *PKAFFINITY;

#ifdef __cplusplus
}
#endif

#endif // _BASETSD_H_
