#ifndef GAME_PLAYER_CONFIGURATION_H
#define GAME_PLAYER_CONFIGURATION_H

#include <string.h>
#include <stddef.h>

#include "game/player_identifier_types.h"

struct s_emblem_info {
    unsigned char foreground;
    unsigned char background;
    unsigned char flags;
    signed char primary_color;
    signed char secondary_color;
    signed char background_color;
    unsigned short pad;
};

struct s_player_appearance {
    union {
        unsigned char data[30];
        struct {
            unsigned char flags;
            union {
                signed char change_colors[3];
                struct {
                    signed char primary_color;
                    signed char secondary_color;
                    signed char tertiary_color;
                };
            };
            union {
                signed char player_model_choice;
                signed char model_choice;
            };
            union {
                s_emblem_info emblem;
                struct {
                    unsigned char foreground_emblem;
                    unsigned char background_emblem;
                    unsigned char emblem_flags;
                    signed char emblem_primary_color;
                    signed char emblem_secondary_color;
                    signed char emblem_background_color;
                    unsigned short emblem_pad;
                };
            };
            union {
                unsigned char model_permutations[2][4];
                signed char selections[2][4];
            };
            wchar_t service_tag[4];
        };
    };
};

// match: 0x1b8750: the typed helper preserves the out-of-line appearance clear.
static __forceinline void player_appearance_initialize(s_player_appearance *appearance)
{
    memset(appearance, 0, sizeof(*appearance));
}

typedef s_player_appearance s_player_appearance_header;
struct s_queried_player_global_statistics {
    bool valid;
    long experience_base;
    long experience_penalty;
    long highest_skill;
};
struct s_queried_player_hopper_statistics {
    bool valid;
    unsigned short hopper_identifier;
    float mu;
    float sigma;
    long old_skill;
    long games_played;
    long games_completed;
    long games_won;
};
struct s_calculated_player_statistics {
    bool global_valid;
    long experience;
    long experience_rank;
    long experience_grade;
    bool hopper_valid;
    long skill_level;
    long displayed_skill_level;
    long skill_update_weight;
};

struct s_player_displayed_statistics {
    bool valid;
    long ranked_played;
    long ranked_completed;
    long ranked_won;
    long unranked_played;
    long unranked_completed;
    long unranked_won;
    long custom_completed;
    long custom_won;
    long first_played;
    long last_played;
};
typedef char player_types_size_displayed_statistics[(sizeof(s_player_displayed_statistics) == 0x2c) ? 1 : -1];
typedef char player_types_size_global_statistics[(sizeof(s_queried_player_global_statistics) == 0x10) ? 1 : -1];
typedef char player_types_size_hopper_statistics[(sizeof(s_queried_player_hopper_statistics) == 0x1c) ? 1 : -1];
typedef char player_types_size_calculated_statistics[(sizeof(s_calculated_player_statistics) == 0x20) ? 1 : -1];

struct s_player_configuration_from_client {
    s_player_configuration_from_client() { memset(this, 0, sizeof(*this)); }

    union {
        unsigned char data[200];
        struct {
            union {
                wchar_t desired_name[16];
                wchar_t name[16];
            };
            s_player_appearance appearance;
            unsigned __int64 xuid;
            bool online;
            bool silver;
            bool is_content_creator;
            signed char desired_team_index;
            bool ready;
            bool desires_rematch;
            unsigned char hopper_access_flags;
            bool is_free_live_gold_account;
            bool guest;
            bool bot;
            bool is_griefer;
            signed char bungienet_user_role;
            long bungienet_user;
            long gamer_region;
            long gamer_zone;
            unsigned long cheat_flags;
            unsigned long player_flags;
            long repeated_play_coefficient;
            bool experience_growth_banned;
            s_queried_player_global_statistics global_statistics;
            s_player_displayed_statistics displayed_statistics;
            s_queried_player_hopper_statistics hopper_statistics;
        };
    };
};

struct s_player_matchmaking_properties {
    bool hopper_stats_valid;
    long hopper_skill;
    long hopper_skill_display;
    long hopper_weight;
};

struct s_player_configuration_from_host {
    s_player_configuration_from_host()
    {
        memset(this, 0, sizeof(*this));
        user_team_index = -1;
        team_index = -1;
    }

    union {
        unsigned char data[72];
        struct {
            union {
                wchar_t name[16];
                wchar_t player_name[16];
            };
            union {
                long team_index;
                long team;
                long default_e8;
                long results_team_index;
            };
            union {
                long user_team_index;
                long default_ec;
            };
            union {
                struct {
            union {
                bool global_stats_valid;
                bool game_team_override;
            };
            union {
                long global_experience;
                long game_team_index;
            };
            union {
                long global_rank;
                long rank;
            };
            union {
                long global_grade;
                long grade;
            };
            union {
                struct {
                    union {
                        bool hopper_stats_valid;
                        bool team_override;
                    };
                    long hopper_skill;
                    long hopper_skill_display;
                    long hopper_weight;
                };
                s_player_matchmaking_properties matchmaking;
            };
                };
                s_calculated_player_statistics calculated_statistics;
            };
        };
    };
};

struct s_player_configuration {
    s_player_configuration_from_client client;
    s_player_configuration_from_host host;
};

typedef char player_configuration_client_size[(sizeof(s_player_configuration_from_client) == 0xc8) ? 1 : -1];
typedef char player_configuration_host_size[(sizeof(s_player_configuration_from_host) == 0x48) ? 1 : -1];
typedef char player_configuration_size[(sizeof(s_player_configuration) == 0x110) ? 1 : -1];
typedef char player_configuration_global_statistics_offset[
    (offsetof(s_player_configuration_from_client, global_statistics) == 0x70) ? 1 : -1];
typedef char player_configuration_displayed_statistics_offset[
    (offsetof(s_player_configuration_from_client, displayed_statistics) == 0x80) ? 1 : -1];
typedef char player_configuration_hopper_statistics_offset[
    (offsetof(s_player_configuration_from_client, hopper_statistics) == 0xac) ? 1 : -1];

#endif
