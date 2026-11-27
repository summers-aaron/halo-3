#ifndef AI_AI_DEBUG_THROTTLED_GLOBAL_H
#define AI_AI_DEBUG_THROTTLED_GLOBAL_H

#if defined(_XBOX) && !defined(__clang__)
struct s_ai_debug_throttled_global
{
    long type;
    bool *value;
};
#else
#include "hs/hs.h"
#include "hs/hs_types.h"

struct s_ai_debug_throttled_global : hs_global_external
{
    explicit s_ai_debug_throttled_global(bool *throttled) :
        hs_global_external{ _hs_type_boolean, throttled }
    {
    }
};

extern s_ai_debug_throttled_global g_ai_debug_throttled_global;
#endif

#endif
