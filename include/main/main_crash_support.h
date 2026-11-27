#ifndef MAIN_MAIN_CRASH_SUPPORT_H
#define MAIN_MAIN_CRASH_SUPPORT_H

struct s_main_crash_report_writer;
struct s_main_block_writer;
struct s_minidump_writer;

#include "cseries/progress.h"
extern "C" unsigned char buffered_file_write(s_main_crash_report_writer *writer, const void *data, long size);
extern "C" unsigned char buffered_file_writer_flush(s_main_crash_report_writer *writer);
#if !defined(_XBOX) || defined(__clang__)
extern "C" void minidump_writer_add_directory_entry(s_minidump_writer *writer, long rva, long data_size, long stream_type);
#else
extern "C" s_main_block_writer *minidump_writer_add_directory_entry(s_minidump_writer *writer, long rva, long data_size, long stream_type);
#endif
extern "C" bool main_write_uint32_value(s_main_block_writer *writer, unsigned long value);

extern "C" void progress_done(void);
extern "C" long progress_update_with_description(long current, long total);
#endif // MAIN_MAIN_CRASH_SUPPORT_H
