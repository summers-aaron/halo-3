#ifndef GPU_PARTICLE_VERTEX_SHADER_INITIALIZE_H
#define GPU_PARTICLE_VERTEX_SHADER_INITIALIZE_H
#include "gpu_particle/vertex_shader.h"
#include "game/game_globals.h"
#include "rasterizer/rasterizer_global_definitions.h"
#include "rasterizer/rasterizer_shader_definitions.h"
#include "rasterizer/c_rasterizer_constant_table_ref.h"
#include "tag_files/tag_groups.h"
template<class ConstantList>
__forceinline void s_gpu_vertex_shader<ConstantList>::initialize()
    {
        s_game_globals *game_globals = ((s_game_globals *)global_game_globals);
        c_rasterizer_globals *rasterizer_globals;
        if (game_globals->rasterizer_globals_ref.index != -1)
            rasterizer_globals = (c_rasterizer_globals *)tag_get('rasg', game_globals->rasterizer_globals_ref.index);
        else
            rasterizer_globals = 0;
        shader_reference = &rasterizer_globals->get_explicit_shaders(shader_index)->vertex;
        const c_rasterizer_vertex_shader *vertex_shader = (const c_rasterizer_vertex_shader *)tag_get('vtsh', shader_reference->index);
        c_rasterizer_compiled_vertex_shader *compiled_shader
            = (c_rasterizer_compiled_vertex_shader *)vertex_shader->get_compiled_shader((e_vertex_type)vertex_type, _entry_point_default, 0);
        c_rasterizer_constant_table_ref table_ref;
        table_ref.table = compiled_shader ? compiled_shader->get_constant_table() : 0;
        constants.initialize(table_ref);
    }
#endif
