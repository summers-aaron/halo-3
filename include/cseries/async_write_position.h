#ifndef CSERIES_ASYNC_WRITE_POSITION_H
#define CSERIES_ASYNC_WRITE_POSITION_H

#include "memory/static_arrays.h"
#include "cseries/write_position_flags_types.h"

class c_synchronized_long;
typedef c_flags<e_write_position_flags, unsigned long, k_write_position_flags_count> c_write_position_flags;

long async_write_position_ex(void *handle, void *buffer, long size, unsigned long offset, c_write_position_flags flags, long category,
    long priority, bool unbuffered, c_synchronized_long *bytes_written, c_synchronized_long *done);

#endif // CSERIES_ASYNC_WRITE_POSITION_H
