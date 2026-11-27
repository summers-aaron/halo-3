#ifndef AI_CL_ENGINE_COMMANDS_H
#define AI_CL_ENGINE_COMMANDS_H

#include "ai/cl_engine.h"

// Typed atom-command interface implemented by cl_engine.cpp.
extern "C" cs_command *cs_initialize_command(long owner_index, short type, bool active);

#endif // AI_CL_ENGINE_COMMANDS_H
