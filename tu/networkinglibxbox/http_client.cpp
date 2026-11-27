// networkinglibxbox/http_client.obj: from the records 0x3a9298 0x408968 0x4089f8 0x408a28 0x408b50 0x408bd8 0x408cd0 0x408f30 0x4091a8 0x409408 0x409410 0x4094c0
// @flags /TP /O2
// @address 0x3a9298 ?print@?$c_static_string@$0BA@@@QAAPBDPBDZZ
// @address 0x409410 ?starts_with@?$c_static_string@$0BAAA@@@QBA_NPBD@Z
// @address 0x4094c0 ?length@?$c_static_string@$0BAAA@@@QBAJXZ
// @address 0x408968 ??0c_http_client@@QAA@XZ
// @address 0x4089f8 ?is_connected@c_http_client@@QAA_NXZ
// @address 0x408a28 ?start@c_http_client@@QAA_NPAVc_http_stream@@JGPBD_N@Z
// @address 0x408b50 ?stop@c_http_client@@QAA_NXZ
// @address 0x4091a8 ?parse_http_response@c_http_client@@AAA_NPBDJPA_NPAJ22@Z
// @address 0x409408 ?transport_shutdown@c_http_client@@CAXPAX@Z
#define NOD3D
#include <xtl.h>
#include "cseries/cseries.h"
#include "networking/transport/transport_globals.h"
#include <stdio.h>
#include <ppcintrinsics.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include "memory/static_arrays.h"
#include "networking/transport/transport_endpoint_winsock.h"
#include "networking/transport/transport_address.h"
#include "networking/transport/transport.h"
#include "networking/transport/transport_available.h"
#include "networking/tools/http_stream_types.h"
#include "networking/tools/http_client.h"

extern "C" {
}



// formats into the 16-character buffer and always terminates it
template<long k_maximum_count>
const char *c_static_string<k_maximum_count>::print(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    _vsnprintf(m_string, k_maximum_count - 1, format, args);
    m_string[k_maximum_count - 1] = 0;
    return m_string;
}

extern "C" {
}

static __forceinline void http_client_transport_endpoint_setup(transport_endpoint *endpoint, long type)
{
    endpoint->socket = -1;
    endpoint->flags = 0;
    endpoint->type = type;
}

c_http_client::c_http_client()
{
    m_current_time = 0;
    m_previous_time = 0;
    m_rate = -1;
    m_current_state = 0;
    memset(&m_endpoint_storage, 0, sizeof(m_endpoint_storage));
    m_endpoint = &m_endpoint_storage;
    transport_register_transition_functions(NULL, c_http_client::transport_shutdown, NULL, this);
}

bool c_http_client::is_connected()
{
    return m_current_state == 2 || m_current_state == 3 || m_current_state == 4;
}

bool c_http_client::start(c_http_stream *stream, long ip_address, unsigned short port, const char *url, bool use_ssl)
{
    bool result = false;
    m_upload_stream = stream;
    m_address.address_length = sizeof(m_address.address.ipv4_address);
    m_address.address.ipv4_address = ip_address;
    m_address.port = port;
    get_ip_address_string(ip_address, &m_ip_address_string);
    if (transport_address_valid(&m_address))
    {
        http_client_transport_endpoint_setup(m_endpoint, 2);
        if (use_ssl)
            m_endpoint->flags |= 0x40;
        if (start_connect())
        {
            m_upload_stream->set_url(url);
            result = true;
        }
    }
    return result;
}

bool c_http_client::stop()
{
    bool success = true;
    if (m_current_state != 0) {
        if (!m_upload_stream->reset())
            success = false;
        transport_endpoint_disconnect(m_endpoint);
        --m_socket_count;
        m_current_state = 0;
    }
    return success;
}

void c_http_client::transport_shutdown(void *context)
{
    c_http_client *client = static_cast<c_http_client *>(context);
    client->stop();
}

template<long k_maximum_count>
bool c_static_string<k_maximum_count>::starts_with(const char *prefix) const
{
    return memcmp(prefix, m_string, csstrnlen(prefix, k_maximum_count)) == 0;
}

template<long k_maximum_count>
long c_static_string<k_maximum_count>::length() const
{
    const char *string = m_string;
    unsigned long count;
    for (count = 0; count < k_maximum_count; count++) {
        if (*string++ == '\0')
            break;
    }
    return count;
}

static __forceinline short http_client_transport_endpoint_write(transport_endpoint *endpoint, const void *buffer, short length)
{
    short bytes_written = 0;
    if (transport_available() && (endpoint->flags & 1) != 0)
    {
        bytes_written = (short)send(endpoint->socket, (const char *)buffer, length, 0);
        if (bytes_written == -1)
        {
            long error = WSAGetLastError();
            if (error != 10035)
            {
                if (error != 10065)
                    bytes_written = -3;
                else
                    bytes_written = -1;
            }
            else
                bytes_written = -2;
        }
    }
    return bytes_written;
}
// 0x408bd8 natural source
bool c_http_client::do_work(
    bool *out_completed_successfully, char *out_response_content_buffer, long *out_response_content_buffer_count, long *out_response_code)
{
    __int64 previous_time = m_current_time;
    bool result = false;
    *out_completed_successfully = false;
    m_previous_time = previous_time;
    m_current_time = __mftb();
    if (m_current_state == 1)
    {
        bool connected = false;
        if (out_response_content_buffer_count)
            *out_response_content_buffer_count = 0;
        if (transport_endpoint_async_is_connected(m_endpoint, &connected))
        {
            result = true;
            if (connected)
                m_current_state = 2;
        }
    }
    else if (m_current_state == 2)
    {
        if (out_response_content_buffer_count)
            *out_response_content_buffer_count = 0;
        if (send_data())
            result = true;
    }
    else if (m_current_state == 3 || m_current_state == 4)
    {
        if (receive_data(out_completed_successfully, out_response_content_buffer, out_response_content_buffer_count, out_response_code))
            result = true;
    }
    if (!result || *out_completed_successfully)
        stop();
    return result;
}

// @address 0x408cd0 ?send_data@c_http_client@@AAA_NXZ
bool c_http_client::send_data()
{
    bool success = true;
    long bytes_to_send = -1;
    if (m_rate != -1)
        bytes_to_send = (long)(m_rate * ((double)(m_current_time - m_previous_time) / 49875.0) / 1000.0);
    while (success && (bytes_to_send == -1 || bytes_to_send > 0))
    {
        long position = m_upload_stream->get_position();
        long bytes_read = 0;
        success = false;
        if (transport_endpoint_writeable(m_endpoint))
        {
            long read_size = 4096;
            if (bytes_to_send != -1)
                read_size = bytes_to_send < 4096 ? bytes_to_send : 4096;
            char buffer[4096];
            if (m_upload_stream->read(buffer, read_size, &bytes_read))
            {
                short bytes_written = 0;
                if (bytes_read == 0)
                    bytes_written = 0;
                else
                    bytes_written = http_client_transport_endpoint_write(m_endpoint, buffer, (short)bytes_read);
                if (bytes_written > 0)
                {
                    success = true;
                    if (bytes_to_send != -1)
                        bytes_to_send -= bytes_written;
                    if (bytes_written < bytes_read)
                    {
                        m_upload_stream->set_position(position + bytes_written);
                        break;
                    }
                    else if (m_upload_stream->at_end())
                    {
                        m_response_position = 0;
                        m_current_state = 3;
                        break;
                    }
                }
                else if (bytes_written == -2)
                {
                    m_upload_stream->set_position(position);
                    success = true;
                    break;
                }
            }
        }
        else
        {
            success = true;
            break;
        }
    }
    return success;
}

bool c_http_client::start_connect()
{
    bool result = false;
    if (::transport_endpoint_async_connect(m_endpoint, &m_address))
    {
        m_current_state = 1;
        m_socket_count++;
        result = true;
    }
    else
    {
        transport_endpoint_disconnect(m_endpoint);
    }
    return result;
}

bool c_http_client::receive_data(
    bool *out_completed_successfully, char *out_response_content_buffer, long *out_response_content_buffer_count, long *out_response_code)
{
    bool result = false;
    long bytes_read = 0;
    long input_buffer_size = 0;
    *out_completed_successfully = false;
    if (out_response_content_buffer_count)
    {
        input_buffer_size = *out_response_content_buffer_count;
        *out_response_content_buffer_count = 0;
    }
    if (4096 - m_response_position > 0 && (m_current_state == 3 || m_unknown > 0))
    {
        bytes_read = (short)transport_endpoint_read(
            m_endpoint, m_response_buffer + m_response_position, (short)(4096 - m_response_position));
        if (bytes_read > 0)
        {
            m_response_position += bytes_read;
            result = true;
        }
        else if (bytes_read == -2)
        {
            result = true;
            bytes_read = 0;
        }
    }
    else
    {
        result = true;
    }
    if (result)
    {
        result = false;
        if (m_current_state == 3)
        {
            bool header_complete = false;
            long http_header_size = 0;
            long content_length = 0;
            long http_response_code = 0;
            if (parse_http_response(m_response_buffer, m_response_position, &header_complete, &http_header_size, &http_response_code, &content_length))
            {
                if (header_complete)
                {
                    if (out_response_code)
                        *out_response_code = http_response_code;
                    if (http_response_code == 200)
                    {
                        if (out_response_content_buffer && out_response_content_buffer_count)
                        {
                            long content_buffer_size = m_response_position - http_header_size;
                            for (long index = 0; index < content_buffer_size; ++index)
                                m_response_buffer[index] = m_response_buffer[index + http_header_size];
                            m_response_position = content_buffer_size;
                            m_unknown = content_length - content_buffer_size;
                            m_current_state = 4;
                            result = true;
                        }
                        else
                        {
                            result = true;
                            *out_completed_successfully = true;
                        }
                    }
                }
                else
                    result = true;
            }
            else
            {
                if (m_response_position < 4096)
                    result = true;
            }
        }
        else
        {
            m_unknown -= bytes_read;
            result = true;
        }
    }
    if (result && m_current_state == 4 && out_response_content_buffer && out_response_content_buffer_count)
    {
        long bytes_to_copy = input_buffer_size > m_response_position ? m_response_position : input_buffer_size;
        memcpy(out_response_content_buffer, m_response_buffer, bytes_to_copy);
        *out_response_content_buffer_count = bytes_to_copy;
        for (long index = 0; index < m_response_position - bytes_to_copy; ++index)
            m_response_buffer[index] = m_response_buffer[index + bytes_to_copy];
        m_response_position -= bytes_to_copy;
        if (m_unknown == 0 && m_response_position == 0)
            *out_completed_successfully = true;
    }
    return result;
}

bool c_http_client::parse_http_response(const char *buffer, long buffer_size, bool *out_completed_successfully, long *http_header_size,
    long *http_response_code, long *content_length)
{
    bool result = true;
    c_static_string<4096> response;
    response.set_bounded(buffer, buffer_size);
    *out_completed_successfully = false;
    *http_header_size = -1;
    *content_length = -1;
    if (response.length() > 9)
    {
        if (!response.starts_with("HTTP/1.0 ") && !response.starts_with("HTTP/1.1 "))
        {
            result = false;
        }
        else
        {
            *http_response_code = atoi(response.get_string() + 9);
            long index = response.index_of("\r\n") + 2;
            long length = response.length();
            while (index < length)
            {
                long next_index = -1;
                if (index < length)
                {
                    const char *found = strstr(response.get_string() + index, "\r\n");
                    if (found)
                        next_index = found - response.get_string();
                }
                if (index == next_index)
                {
                    *http_header_size = next_index + 2;
                    break;
                }
                else
                {
                    c_static_string<4096> line;
                    if (response.substring(index, next_index - index, &line))
                    {
                        const char *content_length_prefix = "Content-Length: ";
                        if (line.starts_with(content_length_prefix))
                            *content_length = atoi(line.get_string() + strlen(content_length_prefix));
                    }
                }
                index = next_index + 2;
                length = response.length();
            }
            if (*http_header_size > 0)
            {
                if (*content_length >= 0)
                    *out_completed_successfully = true;
                else
                    result = false;
            }
        }
    }
    return result;
}

void c_http_client::get_ip_address_string(long ip_address, c_static_string<16> *string)
{
    string->clear();
    string->print("%d.%d.%d.%d", (unsigned char)(ip_address >> 24), (unsigned char)(ip_address >> 16), (unsigned char)(ip_address >> 8),
        (unsigned char)ip_address);
}
