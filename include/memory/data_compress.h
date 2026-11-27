#ifndef MEMORY_DATA_COMPRESS_H
#define MEMORY_DATA_COMPRESS_H

#include "memory/zlib_compression_level_types.h"

// Functions defined in tu/blamlibxbox/data_compress.cpp that other units call.

struct s_blam_zlib_memory;
struct s_online_files_encrypted_writer;
struct z_stream_s;

extern "C" unsigned char data_compress_encrypted_writer_finish(s_online_files_encrypted_writer *writer);

bool agnostic_stream_write(bool deflate, struct s_blam_zlib_memory * memory, struct z_stream_s * stream, char * next_in,
    long avail_in, long * bytes_consumed, char * next_out, long avail_out, long * bytes_written, bool finish, bool * done);
bool agnostic_stream_start(bool deflate, struct s_blam_zlib_memory *zlib_memory_pool_storage, char *buffer, long buffer_size,
    struct z_stream_s *stream, enum e_zlib_compression_level compression_level);
bool runtime_data_compress(const void *buffer, unsigned long buffer_size, void *destination, unsigned long *destination_size,
    unsigned long capacity, e_zlib_compression_level level, unsigned long scratch_size, void *scratch_buffer);

#endif // MEMORY_DATA_COMPRESS_H
