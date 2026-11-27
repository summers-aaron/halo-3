#ifndef INCLUDE_PHYS_CRITICAL_SECTION_ENTRY_H
#define INCLUDE_PHYS_CRITICAL_SECTION_ENTRY_H

#include "phys/phys_critical_section.h"

static __forceinline void havok_critical_section_enter(phys_critical_section *critical_section)
{
    critical_section->enter();
}

#endif
