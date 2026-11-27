#ifndef NETWORKING_LOGIC_STORAGE_DYNAMIC_MATCHMAKING_HOPPER_STATISTICS_TYPES_H
#define NETWORKING_LOGIC_STORAGE_DYNAMIC_MATCHMAKING_HOPPER_STATISTICS_TYPES_H

// dynamic_hopper_statistics.bin: the matchmaking reader and both lobby screens
// agree on the population table and the remaining downloaded payload.
struct s_dynamic_matchmaking_hopper_statistics
{
    long total_population;
    struct s_hopper_population_entry
    {
        long hopper_id;
        long hopper_population;
    } hoppers[32];
    char remaining_statistics[0x9a3 - 4 - 32 * 8];
};

#if defined(_M_PPCBE) || defined(_XBOX)
typedef char dynamic_hopper_statistics_size[(sizeof(s_dynamic_matchmaking_hopper_statistics) == 0x9a4) ? 1 : -1];
#endif

#endif // NETWORKING_LOGIC_STORAGE_DYNAMIC_MATCHMAKING_HOPPER_STATISTICS_TYPES_H
