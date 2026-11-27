#ifndef INCLUDE_PHYS_TRACE_STREAM_H
#define INCLUDE_PHYS_TRACE_STREAM_H

#include "phys/phys_singleton.h"
#include "phys/phys_out_stream.h"

class phys_trace_stream : public phys_singleton<phys_trace_stream> {
public:
    struct trace_title {
        PHYS_DECLARE_NONVIRTUAL_ALLOCATOR(PHYS_MEM_BASE_CLASS, phys_trace_stream::trace_title);
        trace_title(const char* value) { phys_string::str_ncpy(m_value, value, 31); }
        char m_value[32];
    };
    void disable_title(const char* title) { m_trace_titles.array_push_back(trace_title(title)); }
    void reset();
    void printf(const char* title, const char* format_str, ...);
    void skip_printf(const char* title, const char* format_str, ...);
    phys_trace_stream() { m_stream = 0; m_counter = 0; }
    phys_out_stream* m_stream;
    int m_counter;
    phys_array<trace_title> m_trace_titles;
};

#endif // INCLUDE_HAVOK_HK_TRACE_STREAM_H
