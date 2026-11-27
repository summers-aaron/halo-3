// blamlibxbox/game_options.obj: from the records 0x1c24a0 0x1c24a8 0x1c24b0 0x1c24c0 0x1c24e8 0x1c24f0 0x1c24f8 0x1c2520 0x1c2530 0x1c2538 0x1c2540 0x1c2568 0x1c2570 0x1c2578 0x1c25a0 0x1c25a8 0x1c25b0 0x1c25d8 0x1c25e0 0x1c25e8 0x1c2610 0x1c2620 0x1c2628 0x1c2630 0x1c2658 0x1c2660 0x1c2668 0x1c2690 0x1c2698 0x1c26a0 0x1c26c8 0x1c26d0 0x1c26d8 0x1c2700 0x1c27c0 0x1c2878 0x1c28d8 0x1c2950 0x1c29c8 0x1c2ac0 0x1c2b68 0x1c2d28 0x1c2db0 0x1c2e10 0x1c2ed0 0x1c3000 0x1c3138 0x1c3b28 0x1c6298 0x2655e0 0x265878
// @flags /TP /O2
#include "cseries/cseries_wrapped.h"
#include "cseries/cseries.h"
#include "text/unicode.h"
#include <new>
#pragma pack(push, 1)
#include "saved_games/saved_game_item_metadata_packed.h"
#pragma pack(pop)
#include "game/game_globals.h"
#include "game/game_globals_storage.h"
#include "memory/bitstream.h"
#include "game/game_types.h"
#include "cache/security_functions.h"
#include "game/game_engine_util.h"
#include "game/game_engine_variant.h"
#include "networking/tools/network_blf.h"
#include "game/map_variant.h"
#include "game/player_traits.h"
#include "game/multiplayer_definitions.h"
#include "game/game_options.h"
#include "networking/session/network_session_parameters.h"
#include "game/game_variant_parameter_types.h"
#include "interface/text_value_pair_types.h"
enum e_game_variant_parameter;
// @address 0x1c2ac0 ?is_equal_to@c_game_variant@@QBA_NPBV1@@Z
// @address 0x1c29c8 ?copy_from_and_validate@c_game_variant@@QAAXPBV1@@Z
// @address 0x1c2ed0 ?copy_to_and_validate@s_blffile_game_variant@@QBA_NPAVc_game_variant@@PA_N@Z
// @address 0x1c3000 ?copy_to_and_validate@s_blffile_map_variant@@QBA_NPAVc_map_variant@@PA_N@Z
// @address 0x1c2b68 ?recreate_variant_vtable_for_game_engine_index@c_game_variant@@QAAXW4e_game_engine_type@@@Z
// @address 0x1c2db0 ?set_game_engine_index@c_game_variant@@QAAXW4e_game_engine_type@@@Z
// @address 0x1c1258 ?get_game_engine_default_description_string_id@c_game_engine_base_variant@@MBAJXZ
// @address 0x1c1258 ?get_game_engine_name_string_id@c_game_engine_base_variant@@MBAJXZ
// @address 0x1c24b0 ?get_score_to_win_round@c_game_engine_slayer_variant@@UBAJXZ
// @address 0x1c24b0 ?get_score_to_win_round@c_game_engine_juggernaut_variant@@UBAJXZ
// @address 0x1c24b0 ?get_score_to_win_round@c_game_engine_vip_variant@@UBAJXZ
// @address 0x1c2520 ?get_score_to_win_round@c_game_engine_king_variant@@UBAJXZ
// @address 0x1c2520 ?get_score_to_win_round@c_game_engine_oddball_variant@@UBAJXZ
// @address 0x1c2610 ?get_score_to_win_round@c_game_engine_ctf_variant@@UBAJXZ
// @address 0x1c2610 ?get_score_to_win_round@c_game_engine_assault_variant@@UBAJXZ
// @address 0x264d40 ?initialize@c_game_engine_base_variant@@UAAXXZ
// @address 0x264dc8 ?validate@c_game_engine_base_variant@@UAAXXZ
// @address 0x264e28 ?encode@c_game_engine_base_variant@@UBAXPAVc_bitstream@@@Z
// @address 0x265048 ?decode@c_game_engine_base_variant@@UAAXPAVc_bitstream@@@Z
// @address 0x267920 ?initialize@c_game_engine_slayer_variant@@UAAXXZ
// @address 0x267a00 ?validate@c_game_engine_slayer_variant@@UAAXXZ
// @address 0x267f08 ?encode@c_game_engine_slayer_variant@@UBAXPAVc_bitstream@@@Z
// @address 0x268030 ?decode@c_game_engine_slayer_variant@@UAAXPAVc_bitstream@@@Z
// @address 0x268410 ?initialize@c_game_engine_oddball_variant@@UAAXXZ
// @address 0x268540 ?validate@c_game_engine_oddball_variant@@UAAXXZ
// @address 0x268598 ?custom_team_score_stats@c_game_engine_oddball_variant@@UBAXJJJ@Z
// @address 0x268930 ?encode@c_game_engine_oddball_variant@@UBAXPAVc_bitstream@@@Z
// @address 0x268a10 ?decode@c_game_engine_oddball_variant@@UAAXPAVc_bitstream@@@Z
// @address 0x268c78 ?initialize@c_game_engine_ctf_variant@@UAAXXZ
// @address 0x268da0 ?validate@c_game_engine_ctf_variant@@UAAXXZ
// @address 0x2690b8 ?encode@c_game_engine_ctf_variant@@UBAXPAVc_bitstream@@@Z
// @address 0x269180 ?decode@c_game_engine_ctf_variant@@UAAXPAVc_bitstream@@@Z
// @address 0x269320 ?custom_team_score_stats@c_game_engine_ctf_variant@@UBAXJJJ@Z
// @address 0x269330 ?initialize@c_game_engine_infection_variant@@UAAXXZ
// @address 0x269660 ?validate@c_game_engine_infection_variant@@UAAXXZ
// @address 0x269ac8 ?encode@c_game_engine_infection_variant@@UBAXPAVc_bitstream@@@Z
// @address 0x269be0 ?decode@c_game_engine_infection_variant@@UAAXPAVc_bitstream@@@Z
// @address 0x269e90 ?initialize@c_game_engine_king_variant@@UAAXXZ
// @address 0x269fa8 ?validate@c_game_engine_king_variant@@UAAXXZ
// @address 0x26a2d8 ?encode@c_game_engine_king_variant@@UBAXPAVc_bitstream@@@Z
// @address 0x26a3a0 ?decode@c_game_engine_king_variant@@UAAXPAVc_bitstream@@@Z
// @address 0x26a658 ?initialize@c_game_engine_territories_variant@@UAAXXZ
// @address 0x26a7c8 ?validate@c_game_engine_territories_variant@@UAAXXZ
// @address 0x26aae8 ?encode@c_game_engine_territories_variant@@UBAXPAVc_bitstream@@@Z
// @address 0x26ab88 ?decode@c_game_engine_territories_variant@@UAAXPAVc_bitstream@@@Z
// @address 0x26acd8 ?initialize@c_game_engine_vip_variant@@UAAXXZ
// @address 0x26af40 ?validate@c_game_engine_vip_variant@@UAAXXZ
// @address 0x26b508 ?encode@c_game_engine_vip_variant@@UBAXPAVc_bitstream@@@Z
// @address 0x26b668 ?decode@c_game_engine_vip_variant@@UAAXPAVc_bitstream@@@Z
// @address 0x26ba10 ?initialize@c_game_engine_juggernaut_variant@@UAAXXZ
// @address 0x26bb58 ?validate@c_game_engine_juggernaut_variant@@UAAXXZ
// @address 0x26c060 ?encode@c_game_engine_juggernaut_variant@@UBAXPAVc_bitstream@@@Z
// @address 0x26c190 ?decode@c_game_engine_juggernaut_variant@@UAAXPAVc_bitstream@@@Z
// @address 0x26c4c8 ?initialize@c_game_engine_assault_variant@@UAAXXZ
// @address 0x26c678 ?validate@c_game_engine_assault_variant@@UAAXXZ
// @address 0x26ca60 ?encode@c_game_engine_assault_variant@@UBAXPAVc_bitstream@@@Z
// @address 0x26cb50 ?decode@c_game_engine_assault_variant@@UAAXPAVc_bitstream@@@Z
// @address 0x26cd68 ?initialize@c_game_engine_sandbox_variant@@UAAXXZ
// @address 0x26cef8 ?encode@c_game_engine_sandbox_variant@@UBAXPAVc_bitstream@@@Z
// @address 0x26cf70 ?decode@c_game_engine_sandbox_variant@@UAAXPAVc_bitstream@@@Z
// @address 0x42d298 ?can_add_to_recent_list@c_game_engine_sandbox_variant@@UBA_NXZ
// @address 0x42d298 ?get_score_to_win_round@c_game_engine_base_variant@@UBAJXZ
// @address 0x42d298 ?get_score_to_win_round@c_game_engine_sandbox_variant@@UBAJXZ
// @address 0x42d380 ?can_add_to_recent_list@c_game_engine_base_variant@@UBA_NXZ
// @address 0x5bec98 ?custom_team_score_stats@c_game_engine_base_variant@@UBAXJJJ@Z
// @address 0x86d28 ?validate@c_game_engine_sandbox_variant@@UAAXXZ
#include <string.h>

#include "memory/tls_globals.h"

#include "game/game_engine_base_variant.h"
#include "game/game_engine_ctf_variant.h"
#include "game/game_engine_slayer.h"
#include "game/game_engine_oddball_variant.h"
#include "game/game_engine_king_variant.h"
#include "game/game_engine_sandbox_variant.h"
#include "game/game_engine_vip_variant.h"
#include "game/game_engine_juggernaut_variant.h"
#include "game/game_engine_territories_variant.h"
#include "game/game_engine_assault_variant.h"
#include "game/game_engine_infection_variant.h"
struct s_game_engine_setting_variant;


#include "saved_games/saved_game_variant_file_types.h"
#ifdef HALO3_64BIT
#include "saved_games/saved_game_files_blf_variant.h"
#include "saved_games/saved_game_files.h"
#endif

#include "game/universal_types.h"

template <typename T> struct s_setting_flags {
  T bits;
  void set(int bit, bool enable) { if (enable) bits |= (1 << bit); else bits &= ~(1 << bit); }
  void clear(int bit) { bits &= ~(1 << bit); }
};

struct s_variant_fields {
  s_setting_flags<unsigned char> *byte_flags(int o) { return (s_setting_flags<unsigned char> *)((char *)this + o); }
  s_setting_flags<signed char> *char_flags(int o) { return (s_setting_flags<signed char> *)((char *)this + o); }
  s_setting_flags<short> *word_flags(int o) { return (s_setting_flags<short> *)((char *)this + o); }
  char *addr(int o) { return (char *)this + o; }
  void set_u8(int o, unsigned char x) { *((unsigned char *)this + o) = x; }
  void set_s8(int o, signed char x) { *((signed char *)this + o) = x; }
  void set_u16(int o, unsigned short x) { *(unsigned short *)((char *)this + o) = x; }
  void set_s16(int o, short x) { *(short *)((char *)this + o) = x; }
  void set_u32(int o, unsigned int x) { *(unsigned int *)((char *)this + o) = x; }
  void or_u8(int o, unsigned char m) { *((unsigned char *)this + o) |= m; }
  void and_u8(int o, unsigned char m) { *((unsigned char *)this + o) &= m; }
  void or_u16(int o, unsigned short m) { *(unsigned short *)((char *)this + o) |= m; }
  void and_u16(int o, unsigned short m) { *(unsigned short *)((char *)this + o) &= m; }
  void or_u32(int o, unsigned int m) { *(unsigned int *)((char *)this + o) |= m; }
  void and_u32(int o, unsigned int m) { *(unsigned int *)((char *)this + o) &= m; }
};

#pragma pack(push, 4)
struct s_game_engine_setting_variant {
protected:
  virtual long get_game_engine_name_string_id() const;
  virtual long get_game_engine_default_description_string_id() const;
public:
  virtual void initialize();
  virtual void validate();
  virtual void encode(c_bitstream *packet) const;
  virtual void decode(c_bitstream *packet);
  virtual bool can_add_to_recent_list() const;
  virtual long get_score_to_win_round() const;
  virtual bool can_be_cast_to(e_game_engine_type engine_type, const void **variant) const;
  virtual void custom_team_score_stats(long team_index, long old_score, long new_score) const;
  union {
    s_variant_fields fields;
    struct {
      long checksum;
      s_saved_game_item_metadata metadata;
      c_game_engine_miscellaneous_options miscellaneous_options;
      c_game_engine_respawn_options respawn_options;
      c_game_engine_social_options social_options;
      c_game_engine_map_override_options map_override_options;
    };
  };
  s_setting_flags<unsigned short> variant_flags; 
};
#pragma pack(pop)

typedef s_game_engine_setting_variant c_game_engine_variant;

inline c_game_engine_base_variant *c_game_variant::get_active_variant_writeable()
{
    c_game_engine_base_variant *variant = (c_game_engine_base_variant *)variant_storage;
    variant->set_built_in(false);
    return variant;
}

// @address 0x1c24a0 ?get_game_engine_name_string_id@c_game_engine_slayer_variant@@MBAJXZ
long c_game_engine_slayer_variant::get_game_engine_name_string_id() const
{
    return 0x31f;
}

// @address 0x1c24a8 ?get_game_engine_default_description_string_id@c_game_engine_slayer_variant@@MBAJXZ
long c_game_engine_slayer_variant::get_game_engine_default_description_string_id() const
{
    return 0x320;
}

long c_game_engine_slayer_variant::get_score_to_win_round() const
{
    return this->get_score_to_win();
}

// March game_options.obj: direct score-field load, same bytes as the slayer twin above.
long c_game_engine_juggernaut_variant::get_score_to_win_round() const
{
    return m_score_to_win;
}

// March game_options.obj: direct score-field load, same bytes as the slayer twin above.
long c_game_engine_vip_variant::get_score_to_win_round() const
{
    return m_score_to_win_round;
}

// @address 0x1c24c0 ?can_be_cast_to@c_game_engine_slayer_variant@@UBA_NW4e_game_engine_type@@PAPBX@Z
bool c_game_engine_slayer_variant::can_be_cast_to(e_game_engine_type engine_type, const void **variant) const
{
    const void *result = this;
    if (engine_type != _game_engine_slayer)
        result = 0;
    *variant = result;
    return result != 0;
}

// @address 0x1c24e8 ?get_game_engine_name_string_id@c_game_engine_oddball_variant@@MBAJXZ
long c_game_engine_oddball_variant::get_game_engine_name_string_id() const
{
    return 0x321;
}

// @address 0x1c24f0 ?get_game_engine_default_description_string_id@c_game_engine_oddball_variant@@MBAJXZ
long c_game_engine_oddball_variant::get_game_engine_default_description_string_id() const
{
    return 0x322;
}

// @address 0x1c24f8 ?can_be_cast_to@c_game_engine_oddball_variant@@UBA_NW4e_game_engine_type@@PAPBX@Z
bool c_game_engine_oddball_variant::can_be_cast_to(e_game_engine_type engine_type, const void **variant) const
{
    const void *result = this;
    if (engine_type != _game_engine_oddball)
        result = 0;
    *variant = result;
    return result != 0;
}

long c_game_engine_king_variant::get_score_to_win_round() const
{
    return this->get_score_to_win();
}

// March game_options.obj calls get_score_to_win(), same bytes as the king twin above.
long c_game_engine_oddball_variant::get_score_to_win_round() const
{
    return this->get_score_to_win();
}

// @address 0x1c2530 ?get_game_engine_name_string_id@c_game_engine_king_variant@@MBAJXZ
long c_game_engine_king_variant::get_game_engine_name_string_id() const
{
    return 0x323;
}

// @address 0x1c2538 ?get_game_engine_default_description_string_id@c_game_engine_king_variant@@MBAJXZ
long c_game_engine_king_variant::get_game_engine_default_description_string_id() const
{
    return 0x324;
}

// @address 0x1c2540 ?can_be_cast_to@c_game_engine_king_variant@@UBA_NW4e_game_engine_type@@PAPBX@Z
bool c_game_engine_king_variant::can_be_cast_to(e_game_engine_type engine_type, const void **variant) const
{
    const void *result = this;
    if (engine_type != _game_engine_king)
        result = 0;
    *variant = result;
    return result != 0;
}

// @address 0x1c2568 ?get_game_engine_name_string_id@c_game_engine_infection_variant@@MBAJXZ
long c_game_engine_infection_variant::get_game_engine_name_string_id() const
{
    return 0x32b;
}

// @address 0x1c2570 ?get_game_engine_default_description_string_id@c_game_engine_infection_variant@@MBAJXZ
long c_game_engine_infection_variant::get_game_engine_default_description_string_id() const
{
    return 0x32c;
}

// @address 0x1c2578 ?can_be_cast_to@c_game_engine_infection_variant@@UBA_NW4e_game_engine_type@@PAPBX@Z
bool c_game_engine_infection_variant::can_be_cast_to(e_game_engine_type engine_type, const void **variant) const
{
    const void *result = this;
    if (engine_type != _game_engine_infection)
        result = 0;
    *variant = result;
    return result != 0;
}

// @address 0x1c25a0 ?get_game_engine_name_string_id@c_game_engine_territories_variant@@MBAJXZ
long c_game_engine_territories_variant::get_game_engine_name_string_id() const
{
    return 0x327;
}

// @address 0x1c25a8 ?get_game_engine_default_description_string_id@c_game_engine_territories_variant@@MBAJXZ
long c_game_engine_territories_variant::get_game_engine_default_description_string_id() const
{
    return 0x328;
}

// @address 0x1c25b0 ?can_be_cast_to@c_game_engine_territories_variant@@UBA_NW4e_game_engine_type@@PAPBX@Z
bool c_game_engine_territories_variant::can_be_cast_to(e_game_engine_type engine_type, const void **variant) const
{
    const void *result = this;
    if (engine_type != _game_engine_territories)
        result = 0;
    *variant = result;
    return result != 0;
}

// @address 0x1c25d8 ?get_game_engine_name_string_id@c_game_engine_ctf_variant@@MBAJXZ
long c_game_engine_ctf_variant::get_game_engine_name_string_id() const
{
    return 0x31d;
}

// @address 0x1c25e0 ?get_game_engine_default_description_string_id@c_game_engine_ctf_variant@@MBAJXZ
long c_game_engine_ctf_variant::get_game_engine_default_description_string_id() const
{
    return 0x31e;
}

bool c_game_engine_ctf_variant::can_be_cast_to(e_game_engine_type engine_type, const void **variant) const
{
    if (engine_type == _game_engine_ctf) {
        *variant = this;
        return true;
    } else {
        *variant = 0;
        return false;
    }
}

long c_game_engine_ctf_variant::get_score_to_win_round() const
{
    return m_option_f;
}

// March game_options.obj calls get_score_to_win(); m_score_to_win sits at ctf's m_option_f
// offset, so this folds onto the ctf twin above.
long c_game_engine_assault_variant::get_score_to_win_round() const
{
    return this->get_score_to_win();
}

// @address 0x1c2620 ?get_game_engine_name_string_id@c_game_engine_assault_variant@@MBAJXZ
long c_game_engine_assault_variant::get_game_engine_name_string_id() const
{
    return 0x329;
}

// @address 0x1c2628 ?get_game_engine_default_description_string_id@c_game_engine_assault_variant@@MBAJXZ
long c_game_engine_assault_variant::get_game_engine_default_description_string_id() const
{
    return 0x32a;
}

bool c_game_engine_assault_variant::can_be_cast_to(e_game_engine_type engine_type, const void **variant) const
{
    if (engine_type == _game_engine_assault) {
        *variant = this;
        return true;
    } else {
        *variant = 0;
        return false;
    }
}

// @address 0x1c2658 ?get_game_engine_name_string_id@c_game_engine_vip_variant@@MBAJXZ
long c_game_engine_vip_variant::get_game_engine_name_string_id() const
{
    return 0x32d;
}

// @address 0x1c2660 ?get_game_engine_default_description_string_id@c_game_engine_vip_variant@@MBAJXZ
long c_game_engine_vip_variant::get_game_engine_default_description_string_id() const
{
    return 0x32e;
}

// @address 0x1c2668 ?can_be_cast_to@c_game_engine_vip_variant@@UBA_NW4e_game_engine_type@@PAPBX@Z
bool c_game_engine_vip_variant::can_be_cast_to(e_game_engine_type engine_type, const void **out) const
{
    const void *result = this;
    if (engine_type != _game_engine_vip) result = 0;
    *out = result;
    return result != 0;
}

// @address 0x1c2690 ?get_game_engine_name_string_id@c_game_engine_juggernaut_variant@@MBAJXZ
long c_game_engine_juggernaut_variant::get_game_engine_name_string_id() const
{
    return 0x325;
}

// @address 0x1c2698 ?get_game_engine_default_description_string_id@c_game_engine_juggernaut_variant@@MBAJXZ
long c_game_engine_juggernaut_variant::get_game_engine_default_description_string_id() const
{
    return 0x326;
}

// @address 0x1c26a0 ?can_be_cast_to@c_game_engine_juggernaut_variant@@UBA_NW4e_game_engine_type@@PAPBX@Z
bool c_game_engine_juggernaut_variant::can_be_cast_to(e_game_engine_type engine_type, const void **out) const
{
    const void *result = this;
    if (engine_type != _game_engine_juggernaut) result = 0;
    *out = result;
    return result != 0;
}

// @address 0x1c26c8 ?get_game_engine_name_string_id@c_game_engine_sandbox_variant@@MBAJXZ
long c_game_engine_sandbox_variant::get_game_engine_name_string_id() const
{
    return 0x32f;
}

// @address 0x1c26d0 ?get_game_engine_default_description_string_id@c_game_engine_sandbox_variant@@MBAJXZ
long c_game_engine_sandbox_variant::get_game_engine_default_description_string_id() const
{
    return 0x330;
}

static __forceinline game_options *game_options_engine_globals()
{
    return &tls_game_globals()->options;
}

// @address 0x1c2700 game_options_find_player
extern "C" int game_options_find_player(const void *identifier, const void *address)
{
    int result = -1;
    game_options *globals = game_options_engine_globals();
    for (unsigned int index = 0; index < 16; ++index) {
        game_player_options *entry = &globals->players[index];
        if (entry->valid &&
            memcmp(identifier, &entry->player_identifier, sizeof(entry->player_identifier)) == 0 &&
            memcmp(address, &entry->machine_identifier, sizeof(entry->machine_identifier)) == 0) {
            result = index;
            break;
        }
    }
    return result;
}

bool c_game_engine_sandbox_variant::can_be_cast_to(e_game_engine_type engine_type, const void **variant) const
{
    if (engine_type == _game_engine_sandbox) {
        *variant = this;
        return true;
    } else {
        *variant = 0;
        return false;
    }
}

// 0x1c27c0: the variant's identifier, the first 8 bytes of a hash of its storage; a built-in variant is hashed as a
// copy whose metadata date is zeroed and normalized (game_engine_variant_set_metadata)

// @address 0x1c2878 ?encode@c_game_variant@@QBAXPAVc_bitstream@@@Z
void c_game_variant::encode(c_bitstream *packet) const
{
    packet->write_integer("variant-game-engine-index", engine_type, 4);
    if (engine_type != 0) ((const c_game_engine_base_variant *)variant_storage)->encode(packet);
}

// 0x1c28d8: the variant read back from a packet (c_game_variant::encode writes it)
// @address 0x1c28d8 game_variant_decode
extern "C" bool game_variant_decode(c_game_variant *variant, c_bitstream *packet)
{
    bool result = false;
    variant->set_game_engine_index((e_game_engine_type)packet->read_integer("game-engine-index", 4));
    if (variant->get_game_engine_index() != _game_engine_base_variant)
    {
        variant->get_active_variant_writeable()->decode(packet);
        result = true;
    }
    return result;
}
// the size of the variant's engine structure, by engine type (callers pass the
// engine type in r4 too; the body switches on the variant's own)
// @address 0x1c2d28 game_variant_get_size
extern "C" long game_variant_get_size(const c_game_variant *variant, long engine_type)
{
#ifdef HALO3_64BIT
    const long vptr_extra = (long)(sizeof(void *) - 4);
#else
    const long vptr_extra = 0;
#endif
    switch (variant->engine_type) {
    case 0: return 0x1b0 + vptr_extra;
    case 1:
    case 4: return 0x1d8 + vptr_extra;
    case 2:
    case 3:
    case 7: return 0x1e0 + vptr_extra;
    case _game_engine_infection: return 0x230 + vptr_extra;
    case 8: return 0x1f0 + vptr_extra;
    case 9: return 0x200 + vptr_extra;
    case 6: return 0x218 + vptr_extra;
    case 5: return 0x1d0 + vptr_extra;
    // match: 0x1c2d28: unreachable hint omits dead branch check
    default: __assume(0);
    }
}

bool c_game_variant::get_rules_are_symmetric() const
{
  switch (engine_type) {
  case 9:
    return ((const c_game_engine_assault_variant *)variant_storage)->m_game_type != 1;
  case 1:
    return ((const c_game_engine_ctf_variant *)variant_storage)->m_option_b != 1;
  case 7:
  case _game_engine_infection:
    return false;
  case 8:
    return (static_cast<short>(((const c_game_engine_territories_variant *)variant_storage)->flags) & 1) == 0;
  case 6:
    return (((const c_game_engine_vip_variant *)variant_storage)->m_variant_flags & 1) == 0;
  default:
    return true;
  }
}

#ifdef HALO3_64BIT
template<typename t_field>
static void game_options_blf_swap(t_field &field)
{
    unsigned char *bytes = reinterpret_cast<unsigned char *>(&field);
    for (size_t index = 0; index < sizeof(field) / 2; ++index)
    {
        unsigned char first = bytes[index];
        bytes[index] = bytes[sizeof(field) - 1 - index];
        bytes[sizeof(field) - 1 - index] = first;
    }
}

static void game_options_blf_swap_traits(c_player_traits &traits)
{
    game_options_blf_swap(traits.m_weapon_traits.m_grenade_count);
    game_options_blf_swap(traits.m_sensor_traits.m_motion_tracker);
    game_options_blf_swap(traits.m_sensor_traits.m_motion_tracker_range);
}

static void game_options_blf_swap_variant(c_game_variant *variant)
{
    game_options_blf_swap(variant->m_checksum);
    game_options_blf_swap(variant->unique_id);
    for (size_t index = 0; index < NUMBEROF(variant->m_name); ++index)
        game_options_blf_swap(variant->m_name[index]);
    game_options_blf_swap(variant->file_type);
    game_options_blf_swap(variant->author_id);
    game_options_blf_swap(variant->size_in_bytes);
    game_options_blf_swap(variant->date);
    game_options_blf_swap(variant->length_seconds);
    game_options_blf_swap(variant->campaign_id);
    game_options_blf_swap(variant->map_id);
    game_options_blf_swap(variant->game_engine_type);
    game_options_blf_swap(variant->campaign_difficulty);
    game_options_blf_swap(variant->campaign_insertion_point);
    game_options_blf_swap(variant->campaign_survival_enabled);
    game_options_blf_swap(variant->game_id);
    game_options_blf_swap_traits(variant->respawn_player_traits);
    game_options_blf_swap(variant->social_options_flags);
    game_options_blf_swap(variant->team_changing);
    game_options_blf_swap(variant->variant_flags);
    game_options_blf_swap_traits(variant->base_player_traits);
    game_options_blf_swap(variant->weapon_set_absolute_index);
    game_options_blf_swap(variant->vehicle_set_absolute_index);
    game_options_blf_swap_traits(variant->red_powerup_traits);
    game_options_blf_swap_traits(variant->blue_powerup_traits);
    game_options_blf_swap_traits(variant->yellow_powerup_traits);
    game_options_blf_swap(variant->base_variant_flags);
    game_options_blf_swap(variant->team_scoring_method);

    switch (variant->get_game_engine_index())
    {
    case _game_engine_ctf:
    {
        c_game_engine_ctf_variant &ctf = *variant->get_ctf_variant_writeable();
        game_options_blf_swap(ctf.m_option_d);
        game_options_blf_swap(ctf.m_option_e);
        game_options_blf_swap(ctf.m_option_f);
        game_options_blf_swap(ctf.m_option_g);
        game_options_blf_swap_traits(ctf.m_carrier_traits);
        break;
    }
    case _game_engine_slayer:
    {
        c_game_engine_slayer_variant &slayer = *variant->get_slayer_variant_writeable();
        game_options_blf_swap(slayer.m_score_to_win);
        game_options_blf_swap(slayer.m_kill_points);
        game_options_blf_swap_traits(slayer.m_leader_traits);
        break;
    }
    case _game_engine_oddball:
    {
        c_game_engine_oddball_variant &oddball = *variant->get_oddball_variant_writeable();
        game_options_blf_swap(oddball.m_flags);
        game_options_blf_swap(oddball.m_score_to_win);
        game_options_blf_swap(oddball.m_carrying_points);
        game_options_blf_swap(oddball.m_ball_spawn_delay);
        game_options_blf_swap(oddball.m_ball_inactive_respawn_delay);
        game_options_blf_swap_traits(oddball.m_carrier_traits);
        break;
    }
    case _game_engine_king:
    {
        c_game_engine_king_variant &king = *variant->get_king_variant_writeable();
        game_options_blf_swap(king.m_flags);
        game_options_blf_swap(king.m_score_to_win);
        game_options_blf_swap_traits(king.m_inside_hill_traits);
        break;
    }
    case _game_engine_sandbox:
    {
        c_game_engine_sandbox_variant &sandbox = *variant->get_sandbox_variant_writeable();
        game_options_blf_swap(sandbox.m_respawn_time);
        game_options_blf_swap_traits(sandbox.m_player_traits);
        break;
    }
    case _game_engine_vip:
    {
        c_game_engine_vip_variant &vip = *variant->get_vip_variant_writeable();
        game_options_blf_swap(vip.m_score_to_win_round);
        game_options_blf_swap(vip.m_variant_flags);
        game_options_blf_swap(vip.m_influence_radius);
        game_options_blf_swap_traits(vip.m_vip_team_traits);
        game_options_blf_swap_traits(vip.m_vip_influence_traits);
        game_options_blf_swap_traits(vip.m_vip_traits);
        break;
    }
    case _game_engine_juggernaut:
    {
        c_game_engine_juggernaut_variant &juggernaut = *variant->get_juggernaut_variant_writeable();
        game_options_blf_swap(juggernaut.m_score_to_win);
        game_options_blf_swap_traits(juggernaut.m_juggernaut_traits);
        break;
    }
    case _game_engine_territories:
    {
        c_game_engine_territories_variant &territories = *variant->get_territories_variant_writeable();
        game_options_blf_swap(territories.m_flags);
        game_options_blf_swap(territories.m_respawn_on_capture);
        game_options_blf_swap(territories.m_capture_time);
        game_options_blf_swap_traits(territories.m_defender_traits);
        game_options_blf_swap_traits(territories.m_attacker_traits);
        break;
    }
    case _game_engine_assault:
    {
        c_game_engine_assault_variant &assault = *variant->get_assault_variant_writeable();
        game_options_blf_swap(assault.m_assault_flags);
        game_options_blf_swap(assault.m_respawn);
        game_options_blf_swap(assault.m_game_type);
        game_options_blf_swap(assault.m_enemy_bomb_waypoint);
        game_options_blf_swap(assault.m_score_to_win);
        game_options_blf_swap(assault.m_sudden_death_time);
        game_options_blf_swap(assault.m_bomb_reset_time);
        game_options_blf_swap(assault.m_bomb_arming_time);
        game_options_blf_swap(assault.m_bomb_disarming_time);
        game_options_blf_swap(assault.m_bomb_fuse_time);
        game_options_blf_swap_traits(assault.m_carrier_traits);
        game_options_blf_swap_traits(assault.m_arming_traits);
        break;
    }
    case _game_engine_infection:
    {
        c_game_engine_infection_variant &infection = *variant->get_infection_variant_writeable();
        game_options_blf_swap(infection.m_safe_haven_movement_time);
        game_options_blf_swap_traits(infection.m_traits);
        game_options_blf_swap_traits(infection.m_first_zombie_traits);
        game_options_blf_swap_traits(infection.m_safe_haven_defender_traits);
        game_options_blf_swap_traits(infection.m_last_human_traits);
        break;
    }
    }
}

void game_variant_encode_blf(void *destination, const c_game_variant *variant)
{
    s_blf_game_variant_payload payload = {};
    payload.engine_type = variant->get_game_engine_index();
    const long size = game_variant_get_size(variant, payload.engine_type) - sizeof(void *);
    memcpy(payload.engine_data, variant->variant_storage + sizeof(void *), size);
    memcpy(destination, &payload, sizeof(payload));
}

static bool game_options_decode_blf_variant(const char *chunk, long payload_size, bool swap, c_game_variant *variant)
{
    if (payload_size != sizeof(s_blf_game_variant_payload))
        return false;

    s_blf_game_variant_payload payload;
    memcpy(&payload, chunk, sizeof(payload));
    if (swap)
        game_options_blf_swap(payload.engine_type);
    if (payload.engine_type < _game_engine_ctf || payload.engine_type >= k_game_engine_type_count)
        return false;

    variant->recreate_variant_vtable_for_game_engine_index((e_game_engine_type)payload.engine_type);
    const long size = game_variant_get_size(variant, payload.engine_type) - sizeof(void *);
    const char *data = payload.engine_data;

    // Older PC builds wrote the native vptr. Identify the widened layout using
    // the inner metadata's engine and file type, which follow the same shift.
    if (!swap && payload.reserved_vtable_slot)
    {
        const size_t engine_offset = offsetof(c_game_variant, game_engine_type) - offsetof(c_game_variant, m_checksum);
        const size_t type_offset = offsetof(c_game_variant, file_type) - offsetof(c_game_variant, m_checksum);
        const long expected_type = game_engine_index_to_saved_game_file_type((e_game_engine_type)payload.engine_type);
        long native_engine, native_type, disk_engine, disk_type;
        memcpy(&native_engine, data + engine_offset + 4, sizeof(native_engine));
        memcpy(&native_type, data + type_offset + 4, sizeof(native_type));
        memcpy(&disk_engine, data + engine_offset, sizeof(disk_engine));
        memcpy(&disk_type, data + type_offset, sizeof(disk_type));
        if (native_engine == payload.engine_type && native_type == expected_type &&
            (disk_engine != payload.engine_type || disk_type != expected_type))
            data += 4;
    }
    memcpy(variant->variant_storage + sizeof(void *), data, size);
    if (swap)
        game_options_blf_swap_variant(variant);
    return true;
}
#endif

bool s_blffile_game_variant::copy_to_and_validate(c_game_variant *variant, bool *valid) const
{
    const char *file = (const char *)this;
    long file_size = sizeof(*this);
    bool result = false;
    const char *buffer = file;
    long chunk_size = 0;
    bool byte_swap;
    if (network_blf_verify_start_of_file(buffer, file_size, &byte_swap, &chunk_size))
    {
        const char *chunk = 0;
        bool eof = false;
        bool swap = byte_swap;
        buffer += chunk_size;
#ifdef HALO3_64BIT
        long payload_size = 0;
        while (network_blf_read_for_known_chunk(buffer, file_size - (buffer - file), swap, 'mpvr', 3,
                                                &chunk_size, &chunk, &payload_size, 0, &eof))
#else
        while (network_blf_read_for_known_chunk(buffer, file_size - (buffer - file), swap, 'mpvr', 3,
                                                &chunk_size, &chunk, 0, 0, &eof))
#endif
        {
            if (eof)
                break;
#ifdef HALO3_64BIT
            if (chunk)
#else
            if (!swap && chunk)
#endif
            {
                bool validated = false;
#ifdef HALO3_64BIT
                c_game_variant decoded;
                if (!game_options_decode_blf_variant(chunk, payload_size, swap, &decoded))
                    break;
                variant->copy_from_and_validate(&decoded);
#else
                variant->copy_from_and_validate((const c_game_variant *)chunk);
#endif
                validated = game_engine_variant_validate(variant);
                if (valid)
                    *valid = validated;
                result = true;
                break;
            }
            buffer += chunk_size;
        }
    }
    return result;
}

// 0x1c3000: the blf file's map variant chunk, copied into the map variant
bool s_blffile_map_variant::copy_to_and_validate(c_map_variant *map_variant, bool *valid) const
{
    const char *file = (const char *)this;
    long file_size = sizeof(*this);
    bool result = false;
    const char *buffer = file;
    long chunk_size = 0;
    bool byte_swap;
    if (network_blf_verify_start_of_file(buffer, file_size, &byte_swap, &chunk_size))
    {
        const char *chunk = 0;
        bool eof = false;
        buffer += chunk_size;
        while (network_blf_read_for_known_chunk(buffer, file_size - (buffer - file), byte_swap, 'mapv', 12,
                                                &chunk_size, &chunk, 0, 0, &eof))
        {
            if (eof)
                break;
            if (!byte_swap && chunk)
            {
                ((const s_blf_chunk_map_variant_payload *)chunk)->map_variant.save_to(map_variant);
                if (valid)
                    *valid = true;
                result = true;
                break;
            }
            buffer += chunk_size;
        }
    }
    return result;
}

e_text_value_pair_parameter_type c_game_variant::get_expected_game_engine_setting_parameter_type(e_game_variant_parameter parameter)
{
    switch (parameter) {
    case _game_variant_base_miscellaneous_teams:
        return _text_value_pair_parameter_integer;
    case _game_variant_base_miscellaneous_round_reset_map:
        return _text_value_pair_parameter_integer;
    case _game_variant_base_miscellaneous_round_time_limit:
        return _text_value_pair_parameter_integer;
    case _game_variant_base_miscellaneous_round_limit:
        return _text_value_pair_parameter_integer;
    case _game_variant_base_miscellaneous_early_victory_win_count:
        return _text_value_pair_parameter_integer;
    case _game_variant_respawn_inherit_respawn_timer:
        return _text_value_pair_parameter_integer;
    case _game_variant_respawn_with_teammate:
        return _text_value_pair_parameter_integer;
    case _game_variant_respawn_at_location:
        return _text_value_pair_parameter_integer;
    case _game_variant_respawn_upon_kill:
        return _text_value_pair_parameter_integer;
    case _game_variant_respawn_lives_per_round:
        return _text_value_pair_parameter_integer;
    case _game_variant_respawn_team_lives_per_round:
        return _text_value_pair_parameter_integer;
    case _game_variant_respawn_time:
        return _text_value_pair_parameter_integer;
    case _game_variant_respawn_suicide_penalty:
        return _text_value_pair_parameter_integer;
    case _game_variant_respawn_betrayal_penalty:
        return _text_value_pair_parameter_integer;
    case _game_variant_respawn_growth:
        return _text_value_pair_parameter_integer;
    case _game_variant_respawn_traits_duration:
        return _text_value_pair_parameter_integer;
    case _respawn_shld_dmg_resistance:
        return _text_value_pair_parameter_integer;
    case _respawn_shld_multiplier:
        return _text_value_pair_parameter_integer;
    case _respawn_shld_recharge_rate:
        return _text_value_pair_parameter_integer;
    case _respawn_headshot_immunity:
        return _text_value_pair_parameter_integer;
    case _respawn_vampirism:
        return _text_value_pair_parameter_integer;
    case _respawn_dmg_modifier_percentage:
        return _text_value_pair_parameter_integer;
    case _respawn_initial_primary_weapon:
        return _text_value_pair_parameter_string_id;
    case _respawn_initial_secondary_weapon:
        return _text_value_pair_parameter_string_id;
    case _respawn_initial_grenade_count:
        return _text_value_pair_parameter_integer;
    case _respawn_recharging_grenades:
        return _text_value_pair_parameter_integer;
    case _respawn_infinite_ammo:
        return _text_value_pair_parameter_integer;
    case _respawn_weapon_pickup:
        return _text_value_pair_parameter_integer;
    case _respawn_walking_speed:
        return _text_value_pair_parameter_integer;
    case _respawn_personal_gravity:
        return _text_value_pair_parameter_integer;
    case _respawn_vehicle_use:
        return _text_value_pair_parameter_integer;
    case _respawn_motion_tracker:
        return _text_value_pair_parameter_integer;
    case _respawn_motion_tracker_range:
        return _text_value_pair_parameter_integer;
    case _respawn_waypoint:
        return _text_value_pair_parameter_integer;
    case _respawn_active_camo:
        return _text_value_pair_parameter_integer;
    case _respawn_visual:
        return _text_value_pair_parameter_integer;
    case _respawn_forced_change_colors:
        return _text_value_pair_parameter_integer;
    case _game_variant_social_observers:
        return _text_value_pair_parameter_integer;
    case _game_variant_social_team_changing:
        return _text_value_pair_parameter_integer;
    case _game_variant_social_friendly_fire:
        return _text_value_pair_parameter_integer;
    case _game_variant_social_betrayal_booting:
        return _text_value_pair_parameter_integer;
    case _game_variant_social_enemy_voice:
        return _text_value_pair_parameter_integer;
    case _game_variant_social_open_channel_voice:
        return _text_value_pair_parameter_integer;
    case _game_variant_social_dead_player_voice:
        return _text_value_pair_parameter_integer;
    case _game_variant_map_override_grenades:
        return _text_value_pair_parameter_integer;
    case _game_variant_map_override_indestructible_vehicles:
        return _text_value_pair_parameter_integer;
    case _player_shld_dmg_resistance:
        return _text_value_pair_parameter_integer;
    case _player_shld_multiplier:
        return _text_value_pair_parameter_integer;
    case _player_shld_recharge_rate:
        return _text_value_pair_parameter_integer;
    case _player_headshot_immunity:
        return _text_value_pair_parameter_integer;
    case _player_vampirism:
        return _text_value_pair_parameter_integer;
    case _player_dmg_modifier_percentage:
        return _text_value_pair_parameter_integer;
    case _player_initial_primary_weapon:
        return _text_value_pair_parameter_string_id;
    case _player_initial_secondary_weapon:
        return _text_value_pair_parameter_string_id;
    case _player_initial_grenade_count:
        return _text_value_pair_parameter_integer;
    case _player_recharging_grenades:
        return _text_value_pair_parameter_integer;
    case _player_infinite_ammo:
        return _text_value_pair_parameter_integer;
    case _player_weapon_pickup:
        return _text_value_pair_parameter_integer;
    case _player_walking_speed:
        return _text_value_pair_parameter_integer;
    case _player_personal_gravity:
        return _text_value_pair_parameter_integer;
    case _player_vehicle_use:
        return _text_value_pair_parameter_integer;
    case _player_motion_tracker:
        return _text_value_pair_parameter_integer;
    case _player_motion_tracker_range:
        return _text_value_pair_parameter_integer;
    case _player_waypoint:
        return _text_value_pair_parameter_integer;
    case _player_active_camo:
        return _text_value_pair_parameter_integer;
    case _player_visual:
        return _text_value_pair_parameter_integer;
    case _player_forced_change_colors:
        return _text_value_pair_parameter_integer;
    case _game_variant_map_override_weapon_set:
        return _text_value_pair_parameter_string_id;
    case _game_variant_map_override_vehicle_set:
        return _text_value_pair_parameter_string_id;
    case _red_powerup_shld_dmg_resistance:
        return _text_value_pair_parameter_integer;
    case _red_powerup_shld_multiplier:
        return _text_value_pair_parameter_integer;
    case _red_powerup_shld_recharge_rate:
        return _text_value_pair_parameter_integer;
    case _red_powerup_headshot_immunity:
        return _text_value_pair_parameter_integer;
    case _red_powerup_vampirism:
        return _text_value_pair_parameter_integer;
    case _red_powerup_dmg_modifier_percentage:
        return _text_value_pair_parameter_integer;
    case _red_powerup_initial_primary_weapon:
        return _text_value_pair_parameter_string_id;
    case _red_powerup_initial_secondary_weapon:
        return _text_value_pair_parameter_string_id;
    case _red_powerup_initial_grenade_count:
        return _text_value_pair_parameter_integer;
    case _red_powerup_recharging_grenades:
        return _text_value_pair_parameter_integer;
    case _red_powerup_infinite_ammo:
        return _text_value_pair_parameter_integer;
    case _red_powerup_weapon_pickup:
        return _text_value_pair_parameter_integer;
    case _red_powerup_walking_speed:
        return _text_value_pair_parameter_integer;
    case _red_powerup_personal_gravity:
        return _text_value_pair_parameter_integer;
    case _red_powerup_vehicle_use:
        return _text_value_pair_parameter_integer;
    case _red_powerup_motion_tracker:
        return _text_value_pair_parameter_integer;
    case _red_powerup_motion_tracker_range:
        return _text_value_pair_parameter_integer;
    case _red_powerup_waypoint:
        return _text_value_pair_parameter_integer;
    case _red_powerup_active_camo:
        return _text_value_pair_parameter_integer;
    case _red_powerup_visual:
        return _text_value_pair_parameter_integer;
    case _red_powerup_forced_change_colors:
        return _text_value_pair_parameter_integer;
    case _blue_powerup_shld_dmg_resistance:
        return _text_value_pair_parameter_integer;
    case _blue_powerup_shld_multiplier:
        return _text_value_pair_parameter_integer;
    case _blue_powerup_shld_recharge_rate:
        return _text_value_pair_parameter_integer;
    case _blue_powerup_headshot_immunity:
        return _text_value_pair_parameter_integer;
    case _blue_powerup_vampirism:
        return _text_value_pair_parameter_integer;
    case _blue_powerup_dmg_modifier_percentage:
        return _text_value_pair_parameter_integer;
    case _blue_powerup_initial_primary_weapon:
        return _text_value_pair_parameter_string_id;
    case _blue_powerup_initial_secondary_weapon:
        return _text_value_pair_parameter_string_id;
    case _blue_powerup_initial_grenade_count:
        return _text_value_pair_parameter_integer;
    case _blue_powerup_recharging_grenades:
        return _text_value_pair_parameter_integer;
    case _blue_powerup_infinite_ammo:
        return _text_value_pair_parameter_integer;
    case _blue_powerup_weapon_pickup:
        return _text_value_pair_parameter_integer;
    case _blue_powerup_walking_speed:
        return _text_value_pair_parameter_integer;
    case _blue_powerup_personal_gravity:
        return _text_value_pair_parameter_integer;
    case _blue_powerup_vehicle_use:
        return _text_value_pair_parameter_integer;
    case _blue_powerup_motion_tracker:
        return _text_value_pair_parameter_integer;
    case _blue_powerup_motion_tracker_range:
        return _text_value_pair_parameter_integer;
    case _blue_powerup_waypoint:
        return _text_value_pair_parameter_integer;
    case _blue_powerup_active_camo:
        return _text_value_pair_parameter_integer;
    case _blue_powerup_visual:
        return _text_value_pair_parameter_integer;
    case _blue_powerup_forced_change_colors:
        return _text_value_pair_parameter_integer;
    case _yellow_powerup_shld_dmg_resistance:
        return _text_value_pair_parameter_integer;
    case _yellow_powerup_shld_multiplier:
        return _text_value_pair_parameter_integer;
    case _yellow_powerup_shld_recharge_rate:
        return _text_value_pair_parameter_integer;
    case _yellow_powerup_headshot_immunity:
        return _text_value_pair_parameter_integer;
    case _yellow_powerup_vampirism:
        return _text_value_pair_parameter_integer;
    case _yellow_powerup_dmg_modifier_percentage:
        return _text_value_pair_parameter_integer;
    case _yellow_powerup_initial_primary_weapon:
        return _text_value_pair_parameter_string_id;
    case _yellow_powerup_initial_secondary_weapon:
        return _text_value_pair_parameter_string_id;
    case _yellow_powerup_initial_grenade_count:
        return _text_value_pair_parameter_integer;
    case _yellow_powerup_recharging_grenades:
        return _text_value_pair_parameter_integer;
    case _yellow_powerup_infinite_ammo:
        return _text_value_pair_parameter_integer;
    case _yellow_powerup_weapon_pickup:
        return _text_value_pair_parameter_integer;
    case _yellow_powerup_walking_speed:
        return _text_value_pair_parameter_integer;
    case _yellow_powerup_personal_gravity:
        return _text_value_pair_parameter_integer;
    case _yellow_powerup_vehicle_use:
        return _text_value_pair_parameter_integer;
    case _yellow_powerup_motion_tracker:
        return _text_value_pair_parameter_integer;
    case _yellow_powerup_motion_tracker_range:
        return _text_value_pair_parameter_integer;
    case _yellow_powerup_waypoint:
        return _text_value_pair_parameter_integer;
    case _yellow_powerup_active_camo:
        return _text_value_pair_parameter_integer;
    case _yellow_powerup_visual:
        return _text_value_pair_parameter_integer;
    case _yellow_powerup_forced_change_colors:
        return _text_value_pair_parameter_integer;
    case _game_variant_map_override_red_powerup_traits_duration:
        return _text_value_pair_parameter_integer;
    case _game_variant_map_override_blue_powerup_traits_duration:
        return _text_value_pair_parameter_integer;
    case _game_variant_map_override_yellow_powerup_traits_duration:
        return _text_value_pair_parameter_integer;
    case _slayer_team_scoring:
        return _text_value_pair_parameter_integer;
    case _slayer_score_to_win:
        return _text_value_pair_parameter_integer;
    case _slayer_kill_points:
        return _text_value_pair_parameter_integer;
    case _slayer_assist_points:
        return _text_value_pair_parameter_integer;
    case _slayer_death_points:
        return _text_value_pair_parameter_integer;
    case _slayer_suicide_points:
        return _text_value_pair_parameter_integer;
    case _slayer_betrayal_points:
        return _text_value_pair_parameter_integer;
    case _slayer_leader_killed_points:
        return _text_value_pair_parameter_integer;
    case _slayer_elimination_points:
        return _text_value_pair_parameter_integer;
    case _slayer_assassination_points:
        return _text_value_pair_parameter_integer;
    case _slayer_headshot_points:
        return _text_value_pair_parameter_integer;
    case _slayer_melee_points:
        return _text_value_pair_parameter_integer;
    case _slayer_sticky_points:
        return _text_value_pair_parameter_integer;
    case _slayer_splatter_points:
        return _text_value_pair_parameter_integer;
    case _slayer_killing_spree_points:
        return _text_value_pair_parameter_integer;
    case _slayer_leader_shld_dmg_resistance:
        return _text_value_pair_parameter_integer;
    case _slayer_leader_shld_multiplier:
        return _text_value_pair_parameter_integer;
    case _slayer_leader_shld_recharge_rate:
        return _text_value_pair_parameter_integer;
    case _slayer_leader_headshot_immunity:
        return _text_value_pair_parameter_integer;
    case _slayer_leader_vampirism:
        return _text_value_pair_parameter_integer;
    case _slayer_leader_dmg_modifier_percentage:
        return _text_value_pair_parameter_integer;
    case _slayer_leader_initial_primary_weapon:
        return _text_value_pair_parameter_string_id;
    case _slayer_leader_initial_secondary_weapon:
        return _text_value_pair_parameter_string_id;
    case _slayer_leader_initial_grenade_count:
        return _text_value_pair_parameter_integer;
    case _slayer_leader_recharging_grenades:
        return _text_value_pair_parameter_integer;
    case _slayer_leader_infinite_ammo:
        return _text_value_pair_parameter_integer;
    case _slayer_leader_weapon_pickup:
        return _text_value_pair_parameter_integer;
    case _slayer_leader_walking_speed:
        return _text_value_pair_parameter_integer;
    case _slayer_leader_personal_gravity:
        return _text_value_pair_parameter_integer;
    case _slayer_leader_vehicle_use:
        return _text_value_pair_parameter_integer;
    case _slayer_leader_motion_tracker:
        return _text_value_pair_parameter_integer;
    case _slayer_leader_motion_tracker_range:
        return _text_value_pair_parameter_integer;
    case _slayer_leader_waypoint:
        return _text_value_pair_parameter_integer;
    case _slayer_leader_active_camo:
        return _text_value_pair_parameter_integer;
    case _slayer_leader_visual:
        return _text_value_pair_parameter_integer;
    case _slayer_leader_forced_change_colors:
        return _text_value_pair_parameter_integer;
    case _oddball_auto_ball_pickup:
        return _text_value_pair_parameter_integer;
    case _oddball_ball_effect:
        return _text_value_pair_parameter_integer;
    case _oddball_team_scoring:
        return _text_value_pair_parameter_integer;
    case _oddball_waypoints:
        return _text_value_pair_parameter_integer;
    case _oddball_score_to_win:
        return _text_value_pair_parameter_integer;
    case _oddball_carrying_points:
        return _text_value_pair_parameter_integer;
    case _oddball_kill_points:
        return _text_value_pair_parameter_integer;
    case _oddball_ball_kill_points:
        return _text_value_pair_parameter_integer;
    case _oddball_carrier_kill_points:
        return _text_value_pair_parameter_integer;
    case _oddball_ball_count:
        return _text_value_pair_parameter_integer;
    case _oddball_ball_spawn_delay:
        return _text_value_pair_parameter_integer;
    case _oddball_inactive_respawn_delay:
        return _text_value_pair_parameter_integer;
    case _ball_carrier_shld_dmg_resistance:
        return _text_value_pair_parameter_integer;
    case _ball_carrier_shld_multiplier:
        return _text_value_pair_parameter_integer;
    case _ball_carrier_shld_recharge_rate:
        return _text_value_pair_parameter_integer;
    case _ball_carrier_headshot_immunity:
        return _text_value_pair_parameter_integer;
    case _ball_carrier_vampirism:
        return _text_value_pair_parameter_integer;
    case _ball_carrier_dmg_modifier_percentage:
        return _text_value_pair_parameter_integer;
    case _ball_carrier_initial_primary_weapon:
        return _text_value_pair_parameter_string_id;
    case _ball_carrier_initial_secondary_weapon:
        return _text_value_pair_parameter_string_id;
    case _ball_carrier_initial_grenade_count:
        return _text_value_pair_parameter_integer;
    case _ball_carrier_recharging_grenades:
        return _text_value_pair_parameter_integer;
    case _ball_carrier_infinite_ammo:
        return _text_value_pair_parameter_integer;
    case _ball_carrier_weapon_pickup:
        return _text_value_pair_parameter_integer;
    case _ball_carrier_walking_speed:
        return _text_value_pair_parameter_integer;
    case _ball_carrier_personal_gravity:
        return _text_value_pair_parameter_integer;
    case _ball_carrier_vehicle_use:
        return _text_value_pair_parameter_integer;
    case _ball_carrier_motion_tracker:
        return _text_value_pair_parameter_integer;
    case _ball_carrier_motion_tracker_range:
        return _text_value_pair_parameter_integer;
    case _ball_carrier_waypoint:
        return _text_value_pair_parameter_integer;
    case _ball_carrier_active_camo:
        return _text_value_pair_parameter_integer;
    case _ball_carrier_visual:
        return _text_value_pair_parameter_integer;
    case _ball_carrier_forced_change_colors:
        return _text_value_pair_parameter_integer;
    case _ctf_touch_return_timeout:
        return _text_value_pair_parameter_integer;
    case _ctf_flag_at_home_to_score:
        return _text_value_pair_parameter_integer;
    case _ctf_bomb_sticky_arming:
        return _text_value_pair_parameter_integer;
    case _ctf_home_flag_waypoint:
        return _text_value_pair_parameter_integer;
    case _ctf_enemy_bomb_waypoint:
        return _text_value_pair_parameter_integer;
    case _ctf_game_type:
        return _text_value_pair_parameter_integer;
    case _ctf_respawn:
        return _text_value_pair_parameter_integer;
    case _ctf_score_to_win:
        return _text_value_pair_parameter_integer;
    case _ctf_sudden_death_time:
        return _text_value_pair_parameter_integer;
    case _ctf_flag_reset_time:
        return _text_value_pair_parameter_integer;
    case _ctf_bomb_arming_time:
        return _text_value_pair_parameter_integer;
    case _ctf_bomb_fuse_time:
        return _text_value_pair_parameter_integer;
    case _flag_carrier_shld_dmg_resistance:
        return _text_value_pair_parameter_integer;
    case _flag_carrier_shld_multiplier:
        return _text_value_pair_parameter_integer;
    case _flag_carrier_shld_recharge_rate:
        return _text_value_pair_parameter_integer;
    case _flag_carrier_headshot_immunity:
        return _text_value_pair_parameter_integer;
    case _flag_carrier_vampirism:
        return _text_value_pair_parameter_integer;
    case _flag_carrier_dmg_modifier_percentage:
        return _text_value_pair_parameter_integer;
    case _flag_carrier_initial_primary_weapon:
        return _text_value_pair_parameter_string_id;
    case _flag_carrier_initial_secondary_weapon:
        return _text_value_pair_parameter_string_id;
    case _flag_carrier_initial_grenade_count:
        return _text_value_pair_parameter_integer;
    case _flag_carrier_recharging_grenades:
        return _text_value_pair_parameter_integer;
    case _flag_carrier_infinite_ammo:
        return _text_value_pair_parameter_integer;
    case _flag_carrier_weapon_pickup:
        return _text_value_pair_parameter_integer;
    case _flag_carrier_walking_speed:
        return _text_value_pair_parameter_integer;
    case _flag_carrier_personal_gravity:
        return _text_value_pair_parameter_integer;
    case _flag_carrier_vehicle_use:
        return _text_value_pair_parameter_integer;
    case _flag_carrier_motion_tracker:
        return _text_value_pair_parameter_integer;
    case _flag_carrier_motion_tracker_range:
        return _text_value_pair_parameter_integer;
    case _flag_carrier_waypoint:
        return _text_value_pair_parameter_integer;
    case _flag_carrier_active_camo:
        return _text_value_pair_parameter_integer;
    case _flag_carrier_visual:
        return _text_value_pair_parameter_integer;
    case _flag_carrier_forced_change_colors:
        return _text_value_pair_parameter_integer;
    case _infection_respawn_on_haven_move:
        return _text_value_pair_parameter_integer;
    case _infection_safe_haven_setting:
        return _text_value_pair_parameter_integer;
    case _infection_next_zombie:
        return _text_value_pair_parameter_integer;
    case _infection_initial_zombie_count:
        return _text_value_pair_parameter_integer;
    case _infection_safe_haven_movement_time:
        return _text_value_pair_parameter_integer;
    case _infection_zombie_kill_points:
        return _text_value_pair_parameter_integer;
    case _infection_points:
        return _text_value_pair_parameter_integer;
    case _infection_safe_haven_arrival_points:
        return _text_value_pair_parameter_integer;
    case _infection_suicide_points:
        return _text_value_pair_parameter_integer;
    case _infection_betrayal_points:
        return _text_value_pair_parameter_integer;
    case _infection_last_man_standing_bonus_points:
        return _text_value_pair_parameter_integer;
    case _zombie_shld_dmg_resistance:
        return _text_value_pair_parameter_integer;
    case _zombie_shld_multiplier:
        return _text_value_pair_parameter_integer;
    case _zombie_shld_recharge_rate:
        return _text_value_pair_parameter_integer;
    case _zombie_headshot_immunity:
        return _text_value_pair_parameter_integer;
    case _zombie_vampirism:
        return _text_value_pair_parameter_integer;
    case _zombie_dmg_modifier_percentage:
        return _text_value_pair_parameter_integer;
    case _zombie_initial_primary_weapon:
        return _text_value_pair_parameter_string_id;
    case _zombie_initial_secondary_weapon:
        return _text_value_pair_parameter_string_id;
    case _zombie_initial_grenade_count:
        return _text_value_pair_parameter_integer;
    case _zombie_recharging_grenades:
        return _text_value_pair_parameter_integer;
    case _zombie_infinite_ammo:
        return _text_value_pair_parameter_integer;
    case _zombie_weapon_pickup:
        return _text_value_pair_parameter_integer;
    case _zombie_walking_speed:
        return _text_value_pair_parameter_integer;
    case _zombie_personal_gravity:
        return _text_value_pair_parameter_integer;
    case _zombie_vehicle_use:
        return _text_value_pair_parameter_integer;
    case _zombie_motion_tracker:
        return _text_value_pair_parameter_integer;
    case _zombie_motion_tracker_range:
        return _text_value_pair_parameter_integer;
    case _zombie_waypoint:
        return _text_value_pair_parameter_integer;
    case _zombie_active_camo:
        return _text_value_pair_parameter_integer;
    case _zombie_visual:
        return _text_value_pair_parameter_integer;
    case _zombie_forced_change_colors:
        return _text_value_pair_parameter_integer;
    case _first_zombie_shld_dmg_resistance:
        return _text_value_pair_parameter_integer;
    case _first_zombie_shld_multiplier:
        return _text_value_pair_parameter_integer;
    case _first_zombie_shld_recharge_rate:
        return _text_value_pair_parameter_integer;
    case _first_zombie_headshot_immunity:
        return _text_value_pair_parameter_integer;
    case _first_zombie_vampirism:
        return _text_value_pair_parameter_integer;
    case _first_zombie_dmg_modifier_percentage:
        return _text_value_pair_parameter_integer;
    case _first_zombie_initial_primary_weapon:
        return _text_value_pair_parameter_string_id;
    case _first_zombie_initial_secondary_weapon:
        return _text_value_pair_parameter_string_id;
    case _first_zombie_initial_grenade_count:
        return _text_value_pair_parameter_integer;
    case _first_zombie_recharging_grenades:
        return _text_value_pair_parameter_integer;
    case _first_zombie_infinite_ammo:
        return _text_value_pair_parameter_integer;
    case _first_zombie_weapon_pickup:
        return _text_value_pair_parameter_integer;
    case _first_zombie_walking_speed:
        return _text_value_pair_parameter_integer;
    case _first_zombie_personal_gravity:
        return _text_value_pair_parameter_integer;
    case _first_zombie_vehicle_use:
        return _text_value_pair_parameter_integer;
    case _first_zombie_motion_tracker:
        return _text_value_pair_parameter_integer;
    case _first_zombie_motion_tracker_range:
        return _text_value_pair_parameter_integer;
    case _first_zombie_waypoint:
        return _text_value_pair_parameter_integer;
    case _first_zombie_active_camo:
        return _text_value_pair_parameter_integer;
    case _first_zombie_visual:
        return _text_value_pair_parameter_integer;
    case _first_zombie_forced_change_colors:
        return _text_value_pair_parameter_integer;
    case _haven_defender_shld_dmg_resistance:
        return _text_value_pair_parameter_integer;
    case _haven_defender_shld_multiplier:
        return _text_value_pair_parameter_integer;
    case _haven_defender_shld_recharge_rate:
        return _text_value_pair_parameter_integer;
    case _haven_defender_headshot_immunity:
        return _text_value_pair_parameter_integer;
    case _haven_defender_vampirism:
        return _text_value_pair_parameter_integer;
    case _haven_defender_dmg_modifier_percentage:
        return _text_value_pair_parameter_integer;
    case _haven_defender_initial_primary_weapon:
        return _text_value_pair_parameter_string_id;
    case _haven_defender_initial_secondary_weapon:
        return _text_value_pair_parameter_string_id;
    case _haven_defender_initial_grenade_count:
        return _text_value_pair_parameter_integer;
    case _haven_defender_recharging_grenades:
        return _text_value_pair_parameter_integer;
    case _haven_defender_infinite_ammo:
        return _text_value_pair_parameter_integer;
    case _haven_defender_weapon_pickup:
        return _text_value_pair_parameter_integer;
    case _haven_defender_walking_speed:
        return _text_value_pair_parameter_integer;
    case _haven_defender_personal_gravity:
        return _text_value_pair_parameter_integer;
    case _haven_defender_vehicle_use:
        return _text_value_pair_parameter_integer;
    case _haven_defender_motion_tracker:
        return _text_value_pair_parameter_integer;
    case _haven_defender_motion_tracker_range:
        return _text_value_pair_parameter_integer;
    case _haven_defender_waypoint:
        return _text_value_pair_parameter_integer;
    case _haven_defender_active_camo:
        return _text_value_pair_parameter_integer;
    case _haven_defender_visual:
        return _text_value_pair_parameter_integer;
    case _haven_defender_forced_change_colors:
        return _text_value_pair_parameter_integer;
    case _last_human_shld_dmg_resistance:
        return _text_value_pair_parameter_integer;
    case _last_human_shld_multiplier:
        return _text_value_pair_parameter_integer;
    case _last_human_shld_recharge_rate:
        return _text_value_pair_parameter_integer;
    case _last_human_headshot_immunity:
        return _text_value_pair_parameter_integer;
    case _last_human_vampirism:
        return _text_value_pair_parameter_integer;
    case _last_human_dmg_modifier_percentage:
        return _text_value_pair_parameter_integer;
    case _last_human_initial_primary_weapon:
        return _text_value_pair_parameter_string_id;
    case _last_human_initial_secondary_weapon:
        return _text_value_pair_parameter_string_id;
    case _last_human_initial_grenade_count:
        return _text_value_pair_parameter_integer;
    case _last_human_recharging_grenades:
        return _text_value_pair_parameter_integer;
    case _last_human_infinite_ammo:
        return _text_value_pair_parameter_integer;
    case _last_human_weapon_pickup:
        return _text_value_pair_parameter_integer;
    case _last_human_walking_speed:
        return _text_value_pair_parameter_integer;
    case _last_human_personal_gravity:
        return _text_value_pair_parameter_integer;
    case _last_human_vehicle_use:
        return _text_value_pair_parameter_integer;
    case _last_human_motion_tracker:
        return _text_value_pair_parameter_integer;
    case _last_human_motion_tracker_range:
        return _text_value_pair_parameter_integer;
    case _last_human_waypoint:
        return _text_value_pair_parameter_integer;
    case _last_human_active_camo:
        return _text_value_pair_parameter_integer;
    case _last_human_visual:
        return _text_value_pair_parameter_integer;
    case _last_human_forced_change_colors:
        return _text_value_pair_parameter_integer;
    case _game_variant_king_opaque_hill:
        return _text_value_pair_parameter_integer;
    case _king_score_to_win:
        return _text_value_pair_parameter_integer;
    case _king_team_scoring:
        return _text_value_pair_parameter_integer;
    case _king_moving_hill:
        return _text_value_pair_parameter_integer;
    case _king_moving_hill_order:
        return _text_value_pair_parameter_integer;
    case _king_inside_hill_points:
        return _text_value_pair_parameter_integer;
    case _king_outside_hill_points:
        return _text_value_pair_parameter_integer;
    case _king_uncontested_hill_points:
        return _text_value_pair_parameter_integer;
    case _king_kill_points:
        return _text_value_pair_parameter_integer;
    case _on_hill_shld_dmg_resistance:
        return _text_value_pair_parameter_integer;
    case _on_hill_shld_multiplier:
        return _text_value_pair_parameter_integer;
    case _on_hill_shld_recharge_rate:
        return _text_value_pair_parameter_integer;
    case _on_hill_headshot_immunity:
        return _text_value_pair_parameter_integer;
    case _on_hill_vampirism:
        return _text_value_pair_parameter_integer;
    case _on_hill_dmg_modifier_percentage:
        return _text_value_pair_parameter_integer;
    case _on_hill_initial_primary_weapon:
        return _text_value_pair_parameter_string_id;
    case _on_hill_initial_secondary_weapon:
        return _text_value_pair_parameter_string_id;
    case _on_hill_initial_grenade_count:
        return _text_value_pair_parameter_integer;
    case _on_hill_recharging_grenades:
        return _text_value_pair_parameter_integer;
    case _on_hill_infinite_ammo:
        return _text_value_pair_parameter_integer;
    case _on_hill_weapon_pickup:
        return _text_value_pair_parameter_integer;
    case _on_hill_walking_speed:
        return _text_value_pair_parameter_integer;
    case _on_hill_personal_gravity:
        return _text_value_pair_parameter_integer;
    case _on_hill_vehicle_use:
        return _text_value_pair_parameter_integer;
    case _on_hill_motion_tracker:
        return _text_value_pair_parameter_integer;
    case _on_hill_motion_tracker_range:
        return _text_value_pair_parameter_integer;
    case _on_hill_waypoint:
        return _text_value_pair_parameter_integer;
    case _on_hill_active_camo:
        return _text_value_pair_parameter_integer;
    case _on_hill_visual:
        return _text_value_pair_parameter_integer;
    case _on_hill_forced_change_colors:
        return _text_value_pair_parameter_integer;
    case _territories_capture_time:
        return _text_value_pair_parameter_integer;
    case _territories_one_sided:
        return _text_value_pair_parameter_integer;
    case _territories_lock_after_capture:
        return _text_value_pair_parameter_integer;
    case _territories_sudden_death:
        return _text_value_pair_parameter_integer;
    case _territories_respawn_on_capture:
        return _text_value_pair_parameter_integer;
    case _territory_defender_shld_dmg_resistance:
        return _text_value_pair_parameter_integer;
    case _territory_defender_shld_multiplier:
        return _text_value_pair_parameter_integer;
    case _territory_defender_shld_recharge_rate:
        return _text_value_pair_parameter_integer;
    case _territory_defender_headshot_immunity:
        return _text_value_pair_parameter_integer;
    case _territory_defender_vampirism:
        return _text_value_pair_parameter_integer;
    case _territory_defender_dmg_modifier_percentage:
        return _text_value_pair_parameter_integer;
    case _territory_defender_initial_primary_weapon:
        return _text_value_pair_parameter_string_id;
    case _territory_defender_initial_secondary_weapon:
        return _text_value_pair_parameter_string_id;
    case _territory_defender_initial_grenade_count:
        return _text_value_pair_parameter_integer;
    case _territory_defender_recharging_grenades:
        return _text_value_pair_parameter_integer;
    case _territory_defender_infinite_ammo:
        return _text_value_pair_parameter_integer;
    case _territory_defender_weapon_pickup:
        return _text_value_pair_parameter_integer;
    case _territory_defender_walking_speed:
        return _text_value_pair_parameter_integer;
    case _territory_defender_personal_gravity:
        return _text_value_pair_parameter_integer;
    case _territory_defender_vehicle_use:
        return _text_value_pair_parameter_integer;
    case _territory_defender_motion_tracker:
        return _text_value_pair_parameter_integer;
    case _territory_defender_motion_tracker_range:
        return _text_value_pair_parameter_integer;
    case _territory_defender_waypoint:
        return _text_value_pair_parameter_integer;
    case _territory_defender_active_camo:
        return _text_value_pair_parameter_integer;
    case _territory_defender_visual:
        return _text_value_pair_parameter_integer;
    case _territory_defender_forced_change_colors:
        return _text_value_pair_parameter_integer;
    case _territory_attacker_shld_dmg_resistance:
        return _text_value_pair_parameter_integer;
    case _territory_attacker_shld_multiplier:
        return _text_value_pair_parameter_integer;
    case _territory_attacker_shld_recharge_rate:
        return _text_value_pair_parameter_integer;
    case _territory_attacker_headshot_immunity:
        return _text_value_pair_parameter_integer;
    case _territory_attacker_vampirism:
        return _text_value_pair_parameter_integer;
    case _territory_attacker_dmg_modifier_percentage:
        return _text_value_pair_parameter_integer;
    case _territory_attacker_initial_primary_weapon:
        return _text_value_pair_parameter_string_id;
    case _territory_attacker_initial_secondary_weapon:
        return _text_value_pair_parameter_string_id;
    case _territory_attacker_initial_grenade_count:
        return _text_value_pair_parameter_integer;
    case _territory_attacker_recharging_grenades:
        return _text_value_pair_parameter_integer;
    case _territory_attacker_infinite_ammo:
        return _text_value_pair_parameter_integer;
    case _territory_attacker_weapon_pickup:
        return _text_value_pair_parameter_integer;
    case _territory_attacker_walking_speed:
        return _text_value_pair_parameter_integer;
    case _territory_attacker_personal_gravity:
        return _text_value_pair_parameter_integer;
    case _territory_attacker_vehicle_use:
        return _text_value_pair_parameter_integer;
    case _territory_attacker_motion_tracker:
        return _text_value_pair_parameter_integer;
    case _territory_attacker_motion_tracker_range:
        return _text_value_pair_parameter_integer;
    case _territory_attacker_waypoint:
        return _text_value_pair_parameter_integer;
    case _territory_attacker_active_camo:
        return _text_value_pair_parameter_integer;
    case _territory_attacker_visual:
        return _text_value_pair_parameter_integer;
    case _territory_attacker_forced_change_colors:
        return _text_value_pair_parameter_integer;
    case _vip_score_to_win:
        return _text_value_pair_parameter_integer;
    case _vip_single_vip:
        return _text_value_pair_parameter_integer;
    case _vip_destination_zones_enabled:
        return _text_value_pair_parameter_integer;
    case _vip_end_round_on_vip_death:
        return _text_value_pair_parameter_integer;
    case _vip_kill_points:
        return _text_value_pair_parameter_integer;
    case _vip_takedown_points:
        return _text_value_pair_parameter_integer;
    case _vip_kill_as_vip_points:
        return _text_value_pair_parameter_integer;
    case _vip_death_points:
        return _text_value_pair_parameter_integer;
    case _vip_zone_arrival_points:
        return _text_value_pair_parameter_integer;
    case _vip_suicide_points:
        return _text_value_pair_parameter_integer;
    case _vip_vip_friendly_death_points:
        return _text_value_pair_parameter_integer;
    case _vip_betrayal_points:
        return _text_value_pair_parameter_integer;
    case _vip_selection:
        return _text_value_pair_parameter_integer;
    case _vip_zone_movement:
        return _text_value_pair_parameter_integer;
    case _vip_zone_order:
        return _text_value_pair_parameter_integer;
    case _vip_influence_radius:
        return _text_value_pair_parameter_integer;
    case _vip_team_shld_dmg_resistance:
        return _text_value_pair_parameter_integer;
    case _vip_team_shld_multiplier:
        return _text_value_pair_parameter_integer;
    case _vip_team_shld_recharge_rate:
        return _text_value_pair_parameter_integer;
    case _vip_team_headshot_immunity:
        return _text_value_pair_parameter_integer;
    case _vip_team_vampirism:
        return _text_value_pair_parameter_integer;
    case _vip_team_dmg_modifier_percentage:
        return _text_value_pair_parameter_integer;
    case _vip_team_initial_primary_weapon:
        return _text_value_pair_parameter_string_id;
    case _vip_team_initial_secondary_weapon:
        return _text_value_pair_parameter_string_id;
    case _vip_team_initial_grenade_count:
        return _text_value_pair_parameter_integer;
    case _vip_team_recharging_grenades:
        return _text_value_pair_parameter_integer;
    case _vip_team_infinite_ammo:
        return _text_value_pair_parameter_integer;
    case _vip_team_weapon_pickup:
        return _text_value_pair_parameter_integer;
    case _vip_team_walking_speed:
        return _text_value_pair_parameter_integer;
    case _vip_team_personal_gravity:
        return _text_value_pair_parameter_integer;
    case _vip_team_vehicle_use:
        return _text_value_pair_parameter_integer;
    case _vip_team_motion_tracker:
        return _text_value_pair_parameter_integer;
    case _vip_team_motion_tracker_range:
        return _text_value_pair_parameter_integer;
    case _vip_team_waypoint:
        return _text_value_pair_parameter_integer;
    case _vip_team_active_camo:
        return _text_value_pair_parameter_integer;
    case _vip_team_visual:
        return _text_value_pair_parameter_integer;
    case _vip_team_forced_change_colors:
        return _text_value_pair_parameter_integer;
    case _vip_influence_shld_dmg_resistance:
        return _text_value_pair_parameter_integer;
    case _vip_influence_shld_multiplier:
        return _text_value_pair_parameter_integer;
    case _vip_influence_shld_recharge_rate:
        return _text_value_pair_parameter_integer;
    case _vip_influence_headshot_immunity:
        return _text_value_pair_parameter_integer;
    case _vip_influence_vampirism:
        return _text_value_pair_parameter_integer;
    case _vip_influence_dmg_modifier_percentage:
        return _text_value_pair_parameter_integer;
    case _vip_influence_initial_primary_weapon:
        return _text_value_pair_parameter_string_id;
    case _vip_influence_initial_secondary_weapon:
        return _text_value_pair_parameter_string_id;
    case _vip_influence_initial_grenade_count:
        return _text_value_pair_parameter_integer;
    case _vip_influence_recharging_grenades:
        return _text_value_pair_parameter_integer;
    case _vip_influence_infinite_ammo:
        return _text_value_pair_parameter_integer;
    case _vip_influence_weapon_pickup:
        return _text_value_pair_parameter_integer;
    case _vip_influence_walking_speed:
        return _text_value_pair_parameter_integer;
    case _vip_influence_personal_gravity:
        return _text_value_pair_parameter_integer;
    case _vip_influence_vehicle_use:
        return _text_value_pair_parameter_integer;
    case _vip_influence_motion_tracker:
        return _text_value_pair_parameter_integer;
    case _vip_influence_motion_tracker_range:
        return _text_value_pair_parameter_integer;
    case _vip_influence_waypoint:
        return _text_value_pair_parameter_integer;
    case _vip_influence_active_camo:
        return _text_value_pair_parameter_integer;
    case _vip_influence_visual:
        return _text_value_pair_parameter_integer;
    case _vip_influence_forced_change_colors:
        return _text_value_pair_parameter_integer;
    case _vip_shld_dmg_resistance:
        return _text_value_pair_parameter_integer;
    case _vip_shld_multiplier:
        return _text_value_pair_parameter_integer;
    case _vip_shld_recharge_rate:
        return _text_value_pair_parameter_integer;
    case _vip_headshot_immunity:
        return _text_value_pair_parameter_integer;
    case _vip_vampirism:
        return _text_value_pair_parameter_integer;
    case _vip_dmg_modifier_percentage:
        return _text_value_pair_parameter_integer;
    case _vip_initial_primary_weapon:
        return _text_value_pair_parameter_string_id;
    case _vip_initial_secondary_weapon:
        return _text_value_pair_parameter_string_id;
    case _vip_initial_grenade_count:
        return _text_value_pair_parameter_integer;
    case _vip_recharging_grenades:
        return _text_value_pair_parameter_integer;
    case _vip_infinite_ammo:
        return _text_value_pair_parameter_integer;
    case _vip_weapon_pickup:
        return _text_value_pair_parameter_integer;
    case _vip_walking_speed:
        return _text_value_pair_parameter_integer;
    case _vip_personal_gravity:
        return _text_value_pair_parameter_integer;
    case _vip_vehicle_use:
        return _text_value_pair_parameter_integer;
    case _vip_motion_tracker:
        return _text_value_pair_parameter_integer;
    case _vip_motion_tracker_range:
        return _text_value_pair_parameter_integer;
    case _vip_waypoint:
        return _text_value_pair_parameter_integer;
    case _vip_active_camo:
        return _text_value_pair_parameter_integer;
    case _vip_visual:
        return _text_value_pair_parameter_integer;
    case _vip_forced_change_colors:
        return _text_value_pair_parameter_integer;
    case _juggernaut_score_to_win:
        return _text_value_pair_parameter_integer;
    case _juggernaut_kill_points:
        return _text_value_pair_parameter_integer;
    case _juggernaut_juggernaut_kill_points:
        return _text_value_pair_parameter_integer;
    case _juggernaut_kill_as_juggernaut_points:
        return _text_value_pair_parameter_integer;
    case _juggernaut_zone_arrival_points:
        return _text_value_pair_parameter_integer;
    case _juggernaut_suicide_points:
        return _text_value_pair_parameter_integer;
    case _juggernaut_betrayal_points:
        return _text_value_pair_parameter_integer;
    case _juggernaut_initial_juggernaut:
        return _text_value_pair_parameter_integer;
    case _juggernaut_next_juggernaut:
        return _text_value_pair_parameter_integer;
    case _juggernaut_allied_against_juggernaut:
        return _text_value_pair_parameter_integer;
    case _juggernaut_delay:
        return _text_value_pair_parameter_integer;
    case _juggernaut_zones_enabled:
        return _text_value_pair_parameter_integer;
    case _juggernaut_zone_movement:
        return _text_value_pair_parameter_integer;
    case _juggernaut_zone_order:
        return _text_value_pair_parameter_integer;
    case _juggernaut_respawn_on_lone_juggernaut:
        return _text_value_pair_parameter_integer;
    case _juggernaut_shld_dmg_resistance:
        return _text_value_pair_parameter_integer;
    case _juggernaut_shld_multiplier:
        return _text_value_pair_parameter_integer;
    case _juggernaut_shld_recharge_rate:
        return _text_value_pair_parameter_integer;
    case _juggernaut_headshot_immunity:
        return _text_value_pair_parameter_integer;
    case _juggernaut_vampirism:
        return _text_value_pair_parameter_integer;
    case _juggernaut_dmg_modifier_percentage:
        return _text_value_pair_parameter_integer;
    case _juggernaut_initial_primary_weapon:
        return _text_value_pair_parameter_string_id;
    case _juggernaut_initial_secondary_weapon:
        return _text_value_pair_parameter_string_id;
    case _juggernaut_initial_grenade_count:
        return _text_value_pair_parameter_integer;
    case _juggernaut_recharging_grenades:
        return _text_value_pair_parameter_integer;
    case _juggernaut_infinite_ammo:
        return _text_value_pair_parameter_integer;
    case _juggernaut_weapon_pickup:
        return _text_value_pair_parameter_integer;
    case _juggernaut_walking_speed:
        return _text_value_pair_parameter_integer;
    case _juggernaut_personal_gravity:
        return _text_value_pair_parameter_integer;
    case _juggernaut_vehicle_use:
        return _text_value_pair_parameter_integer;
    case _juggernaut_motion_tracker:
        return _text_value_pair_parameter_integer;
    case _juggernaut_motion_tracker_range:
        return _text_value_pair_parameter_integer;
    case _juggernaut_waypoint:
        return _text_value_pair_parameter_integer;
    case _juggernaut_active_camo:
        return _text_value_pair_parameter_integer;
    case _juggernaut_visual:
        return _text_value_pair_parameter_integer;
    case _juggernaut_forced_change_colors:
        return _text_value_pair_parameter_integer;
    case _assault_arming_shld_dmg_resistance:
        return _text_value_pair_parameter_integer;
    case _assault_arming_shld_multiplier:
        return _text_value_pair_parameter_integer;
    case _assault_arming_shld_recharge_rate:
        return _text_value_pair_parameter_integer;
    case _assault_arming_headshot_immunity:
        return _text_value_pair_parameter_integer;
    case _assault_arming_vampirism:
        return _text_value_pair_parameter_integer;
    case _assault_arming_dmg_modifier_percentage:
        return _text_value_pair_parameter_integer;
    case _assault_arming_initial_primary_weapon:
        return _text_value_pair_parameter_string_id;
    case _assault_arming_initial_secondary_weapon:
        return _text_value_pair_parameter_string_id;
    case _assault_arming_initial_grenade_count:
        return _text_value_pair_parameter_integer;
    case _assault_arming_recharging_grenades:
        return _text_value_pair_parameter_integer;
    case _assault_arming_infinite_ammo:
        return _text_value_pair_parameter_integer;
    case _assault_arming_weapon_pickup:
        return _text_value_pair_parameter_integer;
    case _assault_arming_walking_speed:
        return _text_value_pair_parameter_integer;
    case _assault_arming_personal_gravity:
        return _text_value_pair_parameter_integer;
    case _assault_arming_vehicle_use:
        return _text_value_pair_parameter_integer;
    case _assault_arming_motion_tracker:
        return _text_value_pair_parameter_integer;
    case _assault_arming_motion_tracker_range:
        return _text_value_pair_parameter_integer;
    case _assault_arming_waypoint:
        return _text_value_pair_parameter_integer;
    case _assault_arming_active_camo:
        return _text_value_pair_parameter_integer;
    case _assault_arming_visual:
        return _text_value_pair_parameter_integer;
    case _assault_arming_forced_change_colors:
        return _text_value_pair_parameter_integer;
    case _assault_bomb_arming_time:
        return _text_value_pair_parameter_integer;
    case _assault_bomb_disarming_time:
        return _text_value_pair_parameter_integer;
    case _assault_bomb_fuse_time:
        return _text_value_pair_parameter_integer;
    case _assault_bomb_reset_time:
        return _text_value_pair_parameter_integer;
    case _assault_carrier_shld_dmg_resistance:
        return _text_value_pair_parameter_integer;
    case _assault_carrier_shld_multiplier:
        return _text_value_pair_parameter_integer;
    case _assault_carrier_shld_recharge_rate:
        return _text_value_pair_parameter_integer;
    case _assault_carrier_headshot_immunity:
        return _text_value_pair_parameter_integer;
    case _assault_carrier_vampirism:
        return _text_value_pair_parameter_integer;
    case _assault_carrier_dmg_modifier_percentage:
        return _text_value_pair_parameter_integer;
    case _assault_carrier_initial_primary_weapon:
        return _text_value_pair_parameter_string_id;
    case _assault_carrier_initial_secondary_weapon:
        return _text_value_pair_parameter_string_id;
    case _assault_carrier_initial_grenade_count:
        return _text_value_pair_parameter_integer;
    case _assault_carrier_recharging_grenades:
        return _text_value_pair_parameter_integer;
    case _assault_carrier_infinite_ammo:
        return _text_value_pair_parameter_integer;
    case _assault_carrier_weapon_pickup:
        return _text_value_pair_parameter_integer;
    case _assault_carrier_walking_speed:
        return _text_value_pair_parameter_integer;
    case _assault_carrier_personal_gravity:
        return _text_value_pair_parameter_integer;
    case _assault_carrier_vehicle_use:
        return _text_value_pair_parameter_integer;
    case _assault_carrier_motion_tracker:
        return _text_value_pair_parameter_integer;
    case _assault_carrier_motion_tracker_range:
        return _text_value_pair_parameter_integer;
    case _assault_carrier_waypoint:
        return _text_value_pair_parameter_integer;
    case _assault_carrier_active_camo:
        return _text_value_pair_parameter_integer;
    case _assault_carrier_visual:
        return _text_value_pair_parameter_integer;
    case _assault_carrier_forced_change_colors:
        return _text_value_pair_parameter_integer;
    case _assault_enemy_bomb_waypoint:
        return _text_value_pair_parameter_integer;
    case _assault_game_type:
        return _text_value_pair_parameter_integer;
    case _assault_reset_bomb_on_disarm:
        return _text_value_pair_parameter_integer;
    case _assault_respawn:
        return _text_value_pair_parameter_integer;
    case _assault_score_to_win:
        return _text_value_pair_parameter_integer;
    case _assault_sudden_death_time:
        return _text_value_pair_parameter_integer;
    case _sandbox_open_channel_voice:
        return _text_value_pair_parameter_integer;
    case _sandbox_enter_editor_mode:
        return _text_value_pair_parameter_integer;
    case _sandbox_respawn_time:
        return _text_value_pair_parameter_integer;
    case _sandbox_editor_shld_dmg_resistance:
        return _text_value_pair_parameter_integer;
    case _sandbox_editor_shld_multiplier:
        return _text_value_pair_parameter_integer;
    case _sandbox_editor_shld_recharge_rate:
        return _text_value_pair_parameter_integer;
    case _sandbox_editor_headshot_immunity:
        return _text_value_pair_parameter_integer;
    case _sandbox_editor_vampirism:
        return _text_value_pair_parameter_integer;
    case _sandbox_editor_dmg_modifier_percentage:
        return _text_value_pair_parameter_integer;
    case _sandbox_editor_initial_primary_weapon:
        return _text_value_pair_parameter_string_id;
    case _sandbox_editor_initial_secondary_weapon:
        return _text_value_pair_parameter_string_id;
    case _sandbox_editor_initial_grenade_count:
        return _text_value_pair_parameter_integer;
    case _sandbox_editor_recharging_grenades:
        return _text_value_pair_parameter_integer;
    case _sandbox_editor_infinite_ammo:
        return _text_value_pair_parameter_integer;
    case _sandbox_editor_weapon_pickup:
        return _text_value_pair_parameter_integer;
    case _sandbox_editor_walking_speed:
        return _text_value_pair_parameter_integer;
    case _sandbox_editor_personal_gravity:
        return _text_value_pair_parameter_integer;
    case _sandbox_editor_vehicle_use:
        return _text_value_pair_parameter_integer;
    case _sandbox_editor_motion_tracker:
        return _text_value_pair_parameter_integer;
    case _sandbox_editor_motion_tracker_range:
        return _text_value_pair_parameter_integer;
    case _sandbox_editor_waypoint:
        return _text_value_pair_parameter_integer;
    case _sandbox_editor_active_camo:
        return _text_value_pair_parameter_integer;
    case _sandbox_editor_visual:
        return _text_value_pair_parameter_integer;
    case _sandbox_editor_forced_change_colors:
        return _text_value_pair_parameter_integer;
    case _template_shld_dmg_resistance:
        return _text_value_pair_parameter_integer;
    case _template_shld_multiplier:
        return _text_value_pair_parameter_integer;
    case _template_shld_recharge_rate:
        return _text_value_pair_parameter_integer;
    case _template_headshot_immunity:
        return _text_value_pair_parameter_integer;
    case _template_vampirism:
        return _text_value_pair_parameter_integer;
    case _template_dmg_modifier_percentage:
        return _text_value_pair_parameter_integer;
    case _template_initial_primary_weapon:
        return _text_value_pair_parameter_string_id;
    case _template_initial_secondary_weapon:
        return _text_value_pair_parameter_string_id;
    case _template_initial_grenade_count:
        return _text_value_pair_parameter_integer;
    case _template_recharging_grenades:
        return _text_value_pair_parameter_integer;
    case _template_infinite_ammo:
        return _text_value_pair_parameter_integer;
    case _template_weapon_pickup:
        return _text_value_pair_parameter_integer;
    case _template_walking_speed:
        return _text_value_pair_parameter_integer;
    case _template_personal_gravity:
        return _text_value_pair_parameter_integer;
    case _template_vehicle_use:
        return _text_value_pair_parameter_integer;
    case _template_motion_tracker:
        return _text_value_pair_parameter_integer;
    case _template_motion_tracker_range:
        return _text_value_pair_parameter_integer;
    case _template_waypoint:
        return _text_value_pair_parameter_integer;
    case _template_active_camo:
        return _text_value_pair_parameter_integer;
    case _template_visual:
        return _text_value_pair_parameter_integer;
    case _template_forced_change_colors:
        return _text_value_pair_parameter_integer;
    default:
        return _text_value_pair_parameter_invalid;
    }
}

bool c_game_engine_base_variant::can_be_cast_to(e_game_engine_type engine_type, const void **variant) const
{
    *variant = 0;
    return false;
}

// Game variant: read one engine parameter (600 of them) as an integer. The variant's fields are
// addressed by offset: the union of engine variants starts at +4 and each parameter reads its own
// byte, word or bit. A parameter whose value is a name reads its index and returns the name's id.
#ifdef HALO3_64BIT
#define U8(b, o) (*(unsigned char *)((char *)(b) + (o) + (sizeof(void *) - 4)))
#define S8(b, o) (*(signed char *)((char *)(b) + (o) + (sizeof(void *) - 4)))
#define U16(b, o) (*(unsigned short *)((char *)(b) + (o) + (sizeof(void *) - 4)))
#define S16(b, o) (*(short *)((char *)(b) + (o) + (sizeof(void *) - 4)))
#define U32(b, o) (*(unsigned int *)((char *)(b) + (o) + (sizeof(void *) - 4)))
#else
#define U8(b, o) (*(unsigned char *)((char *)(b) + (o)))
#define S8(b, o) (*(signed char *)((char *)(b) + (o)))
#define U16(b, o) (*(unsigned short *)((char *)(b) + (o)))
#define S16(b, o) (*(short *)((char *)(b) + (o)))
#define U32(b, o) (*(unsigned int *)((char *)(b) + (o)))
#endif


static __forceinline s_multiplayer_vehicle_set *name_table_get(int index)
{
  s_multiplayer_universal_globals_definition *table = (s_multiplayer_universal_globals_definition *)scenario_multiplayer_globals_try_and_get_universal_data();
  if (table != 0 && index >= 0 && index < table->vehicle_set_count) return &table->vehicle_sets[index];
  return 0;
}

// @address 0x1c3b28 ?get_game_engine_setting@c_game_variant@@IBA_NW4e_game_variant_parameter@@W4e_text_value_pair_parameter_type@@PAJ@Z
bool c_game_variant::get_game_engine_setting(e_game_variant_parameter id, e_text_value_pair_parameter_type sub, long *out) const
{
  bool success = false;
  int value;
  switch (id) {
  case _game_variant_base_miscellaneous_teams:
    if (sub == _text_value_pair_parameter_integer) {
      value = (U8(this, 0x104) & 0x1);
      success = true;
    }
    break;
  case _game_variant_base_miscellaneous_round_reset_map:
    if (sub == _text_value_pair_parameter_integer) {
      bool enabled = (U8(this, 0x104) >> 1) & 1;
      if (enabled && (U8(this, 0x104) & 4)) {
        value = 2;
      } else {
        value = enabled != 0;
      }
      success = true;
    }
    break;
  case _game_variant_base_miscellaneous_round_time_limit:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x105);
      success = true;
    }
    break;
  case _game_variant_base_miscellaneous_round_limit:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x106);
      success = true;
    }
    break;
  case _game_variant_base_miscellaneous_early_victory_win_count:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x107);
      success = true;
    }
    break;
  case _game_variant_respawn_inherit_respawn_timer:
    if (sub == _text_value_pair_parameter_integer) {
      value = (U8(this, 0x108) & 0x1);
      success = true;
    }
    break;
  case _game_variant_respawn_with_teammate:
    if (sub == _text_value_pair_parameter_integer) {
      value = ((U8(this, 0x108) >> 1) & 1);
      success = true;
    }
    break;
  case _game_variant_respawn_at_location:
    if (sub == _text_value_pair_parameter_integer) {
      value = ((U8(this, 0x108) >> 2) & 1);
      success = true;
    }
    break;
  case _game_variant_respawn_upon_kill:
    if (sub == _text_value_pair_parameter_integer) {
      value = ((U8(this, 0x108) >> 3) & 1);
      success = true;
    }
    break;
  case _game_variant_respawn_lives_per_round:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x109);
      success = true;
    }
    break;
  case _game_variant_respawn_team_lives_per_round:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x10a);
      success = true;
    }
    break;
  case _game_variant_respawn_time:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x10b);
      success = true;
    }
    break;
  case _game_variant_respawn_suicide_penalty:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x10c);
      success = true;
    }
    break;
  case _game_variant_respawn_betrayal_penalty:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x10d);
      success = true;
    }
    break;
  case _game_variant_respawn_growth:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x10e);
      success = true;
    }
    break;
  case _game_variant_respawn_traits_duration:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x10f);
      success = true;
    }
    break;
  case _respawn_shld_dmg_resistance:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x110);
      success = true;
    }
    break;
  case _respawn_shld_multiplier:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x114);
      success = true;
    }
    break;
  case _respawn_shld_recharge_rate:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x111);
      success = true;
    }
    break;
  case _respawn_headshot_immunity:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x113);
      success = true;
    }
    break;
  case _respawn_vampirism:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x112);
      success = true;
    }
    break;
  case _respawn_dmg_modifier_percentage:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x11c);
      success = true;
    }
    break;
  case _respawn_initial_primary_weapon:
    if (sub == _text_value_pair_parameter_string_id) {
      value = game_engine_get_multiplayer_weapon_selection_name(S8(this, 0x11a));
      success = true;
    }
    break;
  case _respawn_initial_secondary_weapon:
    if (sub == _text_value_pair_parameter_string_id) {
      value = game_engine_get_multiplayer_weapon_selection_name(S8(this, 0x11b));
      success = true;
    }
    break;
  case _respawn_initial_grenade_count:
    if (sub == _text_value_pair_parameter_integer) {
      value = S16(this, 0x118);
      success = true;
    }
    break;
  case _respawn_recharging_grenades:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x11d);
      success = true;
    }
    break;
  case _respawn_infinite_ammo:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x11e);
      success = true;
    }
    break;
  case _respawn_weapon_pickup:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x11f);
      success = true;
    }
    break;
  case _respawn_walking_speed:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x120);
      success = true;
    }
    break;
  case _respawn_personal_gravity:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x121);
      success = true;
    }
    break;
  case _respawn_vehicle_use:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x122);
      success = true;
    }
    break;
  case _respawn_motion_tracker:
    if (sub == _text_value_pair_parameter_integer) {
      value = U16(this, 0x128);
      success = true;
    }
    break;
  case _respawn_motion_tracker_range:
    if (sub == _text_value_pair_parameter_integer) {
      value = U16(this, 0x12a);
      success = true;
    }
    break;
  case _respawn_active_camo:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x124);
      success = true;
    }
    break;
  case _respawn_waypoint:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x125);
      success = true;
    }
    break;
  case _respawn_visual:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x126);
      success = true;
    }
    break;
  case _respawn_forced_change_colors:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x127);
      success = true;
    }
    break;
  case _game_variant_social_observers:
    if (sub == _text_value_pair_parameter_integer) {
      value = 0;
      success = true;
    }
    break;
  case _game_variant_social_team_changing:
    if (sub == _text_value_pair_parameter_integer) {
      value = S16(this, 0x12e);
      success = true;
    }
    break;
  case _game_variant_social_friendly_fire:
    if (sub == _text_value_pair_parameter_integer) {
      value = (U16(this, 0x12c) & 0x1);
      success = true;
    }
    break;
  case _game_variant_social_betrayal_booting:
    if (sub == _text_value_pair_parameter_integer) {
      value = ((U16(this, 0x12c) >> 1) & 1);
      success = true;
    }
    break;
  case _game_variant_social_enemy_voice:
    if (sub == _text_value_pair_parameter_integer) {
      value = ((U16(this, 0x12c) >> 2) & 1);
      success = true;
    }
    break;
  case _game_variant_social_open_channel_voice:
    if (sub == _text_value_pair_parameter_integer) {
      value = ((U16(this, 0x12c) >> 3) & 1);
      success = true;
    }
    break;
  case _game_variant_social_dead_player_voice:
    if (sub == _text_value_pair_parameter_integer) {
      value = ((U16(this, 0x12c) >> 4) & 1);
      success = true;
    }
    break;
  case _game_variant_map_override_grenades:
    if (sub == _text_value_pair_parameter_integer) {
      value = (U32(this, 0x130) & 0x1);
      success = true;
    }
    break;
  case _game_variant_map_override_indestructible_vehicles:
    if (sub == _text_value_pair_parameter_integer) {
      value = ((U32(this, 0x130) >> 1) & 1);
      success = true;
    }
    break;
  case _player_shld_dmg_resistance:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x134);
      success = true;
    }
    break;
  case _player_shld_multiplier:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x138);
      success = true;
    }
    break;
  case _player_shld_recharge_rate:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x135);
      success = true;
    }
    break;
  case _player_headshot_immunity:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x137);
      success = true;
    }
    break;
  case _player_vampirism:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x136);
      success = true;
    }
    break;
  case _player_dmg_modifier_percentage:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x140);
      success = true;
    }
    break;
  case _player_initial_primary_weapon:
    if (sub == _text_value_pair_parameter_string_id) {
      value = game_engine_get_multiplayer_weapon_selection_name(S8(this, 0x13e));
      success = true;
    }
    break;
  case _player_initial_secondary_weapon:
    if (sub == _text_value_pair_parameter_string_id) {
      value = game_engine_get_multiplayer_weapon_selection_name(S8(this, 0x13f));
      success = true;
    }
    break;
  case _player_initial_grenade_count:
    if (sub == _text_value_pair_parameter_integer) {
      value = S16(this, 0x13c);
      success = true;
    }
    break;
  case _player_recharging_grenades:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x141);
      success = true;
    }
    break;
  case _player_infinite_ammo:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x142);
      success = true;
    }
    break;
  case _player_weapon_pickup:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x143);
      success = true;
    }
    break;
  case _player_walking_speed:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x144);
      success = true;
    }
    break;
  case _player_personal_gravity:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x145);
      success = true;
    }
    break;
  case _player_vehicle_use:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x146);
      success = true;
    }
    break;
  case _player_motion_tracker:
    if (sub == _text_value_pair_parameter_integer) {
      value = U16(this, 0x14c);
      success = true;
    }
    break;
  case _player_motion_tracker_range:
    if (sub == _text_value_pair_parameter_integer) {
      value = U16(this, 0x14e);
      success = true;
    }
    break;
  case _player_active_camo:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x148);
      success = true;
    }
    break;
  case _player_waypoint:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x149);
      success = true;
    }
    break;
  case _player_visual:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x14a);
      success = true;
    }
    break;
  case _player_forced_change_colors:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x14b);
      success = true;
    }
    break;
  case _game_variant_map_override_weapon_set:
    if (sub == _text_value_pair_parameter_string_id) {
      value = ((const c_game_engine_map_override_options *)&variant_flags)->get_weapon_set();
      success = true;
    }
    break;
  case _game_variant_map_override_vehicle_set:
    if (sub == _text_value_pair_parameter_string_id) {
      short index = S16(this, 0x152);
      int id = 1;
      if (index != -1) {
        s_multiplayer_vehicle_set *entry = name_table_get(index);
        if (entry != 0) id = entry->name;
      }
      value = id;
      success = true;
    }
    break;
  case _red_powerup_shld_dmg_resistance:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x154);
      success = true;
    }
    break;
  case _red_powerup_shld_multiplier:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x158);
      success = true;
    }
    break;
  case _red_powerup_shld_recharge_rate:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x155);
      success = true;
    }
    break;
  case _red_powerup_headshot_immunity:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x157);
      success = true;
    }
    break;
  case _red_powerup_vampirism:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x156);
      success = true;
    }
    break;
  case _red_powerup_dmg_modifier_percentage:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x160);
      success = true;
    }
    break;
  case _red_powerup_initial_primary_weapon:
    if (sub == _text_value_pair_parameter_string_id) {
      value = game_engine_get_multiplayer_weapon_selection_name(S8(this, 0x15e));
      success = true;
    }
    break;
  case _red_powerup_initial_secondary_weapon:
    if (sub == _text_value_pair_parameter_string_id) {
      value = game_engine_get_multiplayer_weapon_selection_name(S8(this, 0x15f));
      success = true;
    }
    break;
  case _red_powerup_initial_grenade_count:
    if (sub == _text_value_pair_parameter_integer) {
      value = S16(this, 0x15c);
      success = true;
    }
    break;
  case _red_powerup_recharging_grenades:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x161);
      success = true;
    }
    break;
  case _red_powerup_infinite_ammo:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x162);
      success = true;
    }
    break;
  case _red_powerup_weapon_pickup:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x163);
      success = true;
    }
    break;
  case _red_powerup_walking_speed:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x164);
      success = true;
    }
    break;
  case _red_powerup_personal_gravity:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x165);
      success = true;
    }
    break;
  case _red_powerup_vehicle_use:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x166);
      success = true;
    }
    break;
  case _red_powerup_motion_tracker:
    if (sub == _text_value_pair_parameter_integer) {
      value = U16(this, 0x16c);
      success = true;
    }
    break;
  case _red_powerup_motion_tracker_range:
    if (sub == _text_value_pair_parameter_integer) {
      value = U16(this, 0x16e);
      success = true;
    }
    break;
  case _red_powerup_active_camo:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x168);
      success = true;
    }
    break;
  case _red_powerup_waypoint:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x169);
      success = true;
    }
    break;
  case _red_powerup_visual:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x16a);
      success = true;
    }
    break;
  case _red_powerup_forced_change_colors:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x16b);
      success = true;
    }
    break;
  case _blue_powerup_shld_dmg_resistance:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x170);
      success = true;
    }
    break;
  case _blue_powerup_shld_multiplier:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x174);
      success = true;
    }
    break;
  case _blue_powerup_shld_recharge_rate:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x171);
      success = true;
    }
    break;
  case _blue_powerup_headshot_immunity:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x173);
      success = true;
    }
    break;
  case _blue_powerup_vampirism:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x172);
      success = true;
    }
    break;
  case _blue_powerup_dmg_modifier_percentage:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x17c);
      success = true;
    }
    break;
  case _blue_powerup_initial_primary_weapon:
    if (sub == _text_value_pair_parameter_string_id) {
      value = game_engine_get_multiplayer_weapon_selection_name(S8(this, 0x17a));
      success = true;
    }
    break;
  case _blue_powerup_initial_secondary_weapon:
    if (sub == _text_value_pair_parameter_string_id) {
      value = game_engine_get_multiplayer_weapon_selection_name(S8(this, 0x17b));
      success = true;
    }
    break;
  case _blue_powerup_initial_grenade_count:
    if (sub == _text_value_pair_parameter_integer) {
      value = S16(this, 0x178);
      success = true;
    }
    break;
  case _blue_powerup_recharging_grenades:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x17d);
      success = true;
    }
    break;
  case _blue_powerup_infinite_ammo:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x17e);
      success = true;
    }
    break;
  case _blue_powerup_weapon_pickup:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x17f);
      success = true;
    }
    break;
  case _blue_powerup_walking_speed:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x180);
      success = true;
    }
    break;
  case _blue_powerup_personal_gravity:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x181);
      success = true;
    }
    break;
  case _blue_powerup_vehicle_use:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x182);
      success = true;
    }
    break;
  case _blue_powerup_motion_tracker:
    if (sub == _text_value_pair_parameter_integer) {
      value = U16(this, 0x188);
      success = true;
    }
    break;
  case _blue_powerup_motion_tracker_range:
    if (sub == _text_value_pair_parameter_integer) {
      value = U16(this, 0x18a);
      success = true;
    }
    break;
  case _blue_powerup_active_camo:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x184);
      success = true;
    }
    break;
  case _blue_powerup_waypoint:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x185);
      success = true;
    }
    break;
  case _blue_powerup_visual:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x186);
      success = true;
    }
    break;
  case _blue_powerup_forced_change_colors:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x187);
      success = true;
    }
    break;
  case _yellow_powerup_shld_dmg_resistance:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x18c);
      success = true;
    }
    break;
  case _yellow_powerup_shld_multiplier:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x190);
      success = true;
    }
    break;
  case _yellow_powerup_shld_recharge_rate:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x18d);
      success = true;
    }
    break;
  case _yellow_powerup_headshot_immunity:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x18f);
      success = true;
    }
    break;
  case _yellow_powerup_vampirism:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x18e);
      success = true;
    }
    break;
  case _yellow_powerup_dmg_modifier_percentage:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x198);
      success = true;
    }
    break;
  case _yellow_powerup_initial_primary_weapon:
    if (sub == _text_value_pair_parameter_string_id) {
      value = game_engine_get_multiplayer_weapon_selection_name(S8(this, 0x196));
      success = true;
    }
    break;
  case _yellow_powerup_initial_secondary_weapon:
    if (sub == _text_value_pair_parameter_string_id) {
      value = game_engine_get_multiplayer_weapon_selection_name(S8(this, 0x197));
      success = true;
    }
    break;
  case _yellow_powerup_initial_grenade_count:
    if (sub == _text_value_pair_parameter_integer) {
      value = S16(this, 0x194);
      success = true;
    }
    break;
  case _yellow_powerup_recharging_grenades:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x199);
      success = true;
    }
    break;
  case _yellow_powerup_infinite_ammo:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x19a);
      success = true;
    }
    break;
  case _yellow_powerup_weapon_pickup:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x19b);
      success = true;
    }
    break;
  case _yellow_powerup_walking_speed:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x19c);
      success = true;
    }
    break;
  case _yellow_powerup_personal_gravity:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x19d);
      success = true;
    }
    break;
  case _yellow_powerup_vehicle_use:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x19e);
      success = true;
    }
    break;
  case _yellow_powerup_motion_tracker:
    if (sub == _text_value_pair_parameter_integer) {
      value = U16(this, 0x1a4);
      success = true;
    }
    break;
  case _yellow_powerup_motion_tracker_range:
    if (sub == _text_value_pair_parameter_integer) {
      value = U16(this, 0x1a6);
      success = true;
    }
    break;
  case _yellow_powerup_active_camo:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x1a0);
      success = true;
    }
    break;
  case _yellow_powerup_waypoint:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x1a1);
      success = true;
    }
    break;
  case _yellow_powerup_visual:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x1a2);
      success = true;
    }
    break;
  case _yellow_powerup_forced_change_colors:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x1a3);
      success = true;
    }
    break;
  case _game_variant_map_override_red_powerup_traits_duration:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x1a8);
      success = true;
    }
    break;
  case _game_variant_map_override_blue_powerup_traits_duration:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x1a9);
      success = true;
    }
    break;
  case _game_variant_map_override_yellow_powerup_traits_duration:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x1aa);
      success = true;
    }
    break;
  case _slayer_score_to_win:
    if (sub == _text_value_pair_parameter_integer) {
      value = S16(this, 0x1b4);
      success = true;
    }
    break;
  case _slayer_killing_spree_points:
    if (sub == _text_value_pair_parameter_integer) {
      value = S8(this, 0x1c3);
      success = true;
    }
    break;
  case _oddball_ball_effect:
    if (sub == _text_value_pair_parameter_integer) {
      value = ((U32(this, 0x1b4) >> 1) & 1);
      success = true;
    }
    break;
  case _ctf_game_type:
  case _infection_next_zombie:
    if (sub == _text_value_pair_parameter_integer) {
      value = S8(this, 0x1b6);
      success = true;
    }
    break;
  case _ctf_respawn:
  case _infection_initial_zombie_count:
    if (sub == _text_value_pair_parameter_integer) {
      value = S8(this, 0x1b7);
      success = true;
    }
    break;
  case _first_zombie_shld_multiplier:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x1e0);
      success = true;
    }
    break;
  case _first_zombie_initial_primary_weapon:
    if (sub == _text_value_pair_parameter_string_id) {
      value = game_engine_get_multiplayer_weapon_selection_name(S8(this, 0x1e6));
      success = true;
    }
    break;
  case _first_zombie_initial_secondary_weapon:
    if (sub == _text_value_pair_parameter_string_id) {
      value = game_engine_get_multiplayer_weapon_selection_name(S8(this, 0x1e7));
      success = true;
    }
    break;
  case _first_zombie_initial_grenade_count:
    if (sub == _text_value_pair_parameter_integer) {
      value = S16(this, 0x1e4);
      success = true;
    }
    break;
  case _first_zombie_weapon_pickup:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x1eb);
      success = true;
    }
    break;
  case _first_zombie_motion_tracker:
    if (sub == _text_value_pair_parameter_integer) {
      value = U16(this, 0x1f4);
      success = true;
    }
    break;
  case _first_zombie_motion_tracker_range:
    if (sub == _text_value_pair_parameter_integer) {
      value = U16(this, 0x1f6);
      success = true;
    }
    break;
  case _haven_defender_shld_multiplier:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x1fc);
      success = true;
    }
    break;
  case _haven_defender_dmg_modifier_percentage:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x204);
      success = true;
    }
    break;
  case _haven_defender_initial_primary_weapon:
    if (sub == _text_value_pair_parameter_string_id) {
      value = game_engine_get_multiplayer_weapon_selection_name(S8(this, 0x202));
      success = true;
    }
    break;
  case _haven_defender_initial_secondary_weapon:
    if (sub == _text_value_pair_parameter_string_id) {
      value = game_engine_get_multiplayer_weapon_selection_name(S8(this, 0x203));
      success = true;
    }
    break;
  case _haven_defender_initial_grenade_count:
    if (sub == _text_value_pair_parameter_integer) {
      value = S16(this, 0x200);
      success = true;
    }
    break;
  case _haven_defender_recharging_grenades:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x205);
      success = true;
    }
    break;
  case _haven_defender_infinite_ammo:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x206);
      success = true;
    }
    break;
  case _haven_defender_weapon_pickup:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x207);
      success = true;
    }
    break;
  case _haven_defender_walking_speed:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x208);
      success = true;
    }
    break;
  case _haven_defender_personal_gravity:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x209);
      success = true;
    }
    break;
  case _haven_defender_motion_tracker:
    if (sub == _text_value_pair_parameter_integer) {
      value = U16(this, 0x210);
      success = true;
    }
    break;
  case _haven_defender_motion_tracker_range:
    if (sub == _text_value_pair_parameter_integer) {
      value = U16(this, 0x212);
      success = true;
    }
    break;
  case _last_human_shld_multiplier:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x218);
      success = true;
    }
    break;
  case _last_human_headshot_immunity:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x217);
      success = true;
    }
    break;
  case _last_human_vampirism:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x216);
      success = true;
    }
    break;
  case _last_human_dmg_modifier_percentage:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x220);
      success = true;
    }
    break;
  case _last_human_initial_primary_weapon:
    if (sub == _text_value_pair_parameter_string_id) {
      value = game_engine_get_multiplayer_weapon_selection_name(S8(this, 0x21e));
      success = true;
    }
    break;
  case _last_human_initial_secondary_weapon:
    if (sub == _text_value_pair_parameter_string_id) {
      value = game_engine_get_multiplayer_weapon_selection_name(S8(this, 0x21f));
      success = true;
    }
    break;
  case _last_human_initial_grenade_count:
    if (sub == _text_value_pair_parameter_integer) {
      value = S16(this, 0x21c);
      success = true;
    }
    break;
  case _last_human_recharging_grenades:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x221);
      success = true;
    }
    break;
  case _last_human_infinite_ammo:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x222);
      success = true;
    }
    break;
  case _last_human_weapon_pickup:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x223);
      success = true;
    }
    break;
  case _last_human_walking_speed:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x224);
      success = true;
    }
    break;
  case _last_human_personal_gravity:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x225);
      success = true;
    }
    break;
  case _last_human_vehicle_use:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x226);
      success = true;
    }
    break;
  case _last_human_motion_tracker:
    if (sub == _text_value_pair_parameter_integer) {
      value = U16(this, 0x22c);
      success = true;
    }
    break;
  case _last_human_motion_tracker_range:
    if (sub == _text_value_pair_parameter_integer) {
      value = U16(this, 0x22e);
      success = true;
    }
    break;
  case _last_human_active_camo:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x228);
      success = true;
    }
    break;
  case _last_human_waypoint:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x229);
      success = true;
    }
    break;
  case _last_human_visual:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x22a);
      success = true;
    }
    break;
  case _last_human_forced_change_colors:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x22b);
      success = true;
    }
    break;
  case _oddball_auto_ball_pickup:
  case _game_variant_king_opaque_hill:
    if (sub == _text_value_pair_parameter_integer) {
      value = (U32(this, 0x1b4) & 0x1);
      success = true;
    }
    break;
  case _slayer_team_scoring:
  case _oddball_team_scoring:
  case _king_team_scoring:
    if (sub == _text_value_pair_parameter_integer) {
      value = S16(this, 0x1ae);
      success = true;
    }
    break;
  case _flag_carrier_initial_primary_weapon:
  case _zombie_initial_primary_weapon:
  case _on_hill_initial_primary_weapon:
    if (sub == _text_value_pair_parameter_string_id) {
      value = game_engine_get_multiplayer_weapon_selection_name(S8(this, 0x1ca));
      success = true;
    }
    break;
  case _flag_carrier_initial_secondary_weapon:
  case _zombie_initial_secondary_weapon:
  case _on_hill_initial_secondary_weapon:
    if (sub == _text_value_pair_parameter_string_id) {
      value = game_engine_get_multiplayer_weapon_selection_name(S8(this, 0x1cb));
      success = true;
    }
    break;
  case _flag_carrier_initial_grenade_count:
  case _zombie_initial_grenade_count:
  case _on_hill_initial_grenade_count:
    if (sub == _text_value_pair_parameter_integer) {
      value = S16(this, 0x1c8);
      success = true;
    }
    break;
  case _flag_carrier_weapon_pickup:
  case _zombie_weapon_pickup:
  case _on_hill_weapon_pickup:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x1cf);
      success = true;
    }
    break;
  case _flag_carrier_motion_tracker:
  case _zombie_motion_tracker:
  case _on_hill_motion_tracker:
    if (sub == _text_value_pair_parameter_integer) {
      value = U16(this, 0x1d8);
      success = true;
    }
    break;
  case _flag_carrier_motion_tracker_range:
  case _zombie_motion_tracker_range:
  case _on_hill_motion_tracker_range:
    if (sub == _text_value_pair_parameter_integer) {
      value = U16(this, 0x1da);
      success = true;
    }
    break;
  case _territories_lock_after_capture:
    if (sub == _text_value_pair_parameter_integer) {
      value = ((U16(this, 0x1b4) >> 1) & 1);
      success = true;
    }
    break;
  case _territory_defender_shld_recharge_rate:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x1bd);
      success = true;
    }
    break;
  case _territory_defender_headshot_immunity:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x1bf);
      success = true;
    }
    break;
  case _territory_defender_vampirism:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x1be);
      success = true;
    }
    break;
  case _territory_defender_initial_primary_weapon:
    if (sub == _text_value_pair_parameter_string_id) {
      value = game_engine_get_multiplayer_weapon_selection_name(S8(this, 0x1c6));
      success = true;
    }
    break;
  case _territory_defender_initial_secondary_weapon:
    if (sub == _text_value_pair_parameter_string_id) {
      value = game_engine_get_multiplayer_weapon_selection_name(S8(this, 0x1c7));
      success = true;
    }
    break;
  case _territory_defender_motion_tracker:
    if (sub == _text_value_pair_parameter_integer) {
      value = U16(this, 0x1d4);
      success = true;
    }
    break;
  case _territory_defender_motion_tracker_range:
    if (sub == _text_value_pair_parameter_integer) {
      value = U16(this, 0x1d6);
      success = true;
    }
    break;
  case _territory_attacker_initial_primary_weapon:
    if (sub == _text_value_pair_parameter_string_id) {
      value = game_engine_get_multiplayer_weapon_selection_name(S8(this, 0x1e2));
      success = true;
    }
    break;
  case _territory_attacker_initial_secondary_weapon:
    if (sub == _text_value_pair_parameter_string_id) {
      value = game_engine_get_multiplayer_weapon_selection_name(S8(this, 0x1e3));
      success = true;
    }
    break;
  case _territory_attacker_initial_grenade_count:
    if (sub == _text_value_pair_parameter_integer) {
      value = S16(this, 0x1e0);
      success = true;
    }
    break;
  case _first_zombie_recharging_grenades:
  case _territory_attacker_personal_gravity:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x1e9);
      success = true;
    }
    break;
  case _first_zombie_infinite_ammo:
  case _territory_attacker_vehicle_use:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x1ea);
      success = true;
    }
    break;
  case _territory_attacker_motion_tracker:
    if (sub == _text_value_pair_parameter_integer) {
      value = U16(this, 0x1f0);
      success = true;
    }
    break;
  case _territory_attacker_motion_tracker_range:
    if (sub == _text_value_pair_parameter_integer) {
      value = U16(this, 0x1f2);
      success = true;
    }
    break;
  case _first_zombie_walking_speed:
  case _territory_attacker_active_camo:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x1ec);
      success = true;
    }
    break;
  case _first_zombie_personal_gravity:
  case _territory_attacker_waypoint:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x1ed);
      success = true;
    }
    break;
  case _vip_single_vip:
    if (sub == _text_value_pair_parameter_integer) {
      value = (U16(this, 0x1b6) & 0x1);
      success = true;
    }
    break;
  case _vip_destination_zones_enabled:
    if (sub == _text_value_pair_parameter_integer) {
      value = ((U16(this, 0x1b6) >> 1) & 1);
      success = true;
    }
    break;
  case _vip_end_round_on_vip_death:
    if (sub == _text_value_pair_parameter_integer) {
      value = ((U16(this, 0x1b6) >> 2) & 1);
      success = true;
    }
    break;
  case _slayer_assist_points:
  case _vip_kill_points:
    if (sub == _text_value_pair_parameter_integer) {
      value = S8(this, 0x1b8);
      success = true;
    }
    break;
  case _slayer_death_points:
  case _vip_takedown_points:
    if (sub == _text_value_pair_parameter_integer) {
      value = S8(this, 0x1b9);
      success = true;
    }
    break;
  case _slayer_suicide_points:
  case _infection_zombie_kill_points:
  case _king_moving_hill:
  case _vip_kill_as_vip_points:
    if (sub == _text_value_pair_parameter_integer) {
      value = S8(this, 0x1ba);
      success = true;
    }
    break;
  case _slayer_betrayal_points:
  case _infection_points:
  case _king_moving_hill_order:
  case _vip_death_points:
    if (sub == _text_value_pair_parameter_integer) {
      value = S8(this, 0x1bb);
      success = true;
    }
    break;
  case _slayer_leader_killed_points:
  case _oddball_kill_points:
  case _infection_safe_haven_arrival_points:
  case _king_inside_hill_points:
  case _vip_zone_arrival_points:
    if (sub == _text_value_pair_parameter_integer) {
      value = S8(this, 0x1bc);
      success = true;
    }
    break;
  case _flag_carrier_shld_dmg_resistance:
  case _zombie_shld_dmg_resistance:
  case _on_hill_shld_dmg_resistance:
  case _territory_defender_shld_multiplier:
  case _vip_selection:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x1c0);
      success = true;
    }
    break;
  case _flag_carrier_shld_recharge_rate:
  case _zombie_shld_recharge_rate:
  case _on_hill_shld_recharge_rate:
  case _vip_zone_movement:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x1c1);
      success = true;
    }
    break;
  case _flag_carrier_vampirism:
  case _zombie_vampirism:
  case _on_hill_vampirism:
  case _vip_zone_order:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x1c2);
      success = true;
    }
    break;
  case _vip_team_initial_primary_weapon:
    if (sub == _text_value_pair_parameter_string_id) {
      value = game_engine_get_multiplayer_weapon_selection_name(S8(this, 0x1d0));
      success = true;
    }
    break;
  case _vip_team_initial_secondary_weapon:
    if (sub == _text_value_pair_parameter_string_id) {
      value = game_engine_get_multiplayer_weapon_selection_name(S8(this, 0x1d1));
      success = true;
    }
    break;
  case _vip_team_initial_grenade_count:
    if (sub == _text_value_pair_parameter_integer) {
      value = S16(this, 0x1ce);
      success = true;
    }
    break;
  case _vip_influence_shld_dmg_resistance:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x1e2);
      success = true;
    }
    break;
  case _vip_influence_shld_recharge_rate:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x1e3);
      success = true;
    }
    break;
  case _first_zombie_vehicle_use:
  case _territory_attacker_visual:
  case _vip_influence_dmg_modifier_percentage:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x1ee);
      success = true;
    }
    break;
  case _vip_influence_initial_primary_weapon:
    if (sub == _text_value_pair_parameter_string_id) {
      value = game_engine_get_multiplayer_weapon_selection_name(S8(this, 0x1ec));
      success = true;
    }
    break;
  case _vip_influence_initial_secondary_weapon:
    if (sub == _text_value_pair_parameter_string_id) {
      value = game_engine_get_multiplayer_weapon_selection_name(S8(this, 0x1ed));
      success = true;
    }
    break;
  case _vip_influence_initial_grenade_count:
    if (sub == _text_value_pair_parameter_integer) {
      value = S16(this, 0x1ea);
      success = true;
    }
    break;
  case _territory_attacker_forced_change_colors:
  case _vip_influence_recharging_grenades:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x1ef);
      success = true;
    }
    break;
  case _vip_influence_motion_tracker:
    if (sub == _text_value_pair_parameter_integer) {
      value = U16(this, 0x1fa);
      success = true;
    }
    break;
  case _vip_influence_waypoint:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x1f7);
      success = true;
    }
    break;
  case _vip_shld_dmg_resistance:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x1fe);
      success = true;
    }
    break;
  case _vip_shld_multiplier:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x202);
      success = true;
    }
    break;
  case _vip_shld_recharge_rate:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x1ff);
      success = true;
    }
    break;
  case _vip_headshot_immunity:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x201);
      success = true;
    }
    break;
  case _vip_vampirism:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x200);
      success = true;
    }
    break;
  case _haven_defender_vehicle_use:
  case _vip_dmg_modifier_percentage:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x20a);
      success = true;
    }
    break;
  case _vip_initial_primary_weapon:
    if (sub == _text_value_pair_parameter_string_id) {
      value = game_engine_get_multiplayer_weapon_selection_name(S8(this, 0x208));
      success = true;
    }
    break;
  case _vip_initial_secondary_weapon:
    if (sub == _text_value_pair_parameter_string_id) {
      value = game_engine_get_multiplayer_weapon_selection_name(S8(this, 0x209));
      success = true;
    }
    break;
  case _vip_initial_grenade_count:
    if (sub == _text_value_pair_parameter_integer) {
      value = S16(this, 0x206);
      success = true;
    }
    break;
  case _vip_recharging_grenades:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x20b);
      success = true;
    }
    break;
  case _haven_defender_active_camo:
  case _vip_infinite_ammo:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x20c);
      success = true;
    }
    break;
  case _haven_defender_waypoint:
  case _vip_weapon_pickup:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x20d);
      success = true;
    }
    break;
  case _haven_defender_visual:
  case _vip_walking_speed:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x20e);
      success = true;
    }
    break;
  case _haven_defender_forced_change_colors:
  case _vip_personal_gravity:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x20f);
      success = true;
    }
    break;
  case _vip_vehicle_use:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x210);
      success = true;
    }
    break;
  case _vip_motion_tracker:
    if (sub == _text_value_pair_parameter_integer) {
      value = U16(this, 0x216);
      success = true;
    }
    break;
  case _vip_motion_tracker_range:
    if (sub == _text_value_pair_parameter_integer) {
      value = U16(this, 0x218);
      success = true;
    }
    break;
  case _vip_active_camo:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x212);
      success = true;
    }
    break;
  case _vip_waypoint:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x213);
      success = true;
    }
    break;
  case _last_human_shld_dmg_resistance:
  case _vip_visual:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x214);
      success = true;
    }
    break;
  case _last_human_shld_recharge_rate:
  case _vip_forced_change_colors:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x215);
      success = true;
    }
    break;
  case _vip_score_to_win:
  case _juggernaut_score_to_win:
    if (sub == _text_value_pair_parameter_integer) {
      value = ((const c_game_engine_base_variant *)variant_storage)->get_score_to_win_round();
      success = true;
    }
    break;
  case _slayer_elimination_points:
  case _oddball_ball_kill_points:
  case _infection_suicide_points:
  case _king_outside_hill_points:
  case _vip_suicide_points:
  case _juggernaut_kill_points:
    if (sub == _text_value_pair_parameter_integer) {
      value = S8(this, 0x1bd);
      success = true;
    }
    break;
  case _slayer_assassination_points:
  case _oddball_carrier_kill_points:
  case _infection_betrayal_points:
  case _king_uncontested_hill_points:
  case _vip_betrayal_points:
  case _juggernaut_juggernaut_kill_points:
    if (sub == _text_value_pair_parameter_integer) {
      value = S8(this, 0x1be);
      success = true;
    }
    break;
  case _slayer_headshot_points:
  case _oddball_ball_count:
  case _infection_last_man_standing_bonus_points:
  case _king_kill_points:
  case _vip_vip_friendly_death_points:
  case _juggernaut_kill_as_juggernaut_points:
    if (sub == _text_value_pair_parameter_integer) {
      value = S8(this, 0x1bf);
      success = true;
    }
    break;
  case _slayer_melee_points:
  case _juggernaut_zone_arrival_points:
    if (sub == _text_value_pair_parameter_integer) {
      value = S8(this, 0x1c0);
      success = true;
    }
    break;
  case _slayer_sticky_points:
  case _juggernaut_suicide_points:
    if (sub == _text_value_pair_parameter_integer) {
      value = S8(this, 0x1c1);
      success = true;
    }
    break;
  case _slayer_splatter_points:
  case _juggernaut_betrayal_points:
    if (sub == _text_value_pair_parameter_integer) {
      value = S8(this, 0x1c2);
      success = true;
    }
    break;
  case _juggernaut_allied_against_juggernaut:
    if (sub == _text_value_pair_parameter_integer) {
      value = (U8(this, 0x1ba) & 0x1);
      success = true;
    }
    break;
  case _flag_carrier_headshot_immunity:
  case _zombie_headshot_immunity:
  case _on_hill_headshot_immunity:
  case _juggernaut_delay:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x1c3);
      success = true;
    }
    break;
  case _juggernaut_zones_enabled:
    if (sub == _text_value_pair_parameter_integer) {
      value = ((U8(this, 0x1ba) >> 2) & 1);
      success = true;
    }
    break;
  case _juggernaut_respawn_on_lone_juggernaut:
    if (sub == _text_value_pair_parameter_integer) {
      value = ((U8(this, 0x1ba) >> 1) & 1);
      success = true;
    }
    break;
  case _slayer_leader_dmg_modifier_percentage:
  case _ball_carrier_dmg_modifier_percentage:
  case _flag_carrier_walking_speed:
  case _zombie_walking_speed:
  case _on_hill_walking_speed:
  case _territory_defender_active_camo:
  case _juggernaut_dmg_modifier_percentage:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x1d0);
      success = true;
    }
    break;
  case _slayer_leader_initial_primary_weapon:
  case _ball_carrier_initial_primary_weapon:
  case _juggernaut_initial_primary_weapon:
    if (sub == _text_value_pair_parameter_string_id) {
      value = game_engine_get_multiplayer_weapon_selection_name(S8(this, 0x1ce));
      success = true;
    }
    break;
  case _slayer_leader_initial_secondary_weapon:
  case _ball_carrier_initial_secondary_weapon:
  case _juggernaut_initial_secondary_weapon:
    if (sub == _text_value_pair_parameter_string_id) {
      value = game_engine_get_multiplayer_weapon_selection_name(S8(this, 0x1cf));
      success = true;
    }
    break;
  case _slayer_leader_initial_grenade_count:
  case _ball_carrier_initial_grenade_count:
  case _juggernaut_initial_grenade_count:
    if (sub == _text_value_pair_parameter_integer) {
      value = S16(this, 0x1cc);
      success = true;
    }
    break;
  case _slayer_leader_recharging_grenades:
  case _ball_carrier_recharging_grenades:
  case _flag_carrier_personal_gravity:
  case _zombie_personal_gravity:
  case _on_hill_personal_gravity:
  case _territory_defender_waypoint:
  case _juggernaut_recharging_grenades:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x1d1);
      success = true;
    }
    break;
  case _slayer_leader_infinite_ammo:
  case _ball_carrier_infinite_ammo:
  case _flag_carrier_vehicle_use:
  case _zombie_vehicle_use:
  case _on_hill_vehicle_use:
  case _territory_defender_visual:
  case _vip_team_dmg_modifier_percentage:
  case _juggernaut_infinite_ammo:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x1d2);
      success = true;
    }
    break;
  case _slayer_leader_weapon_pickup:
  case _ball_carrier_weapon_pickup:
  case _territory_defender_forced_change_colors:
  case _vip_team_recharging_grenades:
  case _juggernaut_weapon_pickup:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x1d3);
      success = true;
    }
    break;
  case _slayer_leader_motion_tracker:
  case _ball_carrier_motion_tracker:
  case _juggernaut_motion_tracker:
    if (sub == _text_value_pair_parameter_integer) {
      value = U16(this, 0x1dc);
      success = true;
    }
    break;
  case _slayer_leader_motion_tracker_range:
  case _ball_carrier_motion_tracker_range:
  case _vip_team_motion_tracker:
  case _juggernaut_motion_tracker_range:
    if (sub == _text_value_pair_parameter_integer) {
      value = U16(this, 0x1de);
      success = true;
    }
    break;
  case _slayer_leader_forced_change_colors:
  case _ball_carrier_forced_change_colors:
  case _territory_attacker_headshot_immunity:
  case _vip_team_waypoint:
  case _juggernaut_forced_change_colors:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x1db);
      success = true;
    }
    break;
  case _territory_attacker_dmg_modifier_percentage:
  case _vip_influence_vampirism:
  case _assault_arming_shld_dmg_resistance:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x1e4);
      success = true;
    }
    break;
  case _first_zombie_dmg_modifier_percentage:
  case _territory_attacker_walking_speed:
  case _assault_arming_shld_multiplier:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x1e8);
      success = true;
    }
    break;
  case _territory_attacker_recharging_grenades:
  case _vip_influence_headshot_immunity:
  case _assault_arming_shld_recharge_rate:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x1e5);
      success = true;
    }
    break;
  case _territory_attacker_weapon_pickup:
  case _assault_arming_headshot_immunity:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x1e7);
      success = true;
    }
    break;
  case _territory_attacker_infinite_ammo:
  case _vip_influence_shld_multiplier:
  case _assault_arming_vampirism:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x1e6);
      success = true;
    }
    break;
  case _first_zombie_active_camo:
  case _vip_influence_infinite_ammo:
  case _assault_arming_dmg_modifier_percentage:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x1f0);
      success = true;
    }
    break;
  case _assault_arming_initial_primary_weapon:
    if (sub == _text_value_pair_parameter_string_id) {
      value = game_engine_get_multiplayer_weapon_selection_name(S8(this, 0x1ee));
      success = true;
    }
    break;
  case _assault_arming_initial_secondary_weapon:
    if (sub == _text_value_pair_parameter_string_id) {
      value = game_engine_get_multiplayer_weapon_selection_name(S8(this, 0x1ef));
      success = true;
    }
    break;
  case _assault_arming_initial_grenade_count:
    if (sub == _text_value_pair_parameter_integer) {
      value = S16(this, 0x1ec);
      success = true;
    }
    break;
  case _first_zombie_waypoint:
  case _vip_influence_weapon_pickup:
  case _assault_arming_recharging_grenades:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x1f1);
      success = true;
    }
    break;
  case _first_zombie_visual:
  case _vip_influence_walking_speed:
  case _assault_arming_infinite_ammo:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x1f2);
      success = true;
    }
    break;
  case _first_zombie_forced_change_colors:
  case _vip_influence_personal_gravity:
  case _assault_arming_weapon_pickup:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x1f3);
      success = true;
    }
    break;
  case _vip_influence_vehicle_use:
  case _assault_arming_walking_speed:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x1f4);
      success = true;
    }
    break;
  case _assault_arming_personal_gravity:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x1f5);
      success = true;
    }
    break;
  case _vip_influence_active_camo:
  case _assault_arming_vehicle_use:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x1f6);
      success = true;
    }
    break;
  case _vip_influence_motion_tracker_range:
  case _assault_arming_motion_tracker:
    if (sub == _text_value_pair_parameter_integer) {
      value = U16(this, 0x1fc);
      success = true;
    }
    break;
  case _assault_arming_motion_tracker_range:
    if (sub == _text_value_pair_parameter_integer) {
      value = U16(this, 0x1fe);
      success = true;
    }
    break;
  case _haven_defender_shld_dmg_resistance:
  case _vip_influence_visual:
  case _assault_arming_active_camo:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x1f8);
      success = true;
    }
    break;
  case _haven_defender_shld_recharge_rate:
  case _vip_influence_forced_change_colors:
  case _assault_arming_waypoint:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x1f9);
      success = true;
    }
    break;
  case _haven_defender_vampirism:
  case _assault_arming_visual:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x1fa);
      success = true;
    }
    break;
  case _haven_defender_headshot_immunity:
  case _assault_arming_forced_change_colors:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x1fb);
      success = true;
    }
    break;
  case _oddball_inactive_respawn_delay:
  case _assault_bomb_arming_time:
    if (sub == _text_value_pair_parameter_integer) {
      value = S16(this, 0x1c2);
      success = true;
    }
    break;
  case _territory_defender_initial_grenade_count:
  case _vip_influence_radius:
  case _assault_bomb_disarming_time:
    if (sub == _text_value_pair_parameter_integer) {
      value = S16(this, 0x1c4);
      success = true;
    }
    break;
  case _assault_bomb_fuse_time:
    if (sub == _text_value_pair_parameter_integer) {
      value = S16(this, 0x1c6);
      success = true;
    }
    break;
  case _territory_defender_weapon_pickup:
  case _assault_carrier_headshot_immunity:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x1cb);
      success = true;
    }
    break;
  case _slayer_leader_walking_speed:
  case _ball_carrier_walking_speed:
  case _flag_carrier_active_camo:
  case _zombie_active_camo:
  case _on_hill_active_camo:
  case _vip_team_infinite_ammo:
  case _juggernaut_walking_speed:
  case _assault_carrier_dmg_modifier_percentage:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x1d4);
      success = true;
    }
    break;
  case _assault_carrier_initial_primary_weapon:
    if (sub == _text_value_pair_parameter_string_id) {
      value = game_engine_get_multiplayer_weapon_selection_name(S8(this, 0x1d2));
      success = true;
    }
    break;
  case _assault_carrier_initial_secondary_weapon:
    if (sub == _text_value_pair_parameter_string_id) {
      value = game_engine_get_multiplayer_weapon_selection_name(S8(this, 0x1d3));
      success = true;
    }
    break;
  case _assault_carrier_initial_grenade_count:
    if (sub == _text_value_pair_parameter_integer) {
      value = S16(this, 0x1d0);
      success = true;
    }
    break;
  case _slayer_leader_personal_gravity:
  case _ball_carrier_personal_gravity:
  case _flag_carrier_waypoint:
  case _zombie_waypoint:
  case _on_hill_waypoint:
  case _vip_team_weapon_pickup:
  case _juggernaut_personal_gravity:
  case _assault_carrier_recharging_grenades:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x1d5);
      success = true;
    }
    break;
  case _slayer_leader_vehicle_use:
  case _ball_carrier_vehicle_use:
  case _flag_carrier_visual:
  case _zombie_visual:
  case _on_hill_visual:
  case _vip_team_walking_speed:
  case _juggernaut_vehicle_use:
  case _assault_carrier_infinite_ammo:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x1d6);
      success = true;
    }
    break;
  case _flag_carrier_forced_change_colors:
  case _zombie_forced_change_colors:
  case _on_hill_forced_change_colors:
  case _vip_team_personal_gravity:
  case _assault_carrier_weapon_pickup:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x1d7);
      success = true;
    }
    break;
  case _slayer_leader_active_camo:
  case _ball_carrier_active_camo:
  case _territory_attacker_shld_dmg_resistance:
  case _vip_team_vehicle_use:
  case _juggernaut_active_camo:
  case _assault_carrier_walking_speed:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x1d8);
      success = true;
    }
    break;
  case _slayer_leader_waypoint:
  case _ball_carrier_waypoint:
  case _territory_attacker_shld_recharge_rate:
  case _juggernaut_waypoint:
  case _assault_carrier_personal_gravity:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x1d9);
      success = true;
    }
    break;
  case _slayer_leader_visual:
  case _ball_carrier_visual:
  case _territory_attacker_vampirism:
  case _vip_team_active_camo:
  case _juggernaut_visual:
  case _assault_carrier_vehicle_use:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x1da);
      success = true;
    }
    break;
  case _vip_team_motion_tracker_range:
  case _assault_carrier_motion_tracker:
    if (sub == _text_value_pair_parameter_integer) {
      value = U16(this, 0x1e0);
      success = true;
    }
    break;
  case _assault_carrier_motion_tracker_range:
    if (sub == _text_value_pair_parameter_integer) {
      value = U16(this, 0x1e2);
      success = true;
    }
    break;
  case _first_zombie_shld_dmg_resistance:
  case _territory_attacker_shld_multiplier:
  case _vip_team_visual:
  case _assault_carrier_active_camo:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x1dc);
      success = true;
    }
    break;
  case _first_zombie_shld_recharge_rate:
  case _vip_team_forced_change_colors:
  case _assault_carrier_waypoint:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x1dd);
      success = true;
    }
    break;
  case _first_zombie_vampirism:
  case _assault_carrier_visual:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x1de);
      success = true;
    }
    break;
  case _first_zombie_headshot_immunity:
  case _assault_carrier_forced_change_colors:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x1df);
      success = true;
    }
    break;
  case _oddball_carrying_points:
  case _ctf_sudden_death_time:
  case _territories_sudden_death:
  case _assault_enemy_bomb_waypoint:
    if (sub == _text_value_pair_parameter_integer) {
      value = S16(this, 0x1ba);
      success = true;
    }
    break;
  case _oddball_score_to_win:
  case _ctf_touch_return_timeout:
  case _infection_safe_haven_movement_time:
  case _king_score_to_win:
  case _territories_capture_time:
  case _assault_game_type:
    if (sub == _text_value_pair_parameter_integer) {
      value = S16(this, 0x1b8);
      success = true;
    }
    break;
  case _territories_one_sided:
  case _assault_reset_bomb_on_disarm:
    if (sub == _text_value_pair_parameter_integer) {
      value = (U16(this, 0x1b4) & 0x1);
      success = true;
    }
    break;
  case _ctf_score_to_win:
  case _assault_score_to_win:
    if (sub == _text_value_pair_parameter_integer) {
      value = S16(this, 0x1bc);
      success = true;
    }
    break;
  case _ctf_flag_reset_time:
  case _assault_sudden_death_time:
    if (sub == _text_value_pair_parameter_integer) {
      value = S16(this, 0x1be);
      success = true;
    }
    break;
  case _ctf_flag_at_home_to_score:
  case _infection_respawn_on_haven_move:
  case _sandbox_open_channel_voice:
    if (sub == _text_value_pair_parameter_integer) {
      value = (U8(this, 0x1b4) & 0x1);
      success = true;
    }
    break;
  case _ctf_home_flag_waypoint:
  case _infection_safe_haven_setting:
  case _sandbox_enter_editor_mode:
    if (sub == _text_value_pair_parameter_integer) {
      value = S8(this, 0x1b5);
      success = true;
    }
    break;
  case _slayer_kill_points:
  case _territories_respawn_on_capture:
  case _assault_respawn:
  case _sandbox_respawn_time:
    if (sub == _text_value_pair_parameter_integer) {
      value = S16(this, 0x1b6);
      success = true;
    }
    break;
  case _juggernaut_initial_juggernaut:
  case _sandbox_editor_shld_dmg_resistance:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x1b8);
      success = true;
    }
    break;
  case _territory_defender_shld_dmg_resistance:
  case _juggernaut_zone_order:
  case _sandbox_editor_shld_multiplier:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x1bc);
      success = true;
    }
    break;
  case _juggernaut_next_juggernaut:
  case _sandbox_editor_shld_recharge_rate:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x1b9);
      success = true;
    }
    break;
  case _juggernaut_zone_movement:
  case _sandbox_editor_headshot_immunity:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x1bb);
      success = true;
    }
    break;
  case _sandbox_editor_vampirism:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x1ba);
      success = true;
    }
    break;
  case _slayer_leader_shld_dmg_resistance:
  case _ball_carrier_shld_dmg_resistance:
  case _flag_carrier_shld_multiplier:
  case _zombie_shld_multiplier:
  case _on_hill_shld_multiplier:
  case _juggernaut_shld_dmg_resistance:
  case _sandbox_editor_dmg_modifier_percentage:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x1c4);
      success = true;
    }
    break;
  case _sandbox_editor_initial_primary_weapon:
    if (sub == _text_value_pair_parameter_string_id) {
      value = game_engine_get_multiplayer_weapon_selection_name(S8(this, 0x1c2));
      success = true;
    }
    break;
  case _sandbox_editor_initial_secondary_weapon:
    if (sub == _text_value_pair_parameter_string_id) {
      value = game_engine_get_multiplayer_weapon_selection_name(S8(this, 0x1c3));
      success = true;
    }
    break;
  case _oddball_ball_spawn_delay:
  case _assault_bomb_reset_time:
  case _sandbox_editor_initial_grenade_count:
    if (sub == _text_value_pair_parameter_integer) {
      value = S16(this, 0x1c0);
      success = true;
    }
    break;
  case _slayer_leader_shld_recharge_rate:
  case _ball_carrier_shld_recharge_rate:
  case _juggernaut_shld_recharge_rate:
  case _sandbox_editor_recharging_grenades:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x1c5);
      success = true;
    }
    break;
  case _slayer_leader_vampirism:
  case _ball_carrier_vampirism:
  case _vip_team_shld_dmg_resistance:
  case _juggernaut_vampirism:
  case _sandbox_editor_infinite_ammo:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x1c6);
      success = true;
    }
    break;
  case _slayer_leader_headshot_immunity:
  case _ball_carrier_headshot_immunity:
  case _vip_team_shld_recharge_rate:
  case _juggernaut_headshot_immunity:
  case _sandbox_editor_weapon_pickup:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x1c7);
      success = true;
    }
    break;
  case _slayer_leader_shld_multiplier:
  case _ball_carrier_shld_multiplier:
  case _territory_defender_dmg_modifier_percentage:
  case _vip_team_vampirism:
  case _juggernaut_shld_multiplier:
  case _assault_carrier_shld_dmg_resistance:
  case _sandbox_editor_walking_speed:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x1c8);
      success = true;
    }
    break;
  case _territory_defender_recharging_grenades:
  case _vip_team_headshot_immunity:
  case _assault_carrier_shld_recharge_rate:
  case _sandbox_editor_personal_gravity:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x1c9);
      success = true;
    }
    break;
  case _territory_defender_infinite_ammo:
  case _vip_team_shld_multiplier:
  case _assault_carrier_vampirism:
  case _sandbox_editor_vehicle_use:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x1ca);
      success = true;
    }
    break;
  case _sandbox_editor_motion_tracker:
    if (sub == _text_value_pair_parameter_integer) {
      value = U16(this, 0x1d0);
      success = true;
    }
    break;
  case _sandbox_editor_motion_tracker_range:
    if (sub == _text_value_pair_parameter_integer) {
      value = U16(this, 0x1d2);
      success = true;
    }
    break;
  case _flag_carrier_dmg_modifier_percentage:
  case _zombie_dmg_modifier_percentage:
  case _on_hill_dmg_modifier_percentage:
  case _territory_defender_walking_speed:
  case _assault_carrier_shld_multiplier:
  case _sandbox_editor_active_camo:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x1cc);
      success = true;
    }
    break;
  case _flag_carrier_recharging_grenades:
  case _zombie_recharging_grenades:
  case _on_hill_recharging_grenades:
  case _territory_defender_personal_gravity:
  case _sandbox_editor_waypoint:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x1cd);
      success = true;
    }
    break;
  case _flag_carrier_infinite_ammo:
  case _zombie_infinite_ammo:
  case _on_hill_infinite_ammo:
  case _territory_defender_vehicle_use:
  case _sandbox_editor_visual:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x1ce);
      success = true;
    }
    break;
  case _sandbox_editor_forced_change_colors:
    if (sub == _text_value_pair_parameter_integer) {
      value = U8(this, 0x1cf);
      success = true;
    }
    break;
  }
  if (success) {
    *out = value;
  }
  return success;
}

// 0x1c6298

inline s_game_engine_setting_variant *c_game_variant::writeable()
{
  s_game_engine_setting_variant *variant = (s_game_engine_setting_variant *)variant_storage;
  variant->variant_flags.clear(0);
  return variant;
}

// @address 0x1c6298 ?set_game_engine_setting@c_game_variant@@IAA_NW4e_game_variant_parameter@@W4e_text_value_pair_parameter_type@@J@Z
bool c_game_variant::set_game_engine_setting(enum e_game_variant_parameter parameter, enum e_text_value_pair_parameter_type parameter_type, long value)
{
  bool result = false;
  switch (parameter) {
  case _game_variant_base_miscellaneous_teams:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if (value != 0) {
        v->fields.or_u8(0xfc, 0x1);
      } else {
        v->fields.and_u8(0xfc, 0xfe);
      }
      result = true; break;
    }
    break;
  case _game_variant_base_miscellaneous_round_reset_map:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 2) {
        bool a = false;
        bool b = false;
        if (value == 1) a = true;
        else if (value == 2) { a = true; b = true; }
        v->fields.byte_flags(0xfc)->set(1, a);
        v->fields.byte_flags(0xfc)->set(2, b);
      } else {
        v->fields.and_u8(0xfc, 0xf9);
      }
      result = true;
    }
    break;
  case _game_variant_base_miscellaneous_round_time_limit:
    if (parameter_type == _text_value_pair_parameter_integer) {
      unsigned char x = (unsigned char)value;
      c_game_engine_variant *v = this->writeable();
      if (x > 60) {
        x = 8;
      }
      v->fields.set_u8(0xfd, x);
      result = true; break;
    }
    break;
  case _game_variant_base_miscellaneous_round_limit:
    if (parameter_type == _text_value_pair_parameter_integer) {
      unsigned char x = (unsigned char)value;
      c_game_engine_variant *v = this->writeable();
      if (x > 10) {
        x = 1;
      }
      v->fields.set_u8(0xfe, x);
      result = true; break;
    }
    break;
  case _game_variant_base_miscellaneous_early_victory_win_count:
    if (parameter_type == _text_value_pair_parameter_integer) {
      unsigned char x = (unsigned char)value;
      c_game_engine_variant *v = this->writeable();
      if (x > 5) {
        x = 2;
      }
      v->fields.set_u8(0xff, x);
      result = true; break;
    }
    break;
  case _game_variant_respawn_inherit_respawn_timer:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if (value != 0) {
        v->fields.or_u8(0x100, 0x1);
      } else {
        v->fields.and_u8(0x100, 0xfe);
      }
      result = true; break;
    }
    break;
  case _game_variant_respawn_with_teammate:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if (value != 0) {
        v->fields.or_u8(0x100, 0x2);
      } else {
        v->fields.and_u8(0x100, 0xfd);
      }
      result = true; break;
    }
    break;
  case _game_variant_respawn_at_location:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if (value != 0) {
        v->fields.or_u8(0x100, 0x4);
      } else {
        v->fields.and_u8(0x100, 0xfb);
      }
      result = true; break;
    }
    break;
  case _game_variant_respawn_upon_kill:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if (value != 0) {
        v->fields.or_u8(0x100, 0x8);
      } else {
        v->fields.and_u8(0x100, 0xf7);
      }
      result = true; break;
    }
    break;
  case _game_variant_respawn_lives_per_round:
    if (parameter_type == _text_value_pair_parameter_integer) {
      unsigned char x = (unsigned char)value;
      c_game_engine_variant *v = this->writeable();
      if (x <= 50) {
        v->fields.set_u8(0x101, x);
      } else {
        v->fields.set_u8(0x101, 0);
      }
      result = true; break;
    }
    break;
  case _game_variant_respawn_team_lives_per_round:
    if (parameter_type == _text_value_pair_parameter_integer) {
      unsigned char x = (unsigned char)value;
      c_game_engine_variant *v = this->writeable();
      if (x <= 100) {
        v->fields.set_u8(0x102, x);
      } else {
        v->fields.set_u8(0x102, 0);
      }
      result = true; break;
    }
    break;
  case _game_variant_respawn_time:
    if (parameter_type == _text_value_pair_parameter_integer) {
      unsigned char x = (unsigned char)value;
      c_game_engine_variant *v = this->writeable();
      if (x > 240) {
        x = 5;
      }
      v->fields.set_u8(0x103, x);
      result = true; break;
    }
    break;
  case _game_variant_respawn_suicide_penalty:
    if (parameter_type == _text_value_pair_parameter_integer) {
      unsigned char x = (unsigned char)value;
      c_game_engine_variant *v = this->writeable();
      if (x > 240) {
        x = 5;
      }
      v->fields.set_u8(0x104, x);
      result = true; break;
    }
    break;
  case _game_variant_respawn_betrayal_penalty:
    if (parameter_type == _text_value_pair_parameter_integer) {
      unsigned char x = (unsigned char)value;
      c_game_engine_variant *v = this->writeable();
      if (x > 240) {
        x = 5;
      }
      v->fields.set_u8(0x105, x);
      result = true; break;
    }
    break;
  case _game_variant_respawn_growth:
    if (parameter_type == _text_value_pair_parameter_integer) {
      unsigned char x = (unsigned char)value;
      c_game_engine_variant *v = this->writeable();
      if (x <= 15) {
        v->fields.set_u8(0x106, x);
      } else {
        v->fields.set_u8(0x106, 0);
      }
      result = true; break;
    }
    break;
  case _game_variant_respawn_traits_duration:
    if (parameter_type == _text_value_pair_parameter_integer) {
      unsigned char x = (unsigned char)value;
      c_game_engine_variant *v = this->writeable();
      if (x > 60) {
        x = 5;
      }
      v->fields.set_u8(0x107, x);
      result = true; break;
    }
    break;
  case _respawn_shld_dmg_resistance:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 12) {
        v->fields.set_u8(0x108, value);
      } else {
        v->fields.set_u8(0x108, 0);
      }
      result = true; break;
    }
    break;
  case _respawn_shld_multiplier:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 5) {
        v->fields.set_u8(0x10c, value);
      } else {
        v->fields.set_u8(0x10c, 0);
      }
      result = true; break;
    }
    break;
  case _respawn_shld_recharge_rate:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 9) {
        v->fields.set_u8(0x109, value);
      } else {
        v->fields.set_u8(0x109, 0);
      }
      result = true; break;
    }
    break;
  case _respawn_headshot_immunity:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 2) {
        v->fields.set_u8(0x10b, value);
      } else {
        v->fields.set_u8(0x10b, 0);
      }
      result = true; break;
    }
    break;
  case _respawn_vampirism:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 5) {
        v->fields.set_u8(0x10a, value);
      } else {
        v->fields.set_u8(0x10a, 0);
      }
      result = true; break;
    }
    break;
  case _respawn_dmg_modifier_percentage:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 12) {
        v->fields.set_u8(0x114, value);
      } else {
        v->fields.set_u8(0x114, 0);
      }
      result = true; break;
    }
    break;
  case _respawn_initial_primary_weapon:
    if (parameter_type == _text_value_pair_parameter_string_id) {
      c_game_engine_variant *v = this->writeable();
      ((c_player_trait_weapons *)v->fields.addr(0x110))->set_initial_primary_weapon(value, true);
      result = true; break;
    }
    break;
  case _respawn_initial_secondary_weapon:
    if (parameter_type == _text_value_pair_parameter_string_id) {
      c_game_engine_variant *v = this->writeable();
      ((c_player_trait_weapons *)v->fields.addr(0x110))->set_initial_secondary_weapon(value, true);
      result = true; break;
    }
    break;
  case _respawn_initial_grenade_count:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 2) {
        v->fields.set_u16(0x110, value);
      } else {
        v->fields.set_u16(0x110, 0);
      }
      result = true; break;
    }
    break;
  case _respawn_recharging_grenades:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 2) {
        v->fields.set_u8(0x115, value);
      } else {
        v->fields.set_u8(0x115, 0);
      }
      result = true; break;
    }
    break;
  case _respawn_infinite_ammo:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 2) {
        v->fields.set_u8(0x116, value);
      } else {
        v->fields.set_u8(0x116, 0);
      }
      result = true; break;
    }
    break;
  case _respawn_weapon_pickup:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 2) {
        v->fields.set_u8(0x117, value);
      } else {
        v->fields.set_u8(0x117, 0);
      }
      result = true; break;
    }
    break;
  case _respawn_walking_speed:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 10) {
        v->fields.set_u8(0x118, value);
      } else {
        v->fields.set_u8(0x118, 0);
      }
      result = true; break;
    }
    break;
  case _respawn_personal_gravity:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 5) {
        v->fields.set_u8(0x119, value);
      } else {
        v->fields.set_u8(0x119, 0);
      }
      result = true; break;
    }
    break;
  case _respawn_vehicle_use:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 3) {
        v->fields.set_u8(0x11a, value);
      } else {
        v->fields.set_u8(0x11a, 0);
      }
      result = true; break;
    }
    break;
  case _respawn_motion_tracker:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 4) {
        v->fields.set_u16(0x120, value);
      } else {
        v->fields.set_u16(0x120, 0);
      }
      result = true; break;
    }
    break;
  case _respawn_motion_tracker_range:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 7) {
        v->fields.set_u16(0x122, value);
      } else {
        v->fields.set_u16(0x122, 0);
      }
      result = true; break;
    }
    break;
  case _respawn_active_camo:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 4) {
        v->fields.set_u8(0x11c, value);
      } else {
        v->fields.set_u8(0x11c, 0);
      }
      result = true; break;
    }
    break;
  case _respawn_waypoint:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 3) {
        v->fields.set_u8(0x11d, value);
      } else {
        v->fields.set_u8(0x11d, 0);
      }
      result = true; break;
    }
    break;
  case _respawn_visual:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 4) {
        v->fields.set_u8(0x11e, value);
      } else {
        v->fields.set_u8(0x11e, 0);
      }
      result = true; break;
    }
    break;
  case _respawn_forced_change_colors:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 13) {
        v->fields.set_u8(0x11f, value);
      } else {
        v->fields.set_u8(0x11f, 0);
      }
      result = true; break;
    }
    break;
  case _game_variant_social_observers:
    if (parameter_type == _text_value_pair_parameter_integer) {
      this->writeable();
      result = true; break;
    }
    break;
  case _game_variant_social_team_changing:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 2) {
        v->fields.set_u16(0x126, value);
      }
      result = true; break;
    }
    break;
  case _game_variant_social_friendly_fire:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      v->fields.word_flags(0x124)->set(0, value != 0);
      result = true; break;
    }
    break;
  case _game_variant_social_betrayal_booting:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      v->fields.word_flags(0x124)->set(1, value != 0);
      result = true; break;
    }
    break;
  case _game_variant_social_enemy_voice:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      v->fields.word_flags(0x124)->set(2, value != 0);
      result = true; break;
    }
    break;
  case _game_variant_social_open_channel_voice:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      v->fields.word_flags(0x124)->set(3, value != 0);
      result = true; break;
    }
    break;
  case _game_variant_social_dead_player_voice:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      v->fields.word_flags(0x124)->set(4, value != 0);
      result = true; break;
    }
    break;
  case _game_variant_map_override_grenades:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if (value != 0) {
        v->fields.or_u32(0x128, 0x1);
      } else {
        v->fields.and_u32(0x128, 0xfffffffe);
      }
      result = true; break;
    }
    break;
  case _game_variant_map_override_indestructible_vehicles:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if (value != 0) {
        v->fields.or_u32(0x128, 0x2);
      } else {
        v->fields.and_u32(0x128, 0xfffffffd);
      }
      result = true; break;
    }
    break;
  case _player_shld_dmg_resistance:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 12) {
        v->fields.set_u8(0x12c, value);
      } else {
        v->fields.set_u8(0x12c, 0);
      }
      result = true; break;
    }
    break;
  case _player_shld_multiplier:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 5) {
        v->fields.set_u8(0x130, value);
      } else {
        v->fields.set_u8(0x130, 0);
      }
      result = true; break;
    }
    break;
  case _player_shld_recharge_rate:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 9) {
        v->fields.set_u8(0x12d, value);
      } else {
        v->fields.set_u8(0x12d, 0);
      }
      result = true; break;
    }
    break;
  case _player_headshot_immunity:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 2) {
        v->fields.set_u8(0x12f, value);
      } else {
        v->fields.set_u8(0x12f, 0);
      }
      result = true; break;
    }
    break;
  case _player_vampirism:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 5) {
        v->fields.set_u8(0x12e, value);
      } else {
        v->fields.set_u8(0x12e, 0);
      }
      result = true; break;
    }
    break;
  case _player_dmg_modifier_percentage:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 12) {
        v->fields.set_u8(0x138, value);
      } else {
        v->fields.set_u8(0x138, 0);
      }
      result = true; break;
    }
    break;
  case _player_initial_primary_weapon:
    if (parameter_type == _text_value_pair_parameter_string_id) {
      c_game_engine_variant *v = this->writeable();
      ((c_player_trait_weapons *)v->fields.addr(0x134))->set_initial_primary_weapon(value, true);
      result = true; break;
    }
    break;
  case _player_initial_secondary_weapon:
    if (parameter_type == _text_value_pair_parameter_string_id) {
      c_game_engine_variant *v = this->writeable();
      ((c_player_trait_weapons *)v->fields.addr(0x134))->set_initial_secondary_weapon(value, true);
      result = true; break;
    }
    break;
  case _player_initial_grenade_count:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 2) {
        v->fields.set_u16(0x134, value);
      } else {
        v->fields.set_u16(0x134, 0);
      }
      result = true; break;
    }
    break;
  case _player_recharging_grenades:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 2) {
        v->fields.set_u8(0x139, value);
      } else {
        v->fields.set_u8(0x139, 0);
      }
      result = true; break;
    }
    break;
  case _player_infinite_ammo:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 2) {
        v->fields.set_u8(0x13a, value);
      } else {
        v->fields.set_u8(0x13a, 0);
      }
      result = true; break;
    }
    break;
  case _player_weapon_pickup:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 2) {
        v->fields.set_u8(0x13b, value);
      } else {
        v->fields.set_u8(0x13b, 0);
      }
      result = true; break;
    }
    break;
  case _player_walking_speed:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 10) {
        v->fields.set_u8(0x13c, value);
      } else {
        v->fields.set_u8(0x13c, 0);
      }
      result = true; break;
    }
    break;
  case _player_personal_gravity:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 5) {
        v->fields.set_u8(0x13d, value);
      } else {
        v->fields.set_u8(0x13d, 0);
      }
      result = true; break;
    }
    break;
  case _player_vehicle_use:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 3) {
        v->fields.set_u8(0x13e, value);
      } else {
        v->fields.set_u8(0x13e, 0);
      }
      result = true; break;
    }
    break;
  case _player_motion_tracker:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 4) {
        v->fields.set_u16(0x144, value);
      } else {
        v->fields.set_u16(0x144, 0);
      }
      result = true; break;
    }
    break;
  case _player_motion_tracker_range:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 7) {
        v->fields.set_u16(0x146, value);
      } else {
        v->fields.set_u16(0x146, 0);
      }
      result = true; break;
    }
    break;
  case _player_active_camo:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 4) {
        v->fields.set_u8(0x140, value);
      } else {
        v->fields.set_u8(0x140, 0);
      }
      result = true; break;
    }
    break;
  case _player_waypoint:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 3) {
        v->fields.set_u8(0x141, value);
      } else {
        v->fields.set_u8(0x141, 0);
      }
      result = true; break;
    }
    break;
  case _player_visual:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 4) {
        v->fields.set_u8(0x142, value);
      } else {
        v->fields.set_u8(0x142, 0);
      }
      result = true; break;
    }
    break;
  case _player_forced_change_colors:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 13) {
        v->fields.set_u8(0x143, value);
      } else {
        v->fields.set_u8(0x143, 0);
      }
      result = true; break;
    }
    break;
  case _game_variant_map_override_weapon_set:
    if (parameter_type == _text_value_pair_parameter_string_id) {
      c_game_engine_variant *v = this->writeable();
      (&v->map_override_options)
          ->set_weapon_set_absolute_index(multiplayer_universal_data_get_weapon_set_absolute_index_from_string_id(value));
      result = true; break;
    }
    break;
  case _game_variant_map_override_vehicle_set:
    if (parameter_type == _text_value_pair_parameter_string_id) {
      c_game_engine_variant *v = this->writeable();
      (&v->map_override_options)
          ->set_vehicle_set_absolute_index(multiplayer_universal_data_get_vehicle_set_absolute_index_from_string_id(value));
      result = true; break;
    }
    break;
  case _red_powerup_shld_dmg_resistance:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 12) {
        v->fields.set_u8(0x14c, value);
      } else {
        v->fields.set_u8(0x14c, 0);
      }
      result = true; break;
    }
    break;
  case _red_powerup_shld_multiplier:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 5) {
        v->fields.set_u8(0x150, value);
      } else {
        v->fields.set_u8(0x150, 0);
      }
      result = true; break;
    }
    break;
  case _red_powerup_shld_recharge_rate:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 9) {
        v->fields.set_u8(0x14d, value);
      } else {
        v->fields.set_u8(0x14d, 0);
      }
      result = true; break;
    }
    break;
  case _red_powerup_headshot_immunity:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 2) {
        v->fields.set_u8(0x14f, value);
      } else {
        v->fields.set_u8(0x14f, 0);
      }
      result = true; break;
    }
    break;
  case _red_powerup_vampirism:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 5) {
        v->fields.set_u8(0x14e, value);
      } else {
        v->fields.set_u8(0x14e, 0);
      }
      result = true; break;
    }
    break;
  case _red_powerup_dmg_modifier_percentage:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 12) {
        v->fields.set_u8(0x158, value);
      } else {
        v->fields.set_u8(0x158, 0);
      }
      result = true; break;
    }
    break;
  case _red_powerup_initial_primary_weapon:
    if (parameter_type == _text_value_pair_parameter_string_id) {
      c_game_engine_variant *v = this->writeable();
      ((c_player_trait_weapons *)v->fields.addr(0x154))->set_initial_primary_weapon(value, true);
      result = true; break;
    }
    break;
  case _red_powerup_initial_secondary_weapon:
    if (parameter_type == _text_value_pair_parameter_string_id) {
      c_game_engine_variant *v = this->writeable();
      ((c_player_trait_weapons *)v->fields.addr(0x154))->set_initial_secondary_weapon(value, true);
      result = true; break;
    }
    break;
  case _red_powerup_initial_grenade_count:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 2) {
        v->fields.set_u16(0x154, value);
      } else {
        v->fields.set_u16(0x154, 0);
      }
      result = true; break;
    }
    break;
  case _red_powerup_recharging_grenades:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 2) {
        v->fields.set_u8(0x159, value);
      } else {
        v->fields.set_u8(0x159, 0);
      }
      result = true; break;
    }
    break;
  case _red_powerup_infinite_ammo:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 2) {
        v->fields.set_u8(0x15a, value);
      } else {
        v->fields.set_u8(0x15a, 0);
      }
      result = true; break;
    }
    break;
  case _red_powerup_weapon_pickup:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 2) {
        v->fields.set_u8(0x15b, value);
      } else {
        v->fields.set_u8(0x15b, 0);
      }
      result = true; break;
    }
    break;
  case _red_powerup_walking_speed:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 10) {
        v->fields.set_u8(0x15c, value);
      } else {
        v->fields.set_u8(0x15c, 0);
      }
      result = true; break;
    }
    break;
  case _red_powerup_personal_gravity:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 5) {
        v->fields.set_u8(0x15d, value);
      } else {
        v->fields.set_u8(0x15d, 0);
      }
      result = true; break;
    }
    break;
  case _red_powerup_vehicle_use:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 3) {
        v->fields.set_u8(0x15e, value);
      } else {
        v->fields.set_u8(0x15e, 0);
      }
      result = true; break;
    }
    break;
  case _red_powerup_motion_tracker:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 4) {
        v->fields.set_u16(0x164, value);
      } else {
        v->fields.set_u16(0x164, 0);
      }
      result = true; break;
    }
    break;
  case _red_powerup_motion_tracker_range:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 7) {
        v->fields.set_u16(0x166, value);
      } else {
        v->fields.set_u16(0x166, 0);
      }
      result = true; break;
    }
    break;
  case _red_powerup_active_camo:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 4) {
        v->fields.set_u8(0x160, value);
      } else {
        v->fields.set_u8(0x160, 0);
      }
      result = true; break;
    }
    break;
  case _red_powerup_waypoint:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 3) {
        v->fields.set_u8(0x161, value);
      } else {
        v->fields.set_u8(0x161, 0);
      }
      result = true; break;
    }
    break;
  case _red_powerup_visual:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 4) {
        v->fields.set_u8(0x162, value);
      } else {
        v->fields.set_u8(0x162, 0);
      }
      result = true; break;
    }
    break;
  case _red_powerup_forced_change_colors:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 13) {
        v->fields.set_u8(0x163, value);
      } else {
        v->fields.set_u8(0x163, 0);
      }
      result = true; break;
    }
    break;
  case _blue_powerup_shld_dmg_resistance:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 12) {
        v->fields.set_u8(0x168, value);
      } else {
        v->fields.set_u8(0x168, 0);
      }
      result = true; break;
    }
    break;
  case _blue_powerup_shld_multiplier:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 5) {
        v->fields.set_u8(0x16c, value);
      } else {
        v->fields.set_u8(0x16c, 0);
      }
      result = true; break;
    }
    break;
  case _blue_powerup_shld_recharge_rate:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 9) {
        v->fields.set_u8(0x169, value);
      } else {
        v->fields.set_u8(0x169, 0);
      }
      result = true; break;
    }
    break;
  case _blue_powerup_headshot_immunity:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 2) {
        v->fields.set_u8(0x16b, value);
      } else {
        v->fields.set_u8(0x16b, 0);
      }
      result = true; break;
    }
    break;
  case _blue_powerup_vampirism:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 5) {
        v->fields.set_u8(0x16a, value);
      } else {
        v->fields.set_u8(0x16a, 0);
      }
      result = true; break;
    }
    break;
  case _blue_powerup_dmg_modifier_percentage:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 12) {
        v->fields.set_u8(0x174, value);
      } else {
        v->fields.set_u8(0x174, 0);
      }
      result = true; break;
    }
    break;
  case _blue_powerup_initial_primary_weapon:
    if (parameter_type == _text_value_pair_parameter_string_id) {
      c_game_engine_variant *v = this->writeable();
      ((c_player_trait_weapons *)v->fields.addr(0x170))->set_initial_primary_weapon(value, true);
      result = true; break;
    }
    break;
  case _blue_powerup_initial_secondary_weapon:
    if (parameter_type == _text_value_pair_parameter_string_id) {
      c_game_engine_variant *v = this->writeable();
      ((c_player_trait_weapons *)v->fields.addr(0x170))->set_initial_secondary_weapon(value, true);
      result = true; break;
    }
    break;
  case _blue_powerup_initial_grenade_count:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 2) {
        v->fields.set_u16(0x170, value);
      } else {
        v->fields.set_u16(0x170, 0);
      }
      result = true; break;
    }
    break;
  case _blue_powerup_recharging_grenades:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 2) {
        v->fields.set_u8(0x175, value);
      } else {
        v->fields.set_u8(0x175, 0);
      }
      result = true; break;
    }
    break;
  case _blue_powerup_infinite_ammo:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 2) {
        v->fields.set_u8(0x176, value);
      } else {
        v->fields.set_u8(0x176, 0);
      }
      result = true; break;
    }
    break;
  case _blue_powerup_weapon_pickup:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 2) {
        v->fields.set_u8(0x177, value);
      } else {
        v->fields.set_u8(0x177, 0);
      }
      result = true; break;
    }
    break;
  case _blue_powerup_walking_speed:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 10) {
        v->fields.set_u8(0x178, value);
      } else {
        v->fields.set_u8(0x178, 0);
      }
      result = true; break;
    }
    break;
  case _blue_powerup_personal_gravity:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 5) {
        v->fields.set_u8(0x179, value);
      } else {
        v->fields.set_u8(0x179, 0);
      }
      result = true; break;
    }
    break;
  case _blue_powerup_vehicle_use:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 3) {
        v->fields.set_u8(0x17a, value);
      } else {
        v->fields.set_u8(0x17a, 0);
      }
      result = true; break;
    }
    break;
  case _blue_powerup_motion_tracker:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 4) {
        v->fields.set_u16(0x180, value);
      } else {
        v->fields.set_u16(0x180, 0);
      }
      result = true; break;
    }
    break;
  case _blue_powerup_motion_tracker_range:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 7) {
        v->fields.set_u16(0x182, value);
      } else {
        v->fields.set_u16(0x182, 0);
      }
      result = true; break;
    }
    break;
  case _blue_powerup_active_camo:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 4) {
        v->fields.set_u8(0x17c, value);
      } else {
        v->fields.set_u8(0x17c, 0);
      }
      result = true; break;
    }
    break;
  case _blue_powerup_waypoint:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 3) {
        v->fields.set_u8(0x17d, value);
      } else {
        v->fields.set_u8(0x17d, 0);
      }
      result = true; break;
    }
    break;
  case _blue_powerup_visual:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 4) {
        v->fields.set_u8(0x17e, value);
      } else {
        v->fields.set_u8(0x17e, 0);
      }
      result = true; break;
    }
    break;
  case _blue_powerup_forced_change_colors:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 13) {
        v->fields.set_u8(0x17f, value);
      } else {
        v->fields.set_u8(0x17f, 0);
      }
      result = true; break;
    }
    break;
  case _yellow_powerup_shld_dmg_resistance:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 12) {
        v->fields.set_u8(0x184, value);
      } else {
        v->fields.set_u8(0x184, 0);
      }
      result = true; break;
    }
    break;
  case _yellow_powerup_shld_multiplier:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 5) {
        v->fields.set_u8(0x188, value);
      } else {
        v->fields.set_u8(0x188, 0);
      }
      result = true; break;
    }
    break;
  case _yellow_powerup_shld_recharge_rate:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 9) {
        v->fields.set_u8(0x185, value);
      } else {
        v->fields.set_u8(0x185, 0);
      }
      result = true; break;
    }
    break;
  case _yellow_powerup_headshot_immunity:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 2) {
        v->fields.set_u8(0x187, value);
      } else {
        v->fields.set_u8(0x187, 0);
      }
      result = true; break;
    }
    break;
  case _yellow_powerup_vampirism:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 5) {
        v->fields.set_u8(0x186, value);
      } else {
        v->fields.set_u8(0x186, 0);
      }
      result = true; break;
    }
    break;
  case _yellow_powerup_dmg_modifier_percentage:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 12) {
        v->fields.set_u8(0x190, value);
      } else {
        v->fields.set_u8(0x190, 0);
      }
      result = true; break;
    }
    break;
  case _yellow_powerup_initial_primary_weapon:
    if (parameter_type == _text_value_pair_parameter_string_id) {
      c_game_engine_variant *v = this->writeable();
      ((c_player_trait_weapons *)v->fields.addr(0x18c))->set_initial_primary_weapon(value, true);
      result = true; break;
    }
    break;
  case _yellow_powerup_initial_secondary_weapon:
    if (parameter_type == _text_value_pair_parameter_string_id) {
      c_game_engine_variant *v = this->writeable();
      ((c_player_trait_weapons *)v->fields.addr(0x18c))->set_initial_secondary_weapon(value, true);
      result = true; break;
    }
    break;
  case _yellow_powerup_initial_grenade_count:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 2) {
        v->fields.set_u16(0x18c, value);
      } else {
        v->fields.set_u16(0x18c, 0);
      }
      result = true; break;
    }
    break;
  case _yellow_powerup_recharging_grenades:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 2) {
        v->fields.set_u8(0x191, value);
      } else {
        v->fields.set_u8(0x191, 0);
      }
      result = true; break;
    }
    break;
  case _yellow_powerup_infinite_ammo:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 2) {
        v->fields.set_u8(0x192, value);
      } else {
        v->fields.set_u8(0x192, 0);
      }
      result = true; break;
    }
    break;
  case _yellow_powerup_weapon_pickup:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 2) {
        v->fields.set_u8(0x193, value);
      } else {
        v->fields.set_u8(0x193, 0);
      }
      result = true; break;
    }
    break;
  case _yellow_powerup_walking_speed:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 10) {
        v->fields.set_u8(0x194, value);
      } else {
        v->fields.set_u8(0x194, 0);
      }
      result = true; break;
    }
    break;
  case _yellow_powerup_personal_gravity:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 5) {
        v->fields.set_u8(0x195, value);
      } else {
        v->fields.set_u8(0x195, 0);
      }
      result = true; break;
    }
    break;
  case _yellow_powerup_vehicle_use:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 3) {
        v->fields.set_u8(0x196, value);
      } else {
        v->fields.set_u8(0x196, 0);
      }
      result = true; break;
    }
    break;
  case _yellow_powerup_motion_tracker:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 4) {
        v->fields.set_u16(0x19c, value);
      } else {
        v->fields.set_u16(0x19c, 0);
      }
      result = true; break;
    }
    break;
  case _yellow_powerup_motion_tracker_range:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 7) {
        v->fields.set_u16(0x19e, value);
      } else {
        v->fields.set_u16(0x19e, 0);
      }
      result = true; break;
    }
    break;
  case _yellow_powerup_active_camo:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 4) {
        v->fields.set_u8(0x198, value);
      } else {
        v->fields.set_u8(0x198, 0);
      }
      result = true; break;
    }
    break;
  case _yellow_powerup_waypoint:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 3) {
        v->fields.set_u8(0x199, value);
      } else {
        v->fields.set_u8(0x199, 0);
      }
      result = true; break;
    }
    break;
  case _yellow_powerup_visual:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 4) {
        v->fields.set_u8(0x19a, value);
      } else {
        v->fields.set_u8(0x19a, 0);
      }
      result = true; break;
    }
    break;
  case _yellow_powerup_forced_change_colors:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 13) {
        v->fields.set_u8(0x19b, value);
      } else {
        v->fields.set_u8(0x19b, 0);
      }
      result = true; break;
    }
    break;
  case _game_variant_map_override_red_powerup_traits_duration:
    if (parameter_type == _text_value_pair_parameter_integer) {
      unsigned char x = (unsigned char)value;
      c_game_engine_variant *v = this->writeable();
      if (x > 120 && x != 0) {
        x = 5;
      }
      v->fields.set_u8(0x1a0, x);
      result = true; break;
    }
    break;
  case _game_variant_map_override_blue_powerup_traits_duration:
    if (parameter_type == _text_value_pair_parameter_integer) {
      unsigned char x = (unsigned char)value;
      c_game_engine_variant *v = this->writeable();
      if (x > 120 && x != 0) {
        x = 30;
      }
      v->fields.set_u8(0x1a1, x);
      result = true; break;
    }
    break;
  case _game_variant_map_override_yellow_powerup_traits_duration:
    if (parameter_type == _text_value_pair_parameter_integer) {
      unsigned char x = (unsigned char)value;
      c_game_engine_variant *v = this->writeable();
      if (x > 120 && x != 0) {
        x = 30;
      }
      v->fields.set_u8(0x1a2, x);
      result = true; break;
    }
    break;
  case _slayer_team_scoring:
  case _oddball_team_scoring:
  case _king_team_scoring:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 3) {
        v->fields.set_u16(0x1a6, value);
      } else {
        v->fields.set_u16(0x1a6, 0);
      }
      result = true; break;
    }
    break;
  case _slayer_score_to_win:
    if (parameter_type == _text_value_pair_parameter_integer) {
      short x = (short)value;
      c_game_engine_variant *v = this->writeable();
      if (x < -1 || x > 250) {
        x = 25;
      }
      v->fields.set_s16(0x1ac, x);
      result = true; break;
    }
    break;
  case _slayer_kill_points:
    if (parameter_type == _text_value_pair_parameter_integer) {
      short x = (signed char)value;
      c_game_engine_variant *v = this->writeable();
      if (x < -10 || x > 10) {
        x = 1;
      }
      v->fields.set_s16(0x1ae, x);
      result = true; break;
    }
    break;
  case _slayer_assist_points:
    if (parameter_type == _text_value_pair_parameter_integer) {
      signed char x = (signed char)value;
      c_game_engine_variant *v = this->writeable();
      if (x >= -10 && x <= 10) {
        v->fields.set_s8(0x1b0, x);
      } else {
        v->fields.set_s8(0x1b0, 0);
      }
      result = true; break;
    }
    break;
  case _slayer_death_points:
    if (parameter_type == _text_value_pair_parameter_integer) {
      signed char x = (signed char)value;
      c_game_engine_variant *v = this->writeable();
      if (x >= -10 && x <= 10) {
        v->fields.set_s8(0x1b1, x);
      } else {
        v->fields.set_s8(0x1b1, 0);
      }
      result = true; break;
    }
    break;
  case _slayer_suicide_points:
    if (parameter_type == _text_value_pair_parameter_integer) {
      signed char x = (signed char)value;
      c_game_engine_variant *v = this->writeable();
      if (x < -10 || x > 10) {
        x = -1;
      }
      v->fields.set_s8(0x1b2, x);
      result = true; break;
    }
    break;
  case _slayer_betrayal_points:
    if (parameter_type == _text_value_pair_parameter_integer) {
      signed char x = (signed char)value;
      c_game_engine_variant *v = this->writeable();
      if (x < -10 || x > 10) {
        x = -1;
      }
      v->fields.set_s8(0x1b3, x);
      result = true; break;
    }
    break;
  case _slayer_leader_killed_points:
    if (parameter_type == _text_value_pair_parameter_integer) {
      signed char x = (signed char)value;
      c_game_engine_variant *v = this->writeable();
      if (x >= -10 && x <= 10) {
        v->fields.set_s8(0x1b4, x);
      } else {
        v->fields.set_s8(0x1b4, 0);
      }
      result = true; break;
    }
    break;
  case _slayer_elimination_points:
    if (parameter_type == _text_value_pair_parameter_integer) {
      signed char x = (signed char)value;
      c_game_engine_variant *v = this->writeable();
      if (x >= -10 && x <= 10) {
        v->fields.set_s8(0x1b5, x);
      } else {
        v->fields.set_s8(0x1b5, 0);
      }
      result = true; break;
    }
    break;
  case _slayer_assassination_points:
    if (parameter_type == _text_value_pair_parameter_integer) {
      signed char x = (signed char)value;
      c_game_engine_variant *v = this->writeable();
      if (x >= -10 && x <= 10) {
        v->fields.set_s8(0x1b6, x);
      } else {
        v->fields.set_s8(0x1b6, 0);
      }
      result = true; break;
    }
    break;
  case _slayer_headshot_points:
    if (parameter_type == _text_value_pair_parameter_integer) {
      signed char x = (signed char)value;
      c_game_engine_variant *v = this->writeable();
      if (x >= -10 && x <= 10) {
        v->fields.set_s8(0x1b7, x);
      } else {
        v->fields.set_s8(0x1b7, 0);
      }
      result = true; break;
    }
    break;
  case _slayer_melee_points:
    if (parameter_type == _text_value_pair_parameter_integer) {
      signed char x = (signed char)value;
      c_game_engine_variant *v = this->writeable();
      if (x >= -10 && x <= 10) {
        v->fields.set_s8(0x1b8, x);
      } else {
        v->fields.set_s8(0x1b8, 0);
      }
      result = true; break;
    }
    break;
  case _slayer_sticky_points:
    if (parameter_type == _text_value_pair_parameter_integer) {
      signed char x = (signed char)value;
      c_game_engine_variant *v = this->writeable();
      if (x >= -10 && x <= 10) {
        v->fields.set_s8(0x1b9, x);
      } else {
        v->fields.set_s8(0x1b9, 0);
      }
      result = true; break;
    }
    break;
  case _slayer_splatter_points:
    if (parameter_type == _text_value_pair_parameter_integer) {
      signed char x = (signed char)value;
      c_game_engine_variant *v = this->writeable();
      if (x >= -10 && x <= 10) {
        v->fields.set_s8(0x1ba, x);
      } else {
        v->fields.set_s8(0x1ba, 0);
      }
      result = true; break;
    }
    break;
  case _slayer_killing_spree_points:
    if (parameter_type == _text_value_pair_parameter_integer) {
      signed char x = (signed char)value;
      c_game_engine_variant *v = this->writeable();
      if (x >= -10 && x <= 10) {
        v->fields.set_s8(0x1bb, x);
      } else {
        v->fields.set_s8(0x1bb, 0);
      }
      result = true; break;
    }
    break;
  case _slayer_leader_shld_dmg_resistance:
  case _ball_carrier_shld_dmg_resistance:
  case _juggernaut_shld_dmg_resistance:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 12) {
        v->fields.set_u8(0x1bc, value);
      } else {
        v->fields.set_u8(0x1bc, 0);
      }
      result = true; break;
    }
    break;
  case _slayer_leader_shld_multiplier:
  case _ball_carrier_shld_multiplier:
  case _juggernaut_shld_multiplier:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 5) {
        v->fields.set_u8(0x1c0, value);
      } else {
        v->fields.set_u8(0x1c0, 0);
      }
      result = true; break;
    }
    break;
  case _slayer_leader_shld_recharge_rate:
  case _ball_carrier_shld_recharge_rate:
  case _juggernaut_shld_recharge_rate:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 9) {
        v->fields.set_u8(0x1bd, value);
      } else {
        v->fields.set_u8(0x1bd, 0);
      }
      result = true; break;
    }
    break;
  case _slayer_leader_headshot_immunity:
  case _ball_carrier_headshot_immunity:
  case _juggernaut_headshot_immunity:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 2) {
        v->fields.set_u8(0x1bf, value);
      } else {
        v->fields.set_u8(0x1bf, 0);
      }
      result = true; break;
    }
    break;
  case _slayer_leader_vampirism:
  case _ball_carrier_vampirism:
  case _juggernaut_vampirism:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 5) {
        v->fields.set_u8(0x1be, value);
      } else {
        v->fields.set_u8(0x1be, 0);
      }
      result = true; break;
    }
    break;
  case _slayer_leader_dmg_modifier_percentage:
  case _ball_carrier_dmg_modifier_percentage:
  case _juggernaut_dmg_modifier_percentage:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 12) {
        v->fields.set_u8(0x1c8, value);
      } else {
        v->fields.set_u8(0x1c8, 0);
      }
      result = true; break;
    }
    break;
  case _slayer_leader_initial_primary_weapon:
  case _ball_carrier_initial_primary_weapon:
  case _juggernaut_initial_primary_weapon:
    if (parameter_type == _text_value_pair_parameter_string_id) {
      c_game_engine_variant *v = this->writeable();
      ((c_player_trait_weapons *)v->fields.addr(0x1c4))->set_initial_primary_weapon(value, true);
      result = true; break;
    }
    break;
  case _slayer_leader_initial_secondary_weapon:
  case _ball_carrier_initial_secondary_weapon:
  case _juggernaut_initial_secondary_weapon:
    if (parameter_type == _text_value_pair_parameter_string_id) {
      c_game_engine_variant *v = this->writeable();
      ((c_player_trait_weapons *)v->fields.addr(0x1c4))->set_initial_secondary_weapon(value, true);
      result = true; break;
    }
    break;
  case _slayer_leader_initial_grenade_count:
  case _ball_carrier_initial_grenade_count:
  case _juggernaut_initial_grenade_count:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 2) {
        v->fields.set_u16(0x1c4, value);
      } else {
        v->fields.set_u16(0x1c4, 0);
      }
      result = true; break;
    }
    break;
  case _slayer_leader_recharging_grenades:
  case _ball_carrier_recharging_grenades:
  case _juggernaut_recharging_grenades:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 2) {
        v->fields.set_u8(0x1c9, value);
      } else {
        v->fields.set_u8(0x1c9, 0);
      }
      result = true; break;
    }
    break;
  case _slayer_leader_infinite_ammo:
  case _ball_carrier_infinite_ammo:
  case _juggernaut_infinite_ammo:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 2) {
        v->fields.set_u8(0x1ca, value);
      } else {
        v->fields.set_u8(0x1ca, 0);
      }
      result = true; break;
    }
    break;
  case _slayer_leader_weapon_pickup:
  case _ball_carrier_weapon_pickup:
  case _juggernaut_weapon_pickup:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 2) {
        v->fields.set_u8(0x1cb, value);
      } else {
        v->fields.set_u8(0x1cb, 0);
      }
      result = true; break;
    }
    break;
  case _slayer_leader_walking_speed:
  case _ball_carrier_walking_speed:
  case _juggernaut_walking_speed:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 10) {
        v->fields.set_u8(0x1cc, value);
      } else {
        v->fields.set_u8(0x1cc, 0);
      }
      result = true; break;
    }
    break;
  case _slayer_leader_personal_gravity:
  case _ball_carrier_personal_gravity:
  case _juggernaut_personal_gravity:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 5) {
        v->fields.set_u8(0x1cd, value);
      } else {
        v->fields.set_u8(0x1cd, 0);
      }
      result = true; break;
    }
    break;
  case _slayer_leader_vehicle_use:
  case _ball_carrier_vehicle_use:
  case _juggernaut_vehicle_use:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 3) {
        v->fields.set_u8(0x1ce, value);
      } else {
        v->fields.set_u8(0x1ce, 0);
      }
      result = true; break;
    }
    break;
  case _slayer_leader_motion_tracker:
  case _ball_carrier_motion_tracker:
  case _juggernaut_motion_tracker:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 4) {
        v->fields.set_u16(0x1d4, value);
      } else {
        v->fields.set_u16(0x1d4, 0);
      }
      result = true; break;
    }
    break;
  case _slayer_leader_motion_tracker_range:
  case _ball_carrier_motion_tracker_range:
  case _juggernaut_motion_tracker_range:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 7) {
        v->fields.set_u16(0x1d6, value);
      } else {
        v->fields.set_u16(0x1d6, 0);
      }
      result = true; break;
    }
    break;
  case _slayer_leader_active_camo:
  case _ball_carrier_active_camo:
  case _juggernaut_active_camo:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 4) {
        v->fields.set_u8(0x1d0, value);
      } else {
        v->fields.set_u8(0x1d0, 0);
      }
      result = true; break;
    }
    break;
  case _slayer_leader_waypoint:
  case _ball_carrier_waypoint:
  case _juggernaut_waypoint:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 3) {
        v->fields.set_u8(0x1d1, value);
      } else {
        v->fields.set_u8(0x1d1, 0);
      }
      result = true; break;
    }
    break;
  case _slayer_leader_visual:
  case _ball_carrier_visual:
  case _juggernaut_visual:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 4) {
        v->fields.set_u8(0x1d2, value);
      } else {
        v->fields.set_u8(0x1d2, 0);
      }
      result = true; break;
    }
    break;
  case _slayer_leader_forced_change_colors:
  case _ball_carrier_forced_change_colors:
  case _juggernaut_forced_change_colors:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 13) {
        v->fields.set_u8(0x1d3, value);
      } else {
        v->fields.set_u8(0x1d3, 0);
      }
      result = true; break;
    }
    break;
  case _oddball_auto_ball_pickup:
  case _game_variant_king_opaque_hill:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if (value != 0) {
        v->fields.or_u32(0x1ac, 0x1);
      } else {
        v->fields.and_u32(0x1ac, 0xfffffffe);
      }
      result = true; break;
    }
    break;
  case _oddball_ball_effect:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if (value != 0) {
        v->fields.or_u32(0x1ac, 0x2);
      } else {
        v->fields.and_u32(0x1ac, 0xfffffffd);
      }
      result = true; break;
    }
    break;
  case _oddball_score_to_win:
    if (parameter_type == _text_value_pair_parameter_integer) {
      short x = (short)value;
      c_game_engine_variant *v = this->writeable();
      if (x < -1 || x > 1000) {
        x = 200;
      }
      v->fields.set_s16(0x1b0, x);
      result = true; break;
    }
    break;
  case _oddball_carrying_points:
    if (parameter_type == _text_value_pair_parameter_integer) {
      short x = (signed char)value;
      c_game_engine_variant *v = this->writeable();
      if (x < -10 || x > 10) {
        x = 1;
      }
      v->fields.set_s16(0x1b2, x);
      result = true; break;
    }
    break;
  case _oddball_kill_points:
  case _infection_safe_haven_arrival_points:
    if (parameter_type == _text_value_pair_parameter_integer) {
      signed char x = (signed char)value;
      c_game_engine_variant *v = this->writeable();
      if (x >= -10 && x <= 10) {
        v->fields.set_s8(0x1b4, x);
      } else {
        v->fields.set_s8(0x1b4, 0);
      }
      result = true; break;
    }
    break;
  case _oddball_ball_kill_points:
    if (parameter_type == _text_value_pair_parameter_integer) {
      signed char x = (signed char)value;
      c_game_engine_variant *v = this->writeable();
      if (x >= -10 && x <= 10) {
        v->fields.set_s8(0x1b5, x);
      } else {
        v->fields.set_s8(0x1b5, 0);
      }
      result = true; break;
    }
    break;
  case _oddball_carrier_kill_points:
    if (parameter_type == _text_value_pair_parameter_integer) {
      signed char x = (signed char)value;
      c_game_engine_variant *v = this->writeable();
      if (x >= -10 && x <= 10) {
        v->fields.set_s8(0x1b6, x);
      } else {
        v->fields.set_s8(0x1b6, 0);
      }
      result = true; break;
    }
    break;
  case _oddball_ball_count:
    if (parameter_type == _text_value_pair_parameter_integer) {
      signed char x = (signed char)value;
      c_game_engine_variant *v = this->writeable();
      if (x < 1 || x > 3) {
        x = 1;
      }
      v->fields.set_s8(0x1b7, x);
      result = true; break;
    }
    break;
  case _oddball_ball_spawn_delay:
    if (parameter_type == _text_value_pair_parameter_integer) {
      short x = (short)value;
      c_game_engine_variant *v = this->writeable();
      if (x < 0 || x > 120) {
        x = 5;
      }
      v->fields.set_s16(0x1b8, x);
      result = true; break;
    }
    break;
  case _oddball_inactive_respawn_delay:
    if (parameter_type == _text_value_pair_parameter_integer) {
      short x = (short)value;
      c_game_engine_variant *v = this->writeable();
      if (x < 0 || x > 120) {
        x = 30;
      }
      v->fields.set_s16(0x1ba, x);
      result = true; break;
    }
    break;
  case _ctf_touch_return_timeout:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      v->fields.set_u16(0x1b0, value);
      result = true; break;
    }
    break;
  case _ctf_flag_at_home_to_score:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      v->fields.char_flags(0x1ac)->set(0, value != 0);
      result = true; break;
    }
    break;
  case _ctf_home_flag_waypoint:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 3) {
        v->fields.set_u8(0x1ad, value);
      } else {
        v->fields.set_u8(0x1ad, 1);
      }
      result = true; break;
    }
    break;
  case _ctf_game_type:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 2) {
        v->fields.set_u8(0x1ae, value);
      } else {
        v->fields.set_u8(0x1ae, 0);
      }
      result = true; break;
    }
    break;
  case _ctf_respawn:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 3) {
        v->fields.set_u8(0x1af, value);
      } else {
        v->fields.set_u8(0x1af, 0);
      }
      result = true; break;
    }
    break;
  case _ctf_score_to_win:
    if (parameter_type == _text_value_pair_parameter_integer) {
      short x = (short)value;
      c_game_engine_variant *v = this->writeable();
      if (x < 0 || x > 50) {
        x = 5;
      }
      v->fields.set_s16(0x1b4, x);
      result = true; break;
    }
    break;
  case _ctf_sudden_death_time:
    if (parameter_type == _text_value_pair_parameter_integer) {
      short x = (short)value;
      c_game_engine_variant *v = this->writeable();
      if (x >= -1 && x <= 300) {
        v->fields.set_s16(0x1b2, x);
      } else {
        v->fields.set_s16(0x1b2, 0);
      }
      result = true; break;
    }
    break;
  case _ctf_flag_reset_time:
    if (parameter_type == _text_value_pair_parameter_integer) {
      short x = (short)value;
      c_game_engine_variant *v = this->writeable();
      if (x < 0 || x > 300) {
        x = 30;
      }
      v->fields.set_s16(0x1b6, x);
      result = true; break;
    }
    break;
  case _flag_carrier_shld_dmg_resistance:
  case _zombie_shld_dmg_resistance:
  case _on_hill_shld_dmg_resistance:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 12) {
        v->fields.set_u8(0x1b8, value);
      } else {
        v->fields.set_u8(0x1b8, 0);
      }
      result = true; break;
    }
    break;
  case _flag_carrier_shld_multiplier:
  case _zombie_shld_multiplier:
  case _on_hill_shld_multiplier:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 5) {
        v->fields.set_u8(0x1bc, value);
      } else {
        v->fields.set_u8(0x1bc, 0);
      }
      result = true; break;
    }
    break;
  case _flag_carrier_shld_recharge_rate:
  case _zombie_shld_recharge_rate:
  case _on_hill_shld_recharge_rate:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 9) {
        v->fields.set_u8(0x1b9, value);
      } else {
        v->fields.set_u8(0x1b9, 0);
      }
      result = true; break;
    }
    break;
  case _flag_carrier_headshot_immunity:
  case _zombie_headshot_immunity:
  case _on_hill_headshot_immunity:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 2) {
        v->fields.set_u8(0x1bb, value);
      } else {
        v->fields.set_u8(0x1bb, 0);
      }
      result = true; break;
    }
    break;
  case _flag_carrier_vampirism:
  case _zombie_vampirism:
  case _on_hill_vampirism:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 5) {
        v->fields.set_u8(0x1ba, value);
      } else {
        v->fields.set_u8(0x1ba, 0);
      }
      result = true; break;
    }
    break;
  case _flag_carrier_dmg_modifier_percentage:
  case _zombie_dmg_modifier_percentage:
  case _on_hill_dmg_modifier_percentage:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 12) {
        v->fields.set_u8(0x1c4, value);
      } else {
        v->fields.set_u8(0x1c4, 0);
      }
      result = true; break;
    }
    break;
  case _flag_carrier_initial_primary_weapon:
  case _zombie_initial_primary_weapon:
  case _on_hill_initial_primary_weapon:
    if (parameter_type == _text_value_pair_parameter_string_id) {
      c_game_engine_variant *v = this->writeable();
      ((c_player_trait_weapons *)v->fields.addr(0x1c0))->set_initial_primary_weapon(value, true);
      result = true; break;
    }
    break;
  case _flag_carrier_initial_secondary_weapon:
  case _zombie_initial_secondary_weapon:
  case _on_hill_initial_secondary_weapon:
    if (parameter_type == _text_value_pair_parameter_string_id) {
      c_game_engine_variant *v = this->writeable();
      ((c_player_trait_weapons *)v->fields.addr(0x1c0))->set_initial_secondary_weapon(value, true);
      result = true; break;
    }
    break;
  case _flag_carrier_initial_grenade_count:
  case _zombie_initial_grenade_count:
  case _on_hill_initial_grenade_count:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 2) {
        v->fields.set_u16(0x1c0, value);
      } else {
        v->fields.set_u16(0x1c0, 0);
      }
      result = true; break;
    }
    break;
  case _flag_carrier_recharging_grenades:
  case _zombie_recharging_grenades:
  case _on_hill_recharging_grenades:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 2) {
        v->fields.set_u8(0x1c5, value);
      } else {
        v->fields.set_u8(0x1c5, 0);
      }
      result = true; break;
    }
    break;
  case _flag_carrier_infinite_ammo:
  case _zombie_infinite_ammo:
  case _on_hill_infinite_ammo:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 2) {
        v->fields.set_u8(0x1c6, value);
      } else {
        v->fields.set_u8(0x1c6, 0);
      }
      result = true; break;
    }
    break;
  case _flag_carrier_weapon_pickup:
  case _zombie_weapon_pickup:
  case _on_hill_weapon_pickup:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 2) {
        v->fields.set_u8(0x1c7, value);
      } else {
        v->fields.set_u8(0x1c7, 0);
      }
      result = true; break;
    }
    break;
  case _flag_carrier_walking_speed:
  case _zombie_walking_speed:
  case _on_hill_walking_speed:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 10) {
        v->fields.set_u8(0x1c8, value);
      } else {
        v->fields.set_u8(0x1c8, 0);
      }
      result = true; break;
    }
    break;
  case _flag_carrier_personal_gravity:
  case _zombie_personal_gravity:
  case _on_hill_personal_gravity:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 5) {
        v->fields.set_u8(0x1c9, value);
      } else {
        v->fields.set_u8(0x1c9, 0);
      }
      result = true; break;
    }
    break;
  case _flag_carrier_vehicle_use:
  case _zombie_vehicle_use:
  case _on_hill_vehicle_use:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 3) {
        v->fields.set_u8(0x1ca, value);
      } else {
        v->fields.set_u8(0x1ca, 0);
      }
      result = true; break;
    }
    break;
  case _flag_carrier_motion_tracker:
  case _zombie_motion_tracker:
  case _on_hill_motion_tracker:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 4) {
        v->fields.set_u16(0x1d0, value);
      } else {
        v->fields.set_u16(0x1d0, 0);
      }
      result = true; break;
    }
    break;
  case _flag_carrier_motion_tracker_range:
  case _zombie_motion_tracker_range:
  case _on_hill_motion_tracker_range:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 7) {
        v->fields.set_u16(0x1d2, value);
      } else {
        v->fields.set_u16(0x1d2, 0);
      }
      result = true; break;
    }
    break;
  case _flag_carrier_active_camo:
  case _zombie_active_camo:
  case _on_hill_active_camo:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 4) {
        v->fields.set_u8(0x1cc, value);
      } else {
        v->fields.set_u8(0x1cc, 0);
      }
      result = true; break;
    }
    break;
  case _flag_carrier_waypoint:
  case _zombie_waypoint:
  case _on_hill_waypoint:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 3) {
        v->fields.set_u8(0x1cd, value);
      } else {
        v->fields.set_u8(0x1cd, 0);
      }
      result = true; break;
    }
    break;
  case _flag_carrier_visual:
  case _zombie_visual:
  case _on_hill_visual:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 4) {
        v->fields.set_u8(0x1ce, value);
      } else {
        v->fields.set_u8(0x1ce, 0);
      }
      result = true; break;
    }
    break;
  case _flag_carrier_forced_change_colors:
  case _zombie_forced_change_colors:
  case _on_hill_forced_change_colors:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 13) {
        v->fields.set_u8(0x1cf, value);
      } else {
        v->fields.set_u8(0x1cf, 0);
      }
      result = true; break;
    }
    break;
  case _infection_respawn_on_haven_move:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if (value != 0) {
        v->fields.or_u8(0x1ac, 0x1);
      } else {
        v->fields.and_u8(0x1ac, 0xfe);
      }
      result = true; break;
    }
    break;
  case _infection_safe_haven_setting:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value > 2) {
        v->fields.set_u8(0x1ad, 0);
      } else {
        v->fields.set_u8(0x1ad, value);
      }
      result = true; break;
    }
    break;
  case _infection_next_zombie:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 3) {
        v->fields.set_u8(0x1ae, value);
      } else {
        v->fields.set_u8(0x1ae, 0);
      }
      result = true; break;
    }
    break;
  case _infection_initial_zombie_count:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value > 16) {
        v->fields.set_u8(0x1af, 2);
      } else {
        v->fields.set_u8(0x1af, value);
      }
      result = true; break;
    }
    break;
  case _infection_safe_haven_movement_time:
    if (parameter_type == _text_value_pair_parameter_integer) {
      short x = (short)value;
      c_game_engine_variant *v = this->writeable();
      if (x < 0 || x > 120) {
        x = 30;
      }
      v->fields.set_s16(0x1b0, x);
      result = true; break;
    }
    break;
  case _infection_zombie_kill_points:
    if (parameter_type == _text_value_pair_parameter_integer) {
      signed char x = (signed char)value;
      c_game_engine_variant *v = this->writeable();
      if (x < -10 || x > 10) {
        x = 1;
      }
      v->fields.set_s8(0x1b2, x);
      result = true; break;
    }
    break;
  case _infection_points:
    if (parameter_type == _text_value_pair_parameter_integer) {
      signed char x = (signed char)value;
      c_game_engine_variant *v = this->writeable();
      if (x >= -10 && x <= 10) {
        v->fields.set_s8(0x1b3, x);
      } else {
        v->fields.set_s8(0x1b3, 0);
      }
      result = true; break;
    }
    break;
  case _infection_suicide_points:
    if (parameter_type == _text_value_pair_parameter_integer) {
      signed char x = (signed char)value;
      c_game_engine_variant *v = this->writeable();
      if (x < -10 || x > 10) {
        x = -1;
      }
      v->fields.set_s8(0x1b5, x);
      result = true; break;
    }
    break;
  case _infection_betrayal_points:
    if (parameter_type == _text_value_pair_parameter_integer) {
      signed char x = (signed char)value;
      c_game_engine_variant *v = this->writeable();
      if (x < -10 || x > 10) {
        x = -1;
      }
      v->fields.set_s8(0x1b6, x);
      result = true; break;
    }
    break;
  case _infection_last_man_standing_bonus_points:
  case _king_kill_points:
    if (parameter_type == _text_value_pair_parameter_integer) {
      signed char x = (signed char)value;
      c_game_engine_variant *v = this->writeable();
      if (x >= -10 && x <= 10) {
        v->fields.set_s8(0x1b7, x);
      } else {
        v->fields.set_s8(0x1b7, 0);
      }
      result = true; break;
    }
    break;
  case _first_zombie_shld_dmg_resistance:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 12) {
        v->fields.set_u8(0x1d4, value);
      } else {
        v->fields.set_u8(0x1d4, 0);
      }
      result = true; break;
    }
    break;
  case _first_zombie_shld_multiplier:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 5) {
        v->fields.set_u8(0x1d8, value);
      } else {
        v->fields.set_u8(0x1d8, 0);
      }
      result = true; break;
    }
    break;
  case _first_zombie_shld_recharge_rate:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 9) {
        v->fields.set_u8(0x1d5, value);
      } else {
        v->fields.set_u8(0x1d5, 0);
      }
      result = true; break;
    }
    break;
  case _first_zombie_headshot_immunity:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 2) {
        v->fields.set_u8(0x1d7, value);
      } else {
        v->fields.set_u8(0x1d7, 0);
      }
      result = true; break;
    }
    break;
  case _first_zombie_vampirism:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 5) {
        v->fields.set_u8(0x1d6, value);
      } else {
        v->fields.set_u8(0x1d6, 0);
      }
      result = true; break;
    }
    break;
  case _first_zombie_dmg_modifier_percentage:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 12) {
        v->fields.set_u8(0x1e0, value);
      } else {
        v->fields.set_u8(0x1e0, 0);
      }
      result = true; break;
    }
    break;
  case _first_zombie_initial_primary_weapon:
    if (parameter_type == _text_value_pair_parameter_string_id) {
      c_game_engine_variant *v = this->writeable();
      ((c_player_trait_weapons *)v->fields.addr(0x1dc))->set_initial_primary_weapon(value, true);
      result = true; break;
    }
    break;
  case _first_zombie_initial_secondary_weapon:
    if (parameter_type == _text_value_pair_parameter_string_id) {
      c_game_engine_variant *v = this->writeable();
      ((c_player_trait_weapons *)v->fields.addr(0x1dc))->set_initial_secondary_weapon(value, true);
      result = true; break;
    }
    break;
  case _first_zombie_initial_grenade_count:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 2) {
        v->fields.set_u16(0x1dc, value);
      } else {
        v->fields.set_u16(0x1dc, 0);
      }
      result = true; break;
    }
    break;
  case _first_zombie_recharging_grenades:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 2) {
        v->fields.set_u8(0x1e1, value);
      } else {
        v->fields.set_u8(0x1e1, 0);
      }
      result = true; break;
    }
    break;
  case _first_zombie_infinite_ammo:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 2) {
        v->fields.set_u8(0x1e2, value);
      } else {
        v->fields.set_u8(0x1e2, 0);
      }
      result = true; break;
    }
    break;
  case _first_zombie_weapon_pickup:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 2) {
        v->fields.set_u8(0x1e3, value);
      } else {
        v->fields.set_u8(0x1e3, 0);
      }
      result = true; break;
    }
    break;
  case _first_zombie_walking_speed:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 10) {
        v->fields.set_u8(0x1e4, value);
      } else {
        v->fields.set_u8(0x1e4, 0);
      }
      result = true; break;
    }
    break;
  case _first_zombie_personal_gravity:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 5) {
        v->fields.set_u8(0x1e5, value);
      } else {
        v->fields.set_u8(0x1e5, 0);
      }
      result = true; break;
    }
    break;
  case _first_zombie_vehicle_use:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 3) {
        v->fields.set_u8(0x1e6, value);
      } else {
        v->fields.set_u8(0x1e6, 0);
      }
      result = true; break;
    }
    break;
  case _first_zombie_motion_tracker:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 4) {
        v->fields.set_u16(0x1ec, value);
      } else {
        v->fields.set_u16(0x1ec, 0);
      }
      result = true; break;
    }
    break;
  case _first_zombie_motion_tracker_range:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 7) {
        v->fields.set_u16(0x1ee, value);
      } else {
        v->fields.set_u16(0x1ee, 0);
      }
      result = true; break;
    }
    break;
  case _first_zombie_active_camo:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 4) {
        v->fields.set_u8(0x1e8, value);
      } else {
        v->fields.set_u8(0x1e8, 0);
      }
      result = true; break;
    }
    break;
  case _first_zombie_waypoint:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 3) {
        v->fields.set_u8(0x1e9, value);
      } else {
        v->fields.set_u8(0x1e9, 0);
      }
      result = true; break;
    }
    break;
  case _first_zombie_visual:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 4) {
        v->fields.set_u8(0x1ea, value);
      } else {
        v->fields.set_u8(0x1ea, 0);
      }
      result = true; break;
    }
    break;
  case _first_zombie_forced_change_colors:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 13) {
        v->fields.set_u8(0x1eb, value);
      } else {
        v->fields.set_u8(0x1eb, 0);
      }
      result = true; break;
    }
    break;
  case _haven_defender_shld_dmg_resistance:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 12) {
        v->fields.set_u8(0x1f0, value);
      } else {
        v->fields.set_u8(0x1f0, 0);
      }
      result = true; break;
    }
    break;
  case _haven_defender_shld_multiplier:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 5) {
        v->fields.set_u8(0x1f4, value);
      } else {
        v->fields.set_u8(0x1f4, 0);
      }
      result = true; break;
    }
    break;
  case _haven_defender_shld_recharge_rate:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 9) {
        v->fields.set_u8(0x1f1, value);
      } else {
        v->fields.set_u8(0x1f1, 0);
      }
      result = true; break;
    }
    break;
  case _haven_defender_headshot_immunity:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 2) {
        v->fields.set_u8(0x1f3, value);
      } else {
        v->fields.set_u8(0x1f3, 0);
      }
      result = true; break;
    }
    break;
  case _haven_defender_vampirism:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 5) {
        v->fields.set_u8(0x1f2, value);
      } else {
        v->fields.set_u8(0x1f2, 0);
      }
      result = true; break;
    }
    break;
  case _haven_defender_dmg_modifier_percentage:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 12) {
        v->fields.set_u8(0x1fc, value);
      } else {
        v->fields.set_u8(0x1fc, 0);
      }
      result = true; break;
    }
    break;
  case _haven_defender_initial_primary_weapon:
    if (parameter_type == _text_value_pair_parameter_string_id) {
      c_game_engine_variant *v = this->writeable();
      ((c_player_trait_weapons *)v->fields.addr(0x1f8))->set_initial_primary_weapon(value, true);
      result = true; break;
    }
    break;
  case _haven_defender_initial_secondary_weapon:
    if (parameter_type == _text_value_pair_parameter_string_id) {
      c_game_engine_variant *v = this->writeable();
      ((c_player_trait_weapons *)v->fields.addr(0x1f8))->set_initial_secondary_weapon(value, true);
      result = true; break;
    }
    break;
  case _haven_defender_initial_grenade_count:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 2) {
        v->fields.set_u16(0x1f8, value);
      } else {
        v->fields.set_u16(0x1f8, 0);
      }
      result = true; break;
    }
    break;
  case _haven_defender_recharging_grenades:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 2) {
        v->fields.set_u8(0x1fd, value);
      } else {
        v->fields.set_u8(0x1fd, 0);
      }
      result = true; break;
    }
    break;
  case _haven_defender_infinite_ammo:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 2) {
        v->fields.set_u8(0x1fe, value);
      } else {
        v->fields.set_u8(0x1fe, 0);
      }
      result = true; break;
    }
    break;
  case _haven_defender_weapon_pickup:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 2) {
        v->fields.set_u8(0x1ff, value);
      } else {
        v->fields.set_u8(0x1ff, 0);
      }
      result = true; break;
    }
    break;
  case _haven_defender_walking_speed:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 10) {
        v->fields.set_u8(0x200, value);
      } else {
        v->fields.set_u8(0x200, 0);
      }
      result = true; break;
    }
    break;
  case _haven_defender_personal_gravity:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 5) {
        v->fields.set_u8(0x201, value);
      } else {
        v->fields.set_u8(0x201, 0);
      }
      result = true; break;
    }
    break;
  case _haven_defender_vehicle_use:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 3) {
        v->fields.set_u8(0x202, value);
      } else {
        v->fields.set_u8(0x202, 0);
      }
      result = true; break;
    }
    break;
  case _haven_defender_motion_tracker:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 4) {
        v->fields.set_u16(0x208, value);
      } else {
        v->fields.set_u16(0x208, 0);
      }
      result = true; break;
    }
    break;
  case _haven_defender_motion_tracker_range:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 7) {
        v->fields.set_u16(0x20a, value);
      } else {
        v->fields.set_u16(0x20a, 0);
      }
      result = true; break;
    }
    break;
  case _haven_defender_active_camo:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 4) {
        v->fields.set_u8(0x204, value);
      } else {
        v->fields.set_u8(0x204, 0);
      }
      result = true; break;
    }
    break;
  case _haven_defender_waypoint:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 3) {
        v->fields.set_u8(0x205, value);
      } else {
        v->fields.set_u8(0x205, 0);
      }
      result = true; break;
    }
    break;
  case _haven_defender_visual:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 4) {
        v->fields.set_u8(0x206, value);
      } else {
        v->fields.set_u8(0x206, 0);
      }
      result = true; break;
    }
    break;
  case _haven_defender_forced_change_colors:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 13) {
        v->fields.set_u8(0x207, value);
      } else {
        v->fields.set_u8(0x207, 0);
      }
      result = true; break;
    }
    break;
  case _last_human_shld_dmg_resistance:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 12) {
        v->fields.set_u8(0x20c, value);
      } else {
        v->fields.set_u8(0x20c, 0);
      }
      result = true; break;
    }
    break;
  case _last_human_shld_multiplier:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 5) {
        v->fields.set_u8(0x210, value);
      } else {
        v->fields.set_u8(0x210, 0);
      }
      result = true; break;
    }
    break;
  case _last_human_shld_recharge_rate:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 9) {
        v->fields.set_u8(0x20d, value);
      } else {
        v->fields.set_u8(0x20d, 0);
      }
      result = true; break;
    }
    break;
  case _last_human_headshot_immunity:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 2) {
        v->fields.set_u8(0x20f, value);
      } else {
        v->fields.set_u8(0x20f, 0);
      }
      result = true; break;
    }
    break;
  case _last_human_vampirism:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 5) {
        v->fields.set_u8(0x20e, value);
      } else {
        v->fields.set_u8(0x20e, 0);
      }
      result = true; break;
    }
    break;
  case _last_human_dmg_modifier_percentage:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 12) {
        v->fields.set_u8(0x218, value);
      } else {
        v->fields.set_u8(0x218, 0);
      }
      result = true; break;
    }
    break;
  case _last_human_initial_primary_weapon:
    if (parameter_type == _text_value_pair_parameter_string_id) {
      c_game_engine_variant *v = this->writeable();
      ((c_player_trait_weapons *)v->fields.addr(0x214))->set_initial_primary_weapon(value, true);
      result = true; break;
    }
    break;
  case _last_human_initial_secondary_weapon:
    if (parameter_type == _text_value_pair_parameter_string_id) {
      c_game_engine_variant *v = this->writeable();
      ((c_player_trait_weapons *)v->fields.addr(0x214))->set_initial_secondary_weapon(value, true);
      result = true; break;
    }
    break;
  case _last_human_initial_grenade_count:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 2) {
        v->fields.set_u16(0x214, value);
      } else {
        v->fields.set_u16(0x214, 0);
      }
      result = true; break;
    }
    break;
  case _last_human_recharging_grenades:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 2) {
        v->fields.set_u8(0x219, value);
      } else {
        v->fields.set_u8(0x219, 0);
      }
      result = true; break;
    }
    break;
  case _last_human_infinite_ammo:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 2) {
        v->fields.set_u8(0x21a, value);
      } else {
        v->fields.set_u8(0x21a, 0);
      }
      result = true; break;
    }
    break;
  case _last_human_weapon_pickup:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 2) {
        v->fields.set_u8(0x21b, value);
      } else {
        v->fields.set_u8(0x21b, 0);
      }
      result = true; break;
    }
    break;
  case _last_human_walking_speed:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 10) {
        v->fields.set_u8(0x21c, value);
      } else {
        v->fields.set_u8(0x21c, 0);
      }
      result = true; break;
    }
    break;
  case _last_human_personal_gravity:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 5) {
        v->fields.set_u8(0x21d, value);
      } else {
        v->fields.set_u8(0x21d, 0);
      }
      result = true; break;
    }
    break;
  case _last_human_vehicle_use:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 3) {
        v->fields.set_u8(0x21e, value);
      } else {
        v->fields.set_u8(0x21e, 0);
      }
      result = true; break;
    }
    break;
  case _last_human_motion_tracker:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 4) {
        v->fields.set_u16(0x224, value);
      } else {
        v->fields.set_u16(0x224, 0);
      }
      result = true; break;
    }
    break;
  case _last_human_motion_tracker_range:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 7) {
        v->fields.set_u16(0x226, value);
      } else {
        v->fields.set_u16(0x226, 0);
      }
      result = true; break;
    }
    break;
  case _last_human_active_camo:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 4) {
        v->fields.set_u8(0x220, value);
      } else {
        v->fields.set_u8(0x220, 0);
      }
      result = true; break;
    }
    break;
  case _last_human_waypoint:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 3) {
        v->fields.set_u8(0x221, value);
      } else {
        v->fields.set_u8(0x221, 0);
      }
      result = true; break;
    }
    break;
  case _last_human_visual:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 4) {
        v->fields.set_u8(0x222, value);
      } else {
        v->fields.set_u8(0x222, 0);
      }
      result = true; break;
    }
    break;
  case _last_human_forced_change_colors:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 13) {
        v->fields.set_u8(0x223, value);
      } else {
        v->fields.set_u8(0x223, 0);
      }
      result = true; break;
    }
    break;
  case _king_score_to_win:
    if (parameter_type == _text_value_pair_parameter_integer) {
      short x = (short)value;
      c_game_engine_variant *v = this->writeable();
      if (x < 0 || x > 1000) {
        x = 100;
      }
      v->fields.set_s16(0x1b0, x);
      result = true; break;
    }
    break;
  case _king_moving_hill:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 8) {
        v->fields.set_u8(0x1b2, value);
      } else {
        v->fields.set_u8(0x1b2, 3);
      }
      result = true; break;
    }
    break;
  case _king_moving_hill_order:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value > 1) {
        v->fields.set_u8(0x1b3, 1);
      } else {
        v->fields.set_u8(0x1b3, value);
      }
      result = true; break;
    }
    break;
  case _king_inside_hill_points:
    if (parameter_type == _text_value_pair_parameter_integer) {
      signed char x = (signed char)value;
      c_game_engine_variant *v = this->writeable();
      if (x < -10 || x > 10) {
        x = 1;
      }
      v->fields.set_s8(0x1b4, x);
      result = true; break;
    }
    break;
  case _king_outside_hill_points:
  case _juggernaut_kill_points:
    if (parameter_type == _text_value_pair_parameter_integer) {
      signed char x = (signed char)value;
      c_game_engine_variant *v = this->writeable();
      if (x >= -10 && x <= 10) {
        v->fields.set_s8(0x1b5, x);
      } else {
        v->fields.set_s8(0x1b5, 0);
      }
      result = true; break;
    }
    break;
  case _king_uncontested_hill_points:
    if (parameter_type == _text_value_pair_parameter_integer) {
      signed char x = (signed char)value;
      c_game_engine_variant *v = this->writeable();
      if (x >= -10 && x <= 10) {
        v->fields.set_s8(0x1b6, x);
      } else {
        v->fields.set_s8(0x1b6, 0);
      }
      result = true; break;
    }
    break;
  case _territories_capture_time:
    if (parameter_type == _text_value_pair_parameter_integer) {
      short x = (short)value;
      c_game_engine_variant *v = this->writeable();
      if (x < 0 || x > 120) {
        x = 30;
      }
      v->fields.set_s16(0x1b0, x);
      result = true; break;
    }
    break;
  case _territories_one_sided:
  case _assault_reset_bomb_on_disarm:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      v->fields.word_flags(0x1ac)->set(0, value != 0);
      result = true; break;
    }
    break;
  case _territories_lock_after_capture:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      v->fields.word_flags(0x1ac)->set(1, value != 0);
      result = true; break;
    }
    break;
  case _territories_sudden_death:
    if (parameter_type == _text_value_pair_parameter_integer) {
      short x = (short)value;
      c_game_engine_variant *v = this->writeable();
      if (x < -1 || x > 300) {
        x = -1;
      }
      v->fields.set_s16(0x1b2, x);
      result = true; break;
    }
    break;
  case _territories_respawn_on_capture:
  case _assault_respawn:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 3) {
        v->fields.set_u16(0x1ae, value);
      } else {
        v->fields.set_u16(0x1ae, 0);
      }
      result = true; break;
    }
    break;
  case _territory_defender_shld_dmg_resistance:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 12) {
        v->fields.set_u8(0x1b4, value);
      } else {
        v->fields.set_u8(0x1b4, 0);
      }
      result = true; break;
    }
    break;
  case _territory_defender_shld_multiplier:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 5) {
        v->fields.set_u8(0x1b8, value);
      } else {
        v->fields.set_u8(0x1b8, 0);
      }
      result = true; break;
    }
    break;
  case _territory_defender_shld_recharge_rate:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 9) {
        v->fields.set_u8(0x1b5, value);
      } else {
        v->fields.set_u8(0x1b5, 0);
      }
      result = true; break;
    }
    break;
  case _territory_defender_headshot_immunity:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 2) {
        v->fields.set_u8(0x1b7, value);
      } else {
        v->fields.set_u8(0x1b7, 0);
      }
      result = true; break;
    }
    break;
  case _territory_defender_vampirism:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 5) {
        v->fields.set_u8(0x1b6, value);
      } else {
        v->fields.set_u8(0x1b6, 0);
      }
      result = true; break;
    }
    break;
  case _territory_defender_dmg_modifier_percentage:
  case _assault_carrier_shld_dmg_resistance:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 12) {
        v->fields.set_u8(0x1c0, value);
      } else {
        v->fields.set_u8(0x1c0, 0);
      }
      result = true; break;
    }
    break;
  case _territory_defender_initial_primary_weapon:
    if (parameter_type == _text_value_pair_parameter_string_id) {
      c_game_engine_variant *v = this->writeable();
      ((c_player_trait_weapons *)v->fields.addr(0x1bc))->set_initial_primary_weapon(value, true);
      result = true; break;
    }
    break;
  case _territory_defender_initial_secondary_weapon:
    if (parameter_type == _text_value_pair_parameter_string_id) {
      c_game_engine_variant *v = this->writeable();
      ((c_player_trait_weapons *)v->fields.addr(0x1bc))->set_initial_secondary_weapon(value, true);
      result = true; break;
    }
    break;
  case _territory_defender_initial_grenade_count:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 2) {
        v->fields.set_u16(0x1bc, value);
      } else {
        v->fields.set_u16(0x1bc, 0);
      }
      result = true; break;
    }
    break;
  case _territory_defender_recharging_grenades:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 2) {
        v->fields.set_u8(0x1c1, value);
      } else {
        v->fields.set_u8(0x1c1, 0);
      }
      result = true; break;
    }
    break;
  case _territory_defender_infinite_ammo:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 2) {
        v->fields.set_u8(0x1c2, value);
      } else {
        v->fields.set_u8(0x1c2, 0);
      }
      result = true; break;
    }
    break;
  case _territory_defender_weapon_pickup:
  case _assault_carrier_headshot_immunity:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 2) {
        v->fields.set_u8(0x1c3, value);
      } else {
        v->fields.set_u8(0x1c3, 0);
      }
      result = true; break;
    }
    break;
  case _territory_defender_walking_speed:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 10) {
        v->fields.set_u8(0x1c4, value);
      } else {
        v->fields.set_u8(0x1c4, 0);
      }
      result = true; break;
    }
    break;
  case _territory_defender_personal_gravity:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 5) {
        v->fields.set_u8(0x1c5, value);
      } else {
        v->fields.set_u8(0x1c5, 0);
      }
      result = true; break;
    }
    break;
  case _territory_defender_vehicle_use:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 3) {
        v->fields.set_u8(0x1c6, value);
      } else {
        v->fields.set_u8(0x1c6, 0);
      }
      result = true; break;
    }
    break;
  case _territory_defender_motion_tracker:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 4) {
        v->fields.set_u16(0x1cc, value);
      } else {
        v->fields.set_u16(0x1cc, 0);
      }
      result = true; break;
    }
    break;
  case _territory_defender_motion_tracker_range:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 7) {
        v->fields.set_u16(0x1ce, value);
      } else {
        v->fields.set_u16(0x1ce, 0);
      }
      result = true; break;
    }
    break;
  case _territory_defender_active_camo:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 4) {
        v->fields.set_u8(0x1c8, value);
      } else {
        v->fields.set_u8(0x1c8, 0);
      }
      result = true; break;
    }
    break;
  case _territory_defender_waypoint:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 3) {
        v->fields.set_u8(0x1c9, value);
      } else {
        v->fields.set_u8(0x1c9, 0);
      }
      result = true; break;
    }
    break;
  case _territory_defender_visual:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 4) {
        v->fields.set_u8(0x1ca, value);
      } else {
        v->fields.set_u8(0x1ca, 0);
      }
      result = true; break;
    }
    break;
  case _territory_defender_forced_change_colors:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 13) {
        v->fields.set_u8(0x1cb, value);
      } else {
        v->fields.set_u8(0x1cb, 0);
      }
      result = true; break;
    }
    break;
  case _territory_attacker_shld_dmg_resistance:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 12) {
        v->fields.set_u8(0x1d0, value);
      } else {
        v->fields.set_u8(0x1d0, 0);
      }
      result = true; break;
    }
    break;
  case _territory_attacker_shld_multiplier:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 5) {
        v->fields.set_u8(0x1d4, value);
      } else {
        v->fields.set_u8(0x1d4, 0);
      }
      result = true; break;
    }
    break;
  case _territory_attacker_shld_recharge_rate:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 9) {
        v->fields.set_u8(0x1d1, value);
      } else {
        v->fields.set_u8(0x1d1, 0);
      }
      result = true; break;
    }
    break;
  case _territory_attacker_headshot_immunity:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 2) {
        v->fields.set_u8(0x1d3, value);
      } else {
        v->fields.set_u8(0x1d3, 0);
      }
      result = true; break;
    }
    break;
  case _territory_attacker_vampirism:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 5) {
        v->fields.set_u8(0x1d2, value);
      } else {
        v->fields.set_u8(0x1d2, 0);
      }
      result = true; break;
    }
    break;
  case _territory_attacker_dmg_modifier_percentage:
  case _assault_arming_shld_dmg_resistance:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 12) {
        v->fields.set_u8(0x1dc, value);
      } else {
        v->fields.set_u8(0x1dc, 0);
      }
      result = true; break;
    }
    break;
  case _territory_attacker_initial_primary_weapon:
    if (parameter_type == _text_value_pair_parameter_string_id) {
      c_game_engine_variant *v = this->writeable();
      ((c_player_trait_weapons *)v->fields.addr(0x1d8))->set_initial_primary_weapon(value, true);
      result = true; break;
    }
    break;
  case _territory_attacker_initial_secondary_weapon:
    if (parameter_type == _text_value_pair_parameter_string_id) {
      c_game_engine_variant *v = this->writeable();
      ((c_player_trait_weapons *)v->fields.addr(0x1d8))->set_initial_secondary_weapon(value, true);
      result = true; break;
    }
    break;
  case _territory_attacker_initial_grenade_count:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 2) {
        v->fields.set_u16(0x1d8, value);
      } else {
        v->fields.set_u16(0x1d8, 0);
      }
      result = true; break;
    }
    break;
  case _territory_attacker_recharging_grenades:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 2) {
        v->fields.set_u8(0x1dd, value);
      } else {
        v->fields.set_u8(0x1dd, 0);
      }
      result = true; break;
    }
    break;
  case _territory_attacker_infinite_ammo:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 2) {
        v->fields.set_u8(0x1de, value);
      } else {
        v->fields.set_u8(0x1de, 0);
      }
      result = true; break;
    }
    break;
  case _territory_attacker_weapon_pickup:
  case _assault_arming_headshot_immunity:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 2) {
        v->fields.set_u8(0x1df, value);
      } else {
        v->fields.set_u8(0x1df, 0);
      }
      result = true; break;
    }
    break;
  case _territory_attacker_walking_speed:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 10) {
        v->fields.set_u8(0x1e0, value);
      } else {
        v->fields.set_u8(0x1e0, 0);
      }
      result = true; break;
    }
    break;
  case _territory_attacker_personal_gravity:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 5) {
        v->fields.set_u8(0x1e1, value);
      } else {
        v->fields.set_u8(0x1e1, 0);
      }
      result = true; break;
    }
    break;
  case _territory_attacker_vehicle_use:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 3) {
        v->fields.set_u8(0x1e2, value);
      } else {
        v->fields.set_u8(0x1e2, 0);
      }
      result = true; break;
    }
    break;
  case _territory_attacker_motion_tracker:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 4) {
        v->fields.set_u16(0x1e8, value);
      } else {
        v->fields.set_u16(0x1e8, 0);
      }
      result = true; break;
    }
    break;
  case _territory_attacker_motion_tracker_range:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 7) {
        v->fields.set_u16(0x1ea, value);
      } else {
        v->fields.set_u16(0x1ea, 0);
      }
      result = true; break;
    }
    break;
  case _territory_attacker_active_camo:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 4) {
        v->fields.set_u8(0x1e4, value);
      } else {
        v->fields.set_u8(0x1e4, 0);
      }
      result = true; break;
    }
    break;
  case _territory_attacker_waypoint:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 3) {
        v->fields.set_u8(0x1e5, value);
      } else {
        v->fields.set_u8(0x1e5, 0);
      }
      result = true; break;
    }
    break;
  case _territory_attacker_visual:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 4) {
        v->fields.set_u8(0x1e6, value);
      } else {
        v->fields.set_u8(0x1e6, 0);
      }
      result = true; break;
    }
    break;
  case _territory_attacker_forced_change_colors:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 13) {
        v->fields.set_u8(0x1e7, value);
      } else {
        v->fields.set_u8(0x1e7, 0);
      }
      result = true; break;
    }
    break;
  case _vip_score_to_win:
    if (parameter_type == _text_value_pair_parameter_integer) {
      short x = (short)value;
      if (x < 0 || x > 500) {
        x = 15;
      }
      ((c_game_engine_vip_variant *)variant_storage)->m_score_to_win_round = x;
      result = true; break;
    }
    break;
  case _vip_single_vip:
    if (parameter_type == _text_value_pair_parameter_integer) {
      ((c_game_engine_vip_variant *)variant_storage)->m_variant_flags.set(c_game_engine_vip_variant::_vip_variant_single_vip, value != 0);
      result = true; break;
    }
    break;
  case _vip_destination_zones_enabled:
    if (parameter_type == _text_value_pair_parameter_integer) {
      ((c_game_engine_vip_variant *)variant_storage)->m_variant_flags.set(c_game_engine_vip_variant::_vip_variant_destination_zones_enabled, value != 0);
      result = true; break;
    }
    break;
  case _vip_end_round_on_vip_death:
    if (parameter_type == _text_value_pair_parameter_integer) {
      ((c_game_engine_vip_variant *)variant_storage)->m_variant_flags.set(c_game_engine_vip_variant::_vip_variant_end_round_on_vip_death, value != 0);
      result = true; break;
    }
    break;
  case _vip_kill_points:
    if (parameter_type == _text_value_pair_parameter_integer) {
      signed char x = (signed char)value;
      if (x >= -10 && x <= 10) {
        ((c_game_engine_vip_variant *)variant_storage)->m_kill_points = x;
      } else {
        ((c_game_engine_vip_variant *)variant_storage)->m_kill_points = 0;
      }
      result = true; break;
    }
    break;
  case _vip_takedown_points:
    if (parameter_type == _text_value_pair_parameter_integer) {
      signed char x = (signed char)value;
      if (x >= -10 && x <= 10) {
        ((c_game_engine_vip_variant *)variant_storage)->m_takedown_points = x;
      } else {
        ((c_game_engine_vip_variant *)variant_storage)->m_takedown_points = 0;
      }
      result = true; break;
    }
    break;
  case _vip_kill_as_vip_points:
    if (parameter_type == _text_value_pair_parameter_integer) {
      signed char x = (signed char)value;
      if (x >= -10 && x <= 10) {
        ((c_game_engine_vip_variant *)variant_storage)->m_kill_as_vip_points = x;
      } else {
        ((c_game_engine_vip_variant *)variant_storage)->m_kill_as_vip_points = 0;
      }
      result = true; break;
    }
    break;
  case _vip_death_points:
    if (parameter_type == _text_value_pair_parameter_integer) {
      signed char x = (signed char)value;
      if (x >= -10 && x <= 10) {
        ((c_game_engine_vip_variant *)variant_storage)->m_vip_death_points = x;
      } else {
        ((c_game_engine_vip_variant *)variant_storage)->m_vip_death_points = 0;
      }
      result = true; break;
    }
    break;
  case _vip_zone_arrival_points:
    if (parameter_type == _text_value_pair_parameter_integer) {
      signed char x = (signed char)value;
      if (x >= -10 && x <= 10) {
        ((c_game_engine_vip_variant *)variant_storage)->m_destination_arrival_points = x;
      } else {
        ((c_game_engine_vip_variant *)variant_storage)->m_destination_arrival_points = 0;
      }
      result = true; break;
    }
    break;
  case _vip_suicide_points:
    if (parameter_type == _text_value_pair_parameter_integer) {
      signed char x = (signed char)value;
      if (x >= -10 && x <= 10) {
        ((c_game_engine_vip_variant *)variant_storage)->m_suicide_points = x;
      } else {
        ((c_game_engine_vip_variant *)variant_storage)->m_suicide_points = 0;
      }
      result = true; break;
    }
    break;
  case _vip_vip_friendly_death_points:
    if (parameter_type == _text_value_pair_parameter_integer) {
      signed char x = (signed char)value;
      if (x >= -10 && x <= 10) {
        ((c_game_engine_vip_variant *)variant_storage)->m_vip_suicide_points = x;
      } else {
        ((c_game_engine_vip_variant *)variant_storage)->m_vip_suicide_points = 0;
      }
      result = true; break;
    }
    break;
  case _vip_betrayal_points:
    if (parameter_type == _text_value_pair_parameter_integer) {
      signed char x = (signed char)value;
      if (x >= -10 && x <= 10) {
        ((c_game_engine_vip_variant *)variant_storage)->m_betrayal_points = x;
      } else {
        ((c_game_engine_vip_variant *)variant_storage)->m_betrayal_points = 0;
      }
      result = true; break;
    }
    break;
  case _vip_selection:
    if (parameter_type == _text_value_pair_parameter_integer) {
      if ((unsigned)value <= 3) {
        U8(this, 0x1c0) = value;
      } else {
        U8(this, 0x1c0) = 0;
      }
      result = true; break;
    }
    break;
  case _vip_zone_movement:
    if (parameter_type == _text_value_pair_parameter_integer) {
      if ((unsigned)value <= 10) {
        U8(this, 0x1c1) = value;
      } else {
        U8(this, 0x1c1) = 0;
      }
      result = true; break;
    }
    break;
  case _vip_zone_order:
    if (parameter_type == _text_value_pair_parameter_integer) {
      if ((unsigned)value > 1) {
        U8(this, 0x1c2) = 1;
      } else {
        U8(this, 0x1c2) = value;
      }
      result = true; break;
    }
    break;
  case _vip_influence_radius:
    if (parameter_type == _text_value_pair_parameter_integer) {
      short x = (short)value;
      if (x >= 0 && x <= 50) {
        ((c_game_engine_vip_variant *)variant_storage)->m_influence_radius = x;
      } else {
        ((c_game_engine_vip_variant *)variant_storage)->m_influence_radius = 0;
      }
      result = true; break;
    }
    break;
  case _vip_team_shld_dmg_resistance:
    if (parameter_type == _text_value_pair_parameter_integer) {
      if ((unsigned)value <= 12) {
        U8(this, 0x1c6) = value;
      } else {
        U8(this, 0x1c6) = 0;
      }
      result = true; break;
    }
    break;
  case _vip_team_shld_multiplier:
    if (parameter_type == _text_value_pair_parameter_integer) {
      if ((unsigned)value <= 5) {
        U8(this, 0x1ca) = value;
      } else {
        U8(this, 0x1ca) = 0;
      }
      result = true; break;
    }
    break;
  case _vip_team_shld_recharge_rate:
    if (parameter_type == _text_value_pair_parameter_integer) {
      if ((unsigned)value <= 9) {
        U8(this, 0x1c7) = value;
      } else {
        U8(this, 0x1c7) = 0;
      }
      result = true; break;
    }
    break;
  case _vip_team_headshot_immunity:
    if (parameter_type == _text_value_pair_parameter_integer) {
      if ((unsigned)value <= 2) {
        U8(this, 0x1c9) = value;
      } else {
        U8(this, 0x1c9) = 0;
      }
      result = true; break;
    }
    break;
  case _vip_team_vampirism:
    if (parameter_type == _text_value_pair_parameter_integer) {
      if ((unsigned)value <= 5) {
        U8(this, 0x1c8) = value;
      } else {
        U8(this, 0x1c8) = 0;
      }
      result = true; break;
    }
    break;
  case _vip_team_dmg_modifier_percentage:
    if (parameter_type == _text_value_pair_parameter_integer) {
      if ((unsigned)value <= 12) {
        U8(this, 0x1d2) = value;
      } else {
        U8(this, 0x1d2) = 0;
      }
      result = true; break;
    }
    break;
  case _vip_team_initial_primary_weapon:
    if (parameter_type == _text_value_pair_parameter_string_id) {
      ((c_game_engine_vip_variant *)variant_storage)->m_vip_team_traits.get_weapons_traits_writeable()->set_initial_primary_weapon(value, true);
      result = true; break;
    }
    break;
  case _vip_team_initial_secondary_weapon:
    if (parameter_type == _text_value_pair_parameter_string_id) {
      ((c_game_engine_vip_variant *)variant_storage)->m_vip_team_traits.get_weapons_traits_writeable()->set_initial_secondary_weapon(value, true);
      result = true; break;
    }
    break;
  case _vip_team_initial_grenade_count:
    if (parameter_type == _text_value_pair_parameter_integer) {
      if ((unsigned)value <= 2) {
        U16(this, 0x1ce) = value;
      } else {
        U16(this, 0x1ce) = 0;
      }
      result = true; break;
    }
    break;
  case _vip_team_recharging_grenades:
    if (parameter_type == _text_value_pair_parameter_integer) {
      if ((unsigned)value <= 2) {
        U8(this, 0x1d3) = value;
      } else {
        U8(this, 0x1d3) = 0;
      }
      result = true; break;
    }
    break;
  case _vip_team_infinite_ammo:
    if (parameter_type == _text_value_pair_parameter_integer) {
      if ((unsigned)value <= 2) {
        U8(this, 0x1d4) = value;
      } else {
        U8(this, 0x1d4) = 0;
      }
      result = true; break;
    }
    break;
  case _vip_team_weapon_pickup:
    if (parameter_type == _text_value_pair_parameter_integer) {
      if ((unsigned)value <= 2) {
        U8(this, 0x1d5) = value;
      } else {
        U8(this, 0x1d5) = 0;
      }
      result = true; break;
    }
    break;
  case _vip_team_walking_speed:
    if (parameter_type == _text_value_pair_parameter_integer) {
      if ((unsigned)value <= 10) {
        U8(this, 0x1d6) = value;
      } else {
        U8(this, 0x1d6) = 0;
      }
      result = true; break;
    }
    break;
  case _vip_team_personal_gravity:
    if (parameter_type == _text_value_pair_parameter_integer) {
      if ((unsigned)value <= 5) {
        U8(this, 0x1d7) = value;
      } else {
        U8(this, 0x1d7) = 0;
      }
      result = true; break;
    }
    break;
  case _vip_team_vehicle_use:
    if (parameter_type == _text_value_pair_parameter_integer) {
      if ((unsigned)value <= 3) {
        U8(this, 0x1d8) = value;
      } else {
        U8(this, 0x1d8) = 0;
      }
      result = true; break;
    }
    break;
  case _vip_team_motion_tracker:
    if (parameter_type == _text_value_pair_parameter_integer) {
      if ((unsigned)value <= 4) {
        U16(this, 0x1de) = value;
      } else {
        U16(this, 0x1de) = 0;
      }
      result = true; break;
    }
    break;
  case _vip_team_motion_tracker_range:
    if (parameter_type == _text_value_pair_parameter_integer) {
      if ((unsigned)value <= 7) {
        U16(this, 0x1e0) = value;
      } else {
        U16(this, 0x1e0) = 0;
      }
      result = true; break;
    }
    break;
  case _vip_team_active_camo:
    if (parameter_type == _text_value_pair_parameter_integer) {
      if ((unsigned)value <= 4) {
        U8(this, 0x1da) = value;
      } else {
        U8(this, 0x1da) = 0;
      }
      result = true; break;
    }
    break;
  case _vip_team_waypoint:
    if (parameter_type == _text_value_pair_parameter_integer) {
      if ((unsigned)value <= 3) {
        U8(this, 0x1db) = value;
      } else {
        U8(this, 0x1db) = 0;
      }
      result = true; break;
    }
    break;
  case _vip_team_visual:
    if (parameter_type == _text_value_pair_parameter_integer) {
      if ((unsigned)value <= 4) {
        U8(this, 0x1dc) = value;
      } else {
        U8(this, 0x1dc) = 0;
      }
      result = true; break;
    }
    break;
  case _vip_team_forced_change_colors:
    if (parameter_type == _text_value_pair_parameter_integer) {
      if ((unsigned)value <= 13) {
        U8(this, 0x1dd) = value;
      } else {
        U8(this, 0x1dd) = 0;
      }
      result = true; break;
    }
    break;
  case _vip_influence_shld_dmg_resistance:
    if (parameter_type == _text_value_pair_parameter_integer) {
      if ((unsigned)value <= 12) {
        U8(this, 0x1e2) = value;
      } else {
        U8(this, 0x1e2) = 0;
      }
      result = true; break;
    }
    break;
  case _vip_influence_shld_multiplier:
    if (parameter_type == _text_value_pair_parameter_integer) {
      if ((unsigned)value <= 5) {
        U8(this, 0x1e6) = value;
      } else {
        U8(this, 0x1e6) = 0;
      }
      result = true; break;
    }
    break;
  case _vip_influence_shld_recharge_rate:
    if (parameter_type == _text_value_pair_parameter_integer) {
      if ((unsigned)value <= 9) {
        U8(this, 0x1e3) = value;
      } else {
        U8(this, 0x1e3) = 0;
      }
      result = true; break;
    }
    break;
  case _vip_influence_headshot_immunity:
    if (parameter_type == _text_value_pair_parameter_integer) {
      if ((unsigned)value <= 2) {
        U8(this, 0x1e5) = value;
      } else {
        U8(this, 0x1e5) = 0;
      }
      result = true; break;
    }
    break;
  case _vip_influence_vampirism:
    if (parameter_type == _text_value_pair_parameter_integer) {
      if ((unsigned)value <= 5) {
        U8(this, 0x1e4) = value;
      } else {
        U8(this, 0x1e4) = 0;
      }
      result = true; break;
    }
    break;
  case _vip_influence_dmg_modifier_percentage:
    if (parameter_type == _text_value_pair_parameter_integer) {
      if ((unsigned)value <= 12) {
        U8(this, 0x1ee) = value;
      } else {
        U8(this, 0x1ee) = 0;
      }
      result = true; break;
    }
    break;
  case _vip_influence_initial_primary_weapon:
    if (parameter_type == _text_value_pair_parameter_string_id) {
      ((c_game_engine_vip_variant *)variant_storage)->m_vip_influence_traits.get_weapons_traits_writeable()->set_initial_primary_weapon(value, true);
      result = true; break;
    }
    break;
  case _vip_influence_initial_secondary_weapon:
    if (parameter_type == _text_value_pair_parameter_string_id) {
      ((c_game_engine_vip_variant *)variant_storage)->m_vip_influence_traits.get_weapons_traits_writeable()->set_initial_secondary_weapon(value, true);
      result = true; break;
    }
    break;
  case _vip_influence_initial_grenade_count:
    if (parameter_type == _text_value_pair_parameter_integer) {
      if ((unsigned)value <= 2) {
        U16(this, 0x1ea) = value;
      } else {
        U16(this, 0x1ea) = 0;
      }
      result = true; break;
    }
    break;
  case _vip_influence_recharging_grenades:
    if (parameter_type == _text_value_pair_parameter_integer) {
      if ((unsigned)value <= 2) {
        U8(this, 0x1ef) = value;
      } else {
        U8(this, 0x1ef) = 0;
      }
      result = true; break;
    }
    break;
  case _vip_influence_infinite_ammo:
    if (parameter_type == _text_value_pair_parameter_integer) {
      if ((unsigned)value <= 2) {
        U8(this, 0x1f0) = value;
      } else {
        U8(this, 0x1f0) = 0;
      }
      result = true; break;
    }
    break;
  case _vip_influence_weapon_pickup:
    if (parameter_type == _text_value_pair_parameter_integer) {
      if ((unsigned)value <= 2) {
        U8(this, 0x1f1) = value;
      } else {
        U8(this, 0x1f1) = 0;
      }
      result = true; break;
    }
    break;
  case _vip_influence_walking_speed:
    if (parameter_type == _text_value_pair_parameter_integer) {
      if ((unsigned)value <= 10) {
        U8(this, 0x1f2) = value;
      } else {
        U8(this, 0x1f2) = 0;
      }
      result = true; break;
    }
    break;
  case _vip_influence_personal_gravity:
    if (parameter_type == _text_value_pair_parameter_integer) {
      if ((unsigned)value <= 5) {
        U8(this, 0x1f3) = value;
      } else {
        U8(this, 0x1f3) = 0;
      }
      result = true; break;
    }
    break;
  case _vip_influence_vehicle_use:
    if (parameter_type == _text_value_pair_parameter_integer) {
      if ((unsigned)value <= 3) {
        U8(this, 0x1f4) = value;
      } else {
        U8(this, 0x1f4) = 0;
      }
      result = true; break;
    }
    break;
  case _vip_influence_motion_tracker:
    if (parameter_type == _text_value_pair_parameter_integer) {
      if ((unsigned)value <= 4) {
        U16(this, 0x1fa) = value;
      } else {
        U16(this, 0x1fa) = 0;
      }
      result = true; break;
    }
    break;
  case _vip_influence_motion_tracker_range:
    if (parameter_type == _text_value_pair_parameter_integer) {
      if ((unsigned)value <= 7) {
        U16(this, 0x1fc) = value;
      } else {
        U16(this, 0x1fc) = 0;
      }
      result = true; break;
    }
    break;
  case _vip_influence_active_camo:
    if (parameter_type == _text_value_pair_parameter_integer) {
      if ((unsigned)value <= 4) {
        U8(this, 0x1f6) = value;
      } else {
        U8(this, 0x1f6) = 0;
      }
      result = true; break;
    }
    break;
  case _vip_influence_waypoint:
    if (parameter_type == _text_value_pair_parameter_integer) {
      if ((unsigned)value <= 3) {
        U8(this, 0x1f7) = value;
      } else {
        U8(this, 0x1f7) = 0;
      }
      result = true; break;
    }
    break;
  case _vip_influence_visual:
    if (parameter_type == _text_value_pair_parameter_integer) {
      if ((unsigned)value <= 4) {
        U8(this, 0x1f8) = value;
      } else {
        U8(this, 0x1f8) = 0;
      }
      result = true; break;
    }
    break;
  case _vip_influence_forced_change_colors:
    if (parameter_type == _text_value_pair_parameter_integer) {
      if ((unsigned)value <= 13) {
        U8(this, 0x1f9) = value;
      } else {
        U8(this, 0x1f9) = 0;
      }
      result = true; break;
    }
    break;
  case _vip_shld_dmg_resistance:
    if (parameter_type == _text_value_pair_parameter_integer) {
      if ((unsigned)value <= 12) {
        U8(this, 0x1fe) = value;
      } else {
        U8(this, 0x1fe) = 0;
      }
      result = true; break;
    }
    break;
  case _vip_shld_multiplier:
    if (parameter_type == _text_value_pair_parameter_integer) {
      if ((unsigned)value <= 5) {
        U8(this, 0x202) = value;
      } else {
        U8(this, 0x202) = 0;
      }
      result = true; break;
    }
    break;
  case _vip_shld_recharge_rate:
    if (parameter_type == _text_value_pair_parameter_integer) {
      if ((unsigned)value <= 9) {
        U8(this, 0x1ff) = value;
      } else {
        U8(this, 0x1ff) = 0;
      }
      result = true; break;
    }
    break;
  case _vip_headshot_immunity:
    if (parameter_type == _text_value_pair_parameter_integer) {
      if ((unsigned)value <= 2) {
        U8(this, 0x201) = value;
      } else {
        U8(this, 0x201) = 0;
      }
      result = true; break;
    }
    break;
  case _vip_vampirism:
    if (parameter_type == _text_value_pair_parameter_integer) {
      if ((unsigned)value <= 5) {
        U8(this, 0x200) = value;
      } else {
        U8(this, 0x200) = 0;
      }
      result = true; break;
    }
    break;
  case _vip_dmg_modifier_percentage:
    if (parameter_type == _text_value_pair_parameter_integer) {
      if ((unsigned)value <= 12) {
        U8(this, 0x20a) = value;
      } else {
        U8(this, 0x20a) = 0;
      }
      result = true; break;
    }
    break;
  case _vip_initial_primary_weapon:
    if (parameter_type == _text_value_pair_parameter_string_id) {
      ((c_game_engine_vip_variant *)variant_storage)->m_vip_traits.get_weapons_traits_writeable()->set_initial_primary_weapon(value, true);
      result = true; break;
    }
    break;
  case _vip_initial_secondary_weapon:
    if (parameter_type == _text_value_pair_parameter_string_id) {
      ((c_game_engine_vip_variant *)variant_storage)->m_vip_traits.get_weapons_traits_writeable()->set_initial_secondary_weapon(value, true);
      result = true; break;
    }
    break;
  case _vip_initial_grenade_count:
    if (parameter_type == _text_value_pair_parameter_integer) {
      if ((unsigned)value <= 2) {
        U16(this, 0x206) = value;
      } else {
        U16(this, 0x206) = 0;
      }
      result = true; break;
    }
    break;
  case _vip_recharging_grenades:
    if (parameter_type == _text_value_pair_parameter_integer) {
      if ((unsigned)value <= 2) {
        U8(this, 0x20b) = value;
      } else {
        U8(this, 0x20b) = 0;
      }
      result = true; break;
    }
    break;
  case _vip_infinite_ammo:
    if (parameter_type == _text_value_pair_parameter_integer) {
      if ((unsigned)value <= 2) {
        U8(this, 0x20c) = value;
      } else {
        U8(this, 0x20c) = 0;
      }
      result = true; break;
    }
    break;
  case _vip_weapon_pickup:
    if (parameter_type == _text_value_pair_parameter_integer) {
      if ((unsigned)value <= 2) {
        U8(this, 0x20d) = value;
      } else {
        U8(this, 0x20d) = 0;
      }
      result = true; break;
    }
    break;
  case _vip_walking_speed:
    if (parameter_type == _text_value_pair_parameter_integer) {
      if ((unsigned)value <= 10) {
        U8(this, 0x20e) = value;
      } else {
        U8(this, 0x20e) = 0;
      }
      result = true; break;
    }
    break;
  case _vip_personal_gravity:
    if (parameter_type == _text_value_pair_parameter_integer) {
      if ((unsigned)value <= 5) {
        U8(this, 0x20f) = value;
      } else {
        U8(this, 0x20f) = 0;
      }
      result = true; break;
    }
    break;
  case _vip_vehicle_use:
    if (parameter_type == _text_value_pair_parameter_integer) {
      if ((unsigned)value <= 3) {
        U8(this, 0x210) = value;
      } else {
        U8(this, 0x210) = 0;
      }
      result = true; break;
    }
    break;
  case _vip_motion_tracker:
    if (parameter_type == _text_value_pair_parameter_integer) {
      if ((unsigned)value <= 4) {
        U16(this, 0x216) = value;
      } else {
        U16(this, 0x216) = 0;
      }
      result = true; break;
    }
    break;
  case _vip_motion_tracker_range:
    if (parameter_type == _text_value_pair_parameter_integer) {
      if ((unsigned)value <= 7) {
        U16(this, 0x218) = value;
      } else {
        U16(this, 0x218) = 0;
      }
      result = true; break;
    }
    break;
  case _vip_active_camo:
    if (parameter_type == _text_value_pair_parameter_integer) {
      if ((unsigned)value <= 4) {
        U8(this, 0x212) = value;
      } else {
        U8(this, 0x212) = 0;
      }
      result = true; break;
    }
    break;
  case _vip_waypoint:
    if (parameter_type == _text_value_pair_parameter_integer) {
      if ((unsigned)value <= 3) {
        U8(this, 0x213) = value;
      } else {
        U8(this, 0x213) = 0;
      }
      result = true; break;
    }
    break;
  case _vip_visual:
    if (parameter_type == _text_value_pair_parameter_integer) {
      if ((unsigned)value <= 4) {
        U8(this, 0x214) = value;
      } else {
        U8(this, 0x214) = 0;
      }
      result = true; break;
    }
    break;
  case _vip_forced_change_colors:
    if (parameter_type == _text_value_pair_parameter_integer) {
      if ((unsigned)value <= 13) {
        U8(this, 0x215) = value;
      } else {
        U8(this, 0x215) = 0;
      }
      result = true; break;
    }
    break;
  case _juggernaut_score_to_win:
    if (parameter_type == _text_value_pair_parameter_integer) {
      short x = (short)value;
      c_game_engine_variant *v = this->writeable();
      if (x < 0 || x > 500) {
        x = 15;
      }
      v->fields.set_s16(0x1ac, x);
      result = true; break;
    }
    break;
  case _juggernaut_juggernaut_kill_points:
    if (parameter_type == _text_value_pair_parameter_integer) {
      signed char x = (signed char)value;
      c_game_engine_variant *v = this->writeable();
      if (x < -10 || x > 10) {
        x = 1;
      }
      v->fields.set_s8(0x1b6, x);
      result = true; break;
    }
    break;
  case _juggernaut_kill_as_juggernaut_points:
    if (parameter_type == _text_value_pair_parameter_integer) {
      signed char x = (signed char)value;
      c_game_engine_variant *v = this->writeable();
      if (x < -10 || x > 10) {
        x = 1;
      }
      v->fields.set_s8(0x1b7, x);
      result = true; break;
    }
    break;
  case _juggernaut_zone_arrival_points:
    if (parameter_type == _text_value_pair_parameter_integer) {
      signed char x = (signed char)value;
      c_game_engine_variant *v = this->writeable();
      if (x < -10 || x > 10) {
        x = 1;
      }
      v->fields.set_s8(0x1b8, x);
      result = true; break;
    }
    break;
  case _juggernaut_suicide_points:
    if (parameter_type == _text_value_pair_parameter_integer) {
      signed char x = (signed char)value;
      c_game_engine_variant *v = this->writeable();
      if (x < -10 || x > 10) {
        x = 1;
      }
      v->fields.set_s8(0x1b9, x);
      result = true; break;
    }
    break;
  case _juggernaut_betrayal_points:
    if (parameter_type == _text_value_pair_parameter_integer) {
      signed char x = (signed char)value;
      c_game_engine_variant *v = this->writeable();
      if (x < -10 || x > 10) {
        x = 1;
      }
      v->fields.set_s8(0x1ba, x);
      result = true; break;
    }
    break;
  case _juggernaut_initial_juggernaut:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 2) {
        v->fields.set_u8(0x1b0, value);
      } else {
        v->fields.set_u8(0x1b0, 0);
      }
      result = true; break;
    }
    break;
  case _juggernaut_next_juggernaut:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 3) {
        v->fields.set_u8(0x1b1, value);
      } else {
        v->fields.set_u8(0x1b1, 0);
      }
      result = true; break;
    }
    break;
  case _juggernaut_allied_against_juggernaut:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if (value != 0) {
        v->fields.or_u8(0x1b2, 0x1);
      } else {
        v->fields.and_u8(0x1b2, 0xfe);
      }
      result = true; break;
    }
    break;
  case _juggernaut_delay:
    if (parameter_type == _text_value_pair_parameter_integer) {
      unsigned char x = (unsigned char)value;
      c_game_engine_variant *v = this->writeable();
      if (x <= 10) {
        v->fields.set_u8(0x1bb, x);
      } else {
        v->fields.set_u8(0x1bb, 0);
      }
      result = true; break;
    }
    break;
  case _juggernaut_zones_enabled:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if (value != 0) {
        v->fields.or_u8(0x1b2, 0x4);
      } else {
        v->fields.and_u8(0x1b2, 0xfb);
      }
      result = true; break;
    }
    break;
  case _juggernaut_zone_movement:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 10) {
        v->fields.set_u8(0x1b3, value);
      } else {
        v->fields.set_u8(0x1b3, 0);
      }
      result = true; break;
    }
    break;
  case _juggernaut_zone_order:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 1) {
        v->fields.set_u8(0x1b4, value);
      } else {
        v->fields.set_u8(0x1b4, 1);
      }
      result = true; break;
    }
    break;
  case _juggernaut_respawn_on_lone_juggernaut:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if (value != 0) {
        v->fields.or_u8(0x1b2, 0x2);
      } else {
        v->fields.and_u8(0x1b2, 0xfd);
      }
      result = true; break;
    }
    break;
  case _assault_arming_shld_multiplier:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 5) {
        v->fields.set_u8(0x1e0, value);
      } else {
        v->fields.set_u8(0x1e0, 0);
      }
      result = true; break;
    }
    break;
  case _assault_arming_shld_recharge_rate:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 9) {
        v->fields.set_u8(0x1dd, value);
      } else {
        v->fields.set_u8(0x1dd, 0);
      }
      result = true; break;
    }
    break;
  case _assault_arming_vampirism:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 5) {
        v->fields.set_u8(0x1de, value);
      } else {
        v->fields.set_u8(0x1de, 0);
      }
      result = true; break;
    }
    break;
  case _assault_arming_dmg_modifier_percentage:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 12) {
        v->fields.set_u8(0x1e8, value);
      } else {
        v->fields.set_u8(0x1e8, 0);
      }
      result = true; break;
    }
    break;
  case _assault_arming_initial_primary_weapon:
    if (parameter_type == _text_value_pair_parameter_string_id) {
      c_game_engine_variant *v = this->writeable();
      ((c_player_trait_weapons *)v->fields.addr(0x1e4))->set_initial_primary_weapon(value, true);
      result = true; break;
    }
    break;
  case _assault_arming_initial_secondary_weapon:
    if (parameter_type == _text_value_pair_parameter_string_id) {
      c_game_engine_variant *v = this->writeable();
      ((c_player_trait_weapons *)v->fields.addr(0x1e4))->set_initial_secondary_weapon(value, true);
      result = true; break;
    }
    break;
  case _assault_arming_initial_grenade_count:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 2) {
        v->fields.set_u16(0x1e4, value);
      } else {
        v->fields.set_u16(0x1e4, 0);
      }
      result = true; break;
    }
    break;
  case _assault_arming_recharging_grenades:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 2) {
        v->fields.set_u8(0x1e9, value);
      } else {
        v->fields.set_u8(0x1e9, 0);
      }
      result = true; break;
    }
    break;
  case _assault_arming_infinite_ammo:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 2) {
        v->fields.set_u8(0x1ea, value);
      } else {
        v->fields.set_u8(0x1ea, 0);
      }
      result = true; break;
    }
    break;
  case _assault_arming_weapon_pickup:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 2) {
        v->fields.set_u8(0x1eb, value);
      } else {
        v->fields.set_u8(0x1eb, 0);
      }
      result = true; break;
    }
    break;
  case _assault_arming_walking_speed:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 10) {
        v->fields.set_u8(0x1ec, value);
      } else {
        v->fields.set_u8(0x1ec, 0);
      }
      result = true; break;
    }
    break;
  case _assault_arming_personal_gravity:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 5) {
        v->fields.set_u8(0x1ed, value);
      } else {
        v->fields.set_u8(0x1ed, 0);
      }
      result = true; break;
    }
    break;
  case _assault_arming_vehicle_use:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 3) {
        v->fields.set_u8(0x1ee, value);
      } else {
        v->fields.set_u8(0x1ee, 0);
      }
      result = true; break;
    }
    break;
  case _assault_arming_motion_tracker:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 4) {
        v->fields.set_u16(0x1f4, value);
      } else {
        v->fields.set_u16(0x1f4, 0);
      }
      result = true; break;
    }
    break;
  case _assault_arming_motion_tracker_range:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 7) {
        v->fields.set_u16(0x1f6, value);
      } else {
        v->fields.set_u16(0x1f6, 0);
      }
      result = true; break;
    }
    break;
  case _assault_arming_active_camo:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 4) {
        v->fields.set_u8(0x1f0, value);
      } else {
        v->fields.set_u8(0x1f0, 0);
      }
      result = true; break;
    }
    break;
  case _assault_arming_waypoint:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 3) {
        v->fields.set_u8(0x1f1, value);
      } else {
        v->fields.set_u8(0x1f1, 0);
      }
      result = true; break;
    }
    break;
  case _assault_arming_visual:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 4) {
        v->fields.set_u8(0x1f2, value);
      } else {
        v->fields.set_u8(0x1f2, 0);
      }
      result = true; break;
    }
    break;
  case _assault_arming_forced_change_colors:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 13) {
        v->fields.set_u8(0x1f3, value);
      } else {
        v->fields.set_u8(0x1f3, 0);
      }
      result = true; break;
    }
    break;
  case _assault_bomb_arming_time:
    if (parameter_type == _text_value_pair_parameter_integer) {
      short x = (short)value;
      c_game_engine_variant *v = this->writeable();
      if (x < 0 || x > 30) {
        x = 5;
      }
      v->fields.set_s16(0x1ba, x);
      result = true; break;
    }
    break;
  case _assault_bomb_disarming_time:
    if (parameter_type == _text_value_pair_parameter_integer) {
      short x = (short)value;
      c_game_engine_variant *v = this->writeable();
      if (x < 0 || x > 30) {
        x = 5;
      }
      v->fields.set_s16(0x1bc, x);
      result = true; break;
    }
    break;
  case _assault_bomb_fuse_time:
    if (parameter_type == _text_value_pair_parameter_integer) {
      short x = (short)value;
      c_game_engine_variant *v = this->writeable();
      if (x < 0 || x > 30) {
        x = 5;
      }
      v->fields.set_s16(0x1be, x);
      result = true; break;
    }
    break;
  case _assault_bomb_reset_time:
    if (parameter_type == _text_value_pair_parameter_integer) {
      short x = (short)value;
      c_game_engine_variant *v = this->writeable();
      if (x < 0 || x > 60) {
        x = 30;
      }
      v->fields.set_s16(0x1b8, x);
      result = true; break;
    }
    break;
  case _assault_carrier_shld_multiplier:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 5) {
        v->fields.set_u8(0x1c4, value);
      } else {
        v->fields.set_u8(0x1c4, 0);
      }
      result = true; break;
    }
    break;
  case _assault_carrier_shld_recharge_rate:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 9) {
        v->fields.set_u8(0x1c1, value);
      } else {
        v->fields.set_u8(0x1c1, 0);
      }
      result = true; break;
    }
    break;
  case _assault_carrier_vampirism:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 5) {
        v->fields.set_u8(0x1c2, value);
      } else {
        v->fields.set_u8(0x1c2, 0);
      }
      result = true; break;
    }
    break;
  case _assault_carrier_dmg_modifier_percentage:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 12) {
        v->fields.set_u8(0x1cc, value);
      } else {
        v->fields.set_u8(0x1cc, 0);
      }
      result = true; break;
    }
    break;
  case _assault_carrier_initial_primary_weapon:
    if (parameter_type == _text_value_pair_parameter_string_id) {
      c_game_engine_variant *v = this->writeable();
      ((c_player_trait_weapons *)v->fields.addr(0x1c8))->set_initial_primary_weapon(value, true);
      result = true; break;
    }
    break;
  case _assault_carrier_initial_secondary_weapon:
    if (parameter_type == _text_value_pair_parameter_string_id) {
      c_game_engine_variant *v = this->writeable();
      ((c_player_trait_weapons *)v->fields.addr(0x1c8))->set_initial_secondary_weapon(value, true);
      result = true; break;
    }
    break;
  case _assault_carrier_initial_grenade_count:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 2) {
        v->fields.set_u16(0x1c8, value);
      } else {
        v->fields.set_u16(0x1c8, 0);
      }
      result = true; break;
    }
    break;
  case _assault_carrier_recharging_grenades:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 2) {
        v->fields.set_u8(0x1cd, value);
      } else {
        v->fields.set_u8(0x1cd, 0);
      }
      result = true; break;
    }
    break;
  case _assault_carrier_infinite_ammo:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 2) {
        v->fields.set_u8(0x1ce, value);
      } else {
        v->fields.set_u8(0x1ce, 0);
      }
      result = true; break;
    }
    break;
  case _assault_carrier_weapon_pickup:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 2) {
        v->fields.set_u8(0x1cf, value);
      } else {
        v->fields.set_u8(0x1cf, 0);
      }
      result = true; break;
    }
    break;
  case _assault_carrier_walking_speed:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 10) {
        v->fields.set_u8(0x1d0, value);
      } else {
        v->fields.set_u8(0x1d0, 0);
      }
      result = true; break;
    }
    break;
  case _assault_carrier_personal_gravity:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 5) {
        v->fields.set_u8(0x1d1, value);
      } else {
        v->fields.set_u8(0x1d1, 0);
      }
      result = true; break;
    }
    break;
  case _assault_carrier_vehicle_use:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 3) {
        v->fields.set_u8(0x1d2, value);
      } else {
        v->fields.set_u8(0x1d2, 0);
      }
      result = true; break;
    }
    break;
  case _assault_carrier_motion_tracker:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 4) {
        v->fields.set_u16(0x1d8, value);
      } else {
        v->fields.set_u16(0x1d8, 0);
      }
      result = true; break;
    }
    break;
  case _assault_carrier_motion_tracker_range:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 7) {
        v->fields.set_u16(0x1da, value);
      } else {
        v->fields.set_u16(0x1da, 0);
      }
      result = true; break;
    }
    break;
  case _assault_carrier_active_camo:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 4) {
        v->fields.set_u8(0x1d4, value);
      } else {
        v->fields.set_u8(0x1d4, 0);
      }
      result = true; break;
    }
    break;
  case _assault_carrier_waypoint:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 3) {
        v->fields.set_u8(0x1d5, value);
      } else {
        v->fields.set_u8(0x1d5, 0);
      }
      result = true; break;
    }
    break;
  case _assault_carrier_visual:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 4) {
        v->fields.set_u8(0x1d6, value);
      } else {
        v->fields.set_u8(0x1d6, 0);
      }
      result = true; break;
    }
    break;
  case _assault_carrier_forced_change_colors:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 13) {
        v->fields.set_u8(0x1d7, value);
      } else {
        v->fields.set_u8(0x1d7, 0);
      }
      result = true; break;
    }
    break;
  case _assault_enemy_bomb_waypoint:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value <= 3) {
        v->fields.set_u16(0x1b2, value);
      } else {
        v->fields.set_u16(0x1b2, 1);
      }
      result = true; break;
    }
    break;
  case _assault_game_type:
    if (parameter_type == _text_value_pair_parameter_integer) {
      c_game_engine_variant *v = this->writeable();
      if ((unsigned)value > 2) {
        v->fields.set_u16(0x1b0, 0);
      } else {
        v->fields.set_u16(0x1b0, value);
      }
      result = true; break;
    }
    break;
  case _assault_score_to_win:
    if (parameter_type == _text_value_pair_parameter_integer) {
      short x = (short)value;
      c_game_engine_variant *v = this->writeable();
      if (x < 0 || x > 50) {
        x = 3;
      }
      v->fields.set_s16(0x1b4, x);
      result = true; break;
    }
    break;
  case _assault_sudden_death_time:
    if (parameter_type == _text_value_pair_parameter_integer) {
      short x = (short)value;
      c_game_engine_variant *v = this->writeable();
      if (x >= -1 && x <= 300) {
        v->fields.set_s16(0x1b6, x);
      } else {
        v->fields.set_s16(0x1b6, 0);
      }
      result = true; break;
    }
    break;
  case _sandbox_open_channel_voice:
    if (parameter_type == _text_value_pair_parameter_integer) {
      ((c_game_engine_sandbox_variant *)variant_storage)->m_variant_flags.set(c_game_engine_sandbox_variant::_sandbox_variant_open_channel_voice, value != 0);
      result = true; break;
    }
    break;
  case _sandbox_enter_editor_mode:
    if (parameter_type == _text_value_pair_parameter_integer) {
      if ((unsigned)value <= 1) {
        U8(this, 0x1b5) = value;
      } else {
        U8(this, 0x1b5) = 0;
      }
      result = true; break;
    }
    break;
  case _sandbox_respawn_time:
    if (parameter_type == _text_value_pair_parameter_integer) {
      short x = (short)value;
      if (x < 0 || x > 60) {
        x = 5;
      }
      ((c_game_engine_sandbox_variant *)variant_storage)->m_respawn_time = x;
      result = true; break;
    }
    break;
  case _sandbox_editor_shld_dmg_resistance:
    if (parameter_type == _text_value_pair_parameter_integer) {
      if ((unsigned)value <= 12) {
        U8(this, 0x1b8) = value;
      } else {
        U8(this, 0x1b8) = 0;
      }
      result = true; break;
    }
    break;
  case _sandbox_editor_shld_multiplier:
    if (parameter_type == _text_value_pair_parameter_integer) {
      if ((unsigned)value <= 5) {
        U8(this, 0x1bc) = value;
      } else {
        U8(this, 0x1bc) = 0;
      }
      result = true; break;
    }
    break;
  case _sandbox_editor_shld_recharge_rate:
    if (parameter_type == _text_value_pair_parameter_integer) {
      if ((unsigned)value <= 9) {
        U8(this, 0x1b9) = value;
      } else {
        U8(this, 0x1b9) = 0;
      }
      result = true; break;
    }
    break;
  case _sandbox_editor_headshot_immunity:
    if (parameter_type == _text_value_pair_parameter_integer) {
      if ((unsigned)value <= 2) {
        U8(this, 0x1bb) = value;
      } else {
        U8(this, 0x1bb) = 0;
      }
      result = true; break;
    }
    break;
  case _sandbox_editor_vampirism:
    if (parameter_type == _text_value_pair_parameter_integer) {
      if ((unsigned)value <= 5) {
        U8(this, 0x1ba) = value;
      } else {
        U8(this, 0x1ba) = 0;
      }
      result = true; break;
    }
    break;
  case _sandbox_editor_dmg_modifier_percentage:
    if (parameter_type == _text_value_pair_parameter_integer) {
      if ((unsigned)value <= 12) {
        U8(this, 0x1c4) = value;
      } else {
        U8(this, 0x1c4) = 0;
      }
      result = true; break;
    }
    break;
  case _sandbox_editor_initial_primary_weapon:
    if (parameter_type == _text_value_pair_parameter_string_id) {
      ((c_game_engine_sandbox_variant *)variant_storage)->m_player_traits.get_weapons_traits_writeable()->set_initial_primary_weapon(value, true);
      result = true; break;
    }
    break;
  case _sandbox_editor_initial_secondary_weapon:
    if (parameter_type == _text_value_pair_parameter_string_id) {
      ((c_game_engine_sandbox_variant *)variant_storage)->m_player_traits.get_weapons_traits_writeable()->set_initial_secondary_weapon(value, true);
      result = true; break;
    }
    break;
  case _sandbox_editor_initial_grenade_count:
    if (parameter_type == _text_value_pair_parameter_integer) {
      if ((unsigned)value <= 2) {
        U16(this, 0x1c0) = value;
      } else {
        U16(this, 0x1c0) = 0;
      }
      result = true; break;
    }
    break;
  case _sandbox_editor_recharging_grenades:
    if (parameter_type == _text_value_pair_parameter_integer) {
      if ((unsigned)value <= 2) {
        U8(this, 0x1c5) = value;
      } else {
        U8(this, 0x1c5) = 0;
      }
      result = true; break;
    }
    break;
  case _sandbox_editor_infinite_ammo:
    if (parameter_type == _text_value_pair_parameter_integer) {
      if ((unsigned)value <= 2) {
        U8(this, 0x1c6) = value;
      } else {
        U8(this, 0x1c6) = 0;
      }
      result = true; break;
    }
    break;
  case _sandbox_editor_weapon_pickup:
    if (parameter_type == _text_value_pair_parameter_integer) {
      if ((unsigned)value <= 2) {
        U8(this, 0x1c7) = value;
      } else {
        U8(this, 0x1c7) = 0;
      }
      result = true; break;
    }
    break;
  case _sandbox_editor_walking_speed:
    if (parameter_type == _text_value_pair_parameter_integer) {
      if ((unsigned)value <= 10) {
        U8(this, 0x1c8) = value;
      } else {
        U8(this, 0x1c8) = 0;
      }
      result = true; break;
    }
    break;
  case _sandbox_editor_personal_gravity:
    if (parameter_type == _text_value_pair_parameter_integer) {
      if ((unsigned)value <= 5) {
        U8(this, 0x1c9) = value;
      } else {
        U8(this, 0x1c9) = 0;
      }
      result = true; break;
    }
    break;
  case _sandbox_editor_vehicle_use:
    if (parameter_type == _text_value_pair_parameter_integer) {
      if ((unsigned)value <= 3) {
        U8(this, 0x1ca) = value;
      } else {
        U8(this, 0x1ca) = 0;
      }
      result = true; break;
    }
    break;
  case _sandbox_editor_motion_tracker:
    if (parameter_type == _text_value_pair_parameter_integer) {
      if ((unsigned)value <= 4) {
        U16(this, 0x1d0) = value;
      } else {
        U16(this, 0x1d0) = 0;
      }
      result = true; break;
    }
    break;
  case _sandbox_editor_motion_tracker_range:
    if (parameter_type == _text_value_pair_parameter_integer) {
      if ((unsigned)value <= 7) {
        U16(this, 0x1d2) = value;
      } else {
        U16(this, 0x1d2) = 0;
      }
      result = true; break;
    }
    break;
  case _sandbox_editor_active_camo:
    if (parameter_type == _text_value_pair_parameter_integer) {
      if ((unsigned)value <= 4) {
        U8(this, 0x1cc) = value;
      } else {
        U8(this, 0x1cc) = 0;
      }
      result = true; break;
    }
    break;
  case _sandbox_editor_waypoint:
    if (parameter_type == _text_value_pair_parameter_integer) {
      if ((unsigned)value <= 3) {
        U8(this, 0x1cd) = value;
      } else {
        U8(this, 0x1cd) = 0;
      }
      result = true; break;
    }
    break;
  case _sandbox_editor_visual:
    if (parameter_type == _text_value_pair_parameter_integer) {
      if ((unsigned)value <= 4) {
        U8(this, 0x1ce) = value;
      } else {
        U8(this, 0x1ce) = 0;
      }
      result = true; break;
    }
    break;
  case _sandbox_editor_forced_change_colors:
    if (parameter_type == _text_value_pair_parameter_integer) {
      if ((unsigned)value <= 13) {
        U8(this, 0x1cf) = value;
      } else {
        U8(this, 0x1cf) = 0;
      }
      result = true; break;
    }
    break;
  }
  return result;
}

// 0x1c2ac0
bool c_game_variant::is_equal_to(const c_game_variant *other) const
{
    bool equal = true;
    equal &= get_game_engine_index() == other->get_game_engine_index();
    if (equal)
    {
        long variant_size = game_variant_get_size(this, get_game_engine_index());
#ifdef HALO3_64BIT
        equal &= memcmp(&variant_storage[sizeof(void *)], &other->variant_storage[sizeof(void *)], variant_size - (long)sizeof(void *)) == 0;
#else
        equal &= memcmp(&variant_storage[4], &other->variant_storage[4], variant_size - 4) == 0;
#endif
    }
    return equal;
}

// 0x1c2b68
void c_game_variant::recreate_variant_vtable_for_game_engine_index(e_game_engine_type game_engine_index)
{
    if (game_engine_index >= _game_engine_base_variant && game_engine_index < k_game_engine_type_count)
        engine_type = game_engine_index;
    else
        engine_type = _game_engine_base_variant;
    switch (engine_type)
    {
    case _game_engine_base_variant:
        new (variant_storage) c_game_engine_base_variant();
        break;
    case _game_engine_ctf:
        new (variant_storage) c_game_engine_ctf_variant();
        break;
    case _game_engine_slayer:
        new (variant_storage) c_game_engine_slayer_variant();
        break;
    case _game_engine_oddball:
        new (variant_storage) c_game_engine_oddball_variant();
        break;
    case _game_engine_king:
        new (variant_storage) c_game_engine_king_variant();
        break;
    case _game_engine_infection:
        new (variant_storage) c_game_engine_infection_variant();
        break;
    case _game_engine_territories:
        new (variant_storage) c_game_engine_territories_variant();
        break;
    case _game_engine_assault:
        new (variant_storage) c_game_engine_assault_variant();
        break;
    case _game_engine_vip:
        new (variant_storage) c_game_engine_vip_variant();
        break;
    case _game_engine_juggernaut:
        new (variant_storage) c_game_engine_juggernaut_variant();
        break;
    case _game_engine_sandbox:
        new (variant_storage) c_game_engine_sandbox_variant();
        break;
    default:
        // match: 0x1c2b68: cases share one vtable-store epilogue via beq; removal adds a cmplwi 10 guard with beqlr returns
        __assume(0);
    }
}

// @address 0x1c2950 game_variant_set_from
extern "C" void game_variant_set_from(c_game_variant *variant, const c_game_variant *source)
{
    variant->recreate_variant_vtable_for_game_engine_index((e_game_engine_type)source->engine_type);
    long size = game_variant_get_size(variant, source->engine_type);
#ifdef HALO3_64BIT
    memcpy(variant->variant_storage + sizeof(void *), source->variant_storage + sizeof(void *), size - (long)sizeof(void *));
#else
    memcpy(variant->variant_storage + 4, source->variant_storage + 4, size - 4);
#endif
    if ((unsigned long)size < sizeof(variant->variant_storage))
        memset(variant->variant_storage + size, 0, sizeof(variant->variant_storage) - size);
}
void c_game_variant::set_game_engine_index(e_game_engine_type engine_type)
{
    memset(variant_storage, 0, sizeof(variant_storage));
    recreate_variant_vtable_for_game_engine_index(engine_type);
    get_active_variant_writeable()->initialize();
}

// 0x1c29c8
void c_game_variant::copy_from_and_validate(const c_game_variant *source)
{
    if (source->get_game_engine_index() >= _game_engine_ctf && source->get_game_engine_index() <= _game_engine_infection)
        game_variant_set_from(this, source);
    else
        set_game_engine_index(_game_engine_base_variant);
    bool built_in = get_active_variant()->get_built_in();
    switch (get_game_engine_index())
    {
    case _game_engine_base_variant:
        get_active_variant_writeable()->validate();
        break;
    case _game_engine_ctf:
        get_ctf_variant_writeable()->validate();
        break;
    case _game_engine_slayer:
        get_slayer_variant_writeable()->validate();
        break;
    case _game_engine_oddball:
        get_oddball_variant_writeable()->validate();
        break;
    case _game_engine_king:
        get_king_variant_writeable()->validate();
        break;
    case _game_engine_infection:
        get_infection_variant_writeable()->validate();
        break;
    case _game_engine_territories:
        get_territories_variant_writeable()->validate();
        break;
    case _game_engine_assault:
        get_assault_variant_writeable()->validate();
        break;
    case _game_engine_vip:
        get_vip_variant_writeable()->validate();
        break;
    case _game_engine_juggernaut:
        get_juggernaut_variant_writeable()->validate();
        break;
    case _game_engine_sandbox:
        get_sandbox_variant_writeable()->validate();
        break;
    default:
        // match: 0x1c29c8: switch omits the cmplwi 10 range guard; removal adds it and swaps the r30/r31 allocation
        __assume(0);
    }
    get_active_variant_writeable()->set_built_in(built_in);
}

// 0x265878: the variant's metadata set from a copy, its strings terminated and zero-padded (the map variant's twin is
// saved_game_files.cpp map_variant_set_metadata)
// match: 0x265878: out-of-line memset; the aligned dests would otherwise inline where the image calls
#pragma function(memset)
// @address 0x265878 game_engine_variant_set_metadata
extern "C" void game_engine_variant_set_metadata(void *variant, const s_saved_game_item_metadata_packed *metadata)
{
    // Adapt the opaque C entry point to canonical storage; the packed
    // on-disk metadata shares its layout with the in-memory struct.
    c_game_engine_base_variant *base_variant = (c_game_engine_base_variant *)variant;
    memcpy(&base_variant->m_metadata, metadata, sizeof(*metadata));
    // The name and author scratch buffers are used sequentially, so they share one slot.
    union {
        wchar_t name[16];
        char author[16];
    } scratch;
    char description[128];
    ustrnzcpy_inline(scratch.name, base_variant->m_metadata.name, 16);
    memset(base_variant->m_metadata.name, 0, sizeof(metadata->name));
    ustrnzcpy_inline(base_variant->m_metadata.name, scratch.name, 16);
    csstrnzcpy(description, base_variant->m_metadata.description, 128);
    memset(base_variant->m_metadata.description, 0, sizeof(metadata->description));
    csstrnzcpy(base_variant->m_metadata.description, description, 128);
    csstrnzcpy(scratch.author, base_variant->m_metadata.author, 16);
    memset(base_variant->m_metadata.author, 0, sizeof(metadata->author));
    csstrnzcpy(base_variant->m_metadata.author, scratch.author, 16);
    base_variant->m_metadata.unique_id = 0;
    base_variant->set_built_in(false);
}
// match: 0x265878: restore intrinsic memset after the out-of-line region above
#pragma intrinsic(memset)

// @address 0x1c27c0 game_variant_compute_unique_id
extern "C" unsigned __int64 game_variant_compute_unique_id(const c_game_variant *variant)
{
    s_network_http_request_hash hash;
    s_saved_game_item_metadata_packed metadata;
    const char *data;
    if (variant->get_active_variant()->get_built_in())
    {
        c_game_variant copy;
        game_variant_set_from(&copy, variant);
        memcpy(&metadata, &copy.get_active_variant()->m_metadata, sizeof(metadata));
        metadata.date = 0;
        game_engine_variant_set_metadata(copy.variant_storage, &metadata);
        data = copy.variant_storage + 4;
    }
    else
    {
        data = variant->variant_storage + 4;
    }
    security_calculate_hash(data, sizeof(variant->variant_storage) - 4, false, &hash);
    return *(const unsigned __int64 *)hash.hash;
}
