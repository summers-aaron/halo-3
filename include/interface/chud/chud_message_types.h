#ifndef INTERFACE_CHUD_CHUD_MESSAGE_TYPES_H
#define INTERFACE_CHUD_CHUD_MESSAGE_TYPES_H

#include <stddef.h>

#include "interface/chud/chud_message_context_types.h"

struct s_chud_message
{
    wchar_t text[96];
    bool visible;
    bool active;
    long time;
    long start_time;
    float target_row;
    float current_row;
    float offset;
    float alpha;
    float height;
    float scale;
    e_chud_message_context context;
};

typedef char chud_message_size[(sizeof(s_chud_message) == 0xe8) ? 1 : -1];
typedef char chud_message_start_time_offset[(offsetof(s_chud_message, start_time) == 0xc8) ? 1 : -1];

#endif
