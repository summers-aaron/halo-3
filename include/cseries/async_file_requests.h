#ifndef CSERIES_ASYNC_FILE_REQUESTS_H
#define CSERIES_ASYNC_FILE_REQUESTS_H

#include "cseries/async_types.h"
#include "tag_files/file_open_disposition_types.h"
struct s_file_handle;
struct s_create_file_task;
struct s_async_task;
class c_synchronized_long;
void async_create_file(const char *path, unsigned long access, e_file_open_disposition disposition, unsigned long attributes,
    e_async_category category, e_async_priority priority, s_file_handle *handle, s_create_file_task *task);
extern "C" long async_read_header(void *handle, void *buffer, long size, unsigned long offset, long category, long priority,
    c_synchronized_long *progress, c_synchronized_long *done, bool unbuffered);
extern "C" e_async_completion async_copy_position_callback(s_async_task *task);
extern "C" e_async_completion async_write_header_callback(s_async_task *task);

#endif // CSERIES_ASYNC_FILE_REQUESTS_H
