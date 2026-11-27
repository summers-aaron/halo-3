#ifndef INCLUDE_PHYS_BUFFERED_WRITER_H
#define INCLUDE_PHYS_BUFFERED_WRITER_H

#include "phys/phys_writer.h"

class phys_buffered_writer : public phys_writer {
public:
    phys_buffered_writer(phys_writer* s, int buffer_bytes = 4096);
    phys_buffered_writer(void* mem, int mem_byte_count, phys_bool mem_is_string);
    ~phys_buffered_writer();
    virtual int write(const void* buf, int nbytes);
    virtual void flush();
    virtual phys_bool is_stream_ok() const;
    virtual phys_bool seek_tell_supported() const;
    virtual phys_result seek(int offset, seek_whence seek_origin);
    virtual int tell() const;
protected:
    phys_writer* m_stream;
    char* m_read_buf;
    int m_buf_size;
    int m_buf_capacity;
    phys_bool m_owns_buffer;
    int flush_stream_buffer();
};

#endif // INCLUDE_HAVOK_HK_BUFFERED_STREAM_WRITER_H
