#ifndef TAG_FILES_FILE_ENUMERATION_TYPES_H
#define TAG_FILES_FILE_ENUMERATION_TYPES_H

#include <stddef.h>

struct s_find_file_data
{
    unsigned long flags;
    short depth;
    unsigned short location;
    char path[256];
    char search_spec[256];
    void *handles[16];

    // Storage for the native WIN32_FIND_DATAA output of the file APIs.
    __declspec(align(4)) unsigned char native_find_data[0x140];
};

typedef char find_file_data_native_offset[(sizeof(void *) != 4 || offsetof(s_find_file_data, native_find_data) == 0x248) ? 1 : -1];
typedef char find_file_data_size[(sizeof(void *) != 4 || sizeof(s_find_file_data) == 0x388) ? 1 : -1];

#endif
