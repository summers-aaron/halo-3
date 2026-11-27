#ifndef SOUND_SOUND_CACHE_FILE_GESTALT_GLOBALS_H
#define SOUND_SOUND_CACHE_FILE_GESTALT_GLOBALS_H

struct s_sound_cache_file_gestalt_definition;

// The current map's gestalt and the permutation-count format selected while loading it.
struct gestalt_globals
{
    s_sound_cache_file_gestalt_definition *data;
    bool use_uncompressed_permutation_counts;
};

extern "C" { extern gestalt_globals g_sound_cache_file_gestalt; }

#endif // SOUND_SOUND_CACHE_FILE_GESTALT_GLOBALS_H
