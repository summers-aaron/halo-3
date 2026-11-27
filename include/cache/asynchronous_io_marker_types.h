#ifndef CACHE_ASYNCHRONOUS_IO_MARKER_TYPES_H
#define CACHE_ASYNCHRONOUS_IO_MARKER_TYPES_H

#include <ppcintrinsics.h>

// cache_file_bulk_reader.cpp and xbox_sound_cache.cpp agree on these fields.
class c_asynchronous_io_marker
{
public:
    __forceinline long get_io_status() const { long status = io_status; __lwsync(); return status; }
    __forceinline bool request_completed() const { return get_io_status() != 2; }
    __forceinline void set_io_status(long status) { io_status = status; }

#ifdef HALO3_64BIT
    unsigned __int64 internal[2];
#else
    long internal[2];
#endif
    unsigned long offset;
    unsigned long offset_high;
    void *event;
    unsigned int requested_size;
    volatile long io_status;
};

#if defined(_M_PPCBE) || defined(_XBOX)
typedef char asynchronous_io_marker_size[(sizeof(void *) != 4 || (sizeof(c_asynchronous_io_marker) == 28)) ? 1 : -1];
#endif

#endif // CACHE_ASYNCHRONOUS_IO_MARKER_TYPES_H
