#ifndef RASTERIZER_RASTERIZER_OCCLUSION_QUERIES_H
#define RASTERIZER_RASTERIZER_OCCLUSION_QUERIES_H

#include "rasterizer/occlusion_query_types.h"

// Functions defined in tu/graphicslibxbox/rasterizer_occlusion_queries.cpp that other units call.

void rasterizer_occlusions_retrieve(long pool_index);
extern "C" void rasterizer_occlusion_initialize(void);
bool rasterizer_occlusions_get_result(e_occlusion_query_types type, long key, long player_window_index, long *result, long *context);
long rasterizer_occlusion_submit(e_occlusion_query_types type, long flags, long key, long player_window_index, void (*callback)(long));

// Data defined in tu/graphicslibxbox/rasterizer_occlusion_queries.cpp that other units use.

struct s_occlusion_query_globals;

extern s_occlusion_query_globals g_occlusion_query_globals;

#endif // RASTERIZER_RASTERIZER_OCCLUSION_QUERIES_H
