#ifndef RASTERIZER_RASTERIZER_IMPLICIT_GEOMETRY_H
#define RASTERIZER_RASTERIZER_IMPLICIT_GEOMETRY_H

// Functions defined in tu/graphicslibxbox/rasterizer_implicit_geometry.cpp that other units call.

void initialize_circle_strip();
void __tls_set_g_rasterizer_implicit_geometry_data_allocator(void *allocator);
extern "C" void implicit_geometry_render_primitive(const void *context, long user);
extern "C" void *implicit_geometry_new_primitive(int object_index, const void *color, const void *transform);

template<long, void (*)(void *), void (*)(void *), void (*)(void *)>
class t_restricted_allocation_manager;
typedef t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_rasterizer_implicit_geometry_data_allocator> t_implicit_geometry_allocation_manager;
extern t_implicit_geometry_allocation_manager g_rasterizer_implicit_geometry_data_allocator;

extern "C" {
extern __declspec(thread) void *g_rasterizer_implicit_geometry_data;
}
#endif
