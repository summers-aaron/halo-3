#include <xtl.h>
// blamlibxbox/particle_definitions.obj: from the records 0x180b70 0x182710
#include "render/render_objects_static_lighting.h"
// @flags /TP /O2 /Oc
#include "cseries/cseries.h"
#include "math/real_math.h"
#include "objects/object_reference_types.h"
#include <vectorintrinsics.h>
#include "tag_files/tag_types.h"
#include "tag_files/tag_groups.h"
#include "game/particle_location_data_array_types.h"
#include "effects/transparent_sort_layer_types.h"
#include "effects/particle_emitter_gpu_array_types.h"
#include "effects/particle_system.h"
#include "effects/particle_emitter_definitions.h"
#include "effects/particle_emitter.h"
#include "visibility/visibility_collection.h"
#include "effects/particle_system_definitions.h"
#include "effects/effect_definitions.h"
#include "memory/data.h"
#include "memory/datum_index.h"
#include "math/matrix_math_types.h"

#include "effects/particle_emitter_gpu.h"

#include "effects/particle_definitions.h"



#include "rasterizer/rasterizer_tiling_types.h"
#include "rasterizer/rasterizer_tiling_globals.h"
#include "rasterizer/rasterizer_distortion_state.h"
#include "rasterizer/rasterizer_state_globals.h"
#include "visibility/visibility_globals.h"

#include "effects/particle_movement_internals.h"
#include "effects/particle_location_runtime_types.h"
#include "effects/particle_definitions_globals.h"
#include "visibility/visibility_projections_and_volumes.h"
typedef char particle_location_size[(sizeof(s_particle_location_datum) == 0x60) ? 1 : -1];
typedef char particle_location_matrix_offset[(offsetof(s_particle_location_datum, matrix) == 0x18) ? 1 : -1];
typedef char particle_location_radius_offset[(offsetof(s_particle_location_datum, bounding_sphere_radius) == 0x5c) ? 1 : -1];

// @address 0x182710 particle_emitter_gpu_render_callback
extern "C" void particle_emitter_gpu_render_callback(const void *data, long user_data)
{
    static_cast<const c_particle_emitter_gpu *>(data)->render();
}





// @address 0x180b70 particle_emitter_render
// Per-emitter body of March c_particle_definition::render_particles (the caller loops over locations/emitters).
extern "C" void particle_emitter_render(c_particle_emitter *emitter, long pass)
{
    s_particle_location_datum *location = datum_get<s_particle_location_datum>(g_particle_location_data_array, emitter->m_location_index);
    c_particle_system *system = datum_get<c_particle_system>(g_particle_system_data_array, location->system_index);
    const c_particle_system_definition *definition = system->get_particle_system_definition();
    const c_particle_definition *particle = system->get_particle_definition();
    const c_particle_emitter_definition *emitter_definition = definition->get_emitter(emitter->m_definition_index);
    s_particle_emitter_gpu_block *gpu
        = &((s_particle_emitter_gpu_block *)g_particle_gpu_block_data->data)[datum_index_to_absolute_index(emitter->m_gpu_index)];
    void (*callback)(const void *, long) = particle_emitter_gpu_render_callback;
    s_visibility_input *collection_input = g_visibility_globals.collection_input;
    real_point3d point;
    const real_point3d *position = emitter->get_position_world(&point, system, definition);
    bool first_person = (location->state & 1) || (bool)((system->m_flags >> 7) & 1);
    volatile s_rasterizer_tiling_state *tiling = rasterizer_tiling_get_state();
    if (!first_person && !tiling->enabled) {
        if (visibility_volume_test_sphere_planes((const __vector4 *)&collection_input->storage[160], position,
                location->get_bounding_sphere_radius() + emitter_definition->m_bounding_sphere_radius) == 0) {
            emitter->set_rendered_last_frame(false);
            return;
        }
    }
    float sort_bias = definition->get_sort_bias();
    e_transparent_sort_layer sort_layer = (e_transparent_sort_layer)particle->m_sort_layer;
    if (pass == 2)
        g_rasterizer_distortion_visible = true;
    if (pass == 0) {
        callback(gpu, -1);
        emitter->set_rendered_last_frame(true);
        return;
    }
    render_objects_add_transparent_element(position, 0, sort_bias, sort_layer, callback, gpu, -1, 0.0f);
    emitter->set_rendered_last_frame(true);
}
