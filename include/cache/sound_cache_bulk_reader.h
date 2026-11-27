#ifndef CACHE_SOUND_CACHE_BULK_READER_H
#define CACHE_SOUND_CACHE_BULK_READER_H

#include "cache/asynchronous_io_marker_types.h"
#include "memory/basic_buffer.h"
class c_asynchronous_io_arena;

// Both file and indirect readers supply these three virtual operations.
class c_sound_cache_reader {
public:
    virtual bool start_read(c_basic_buffer<void> buffer, unsigned long offset, unsigned long *bytes_read, long request_index) = 0;
    virtual void update(unsigned long timeout_milliseconds) = 0;
    virtual bool read_complete(long request_index, unsigned long *bytes_read, bool *success) = 0;
};

// The sound cache's reader over the asynchronous io arena.
class c_sound_cache_bulk_reader : public c_sound_cache_reader {
public:
    virtual bool start_read(c_basic_buffer<void> buffer, unsigned long offset, unsigned long *bytes_read, long request_index) = 0;
    virtual void update(unsigned long timeout_milliseconds);
    virtual bool read_complete(long request_index, unsigned long *bytes_read, bool *success) = 0;
protected:
    c_asynchronous_io_marker m_markers[2];
    unsigned long m_pending_requests;
    c_asynchronous_io_arena *m_io_arena;
};

#endif
