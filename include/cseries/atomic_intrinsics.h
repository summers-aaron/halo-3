#ifndef CSERIES_ATOMIC_INTRINSICS_H
#define CSERIES_ATOMIC_INTRINSICS_H



extern "C" long _InterlockedDecrement(long volatile *);
extern "C" long _InterlockedExchange(long volatile *, long);
extern "C" long _InterlockedExchange(volatile long *, long);

#endif // CSERIES_ATOMIC_INTRINSICS_H
