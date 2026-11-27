#ifndef RENDER_METHODS_RENDER_METHOD_H
#define RENDER_METHODS_RENDER_METHOD_H

#include "memory/data_types.h"
#include "render_methods/render_method_definitions.h"
#include "rasterizer/rasterizer_vertex_types.h"
#include "tag_files/tag_groups.h"

class c_rasterizer_compiled_pixel_shader;
class c_rasterizer_compiled_vertex_shader;

class c_render_method
{
public:
    const c_rasterizer_compiled_pixel_shader *get_compiled_pixel_shader(e_entry_point entry_point) const;
    const c_rasterizer_compiled_vertex_shader *get_compiled_vertex_shader(e_vertex_type vertex_type, e_entry_point entry_point) const;
    s_tag_reference definition;
    s_tag_block options;
    s_tag_block parameters;
    s_tag_block postprocess;
    unsigned short flags;
    unsigned char sort_layer;
    long atmosphere_index;

    inline long get_selected_option(short category) const
    {
        if (category >= 0 && category < options.count)
            return (short)((const short *)options.address)[category];
        return -1;
    }
    __forceinline const s_render_method_postprocess_definition *get_postprocess_definition() const
    {
        return (const s_render_method_postprocess_definition *)postprocess.address;
    }
    __forceinline unsigned char get_sort_layer() const { return sort_layer; }
    static __forceinline const c_render_method *get(long index)
    {
        return (const c_render_method *)global_tag_instances[DATUM_INDEX_TO_ABSOLUTE_INDEX(index)].definition;
    }
    static bool get_is_transparent(long index);
    static bool get_is_decal(long index);
};

typedef char render_method_size[(sizeof(c_render_method) == 0x3c) ? 1 : -1];
typedef char render_method_postprocess_offset[(offsetof(c_render_method, postprocess) == 0x28) ? 1 : -1];
typedef char render_method_flags_offset[(offsetof(c_render_method, flags) == 0x34) ? 1 : -1];
typedef char render_method_sort_layer_offset[(offsetof(c_render_method, sort_layer) == 0x36) ? 1 : -1];
typedef char render_method_atmosphere_offset[(offsetof(c_render_method, atmosphere_index) == 0x38) ? 1 : -1];

#endif // RENDER_METHODS_RENDER_METHOD_H
