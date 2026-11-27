#ifndef INCLUDE_PHYS_SHAPE_TYPE_H
#define INCLUDE_PHYS_SHAPE_TYPE_H

// Functions defined in tu/hkcollide/hkShapeType.cpp that other units call.

class phys_dispatcher;

void phys_register_alt_shape_types(phys_dispatcher *dispatcher);

#include "phys/phys_shape_type_enum.h"
const char *phys_get_shape_type_name(phys_shape_type type);

#endif // HAVOK_HK_SHAPE_TYPE_H
