#ifndef INTERFACE_FILE_SHARE_ENUMERATION_TASK_TYPES_H
#define INTERFACE_FILE_SHARE_ENUMERATION_TASK_TYPES_H

#include <xtl.h>
#include "cseries/async_xoverlapped_types.h"
#include "shell/shell.h"

// Embedded at 0x2074 in the selected-item screen, ending at keyboard 0x6d18.
struct s_file_share_enumeration_task : c_overlapped_task
{
    virtual const char *get_context_string() const;
    virtual unsigned long start(void *overlapped);
    e_controller_index controller;
    long reserved;
    XONLINE_FRIEND records[100];
    unsigned long count;
};

typedef char file_share_enumeration_task_size[(sizeof(void *) != 4 || sizeof(s_file_share_enumeration_task) == 0x4ca4) ? 1 : -1];

#endif
