// networkinglibxbox/network_session_parameters_map_variant.obj: from the records 0x412e50 0x412ff0 0x413040 0x413100 0x413120 0x413130 0x413668
// @flags /TP /O2 /Oc
#include "memory/bitstream.h"
#include "game/game_types.h"
// @address 0x412e50 ?set@c_network_session_parameter_map_variant@@QAA_NPBVc_map_variant@@@Z
// @address 0x412ff0 ?encode_update@c_network_session_parameter_map_variant@@EAAXPAVc_bitstream@@PBUs_network_session_parameter_map_variant@@@Z
// @address 0x413120 ?encode_change_request@c_network_session_parameter_map_variant@@EAAXPAVc_bitstream@@PBUs_network_session_parameter_map_variant@@@Z
// @address 0x413130 ?decode_change_request@c_network_session_parameter_map_variant@@EAA_NPAVc_bitstream@@PAUs_network_session_parameter_map_variant@@@Z
#include "cseries/cseries_types.h"
#include <string.h>
#include "main/levels.h"
#include "networking/logic/network_session_interface.h"
#include "scenario/scenario_map_variant.h"

#include "networking/session/network_session_parameters.h"
#include "networking/session/network_session_parameter_access.h"
#include "networking/session/network_session_parameter_variant_types.h"


bool c_network_session_parameter_map_variant::set(const c_map_variant *variant)
{
    bool result = false;
    if (this->set_allowed()) {
        if (!variant) {
            if (m_data.valid) {
                m_data.valid = false;
                memset(&m_data.map_variant, 0, sizeof(m_data.map_variant));
                set_update_required();
            }
            result = true;
        } else {
            m_data.valid = true;
            if (variant->version != 12) {
                c_map_variant temporary;
                e_campaign_id campaign_id;
                e_map_id map_id;
                if (network_life_cycle_session_get_map(&campaign_id, &map_id))
                    temporary.create_default(map_id);
                else
                    temporary.create_default((e_map_id)levels_get_default_multiplayer_map_id());
                m_data.map_variant = temporary;
            } else {
                c_map_variant *current_variant = &m_data.map_variant;
                *current_variant = *variant;
                if (!map_variant_is_valid((const s_map_variant_validation *)current_variant))
                    current_variant->create_default((e_map_id)current_variant->map_id);
            }
            set_update_required();
            result = true;
        }
    }
    return result;
}

void c_network_session_parameter_map_variant::encode_update(c_bitstream *packet, const s_network_session_parameter_map_variant *data)
{
    packet->write_bool("map-variant-valid", data->valid);
    if (data->valid)
        data->map_variant.encode(packet);
}

// @address 0x413100 network_session_parameter_map_variant_reset_data
extern "C" void network_session_parameter_map_variant_reset_data(
    void *, s_network_session_parameter_map_variant *data, const s_network_session_parameter_map_variant *variant)
{
    // Clear the padding in the validity word before resetting the map payload.
    unsigned char *const validity_padding = reinterpret_cast<unsigned char *>(data) + sizeof(data->valid);
    validity_padding[2] = 0;
    validity_padding[1] = 0;
    validity_padding[0] = 0;
    map_variant_reset_transient_state(&data->map_variant, (c_map_variant *)&variant->map_variant);
}

void c_network_session_parameter_map_variant::encode_change_request(c_bitstream *packet, const s_network_session_parameter_map_variant *data)
{
    encode_update(packet, data);
}

bool c_network_session_parameter_map_variant::decode_change_request(c_bitstream *packet, s_network_session_parameter_map_variant *data)
{
    return decode_update(packet, data);
}

// @address 0x413668 network_session_parameter_map_variant_reset_transmit_data
extern "C" void network_session_parameter_map_variant_reset_transmit_data(
    void *, s_network_session_parameter_saved_film_game_options *data,
    const s_network_session_parameter_saved_film_game_options *variant)
{
    map_variant_reset_transient_state(&data->options.map_variant, (c_map_variant *)&variant->options.map_variant);
}

// @address 0x413040 ?decode_update@c_network_session_parameter_map_variant@@EAA_NPAVc_bitstream@@PAUs_network_session_parameter_map_variant@@@Z
bool c_network_session_parameter_map_variant::decode_update(c_bitstream *packet, s_network_session_parameter_map_variant *data)
{
    bool success = true;
    data->valid = packet->read_bool("map-variant-valid");
    if (data->valid)
        success = success && map_variant_decode(&data->map_variant, packet);
    else
        memset(&data->map_variant, 0, sizeof(data->map_variant));
    success = success && !packet->error_occurred();
    return success;
}

