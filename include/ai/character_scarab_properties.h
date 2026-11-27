#ifndef AI_CHARACTER_SCARAB_PROPERTIES_H
#define AI_CHARACTER_SCARAB_PROPERTIES_H

// March character_definition_scarab_properties_get selects a 24-byte element.
// actor_type_scarab reads the first two distances; giant steering reads +8..+20.
struct character_scarab_properties
{
    float minimum_distance;
    float maximum_distance;
    float near_distance;
    float far_distance;
    float direct_steering_limit;
    float indirect_steering_limit;
};
typedef char character_scarab_properties_size[(sizeof(character_scarab_properties) == 24) ? 1 : -1];

#endif
