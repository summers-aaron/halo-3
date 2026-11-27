#ifndef ANIMATIONS_SUSPENSION_ANIMATION_ITERATOR_H
#define ANIMATIONS_SUSPENSION_ANIMATION_ITERATOR_H

#include "cseries/cseries.h"
#include "animations/animation_id.h"

class suspension_animation_iterator {
public:
    long marker;
    long region;
    float extension, compression, rate;
    float scale, offset, limit;
    long suspension_index;
    c_animation_id animation;
    short unknown, cursor;
    __forceinline suspension_animation_iterator() : marker(NONE), extension(0.0f), compression(0.0f), rate(0.0f),
        suspension_index(NONE), unknown(NONE), cursor(NONE) {}
};

#endif
