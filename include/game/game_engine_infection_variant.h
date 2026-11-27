#ifndef GAME_GAME_ENGINE_INFECTION_VARIANT_H
#define GAME_GAME_ENGINE_INFECTION_VARIANT_H

#include "game/game_engine_base_variant.h"
#include "game/game_engine_variant_definitions.h"

enum e_infection_variant_flags
{
    _infection_variant_flags_respawn_on_haven_move = 0,
    k_infection_variant_flags
};

class c_infection_variant_flags
{
public:
    __forceinline void clear() { m_storage = 0; }
    __forceinline void set(e_infection_variant_flags bit, bool enabled)
    {
        if (enabled)
            m_storage |= 1 << bit;
        else
            m_storage &= ~(1 << bit);
    }
private:
    unsigned char m_storage;
};

class c_game_engine_infection_variant : public c_game_engine_base_variant
{
protected:
    virtual long get_game_engine_name_string_id() const;
    virtual long get_game_engine_default_description_string_id() const;
public:
    virtual void initialize();
    virtual void validate();
    virtual void encode(c_bitstream *packet) const;
    virtual void decode(c_bitstream *packet);
    virtual bool can_be_cast_to(e_game_engine_type engine_type, const void **variant) const;
    void set(const c_game_engine_infection_variant *variant, bool force);
    void set(const struct s_game_engine_infection_variant_definition *definition);

    // Padding between the base variant fields and the infection fields.
    char m_base_padding[4];
    union { signed char m_flags; unsigned char m_flags_byte; c_infection_variant_flags m_infection_flags; };
    signed char m_safe_havens;
    signed char m_next_zombie;
    signed char m_initial_zombie_count;
    unsigned short m_safe_haven_movement_time;
    union
    {
        unsigned short m_points;
        unsigned char m_points_bytes[2];
        signed char m_points_signed_bytes[2];
    };
    union
    {
        unsigned short m_limit;
        unsigned char m_limit_bytes[2];
        signed char m_limit_signed_bytes[2];
    };
    union
    {
        unsigned short m_time;
        unsigned char m_time_bytes[2];
        signed char m_time_signed_bytes[2];
    };
    c_player_traits m_traits;
    c_player_traits m_first_zombie_traits;
    c_player_traits m_safe_haven_defender_traits;
    c_player_traits m_last_human_traits;
};
struct s_game_engine_infection_variant_definition
{
    s_game_engine_base_variant_definition base;
    unsigned long flags;
    short haven, next, initial, movement, points, limit, time;
    short pad;
    long traits;
};

typedef char infection_variant_flags_offset[(sizeof(void *) != 4 || offsetof(c_game_engine_infection_variant, m_flags) == 0x1b0) ? 1 : -1];
typedef char infection_variant_traits_offset[(sizeof(void *) != 4 || offsetof(c_game_engine_infection_variant, m_traits) == 0x1bc) ? 1 : -1];
typedef char infection_variant_size[(sizeof(void *) != 4 || sizeof(c_game_engine_infection_variant) == 0x22c) ? 1 : -1];
typedef char infection_variant_definition_size[(sizeof(long) != 4 || sizeof(s_game_engine_infection_variant_definition) == 0x50) ? 1 : -1];

#endif
