#ifndef CACHE_SOUND_CACHE_FILE_READER_H
#define CACHE_SOUND_CACHE_FILE_READER_H

#include "cache/sound_cache_bulk_reader.h"
#include "cache/cache_file_bulk_reader.h"
#include "cseries/async_read_context.h"

// A reader over a span of an open file, used by gui_custom_bitmap_storage_queue_async_read.
// Its two OVERLAPPED contexts and completion records occupy the full 0x58 bytes.
class c_sound_cache_file_reader : public c_sound_cache_reader
{
public:
    __forceinline c_sound_cache_file_reader(void *handle, unsigned long base_offset, unsigned long total_size) :
        m_handle(handle), m_base_offset(base_offset), m_total_size(total_size)
    {
    }

    virtual bool start_read(c_basic_buffer<void> buffer, unsigned long offset, unsigned long *bytes_read, long request_index);
    virtual void update(unsigned long timeout_milliseconds);
    virtual bool read_complete(long request_index, unsigned long *bytes_read, bool *success);

private:
    void *m_handle;
    unsigned long m_base_offset;
    unsigned long m_total_size;
    s_async_read_context m_contexts[2];
    s_async_read_completion m_completions[2];
};

typedef char sound_cache_file_reader_size[(sizeof(void *) != 4 || ((sizeof(c_sound_cache_file_reader) == 0x58))) ? 1 : -1];

#endif // CACHE_SOUND_CACHE_FILE_READER_H
