#ifndef INTERFACE_CHUD_CHUD_MEDAL_MESSAGE_TYPES_H
#define INTERFACE_CHUD_CHUD_MEDAL_MESSAGE_TYPES_H

#include <stddef.h>

struct s_chud_medal_message
{
    bool active;
    long medal_value;
    long time;
    long display_time;
    unsigned char shown;
    float target;
    float value;
    float eased;
    float alpha;
};

typedef char chud_medal_message_size[(sizeof(s_chud_medal_message) == 0x24) ? 1 : -1];
typedef char chud_medal_message_display_time_offset[(offsetof(s_chud_medal_message, display_time) == 0xc) ? 1 : -1];
typedef char chud_medal_message_alpha_offset[(offsetof(s_chud_medal_message, alpha) == 0x20) ? 1 : -1];

#endif // INTERFACE_CHUD_CHUD_MEDAL_MESSAGE_TYPES_H
