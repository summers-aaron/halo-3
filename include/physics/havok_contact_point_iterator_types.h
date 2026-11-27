#ifndef PHYSICS_HAVOK_CONTACT_POINT_ITERATOR_TYPES_H
#define PHYSICS_HAVOK_CONTACT_POINT_ITERATOR_TYPES_H

#include "objects/object_header.h"
#include "physics/havok_component.h"

class c_havok_contact_point;
struct s_havok_contact_point_reference;

class c_havok_contact_point_iterator
{
public:
    // March havok_contact_points.cpp keeps the constructor and get_contact_point out of line (82c27530,
    // The constructor and contact-point accessor are inlined at every call site.
    __forceinline c_havok_contact_point_iterator(long object_index, bool unfiltered);

    void next();
    bool valid();

    // March 82c279b0 returns the adjacent rigid-body/contact-point indices at +8.
    __forceinline const s_havok_contact_point_reference *get_reference() const;

    __forceinline c_havok_contact_point *get_contact_point();

    __forceinline c_havok_contact_point *get_contact_point_through_reference();

    bool unfiltered;
    long object_index;
    short rigid_body_index;
    short contact_point_index;
};

typedef char havok_contact_point_iterator_size[(sizeof(c_havok_contact_point_iterator) == 12) ? 1 : -1];

#endif
