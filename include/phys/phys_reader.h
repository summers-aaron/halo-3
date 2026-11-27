#ifndef INCLUDE_PHYS_READER_H
#define INCLUDE_PHYS_READER_H

// hkStreamReader, defined in tu/hkbase/hkStreamReader.cpp.

#include "phys/phys_base.h"

class phys_reader : public phys_ref_object
{
public:
    PHYS_DECLARE_ALLOCATOR(PHYS_MEM_STREAM);

    phys_reader() {}

    virtual phys_bool is_stream_ok() const = 0;
    virtual int read(void *buf, int nbytes) = 0;
    virtual int skip(int nbytes);
    int read_char();
    virtual phys_bool mark_supported() const;
    virtual phys_result set_stream_mark(int mark_limit);
    virtual phys_result rewind_to_mark();
    virtual phys_bool seek_tell_supported() const;

    enum seek_whence { SEEK_FROM_START = 0, SEEK_FROM_CURRENT = 1, AGENT_STREAM_END = 2 };

    virtual phys_result seek(int offset, seek_whence seek_origin);
    virtual int tell() const;
};

#endif // HAVOK_HK_STREAM_READER_H
