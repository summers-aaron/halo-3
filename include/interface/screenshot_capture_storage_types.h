#ifndef INTERFACE_SCREENSHOT_CAPTURE_STORAGE_TYPES_H
#define INTERFACE_SCREENSHOT_CAPTURE_STORAGE_TYPES_H

#include "memory/data_types.h"
#include "multithreading/synchronized_value.h"

struct s_screenshot_capture_storage
{
    __forceinline s_screenshot_capture_storage() {}
    __forceinline ~s_screenshot_capture_storage() {}

    long *slot;
    long buffer;
    long texture_size;
    long slot_size;
    long aligned_texture_size;
    char *scratch_buffer;
    unsigned long scratch_size;
    void *secondary_buffer;
    long secondary_size;
    unsigned long data_size;
    bool valid;
    short row;
    short column;
    unsigned short width;
    short height;
    short rows_per_block;
    c_synchronized_long lock;
};

#endif // INTERFACE_SCREENSHOT_CAPTURE_STORAGE_TYPES_H
