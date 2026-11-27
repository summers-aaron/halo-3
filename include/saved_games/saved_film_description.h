#ifndef SAVED_GAMES_SAVED_FILM_DESCRIPTION_H
#define SAVED_GAMES_SAVED_FILM_DESCRIPTION_H
#include <stddef.h>
// Describes a saved film: type, source, file path, display name, owning controller, and length in seconds.
struct s_saved_film_description
{
    long film_type;
    long campaign_id;
    long map_id;
    long difficulty;
    char file_path[0x100];
    wchar_t name[128];
    long controller_index;
    long length_seconds;
};
#if defined(_M_PPCBE) || defined(_XBOX)
typedef char saved_film_description_size[(sizeof(s_saved_film_description) == 0x218) ? 1 : -1];
typedef char saved_film_description_duration[(offsetof(s_saved_film_description, length_seconds) == 0x214) ? 1 : -1];
#endif
#endif
