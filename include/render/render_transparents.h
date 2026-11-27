#ifndef RENDER_RENDER_TRANSPARENTS_H
#define RENDER_RENDER_TRANSPARENTS_H

#include "math/real_math_types.h"
#include "memory/static_arrays.h"
#include "cseries/long_sort.h"
#include "render/render_objects_static_lighting.h"
#include "render/views/render_player_view.h"
#include "render/render_objects.h"
#include "render/render_texture_camera.h"

// Functions defined in tu/graphicslibxbox/render_transparents.cpp that other units call.

extern "C" long render_transparents_compute_side_votes(unsigned short index_a, unsigned short index_b);

// One element of c_transparency_renderer::transparents (c_static_array<s_transparent_types, 1024> in the March
// 2007 build).
struct s_transparent_types
{
    bool has_plane;
    float sort_key;
    real_point3d position;
    real_plane3d plane;
    long sort_layer;
    real_point3d sample_points[9];
    float radius;
    void (*render_callback)(const void *, long);
    const void *render_data;
    long user_data;
};

// The sorted element order (March c_sorter<s_transparent_types, 1024>: get_order_entry, set_count, clear), which
// keeps four spare entries after the order and the element count at +0x808.
template<class t_element, long k_maximum_count>
class c_sorter
{
public:
    __forceinline unsigned short get_count() const { return (unsigned short)m_count; }
    __forceinline unsigned short get_order_entry(long index) const { return (unsigned short)m_order[index]; }
    __forceinline void set_order_entry(long index, unsigned short entry)
    {
      m_order[index] = (short)entry;
      m_count = (short)((long)get_count() > index + 1 ? (long)get_count() : index + 1);
    }
    __forceinline void fill_order(unsigned short count, long first)
    {
      for (long index = 0; index < (count > k_maximum_count ? k_maximum_count : count); index++)
      {
        m_order[index] = (short)(index + first);
      }
      m_count = (short)count;
    }
    __forceinline void sort(bool (*compare)(short, short, const void *), long first)
    {
      long count = get_count() - first;
      if ((unsigned long)count >= 2)
      {
        short *order = &m_order[first];
        templated_sort<short>::qsort(order, count, compare, m_context);
      }
    }

    __forceinline void set_count(long count) { m_count = (short)count; }
    __forceinline void clear() { m_count = 0; }

    short m_order[k_maximum_count + 4];
    short m_count;
    const void *m_context;
};

// One saved transparent count per push_marker (March k_max_number_of_transparency_markers is 6).
struct s_transparency_marker
{
    unsigned short first_transparent_index;
};

// c_transparency_renderer (render_transparents.obj in the March 2007 build, which also has add_element and sort;
// only the methods some unit calls are listed). The data members are private in March, which names each one; the
// tree's units outside the class reach them through the friends (March's add_element and sort still sit in
// render_objects_static_lighting.cpp and render_player_view.cpp as free functions).
class c_transparency_renderer
{
public:
    static void render(bool set_z_buffer);

    static __forceinline void reset()
    {
        m_current_marker_index = -1;
        m_total_transparent_count = 0;
        transparent_sorted_order.clear();
    }
    static __forceinline void push_marker()
    {
        ++m_current_marker_index;
        m_markers[m_current_marker_index].first_transparent_index = (unsigned short)m_total_transparent_count;
        transparent_sorted_order.set_count((unsigned short)m_total_transparent_count);
    }
    static __forceinline void pop_marker()
    {
        m_total_transparent_count = m_markers[m_current_marker_index].first_transparent_index;
        --m_current_marker_index;
        transparent_sorted_order.set_count((unsigned short)m_total_transparent_count);
    }

private:
    friend long render_transparents_compute_side_votes(unsigned short index_a, unsigned short index_b);
    friend bool render_objects_add_transparent_element(const real_point3d *point, const real_plane3d *plane, float bias,
        e_transparent_sort_layer sort_layer, void (*callback)(const void *, long), const void *context, long user, float radius);
    friend void render_player_view_find_transparent_boundary(void *buffer, unsigned short first, unsigned short last,
        unsigned short *boundary);
    friend void render_player_view_sort_transparents_recursive(void *buffer, unsigned short first, unsigned short last,
        unsigned short boundary, void *scratch, unsigned short original_boundary, unsigned short depth);
    friend void render_player_view_sort_transparents();
    friend void ::player_view_render_pass(unsigned int p0, unsigned int p1);
    friend void ::render_objects_render_transparent_pass(c_player_view *view);
    friend void ::texture_camera_view_render_texture_camera(c_texture_camera_view *view, long view_index);
    friend void render_dispose_from_old_map();
    friend class c_texture_camera_view;

    static long m_current_marker_index;
    static s_transparency_marker m_markers[6];
    static c_static_array<s_transparent_types, 1024> transparents;
    static long m_total_transparent_count;
    static c_sorter<s_transparent_types, 1024> transparent_sorted_order;
};

// The transparent sort mode: 2 orders a planar transparent against a non-planar one by which side of the plane
// the other lies on.
// @address 0x8c484c g_transparent_sort_mode
extern "C" unsigned short g_transparent_sort_mode;

#endif
