#ifndef RENDER_RENDER_VISIBILITY_COLLECTION_GLOBALS_H
#define RENDER_RENDER_VISIBILITY_COLLECTION_GLOBALS_H

#include "structures/cluster_bit_vectors.h"

struct s_render_visibility_collection_globals
{
    s_game_cluster_bit_vectors visible_clusters;
};

extern s_render_visibility_collection_globals render_visibility_collection_globals;

typedef char render_visibility_collection_globals_size[(sizeof(s_render_visibility_collection_globals) == 0x200) ? 1 : -1];

#endif // RENDER_RENDER_VISIBILITY_COLLECTION_GLOBALS_H
