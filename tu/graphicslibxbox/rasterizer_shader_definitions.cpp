// graphicslibxbox/rasterizer_shader_definitions.obj: from the records 0x447c70
// @compiler xdk6130
// @flags /TP /O2 /Oc
#include "rasterizer/rasterizer_shader_definitions.h"

// @address 0x447c70 ?get_compiled_shader@c_rasterizer_vertex_shader@@QBAPBVc_rasterizer_compiled_vertex_shader@@W4e_vertex_type@@W4e_entry_point@@J@Z
const c_rasterizer_compiled_vertex_shader *c_rasterizer_vertex_shader::get_compiled_shader(
    e_vertex_type vertex_type, e_entry_point entry, long permutation) const
{
    const c_rasterizer_compiled_vertex_shader *result = 0;
    if (entry < m_entry_point_shaders.count()) {
        const s_rasterizer_vertex_shader_entry_point &entry_point = m_entry_point_shaders[entry];
        if (vertex_type < entry_point.vertex_types.count()) {
            const s_compiled_shader_reference &reference = entry_point.vertex_types[vertex_type];
            if (reference.count != 0) {
                result = &m_shaders[reference.start_index];
            }
        }
    }
    return result;
}
