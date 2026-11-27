#ifndef CSERIES_CREATE_FILE_TASK_TYPES_H
#define CSERIES_CREATE_FILE_TASK_TYPES_H

#include "cseries/file_handle_types.h"
#include "memory/data_types.h"

struct s_create_file_task
{
    char file_name[256];
    unsigned long access;
    unsigned long share_mode;
    unsigned long disposition;
    unsigned long flags;
    s_file_handle volatile *file;
    bool create_directories;
    // Remainder of the task payload copied by async_task_add_ex (0x120 bytes on 32-bit, 512 bytes on 64-bit).
#ifdef HALO3_64BIT
    char task_payload_storage[231];
#else
    char task_payload_storage[11];
#endif
};

#endif // CSERIES_CREATE_FILE_TASK_TYPES_H
