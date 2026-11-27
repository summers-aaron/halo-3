#ifndef INCLUDE_CSERIES_ASYNC_FILE_WRITE_H
#define INCLUDE_CSERIES_ASYNC_FILE_WRITE_H

class c_synchronized_long;

extern "C" long async_write_buffer_to_file(const char *path, const void *buffer, unsigned long size, long category, long priority,
    void *unused, c_synchronized_long *completion, void *done);

#endif // INCLUDE_CSERIES_ASYNC_FILE_WRITE_H
