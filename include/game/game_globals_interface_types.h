#ifndef GAME_GAME_GLOBALS_INTERFACE_TYPES_H
#define GAME_GAME_GLOBALS_INTERFACE_TYPES_H

#include "tag_files/tag_types.h"
#include <stddef.h>

// Element of s_game_globals::interface_tags. March interface_get_tag_index reads the
// leading tag references as an array indexed by interface tag.
struct s_game_globals_interface_tags
{
    s_tag_reference interface_tags[14];
    s_tag_reference mainmenu;
    s_tag_reference campaign;
    s_tag_reference multiplayer;
    s_tag_reference chud_globals;
};

typedef char game_globals_interface_tags_mainmenu_offset[(offsetof(s_game_globals_interface_tags, mainmenu) == 0xe0) ? 1 : -1];
typedef char game_globals_interface_tags_chud_globals_offset[(offsetof(s_game_globals_interface_tags, chud_globals) == 0x110) ? 1 : -1];

#endif
