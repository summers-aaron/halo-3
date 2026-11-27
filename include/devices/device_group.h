#ifndef DEVICES_DEVICE_GROUP_H
#define DEVICES_DEVICE_GROUP_H

#include <stddef.h>

struct s_device_group_datum {
    unsigned short identifier;
    unsigned short flags;
    float actual_value;
    float desired_value;
};

typedef char device_group_datum_size[(sizeof(s_device_group_datum) == 12) ? 1 : -1];
typedef char device_group_datum_actual_offset[(offsetof(s_device_group_datum, actual_value) == 4) ? 1 : -1];
typedef char device_group_datum_desired_offset[(offsetof(s_device_group_datum, desired_value) == 8) ? 1 : -1];

#endif // DEVICES_DEVICE_GROUP_H
