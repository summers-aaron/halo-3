#ifndef SIMULATION_GAME_INTERFACE_PACKED_VALUE_TYPES_H
#define SIMULATION_GAME_INTERFACE_PACKED_VALUE_TYPES_H

// One word shared with s_simulation_event_requirements: the console allocates bit-fields from the most
// significant bit, the PC compilers from the least, so the PC declaration runs in reverse.
struct s_packed_value
{
#if defined(_XBOX) && !defined(__clang__)
    unsigned int kind : 9;
    unsigned int payload : 23;
#else
    unsigned int payload : 23;
    unsigned int kind : 9;
#endif
};

#endif // SIMULATION_GAME_INTERFACE_PACKED_VALUE_TYPES_H
