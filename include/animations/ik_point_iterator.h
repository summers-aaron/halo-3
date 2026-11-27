#ifndef ANIMATIONS_IK_POINT_ITERATOR_H
#define ANIMATIONS_IK_POINT_ITERATOR_H
#include "cseries/cseries.h"

class ik_point_iterator
{
public:
    __forceinline ik_point_iterator()
    {
        marker = NONE;
        attach_to_marker = NONE;
        index = NONE;
        mode = 0;
        mode_name = NONE;
        weapon_class_name = NONE;
    }
    long marker;
    long attach_to_marker;
    short index;
    short mode;
    long mode_name;
    long weapon_class_name;
};

typedef char ik_point_iterator_size[(sizeof(ik_point_iterator) == 0x14) ? 1 : -1];

#endif
