#ifndef RASTERIZER_RASTERIZER_MAIN_STORAGE_H
#define RASTERIZER_RASTERIZER_MAIN_STORAGE_H

#include "cache/restricted_memory_regions.h"
#include "gpu_particle/contrail_vertex_declarations.h"
#include "camera/director.h"

struct s_rasterizer_main_declaration;
struct s_rasterizer_main_declaration_element;
#include "main/main_render_loading_types.h"
template<long k_table_size> struct rasterizer_main_queued_frame;

extern "C" {
extern s_main_render_loading_globals g_main_render_loading_globals;
}

#endif // RASTERIZER_RASTERIZER_MAIN_STORAGE_H
