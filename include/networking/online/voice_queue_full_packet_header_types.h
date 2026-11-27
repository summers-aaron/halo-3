#ifndef NETWORKING_ONLINE_VOICE_QUEUE_FULL_PACKET_HEADER_TYPES_H
#define NETWORKING_ONLINE_VOICE_QUEUE_FULL_PACKET_HEADER_TYPES_H

struct s_voice_queue_full_packet_header {
    unsigned char sequence;
    unsigned short recipients;
};

#endif // NETWORKING_ONLINE_VOICE_QUEUE_FULL_PACKET_HEADER_TYPES_H
