#ifndef CACHE_TAG_RESOURCE_PAGING_TASK_H
#define CACHE_TAG_RESOURCE_PAGING_TASK_H

#include <stddef.h>
#include "memory/basic_buffer.h"
#include "cache/tag_resource_paging_decoder.h"

#ifdef HALO3_64BIT
typedef c_basic_buffer<void> t_tag_resource_paging_buffer_param;
#else
typedef unsigned __int64 t_tag_resource_paging_buffer_param;
#endif

class c_tag_resource_paging_receiver
{
public:
    virtual void handle(t_tag_resource_paging_buffer_param buffer) = 0;
};

// 0x2af730, 0x2af618. The fifth operation constructs the context's object.
class c_tag_resource_paging_task
{
public:
    virtual int is_receiver_null();
    virtual c_tag_resource_paging_receiver *process();
    virtual c_tag_resource_paging_receiver *process_and_handle(t_tag_resource_paging_buffer_param buffer);
    virtual void reset_receiver(int value, int slot, int argument);
    virtual void initialize_context(void *context) = 0;

#ifdef HALO3_64BIT
    __declspec(align(8)) char context[48];
#else
    __declspec(align(8)) char context[24];
#endif
    c_tag_resource_paging_receiver *receiver;
};

class c_tag_resource_paging_secondary_task
{
public:
    virtual int is_receiver_null();
    virtual s_saved_film_stream_compressor *process();
    virtual s_saved_film_stream_compressor *process_and_handle(t_tag_resource_paging_buffer_param buffer);
    virtual void reset_receiver(int value, int slot, int argument);
    virtual void initialize_context(void *context);

    __declspec(align(8)) s_tag_resource_paging_owner context;
};

typedef char paging_stream_factory_base_size[(sizeof(void *) != 4 || sizeof(c_tag_resource_paging_task) == 0x28) ? 1 : -1];
typedef char paging_task_context_offset[(offsetof(c_tag_resource_paging_task, context) == 8) ? 1 : -1];
typedef char paging_task_receiver_offset[(sizeof(void *) != 4 || offsetof(c_tag_resource_paging_task, receiver) == 0x20) ? 1 : -1];
typedef char paging_secondary_task_context_offset[(offsetof(c_tag_resource_paging_secondary_task, context) == 8) ? 1 : -1];
typedef char paging_secondary_task_receiver_offset[
    (sizeof(void *) != 4 || offsetof(c_tag_resource_paging_secondary_task, context) + offsetof(s_tag_resource_paging_owner, decoder) == 0x9858) ? 1 : -1];

#endif
