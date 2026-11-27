#ifndef SIMULATION_SIMULATION_MACHINE_UPDATE_ENTRY_TYPES_H
#define SIMULATION_SIMULATION_MACHINE_UPDATE_ENTRY_TYPES_H

#include "math/matrix_math_types.h"
#include "memory/data_types.h"

struct s_simulation_camera_update {
    long type;
    long machine_index;
    real_point3d position;
    unsigned char identifier[12];
    bool valid;
};

typedef char simulation_camera_update_size[(sizeof(s_simulation_camera_update) == 0x24) ? 1 : -1];
typedef char simulation_camera_update_position_offset[(offsetof(s_simulation_camera_update, position) == 0x8) ? 1 : -1];

#endif // SIMULATION_SIMULATION_MACHINE_UPDATE_ENTRY_TYPES_H
