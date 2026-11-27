#ifndef NETWORKING_ONLINE_VOICE_QUEUE_ENTRY_TYPES_H
#define NETWORKING_ONLINE_VOICE_QUEUE_ENTRY_TYPES_H

struct s_voice_queue_entry {
    char spare_before_enabled[4];
    char enabled;
    char spare_after_enabled[7];
    unsigned short recipient_mask;
    char spare_after_recipient_mask[432 - 0x0e];
};

#endif // NETWORKING_ONLINE_VOICE_QUEUE_ENTRY_TYPES_H
