#ifndef INTERFACE_MULTIPLAYER_VARIANT_SETTINGS_INTERFACE_DEFINITION_H
#define INTERFACE_MULTIPLAYER_VARIANT_SETTINGS_INTERFACE_DEFINITION_H

#include <stddef.h>
#include "tag_files/tag_types.h"

// March's recursive settings readers name the interface/edit-reference types.
// Each option holds its category, display strings, flags, and parameter.
struct s_variant_setting_option
{
    s_tag_reference category;
    s_tag_reference template_category;
    long flags;
    long name;
    long description;
    s_tag_reference parameter;
};

struct s_variant_setting_edit_reference
{
    long name;
    long category;
    s_tag_block options;
};

struct s_multiplayer_variant_settings_interface_definition
{
    long name;
    s_tag_block edit_references;
};

#if defined(_M_PPCBE) || defined(_XBOX)
typedef char variant_setting_option_size[(sizeof(s_variant_setting_option) == 0x3c) ? 1 : -1];
typedef char variant_setting_option_parameter_offset[(offsetof(s_variant_setting_option, parameter) == 0x2c) ? 1 : -1];
typedef char variant_setting_edit_reference_size[(sizeof(s_variant_setting_edit_reference) == 0x14) ? 1 : -1];
typedef char multiplayer_variant_settings_interface_size[(sizeof(s_multiplayer_variant_settings_interface_definition) == 0x10) ? 1 : -1];
#endif

#endif // INTERFACE_MULTIPLAYER_VARIANT_SETTINGS_INTERFACE_DEFINITION_H
