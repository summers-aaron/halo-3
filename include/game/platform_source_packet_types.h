#ifndef GAME_PLATFORM_SOURCE_PACKET_TYPES_H
#define GAME_PLATFORM_SOURCE_PACKET_TYPES_H

#include <xtl.h>
#include <xaudio.h>

struct s_platform_source_packet {
    long type;
    union
    {
        XAUDIOSOURCEBUFFER source_buffer;
        XAUDIOPACKET packet;
    };
};

#if defined(_M_PPCBE) || defined(_XBOX)
typedef char platform_source_packet_size[(sizeof(void *) != 4 || sizeof(s_platform_source_packet) == 0x5c) ? 1 : -1];
#endif

#endif // GAME_PLATFORM_SOURCE_PACKET_TYPES_H
