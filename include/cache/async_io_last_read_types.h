#ifndef CACHE_ASYNC_IO_LAST_READ_TYPES_H
#define CACHE_ASYNC_IO_LAST_READ_TYPES_H

struct s_async_io_last_read {
    long state;
    long reserved4;
    long file_index;
    unsigned long offset;
};

#endif // CACHE_ASYNC_IO_LAST_READ_TYPES_H
