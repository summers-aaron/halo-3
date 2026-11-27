#ifndef AI_INTERACTION_TARGET_H
#define AI_INTERACTION_TARGET_H

#include <stddef.h>

#include "ai/hint_ref.h"
#include <string.h>

// An interaction target is either an AI hint or an object, selected by the type discriminator.
class c_interaction_target
{
public:
    union
    {
        c_hint_ref hint;
        long object_index;
    };
    char type;

    // March 8287acf8: checks the complete first word, for either target type.
    __forceinline bool valid()
    {
        return object_index != 0xffff;
    }

    // Resets the target to the empty (invalid hint) value.
    __forceinline void clear()
    {
        memset(this, 0, sizeof(*this));
        object_index = 0xffff;
        type = 0;
    }

    // March 82ed0708: copies the two hint halfwords, then the discriminator.
    __forceinline void set_hint(c_hint_ref reference)
    {
        hint = reference;
        type = 0;
    }

    // March 82f85940.
    __forceinline void set_object(long index)
    {
        object_index = index;
        type = 1;
    }
};

typedef char interaction_target_size[(sizeof(c_interaction_target) == 8) ? 1 : -1];
typedef char interaction_target_type_offset[((offsetof(c_interaction_target, type)) == 4) ? 1 : -1];

#endif // AI_INTERACTION_TARGET_H
