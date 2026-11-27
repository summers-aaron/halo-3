#ifndef NETWORKING_LOGIC_STORAGE_NETWORK_STORAGE_ITEM_H
#define NETWORKING_LOGIC_STORAGE_NETWORK_STORAGE_ITEM_H

#include <stddef.h>
#include "shell/shell.h"
#include "networking/logic/storage/network_storage_queue.h"

class c_network_storage_item_interface;

enum e_network_storage_queue_task_result
{
    _network_storage_queue_task_result_value_2 = 2,
    _network_storage_queue_task_result_value_3 = 3,
    _network_storage_queue_task_result_value_4 = 4,
    _network_storage_queue_task_result_value_5 = 5,
    _network_storage_queue_task_result_value_6 = 6,
    _network_storage_queue_task_result_aborted = 13
};

struct s_network_storage_file_header
{
    unsigned long checksum;
};

// Incremented when notify_task_completed cannot build default data, before retrying.
extern "C" long g_network_storage_default_data_failures;

// March initialize (8266fef8) stores a definition pointer, not a vfptr.
struct s_network_storage_item_definition {
    const char *local_directory;
    const wchar_t *online_filename;
    void *reserved_08;
    void *reserved_0c;
    bool (*use_default_data_proc)(c_network_storage_item_interface *);
    bool (*upload_completed_proc)(c_network_storage_item_interface *);
    bool (*download_completed_proc)(c_network_storage_item_interface *);
};
class c_network_storage_item_interface {
public:
    s_network_storage_item_definition *m_definition;
    unsigned short flags;
    unsigned short flags2;
    short valid_state;
    short current_task;
    short previous_task;
    short task_result;
    e_controller_index value;
    unsigned __int64 wide;
    long m_queue_id;
    s_network_storage_file_header *buffer;
    long buffer_size;
    __forceinline bool valid() const { return (short)valid_state != 0; }
    __forceinline bool ready() const { return valid() && (flags & 2) != 0; }
    __forceinline short get_current_action() const { return current_task; }
    bool use_default_data_proc();
    short get_type(void) const { return valid_state; }
    long get_queue_id(void) const { return m_queue_id; }
    void abort_online_task(void)
    {
        if (get_type() != 0 && get_queue_id() != -1)
            network_storage_queue_dispose_member(get_queue_id(), _network_storage_queue_task_result_aborted);
    }
    void dispose_temp_buffer(long clear_value)
    {
        const long store_value = (long)clear_value;
        if (buffer != 0)
        {
            buffer = (s_network_storage_file_header *)(ptrdiff_t)store_value;
            buffer_size = store_value;
        }
    }
    bool begin_download(e_controller_index controller, unsigned __int64 identifier, long flags);
    bool begin_upload(e_controller_index controller, unsigned __int64 identifier, long flags);
    void notify_task_completed(enum e_network_storage_queue_task_result result);
};

#endif
