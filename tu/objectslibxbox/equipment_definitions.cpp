// objectslibxbox/equipment_definitions.obj: from the records 0xe5898
// @compiler xdk6021
// @flags /TP /O2 /Oc
// @address 0xe5898 ?equipment_definition_get_type@@YA?AW4e_equipment_type@@JJ@Z

#include "tag_files/tag_groups.h"
#include "items/equipment_definitions.h"

e_equipment_type equipment_definition_get_type(long tag_index, long which)
{
    long order[9];
    long result = -1;
    long found;
    long *p;
    s_equipment_definition *definition = TAG_GET('eqip', s_equipment_definition, tag_index);
    order[0] = 0; order[1] = 1; order[2] = 2; order[3] = 3; order[4] = 4; order[5] = 5;
    p = order;
    found = 0;
    p[6] = 6; p[7] = 7; p[8] = 8;
    for (unsigned int i = 0; i < 9; i++, p++) {
        if (definition->equipment_types[*p].count != 0) {
            if (found == which) {
                result = order[i];
                break;
            }
            found++;
        }
    }
    return static_cast<e_equipment_type>(result);
}
