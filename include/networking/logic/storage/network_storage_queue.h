#ifndef NETWORKING_LOGIC_STORAGE_NETWORK_STORAGE_QUEUE_H
#define NETWORKING_LOGIC_STORAGE_NETWORK_STORAGE_QUEUE_H

// Functions defined in tu/networkinglibxbox/network_storage_queue.cpp that other units call.

class c_network_storage_item_interface;
struct s_network_storage_file_header;

extern "C" const long k_network_storage_queue_limits[2];

void network_storage_queue_clear(void);
long network_storage_queue_download_item(c_network_storage_item_interface *item, s_network_storage_file_header *buffer, long buffer_size);
long network_storage_queue_upload_item(c_network_storage_item_interface *item, s_network_storage_file_header *buffer, long buffer_size);
void network_storage_queue_update(void);

enum e_network_storage_queue_task_result;
void network_storage_queue_dispose_member(long, e_network_storage_queue_task_result);

#endif // NETWORKING_LOGIC_STORAGE_NETWORK_STORAGE_QUEUE_H
