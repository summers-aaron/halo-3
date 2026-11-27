#ifndef DEVICES_MACHINE_DATUM_H
#define DEVICES_MACHINE_DATUM_H
#include "devices/device_datum.h"
enum e_machine_flags
{
    _machine_flag_0 = 0,
    _machine_flag_1 = 1,
    _machine_flag_2 = 2,
    _machine_flag_3 = 3,
    _machine_flag_4 = 4,
    _machine_flag_5 = 5,
    _machine_flag_6 = 6,
    _machine_flag_7 = 7
};
struct s_machine_datum : device_datum
{
    // This unit uses the nonvolatile object flags view; keep c_flags set codegen.
    __forceinline c_flags_no_init<e_object_data_flags, unsigned long, 32> &object_flag_bits()
    {
        return *(c_flags_no_init<e_object_data_flags, unsigned long, 32> *)&flags;
    }
    c_flags_no_init<e_machine_flags, unsigned long, 32> machine_flags;
    long door_open_ticks; // device_update increments +0x22c while the door is open.
    short pad230;
    short pathfinding_object_index;
    short pathfinding_count;
    short pathfinding_bsp_index;
};
typedef char machine_datum_header_flags_offset[(offsetof(s_machine_datum, machine_flags) == 0x228) ? 1 : -1];
typedef char machine_datum_door_open_ticks_offset[(offsetof(s_machine_datum, door_open_ticks) == 0x22c) ? 1 : -1];
typedef char machine_datum_pathfinding_offset[(offsetof(s_machine_datum, pathfinding_object_index) == 0x232) ? 1 : -1];
#endif
