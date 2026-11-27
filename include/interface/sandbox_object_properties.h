#ifndef INTERFACE_SANDBOX_OBJECT_PROPERTIES_H
#define INTERFACE_SANDBOX_OBJECT_PROPERTIES_H

#include <stddef.h>
#include "tag_files/tag_types.h"
#include "cseries/cseries_wrapped.h"
#include "text_value_pair_types.h"

struct s_text_value_pair
{
    unsigned char flags;
    // March 83725c00/83725c40: assignment and conversion use the byte enum wrapper.
    c_enum<e_text_value_pair_parameter_type, char, 0, 3> type;
    short pad2;
    long value;
    long string_value;
    long display_string;
    long description_string;
};

struct s_object_property_definition
{
    long name;
    c_typed_tag_block<s_text_value_pair> values;
};

struct s_object_properties_tag_block_definition
{
    c_typed_tag_block<s_object_property_definition> properties;
};

#if defined(_M_PPCBE) || defined(_XBOX)
typedef char sandbox_text_value_pair_size[(sizeof(s_text_value_pair) == 20) ? 1 : -1];
typedef char sandbox_text_value_pair_integer_offset[(offsetof(s_text_value_pair, value) == 4) ? 1 : -1];
typedef char sandbox_object_property_size[(sizeof(s_object_property_definition) == 16) ? 1 : -1];
typedef char sandbox_object_property_values_offset[(offsetof(s_object_property_definition, values) == 4) ? 1 : -1];
typedef char sandbox_object_properties_size[(sizeof(s_object_properties_tag_block_definition) == 12) ? 1 : -1];
#endif

#endif // INTERFACE_SANDBOX_OBJECT_PROPERTIES_H
