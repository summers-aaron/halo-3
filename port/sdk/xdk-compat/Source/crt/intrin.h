// Port override of the XDK's Source/crt/intrin.h. Game headers include it by the XDK-relative path
// <../../Source/crt/intrin.h>, which resolves here through port/sdk/xdk-compat/include/xbox.
// Two of its declarations clash with clang's builtins (clang declares the bit-test pair with a volatile pointer),
// so they are renamed while the XDK file is read; calls then reach the builtins.
#define _interlockedbittestandset port_xdk_interlockedbittestandset
#define _interlockedbittestandreset port_xdk_interlockedbittestandreset
#include PORT_XDK_CRT_INTRIN_H
#undef _interlockedbittestandset
#undef _interlockedbittestandreset
