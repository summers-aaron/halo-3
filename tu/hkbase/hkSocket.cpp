// hkbase/hkSocket.obj: from the records 0x610280 0x6102f0 0x610330 0x6103a0 0x6103e0 0x610428 0x6104c0 0x610520
// @compiler xdk6130
// @flags /TP /O2 /Oc /GF /EHs-c- /GR-
// @address 0x610280 ?read@reader_adapter@phys_socket@@UAAHPAXH@Z
// @address 0x6102f0 ?is_stream_ok@reader_adapter@phys_socket@@UBA?AVphys_bool@@XZ
// @address 0x610330 ?write@socket_writer_adapter@phys_socket@@UAAHPBXH@Z
// @address 0x6103a0 ?is_stream_ok@socket_writer_adapter@phys_socket@@UBA?AVphys_bool@@XZ
// @address 0x6103e0 ?phys_socket_create@@YAPAVphys_socket@@XZ
// @address 0x610428 ??0phys_socket@@IAA@XZ
// @address 0x6104c0 ??_Greader_adapter@phys_socket@@UAAPAXI@Z
// @address 0x610520 ??_Gphys_socket@@UAAPAXI@Z
#define PHYS_CONFIG_SIMD 1
#include "phys/phys_bsd_socket.h"

#include "phys/phys_base.h"

#include "phys/phys_socket.h"
    phys_socket* PHYS_CALL phys_socket_create()
    {
        return new phys_bsd_socket();
    }

// @address 0x7ce595 ?s_platformNetInitialized@phys_socket@@2Vphys_bool@@A
phys_bool phys_socket::s_platformNetInitialized;
// @address 0x724590 ?s_platformNetInit@phys_socket@@2P6AXXZA
void (PHYS_CALL *phys_socket::s_platformNetInit)(void) = phys_bsd_network_init;
// @address 0x724594 ?s_platformNetQuit@phys_socket@@2P6AXXZA
void (PHYS_CALL *phys_socket::s_platformNetQuit)(void) = phys_bsd_network_quit;

phys_socket::phys_socket()
{
    m_reader.m_reader_socket = this;
    m_writer.m_reader_socket = this;

    if (s_platformNetInitialized == false && s_platformNetInit)
    {
        s_platformNetInit();
        s_platformNetInitialized = true;
    }
}

int phys_socket::reader_adapter::read( void* buf, int nbytes )
{
    char* char_buffer = static_cast<char*>(buf);
    int size = 0;
    while( size < nbytes )
    {
        int r = m_reader_socket->read(char_buffer+size, nbytes-size);
        size += r;
        if( r == 0 )
        {
            return size;
        }
    }
    return nbytes;
}

phys_bool phys_socket::reader_adapter::is_stream_ok() const
{
    return m_reader_socket->is_stream_ok();
}

int phys_socket::socket_writer_adapter::write( const void* buf, int nbytes )
{
    const char* char_buffer = static_cast<const char*>(buf);
    int size = 0;
    while( size < nbytes )
    {
        int w = m_reader_socket->write(char_buffer+size, nbytes-size);
        size += w;
        if( w == 0 )
        {
            return size;
        }
    }
    return nbytes;

}

phys_bool phys_socket::socket_writer_adapter::is_stream_ok() const
{
    return m_reader_socket->is_stream_ok();
}

