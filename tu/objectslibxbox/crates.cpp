// objectslibxbox/crates.obj: from the records 0x35d258 0x35d3f0
// @flags /TP /O2 /Oc
#include "game/game_time_types.h"
#include "memory/tls_globals.h"
#include "objects/object_placement.h"
// @address 0x35d3f0 ?crate_place@@YAXJPAUs_scenario_crate@@@Z
#include <ppcintrinsics.h>
#include <math.h>
#include <stdlib.h>
#include "tag_files/tag_types.h"
#include "objects/objects.h"
#include "objects/object_header.h"
#include "tag_files/tag_groups.h"
#include "physics/physics_models.h"
#include "render/views/render_player_view.h"
#include "objects/crates.h"
#include "objects/crate_definitions.h"
#include "models/model_types.h"
#include "physics/physics_model_types.h"
#include "scenario/scenario_objects.h"
#include "physics/physics_model_instance.h"
#include "render/render_texture_camera_globals.h"
#include "ai/ai_reference_frame.h"

static __forceinline void crates_flags_set(unsigned long *flags, long bit, bool value)
{
    if (value) *flags |= 1 << bit;
    else *flags &= ~(1 << bit);
}

// @address 0x35d258 ?crate_new@@YA_NJPAUobject_placement_data@@PA_N@Z
bool crate_new(long object_index, object_placement_data *placement_data, bool *out_of_memory)
{
    bool result = false;
    crate_datum *crate = (crate_datum *)object_get_and_verify_type(object_index, _object_mask_crate);
    crate->flags = 0;
    crate->lifetime_ticks = -1;
    if (placement_data->definition_index != -1)
    {
        s_crate_definition *definition = TAG_GET('bloc', s_crate_definition, placement_data->definition_index);
        if (definition->crate_flags & 1)
            crates_flags_set(&crate->flags, 1, true);
        if (definition->crate_flags & 2)
        {
            s_render_texture_camera_globals *camera = tls_render_texture_camera_globals();
            camera->enabled = true;
            camera->render_mode = 0;
            render_texture_camera_set_position_object(object_index, 0);
            camera->orientation_type = 4;
        }
        if (definition->object.model.index != -1)
        {
            s_model_definition *model = TAG_GET('hlmt', s_model_definition, definition->object.model.index);
            if (model->physics_model_index != -1)
            {
                s_physics_model_definition *physics_model = TAG_GET('phmo', s_physics_model_definition, model->physics_model_index);
                bool has_dynamic_body = false;
                for (long i = 0; i < physics_model->rigid_bodies.count(); i++)
                {
                    s_crate_rigid_body_definition *rigid_body = (s_crate_rigid_body_definition *)physics_model->rigid_bodies.address + i;
                    if (rigid_body->motion_type != 1 && rigid_body->motion_type != 2)
                        has_dynamic_body = true;
                }
                if (!has_dynamic_body)
                    crates_flags_set(&crate->flags, 1, true);
            }
        }
        if (definition->self_destruction_timer > 0)
            crate->lifetime_ticks = tls_game_time_globals()->ticks_per_second * definition->self_destruction_timer;
    }
    s_physics_model_instance instance;
    if (physics_model_instance_new(&instance, object_index))
    {
        if (instance.physics_model->rigid_bodies.count() > 0)
            result = true;
    }
    return result;
}

void crate_place(long object_index, s_scenario_crate *scenario_crate)
{
    crate_datum *crate = (crate_datum *)object_get_and_verify_type(object_index, _object_mask_crate);
    crate->object.flag_bits.set(_object_data_flag_22, false);
    crate->object.flag_bits.set(_object_data_flag_23, false);
    short policy = -1;
    switch (scenario_crate->pathfinding_policy)
    {
    case 0:
    case 1:
        policy = 2;
        break;
    case 2:
        policy = 0;
        break;
    case 3:
        policy = 1;
        break;
    case 4:
        policy = 3;
        break;
    }
    switch (policy)
    {
    case 0:
    case 3:
        crate->object.flag_bits.set(_object_data_flag_23, true);
        break;
    case 1:
        crate->object.flag_bits.set(_object_data_flag_22, true);
        object_refresh_reference_frames(object_index);
        crate->object.flag_bits.set(_object_data_flag_23, true);
        break;
    }
    object_update_collision_culling(object_index);
}
