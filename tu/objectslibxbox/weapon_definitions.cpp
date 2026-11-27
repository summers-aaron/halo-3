// objectslibxbox/weapon_definitions.obj: from the records 0x11a000
// @compiler xdk6130
// @flags /TP /O2 /Oc
#include "tag_files/tag_groups.h"
#include "items/weapon_definitions.h"
#include "objects/weapon_first_person_interface_definition_types.h"

const weapon_first_person_interface_definition *weapon_definition_get_first_person_interface_by_character(
    const weapon_definition *definition, long character_type)
{
    const weapon_first_person_interface_definition *result = 0;
    long interface_index = 0;
    switch (character_type)
    {
    case 0:
    case 2:
        interface_index = 0;
        break;
    case 1:
    case 3:
    case 4:
        interface_index = 1;
        break;
    default:
        break;
    }

    if (interface_index >= 0 && interface_index < definition->first_person_interfaces.count)
        result = (const weapon_first_person_interface_definition *)tag_block_get_element_with_size(
            &definition->first_person_interfaces, interface_index,
            sizeof(weapon_first_person_interface_definition));
    else if (definition->first_person_interfaces.count > 0)
        result = (const weapon_first_person_interface_definition *)tag_block_get_element_with_size(
            &definition->first_person_interfaces, 0,
            sizeof(weapon_first_person_interface_definition));
    return result;
}
