#ifndef NETWORKING_LOGIC_STORAGE_NETWORK_STORAGE_BITMAP_LOAD_TASK_TYPES_H
#define NETWORKING_LOGIC_STORAGE_NETWORK_STORAGE_BITMAP_LOAD_TASK_TYPES_H

class c_synchronized_long;

struct s_network_storage_bitmap_load_task
{
    const void *image_data;
    unsigned long image_size;
    long bitmap_index;
    long aspect_index;
    c_synchronized_long *success;
};

#endif // NETWORKING_LOGIC_STORAGE_NETWORK_STORAGE_BITMAP_LOAD_TASK_TYPES_H
