#ifndef OBJECTS_WIDGETS_WIDGET_TYPES_H
#define OBJECTS_WIDGETS_WIDGET_TYPES_H

#include <stddef.h>
#include "objects/widgets/widget_type.h"
#include "objects/object_reference_types.h"

// March widget_get_type_data sign-extends the type at +2.
struct s_widget_datum
{
    short identifier;
    short type;
    long type_data;
    long next_widget;
};

extern "C" { extern s_widget_type g_widget_types[]; }

static __forceinline s_widget_type *widget_type_table() { return g_widget_types; }

typedef char widget_datum_size[(sizeof(s_widget_datum) == 0xc) ? 1 : -1];
typedef char widget_type_transfer_offset[(sizeof(void *) != 4 || offsetof(s_widget_type, transfer) == 0x20) ? 1 : -1];
typedef char widget_type_render_unattached_offset[(sizeof(void *) != 4 || offsetof(s_widget_type, render_unattached) == 0x3c) ? 1 : -1];

#endif // OBJECTS_WIDGETS_WIDGET_TYPES_H
