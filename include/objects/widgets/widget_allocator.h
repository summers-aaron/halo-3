#ifndef OBJECTS_WIDGETS_WIDGET_ALLOCATOR_H
#define OBJECTS_WIDGETS_WIDGET_ALLOCATOR_H

#include "cache/restricted_memory_regions.h"
#include "objects/widgets/widgets.h"

extern t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_widget_data_allocator> g_widget_data_allocator;

#endif // OBJECTS_WIDGETS_WIDGET_ALLOCATOR_H
