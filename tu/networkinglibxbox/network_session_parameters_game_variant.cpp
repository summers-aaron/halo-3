// networkinglibxbox/network_session_parameters_game_variant.obj: from the records 0x413140 0x4132a8 0x4132f8
// @compiler xdk6130
// @flags /TP /O2 /Oc /J
#include "game/game_variant.h"
#include <string.h>
#include "game/game_engine_variant.h"
#include "memory/bitstream.h"
#include "networking/session/network_session_parameters.h"
#include "networking/session/network_session_parameter_access.h"
#include "math/real_math_types.h"




struct s_owner;


bool c_network_session_parameter_game_variant::set(const c_game_variant *variant)
{
    bool result = false;
    if (this->set_allowed())
    {
        if (!variant)
        {
            if (m_data.valid)
            {
                m_data.valid = false;
                memset(&m_data.game_variant, 0, sizeof(m_data.game_variant));
                set_update_required();
            }
            result = true;
        }
        else if (!variant_valid(variant))
        {
            game_engine_variant_describe_invalidity(variant);
        }
        else
        {
            m_data.valid = true;
            if (!variant->is_equal_to(&m_data.game_variant) || !get_allowed())
            {
                m_data.game_variant.copy_from_and_validate(variant);
                set_update_required();
            }
            result = true;
        }
    }
    return result;
}

void c_network_session_parameter_game_variant::encode_update(c_bitstream *packet, const s_network_session_parameter_game_variant *variant)
{
    packet->write_bool("variant-valid", variant->valid);
    if (variant->valid)
    {
        variant->game_variant.encode(packet);
    }
}

bool c_network_session_parameter_game_variant::decode_change_request(c_bitstream *packet, s_network_session_parameter_game_variant *variant)
{
    bool success = true;
    variant->valid = packet->read_bool("variant-valid");
    if (variant->valid)
    {
        success = success && game_variant_decode(&variant->game_variant, packet);
        success = success && variant_valid(&variant->game_variant);
    }
    else
    {
        memset(&variant->game_variant, 0, sizeof(variant->game_variant));
    }
    return success && !packet->error_occurred();
}
