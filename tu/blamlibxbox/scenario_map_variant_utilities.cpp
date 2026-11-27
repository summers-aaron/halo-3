// blamlibxbox/scenario_map_variant_utilities.obj: from the records 0x580128 0x580270 0x580338 0x5803f8
// @compiler xdk6130
// @flags /TP /O2 /Oc

#include "interface/user_interface_multiplayer_game_settings_group.h"
#include "scenario/scenario_map_variant_utilities.h"
#include "interface/sandbox_object_properties.h"


long map_variant_value_pair_get_value_index(long property_name, long search_value)
{
    long result = -1;
    const s_object_properties_tag_block_definition *properties = get_sandbox_object_property_values();
    if (properties)
    {
        for (short property_index = 0; property_index < properties->properties.count(); ++property_index)
        {
            const s_object_property_definition *property = &properties->properties[property_index];
            if (property->name == property_name)
            {
                for (long value_index = 0; value_index < property->values.count(); ++value_index)
                {
                    const s_text_value_pair *value = &property->values[value_index];
                    if (value && value->value == search_value)
                    {
                        result = value_index;
                        break;
                    }
                }
                break;
            }
        }
        if (result == -1)
        {
            long closest_value = -1;
            for (short property_index = 0; property_index < properties->properties.count(); ++property_index)
            {
                const s_object_property_definition *property = &properties->properties[property_index];
                if (property->name == property_name)
                {
                    for (long value_index = 0; value_index < property->values.count(); ++value_index)
                    {
                        const s_text_value_pair *value = &property->values[value_index];
                        if (value && value->value < search_value && value->value > closest_value)
                        {
                            closest_value = value->value;
                            result = value_index;
                        }
                    }
                    break;
                }
            }
        }
    }
    return result;
}

long map_variant_value_pair_get_value_index(long property_name, float search_value)
{
    long result = -1;
    const s_object_properties_tag_block_definition *properties = get_sandbox_object_property_values();
    if (properties)
    {
        for (short property_index = 0; property_index < properties->properties.count(); ++property_index)
        {
            const s_object_property_definition *property = &properties->properties[property_index];
            if (property->name == property_name && property->values.count() > 1)
            {
                const s_text_value_pair *value = &property->values[0];
                float min_value = (float)value->value;
                value = &property->values[1];
                float max_value = (float)value->value;
                result = (long)((search_value - min_value) / 0.5f);
                break;
            }
        }
    }
    return result;
}

float map_variant_value_pair_get_real(long property_name, long value_index)
{
    float result = 0.0f;
    if (value_index != -1)
    {
        const s_object_properties_tag_block_definition *properties = get_sandbox_object_property_values();
        if (properties)
        {
            for (short property_index = 0; property_index < properties->properties.count(); ++property_index)
            {
                const s_object_property_definition *property = &properties->properties[property_index];
                if (property->name == property_name)
                {
                    const s_text_value_pair *value = &property->values[0];
                    float min_value = (float)value->value;
                    result = min_value + 0.5f * (float)value_index;
                    break;
                }
            }
        }
    }
    return result;
}

long map_variant_value_pair_get_integer(long property_name, long value_index)
{
    long result = -1;
    if (value_index != -1)
    {
        const s_object_properties_tag_block_definition *properties = get_sandbox_object_property_values();
        if (properties)
        {
            for (short property_index = 0; property_index < properties->properties.count(); ++property_index)
            {
                const s_object_property_definition *property = &properties->properties[property_index];
                if (property->name == property_name)
                {
                    const s_text_value_pair *value = &property->values[value_index];
                    if (value)
                    {
                        result = value->value;
                        break;
                    }
                }
            }
        }
    }
    return result;
}
