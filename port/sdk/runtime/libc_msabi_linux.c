// port/sdk/runtime/libc_msabi_linux.c: Win64 ms_abi -> SysV AMD64 libc/libm forwarders for Linux x86_64.
// Game/SDK objects are compiled with --target=x86_64-pc-windows-msvc-elf (Win64 ms_abi, 32-bit long).
// Their undefined libc/libm references are renamed foo -> __ms_foo at link time via llvm-objcopy so that
// host runtime/data/gpu objects and glibc/libm continue to call native SysV libc/libm directly.
#ifdef __x86_64__

#include "port_abi.h"

#include <ctype.h>
#include <emmintrin.h>
#include <math.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wctype.h>

// ---- process / allocation ----
PORT_MS_ABI void __ms_abort(void) { abort(); }
PORT_MS_ABI void *__ms_malloc(size_t n) { return malloc(n); }
PORT_MS_ABI void *__ms_calloc(size_t nmemb, size_t size) { return calloc(nmemb, size); }
PORT_MS_ABI void *__ms_realloc(void *p, size_t n) { return realloc(p, n); }
PORT_MS_ABI void __ms_free(void *p) { free(p); }
PORT_MS_ABI char *__ms_getenv(const char *name) { return getenv(name); }
PORT_MS_ABI int __ms_abs(int x) { return abs(x); }
PORT_MS_ABI int32_t __ms_labs(int32_t x) { return x < 0 ? -x : x; }
PORT_MS_ABI int __ms_atoi(const char *s) { return atoi(s); }
PORT_MS_ABI int32_t __ms_atol(const char *s) { return (int32_t)atol(s); }
PORT_MS_ABI double __ms_strtod(const char *s, char **end) { return strtod(s, end); }
PORT_MS_ABI int32_t __ms_strtol(const char *s, char **end, int base) { return (int32_t)strtol(s, end, base); }
PORT_MS_ABI uint32_t __ms_strtoul(const char *s, char **end, int base) { return (uint32_t)strtoul(s, end, base); }

// ---- atexit (callback is ms_abi) ----
typedef void (PORT_MS_ABI *port_ms_atexit_fn)(void);
static port_ms_atexit_fn port_ms_atexit_list[256];
static int port_ms_atexit_count = 0;

static void port_ms_atexit_runner(void)
{
    while (port_ms_atexit_count > 0) {
        port_ms_atexit_fn fn = port_ms_atexit_list[--port_ms_atexit_count];
        if (fn)
            fn();
    }
}

PORT_MS_ABI int __ms_atexit(port_ms_atexit_fn fn)
{
    if (!fn || port_ms_atexit_count >= 256)
        return -1;
    if (port_ms_atexit_count == 0)
        atexit(port_ms_atexit_runner);
    port_ms_atexit_list[port_ms_atexit_count++] = fn;
    return 0;
}

// ---- qsort (comparator is ms_abi) ----
typedef int (PORT_MS_ABI *port_ms_cmp_fn)(const void *, const void *);
static __thread port_ms_cmp_fn port_ms_qsort_cmp = NULL;

static int port_ms_qsort_trampoline(const void *a, const void *b)
{
    return port_ms_qsort_cmp(a, b);
}

PORT_MS_ABI void __ms_qsort(void *base, size_t nmemb, size_t size, port_ms_cmp_fn cmp)
{
    port_ms_cmp_fn prev = port_ms_qsort_cmp;
    port_ms_qsort_cmp = cmp;
    qsort(base, nmemb, size, port_ms_qsort_trampoline);
    port_ms_qsort_cmp = prev;
}

// ---- memory / string ----
PORT_MS_ABI void *__ms_memcpy(void *dst, const void *src, size_t n) { return memcpy(dst, src, n); }
PORT_MS_ABI void *__ms_memmove(void *dst, const void *src, size_t n) { return memmove(dst, src, n); }
PORT_MS_ABI void *__ms_memset(void *dst, int c, size_t n) { return memset(dst, c, n); }
PORT_MS_ABI int __ms_memcmp(const void *a, const void *b, size_t n) { return memcmp(a, b, n); }
PORT_MS_ABI size_t __ms_strlen(const char *s) { return strlen(s); }
PORT_MS_ABI int __ms_strcmp(const char *a, const char *b) { return strcmp(a, b); }
PORT_MS_ABI int __ms_strncmp(const char *a, const char *b, size_t n) { return strncmp(a, b, n); }
PORT_MS_ABI char *__ms_strcpy(char *dst, const char *src) { return strcpy(dst, src); }
PORT_MS_ABI char *__ms_strncpy(char *dst, const char *src, size_t n) { return strncpy(dst, src, n); }
PORT_MS_ABI char *__ms_strcat(char *dst, const char *src) { return strcat(dst, src); }
PORT_MS_ABI char *__ms_strncat(char *dst, const char *src, size_t n) { return strncat(dst, src, n); }
PORT_MS_ABI char *__ms_strchr(const char *s, int c) { return strchr(s, c); }
PORT_MS_ABI char *__ms_strrchr(const char *s, int c) { return strrchr(s, c); }
PORT_MS_ABI char *__ms_strstr(const char *hay, const char *ndl) { return strstr(hay, ndl); }
PORT_MS_ABI char *__ms_strpbrk(const char *s, const char *acc) { return strpbrk(s, acc); }
PORT_MS_ABI size_t __ms_strspn(const char *s, const char *acc) { return strspn(s, acc); }
PORT_MS_ABI size_t __ms_strcspn(const char *s, const char *rej) { return strcspn(s, rej); }

// ---- stdio ----
PORT_MS_ABI FILE *__ms_fopen(const char *path, const char *mode) { return fopen(path, mode); }
PORT_MS_ABI int __ms_fclose(FILE *f) { return fclose(f); }
PORT_MS_ABI int __ms_fflush(FILE *f) { return fflush(f); }
PORT_MS_ABI size_t __ms_fread(void *ptr, size_t size, size_t nmemb, FILE *f) { return fread(ptr, size, nmemb, f); }
PORT_MS_ABI size_t __ms_fwrite(const void *ptr, size_t size, size_t nmemb, FILE *f) { return fwrite(ptr, size, nmemb, f); }
PORT_MS_ABI char *__ms_fgets(char *s, int size, FILE *f) { return fgets(s, size, f); }
PORT_MS_ABI int __ms_fseek(FILE *f, int32_t offset, int whence) { return fseek(f, (long)offset, whence); }
PORT_MS_ABI int32_t __ms_ftell(FILE *f) { return (int32_t)ftell(f); }
PORT_MS_ABI int __ms_fscanf(FILE *f, const char *fmt, ...)
{
    // Minimal bridge for up to 4 pointer arguments if ever reached (rare/unused in retail).
    __builtin_ms_va_list ap;
    __builtin_ms_va_start(ap, fmt);
    void *a0 = __builtin_va_arg(ap, void *);
    void *a1 = __builtin_va_arg(ap, void *);
    void *a2 = __builtin_va_arg(ap, void *);
    void *a3 = __builtin_va_arg(ap, void *);
    __builtin_ms_va_end(ap);
    return fscanf(f, fmt, a0, a1, a2, a3);
}

// ---- ctype / wctype ----
PORT_MS_ABI int __ms_iswalnum(uint16_t c) { return iswalnum((wint_t)c); }
PORT_MS_ABI int __ms_iswalpha(uint16_t c) { return iswalpha((wint_t)c); }
PORT_MS_ABI int __ms_iswdigit(uint16_t c) { return iswdigit((wint_t)c); }
PORT_MS_ABI int __ms_iswspace(uint16_t c) { return iswspace((wint_t)c); }
PORT_MS_ABI uint16_t __ms_towlower(uint16_t c) { return (uint16_t)towlower((wint_t)c); }
PORT_MS_ABI int __ms_tolower(int c) { return tolower(c); }
PORT_MS_ABI int __ms_toupper(int c) { return toupper(c); }
PORT_MS_ABI int __ms_isalpha(int c) { return isalpha(c); }
PORT_MS_ABI int __ms_isdigit(int c) { return isdigit(c); }
PORT_MS_ABI int __ms_isspace(int c) { return isspace(c); }
PORT_MS_ABI int __ms_isalnum(int c) { return isalnum(c); }
PORT_MS_ABI int __ms_isxdigit(int c) { return isxdigit(c); }

// ---- libm ----
// In IEEE binary32/binary64 the ordering of the magnitude bits matches the ordering of the values.
// Clear the fractional bits, with an integer carry for floor of a negative or ceil of a positive.
static inline uint64_t port_round_integral(uint64_t bits, unsigned int fraction, unsigned int bias,
                                           uint64_t sign, int upward)
{
    unsigned int exponent = (unsigned int)((bits & (sign - 1)) >> fraction);
    if (exponent >= bias + fraction)
        return bits;
    int away_from_zero = ((bits & sign) == 0) == upward;
    if (exponent < bias)
        return (bits & sign) | (away_from_zero ? (uint64_t)bias << fraction : 0);
    uint64_t mask = (UINT64_C(1) << (bias + fraction - exponent)) - 1;
    return (bits + (away_from_zero ? mask : 0)) & ~mask;
}

static inline double port_round_double(double x, int upward)
{
    uint64_t bits;
    __builtin_memcpy(&bits, &x, sizeof(bits));
    uint64_t magnitude = bits & UINT64_C(0x7fffffffffffffff);
    if (magnitude >= UINT64_C(0x0010000000000000) && magnitude <= UINT64_C(0x7ff0000000000000)) {
        bits = port_round_integral(bits, 52, 1023, UINT64_C(1) << 63, upward);
        __builtin_memcpy(&x, &bits, sizeof(x));
        return x;
    }
    // The host handles signaling NaNs and the current treatment of subnormals.
    return magnitude == 0 ? x : upward ? ceil(x) : floor(x);
}

static inline float port_round_float(float x, int upward)
{
    uint32_t bits;
    __builtin_memcpy(&bits, &x, sizeof(bits));
    uint32_t magnitude = bits & 0x7fffffffu;
    if (magnitude >= 0x00800000u && magnitude <= 0x7f800000u) {
        bits = (uint32_t)port_round_integral(bits, 23, 127, UINT64_C(1) << 31, upward);
        __builtin_memcpy(&x, &bits, sizeof(x));
        return x;
    }
    return magnitude == 0 ? x : upward ? ceilf(x) : floorf(x);
}

PORT_MS_ABI double __ms_acos(double x) { return acos(x); }
PORT_MS_ABI double __ms_asin(double x) { return asin(x); }
PORT_MS_ABI double __ms_atan(double x) { return atan(x); }
PORT_MS_ABI double __ms_atan2(double y, double x) { return atan2(y, x); }
PORT_MS_ABI double __ms_ceil(double x) { return port_round_double(x, 1); }
PORT_MS_ABI double __ms_cos(double x) { return cos(x); }
PORT_MS_ABI double __ms_exp(double x) { return exp(x); }
PORT_MS_ABI double __ms_fabs(double x) { return __builtin_fabs(x); }
PORT_MS_ABI double __ms_floor(double x) { return port_round_double(x, 0); }
PORT_MS_ABI double __ms_fmod(double x, double y) { return fmod(x, y); }
PORT_MS_ABI double __ms_ldexp(double x, int e) { return ldexp(x, e); }
PORT_MS_ABI double __ms_log(double x) { return log(x); }
PORT_MS_ABI double __ms_log10(double x) { return log10(x); }
PORT_MS_ABI double __ms_pow(double x, double y) { return pow(x, y); }
PORT_MS_ABI double __ms_sin(double x) { return sin(x); }
PORT_MS_ABI double __ms_sqrt(double x)
{
    uint64_t bits;
    __builtin_memcpy(&bits, &x, sizeof(bits));
    if (bits > UINT64_C(0x8000000000000000))
        return sqrt(x); // preserve the host's domain error handling
    return _mm_cvtsd_f64(_mm_sqrt_sd(_mm_setzero_pd(), _mm_set_sd(x)));
}
PORT_MS_ABI double __ms_tan(double x) { return tan(x); }
PORT_MS_ABI float __ms_acosf(float x) { return acosf(x); }
PORT_MS_ABI float __ms_asinf(float x) { return asinf(x); }
PORT_MS_ABI float __ms_atanf(float x) { return atanf(x); }
PORT_MS_ABI float __ms_atan2f(float y, float x) { return atan2f(y, x); }
PORT_MS_ABI float __ms_ceilf(float x) { return port_round_float(x, 1); }
PORT_MS_ABI float __ms_cosf(float x) { return cosf(x); }
PORT_MS_ABI float __ms_expf(float x) { return expf(x); }
PORT_MS_ABI float __ms_fabsf(float x) { return __builtin_fabsf(x); }
PORT_MS_ABI float __ms_floorf(float x) { return port_round_float(x, 0); }
PORT_MS_ABI float __ms_fmodf(float x, float y) { return fmodf(x, y); }
PORT_MS_ABI float __ms_logf(float x) { return logf(x); }
PORT_MS_ABI float __ms_log10f(float x) { return log10f(x); }
PORT_MS_ABI float __ms_powf(float x, float y) { return powf(x, y); }
PORT_MS_ABI float __ms_sinf(float x) { return sinf(x); }
PORT_MS_ABI float __ms_sqrtf(float x)
{
    uint32_t bits;
    __builtin_memcpy(&bits, &x, sizeof(bits));
    if (bits > 0x80000000u)
        return sqrtf(x);
    return _mm_cvtss_f32(_mm_sqrt_ss(_mm_set_ss(x)));
}
PORT_MS_ABI float __ms_tanf(float x) { return tanf(x); }

/* Clang emits libatomic calls when -fmax-type-align=4 makes 8-byte atomic
 * accesses appear potentially under-aligned. Implement them in ms_abi. */
PORT_MS_ABI void port_ms_atomic_exchange(size_t size, void *ptr, const void *val, void *ret, int memorder)
    __asm__("__atomic_exchange");
PORT_MS_ABI void port_ms_atomic_exchange(size_t size, void *ptr, const void *val, void *ret, int memorder)
{
    (void)memorder;
    switch (size) {
    case 1: { uint8_t v; memcpy(&v, val, 1); v = __atomic_exchange_n((uint8_t *)ptr, v, __ATOMIC_SEQ_CST); memcpy(ret, &v, 1); break; }
    case 2: { uint16_t v; memcpy(&v, val, 2); v = __atomic_exchange_n((uint16_t *)ptr, v, __ATOMIC_SEQ_CST); memcpy(ret, &v, 2); break; }
    case 4: { uint32_t v; memcpy(&v, val, 4); v = __atomic_exchange_n((uint32_t *)ptr, v, __ATOMIC_SEQ_CST); memcpy(ret, &v, 4); break; }
    case 8: { uint64_t v; memcpy(&v, val, 8); v = __atomic_exchange_n((uint64_t *)ptr, v, __ATOMIC_SEQ_CST); memcpy(ret, &v, 8); break; }
    default: memcpy(ret, ptr, size); memcpy(ptr, val, size); __atomic_thread_fence(__ATOMIC_SEQ_CST); break;
    }
}

PORT_MS_ABI void port_ms_atomic_store(size_t size, void *ptr, const void *val, int memorder)
    __asm__("__atomic_store");
PORT_MS_ABI void port_ms_atomic_store(size_t size, void *ptr, const void *val, int memorder)
{
    (void)memorder;
    switch (size) {
    case 1: { uint8_t v; memcpy(&v, val, 1); __atomic_store_n((uint8_t *)ptr, v, __ATOMIC_SEQ_CST); break; }
    case 2: { uint16_t v; memcpy(&v, val, 2); __atomic_store_n((uint16_t *)ptr, v, __ATOMIC_SEQ_CST); break; }
    case 4: { uint32_t v; memcpy(&v, val, 4); __atomic_store_n((uint32_t *)ptr, v, __ATOMIC_SEQ_CST); break; }
    case 8: { uint64_t v; memcpy(&v, val, 8); __atomic_store_n((uint64_t *)ptr, v, __ATOMIC_SEQ_CST); break; }
    default: memcpy(ptr, val, size); __atomic_thread_fence(__ATOMIC_SEQ_CST); break;
    }
}

PORT_MS_ABI void port_ms_atomic_load(size_t size, const void *ptr, void *ret, int memorder)
    __asm__("__atomic_load");
PORT_MS_ABI void port_ms_atomic_load(size_t size, const void *ptr, void *ret, int memorder)
{
    (void)memorder;
    switch (size) {
    case 1: { uint8_t v = __atomic_load_n((const uint8_t *)ptr, __ATOMIC_SEQ_CST); memcpy(ret, &v, 1); break; }
    case 2: { uint16_t v = __atomic_load_n((const uint16_t *)ptr, __ATOMIC_SEQ_CST); memcpy(ret, &v, 2); break; }
    case 4: { uint32_t v = __atomic_load_n((const uint32_t *)ptr, __ATOMIC_SEQ_CST); memcpy(ret, &v, 4); break; }
    case 8: { uint64_t v = __atomic_load_n((const uint64_t *)ptr, __ATOMIC_SEQ_CST); memcpy(ret, &v, 8); break; }
    default: __atomic_thread_fence(__ATOMIC_SEQ_CST); memcpy(ret, ptr, size); break;
    }
}

#endif
