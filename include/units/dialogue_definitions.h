#ifndef UNITS_DIALOGUE_DEFINITIONS_H
#define UNITS_DIALOGUE_DEFINITIONS_H

#include <stddef.h>
#include "tag_files/tag_types.h"

// ai_mission_dialogue reads the leading reference and flags;
// unit_dialogue reads the vocalizations block; AI dialogue reads the designator.
struct dialogue_definition
{
    s_tag_reference global_dialogue_info;
    unsigned long flags;
    s_tag_block vocalizations;
    long mission_dialogue_designator;
};

typedef char dialogue_definition_flags_offset[(offsetof(dialogue_definition, flags) == 0x10) ? 1 : -1];
typedef char dialogue_definition_vocalizations_offset[(offsetof(dialogue_definition, vocalizations) == 0x14) ? 1 : -1];
typedef char dialogue_definition_designator_offset[(offsetof(dialogue_definition, mission_dialogue_designator) == 0x20) ? 1 : -1];

#endif // UNITS_DIALOGUE_DEFINITIONS_H
