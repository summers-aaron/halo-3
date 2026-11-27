#ifndef NETWORKING_TOOLS_HTTP_STREAM_TYPES_H
#define NETWORKING_TOOLS_HTTP_STREAM_TYPES_H

#include "networking/tools/http_stream.h"
#include <stddef.h>

// The base stream declares six virtual operations in this order.
// The constructor and accessors are inline; this class sizes
// the built-header and extra-header strings to 1536 and 1024 characters.
class __declspec(novtable) c_http_stream
{
public:
    __forceinline c_http_stream() : m_header_size(0), m_position(0) {}
    virtual void build_headers() = 0;
    virtual bool verify_nescessary_state_is_set() = 0;
    virtual bool read(char *buffer, long buffer_size, long *bytes_read) = 0;
    virtual long get_length() = 0;
    virtual bool at_end() = 0;
    virtual bool reset();
    void set_url(const char *url) { m_url.set(url); }
    const char *get_url() const { return m_url.get_string(); }
    c_static_string<1024> &get_headers() { return m_content; }
    void clear_headers() { m_content.clear(); }
    void set_headers(const char *headers) { m_content.set(headers); }
    long get_position() { return m_position; }
    void set_position(long position) { m_position = position; }
protected:
    c_static_string<256> m_url;
    c_static_string<1536> m_headers;
    long m_header_size;
    c_static_string<1024> m_content;
    long m_position;
};

class c_http_post_stream : public c_http_stream
{
public:
    c_http_post_stream();
    virtual bool read(char *buffer, long buffer_size, long *bytes_read);
    virtual bool reset();
    virtual long get_length();
    virtual bool at_end();
    const c_http_post_source *get_source() const { return &m_post_source; }
    void set_source(const c_http_post_source *source)
    {
        m_post_source = *source;
        reset();
    }
protected:
    virtual void build_headers();
    virtual bool verify_nescessary_state_is_set();
private:
    c_http_post_source m_post_source;
    c_static_string<256> m_cookies;
    c_static_string<128> m_boundary;
    long m_post_header_size;
    long m_prologue_size;
    long m_epilogue_size;
};

class c_http_get_stream : public c_http_stream
{
public:
    __forceinline c_http_get_stream() {}
    virtual bool read(char *buffer, long buffer_size, long *bytes_read);
    virtual long get_length();
    virtual bool at_end();
protected:
    virtual void build_headers();
    virtual bool verify_nescessary_state_is_set();
};

typedef char http_stream_size_check[(sizeof(void *) != 4 || ((sizeof(c_http_stream) == 0xb0c) )) ? 1 : -1];
typedef char http_post_stream_size_check[(sizeof(void *) != 4 || ((sizeof(c_http_post_stream) == 0xdd0) )) ? 1 : -1];
typedef char http_get_stream_size_check[(sizeof(void *) != 4 || ((sizeof(c_http_get_stream) == 0xb0c) )) ? 1 : -1];

template<> long c_static_string<1536>::length() const;

#endif
