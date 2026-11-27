#ifndef RENDER_METHODS_RENDER_METHOD_SUBMIT_CACHE_H
#define RENDER_METHODS_RENDER_METHOD_SUBMIT_CACHE_H

class c_render_method;
struct s_render_method_submit_cache
{
    bool valid;
    const c_render_method *render_method;
    int vertex_type;
    int entry_point;
};

extern s_render_method_submit_cache g_render_method_submit_cache;
extern s_render_method_submit_cache k_default_render_method_submit_cache;

#endif
