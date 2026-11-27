#ifndef AI_PROP_VIEW_INLINE_H
#define AI_PROP_VIEW_INLINE_H
#include "ai/prop_datum.h"
#include "ai/prop_ref_datum.h"
#include "memory/data.h"
#include "memory/tls_globals.h"

static __forceinline prop_view *prop_view_get(const prop_ref_datum *ref)
{
    prop_view *result = 0;
    long prop_index = ref->view_index;
    if (prop_index != NONE) {
        tracking_datum *prop = datum_get<tracking_datum>(tls_prop_data(), prop_index);
        if (prop)
            result = &prop->tracking_view;
    }
    return result;
}
#endif
