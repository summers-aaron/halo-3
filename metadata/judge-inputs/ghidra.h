/* Ghidra decompiler base types, for compiling decompiler output as C.
   Shipped with Byte-for-Byte for its automatic author; reconstructed sources should prefer real types. */
#pragma once
typedef unsigned char undefined;
typedef unsigned char undefined1;
typedef unsigned short undefined2;
typedef unsigned int undefined4;
typedef unsigned long long undefined8;
/* Wider pieces (a vector register's bytes 4..15 and so on) as byte arrays, copied whole. */
typedef struct { unsigned char bytes[9]; } undefined9;
typedef struct { unsigned char bytes[10]; } undefined10;
typedef struct { unsigned char bytes[11]; } undefined11;
typedef struct { unsigned char bytes[12]; } undefined12;
typedef struct { unsigned char bytes[13]; } undefined13;
typedef struct { unsigned char bytes[14]; } undefined14;
typedef struct { unsigned char bytes[15]; } undefined15;
/* Odd-width values Ghidra types by their byte count, held in the next wider register. */
typedef unsigned int undefined3;
typedef unsigned long long undefined5;
typedef unsigned long long undefined6;
typedef unsigned long long undefined7;
typedef unsigned char byte;
typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned long ulong;
typedef long long longlong;
typedef unsigned long long ulonglong;
typedef unsigned short wchar16;
#ifndef __cplusplus
typedef unsigned char bool;
typedef unsigned short wchar_t;
#define true 1
#define false 0
#endif
#ifdef __cplusplus
typedef long long code(...);
#else
typedef long long code();
#endif
#define CONCAT_(x, y, a, b) ((((ulonglong)(a)) << ((y) * 8)) | (((ulonglong)(b)) & (~0ULL >> (64 - (y) * 8))))
#define CONCAT11(a, b) ((ushort)CONCAT_(1, 1, a, b))
#define CONCAT12(a, b) ((uint)CONCAT_(1, 2, a, b))
#define CONCAT13(a, b) ((uint)CONCAT_(1, 3, a, b))
#define CONCAT14(a, b) CONCAT_(1, 4, a, b)
#define CONCAT21(a, b) ((uint)CONCAT_(2, 1, a, b))
#define CONCAT22(a, b) ((uint)CONCAT_(2, 2, a, b))
#define CONCAT24(a, b) CONCAT_(2, 4, a, b)
#define CONCAT26(a, b) CONCAT_(2, 6, a, b)
#define CONCAT31(a, b) ((uint)CONCAT_(3, 1, a, b))
#define CONCAT41(a, b) CONCAT_(4, 1, a, b)
#define CONCAT44(a, b) CONCAT_(4, 4, a, b)
#define CONCAT51(a, b) CONCAT_(5, 1, a, b)
#define CONCAT61(a, b) CONCAT_(6, 1, a, b)
#define CONCAT62(a, b) CONCAT_(6, 2, a, b)
#define CONCAT71(a, b) CONCAT_(7, 1, a, b)
/* The other CONCATab (a + b <= 8 bytes) and SUBab(x, n): the b bytes of the a-byte x from its byte n (counted from the least significant). */
#define CONCAT15(a, b) CONCAT_(1, 5, a, b)
#define CONCAT16(a, b) CONCAT_(1, 6, a, b)
#define CONCAT17(a, b) CONCAT_(1, 7, a, b)
#define CONCAT23(a, b) CONCAT_(2, 3, a, b)
#define CONCAT25(a, b) CONCAT_(2, 5, a, b)
#define CONCAT32(a, b) CONCAT_(3, 2, a, b)
#define CONCAT33(a, b) CONCAT_(3, 3, a, b)
#define CONCAT34(a, b) CONCAT_(3, 4, a, b)
#define CONCAT35(a, b) CONCAT_(3, 5, a, b)
#define CONCAT42(a, b) CONCAT_(4, 2, a, b)
#define CONCAT43(a, b) CONCAT_(4, 3, a, b)
#define CONCAT52(a, b) CONCAT_(5, 2, a, b)
#define CONCAT53(a, b) CONCAT_(5, 3, a, b)
#define SUB21(x, n) ((uchar)((ulonglong)(x) >> ((n) * 8)))
#define SUB31(x, n) ((uchar)((ulonglong)(x) >> ((n) * 8)))
#define SUB32(x, n) ((ushort)((ulonglong)(x) >> ((n) * 8)))
#define SUB41(x, n) ((uchar)((ulonglong)(x) >> ((n) * 8)))
#define SUB42(x, n) ((ushort)((ulonglong)(x) >> ((n) * 8)))
#define SUB43(x, n) ((uint)((ulonglong)(x) >> ((n) * 8)))
#define SUB51(x, n) ((uchar)((ulonglong)(x) >> ((n) * 8)))
#define SUB52(x, n) ((ushort)((ulonglong)(x) >> ((n) * 8)))
#define SUB53(x, n) ((uint)((ulonglong)(x) >> ((n) * 8)))
#define SUB54(x, n) ((uint)((ulonglong)(x) >> ((n) * 8)))
#define SUB61(x, n) ((uchar)((ulonglong)(x) >> ((n) * 8)))
#define SUB62(x, n) ((ushort)((ulonglong)(x) >> ((n) * 8)))
#define SUB63(x, n) ((uint)((ulonglong)(x) >> ((n) * 8)))
#define SUB64(x, n) ((uint)((ulonglong)(x) >> ((n) * 8)))
#define SUB65(x, n) ((ulonglong)((ulonglong)(x) >> ((n) * 8)))
#define SUB71(x, n) ((uchar)((ulonglong)(x) >> ((n) * 8)))
#define SUB72(x, n) ((ushort)((ulonglong)(x) >> ((n) * 8)))
#define SUB73(x, n) ((uint)((ulonglong)(x) >> ((n) * 8)))
#define SUB74(x, n) ((uint)((ulonglong)(x) >> ((n) * 8)))
#define SUB75(x, n) ((ulonglong)((ulonglong)(x) >> ((n) * 8)))
#define SUB76(x, n) ((ulonglong)((ulonglong)(x) >> ((n) * 8)))
#define SUB81(x, n) ((uchar)((ulonglong)(x) >> ((n) * 8)))
#define SUB82(x, n) ((ushort)((ulonglong)(x) >> ((n) * 8)))
#define SUB83(x, n) ((uint)((ulonglong)(x) >> ((n) * 8)))
#define SUB84(x, n) ((uint)((ulonglong)(x) >> ((n) * 8)))
#define SUB85(x, n) ((ulonglong)((ulonglong)(x) >> ((n) * 8)))
#define SUB86(x, n) ((ulonglong)((ulonglong)(x) >> ((n) * 8)))
#define SUB87(x, n) ((ulonglong)((ulonglong)(x) >> ((n) * 8)))
/* Zero and sign extension from n to m bytes (ZEXTnm, SEXTnm) for the scalar sizes. */
#define ZEXT12(x) ((ushort)(uchar)(x))
#define ZEXT14(x) ((uint)(uchar)(x))
#define ZEXT18(x) ((ulonglong)(uchar)(x))
#define ZEXT24(x) ((uint)(ushort)(x))
#define ZEXT28(x) ((ulonglong)(ushort)(x))
#define ZEXT48(x) ((ulonglong)(uint)(x))
#define SEXT12(x) ((short)(signed char)(x))
#define SEXT14(x) ((int)(signed char)(x))
#define SEXT18(x) ((longlong)(signed char)(x))
#define SEXT24(x) ((int)(short)(x))
#define SEXT28(x) ((longlong)(short)(x))
#define SEXT48(x) ((longlong)(int)(x))
/* Windows scalar typedefs Ghidra uses by name. */
typedef unsigned int DWORD;
typedef unsigned int UINT;
typedef unsigned int ULONG;
typedef int BOOL;
typedef int LONG;
typedef int HRESULT;
typedef unsigned short WORD;
typedef unsigned char BYTE;
typedef unsigned long long SIZE_T;
typedef unsigned long long ULONG_PTR;
typedef long long LONG_PTR;
typedef unsigned long long DWORD64;
typedef unsigned long long ULONGLONG;
typedef long long LONGLONG;
typedef void *HANDLE;
typedef void *LPVOID;
typedef void *PVOID;
typedef const void *LPCVOID;
typedef char CHAR;
typedef wchar_t WCHAR;
typedef float FLOAT;
typedef unsigned long long __uint64;
typedef long long LPARAM;
typedef unsigned long long WPARAM;
typedef long long LRESULT;
typedef int int3;
typedef long long int5;
typedef long long int6;
typedef long long int7;
typedef unsigned int uint3;
typedef unsigned long long uint5;
typedef unsigned long long uint6;
typedef unsigned long long uint7;
typedef signed char sbyte;
/* Thread-local storage as Ghidra spells it: the TEB's TLS array and the image's slot index. */
unsigned long long __readgsqword(unsigned long);
#pragma intrinsic(__readgsqword)
#define ThreadLocalStoragePointer __readgsqword(0x58)
extern unsigned int _tls_index;
/* A word Ghidra types two ways (a float read from a pointer-typed slot, a pointer from a float): its bits
   reinterpreted through memory, as the program stores one register and loads the other. */
static __inline float __bits_float(unsigned int bits) { return *(float *)&bits; }
static __inline unsigned int __float_bits(float value) { return *(unsigned int *)&value; }
