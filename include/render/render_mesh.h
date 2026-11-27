#ifndef RENDER_RENDER_MESH_H
#define RENDER_RENDER_MESH_H

// Functions defined in tu/graphicslibxbox/render_mesh.cpp that other units call.

struct real_matrix4x3;
struct s_model_skinning_matrix;
struct s_shader_extern_info;

void real_matrix4x3_to_s_model_skinning_matrix(s_model_skinning_matrix *skinning, const real_matrix4x3 *matrix);
void render_mesh_submit_rigid_matrix(struct real_matrix4x3 const * _sym_p0);
void render_mesh_submit_skinning(const s_model_skinning_matrix *matrices, long matrix_count);
void render_dynamic_mesh(void *p0, long p1, long p2, long p3, long p4, struct s_shader_extern_info *shader_extern_info, bool);

struct geometry_material;
struct s_mesh;
struct s_part;
struct s_render_geometry;

extern "C" void render_mesh_part(const s_render_geometry *geometry, const s_mesh *mesh, const s_part *part, int mode,
    const unsigned long *selection, const geometry_material *materials);

extern "C" long g_last_skinning_matrix_count;

extern "C" {
extern const s_model_skinning_matrix *g_last_skinning_matrix_address;
void rasterizer_set_packed_blend_state(void *device, unsigned long index, unsigned long state);
}

#endif // RENDER_RENDER_MESH_H
