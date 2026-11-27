#ifndef SIMULATION_GAME_INTERFACE_SIMULATION_GAME_OBJECT_ENTITY_DEFINITION_H
#define SIMULATION_GAME_INTERFACE_SIMULATION_GAME_OBJECT_ENTITY_DEFINITION_H

#include "simulation/game_interface/simulation_game_device_machines.h"
#include "memory/bitstream.h"
#include "networking/simulation/simulation_entity_definition_types.h"
#include "networking/simulation/simulation_view_telemetry.h"
#include "simulation/game_interface/simulation_game_object_data_types.h"

struct object_placement_data;
struct s_entity_update_data;
struct s_simulation_object_creation_data;
struct s_simulation_object_state_data;
struct s_simulation_view_telemetry_data;
class c_bitstream;
class c_simulation_generic_entity_definition;
class c_simulation_item_entity_definition;
class c_simulation_object_entity_definition;

extern "C" bool simulation_item_decode_update(c_simulation_item_entity_definition *entity_definition, bool initial_update,
    unsigned long *update_mask, void *state_data, c_bitstream *packet, bool final_update);

class c_simulation_object_entity_definition : public c_simulation_entity_definition
{
#if !defined(__clang__)
    virtual const char *entity_type_name();
#endif
    friend class c_simulation_projectile_entity_definition;
    friend class c_simulation_projectile_attached_event_definition;
    friend class c_simulation_device_entity_definition;
    friend class c_simulation_generic_entity_definition;
    friend class c_simulation_weapon_entity_definition;
    friend class c_simulation_unit_entity_definition;
    friend class c_simulation_vehicle_entity_definition;
    friend class c_simulation_item_entity_definition;
    friend bool simulation_item_decode_update(c_simulation_item_entity_definition *entity_definition, bool initial_update,
        unsigned long *update_mask, void *state_data, c_bitstream *packet, bool final_update);

public:
    virtual void build_object_creation_data(long object_index, long creation_data_size, void *creation_data) = 0;
    virtual unsigned long handle_object_update(long object_index, unsigned long update_mask, long argument, void *data) = 0;
    virtual bool object_required_to_join_game(long object_index) = 0;
    virtual long create_object(long creation_data_size, const void *creation_data, unsigned long *update_mask,
        long state_data_size, const void *state_data) = 0;
    virtual bool handle_delete_object(long object_index) = 0;
    virtual void apply_object_update(long object_index, unsigned long update_mask, long argument, const void *data) = 0;
    virtual bool promote_object_to_authority(long object_index) = 0;
    virtual bool entity_replication_required_for_view_activation(const s_simulation_entity *entity);
    virtual void build_creation_data(long entity_index, long creation_data_size, void *creation_data);
    virtual bool build_updated_state_data(const s_simulation_entity *entity, unsigned long *update_mask, long argument, void *data);
    virtual bool create_game_entity(long gamestate_index, long creation_data_size, const void *creation_data,
        unsigned long update_mask, long state_data_size, const void *state_data);
    virtual bool update_game_entity(long entity_index, unsigned long update_mask, long argument, const void *data);
    virtual bool delete_game_entity(long entity_index);
    virtual bool promote_game_entity_to_authority(long entity_index);
    virtual bool gameworld_attachment_valid(long entity_index);
    // PC: left to each object definition, see entity_type_name.
#if !defined(__clang__)
    bool entity_type_is_gameworld_object();
#endif
    virtual bool ensure_object_position_update_quantization_inside_bsp() = 0;
    __forceinline void object_build_creation_data(long object_index, s_simulation_object_creation_data *creation);
    __forceinline bool object_can_be_created(const s_simulation_object_creation_data *creation,
        const s_simulation_object_state_data *state, const s_simulation_view_telemetry_data *telemetry)
    {
        bool result = true;
        if (state->parent_entity_index != -1)
        {
            c_simulation_view_telemetry_provider *provider = telemetry->provider;
            result = provider->entity_is_active(state->parent_entity_index);
        }
        return result;
    }
private:
    void object_attach_to_initial_parent(long object_index,
        const s_simulation_object_creation_data *creation_data, const s_simulation_object_state_data *state_data);
protected:
    bool object_entity_can_be_created(const s_simulation_entity *entity, const s_simulation_view_telemetry_data *telemetry);
    // match: 0x43ae28: Keep object_creation_decode and the overflow test in the virtual decoder. An out-of-line helper leaves a tail branch here.
    __forceinline bool object_entity_creation_decode(long creation_data_size, void *creation_data, c_bitstream *packet, bool quantize)
    {
        bool decoded = c_simulation_object_entity_definition::object_creation_decode((s_simulation_object_creation_data *)creation_data, packet, quantize);
        bool overrun = packet->m_current.current_stream_bit_position > (packet->m_data_size_bytes << 3);
        return !overrun && decoded;
    }
    void object_entity_build_creation_data(long object_index, long creation_data_size, void *creation_data);
    void object_entity_rotate_indices(const s_simulation_entity *entity);
    void object_entity_prepare_state_data(unsigned long update_mask, long state_data_size, void *state_data);
    // generic and vehicle creation data: the object creation data and a variant name
    void object_entity_variant_creation_encode(long creation_data_size, const void *creation_data,
        const s_simulation_view_telemetry_data *telemetry, c_bitstream *packet, bool final_update);
    bool object_entity_variant_creation_decode(long creation_data_size, void *creation_data, c_bitstream *packet,
        bool final_update);
    void object_entity_variant_build_creation_data(long object_index, long creation_data_size, void *creation_data);
    static void object_creation_encode(const s_simulation_object_creation_data *creation_data, c_bitstream *packet, bool quantize);
    static void object_calculate_update_requirements(const s_simulation_entity *entity,
        const s_entity_update_data *update, long *minimum_bits);
    static bool object_state_compare(s_simulation_object_state_data *first, s_simulation_object_state_data *second);
    static bool object_build_baseline_state_data(
        const s_simulation_object_creation_data *creation_data, s_simulation_object_state_data *state_data);
    static bool object_creation_decode(s_simulation_object_creation_data *creation_data, c_bitstream *packet, bool quantize);
    static bool object_update_encode(bool initial_update, unsigned long update_mask, unsigned long *updated_mask,
        const s_simulation_view_telemetry_data *telemetry, const s_simulation_object_state_data *state_data, c_bitstream *packet,
        long maximum_bits, bool final_update, bool is_replicated);
    static bool object_update_decode(bool initial_update, unsigned long *update_mask, s_simulation_object_state_data *state_data,
        c_bitstream *packet, bool is_replicated);
    static unsigned long object_handle_update(long object_index, unsigned long update_mask, s_simulation_object_state_data *state_data);
    void object_apply_update(long object_index, unsigned long update_mask, const s_simulation_object_state_data *state_data);
    static bool object_setup_placement_data(const s_simulation_object_creation_data *creation_data,
        const s_simulation_object_state_data *state_data, unsigned long *flags, object_placement_data *placement);
    long object_create_object(const s_simulation_object_creation_data *creation_data, const s_simulation_object_state_data *state_data,
        unsigned long *flags, object_placement_data *placement);
};

#endif // SIMULATION_GAME_INTERFACE_SIMULATION_GAME_OBJECT_ENTITY_DEFINITION_H
