#ifndef GPU_PARTICLE_PARTICLE_BLOCK_VERTEX_SHADER_INITIALIZE_H
#define GPU_PARTICLE_PARTICLE_BLOCK_VERTEX_SHADER_INITIALIZE_H

template<> void s_gpu_shader<c_particle_emitter_gpu::s_update_vertex_shader_constant_list>::initialize(c_rasterizer_constant_table_ref table_ref);

template<> __forceinline void s_gpu_vertex_shader<c_particle_emitter_gpu::s_update_vertex_shader_constant_list>::initialize()
{
    s_game_globals *game_globals = global_game_globals;
    c_rasterizer_globals *rasterizer_globals;
    if (game_globals->rasterizer_globals_ref.index != -1)
        rasterizer_globals = (c_rasterizer_globals *)tag_get('rasg', game_globals->rasterizer_globals_ref.index);
    else
        rasterizer_globals = 0;
    shader_reference = &rasterizer_globals->get_explicit_shaders(shader_index)->vertex;
    c_rasterizer_constant_table_ref table_ref = initialize_constant_table();
    constants.initialize(table_ref);
}

template<> __forceinline void s_gpu_vertex_shader<s_particle_block_contrail_vertex_shader_constant_list>::initialize()
{
    s_game_globals *game_globals = global_game_globals;
    c_rasterizer_globals *rasterizer_globals;
    if (game_globals->rasterizer_globals_ref.index != -1)
        rasterizer_globals = (c_rasterizer_globals *)tag_get('rasg', game_globals->rasterizer_globals_ref.index);
    else
        rasterizer_globals = 0;
    shader_reference = &rasterizer_globals->get_explicit_shaders(shader_index)->vertex;
    c_rasterizer_constant_table_ref table_ref = initialize_constant_table();
    constants.initialize(table_ref);
}
#endif
