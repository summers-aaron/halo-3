#ifndef INTERFACE_CHUD_CHUD_DISPOSE_STATE_TYPES_H
#define INTERFACE_CHUD_CHUD_DISPOSE_STATE_TYPES_H

#include "memory/data_types.h"

struct chud_dispose_state
{
    union { long index; long definition_index; long tag; void *address; };
    long start_time;
    union { long ticks; long last_ticks; long frame; };
    union { long definition_value; long end_ticks; };
};

typedef char chud_dispose_state_size[(sizeof(void *) != 4 || ((sizeof(chud_dispose_state) == 0x10))) ? 1 : -1];

extern "C" { extern __declspec(thread) chud_dispose_state *g_chud_cinematic_state; }

static __forceinline chud_dispose_state *chud_cinematic_state() { return g_chud_cinematic_state; }

#endif // INTERFACE_CHUD_CHUD_DISPOSE_STATE_TYPES_H
