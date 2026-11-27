#ifndef AI_CHARACTER_IDLE_PROPERTIES_H
#define AI_CHARACTER_IDLE_PROPERTIES_H
#include <stddef.h>

// Character idle behavior: flags plus the minimum/maximum delay range.
struct character_idle_properties
{
    unsigned long flags;
    float minimum_delay;
    float maximum_delay;
};
typedef char character_idle_properties_size[(sizeof(character_idle_properties) == 12) ? 1 : -1];
typedef char character_idle_properties_delay_offset[(offsetof(character_idle_properties, minimum_delay) == 4) ? 1 : -1];
#endif
