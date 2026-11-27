#ifndef GAME_GAME_ENGINE_TERRITORIES_VARIANT_H
#define GAME_GAME_ENGINE_TERRITORIES_VARIANT_H

#include "game/game_engine_base_variant.h"

struct s_game_engine_territories_variant_definition;

class c_game_engine_territories_variant : public c_game_engine_base_variant
{
public:
    enum e_territories_flags
    {
        _territories_flags_one_sided = 0,
        _territories_flags_lock_after_first_capture,
        k_territories_flags
    };

    enum e_territories_respawn_on_capture_settings
    {
        _territories_respawn_on_capture_disabled = 0,
        _territories_respawn_on_capture_ally,
        _territories_respawn_on_capture_enemy,
        _territories_respawn_on_capture_all,
        k_territories_respawn_on_capture_settings
    };

protected:
    virtual long get_game_engine_name_string_id() const;
    virtual long get_game_engine_default_description_string_id() const;
public:
    virtual void initialize();
    virtual void validate();
    virtual void encode(c_bitstream *packet) const;
    virtual void decode(c_bitstream *packet);
    virtual bool can_be_cast_to(e_game_engine_type engine_type, const void **variant) const;

    void set(const c_game_engine_territories_variant *source);
    void set(const s_game_engine_territories_variant_definition *definition);
    __forceinline void set_one_sided(bool enabled) { m_flags.set(_territories_flags_one_sided, enabled); }
    __forceinline void set_lock_after_first_capture(bool enabled) { m_flags.set(_territories_flags_lock_after_first_capture, enabled); }
    __forceinline bool get_one_sided() const { return m_flags.test(_territories_flags_one_sided); }
    __forceinline bool get_lock_after_first_capture() const { return m_flags.test(_territories_flags_lock_after_first_capture); }
    __forceinline e_territories_respawn_on_capture_settings get_respawn_on_capture() const { return m_respawn_on_capture; }
    __forceinline short get_capture_time() const { return m_capture_time; }
    __forceinline short get_sudden_death_time() const { return sudden_death_time; }
    __forceinline bool get_sudden_death_enabled() const { return sudden_death_time != 0; }

    char m_base_padding[4];
    union { unsigned short flags; c_flags_no_init<e_territories_flags, short, k_territories_flags> m_flags; };
    c_enum<e_territories_respawn_on_capture_settings,
        short,
        _territories_respawn_on_capture_disabled,
        k_territories_respawn_on_capture_settings> m_respawn_on_capture;
    short m_capture_time;
    short sudden_death_time;
    c_player_traits m_defender_traits;
    c_player_traits m_attacker_traits;
};

typedef char territories_variant_layout_1[(sizeof(void *) != 4 || ((offsetof(c_game_engine_territories_variant, m_flags) == 0x1b0))) ? 1 : -1];
typedef char territories_variant_layout_2[(sizeof(void *) != 4 || ((offsetof(c_game_engine_territories_variant, m_defender_traits) == 0x1b8))) ? 1 : -1];
typedef char territories_variant_layout_3[(sizeof(void *) != 4 || ((offsetof(c_game_engine_territories_variant, m_attacker_traits) == 0x1d4))) ? 1 : -1];
typedef char territories_variant_layout_4[(sizeof(void *) != 4 || ((offsetof(c_game_engine_territories_variant, m_respawn_on_capture) == 0x1b2))) ? 1 : -1];

#endif
