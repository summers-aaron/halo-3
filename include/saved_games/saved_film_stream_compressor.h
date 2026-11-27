#ifndef SAVED_GAMES_SAVED_FILM_STREAM_COMPRESSOR_H
#define SAVED_GAMES_SAVED_FILM_STREAM_COMPRESSOR_H

#include <stddef.h>
#include <string.h>
#include "memory/basic_buffer.h"
#include "cache/tag_resource_paging_decoder.h"

struct s_saved_film_stream_compressor
{
#ifdef HALO3_64BIT
    virtual bool initialize(c_basic_buffer<void> buffer);
    virtual bool compress(c_basic_buffer<void> data, void *result);
#else
    virtual bool initialize(__int64 buffer);
    virtual bool compress(__int64 data, void *result);
#endif
    virtual bool finish(void *result);
#pragma pack(push, 4)
    union
    {
        struct
        {
            char *m_begin;
            long m_input_size;
            char *m_cursor;
            unsigned long m_available;
        };
        long input[4];
        __int64 input_packed[2];
    };
#pragma pack(pop)
    long pad14;
    union
    {
        s_tag_resource_paging_codec_stream m_stream;
        // Preserve the constructor's array initialization groups over the
        // codec's typed state, fiber and sliding window.
        struct
        {
            char codec_state[0x1818];
            long fiber[4];
            char codec_window[0x8008];
            long status;
        };
    };
#ifdef HALO3_64BIT
    __forceinline s_saved_film_stream_compressor()
    {
        m_begin = 0;
        m_input_size = 0;
        m_cursor = 0;
        m_available = 0;
        memset(&m_stream.fiber, 0, sizeof(m_stream.fiber));
        m_stream.status = 0;
    }
#else
    __forceinline s_saved_film_stream_compressor() : input(), fiber(), status(0) {}
#endif
};

typedef char saved_film_stream_compressor_stream_offset[(sizeof(void *) != 4 || ((offsetof(s_saved_film_stream_compressor, m_stream) == 0x18))) ? 1 : -1];
typedef char saved_film_stream_compressor_size[(sizeof(void *) != 4 || ((sizeof(s_saved_film_stream_compressor) == 0x984c))) ? 1 : -1];

#endif
