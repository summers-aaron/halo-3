#ifndef GAME_GAME_RESULTS_STATISTIC_SHARED_TYPES_H
#define GAME_GAME_RESULTS_STATISTIC_SHARED_TYPES_H

// One statistic as a halfword: the value in the high 15 bits, its flag in bit 0.
// The packet encoder reads the whole halfword (packed) and splits it itself.
struct s_game_results_statistic
{
    union
    {
        struct
        {
            // flag is bit 0 and value the high 15 bits: MSB-first on the console, LSB-first on the PC.
#if defined(_XBOX) && !defined(__clang__)
            unsigned short value : 15;
            unsigned short flag : 1;
#else
            unsigned short flag : 1;
            unsigned short value : 15;
#endif
        };
        unsigned short packed;
    };
};

#endif // GAME_GAME_RESULTS_STATISTIC_SHARED_TYPES_H
