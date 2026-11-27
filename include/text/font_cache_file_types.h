#ifndef TEXT_FONT_CACHE_FILE_TYPES_H
#define TEXT_FONT_CACHE_FILE_TYPES_H

#include <stddef.h>
#include "tag_files/file_reference.h"
#include "multithreading/synchronized_value.h"

#pragma pack(push, 1)
struct s_font_cache_file
{
    char name[32];
    long font_index;
    bool open;
    s_file_reference reference;
    unsigned char alignment[3];
    long task_id;
    c_synchronized_long started;
    c_synchronized_long reserved_140;
    c_synchronized_long done;
};
#pragma pack(pop)

typedef char font_cache_file_size[(sizeof(void *) != 4 || sizeof(s_font_cache_file) == 0x148) ? 1 : -1];

#endif
