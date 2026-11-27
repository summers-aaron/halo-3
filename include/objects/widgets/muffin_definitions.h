#ifndef OBJECTS_WIDGETS_MUFFIN_DEFINITIONS_H
#define OBJECTS_WIDGETS_MUFFIN_DEFINITIONS_H

#include <stddef.h>
#include "tag_files/tag_types.h"
#include "math/function_definitions.h"

// The 'mufn' tag. Field names come from its two readers: muffins.cpp (creation and transfer)
// and havok_component.cpp (havok_muffin_update, the spring that animates each piece).

// One marker the muffin may grow a piece on.
struct s_muffin_entry
{
    long marker_name;
    unsigned short probability;
    unsigned short piece_mask;
};

struct s_muffin_definition_piece
{
    short permutation_index;
    unsigned short flags;
    float minimum_delay;
    float maximum_delay;
    float minimum_scale;
    float maximum_scale;
    c_function_definition lifetime_curve;
    float curve_duration;
    c_function_definition scale_curve;
    float blend;
    float distance;
    float acceleration;
    float damping;
    float maximum_velocity;
    float minimum_lifetime;
    float maximum_lifetime;
    float immortality;
    s_tag_reference destroyed_effect;
};

struct s_muffin_definition
{
    s_tag_reference render_model;
    char spare_before_entries[0x20 - 0x10];
    s_tag_block entries;
    long piece_count;
    TAG_POINTER(s_muffin_definition_piece) pieces;
    TAG_POINTER(void) pieces_definition;
};

#if defined(_M_PPCBE) || defined(_XBOX)
typedef char muffin_definition_piece_size[(sizeof(s_muffin_definition_piece) == 0x70) ? 1 : -1];
typedef char muffin_definition_piece_curve_offset[(offsetof(s_muffin_definition_piece, curve_duration) == 0x28) ? 1 : -1];
typedef char muffin_definition_piece_immortality_offset[(offsetof(s_muffin_definition_piece, immortality) == 0x5c) ? 1 : -1];
typedef char muffin_definition_entries_offset[(offsetof(s_muffin_definition, entries) == 0x20) ? 1 : -1];
#endif

#endif // OBJECTS_WIDGETS_MUFFIN_DEFINITIONS_H
