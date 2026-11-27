/*
 * Port override of the XDK's string.h (searched ahead of port/sdk/xbox-api/include).
 *
 * Game TUs compile with -fno-builtin so Xbox SDK prototypes do not collide with compiler
 * library builtins; without this override, every fixed-size memcpy/memset/memmove/memcmp
 * becomes an out-of-line CRT call (and on Linux64, an MS-ABI -> SysV trampoline).
 *
 * Using function-like macros forwarding to static inline helpers in the global namespace:
 * - works with both unqualified calls and ::memcpy / ::memset / ::memcmp / ::memmove;
 * - leaves `using ::memcpy;` and address-of `(&memset)` untouched (no macro expansion without '(');
 * - never emits a TU-local definition of memcpy/memset/memcmp, avoiding symbol clashes and
 *   self-recursion when LLVM lowers dynamic-size intrinsics to external library calls.
 */
#include_next <string.h>

#if defined(HALO3_64BIT) && !defined(PORT_SDK_STRING_H)
#define PORT_SDK_STRING_H

static __attribute__((always_inline)) inline void *port_crt_memcpy(void *_Dst, const void *_Src, size_t _Size)
{
    return __builtin_memcpy(_Dst, _Src, _Size);
}

static __attribute__((always_inline)) inline void *port_crt_memmove(void *_Dst, const void *_Src, size_t _Size)
{
    return __builtin_memmove(_Dst, _Src, _Size);
}

static __attribute__((always_inline)) inline void *port_crt_memset(void *_Dst, int _Val, size_t _Size)
{
    return __builtin_memset(_Dst, _Val, _Size);
}

static __attribute__((always_inline)) inline int port_crt_memcmp(const void *_Buf1, const void *_Buf2, size_t _Size)
{
    if (__builtin_constant_p(_Size)) {
        if (_Size == 0)
            return 0;
        if (_Size <= 64) {
            const unsigned char *a = (const unsigned char *)_Buf1;
            const unsigned char *b = (const unsigned char *)_Buf2;
            size_t i = 0;
            for (; i + 8 <= _Size; i += 8) {
                unsigned long long wa, wb;
                __builtin_memcpy(&wa, a + i, 8);
                __builtin_memcpy(&wb, b + i, 8);
                if (wa != wb) {
                    unsigned int byte_idx = (unsigned int)__builtin_ctzll(wa ^ wb) >> 3;
                    return (int)a[i + byte_idx] - (int)b[i + byte_idx];
                }
            }
            for (; i + 4 <= _Size; i += 4) {
                unsigned int wa, wb;
                __builtin_memcpy(&wa, a + i, 4);
                __builtin_memcpy(&wb, b + i, 4);
                if (wa != wb) {
                    unsigned int byte_idx = (unsigned int)__builtin_ctz(wa ^ wb) >> 3;
                    return (int)a[i + byte_idx] - (int)b[i + byte_idx];
                }
            }
            for (; i < _Size; i++) {
                if (a[i] != b[i])
                    return (int)a[i] - (int)b[i];
            }
            return 0;
        }
    }
    return (memcmp)(_Buf1, _Buf2, _Size);
}

#define memcpy(dst, src, size) port_crt_memcpy((dst), (src), (size))
#define memmove(dst, src, size) port_crt_memmove((dst), (src), (size))
#define memset(dst, val, size) port_crt_memset((dst), (val), (size))
#define memcmp(buf1, buf2, size) port_crt_memcmp((buf1), (buf2), (size))

#endif /* HALO3_64BIT && !PORT_SDK_STRING_H */
