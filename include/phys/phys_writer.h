#ifndef INCLUDE_PHYS_WRITER_H
#define INCLUDE_PHYS_WRITER_H

// hkStreamWriter, defined in tu/hkbase/hkStreamWriter.cpp.

#include "phys/phys_base.h"

class phys_writer : public phys_ref_object
{
public:
    PHYS_DECLARE_ALLOCATOR(PHYS_MEM_STREAM);

    phys_writer() {}

    virtual phys_bool is_stream_ok() const = 0;
    virtual int write(const void *buf, int nbytes) = 0;
    virtual void flush() {}
    virtual phys_bool seek_tell_supported() const;

    enum seek_whence { SEEK_FROM_START = 0, SEEK_FROM_CURRENT = 1, AGENT_STREAM_END = 2 };

    virtual phys_result seek(int offset, seek_whence seek_origin);
    virtual int tell() const;
};

#endif // HAVOK_HK_STREAM_WRITER_H
