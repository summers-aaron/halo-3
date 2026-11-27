// blamlibxbox/scenario_trigger_volumes.obj: from the records 0xac230 0xac2e8 0xe0e18 0x19a0e0 0x1c17d0 0x1c18a0
// @flags /TP /O2 /Oc
#include "memory/data_types.h"
#include "memory/tls_globals.h"
#include "math/matrix_math_types.h"
#include "math/real_math.h"
#include "tag_files/tag_types.h"
#include "objects/object_reference_types.h"
#include "objects/object_datum.h"
#include "objects/object_header.h"
#include "tag_files/tag_groups.h"
#include <ppcintrinsics.h>
#include "objects/objects.h"
#include "scenario/scenario.h"
#include "scenario/scenario_tag.h"
#include "hs/object_lists.h"
#include "models/models.h"
#include "scenario/scenario_trigger_volumes.h"
#include "memory/data.h"
#include "math/matrix_math.h"
#include "game/player_types.h"
#include "scenario/scenario_trigger_volume.h"
#include "objects/object_material_types.h"
#include "scenario/scenario_object_names.h"

// @address 0x84740 ?data_iterator_next@@YAPAXPAUs_data_iterator@@@Z
namespace {
template<typename datum_type> struct trigger_volume_data_iterator {
    datum_type *datum;
    s_data_iterator iterator;
    __forceinline void begin(s_data_array *data) {
        iterator.data = data;
        iterator.absolute_index = -1;
        iterator.index = -1;
    }
    __forceinline bool next() {
        datum = (datum_type *)data_iterator_next(&iterator);
        return datum != 0;
    }
    __forceinline datum_type *get_datum() const { return datum; }
};
}

// @address 0xac2e8 ?trigger_volume_get_matrix@@YA_NPBUscenario_trigger_volume@@PAUreal_matrix4x3@@@Z

/* Builds a trigger volume world matrix from its frame and attached object node. */

// @address 0x7cff0c ?global_tag_instances@@3PBUcache_file_tag_instance@@B

// @address 0xf07d0 ?matrix4x3_rotation_from_vectors@@YAXPAUreal_matrix4x3@@PBTreal_vector3d@@1@Z
// @address 0x263870 ?model_find_node@@YAJJJ@Z

bool trigger_volume_get_matrix(const scenario_trigger_volume *trigger_volume, real_matrix4x3 *matrix)
{
  bool result = false;
  if (trigger_volume->object_slot != 0xffff) {
    short slot = trigger_volume->object_slot;
    if (slot >= 0 && slot < 2048) {
      long object_index = scenario_object_name_get_object_index(slot);
      if (object_index != -1) {
        long node = model_find_node(
            TAG_GET('obje', s_object_material_definition, ((object_datum *)object_get_and_verify_type(object_index, NONE))->definition_index)->model.index,
            trigger_volume->node_name);
        if (node != -1) {
          matrix4x3_rotation_from_vectors(matrix, &trigger_volume->forward, &trigger_volume->up);
          matrix->position = trigger_volume->position;
          object_datum *object = (object_datum *)object_get_and_verify_type(object_index, NONE);
          matrix4x3_compute_product((real_matrix4x3 *)((char *)object + object->node_matrices.offset + (short)node * sizeof(real_matrix4x3)), matrix, matrix);
          result = true;
        }
      }
    }
  } else {
    matrix4x3_rotation_from_vectors(matrix, &trigger_volume->forward, &trigger_volume->up);
    matrix->position = trigger_volume->position;
    result = true;
  }
  return result;
}

// @address 0x7cff08 ?global_scenario@@3PAUscenario@@A
// @address 0x9fb10 ?matrix4x3_inverse_transform_point@@YAPATreal_point3d@@PBUreal_matrix4x3@@PBT1@PAT1@@Z

// @address 0xac230 ?scenario_trigger_volume_test_point@@YA_NJPBTreal_point3d@@@Z
bool scenario_trigger_volume_test_point(long volume_index, const real_point3d *point)
{
    real_point3d local;
    real_matrix4x3 matrix;
    bool result = false;
    if (trigger_volume_get_matrix(&((scenario_trigger_volume *)global_scenario->trigger_volumes.address)[volume_index], &matrix)) {
        scenario_trigger_volume *volume = &((scenario_trigger_volume *)global_scenario->trigger_volumes.address)[volume_index];
        matrix4x3_inverse_transform_point(&matrix, point, &local);
        if (local.x > 0.0f && local.y > 0.0f && local.z > 0.0f &&
            local.x < volume->extents.x && local.y < volume->extents.y && local.z < volume->extents.z)
            result = true;
    }
    return result;
}

// @address 0xe0e18 ?scenario_trigger_volume_test_object@@YA_NJJ@Z
bool scenario_trigger_volume_test_object(long trigger_volume_index, long object_index)
{
    bool result = false;
    if (object_index != -1) {
    const real_point3d *position = &((object_datum *)object_get_and_verify_type(object_index, NONE))->bounding_center;
        if (scenario_trigger_volume_test_point(trigger_volume_index, position))
            result = true;
    }
    return result;
}



bool scenario_trigger_volume_test_players_all(long trigger_volume_index)
{
    bool result = true;
    bool found_player = false;
    trigger_volume_data_iterator<player_datum> players;
    players.begin(tls_player_data());
    while (players.next()) {
        player_datum *player = players.get_datum();
        if (player->unit_index != -1) {
            const real_point3d *position = &((object_datum *)object_get_and_verify_type(player->unit_index, NONE))->bounding_center;
            found_player = true;
            if (!scenario_trigger_volume_test_point(trigger_volume_index, position)) {
                result = false;
                break;
            }
        }
    }
    return result && found_player;
}

// @address 0x19a0e0 scenario_trigger_volumes_test_point
extern "C" unsigned char scenario_trigger_volumes_test_point(const real_point3d *point)
{
    bool result = false;
    scenario *data = global_scenario;
    if (data->trigger_volume_indices.count > 0) {
        for (long index = 0; index < data->trigger_volume_indices.count; ++index) {
            short trigger_volume_index = ((short *)data->trigger_volume_indices.address)[index];
            if (trigger_volume_index != -1)
                result = (unsigned char)(scenario_trigger_volume_test_point(trigger_volume_index, point) | result);
        }
        return result;
    }
    return 1;
}

// @address 0x120400 object_list_add
// @address 0xade58 ?matrix4x3_transform_point@@YAPATreal_point3d@@PBUreal_matrix4x3@@PBT1@PAT1@@Z
// @address 0xbab88 ?scenario_location_from_point@@YAXPAUs_location@@PBTreal_point3d@@@Z
// @address 0xa0d80 ?objects_in_sphere@@YAFKKPBUs_location@@PBTreal_point3d@@MPAJF@Z

static __forceinline bool scenario_trigger_volumes_location_valid(const s_location *location)
{
    return location->cluster_reference.bsp_index != -1;
}


// @address 0x1c18a0 ?scenario_trigger_volume_return_objects_by_type@@YAJJJ@Z
long scenario_trigger_volume_return_objects_by_type(long volume_index, long type_mask)
{
    long object_list = ::object_list_new();
    if (object_list != -1) {
        real_matrix4x3 matrix;
        if (trigger_volume_get_matrix(&((scenario_trigger_volume *)global_scenario->trigger_volumes.address)[volume_index], &matrix)) {
            const scenario_trigger_volume *volume = &((scenario_trigger_volume *)global_scenario->trigger_volumes.address)[volume_index];
            real_point3d center;
            // match: 0x1c18a0: pointer __restrict qualifier prevents alias reload
            const real_point3d *__restrict extents = &volume->extents;
            real_point3d * out = &center;
            out->x = extents->x * 0.5f;
            out->y = extents->y * 0.5f;
            out->z = extents->z * 0.5f;
            matrix4x3_transform_point(&matrix, &center, &center);
            s_location location;
            scenario_location_from_point(&location, &center);
            if (scenario_trigger_volumes_location_valid(&location)) {
                float radius = volume->radius;
                long objects[128];
                long count = objects_in_sphere(0, type_mask, &location, &center, radius, objects, 128);
                for (long i = 0; i < count; ++i) {
                    long object = objects[i];
                    if (scenario_trigger_volume_test_object(volume_index, object))
                        object_list_add(object_list, object);
                }
            }
        }
    }
    return object_list;
}
