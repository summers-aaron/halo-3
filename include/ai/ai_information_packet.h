#ifndef AI_AI_INFORMATION_PACKET_H
#define AI_AI_INFORMATION_PACKET_H
#include "ai/ai_information_data.h"
struct ai_information_packet
{
    short type;
    short padding;
    long listener_object_index;
    ai_information_data payload;
};
typedef char ai_information_packet_size[(sizeof(ai_information_packet) == 32) ? 1 : -1];
#endif
