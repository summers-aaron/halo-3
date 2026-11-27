#ifndef INTERFACE_EVENT_MANAGER_TYPES_H
#define INTERFACE_EVENT_MANAGER_TYPES_H

#include <stddef.h>
#include "interface/controller_types.h"

struct s_event_record
{
    long type;
    e_controller_index controller;
    long component;
    short value;
};

typedef char event_record_size[(sizeof(s_event_record) == 0x10) ? 1 : -1];
typedef char event_record_value_offset[(offsetof(s_event_record, value) == 0xc) ? 1 : -1];

#endif // INTERFACE_EVENT_MANAGER_TYPES_H
