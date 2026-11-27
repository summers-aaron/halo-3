#ifndef AI_AI_GLOBALS_H
#define AI_AI_GLOBALS_H

#include "ai/ai_collision_constants.h"

struct s_data_array;

// Shared pool views consumed by ai.cpp. Storage remains in its owning units.
extern "C" {
#include "effects/effect_data.h"
}

#endif // AI_AI_GLOBALS_H
