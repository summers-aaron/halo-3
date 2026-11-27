#ifndef SAVED_GAMES_GAME_STATE_HEADER_TYPES_H
#define SAVED_GAMES_GAME_STATE_HEADER_TYPES_H

#include "game/game_options.h"
#include "networking/logic/storage/network_http_request_hash.h"
#include "scenario/scenario_game_state.h"

// The hash occupies 20 bytes; the containing header also has the natural
// four bytes of padding required by this aligned storage view.
struct game_state_digest_payload {
    __int64 parts[2];
    long tail;
};
struct s_game_state_scenario {
    s_scenario_game_state state;
    // Bytes 0x1c-0x2b are named by no March game_state assert and read by no in-tree scenario consumer.
    unsigned long : 32;
    unsigned long : 32;
    unsigned long : 32;
    unsigned long : 32;
    unsigned char hash_salt[20];
    union {
        s_network_http_request_hash digest;
        game_state_digest_payload digest_payload;
    };
};
struct s_game_state_header_layout {
    long size;
    unsigned long base_address;     // game_state_globals.base_address when the state was written
    char scenario_name[256];
    char build[32];
    unsigned long checksum;
    game_options game_options;
    s_game_state_scenario scenario;
    __forceinline s_game_state_header_layout() {}
};

// Persistent saves share the live header prefix and embedded game options.
struct s_saved_game_header : s_game_state_header_layout {};
struct game_state_header : s_game_state_header_layout {};

typedef char saved_game_header_players_offset[(offsetof(game_state_header, game_options) + offsetof(game_options, players) == 0xe6c0) ? 1 : -1];

typedef char game_state_header_size_check[(sizeof(s_game_state_header_layout) == 0xf998) ? 1 : -1];
typedef char game_state_header_options_offset[(offsetof(s_game_state_header_layout, game_options) == 0x130) ? 1 : -1];
typedef char game_state_header_scenario_offset[(offsetof(s_game_state_header_layout, scenario) == 0xf940) ? 1 : -1];
typedef char game_state_header_digest_offset[
    (offsetof(s_game_state_header_layout, scenario) + offsetof(s_game_state_scenario, digest_payload) == 0xf980) ? 1 : -1];

typedef char game_state_scenario_state_offset[(offsetof(s_game_state_scenario, state) == 0) ? 1 : -1];
typedef char game_state_scenario_salt_offset[(offsetof(s_game_state_scenario, hash_salt) == 0x2c) ? 1 : -1];

#endif
