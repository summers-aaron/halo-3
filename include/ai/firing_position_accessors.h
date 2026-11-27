#ifndef AI_FIRING_POSITION_ACCESSORS_H
#define AI_FIRING_POSITION_ACCESSORS_H

#include "ai/firing_position_ref.h"

struct firing_position_definition;
extern "C" firing_position_definition *firing_position_ref_get_definition(firing_position_ref handle);

#endif // AI_FIRING_POSITION_ACCESSORS_H
