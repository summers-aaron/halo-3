#ifndef NETWORKING_ONLINE_VOICE_AVAILABILITY_H
#define NETWORKING_ONLINE_VOICE_AVAILABILITY_H

#include <string.h>
#include "memory/static_arrays.h"

class c_voice_availability
{
public:
    unsigned long get_outgoing_player_mask(long local_player_index);
    __forceinline void initialize()
    {
        memset(&outgoing_player_masks, 0, sizeof(outgoing_player_masks));
        initialized = true;
    }
    static __forceinline void update_local_outgoing_player_masks_bridge(c_voice_availability *self)
    {
        self->update_local_outgoing_player_masks();
    }
    bool initialized;
    c_static_array<unsigned long, 16> outgoing_player_masks;
private:
    void update_local_outgoing_player_masks();
};

#endif
