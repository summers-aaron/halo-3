#ifndef GAME_GAME_RESULTS_STATISTIC_TYPES_H
#define GAME_GAME_RESULTS_STATISTIC_TYPES_H

#include <stddef.h>

// The result tables use a 16-byte record (g_game_results_statistic_definitions).
// March game_results_statistics_encode names the bounds and encoding width;
// the bounds are unsigned halfwords and the encoding width is a word.
struct s_integer_statistic_definition
{
    const char *name;
    long pad4;
    unsigned short minimum;
    unsigned short maximum;
    long bit_count;
};

typedef char integer_statistic_definition_size[(sizeof(void *) != 4 || ((sizeof(s_integer_statistic_definition) == 0x10) )) ? 1 : -1];
typedef char integer_statistic_definition_minimum_offset[(sizeof(void *) != 4 || ((offsetof(s_integer_statistic_definition, minimum) == 8) )) ? 1 : -1];
typedef char integer_statistic_definition_maximum_offset[(sizeof(void *) != 4 || ((offsetof(s_integer_statistic_definition, maximum) == 10) )) ? 1 : -1];
typedef char integer_statistic_definition_bits_offset[(sizeof(void *) != 4 || ((offsetof(s_integer_statistic_definition, bit_count) == 12) )) ? 1 : -1];

#endif // GAME_GAME_RESULTS_STATISTIC_TYPES_H
