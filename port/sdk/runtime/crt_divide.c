// port/sdk/runtime: the MSVC 64-bit division helpers. Compiled code calls them for / and % on 64-bit integers:
// both operands on the stack, the helper pops them (stdcall), the result in edx:eax. Their linker names are fixed:
// __alldiv in COFF (the C name _alldiv with COFF's underscore) and _alldiv in ELF, where the MS-ABI game objects
// name it without the prefix; the asm labels give exactly that name, with no stdcall decoration.
#include "port_runtime.h"

#if defined(__i386__)
#define PORT_CRT_STDCALL __attribute__((stdcall))
#else
#define PORT_CRT_STDCALL
#endif

#ifdef _WIN32
#define PORT_CRT_HELPER(name) __asm__("_" #name)
#else
#define PORT_CRT_HELPER(name) __asm__(#name)
#endif

// Shift-and-subtract: it must not use 64-bit division itself.
static unsigned long long port_udivmod(unsigned long long dividend, unsigned long long divisor,
    unsigned long long *remainder)
{
    unsigned long long quotient = 0, rest = 0;
    if (divisor == 0) {
        port_write_error("port: 64-bit division by zero\n");
        port_exit(4);
    }
    for (int bit = 63; bit >= 0; bit--) {
        rest = (rest << 1) | ((dividend >> bit) & 1);
        if (rest >= divisor) {
            rest -= divisor;
            quotient |= 1ull << bit;
        }
    }
    if (remainder != 0)
        *remainder = rest;
    return quotient;
}

static unsigned long long port_magnitude(long long value)
{
    return value < 0 ? 0 - (unsigned long long)value : (unsigned long long)value;
}

PORT_CRT_STDCALL unsigned long long port_aulldiv(unsigned long long dividend, unsigned long long divisor)
    PORT_CRT_HELPER(_aulldiv);
PORT_CRT_STDCALL unsigned long long port_aullrem(unsigned long long dividend, unsigned long long divisor)
    PORT_CRT_HELPER(_aullrem);
PORT_CRT_STDCALL long long port_alldiv(long long dividend, long long divisor) PORT_CRT_HELPER(_alldiv);
PORT_CRT_STDCALL long long port_allrem(long long dividend, long long divisor) PORT_CRT_HELPER(_allrem);

PORT_CRT_STDCALL unsigned long long port_aulldiv(unsigned long long dividend, unsigned long long divisor)
{
    return port_udivmod(dividend, divisor, 0);
}

PORT_CRT_STDCALL unsigned long long port_aullrem(unsigned long long dividend, unsigned long long divisor)
{
    unsigned long long remainder;
    port_udivmod(dividend, divisor, &remainder);
    return remainder;
}

PORT_CRT_STDCALL long long port_alldiv(long long dividend, long long divisor)
{
    unsigned long long quotient = port_udivmod(port_magnitude(dividend), port_magnitude(divisor), 0);
    return (dividend < 0) != (divisor < 0) ? (long long)(0 - quotient) : (long long)quotient;
}

PORT_CRT_STDCALL long long port_allrem(long long dividend, long long divisor)
{
    unsigned long long remainder;
    port_udivmod(port_magnitude(dividend), port_magnitude(divisor), &remainder);
    return dividend < 0 ? (long long)(0 - remainder) : (long long)remainder;
}

#ifdef _WIN32
// libgcc's names for the unsigned 64-bit helpers (cdecl), which MinGW's import library code calls (libmsvcrt.a's
// _time64 shim): the same arithmetic.
unsigned long long port_udivdi3(unsigned long long dividend, unsigned long long divisor) __asm__("___udivdi3");
unsigned long long port_umoddi3(unsigned long long dividend, unsigned long long divisor) __asm__("___umoddi3");
unsigned long long port_udivdi3(unsigned long long dividend, unsigned long long divisor)
{
    return port_udivmod(dividend, divisor, 0);
}
unsigned long long port_umoddi3(unsigned long long dividend, unsigned long long divisor)
{
    unsigned long long remainder;
    port_udivmod(dividend, divisor, &remainder);
    return remainder;
}
#endif
