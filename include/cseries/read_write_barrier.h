#ifndef CSERIES_READ_WRITE_BARRIER_H
#define CSERIES_READ_WRITE_BARRIER_H

#if defined(_MSC_VER) && !defined(__clang__)
// Use the XDK CRT intrinsic declarations.
#include <../../Source/crt/intrin.h>
#endif
// Clang provides _ReadWriteBarrier as a builtin, and the XDK CRT intrin.h redeclares
// clang's interlocked builtins with conflicting types.

#endif
