#ifndef AI_COMMAND_SCRIPT_DATA_H
#define AI_COMMAND_SCRIPT_DATA_H

#include "ai/command_script_types.h"

// cl_engine's command-script datum array. Kept in this leaf until the remaining
// command-script units adopt s_data_array in place of their private array types.
#include "ai/command_script_tls.h"

static __forceinline s_data_array *tls_command_script_data()
{
    return command_script_data;
}

static __forceinline s_data_array *&tls_command_script_data_slot()
{
    return command_script_data;
}

#endif // AI_COMMAND_SCRIPT_DATA_H
