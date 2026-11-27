#ifndef RENDER_RENDER_STRUCTURE_H
#define RENDER_RENDER_STRUCTURE_H

#include "render_methods/render_method_definitions.h"
#include "render/render_structure_globals_types.h"
#include "render/render_transparents.h"
#include "render/structure_renderer.h"

// Functions defined in tu/graphicslibxbox/render_structure.cpp.

bool cluster_sort_proc(short first_index, short second_index, const void *context);
extern "C" void structure_renderer_render_lighting();

struct render_structure_globals;

extern "C" void structure_renderer_render_instance_part(long part_index, long mode, long count);

// Data defined in tu/graphicslibxbox/render_structure.cpp that other units use.
extern render_structure_globals g_render_structure_globals;
struct s_render_cluster_part;
template<class t_element, long k_maximum_count> class c_sorter;
extern c_sorter<s_render_cluster_part, 512> g_cluster_sorted_order;
extern "C" void structure_renderer_render_shadows_generate(e_entry_point entry_point);

inline void c_structure_renderer::reset()
{
    g_render_structure_globals.current_a = 0;
    g_render_structure_globals.current_b = 0;
    g_render_structure_globals.depth = -1;
    g_cluster_sorted_order.m_count = 0;
    g_render_structure_globals.stack_b[5] = -1;
    g_render_structure_globals.cached_definition_index = -1;
    g_render_structure_globals.cached_instance_marker = -1;
}

#endif // RENDER_RENDER_STRUCTURE_H
