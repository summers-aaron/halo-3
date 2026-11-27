#ifndef INCLUDE_PHYS_ARRAY_WRITER_H
#define INCLUDE_PHYS_ARRAY_WRITER_H

#include "phys/phys_base.h"
#include "phys/phys_writer.h"

class phys_array_writer : public phys_writer {
public:
    enum array_ownership { WRITER_ARRAY_OWNED, WRITER_ARRAY_BORROWED };
    phys_array_writer(phys_array<char> *buffer, array_ownership stream_array_ownership)
        : array(buffer), position(buffer->getSize()), owns_array(stream_array_ownership) { null_terminate(); }
    virtual ~phys_array_writer() { if (owns_array == WRITER_ARRAY_OWNED) delete array; }
    phys_array<char> *array;
    int position;
    int owns_array;
    void null_terminate();
    virtual int write(const void *buffer, int nbytes);
    virtual phys_bool is_stream_ok() const { return true; }
    virtual phys_bool seek_tell_supported() const { return true; }
    virtual void clear();
    virtual int tell() const;
    virtual phys_result seek(int offset, seek_whence seek_origin);
};

#endif // HAVOK_HK_ARRAY_STREAM_WRITER_H
