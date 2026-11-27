#ifndef SIMULATION_SIMULATION_UPDATE_METADATA_H
#define SIMULATION_SIMULATION_UPDATE_METADATA_H

struct s_simulation_update_metadata
{
    unsigned short flags;
    int async_buffer_position;
    int film_storage_used;
    s_simulation_update_metadata() : flags(0) {}
};

typedef char simulation_update_metadata_size[(sizeof(s_simulation_update_metadata) == 12) ? 1 : -1];

#endif
