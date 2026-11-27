#ifndef PHYSICS_CHARACTER_PHYSICS_MOVE_OUTPUT_H
#define PHYSICS_CHARACTER_PHYSICS_MOVE_OUTPUT_H

#include "physics/havok_contact_point_reference.h"

// 16-byte result of a character physics move, consumed by the biped motor system.
struct s_character_physics_move_output_datum
{
    bool flags_test(long bit) const { return (flags & (1UL << bit)) != 0; }
    void set_flag(long bit, bool value) { unsigned long mask = 1UL << bit; if (value) flags |= mask; else flags &= ~mask; }
    long object_index;
    unsigned long flags;
    s_havok_contact_point_reference contact;
    long material;
};

typedef char character_physics_move_output_size[(sizeof(s_character_physics_move_output_datum) == 16) ? 1 : -1];

#endif // PHYSICS_CHARACTER_PHYSICS_MOVE_OUTPUT_H
