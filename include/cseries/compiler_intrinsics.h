#ifndef CSERIES_COMPILER_INTRINSICS_H
#define CSERIES_COMPILER_INTRINSICS_H

extern "C" {
long _InterlockedExchange(volatile long *target, long value);
}

#endif // CSERIES_COMPILER_INTRINSICS_H
