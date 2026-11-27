#ifndef INCLUDE_PHYS_STREAMBUF_FACTORY_H
#define INCLUDE_PHYS_STREAMBUF_FACTORY_H

#include "phys/phys_singleton.h"

class phys_reader;
class phys_writer;

class phys_streambuf_factory : public phys_singleton<phys_streambuf_factory> {
public:
    phys_streambuf_factory() {}
    enum std_stream { STD_STREAM_OUT = 1, STD_STREAM_ERR };
    virtual phys_reader *open_reader(const char *name) = 0;
    virtual phys_writer *open_writer(const char *name) = 0;
};

template <>
void phys_singleton<phys_streambuf_factory>::replace_singleton_instance(phys_streambuf_factory *instance);

#endif // HAVOK_HK_STREAMBUF_FACTORY_H
