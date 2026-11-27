#ifndef NETWORKING_SESSION_NETWORK_SESSION_PARAMETER_VARIANTS_H
#define NETWORKING_SESSION_NETWORK_SESSION_PARAMETER_VARIANTS_H

#include "networking/session/network_session_parameter_variant_types.h"
#include "networking/session/network_session_parameters_chunked.h"
#include "game/game_engine_variant_validation.h"

class c_network_session_parameter_game_variant : public c_network_session_parameter_chunked<s_network_session_parameter_game_variant>
{
public:
    using c_network_session_parameter_base::m_flags;
    using c_network_session_parameter_base::set_desires_change_request;
    c_network_session_parameter_game_variant();

    // Returns the current game variant, or null when unavailable
    const c_game_variant *get() const
    {
        const c_game_variant *game_variant = 0;
        if (get_allowed())
        {
            if (m_data.valid)
                game_variant = &m_data.game_variant;
        }
        return game_variant;
    }
    bool set(const c_game_variant *game_variant);
    bool request_change(const c_game_variant *game_variant);

private:
    __forceinline bool variant_valid(const c_game_variant *variant)
    {
        return variant->m_game_engine_index != 0 && game_engine_variant_is_valid(variant);
    }
    virtual void encode_update(c_bitstream *packet, const s_network_session_parameter_game_variant *data);
    virtual bool decode_update(c_bitstream *packet, s_network_session_parameter_game_variant *data);
    virtual void encode_change_request(c_bitstream *packet, const s_network_session_parameter_game_variant *data);
    virtual bool decode_change_request(c_bitstream *packet, s_network_session_parameter_game_variant *data);
};

class c_network_session_parameter_map_variant : public c_network_session_parameter_chunked<s_network_session_parameter_map_variant>
{
public:
    c_network_session_parameter_map_variant();

    // Returns the current map variant, or null when unavailable
    const c_map_variant *get() const
    {
        const c_map_variant *map_variant = 0;
        if (get_allowed())
        {
            if (m_data.valid)
                map_variant = &m_data.map_variant;
        }
        return map_variant;
    }
    bool set(const c_map_variant *map_variant);
    bool request_change(const c_map_variant *map_variant);

private:
    bool variant_valid(const c_map_variant *map_variant);
    virtual void encode_update(c_bitstream *packet, const s_network_session_parameter_map_variant *data);
    virtual bool decode_update(c_bitstream *packet, s_network_session_parameter_map_variant *data);
    virtual void encode_change_request(c_bitstream *packet, const s_network_session_parameter_map_variant *data);
    virtual bool decode_change_request(c_bitstream *packet, s_network_session_parameter_map_variant *data);
};

class c_network_session_parameter_saved_film_game_options : public c_network_session_parameter_chunked<s_network_session_parameter_saved_film_game_options>
{
public:
    using c_network_session_parameter_base::set_desires_change_request;
private:
    virtual void encode_update(c_bitstream *packet, const s_network_session_parameter_saved_film_game_options *data);
    virtual bool decode_update(c_bitstream *packet, s_network_session_parameter_saved_film_game_options *data);
    virtual void encode_change_request(c_bitstream *packet, const s_network_session_parameter_saved_film_game_options *data);
    virtual bool decode_change_request(c_bitstream *packet, s_network_session_parameter_saved_film_game_options *data);
};

// Defined in tu/networkinglibxbox/network_session_parameters.cpp.
template<> void c_network_session_parameter_chunked<s_network_session_parameter_map_variant>::clear_internal();
template<> void c_network_session_parameter_chunked<s_network_session_parameter_saved_film_game_options>::notify_set_update_required();

#endif
