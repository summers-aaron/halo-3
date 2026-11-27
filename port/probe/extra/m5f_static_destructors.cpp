// tmp/probe/extra (GAME-CHANGES F49): the atexit destructors of static objects that the source registers by name
// (particle_block.cpp, rasterizer_main.cpp, simulation_game_interface.cpp) but does not define. Each is what MSVC
// emits for the destructor of a global with a virtual destructor whose body is empty: the vtable pointer is set to the
// class's base vtable, nothing else (retail 0x70c590..0x70ca98: lis / addi / stw / blr).
#include "simulation/entity_definition_types.h"

// ---- particle_block.cpp's allocator objects (t_restricted_allocation_manager: ~ resets to its own vtable) ----
// The probe defines these objects with the retail vtables relocated (tmp/probe/fill/vtobjects.spec retail_vt_*,
// tmp/probe/extra/particle_allocators.cpp); a source build gets the same from the C++ destructor.
struct probe_vtable_object { const void *vtable; };
extern "C" probe_vtable_object
    g_light_volume_emitter_allocation, g_beam_gpu_allocator, g_beam_gpu_row_allocator, g_beam_gpu_buffer_allocation;
extern "C" const void *const retail_vt_4b9f8[], *const retail_vt_4ba20[], *const retail_vt_4ba28[], *const retail_vt_4ba30[];

extern "C" void light_volume_gpu_reset_emitter_allocation(void) { g_light_volume_emitter_allocation.vtable = retail_vt_4b9f8; }
extern "C" void beam_gpu_reset_allocator(void) { g_beam_gpu_allocator.vtable = retail_vt_4ba20; }
extern "C" void beam_gpu_reset_row_allocator(void) { g_beam_gpu_row_allocator.vtable = retail_vt_4ba28; }
extern "C" void beam_gpu_reset_buffer_allocation(void) { g_beam_gpu_buffer_allocation.vtable = retail_vt_4ba30; }

// ---- simulation entity definitions: each destructor leaves the base class vtable (retail addresses, the source's
// own spelling of these vtables, simulation_game_*.cpp) ----
extern "C" probe_vtable_object g_simulation_generic_garbage_entity_definition, g_simulation_vehicle_entity_definition,
    g_simulation_projectile_entity_definition, g_simulation_weapon_entity_definition, g_simulation_device_entity_definition,
    g_simulation_juggernaut_engine_globals_definition, g_simulation_game_engine_player_entity_definition,
    g_simulation_game_statborg_entity_definition, g_simulation_breakable_surface_group_entity_definition;

// rasterizer_main.cpp registers these five (the names are the source's; retail 0x70c838..0x70c898)
extern "C" void rasterizer_main_dispose_particle_spawn_table(void) { g_simulation_generic_garbage_entity_definition.vtable = (const void *)0x82049068; }
extern "C" void rasterizer_main_dispose_particle_vertex_buffer(void) { g_simulation_vehicle_entity_definition.vtable = (const void *)0x820491f8; }
extern "C" void rasterizer_main_dispose_contrail_spawn_buffers(void) { g_simulation_projectile_entity_definition.vtable = (const void *)0x82049408; }
extern "C" void rasterizer_main_dispose_contrail_vertex_buffer(void) { g_simulation_weapon_entity_definition.vtable = (const void *)0x82048f68; }
extern "C" void rasterizer_main_dispose_contrail_frame_vertex_buffer(void) { g_simulation_device_entity_definition.vtable = (const void *)0x82049a18; }

// simulation_game_interface.cpp (retail 0x70c778..0x70c7c0)
extern "C" void simulation_game_dispose_first_static_definition(void) { g_simulation_juggernaut_engine_globals_definition.vtable = (const void *)0x82047b80; }
extern "C" void simulation_game_dispose_second_static_definition(void) { g_simulation_game_engine_player_entity_definition.vtable = (const void *)0x820480e8; }
extern "C" void simulation_game_dispose_third_static_definition(void) { g_simulation_game_statborg_entity_definition.vtable = (const void *)0x82048278; }
extern "C" void simulation_game_dispose_fourth_static_definition(void) { g_simulation_breakable_surface_group_entity_definition.vtable = (const void *)0x82048508; }
