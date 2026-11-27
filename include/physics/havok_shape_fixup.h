#ifndef PHYSICS_HAVOK_SHAPE_FIXUP_H
#define PHYSICS_HAVOK_SHAPE_FIXUP_H

#include "phys/phys_collide.h"

static __forceinline void initialize_tag_shape_reference_count_and_user_data(phys_shape *shape, phys_ulong user_data)
{
    shape->m_object_ref_count = 128;
    shape->m_user_data = user_data;
    shape->m_mem_size_and_flags = 0;
}

#endif
