#ifndef PHYSICS_HAVOK_CONTACT_POINT_ITERATOR_INLINES_H
#define PHYSICS_HAVOK_CONTACT_POINT_ITERATOR_INLINES_H

#include "physics/havok_component_datum_access.h"

// Include after the complete component and iterator definitions.
__forceinline c_havok_contact_point_iterator::c_havok_contact_point_iterator(long object_index, bool unfiltered)
    {
        rigid_body_index = 0;
        contact_point_index = NONE;
        this->object_index = object_index;
        this->unfiltered = unfiltered;
        next();
    }

__forceinline const s_havok_contact_point_reference *c_havok_contact_point_iterator::get_reference() const
    {
        return (const s_havok_contact_point_reference *)&rigid_body_index;
    }

__forceinline c_havok_contact_point *c_havok_contact_point_iterator::get_contact_point()
    {
        object_datum *object = (object_datum *)object_get_and_verify_type(object_index, NONE);
        c_havok_component *component = havok_component_get(object->havok_component_index);
        return component->get_contact_point(rigid_body_index, contact_point_index);
    }

__forceinline c_havok_contact_point *c_havok_contact_point_iterator::get_contact_point_through_reference()
    {
        object_datum *object = (object_datum *)object_get_and_verify_type(object_index, 0xffffffff);
        c_havok_component *component = havok_component_get(object->havok_component_index);
        return component->get_contact_point(get_reference());
    }

#endif
