// blamlibxbox/data_compress.obj: from the records 0x1a7d48 0x1a7dc8 0x1a7ea8 0x1ebe20 0x1ebf98 0x1ec090 0x2fedc0
// @flags /TP /O2 /Oc
// @address 0x70dd34 XeCryptAesKey
// @address 0x70dd44 XeCryptAesCbc
#include <string.h>

#include "zlib/blam_zlib_memory.h"
#include "zlib/blam_zlib_memory_types.h"
#include "xbox/xecrypt.h"
#include "memory/memory_pool.h"
#include "memory/memory_pool_types.h"
#include "zlib/zlib.h"
#include "memory/data_compress.h"
#include "memory/zlib_compression_level_types.h"
#include "memory/data.h"
#include "saved_games/buffered_file_writer_types.h"
#include "saved_games/saved_game_files_io.h"
#include "cseries/minidump_writer.h"
#include "shell/shell_havok_memory.h"
#include "tag_files/file_reference.h"
#include "memory/data_compress_encrypted_writer_types.h"

struct s_data_compress_file_writer
{
    virtual void create(const char *path, void *context) = 0;
    virtual bool finish() = 0;
    virtual bool is_open() = 0;
    bool open;
    long m_count;
    long m_size;
    s_file_reference file;
    s_buffered_file_writer buffered_writer;
    char buffer[4096];
    s_online_files_encrypted_writer encrypted_writer;
};

typedef char data_compress_writer_file_offset[(sizeof(void *) != 4 || ((offsetof(s_data_compress_file_writer, file) == 0x10))) ? 1 : -1];
typedef char data_compress_writer_buffered_offset[(sizeof(void *) != 4 || ((offsetof(s_data_compress_file_writer, buffered_writer) == 0x120))) ? 1 : -1];
typedef char data_compress_writer_encrypted_offset[(sizeof(void *) != 4 || ((offsetof(s_data_compress_file_writer, encrypted_writer) == 0x1130))) ? 1 : -1];


static unsigned char data_compress_output_buffer[4096], data_compress_deflate_buffer[4096], data_compress_uncompressed_buffer[4096];

static __forceinline void *data_compress_secure_zero_memory(void *pointer, unsigned long count)
{
    volatile unsigned char *destination = (unsigned char *)pointer;
    while (count)
    {
        *destination = 0;
        destination++;
        count--;
    }
    return pointer;
}

// @address 0x1a7d48 data_compress_encrypted_writer_dispose
extern "C" void data_compress_encrypted_writer_dispose(s_online_files_encrypted_writer *writer)
{
    data_compress_secure_zero_memory(data_compress_uncompressed_buffer, sizeof(data_compress_uncompressed_buffer));
    data_compress_secure_zero_memory(data_compress_deflate_buffer, sizeof(data_compress_deflate_buffer));
    data_compress_secure_zero_memory(data_compress_output_buffer, sizeof(data_compress_output_buffer));
    data_compress_secure_zero_memory(writer->key, sizeof(writer->key));
}

// @address 0x1a7dc8 data_compress_encrypted_writer_initialize
extern "C" unsigned char data_compress_encrypted_writer_initialize(s_online_files_encrypted_writer *writer)
{
    memset(&writer->stream, 0, sizeof(writer->stream));
    memset(&writer->zlib_memory, 0, sizeof(writer->zlib_memory));
    memset(writer->feedback, 0, sizeof(writer->feedback));
    memset(writer->aes_state, 0, sizeof(writer->aes_state));
    writer->pending_size = 0;
    writer->compressed_size = 0;
    writer->uncompressed_size = 0;
    writer->written_size = 0;

    memset(data_compress_uncompressed_buffer, 0, sizeof(data_compress_uncompressed_buffer));
    memset(data_compress_deflate_buffer, 0, sizeof(data_compress_deflate_buffer));
    memset(data_compress_output_buffer, 0, sizeof(data_compress_output_buffer));
    bool success = agnostic_stream_start(true, &writer->zlib_memory, (char *)g_shell_havok_memory, 0x4b000, &writer->stream, _zlib_compression_level_fast);
    XeCryptAesKey(writer->aes_state, writer->key);
    return success;
}



static __forceinline bool data_compress_encrypted_writer_flush(s_online_files_encrypted_writer *writer, long size)
{
    XeCryptAesCbc(writer->aes_state, data_compress_output_buffer, size, data_compress_output_buffer, writer->feedback, true);
    bool success = buffered_file_write(writer->writer, data_compress_output_buffer, size);
    writer->pending_size = 0;
    writer->written_size += size;
    return success;
}

// @address 0x1a7ea8 data_compress_encrypted_writer_write
extern "C" unsigned char data_compress_encrypted_writer_write(s_online_files_encrypted_writer *writer, const void *data, unsigned long size, bool finish)
{
    bool done = false;
    unsigned char success = true;
    unsigned long offset = 0;
    while (size > offset || (finish && !done))
    {
        long chunk = size - offset;
        if (chunk >= 4096)
            chunk = 4096;
        long consumed = 0;
        const char *source = (const char *)data + offset;
        long written = 0;
        if (chunk > 0)
        {
            if (!minidump_test_memory_readable(source, chunk))
            {
                if (source + chunk > (const char *)data_compress_uncompressed_buffer &&
                    (const char *)data_compress_uncompressed_buffer + sizeof(data_compress_uncompressed_buffer) > source)
                    memmove(data_compress_uncompressed_buffer, source, chunk);
                else
                    memcpy(data_compress_uncompressed_buffer, source, chunk);
            }
            else
            {
                memset(data_compress_uncompressed_buffer, 0xfc, chunk);
            }
        }
        success = agnostic_stream_write(true, &writer->zlib_memory, &writer->stream, (char *)data_compress_uncompressed_buffer, chunk, &consumed,
            (char *)data_compress_deflate_buffer, sizeof(data_compress_deflate_buffer), &written, finish, &done);
        if (success && written > 0)
        {
            unsigned long pending = writer->pending_size;
            long copy_size = sizeof(data_compress_output_buffer) - pending;
            if (written <= copy_size)
                copy_size = written;
            long remaining = written - copy_size;
            writer->uncompressed_size += written;
            if (copy_size > 0)
            {
                memcpy(data_compress_output_buffer + pending, data_compress_deflate_buffer, copy_size);
                writer->pending_size += copy_size;
            }
            if (writer->pending_size == sizeof(data_compress_output_buffer))
            {
                success = data_compress_encrypted_writer_flush(writer, sizeof(data_compress_output_buffer));
            }
            if (remaining > 0)
            {
                memcpy(data_compress_output_buffer, data_compress_deflate_buffer + copy_size, remaining);
                writer->pending_size += remaining;
            }
        }
        offset += consumed;
        if (!success)
            break;
    }
    if (success)
        writer->compressed_size += size;
    return success;
}

extern "C" unsigned char data_compress_encrypted_writer_finish(s_online_files_encrypted_writer *writer)
{
    unsigned char success = data_compress_encrypted_writer_write(writer, 0, 0, true);
    long pending_size = writer->pending_size;
    if (pending_size > 0)
    {
        long size = pending_size;
        if ((pending_size & 15) != 0)
            size = (pending_size | 15) + 1;
        XeCryptAesCbc(writer->aes_state, data_compress_output_buffer, size, data_compress_output_buffer, writer->feedback, true);
        success = buffered_file_write(writer->writer, data_compress_output_buffer, size);
        writer->written_size += size;
    }
    z_deflateEnd(&writer->stream);
    s_blam_zlib_memory *memory = &writer->zlib_memory;
    memory->heap = 0;
    return success;
}

// @address 0x1ebf98 ?agnostic_stream_start@@YA_N_NPAUs_blam_zlib_memory@@PADJPAUz_stream_s@@W4e_zlib_compression_level@@@Z
bool agnostic_stream_start(bool deflate, struct s_blam_zlib_memory *zlib_memory_pool_storage, char *buffer, long buffer_size,
    struct z_stream_s *stream, enum e_zlib_compression_level compression_level)
{
    bool success = false;
    s_memory_pool *zlib_pool = fixed_memory_pool_new("zlib pool", buffer, buffer_size, _memory_pool_callback_list_none);

    memset(stream, 0, sizeof(*stream));

    if (zlib_pool)
    {
        int err = Z_OK;

        blam_zlib_initialize(zlib_pool, zlib_memory_pool_storage);

        stream->zalloc = blam_zlib_malloc;
        stream->zfree = blam_zlib_free;
        stream->opaque = zlib_memory_pool_storage;

        err = deflate
            ? z_deflateInit2_(stream, compression_level, Z_DEFLATED, MAX_WBITS, 8, Z_DEFAULT_STRATEGY, ZLIB_VERSION, sizeof(*stream))
            : z_inflateInit2_(stream, MAX_WBITS, ZLIB_VERSION, sizeof(*stream));
        if (err == Z_OK)
        {
            success = true;
        }
        else
        {
            s_memory_pool *heap = zlib_memory_pool_storage->heap;
            blam_zlib_dispose(zlib_memory_pool_storage);
            fixed_memory_pool_free(heap);
        }
    }

    return success;
}

// @address 0x1ec090 ?agnostic_stream_write@@YA_N_NPAUs_blam_zlib_memory@@PAUz_stream_s@@PADJPAJ3J40PA_N@Z
bool agnostic_stream_write(bool deflate, struct s_blam_zlib_memory *memory, struct z_stream_s *stream, char *next_in, long avail_in,
    // match: 0x1ec090: pointer __restrict qualifier prevents alias reload
    long *__restrict bytes_consumed, char *next_out, long avail_out, long *bytes_written, bool finish, bool *done)
{
    bool success = false;
    int err = Z_OK;

    stream->next_in = (Bytef *)next_in;
    stream->avail_in = avail_in;
    stream->next_out = (Bytef *)next_out;
    stream->avail_out = avail_out;

    err = deflate ? z_deflate(stream, finish ? Z_FINISH : Z_NO_FLUSH) : z_inflate(stream, Z_NO_FLUSH);

    if (err == Z_OK || err == Z_STREAM_END)
    {
        *bytes_consumed = avail_in - stream->avail_in;
        *bytes_written = avail_out - stream->avail_out;
        if (finish && err == Z_STREAM_END)
        {
            success = true;
            *done = true;
        }
        else
        {
            success = true;
        }
    }

    return success;
}


// @address 0x2fedc0 data_compress_file_writer_write
extern "C" bool data_compress_file_writer_write(s_data_compress_file_writer *writer, const void *data, unsigned long size)
{
    bool success = false;
    if (data_compress_encrypted_writer_write(&writer->encrypted_writer, data, size, false)) {
        success = true;
        writer->m_size += size;
    }
    if (!success)
        writer->finish();
    return success;
}

// @address 0x1ebe20 ?runtime_data_compress@@YA_NPBXKPAXPAKKW4e_zlib_compression_level@@K1@Z
bool runtime_data_compress(const void *buffer, unsigned long buffer_size, void *destination, unsigned long *destination_size,
    unsigned long capacity, e_zlib_compression_level level, unsigned long scratch_size, void *scratch_buffer)
{
    struct data_compress_runtime_header { unsigned long uncompressed_size; unsigned char compressed_data[1]; };
    bool success = false;
    if (capacity >= 4)
    {
        s_memory_pool *pool = fixed_memory_pool_new("zlib pool", scratch_buffer, (long)scratch_size, _memory_pool_callback_list_none);
        if (pool)
        {
            s_blam_zlib_memory memory;
            blam_zlib_initialize(pool, &memory);
            data_compress_runtime_header *header = (data_compress_runtime_header *)destination;
            header->uncompressed_size = buffer_size;
            *destination_size = capacity - 4;
            z_stream stream;
            stream.next_in = (Bytef *)buffer;
            stream.avail_in = buffer_size;
            stream.next_out = header->compressed_data;
            stream.avail_out = *destination_size;
            int error = compress3(destination_size, level, &memory, &stream);
            if (error == Z_OK)
            {
                *destination_size += 4;
                success = true;
            }
            blam_zlib_dispose(&memory);
            fixed_memory_pool_free(pool);
        }
    }
    if (scratch_buffer == 0)
        return success;
    return success;
}
