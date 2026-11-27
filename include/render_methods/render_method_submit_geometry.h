#ifndef RENDER_METHODS_RENDER_METHOD_SUBMIT_GEOMETRY_H
#define RENDER_METHODS_RENDER_METHOD_SUBMIT_GEOMETRY_H

struct s_cluster_reference;
struct s_shader_reference;
struct geometry_material;
struct s_mesh;
struct s_part;
struct s_render_geometry;

extern "C" void render_method_submit_set_structure_data(bool valid, long structure_bsp_index,
    s_cluster_reference lightmap_cluster_reference, long lightmap_instance_index,
    s_cluster_reference cubemap_dynamic_0_cluster_reference, long cubemap_dynamic_0_index,
    s_cluster_reference cubemap_dynamic_1_cluster_reference, long cubemap_dynamic_1_index, float cubemap_blend_factor,
    const s_shader_reference *change_colors, long context, float (*context_interface)(long, long, long));
extern "C" void render_method_submit_draw_mesh_part(unsigned __int64 key, const s_render_geometry *geometry, const s_mesh *mesh,
    const s_part *part, int mode, unsigned long *selection, const geometry_material *materials);

#endif // RENDER_METHODS_RENDER_METHOD_SUBMIT_GEOMETRY_H
