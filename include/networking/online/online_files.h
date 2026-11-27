#ifndef NETWORKING_ONLINE_ONLINE_FILES_H
#define NETWORKING_ONLINE_ONLINE_FILES_H

#include "shell/shell.h"

// Functions defined in tu/networkinglibxbox/online_files.cpp.

struct s_buffered_file_writer;
struct s_netdebug_upload_task;

extern "C" bool buffered_file_writer_flush(s_buffered_file_writer *writer);
extern "C" int netdebug_upload_file(s_netdebug_upload_task *task);
extern "C" void online_file_manager_delete_transfer(void *p0, const char *p1);

struct s_file_reference;
struct s_queued_work_item;
struct s_queued_work_item_status;
class c_minidump_file_output_secure;

extern "C" unsigned char online_files_write_archive_header(c_minidump_file_output_secure *object, long value);
extern "C" unsigned char online_files_writer_begin_file(c_minidump_file_output_secure *object);
extern "C" unsigned char online_files_writer_end_file(c_minidump_file_output_secure *object);
bool read_queued_file(s_file_reference *reference, s_queued_work_item *item);
bool write_queued_file(s_file_reference *reference, const s_queued_work_item *item);
bool get_work_item_status_from_queued_file(s_file_reference *reference, s_queued_work_item_status *status, char *const filename, bool *found);
extern "C" unsigned char online_files_is_catalogue_ready(void);
int work_item_tracker_sort_proc(const void *a, const void *b);
enum e_controller_index get_controller_index_from_xuid(unsigned __int64 xuid);

class c_online_file_manager;

// Data defined in tu/networkinglibxbox/online_files.cpp that other units use.
extern c_online_file_manager g_online_file_manager;

struct s_online_files_writer_statistics;
struct s_online_files_encrypted_writer;
extern "C" s_online_files_writer_statistics g_online_files_writer_statistics[];
extern "C" unsigned char data_compress_encrypted_writer_write(s_online_files_encrypted_writer *writer, const void *data, unsigned long size, bool finish);
extern "C" unsigned char data_compress_encrypted_writer_initialize(s_online_files_encrypted_writer *writer);
extern "C" void data_compress_encrypted_writer_dispose(s_online_files_encrypted_writer *writer);

enum e_map_memory_configuration;
void online_files_memory_initialize(e_map_memory_configuration configuration);
extern "C" void online_files_memory_dispose(void);

#endif // NETWORKING_ONLINE_ONLINE_FILES_H
