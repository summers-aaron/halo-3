#ifndef NETWORKING_TOOLS_HTTP_STREAM_H
#define NETWORKING_TOOLS_HTTP_STREAM_H

#include "memory/static_arrays.h"

struct s_file_reference;

// March http_stream.cpp:122..176 names networking\tools\http_stream.cpp.
// The content buffer and length getters skip past the start offset.
class c_http_post_source
{
public:
    enum e_contents_source
    {
        _contents_source_none,
        _contents_source_buffer,
        _contents_source_file
    };
    // March constructor: none, "text/plain", "blob", null buffer/file, zero length.
    // The start offset is initialized to zero.
    c_http_post_source();
    // March: buffer source, m_contents = contents_buffer, m_contents_length = contents_length.
    void set_source_as_buffer(const char *contents_buffer, long contents_length)
    {
        m_source_type = _contents_source_buffer;
        m_contents = contents_buffer;
        m_contents_length = contents_length;
    }
    // March: file source, m_contents_file = contents_file; file_get_size(..., &m_contents_length).
    bool set_source_as_file(s_file_reference *contents_file);
    // March: m_content_type.set(content_type), m_filename.set(filename), m_contents_offset = start_position.
    void set_content_type(const char *content_type) { m_content_type.set(content_type); }
    __forceinline void set_filename(const char *filename) { m_filename.set(filename); }
    __forceinline void set_start_position(long start_position) { m_contents_offset = start_position; }
    __forceinline e_contents_source get_source_type() const { return m_source_type; }
    __forceinline const char *get_content_type() const { return m_content_type.get_string(); }
    __forceinline const char *get_filename() const { return m_filename.get_string(); }
    __forceinline const char *get_contents_buffer() const { return m_contents + m_contents_offset; }
    __forceinline s_file_reference *get_contents_file() const { return m_contents_file; }
    __forceinline long get_contents_length() const { return m_contents_length - m_contents_offset; }
    __forceinline long get_start_position() const { return m_contents_offset; }
private:
    e_contents_source m_source_type;
    c_static_string<32> m_content_type;
    c_static_string<260> m_filename;
    const char *m_contents;
    s_file_reference *m_contents_file;
    long m_contents_length;
    long m_contents_offset;
};
typedef char http_post_source_size_check[(sizeof(void *) != 4 || ((sizeof(c_http_post_source) == 0x138) )) ? 1 : -1];

#endif
