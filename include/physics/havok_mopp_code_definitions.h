#ifndef PHYSICS_HAVOK_MOPP_CODE_DEFINITIONS_H
#define PHYSICS_HAVOK_MOPP_CODE_DEFINITIONS_H

// Functions defined in tu/blamlibxbox/havok_mopp_code_definitions.cpp that other units call.

#include "phys/phys_collide.h"
#include "tag_files/tag_types.h"

struct s_havok_mopp_code_definition
{
    phys_bvh_bytecode runtime_mopp_code;
    s_tag_block mopp_code;
};

typedef char havok_mopp_definition_size[(sizeof(void *) != 4 || sizeof(s_havok_mopp_code_definition) == 64) ? 1 : -1];

void havok_mopp_code_definitions_fixup(s_tag_block *mopp_codes);

#endif // PHYSICS_HAVOK_MOPP_CODE_DEFINITIONS_H
