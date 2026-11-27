#ifndef INTERFACE_TEXT_VALUE_PAIR_TYPES_H
#define INTERFACE_TEXT_VALUE_PAIR_TYPES_H

#include <stddef.h>
#include "tag_files/tag_types.h"

// March's c_game_variant accessors and text-value-pair readers use this enum.
enum e_text_value_pair_parameter_type
{
    _text_value_pair_parameter_invalid = -1,
    _text_value_pair_parameter_integer = 0,
    _text_value_pair_parameter_string_id = 1,
    _text_value_pair_parameter_real = 2
};

// The settings tag identifies a parameter and its display strings before the
// values block.
struct s_text_value_pair_definition
{
    long parameter;
    long name;
    long description;
    s_tag_block values;
};

#if defined(_M_PPCBE) || defined(_XBOX)
typedef char text_value_pair_definition_size[(sizeof(s_text_value_pair_definition) == 0x18) ? 1 : -1];
typedef char text_value_pair_definition_values_offset[(offsetof(s_text_value_pair_definition, values) == 0xc) ? 1 : -1];
#endif

#endif // INTERFACE_TEXT_VALUE_PAIR_TYPES_H
