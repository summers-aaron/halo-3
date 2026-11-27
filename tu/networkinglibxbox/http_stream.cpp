// networkinglibxbox/http_stream.obj: from the records 0x1e4fd0 0x3c4118 0x3c4130 0x3c41a0 0x3c41f8 0x3c4270 0x3c4338 0x3c4590 0x3c45b8 0x3c45f0 0x3c4660 0x3c46c0 0x3c4718 0x3c4840 0x3c4848 0x3c4860 0x3c4900
// @compiler xdk6130
// @flags /TP /O2 /Oc
#include "cseries/cseries.h"
#include "cseries/crt_xdk.h"
#include "tag_files/files_windows.h"
#include "networking/tools/http_stream_types.h"
#include "networking/tools/http_stream.h"
#include "tag_files/file_reference.h"

// @address 0x82308 ?print@?$c_static_string@$0BAA@@@QAAPBDPBDZZ
// @address 0x1aaf98 ?length@?$c_static_string@$0BAA@@@QBAJXZ
// @address 0x1ae380 ?file_get_size@@YA_NPBUs_file_reference@@PAK@Z
// @address 0x1ae0c8 ?file_set_position@@YA_NPAUs_file_reference@@K_N@Z
// @address 0x1ae220 ?file_read@@YA_NPAUs_file_reference@@K_NPAX@Z
// @address 0x1e4fd0 ?length@?$c_static_string@$0IA@@@QBAJXZ
// @address 0x3c4130 ??0c_http_post_source@@QAA@XZ
// @address 0x3c41a0 ?set_source_as_file@c_http_post_source@@QAA_NPAUs_file_reference@@@Z
// @address 0x3c41f8 ??0c_http_post_stream@@QAA@XZ
// @address 0x3c4898 ?print@?$c_static_string@$0GAA@@@QAAPBDPBDZZ
// @address 0x3c4900 ?length@?$c_static_string@$0GAA@@@QBAJXZ

// 0x1e4fd0
template<>
long c_static_string<128>::length() const
{
    const char *string = m_string;
    unsigned long len;
    for (len = 0; len < 128; ++len)
    {
        if (*string++ == '\0')
            break;
    }
    return len;
}

// @address 0x3c4118 ?reset@c_http_stream@@UAA_NXZ
bool c_http_stream::reset()
{
    bool result = true;
    m_position = 0;
    return result;
}

c_http_post_source::c_http_post_source() :
    m_source_type(_contents_source_none),
    m_content_type("text/plain"),
    m_filename("blob"),
    m_contents(0),
    m_contents_file(0),
    m_contents_length(0),
    m_contents_offset(0)
{
}

bool c_http_post_source::set_source_as_file(s_file_reference *file)
{
    bool result = false;
    m_source_type = _contents_source_file;
    m_contents_file = file;
    if (file_get_size(file, (unsigned long *)&m_contents_length))
        result = true;
    return result;
}

c_http_post_stream::c_http_post_stream()
{
    m_post_header_size = 0;
    m_prologue_size = 0;
    m_epilogue_size = 0;
}

// @address 0x3c4270 ?build_headers@c_http_post_stream@@MAAXXZ
void c_http_post_stream::build_headers()
{
    m_boundary.set("\r\n--BUNGIEr0x0rz--\r\n");
    m_epilogue_size = m_boundary.length();
    m_cookies.print("--BUNGIEr0x0rz\r\nContent-Disposition: form-data; name=\"upload\"; filename=\"%s\"\r\n"
        "Content-Type: %s\r\n\r\n", m_post_source.get_filename(), m_post_source.get_content_type());
    m_prologue_size = m_cookies.length();
    m_headers.print("POST %s HTTP/1.0\r\nContent-type: multipart/form-data; boundary=BUNGIEr0x0rz\r\n"
        "Content-Length: %d\r\n%s\r\n", m_url.get_string(), m_post_source.get_contents_length() + m_prologue_size + m_epilogue_size,
        m_content.get_string());
    m_post_header_size = m_headers.length();
}

// @address 0x3c4338 ?read@c_http_post_stream@@UAA_NPADJPAJ@Z
bool c_http_post_stream::read(char *buffer, long buffer_size, long *bytes_read)
{
    bool success = false;
    char *cursor = buffer;
    long remaining = buffer_size;
    long stream_position = m_position;
    long copy_size = 0;
    if (verify_nescessary_state_is_set())
    {
        if (m_position == 0)
            build_headers();
        success = true;
        while (!at_end() && remaining > 0)
        {
            stream_position = m_position;
            if (stream_position < m_post_header_size)
            {
                copy_size = remaining > m_post_header_size - stream_position ? m_post_header_size - stream_position : remaining;
                memcpy(cursor, m_headers.get_string() + stream_position, copy_size);
            }
            else
            {
                long prologue_position = stream_position - m_post_header_size;
                if (prologue_position < m_prologue_size)
                {
                    copy_size = remaining > m_prologue_size - prologue_position ? m_prologue_size - prologue_position : remaining;
                    memcpy(cursor, m_cookies.get_string() + prologue_position, copy_size);
                }
                else
                {
                    long content_position = prologue_position - m_prologue_size;
                    if (content_position < m_post_source.get_contents_length())
                    {
                        copy_size = remaining > m_post_source.get_contents_length() - content_position
                            ? m_post_source.get_contents_length() - content_position
                            : remaining;
                        if (m_post_source.get_source_type() == 1)
                        {
                            memcpy(cursor, m_post_source.get_contents_buffer() + content_position, copy_size);
                        }
                        else
                        {
                            s_file_reference *file = m_post_source.get_contents_file();
                            bool read_result = file_set_position(file, m_post_source.get_start_position() + content_position, false) &&
                                file_read(file, copy_size, false, cursor);
                            if (!read_result)
                                success = false;
                        }
                    }
                    else
                    {
                        long epilogue_position = content_position - m_post_source.get_contents_length();
                        if (epilogue_position < m_epilogue_size)
                        {
                            copy_size = remaining > m_epilogue_size - epilogue_position ? m_epilogue_size - epilogue_position : remaining;
                            memcpy(cursor, m_boundary.get_string() + epilogue_position, copy_size);
                        }
                        else
                        {
                            success = false;
                            break;
                        }
                    }
                }
            }
            cursor += copy_size;
            m_position += copy_size;
            remaining = buffer_size - (cursor - buffer);
        }
        *bytes_read = cursor - buffer;
    }
    return success;
}

long c_http_post_stream::get_length()
{
    return m_post_header_size + m_prologue_size + m_post_source.get_contents_length() + m_epilogue_size;
}

bool c_http_post_stream::at_end()
{
    return m_position == m_post_header_size + m_prologue_size + m_post_source.get_contents_length() + m_epilogue_size;
}

// @address 0x3c45f0 ?reset@c_http_post_stream@@UAA_NXZ
bool c_http_post_stream::reset()
{
    bool result = true;
    if (m_post_source.get_source_type() == 2) {
        if (!file_set_position(m_post_source.get_contents_file(), m_post_source.get_start_position(), true))
            result = false;
    }
    c_http_stream::reset();
    return result;
}

// @address 0x3c4660 ?verify_nescessary_state_is_set@c_http_post_stream@@MAA_NXZ
bool c_http_post_stream::verify_nescessary_state_is_set()
{
    long source_type = m_post_source.get_source_type();
    bool source_is_set = source_type == 2 || source_type == 1;
    return (source_is_set & (m_url.length() != 0)) & 1;
}

bool c_http_get_stream::read(char *buffer, long buffer_size, long *bytes_read)
{
    bool success = false;
    char *cursor = buffer;
    long remaining = buffer_size;
    long stream_position = m_position;
    long copy_size = 0;
    if (verify_nescessary_state_is_set())
    {
        if (m_position == 0)
            build_headers();
        success = true;
        while (!at_end() && remaining > 0)
        {
            stream_position = m_position;
            if (stream_position < m_header_size)
            {
                copy_size = remaining > m_header_size - stream_position ? m_header_size - stream_position : remaining;
                memcpy(cursor, m_headers.get_string() + stream_position, copy_size);
            }
            else
            {
                success = false;
                break;
            }
            cursor += copy_size;
            m_position += copy_size;
            remaining = buffer_size - (cursor - buffer);
        }
        *bytes_read = cursor - buffer;
    }
    return success;
}


// @address 0x3c46c0 ?build_headers@c_http_get_stream@@MAAXXZ
void c_http_get_stream::build_headers()
{
    m_headers.print("GET %s HTTP/1.0\r\n%s\r\n", m_url.get_string(), m_content.get_string());
    m_header_size = m_headers.length();
}

// @address 0x3c4840 ?get_length@c_http_get_stream@@UAAJXZ
long c_http_get_stream::get_length()
{
    return m_header_size;
}

// @address 0x3c4848 ?at_end@c_http_get_stream@@UAA_NXZ
bool c_http_get_stream::at_end()
{
    return m_position == m_header_size;
}

// @address 0x3c4860 ?verify_nescessary_state_is_set@c_http_get_stream@@MAA_NXZ
bool c_http_get_stream::verify_nescessary_state_is_set()
{
    return m_url.length() != 0 & 1;
}

// 0x3c4900
template<>
long c_static_string<1536>::length() const
{
    const char *string = m_string;
    unsigned long len;
    for (len = 0; len < 1536; ++len)
    {
        if (*string++ == '\0')
            break;
    }
    return len;
}
