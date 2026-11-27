#ifndef CACHE_ASYNC_IO_FILE_TYPES_H
#define CACHE_ASYNC_IO_FILE_TYPES_H

struct s_async_io_file {
    void *handle;
    unsigned int outstanding_io_count;
    int reserved8;
};

#endif // CACHE_ASYNC_IO_FILE_TYPES_H
