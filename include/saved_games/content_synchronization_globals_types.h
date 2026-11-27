#ifndef SAVED_GAMES_CONTENT_SYNCHRONIZATION_GLOBALS_TYPES_H
#define SAVED_GAMES_CONTENT_SYNCHRONIZATION_GLOBALS_TYPES_H

#include "saved_games/content_critical_section_types.h"

struct s_content_synchronization_globals {
    s_content_critical_section critical_sections[22];
};

#endif // SAVED_GAMES_CONTENT_SYNCHRONIZATION_GLOBALS_TYPES_H
