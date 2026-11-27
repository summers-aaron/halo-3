#ifndef NETWORKING_SESSION_NETWORK_SESSION_PARAMETER_VARIANT_TYPES_H
#define NETWORKING_SESSION_NETWORK_SESSION_PARAMETER_VARIANT_TYPES_H

#include "game/game_options.h"
#include <stddef.h>

// Chunked variant payloads shared by the session parameter owner and accessors.
// The owner constructors/codecs and network_session_parameters.h agree on these fields.
struct s_network_session_parameter_game_variant
{
    bool valid;
    c_game_variant game_variant;
};

struct s_network_session_parameter_map_variant
{
    bool valid;
    c_map_variant map_variant;
};

struct s_network_session_parameter_saved_film_game_options
{
    long length_in_ticks;
    long start_ticks;
    game_options options;
};

typedef char network_session_parameter_game_variant_size[(sizeof(s_network_session_parameter_game_variant) == 0x268) ? 1 : -1];
typedef char network_session_parameter_game_variant_offset[(offsetof(s_network_session_parameter_game_variant, game_variant) == 4) ? 1 : -1];
typedef char network_session_parameter_map_variant_size[(sizeof(s_network_session_parameter_map_variant) == 0xe098) ? 1 : -1];
typedef char network_session_parameter_map_variant_offset[(offsetof(s_network_session_parameter_map_variant, map_variant) == 8) ? 1 : -1];
typedef char network_session_parameter_saved_film_options_size[(sizeof(s_network_session_parameter_saved_film_game_options) == 0xf818) ? 1 : -1];
typedef char network_session_parameter_saved_film_options_offset[(offsetof(s_network_session_parameter_saved_film_game_options, options) == 8) ? 1 : -1];

template<typename t_type> struct s_network_session_parameter_chunked_capacity;
template<> struct s_network_session_parameter_chunked_capacity<s_network_session_parameter_game_variant> { enum { size = 6144 }; };
template<> struct s_network_session_parameter_chunked_capacity<s_network_session_parameter_map_variant> { enum { size = 28672 }; };
template<> struct s_network_session_parameter_chunked_capacity<s_network_session_parameter_saved_film_game_options> { enum { size = 34816 }; };

#endif // NETWORKING_SESSION_NETWORK_SESSION_PARAMETER_VARIANT_TYPES_H
