// Port override of the XDK's stddef.h (no include guard of its own, so it re-applies after every inclusion).
// The XDK's offsetof takes a member's address through a null pointer. Clang keeps a null pointer null when it
// converts to a second base class, so members of a second base come out short (0x160 in the weapon and projectile
// checks). The layouts are right; the builtin gives the true offsets.
#include_next <stddef.h>
#undef offsetof
#define offsetof(s, m) __builtin_offsetof(s, m)
