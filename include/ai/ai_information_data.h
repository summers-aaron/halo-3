#ifndef AI_AI_INFORMATION_DATA_H
#define AI_AI_INFORMATION_DATA_H

struct ai_information_data
{
    union
    {
        long values[6];
        struct
        {
            long type;
            long prop_index;
            long object_index;
        };
        // Type 1: an allegiance incident between two teams, passed to game_allegiance_incident.
        struct
        {
            long type;
            long prop_index;
            short team;
            short other_team;
            short incident;
        } allegiance_incident;
    };
};

typedef char ai_information_data_size_check[(sizeof(void *) != 4 || (sizeof(ai_information_data) == 24)) ? 1 : -1];

#endif // AI_AI_INFORMATION_DATA_H
