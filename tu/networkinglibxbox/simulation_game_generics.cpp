// networkinglibxbox/simulation_game_generics.obj: from the records 0x434440 0x434450 0x434460 0x4344e0 0x4345b8 0x434608 0x434788 0x434848 0x434a78 0x434ae8 0x434b90 0x434be0 0x434d48 0x434ee0 0x435008 0x4350e8 0x4350f8 0x43add8 0x709f48 0x709f58 0x70c820 0x70c838
// @flags /TP /O2 /Oc
#include <string.h>
#include "networking/simulation/simulation_entity_definition.h"
#include "simulation/simulation_game_entity_definitions.h"
#include "memory/tls_data_arrays.h"
#include "networking/simulation/simulation_entity_database.h"
#include "math/matrix_math_types.h"
#include "math/real_math.h"
#include "tag_files/tag_groups.h"
#include "objects/object_types.h"
#include "objects/object_placement.h"
#include "phys/phys_base.h"
#include "cache/cache_files.h"
#include "physics/havok_utilities.h"
#include "simulation/simulation_encoding.h"
#include "physics/havok.h"
#include "render/render_lens_flares.h"
#include "memory/bitstream.h"
#include "simulation/simulation_priority.h"
#include "memory/data_types.h"
#include "physics/havok_component.h"
#include "objects/objects.h"
#include "math/real_math_types.h"
#include "objects/object_hierarchy.h"
#include "objects/object_definitions.h"
#include "models/model_types.h"
#include "simulation/game_interface/simulation_game_generic_data_types.h"
#include "simulation/game_interface/simulation_game_object_entity_definition.h"
#include "simulation/game_interface/simulation_game_generics.h"
#include "simulation/game_interface/simulation_game_vehicles_internals.h"
#include "physics/havok_component_class.h"
#include "models/model_variant_types.h"
#include "simulation/game_interface/simulation_game_object_creation_inline.h"
#include "simulation/simulation_entity_types.h"
// @address 0x434450 ?state_data_size@c_simulation_generic_entity_definition@@UAAJXZ
// @address 0x4344e0 ?build_object_creation_data@c_simulation_generic_entity_definition@@UAAXJJPAX@Z
// @address 0x434460 ?object_required_to_join_game@c_simulation_generic_entity_definition@@UAA_NJ@Z
// @address 0x434b90 ?update_minimum_required_bits@c_simulation_generic_entity_definition@@UAAJPBUs_simulation_entity@@PBUs_entity_update_data@@@Z
// @address 0x43add8 ?rotate_entity_indices@c_simulation_generic_entity_definition@@UAAXPBUs_simulation_entity@@@Z
// @address 0x40d178 ?object_update_decode@c_simulation_object_entity_definition@@KA_N_NPAKPAUs_simulation_object_state_data@@PAVc_bitstream@@0@Z
// @address 0x220c70 ?read_axes@c_bitstream@@QAAXPBDPATreal_vector3d@@1@Z
// @address 0x221648 ?read_bit_internal@c_bitstream@@AAA_NXZ
// @address 0x221768 ?read_value_internal@c_bitstream@@AAAKJ@Z
// @address 0xbbcf0 ?real_vector3d_build_axes_from_vectors@@YAXPBTreal_vector3d@@0PAT1@11@Z
// @address 0x12b7d0 ?valid_real_vector3d_axes2@@YA_NPBTreal_vector3d@@0@Z

static __forceinline void simulation_generic_read_raw_data(c_bitstream *stream, const char *name, void *buffer, long size_in_bits)
{
    unsigned char *data = (unsigned char *)buffer;
    for (; size_in_bits >= 64; size_in_bits -= 64, data += 8)
        *(unsigned __int64 *)data = bitstream_read_window_bits(stream, 64);
    if (size_in_bits > 0)
    {
        unsigned __int64 value = bitstream_read_window_bits(stream, size_in_bits) << (64 - size_in_bits);
        for (unsigned long byte_count = (unsigned long)size_in_bits / 8; byte_count != 0; --byte_count)
        {
            *data++ = (unsigned char)(value >> 56);
            value <<= 8;
        }
        if (size_in_bits % 8)
            *data = (unsigned char)(value >> 56);
    }
}

static __forceinline object_datum *simulation_game_generics_get_creation_object(long index)
{
    return (object_datum *)object_get_and_verify_type(index, ~0);
}

// 0x434440
// @address 0x434440 ?entity_type_name@c_simulation_generic_entity_definition@@UAAPBDXZ
const char *c_simulation_generic_entity_definition::entity_type_name()
{
    return "generic";
}

// 0x434450
long c_simulation_generic_entity_definition::state_data_size()
{
    return sizeof(s_simulation_generic_state_data);
}

bool c_simulation_generic_entity_definition::object_required_to_join_game(long object_index) {
 long root_object_index=object_get_root_object(object_index);
 object_datum *object=(object_datum *)object_get_and_verify_type(root_object_index,~0);
 bool result=false;
 if(object->identifier.get_source()!=2 || object->map_variant_index!=-1) result=true;
 return result;
}

long c_simulation_generic_entity_definition::update_minimum_required_bits(const s_simulation_entity *entity, const s_entity_update_data *update)
{
    long value;
    object_calculate_update_requirements(entity, update, &value);
    if ((update->update_mask & 0x4000) != 0 && value > 366) {
        value = 366;
    }
    return value;
}

// 0x4350e8
// @address 0x4350e8 ?entity_type_name@c_simulation_generic_garbage_entity_definition@@UAAPBDXZ
const char *c_simulation_generic_garbage_entity_definition::entity_type_name()
{
    return "generic-garbage";
}

// 0x4350f8
// @address 0x4350f8 ?create_object@c_simulation_generic_garbage_entity_definition@@UAAJJPBXPAKJ0@Z
long c_simulation_generic_garbage_entity_definition::create_object(
    long object_index, const void *creation_data, unsigned long *state_data_size, long p4, const void *p5)
{
    long garbage_index = c_simulation_generic_entity_definition::create_object(object_index, creation_data, state_data_size, p4, p5);
    if (garbage_index != -1)
        object_set_garbage(garbage_index, true, 0);
    return garbage_index;
}

void c_simulation_object_entity_definition::object_entity_rotate_indices(const s_simulation_entity *entity)
{
    s_simulation_generic_state_data *state = (s_simulation_generic_state_data *)entity->state_data;
    int entity_index = state->object.parent_entity_index;
    if (entity_index == -1)
        return;
    state->object.parent_entity_index = ((((int)((unsigned int)entity_index >> 28) + 1) % 16) << 28) | (entity_index & 0x3ff);
}

// 0x4344e0
__forceinline void c_simulation_object_entity_definition::object_entity_variant_build_creation_data(
    long object_index, long creation_data_size, void *creation_data)
{
    s_simulation_generic_creation_data *data = (s_simulation_generic_creation_data *)creation_data;
    object_datum *object = simulation_game_generics_get_creation_object(object_index);
    memset(data, 0, sizeof(s_simulation_generic_creation_data));
    object_build_creation_data(object_index, data);
    s_object_definition *definition = (s_object_definition *)tag_get(0, object->definition_index);
    if (object->variant_index != -1 && definition->model.index != -1) {
        s_model_definition *model = (s_model_definition *)tag_get(0, definition->model.index);
        data->variant_name = TAG_BLOCK_GET_ELEMENT(&model->variants, object->variant_index, s_model_variant)->name;
    } else {
        data->variant_name = 0;
    }
}

// 0x4345b8
// @address 0x4345b8 ?build_baseline_state_data@c_simulation_generic_entity_definition@@UAA_NJPBXJPAX@Z
bool c_simulation_generic_entity_definition::build_baseline_state_data(long, const void *creation_data, long, void *state_data)
{
    bool result = false;
    memset(state_data, 0, sizeof(s_simulation_generic_state_data));
    if (c_simulation_object_entity_definition::object_build_baseline_state_data(
            (const s_simulation_object_creation_data *)creation_data, (s_simulation_object_state_data *)state_data))
        result = true;
    return result;
}

// @address 0x434a78 ?write_creation_description_to_string@c_simulation_generic_entity_definition@@UAAXPBUs_simulation_entity@@PBUs_simulation_view_telemetry_data@@JPAD@Z
void c_simulation_generic_entity_definition::write_creation_description_to_string(
    const s_simulation_entity *entity, const s_simulation_view_telemetry_data *telemetry, long size, char *buffer)
{
  float relevance = 0.0f;
  simulation_calculate_entity_creation_priority(entity, telemetry, &relevance);
  csnzprintf(buffer, size, "generic creation: relevance=%5.3f", relevance);
}

// @address 0x434ae8 ?entity_creation_decode@c_simulation_generic_entity_definition@@UAA_NJPAXPAVc_bitstream@@_N@Z
__forceinline bool c_simulation_object_entity_definition::object_entity_variant_creation_decode(long creation_data_size, void *creation_data,
    c_bitstream *packet, bool quantize)
{
    s_simulation_generic_creation_data *generic_creation = (s_simulation_generic_creation_data *)creation_data;
    bool object_decoded = c_simulation_object_entity_definition::object_creation_decode(generic_creation, packet, quantize);
    simulation_generic_read_raw_data(packet, "variant-name", &generic_creation->variant_name, 32);
    bool success = !packet->overflowed();
    success = success && object_decoded;
    return success;
}

// @address 0x49028 k_simulation_generic_rigid_body_transforms_name
// @address 0x49048 k_simulation_generic_rigid_body_axes_name
// @rdata 0x82049028..0x82049058
const char k_simulation_generic_rigid_body_transforms_name[32] = "rigid-body-transforms-exists";
const char k_simulation_generic_rigid_body_axes_name[16] = "transform-axes";
// @rdata-end
// @address 0x49068 ??_7c_simulation_generic_entity_definition@@6B@
// @address 0x709f48 ??__Eg_simulation_generic_entity_definition@@YAXXZ
// @address 0x70c820 ??__Fg_simulation_generic_entity_definition@@YAXXZ
// @address 0x7bb4c0 g_simulation_generic_entity_definition
c_simulation_generic_entity_definition g_simulation_generic_entity_definition;
// @address 0x49100 ??_7c_simulation_generic_garbage_entity_definition@@6B@
// @address 0x709f58 ??__Eg_simulation_generic_garbage_entity_definition@@YAXXZ
// @address 0x70c838 ??__Fg_simulation_generic_garbage_entity_definition@@YAXXZ
// @address 0x7bb4c4 g_simulation_generic_garbage_entity_definition
c_simulation_generic_garbage_entity_definition g_simulation_generic_garbage_entity_definition;




// @address 0x40c8e0 ?object_creation_decode@c_simulation_object_entity_definition@@KA_NPAUs_simulation_object_creation_data@@PAVc_bitstream@@_N@Z
// @address 0x40c9d0 ?object_calculate_update_requirements@c_simulation_object_entity_definition@@KAXPBUs_simulation_entity@@PBUs_entity_update_data@@PAJ@Z
// @address 0x40b128 ?object_build_baseline_state_data@c_simulation_object_entity_definition@@KA_NPBUs_simulation_object_creation_data@@PAUs_simulation_object_state_data@@@Z
// @address 0x1858b8 ?csnzprintf@@YAPADPADIPBDZZ
// @address 0xfcaa0 ?object_set_garbage@@YAXJ_NJ@Z
// @address 0x7cff0c ?global_tag_instances@@3PBUcache_file_tag_instance@@B
// @address 0x221480 ?write_value_internal@c_bitstream@@QAAXKJ@Z
// @address 0x2208b0 ?write_axes@c_bitstream@@QAAXPBDPBTreal_vector3d@@1@Z
// @address 0x3adbd8 ?simulation_write_quantized_position@@YAXPAVc_bitstream@@PBTreal_point3d@@J_NPBTreal_rectangle3d@@@Z
// @address 0x40cb38 ?object_update_encode@c_simulation_object_entity_definition@@KA_N_NKPAKPBUs_simulation_view_telemetry_data@@PBUs_simulation_object_state_data@@PAVc_bitstream@@J00@Z
// @address 0x446e88 ?make_room_for_update@c_entity_update_encode_helper@@QAA_NPAVc_bitstream@@JJJK@Z
// @address 0x446ef8 ?write_component_header@c_entity_update_encode_helper@@QAA_NJPBD@Z
// @address 0x446f80 ?finish_component@c_entity_update_encode_helper@@QAAXXZ
// @address 0x434be0 ?entity_update_encode@c_simulation_generic_entity_definition@@UAA_N_NKPAKJPBXPBUs_simulation_view_telemetry_data@@PAVc_bitstream@@J0@Z
bool c_simulation_generic_entity_definition::entity_update_encode(bool initial_update, unsigned long update_mask,
    unsigned long *updated_mask, long state_data_size, const void *state_data, const s_simulation_view_telemetry_data *telemetry,
    c_bitstream *packet, long maximum_bits, bool final_update)
{
    const s_simulation_generic_state_data *state = (const s_simulation_generic_state_data *)state_data;
    bool result = false;
    if (c_simulation_object_entity_definition::object_update_encode(initial_update, update_mask & 0x3fff, updated_mask, telemetry,
            (const s_simulation_object_state_data *)state, packet, maximum_bits + 1,
            ensure_object_position_update_quantization_inside_bsp(), final_update))
    {
        c_entity_update_encode_helper helper;
        if (helper.make_room_for_update(packet, maximum_bits, 14, 1, update_mask & 0x4000))
        {
            if (helper.write_component_header(14, k_simulation_generic_rigid_body_transforms_name))
            {
                packet->write_integer("rigid-body-mask", state->mask, 8);
                for (long i = 0; i < 8; i++)
                {
                    if (state->mask & (1 << i))
                    {
                        packet->write_axes(k_simulation_generic_rigid_body_axes_name, &state->transforms[i].forward, &state->transforms[i].up);
                        simulation_write_quantized_position(packet, &state->transforms[i].position, 16, false, 0);
                    }
                }
            }
            helper.finish_component();
            helper.finish_update(updated_mask);
            result = true;
        }
    }
    return result;
}


// @address 0x434d48 ?entity_update_decode@c_simulation_generic_entity_definition@@UAA_N_NPAKJPAXPAVc_bitstream@@0@Z
bool c_simulation_generic_entity_definition::entity_update_decode(
    bool initial_update, unsigned long *update_mask, long state_data_size, void *state_data, c_bitstream *packet, bool unknown)
{
    s_simulation_generic_state_data *state = (s_simulation_generic_state_data *)state_data;
    bool valid = true;
    bool object_valid = c_simulation_object_entity_definition::object_update_decode(
        initial_update, update_mask, (s_simulation_object_state_data *)state, packet, unknown);
    if (packet->read_bool("rigid-body-transforms-exists"))
    {
        state->mask = packet->read_integer("rigid-body-mask", 8);
        for (long index = 0; index < 8; ++index)
        {
            if (state->mask & (1 << index))
            {
                real_matrix4x3 *transform = &state->transforms[index];
                real_vector3d forward, up;
                transform->scale = 1.0f;
                packet->read_axes(k_simulation_generic_rigid_body_axes_name, &forward, &up);
                real_vector3d_build_axes_from_vectors(&forward, &up, &transform->forward, &transform->left, &transform->up);
                ::simulation_read_quantized_position(packet, &transform->position, 16, 0);
                valid = valid && valid_real_vector3d_axes2(&forward, &up);
            }
        }
        *update_mask |= 0x4000;
    }
    valid = valid && object_valid;
    valid = valid && !packet->overflowed();
    return valid;
}

// @address 0x434ee0 ?entity_state_lossy_compare@c_simulation_generic_entity_definition@@UAA_NPAX0J@Z
bool c_simulation_generic_entity_definition::entity_state_lossy_compare(void *first_state, void *second_state, long)
{
    s_simulation_generic_state_data *first = static_cast<s_simulation_generic_state_data *>(first_state);
    s_simulation_generic_state_data *second = static_cast<s_simulation_generic_state_data *>(second_state);
    bool valid = (unsigned char)c_simulation_object_entity_definition::object_state_compare(
        (s_simulation_object_state_data *)first,
        (s_simulation_object_state_data *)second) != 0
        && first->mask == second->mask;
    if (valid)
    {
        for (long index = 0; index < 8; index++)
        {
            if ((((1 << index) & first->mask)) != 0)
            {
                real_matrix4x3 *first_transform = &first->transforms[index];
                real_matrix4x3 *second_transform = &second->transforms[index];
                bool axes_equal = (unsigned char)valid != 0 && c_bitstream::compare_axes(
                    &first_transform->forward,
                    &first_transform->up,
                    &second_transform->forward,
                    &second_transform->up) != 0;
                valid = (unsigned char)axes_equal != 0 && simulation_compare_quantized_positions(
                    &first_transform->position,
                    &second_transform->position,
                    16, 0) != 0;
                memset(first_transform, 0, sizeof(*first_transform));
                memset(second_transform, 0, sizeof(*second_transform));
            }
        }
    }
    return valid;
}

static __forceinline c_havok_component *c_havok_component_get(long component_index)
{
    return (c_havok_component *)(g_havok_component_data->data + (component_index & 0xffff) * sizeof(c_havok_component));
}
static __forceinline bool simulation_game_generics_rigid_body_is_dynamic(unsigned char motion_type)
{
    return motion_type != 7 && motion_type != 6;
}
// Specialized rigid-body mask query without an unused entity-definition receiver.
// @address 0x435008 simulation_generic_object_get_rigid_body_mask
extern "C" unsigned long simulation_generic_object_get_rigid_body_mask(long object_index)
{
    unsigned long rigid_body_mask = 0;
    if ((unsigned char)havok_crate_needs_physics_update(object_index) != 0) {
        long body_index = 0;
        object_datum *object = (object_datum *)object_get_and_verify_type(object_index, ~0);
        c_havok_component *component = c_havok_component_get(object->havok_component_index);
        long body_count = component->m_rigid_bodies.getSize();
        if (body_count > 0) {
            for (; body_index < body_count; body_index++) {
                unsigned char motion_type = component->m_rigid_bodies[body_index].m_rigid_body->get_motion()->get_type_id();
                bool is_dynamic = simulation_game_generics_rigid_body_is_dynamic(motion_type);
                unsigned long bit = 1 << body_index;
                if (is_dynamic)
                    rigid_body_mask |= bit;
                else
                    rigid_body_mask &= ~bit;
            }
        }
    }
    return rigid_body_mask;
}

// @address 0x434608 ?handle_object_update@c_simulation_generic_entity_definition@@UAAKJKJPAX@Z
unsigned long c_simulation_generic_entity_definition::handle_object_update(long object_index, unsigned long update_mask, long state_data_size, void *state_data)
{
    s_simulation_generic_state_data *state = (s_simulation_generic_state_data *)state_data;
    unsigned long result = 0;
    if (update_mask & 0x4000)
    {
        unsigned long rigid_body_mask = simulation_generic_object_get_rigid_body_mask(object_index);
        real_matrix4x3 transforms[8] = {0};
        if (rigid_body_mask)
        {
            object_datum *object = ((s_object_header *)tls_object_header_data()->data)[(unsigned short)object_index].datum;
            c_havok_component *component = c_havok_component_get(object->havok_component_index);
            for (unsigned long i = 0; i < 8; i++)
            {
                if (rigid_body_mask & (1 << i))
                    ((c_havok_component *)component)->get_rigid_body_transform(i, &transforms[i]);
            }
        }
        if (rigid_body_mask && (rigid_body_mask != state->mask || memcmp(transforms, state->transforms, sizeof(transforms))))
        {
            state->mask = rigid_body_mask;
            memcpy(state->transforms, transforms, sizeof(transforms));
            result |= 0x4000;
        }
    }
    unsigned long object_update_mask = update_mask & 0x3fff;
    result |= c_simulation_object_entity_definition::object_handle_update(object_index, object_update_mask, (s_simulation_object_state_data *)state);
    return result;
}
static const float k_simulation_generic_orientation_threshold = 0.9f;

// @address 0x434848 ?apply_object_update@c_simulation_generic_entity_definition@@UAAXJKJPBX@Z
void c_simulation_generic_entity_definition::apply_object_update(long object_index, unsigned long update_mask, long state_data_size, const void *state_data)
{
    const s_simulation_generic_state_data *state = (const s_simulation_generic_state_data *)state_data;
    if (update_mask & 0x3fff)
        c_simulation_object_entity_definition::object_apply_update(object_index, update_mask & 0x3fff, (const s_simulation_object_state_data *)state);
    if (update_mask & 0x4000)
    {
        object_activate(object_index);
        unsigned long rigid_body_mask = simulation_generic_object_get_rigid_body_mask(object_index);
        if (rigid_body_mask != 0 && rigid_body_mask == state->mask)
        {
            object_datum *object = (object_datum *)object_get_and_verify_type(object_index, ~0);
            c_havok_component *component = (c_havok_component *)c_havok_component_get(object->havok_component_index);
            bool transforms_changed = false;
            for (unsigned long rigid_body_index = 0; rigid_body_index < 8; rigid_body_index++)
            {
                if (state->mask & (1 << rigid_body_index))
                {
                    const real_matrix4x3 *desired = (const real_matrix4x3 *)&state->transforms[rigid_body_index];
                    real_matrix4x3 current;
                    component->get_rigid_body_transform(rigid_body_index, &current);
                    bool position_changed = distance_squared3d(&current.position, &desired->position) > 0.5f * 0.5f;
                    bool orientation_changed =
                        dot_product3d(&current.rotation.forward, &desired->rotation.forward) < k_simulation_generic_orientation_threshold ||
                        dot_product3d(&current.rotation.up, &desired->rotation.up) < k_simulation_generic_orientation_threshold ||
                        dot_product3d(&current.rotation.left, &desired->rotation.left) < k_simulation_generic_orientation_threshold;
                    if (position_changed || orientation_changed)
                    {
                        component->set_rigid_body_transform(rigid_body_index, desired);
                        transforms_changed = true;
                    }
                }
            }
            if (transforms_changed)
                component->align_constraint_positions();
            object_set_at_rest(object_index, false);
        }
    }
}

// @address 0x434788 ?create_object@c_simulation_generic_entity_definition@@UAAJJPBXPAKJ0@Z
long c_simulation_generic_entity_definition::create_object(long p0, void const *p1, unsigned long *p2, long p3, const void *p4)
{
    const s_simulation_generic_creation_data *creation = (const s_simulation_generic_creation_data *)p1;
    long result = -1;
    object_placement_data placement;
    if (creation->definition_index != -1
        && (tag_try_and_get_unsafe(0x7363656e, creation->definition_index) != 0
            || tag_try_and_get_unsafe(0x626c6f63, creation->definition_index) != 0))
    {
        ::c_simulation_object_entity_definition::object_setup_placement_data((s_simulation_object_creation_data const *)p1,
            (s_simulation_object_state_data const *)p4, (unsigned long *)(p2), (object_placement_data *)&placement);
        placement.model_variant_index = creation->variant_name;
        result = c_simulation_object_entity_definition::object_create_object(
                         (const s_simulation_object_creation_data *)p1, (const s_simulation_object_state_data *)p4, p2, &placement);
        if (result != -1)
            object_set_at_rest(result, true);
    }
    return result;
}

// @address 0x42e748 ?entity_type@c_simulation_generic_entity_definition@@UAA?AW4e_simulation_entity_type@@XZ
e_simulation_entity_type c_simulation_generic_entity_definition::entity_type()
{
    return _simulation_entity_type_generic;
}

// @address 0x42e7c0 ?creation_data_size@c_simulation_generic_entity_definition@@UAAJXZ
long c_simulation_generic_entity_definition::creation_data_size()
{
    return 20;
}

// @address 0x42e678 ?update_flag_count@c_simulation_generic_entity_definition@@UAAJXZ
long c_simulation_generic_entity_definition::update_flag_count()
{
    return 15;
}

// @address 0x434458 ?initial_update_mask@c_simulation_generic_entity_definition@@UAAKXZ
unsigned long c_simulation_generic_entity_definition::initial_update_mask()
{
    return 0x5b3f;
}

// @address 0x42d380 ?entity_type_is_gameworld_object@c_simulation_generic_entity_definition@@UAA_NXZ
bool c_simulation_generic_entity_definition::entity_type_is_gameworld_object()
{
    return true;
}

// @address 0x433fa0 ?entity_can_be_created@c_simulation_generic_entity_definition@@UAA_NPBUs_simulation_entity@@PBUs_simulation_view_telemetry_data@@@Z
bool c_simulation_generic_entity_definition::entity_can_be_created(const s_simulation_entity *entity,
    const s_simulation_view_telemetry_data *telemetry)
{
    return object_can_be_created(0, static_cast<const s_simulation_object_state_data *>(entity->state_data), telemetry);
}

// @address 0x42cc28 ?creation_minimum_required_bits@c_simulation_generic_entity_definition@@UAAJPBUs_simulation_entity@@PBUs_simulation_view_telemetry_data@@@Z
long c_simulation_generic_entity_definition::creation_minimum_required_bits(const s_simulation_entity *entity,
    const s_simulation_view_telemetry_data *telemetry)
{
    return 142;
}

// @address 0x435738 ?entity_creation_encode@c_simulation_generic_entity_definition@@UAAXJPBXPBUs_simulation_view_telemetry_data@@PAVc_bitstream@@_N@Z
void c_simulation_generic_entity_definition::entity_creation_encode(long creation_data_size, const void *creation_data,
    const s_simulation_view_telemetry_data *telemetry, c_bitstream *packet, bool final_update)
{
    object_entity_variant_creation_encode(creation_data_size, creation_data, telemetry, packet, final_update);
}

bool c_simulation_generic_entity_definition::entity_creation_decode(long creation_data_size, void *creation_data, c_bitstream *packet,
    bool quantize)
{
    return object_entity_variant_creation_decode(creation_data_size, creation_data, packet, quantize);
}

// @address 0x42d380 ?entity_creation_lossy_compare@c_simulation_generic_entity_definition@@UAA_NPAX0J@Z
bool c_simulation_generic_entity_definition::entity_creation_lossy_compare(void *first, void *second, long size)
{
    return true;
}

void c_simulation_generic_entity_definition::rotate_entity_indices(const s_simulation_entity *entity)
{
    object_entity_rotate_indices(entity);
}

// @address 0x5bec98 ?prepare_creation_data_for_gameworld@c_simulation_generic_entity_definition@@UAAXJPAX@Z
void c_simulation_generic_entity_definition::prepare_creation_data_for_gameworld(long size, void *creation_data)
{
}

// @address 0x434748 ?prepare_state_data_for_gameworld@c_simulation_generic_entity_definition@@UAAXKJPAX@Z
void c_simulation_generic_entity_definition::prepare_state_data_for_gameworld(unsigned long update_mask, long size, void *state)
{
    object_entity_prepare_state_data(update_mask, size, state);
}

void c_simulation_generic_entity_definition::build_object_creation_data(long object_index, long creation_data_size, void *creation_data)
{
    object_entity_variant_build_creation_data(object_index, creation_data_size, creation_data);
}

// @address 0x42d298 ?handle_delete_object@c_simulation_generic_entity_definition@@UAA_NJ@Z
bool c_simulation_generic_entity_definition::handle_delete_object(long object_index)
{
    return false;
}

// @address 0x42d380 ?promote_object_to_authority@c_simulation_generic_entity_definition@@UAA_NJ@Z
bool c_simulation_generic_entity_definition::promote_object_to_authority(long object_index)
{
    return true;
}

// @address 0x42d298 ?ensure_object_position_update_quantization_inside_bsp@c_simulation_generic_entity_definition@@UAA_NXZ
bool c_simulation_generic_entity_definition::ensure_object_position_update_quantization_inside_bsp()
{
    return false;
}

void c_simulation_generic_entity_definition::write_update_description_to_string(const s_simulation_entity *entity,
    const s_entity_update_data *update, long size, char *buffer)
{
}

// @address 0x45fca8 ?entity_type@c_simulation_generic_garbage_entity_definition@@UAA?AW4e_simulation_entity_type@@XZ
e_simulation_entity_type c_simulation_generic_garbage_entity_definition::entity_type()
{
    return _simulation_entity_type_generic_garbage;
}

// @address 0x4344e0 ?build_object_creation_data@c_simulation_vehicle_entity_definition@@UAAXJJPAX@Z
void c_simulation_vehicle_entity_definition::build_object_creation_data(long object_index, long creation_data_size, void *creation_data)
{
    object_entity_variant_build_creation_data(object_index, creation_data_size, creation_data);
}


// @address 0x434ae8 ?entity_creation_decode@c_simulation_vehicle_entity_definition@@UAA_NJPAXPAVc_bitstream@@_N@Z
bool c_simulation_vehicle_entity_definition::entity_creation_decode(long creation_data_size, void *creation_data, c_bitstream *packet,
    bool final_update)
{
    return object_entity_variant_creation_decode(creation_data_size, creation_data, packet, final_update);
}
