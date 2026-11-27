// port/sdk/runtime (Linux host): the MSVC C runtime the game's code expects, where 32-bit glibc lacks it or means
// something else. The game is compiled with the MS ABI, so:
//   wchar_t is 16 bits (glibc's is 32): every wide-string function the game calls is defined here, so glibc's never
//     binds;
//   rand() is MSVC's 15-bit generator (RAND_MAX 0x7fff), which the game's arithmetic assumes;
//   printf formats are MSVC's (%I64d, %S, three-digit exponents): ms_printf_linux.c formats them;
//   the stack probe, _errno, the _s functions and the 64-bit time functions are MSVC names (_purecall: crt_purecall.c).
// Host code, compiled for i686-linux-gnu: it never sees the XDK headers.
#include "port_runtime.h"

#include <errno.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>

extern long strtol(const char *, char **, int);
extern unsigned long strtoul(const char *, char **, int);
extern long long strtoll(const char *, char **, int);

typedef unsigned short port_wchar; // the game's wchar_t

#ifdef __x86_64__
// Win64 __chkstk: %rax holds byte count; probes pages downward from %rsp without modifying %rsp or %rax.
__asm__(
    "\t.text\n"
    "\t.globl\t__chkstk\n"
    "\t.type\t__chkstk, @function\n"
    "__chkstk:\n"
    "\tsubq\t$16, %rsp\n"
    "\tmovq\t%r10, (%rsp)\n"
    "\tmovq\t%r11, 8(%rsp)\n"
    "\tleaq\t24(%rsp), %r10\n"
    "\tmovq\t%r10, %r11\n"
    "\tsubq\t%rax, %r10\n"
    "\tjb\t2f\n"
    "1:\n"
    "\tsubq\t$0x1000, %r11\n"
    "\ttestb\t$0, (%r11)\n"
    "\tcmpq\t%r10, %r11\n"
    "\tja\t1b\n"
    "2:\n"
    "\tmovq\t(%rsp), %r10\n"
    "\tmovq\t8(%rsp), %r11\n"
    "\taddq\t$16, %rsp\n"
    "\tretq\n");
#else
// _chkstk: eax bytes of new stack frame. Linux grows the main thread's stack on demand and gives other threads
// their whole stack up front, so the probe needs no page order; it still touches each page, as the caller expects.
__asm__(
    "\t.text\n"
    "\t.globl\t_chkstk\n"
    "\t.type\t_chkstk, @function\n"
    "_chkstk:\n"
    "\tpushl\t%ecx\n"
    "\tleal\t8(%esp), %ecx\n"     // the caller's esp before the call
    "1:\n"
    "\tcmpl\t$0x1000, %eax\n"
    "\tjb\t2f\n"
    "\tsubl\t$0x1000, %ecx\n"
    "\ttestl\t%ecx, (%ecx)\n"
    "\tsubl\t$0x1000, %eax\n"
    "\tjmp\t1b\n"
    "2:\n"
    "\tsubl\t%eax, %ecx\n"
    "\ttestl\t%ecx, (%ecx)\n"
    "\tleal\t4(%esp), %eax\n"     // address of the return address
    "\tmovl\t%ecx, %esp\n"        // the new stack top
    "\tmovl\t-4(%eax), %ecx\n"    // restore ecx
    "\tpushl\t(%eax)\n"           // return address on the new stack
    "\tsubl\t%esp, %eax\n"        // eax back to the original size
    "\tret\n");
#endif

int *PORT_MS_ABI _errno(void)
{
    return &errno;
}

void PORT_MS_ABI _invalid_parameter(const port_wchar *expression, const port_wchar *function, const port_wchar *file,
    unsigned int line, unsigned int reserved)
{
    (void)expression; (void)function; (void)file; (void)line; (void)reserved;
    port_write_error("port: invalid parameter passed to a C runtime function\n");
    port_exit(6);
}

long long PORT_MS_ABI _atoi64(const char *text)
{
    return strtoll(text, 0, 10);
}

unsigned long PORT_MS_ABI _byteswap_ulong(unsigned long value)
{
    return __builtin_bswap32(value);
}

unsigned short PORT_MS_ABI _byteswap_ushort(unsigned short value)
{
    return __builtin_bswap16(value);
}

// The x87 control word as MSVC reports it (the _MCW_* bits): 53-bit precision, round to nearest, all exceptions
// masked. Changes are accepted and ignored: the game sets the defaults.
unsigned int PORT_MS_ABI _controlfp(unsigned int value, unsigned int mask)
{
    (void)value; (void)mask;
    return 0x0009001f;
}

unsigned int PORT_MS_ABI _clearfp(void)
{
    return 0;
}

// MSVC's rand: per-process state, 15 bits.
static unsigned int port_rand_state = 1;

void PORT_MS_ABI srand(unsigned int seed)
{
    port_rand_state = seed;
}

int PORT_MS_ABI rand(void)
{
    port_rand_state = port_rand_state * 214013u + 2531011u;
    return (int)((port_rand_state >> 16) & 0x7fff);
}

// Bounds-checked copies: 0 on success, an errno value (and an empty destination) on failure.
int PORT_MS_ABI memcpy_s(void *destination, size_t size, const void *source, size_t count)
{
    if (count == 0)
        return 0;
    if (destination == 0 || source == 0 || size < count) {
        if (destination != 0)
            memset(destination, 0, size);
        return destination == 0 || source == 0 ? EINVAL : ERANGE;
    }
    memcpy(destination, source, count);
    return 0;
}

int PORT_MS_ABI memmove_s(void *destination, size_t size, const void *source, size_t count)
{
    if (count == 0)
        return 0;
    if (destination == 0 || source == 0 || size < count)
        return destination == 0 || source == 0 ? EINVAL : ERANGE;
    memmove(destination, source, count);
    return 0;
}

int PORT_MS_ABI strcpy_s(char *destination, size_t size, const char *source)
{
    if (destination == 0 || size == 0)
        return EINVAL;
    if (source == 0) {
        destination[0] = 0;
        return EINVAL;
    }
    size_t length = strlen(source);
    if (length >= size) {
        destination[0] = 0;
        return ERANGE;
    }
    memcpy(destination, source, length + 1);
    return 0;
}

// printf formatting (vsprintf, _vsnprintf, vsprintf_s, _vsprintf_s_l): ms_printf_linux.c.

// 64-bit time. MSVC's struct tm is glibc's first nine ints; glibc's has two more fields, so results are copied out
// field by field into the caller's nine.
typedef struct port_ms_tm {
    int tm_sec, tm_min, tm_hour, tm_mday, tm_mon, tm_year, tm_wday, tm_yday, tm_isdst;
} port_ms_tm;

static __thread port_ms_tm port_local_tm;
static __thread char port_ctime_text[32];

// Not time(): the game defines its own time() on top of _time64 (Havok's hkSolverTimeDate), and the executable's
// definition wins over glibc's.
long long PORT_MS_ABI _time64(long long *result)
{
    struct timespec clock;
    clock_gettime(CLOCK_REALTIME, &clock);
    long long now = (long long)clock.tv_sec;
    if (result != 0)
        *result = now;
    return now;
}

int PORT_MS_ABI _localtime64_s(port_ms_tm *result, const long long *when)
{
    struct tm host;
    time_t seconds = (time_t)*when;
    if (result == 0 || when == 0 || localtime_r(&seconds, &host) == 0)
        return EINVAL;
    result->tm_sec = host.tm_sec;
    result->tm_min = host.tm_min;
    result->tm_hour = host.tm_hour;
    result->tm_mday = host.tm_mday;
    result->tm_mon = host.tm_mon;
    result->tm_year = host.tm_year;
    result->tm_wday = host.tm_wday;
    result->tm_yday = host.tm_yday;
    result->tm_isdst = host.tm_isdst;
    return 0;
}

port_ms_tm *PORT_MS_ABI _localtime64(const long long *when)
{
    return _localtime64_s(&port_local_tm, when) == 0 ? &port_local_tm : 0;
}

char *PORT_MS_ABI _ctime64(const long long *when)
{
    time_t seconds = (time_t)*when;
    return ctime_r(&seconds, port_ctime_text);
}

// 16-bit wide strings. Use asm labels on names that clang recognizes as C library builtins
// (wcslen/wcschr/wcscmp/wcsncmp) so clang does not ignore ms_abi on builtin functions.
size_t PORT_MS_ABI port_ms_wcslen(const port_wchar *text) __asm__("wcslen");
size_t PORT_MS_ABI port_ms_wcslen(const port_wchar *text)
{
    size_t length = 0;
    while (text[length] != 0)
        length++;
    return length;
}

port_wchar *PORT_MS_ABI port_ms_wcschr(const port_wchar *text, port_wchar character) __asm__("wcschr");
port_wchar *PORT_MS_ABI port_ms_wcschr(const port_wchar *text, port_wchar character)
{
    for (;; text++) {
        if (*text == character)
            return (port_wchar *)text;
        if (*text == 0)
            return 0;
    }
}

int PORT_MS_ABI port_ms_wcscmp(const port_wchar *first, const port_wchar *second) __asm__("wcscmp");
int PORT_MS_ABI port_ms_wcscmp(const port_wchar *first, const port_wchar *second)
{
    while (*first != 0 && *first == *second)
        first++, second++;
    return (int)*first - (int)*second;
}

int PORT_MS_ABI port_ms_wcsncmp(const port_wchar *first, const port_wchar *second, size_t count) __asm__("wcsncmp");
int PORT_MS_ABI port_ms_wcsncmp(const port_wchar *first, const port_wchar *second, size_t count)
{
    for (; count > 0; count--, first++, second++) {
        if (*first != *second || *first == 0)
            return (int)*first - (int)*second;
    }
    return 0;
}

port_wchar *PORT_MS_ABI wcsstr(const port_wchar *text, const port_wchar *pattern)
{
    if (*pattern == 0)
        return (port_wchar *)text;
    for (; *text != 0; text++) {
        size_t i = 0;
        while (pattern[i] != 0 && text[i] == pattern[i])
            i++;
        if (pattern[i] == 0)
            return (port_wchar *)text;
    }
    return 0;
}

static port_wchar port_wlower(port_wchar c)
{
    return c >= 'A' && c <= 'Z' ? (port_wchar)(c + ('a' - 'A')) : c;
}

int PORT_MS_ABI _wcsicmp(const port_wchar *first, const port_wchar *second)
{
    while (*first != 0 && port_wlower(*first) == port_wlower(*second))
        first++, second++;
    return (int)port_wlower(*first) - (int)port_wlower(*second);
}

int PORT_MS_ABI _wcsnicmp(const port_wchar *first, const port_wchar *second, size_t count)
{
    for (; count > 0; count--, first++, second++) {
        if (port_wlower(*first) != port_wlower(*second) || *first == 0)
            return (int)port_wlower(*first) - (int)port_wlower(*second);
    }
    return 0;
}

// wcstol / wcstoul: the digits are ASCII, so narrow them and use the narrow parser.
static unsigned long long port_wide_to_integer(const port_wchar *text, port_wchar **end, int base, int is_signed)
{
    char narrow[64];
    size_t length = 0;
    while (length + 1 < sizeof(narrow) && text[length] != 0 && text[length] < 0x80) {
        narrow[length] = (char)text[length];
        length++;
    }
    narrow[length] = 0;
    char *narrow_end;
    unsigned long long value = is_signed ? (unsigned long long)strtol(narrow, &narrow_end, base)
                                         : (unsigned long long)strtoul(narrow, &narrow_end, base);
    if (end != 0)
        *end = (port_wchar *)text + (narrow_end - narrow);
    return value;
}

long PORT_MS_ABI wcstol(const port_wchar *text, port_wchar **end, int base)
{
    return (long)port_wide_to_integer(text, end, base, 1);
}

unsigned long PORT_MS_ABI wcstoul(const port_wchar *text, port_wchar **end, int base)
{
    return (unsigned long)port_wide_to_integer(text, end, base, 0);
}
