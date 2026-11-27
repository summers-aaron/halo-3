#ifndef INTERFACE_GUI_PREGAME_SETUP_TYPES_H
#define INTERFACE_GUI_PREGAME_SETUP_TYPES_H

#include "game/game_options.h"
#include "game/player_types.h"
#include "saved_games/saved_film_description.h"

struct s_campaign_setup_data {
    unsigned char valid;
    long campaign_id;
    long map_id;
    long difficulty;
};

struct s_matchmaking_setup_data {
    unsigned char valid;
    unsigned short hopper;
};

struct s_custom_setup_data {
    unsigned char valid;
    c_game_variant variant;
    unsigned char selection_valid;
    long selection;
    s_player_identifier variant_owner;
    char variant_name[256];
    char map_variant_data[0xe408 - 0x378];
    unsigned char selection1_valid;
    long selection1;
    s_player_identifier map_owner;
    char map_name[256];
};

struct s_mapeditor_setup_data {
    unsigned char valid;
    long location;
    char map_variant_data[0xe098 - 8];
    unsigned char selection_valid;
    long selection;
    s_player_identifier map_owner;
    char map_name[256];
};

struct s_theater_setup_data {
    unsigned char valid;
    long location;
    s_player_identifier owner;
    s_saved_film_description film;
    char options_storage[sizeof(game_options)];
    long game_engine_index;
    long game_variant_index;
};

union u_pregame_setup_data {
    s_campaign_setup_data campaign;
    s_matchmaking_setup_data matchmaking;
    char custom_storage[sizeof(s_custom_setup_data)];
    __forceinline s_custom_setup_data &custom() { return *(s_custom_setup_data *)custom_storage; }
    s_mapeditor_setup_data mapeditor;
    s_theater_setup_data theater;
};

struct s_pregame_setup_params {
    long mode;
    u_pregame_setup_data data;
};

typedef char pregame_setup_params_size[(sizeof(s_pregame_setup_params) == 0xfa44) ? 1 : -1];

#endif
