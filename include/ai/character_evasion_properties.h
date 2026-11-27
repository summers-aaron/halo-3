#ifndef AI_CHARACTER_EVASION_PROPERTIES_H
#define AI_CHARACTER_EVASION_PROPERTIES_H
#include <stddef.h>

// Character tag block with evasion behavior settings.
struct character_evasion_properties
{
    float threshold;
    float cooldown;
    float chance;
    float range;
    float dive_retreat_chance;
};
typedef char character_evasion_properties_size[(sizeof(character_evasion_properties) == 20) ? 1 : -1];
typedef char character_evasion_properties_retreat_offset[(offsetof(character_evasion_properties, dive_retreat_chance) == 16) ? 1 : -1];
#endif
