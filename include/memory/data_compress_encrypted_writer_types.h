#ifndef MEMORY_DATA_COMPRESS_ENCRYPTED_WRITER_TYPES_H
#define MEMORY_DATA_COMPRESS_ENCRYPTED_WRITER_TYPES_H
#include <stddef.h>
#include "saved_games/buffered_file_writer_types.h"
#include "zlib/zlib.h"
#include "zlib/blam_zlib_memory_types.h"

// Shared by the compression owner and the online archive writer.
struct s_online_files_encrypted_writer
{
    unsigned char key[16];
    s_buffered_file_writer *writer;
    z_stream_s stream;
    s_blam_zlib_memory zlib_memory;
    unsigned char feedback[16];
    char aes_state[352];
    unsigned long pending_size;
    long compressed_size;
    long uncompressed_size;
    long written_size;
};

typedef char encrypted_writer_writer_offset[(offsetof(s_online_files_encrypted_writer, writer) == 0x10) ? 1 : -1];
typedef char encrypted_writer_pending_offset[(sizeof(void *) != 4 || ((offsetof(s_online_files_encrypted_writer, pending_size) == 0x1c0))) ? 1 : -1];
typedef char encrypted_writer_size[(sizeof(void *) != 4 || ((sizeof(s_online_files_encrypted_writer) == 0x1d0))) ? 1 : -1];
#endif
