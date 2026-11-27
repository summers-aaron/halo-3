#ifndef GAME_CAMPAIGN_METAGAME_DEFINITIONS_H
#define GAME_CAMPAIGN_METAGAME_DEFINITIONS_H
#include <stddef.h>
#include "tag_files/tag_types.h"

// Four multiplier blocks followed by the kill penalties and scoring window.
struct s_campaign_metagame_definition
{
    c_typed_tag_block<float> style_multipliers;
    c_typed_tag_block<float> player_count_multipliers;
    c_typed_tag_block<float> primary_skull_multipliers;
    c_typed_tag_block<float> secondary_skull_multipliers;
    long reserved30;
    long suicide_points;
    long betrayal_points;
    long minimum_kills;
    float window_seconds;
};
typedef char campaign_metagame_definition_size[(sizeof(s_campaign_metagame_definition) == 0x44) ? 1 : -1];
typedef char campaign_metagame_suicide_offset[(offsetof(s_campaign_metagame_definition, suicide_points) == 0x34) ? 1 : -1];
typedef char campaign_metagame_window_offset[(offsetof(s_campaign_metagame_definition, window_seconds) == 0x40) ? 1 : -1];
// Scenario time multipliers and the score needed for the map achievement.
struct s_campaign_metagame_time_entry
{
    float time;
    float value;
};
struct s_campaign_metagame_time_curve
{
    c_typed_tag_block<s_campaign_metagame_time_entry> entries;
    float score_threshold;
};
typedef char campaign_metagame_time_entry_size[(sizeof(s_campaign_metagame_time_entry) == 8) ? 1 : -1];
typedef char campaign_metagame_time_threshold_offset[(offsetof(s_campaign_metagame_time_curve, score_threshold) == 0xc) ? 1 : -1];
typedef char campaign_metagame_time_curve_size[(sizeof(s_campaign_metagame_time_curve) == 0x10) ? 1 : -1];
#endif
