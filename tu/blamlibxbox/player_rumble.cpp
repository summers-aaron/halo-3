// blamlibxbox/player_rumble.obj: from the records 0x1372c0 0x217060 0x217070 0x2170b8 0x217140 0x217148 0x217230 0x2172e0 0x217568 0x217608 0x217620 0x707f60
// @compiler xdk6130
// @flags /TP /O2 /Oc
#include "shell/shell.h"
#include "effects/player_effect_tls.h"
#include "math/real_math_types.h"
#include "tag_files/tag_groups.h"
#include "multithreading/thread_tls.h"
#include "input/input_globals.h"
#include "interface/chud/chud.h"
#include "interface/chud/chud_dispose_state_types.h"
#include "cache/restricted_memory_registration.h"
#include "cache/restricted_memory_regions_inline.h"
#include <string.h>
// @address 0x2170b8 ?rumble_initialize_for_new_map@@YAXXZ
// @address 0x217140 ?rumble_dispose_from_old_map@@YAXXZ
// @address 0x217148 ?rumble_player_impulse@@YAXJJJM@Z
// @address 0x217230 ?rumble_clear_all_now@@YAXXZ
// @address 0x58e5c0 memset
#include <ppcintrinsics.h>
#include <math.h>
#include <stdlib.h>
#include "memory/data.h"
#include "cache/restricted_memory_regions.h"
#include "game/game.h"
#include "game/player_rumble.h"
#include "interface/user_interface_controller.h"
#include "simulation/simulation.h"
#include "effects/rumble_entry_types.h"
#include "effects/rumble_globals_types.h"
#include "effects/rumble_player_types.h"
#include "cache/cache_files.h"
#include "math/function_definitions.h"
#include "objects/damage_response_definition.h"
#include "interface/chud/cortana_effect_definition.h"
#include "cache/restricted_memory_layout.h"
#include "memory/tls_globals.h"
#include "game/player_mapping_types.h"

struct s_rumble_entry_data
{
    long tag_index;
    long function_index;
    float intensity;
};
struct player_rumble_datum
{
    s_rumble_entry_data entries[8];
    float scales[8];
    real_point2d target;
};
struct rumble_data
{
    unsigned short x;
    unsigned short y;
};

class c_rumble_globals_allocator : public c_allocation_base
{
public:
    c_rumble_globals_allocator() : m_member_index(-1), m_owner(-1) {}
    ~c_rumble_globals_allocator() {}
    virtual void *allocate(unsigned int size, const char *name);
    virtual void deallocate(void *address);
    void *reserve_memory(const char *name, const char *type, unsigned int size, long alignment_bits);
    int m_member_index;
    int m_owner;
};

// @address 0x217060 __tls_set_g_rumble_globals_allocator
void __tls_set_g_rumble_globals_allocator(void *value)
{
    g_rumble_globals = (s_rumble_globals *)value;
}

void rumble_initialize(void)
{
    g_rumble_globals = (s_rumble_globals *)g_rumble_globals_allocator.reserve_memory("rumble", 0, sizeof(s_rumble_globals), 0);
}

// @address 0x5bec98 ?rumble_dispose@@YAXXZ
void rumble_dispose(void)
{
}

void rumble_initialize_for_new_map(void)
{
    s_rumble_globals *rumble_globals = g_rumble_globals;
    memset(rumble_globals, 0, sizeof(s_rumble_globals));
    float initial_value = 1.0f;
    int player_index = 0;
    do {
        int entry_index = 0;
        do {
            s_rumble_entry *entry = &rumble_globals->players[player_index].entries[entry_index];
            entry->rumble_index = -1;
            entry->rumble_type = -1;
            entry->intensity = initial_value;
            entry_index++;
        } while (entry_index < 8);
        player_index++;
    } while (player_index < 4);
}

void rumble_dispose_from_old_map(void)
{
    rumble_clear_all_now();
}

void rumble_player_impulse(long player_index, long rumble_index, long rumble_type, float intensity)
{
    if (rumble_index != -1 && rumble_type != -1) {
        s_rumble_player *player = (s_rumble_player *)g_rumble_globals + player_index;
        int slot = 0;
        float best = player->priorities[0];
        for (int i = 1; i < 8; i++) {
            if (player->priorities[i] > best) {
                slot = i;
                best = player->priorities[i];
            }
        }
        player->entries[slot].rumble_index = rumble_index;
        player->entries[slot].rumble_type = rumble_type;
        player->entries[slot].intensity = intensity;
        player->priorities[slot] = 0.0f;
    }
}

void rumble_clear_all_now(void)
{
    s_rumble_globals *rumble_globals;
    int player_index;
    int entry_index;
    s_rumble_entry *entries;
    float *cursor;
    float intensity;
    rumble_globals = g_rumble_globals;
    memset(rumble_globals, 0, sizeof(s_rumble_globals));
    intensity = 1.0f;
    player_index = 0;
    do {
        entries = rumble_globals->players[player_index].entries;
        cursor = &entries->intensity;
        entry_index = 0;
        do {
            *cursor = intensity;
            ((int *)cursor)[-2] = -1;
            ((int *)cursor)[-1] = -1;
            cursor += 3;
            entry_index++;
        } while (entry_index < 8);
        player_index++;
    } while (player_index < 4);
    input_globals.rumble[0][0] = 0;
    input_globals.rumble[0][1] = 0;
    input_globals.rumble[1][0] = 0;
    input_globals.rumble[1][1] = 0;
    input_globals.rumble[2][0] = 0;
    input_globals.rumble[2][1] = 0;
    input_globals.rumble[3][0] = 0;
    input_globals.rumble[3][1] = 0;
}

// @address 0x2172e0 ?rumble_calculate@@YA?AUrumble_data@@PAUplayer_rumble_datum@@@Z
rumble_data rumble_calculate(player_rumble_datum *datum)
{
    real_point2d position;
    position.x = datum->target.x;
    position.y = datum->target.y;
    for (long i = 0; i < 8; i++) {
        if (datum->entries[i].tag_index == -1)
            continue;
        if (datum->entries[i].function_index == -1)
            continue;
        float scale = datum->scales[i];
        s_damage_response_definition *definition =
            (s_damage_response_definition *)tag_get('drdf', datum->entries[i].tag_index);
        if (datum->entries[i].function_index < 0
            || datum->entries[i].function_index >= definition->classes.count())
            continue;
        s_damage_response_class *element = &definition->classes[datum->entries[i].function_index];
        s_damage_response_rumble_frequency *tracks = element->tracks;
        float *accumulator = &position.x;
        for (long j = 0; j < 2; j++) {
            if (tracks[j].range > scale) {
                accumulator[j] += tracks[j].function.evaluate_legacy(
                                      PIN(scale / tracks[j].range,
                                                 0.0f, 1.0f),
                                      1.0f)
                                  * datum->entries[i].intensity;
            }
        }
    }

    s_rumble_globals *globals = (s_rumble_globals *)g_rumble_globals;
    float x = 0.0f;
    float y = 0.0f;
    float scripted_scale = globals->scripted_scale;
    if (scripted_scale != 0.0f) {
        x = globals->scripted_left * scripted_scale + position.x;
        y = globals->scripted_right * scripted_scale + position.y;
    } else {
        y = position.y;
        x = position.x;
    }

    chud_dispose_state *cinematic = chud_cinematic_state();
    long cinematic_index = cinematic->index;
    if (cinematic_index != -1) {
        s_cortana_effect_definition *definition =
            (s_cortana_effect_definition *)tag_get('crte', cinematic_index);
        float add_x = 0.0f;
        float add_y = 0.0f;
        if (definition->postprocessing.count() != 0) {
            s_cortana_effect_postprocessing *effect = definition->postprocessing.begin();
            if (effect != 0 && effect->rumble.count != 0) {
                real_point3d offset;
                chud_compute_follower_value_2d((s_chud_curve_follower_2d *)effect->rumble.address, cinematic->ticks, &position, &offset, &offset);
                add_x = position.x;
                add_y = position.y;
            }
        }
        x = add_x + x;
        y = add_y + y;
    }

    rumble_data result;
    x = PIN(x * 65535.0f, 0.0f, 65535.0f);
    result.x = (unsigned short)(long)(x + 0.5f);
    y = PIN(y * 65535.0f, 0.0f, 65535.0f);
    result.y = (unsigned short)(long)(y + 0.5f);
    return result;
}

// Registers a member of field layout 1 (see restricted_memory_register_member) and publishes its
// per-thread address. One of a family of identical allocators that differ
// only in the layout index, the update callback and the thread-local pointer.

// @address 0x217568 ?reserve_memory@c_rumble_globals_allocator@@QAAPAXPBD0IJ@Z
void *c_rumble_globals_allocator::reserve_memory(const char *name, const char *type, unsigned int size, long alignment_bits)
{
  int member_index = restricted_memory_register_member(&g_restricted_regions[1], name, type, size, 0, __tls_set_g_rumble_globals_allocator, 0, 0);
  unsigned int offset = restricted_region_get_member_offset(1, member_index);
  void *address = restricted_region_get_base_address(1) + offset;
  int owner = get_current_thread_index();
  m_owner = owner;
  m_member_index = member_index;
  g_rumble_globals = (s_rumble_globals *)address;
  return restricted_region_get_base_address(1) + restricted_region_get_member_offset(1, member_index);
}

// @address 0x217608 ?allocate@c_rumble_globals_allocator@@UAAPAXIPBD@Z
void *c_rumble_globals_allocator::allocate(unsigned int size, const char *name)
{
    return reserve_memory(name, "", size, 0);
}

// @address 0x1d6bb0 ?restricted_region_free_member@@YAXJJ@Z
// @address 0x217620 ?deallocate@c_rumble_globals_allocator@@UAAXPAX@Z
void c_rumble_globals_allocator::deallocate(void *address)
{
    int value = m_member_index;
    g_rumble_globals = 0;
    restricted_region_free_member(1, value);
}

// @address 0x11df79c g_rumble_globals
__declspec(thread) s_rumble_globals *g_rumble_globals;

c_rumble_globals_allocator g_rumble_globals_allocator;


static __forceinline player_rumble_datum *player_rumble_get(long user)
{
    int _salloc_slab0 = 0;
    int _salloc_slab1 = 0;
    int _salloc_slab2 = 0;
    int _salloc_slab3 = 0;
    int _salloc_slab4 = 0;
    int _salloc_slab5 = 0;
    int _salloc_slab6 = 0;
    int _salloc_slab7 = 0;
    int _salloc_slab8 = 0;
    int _salloc_slab9 = 0;
    int _salloc_slab10 = 0;
    int _salloc_slab11 = 0;
    int _salloc_slab12 = 0;
    int _salloc_slab13 = 0;
    int _salloc_slab14 = 0;
    int _salloc_slab15 = 0;
    int _salloc_slab16 = 0;
    int _salloc_slab17 = 0;
    int _salloc_slab18 = 0;
    int _salloc_slab19 = 0;
    int _salloc_slab20 = 0;
    int _salloc_slab21 = 0;
    int _salloc_slab22 = 0;
    int _salloc_slab23 = 0;
    int _salloc_slab24 = 0;
    int _salloc_slab25 = 0;
    int _salloc_slab26 = 0;
    int _salloc_slab27 = 0;
    int _salloc_slab28 = 0;
    int _salloc_slab29 = 0;
    int _salloc_slab30 = 0;
    int _salloc_slab31 = 0;
    int _salloc_slab32 = 0;
    int _salloc_slab33 = 0;
    int _salloc_slab34 = 0;
    int _salloc_slab35 = 0;
    int _salloc_slab36 = 0;
    int _salloc_slab37 = 0;
    int _salloc_slab38 = 0;
    int _salloc_slab39 = 0;
    int _salloc_slab40 = 0;
    int _salloc_slab41 = 0;
    int _salloc_slab42 = 0;
    int _salloc_slab43 = 0;
    int _salloc_slab44 = 0;
    int _salloc_slab45 = 0;
    int _salloc_slab46 = 0;
    int _salloc_slab47 = 0;
    int _salloc_slab48 = 0;
    int _salloc_slab49 = 0;
    int _salloc_slab50 = 0;
    int _salloc_slab51 = 0;
    int _salloc_slab52 = 0;
    int _salloc_slab53 = 0;
    int _salloc_slab54 = 0;
    int _salloc_slab55 = 0;
    int _salloc_slab56 = 0;
    int _salloc_slab57 = 0;
    int _salloc_slab58 = 0;
    int _salloc_slab59 = 0;
    int _salloc_slab60 = 0;
    int _salloc_slab61 = 0;
    int _salloc_slab62 = 0;
    int _salloc_slab63 = 0;
    int _salloc_slab64 = 0;
    int _salloc_slab65 = 0;
    int _salloc_slab66 = 0;
    int _salloc_slab67 = 0;
    int _salloc_slab68 = 0;
    int _salloc_slab69 = 0;
    int _salloc_slab70 = 0;
    int _salloc_slab71 = 0;
    int _salloc_slab72 = 0;
    int _salloc_slab73 = 0;
    int _salloc_slab74 = 0;
    int _salloc_slab75 = 0;
    int _salloc_slab76 = 0;
    int _salloc_slab77 = 0;
    int _salloc_slab78 = 0;
    int _salloc_slab79 = 0;
    int _salloc_slab80 = 0;
    int _salloc_slab81 = 0;
    int _salloc_slab82 = 0;
    int _salloc_slab83 = 0;
    int _salloc_slab84 = 0;
    int _salloc_slab85 = 0;
    int _salloc_slab86 = 0;
    int _salloc_slab87 = 0;
    int _salloc_slab88 = 0;
    int _salloc_slab89 = 0;
    int _salloc_slab90 = 0;
    int _salloc_slab91 = 0;
    int _salloc_slab92 = 0;
    int _salloc_slab93 = 0;
    int _salloc_slab94 = 0;
    int _salloc_slab95 = 0;
    int _salloc_slab96 = 0;
    int _salloc_slab97 = 0;
    int _salloc_slab98 = 0;
    int _salloc_slab99 = 0;
    int _salloc_slab100 = 0;
    int _salloc_slab101 = 0;
    int _salloc_slab102 = 0;
    int _salloc_slab103 = 0;
    int _salloc_slab104 = 0;
    int _salloc_slab105 = 0;
    int _salloc_slab106 = 0;
    int _salloc_slab107 = 0;
    int _salloc_slab108 = 0;
    int _salloc_slab109 = 0;
    int _salloc_slab110 = 0;
    int _salloc_slab111 = 0;
    int _salloc_slab112 = 0;
    int _salloc_slab113 = 0;
    int _salloc_slab114 = 0;
    int _salloc_slab115 = 0;
    int _salloc_slab116 = 0;
    int _salloc_slab117 = 0;
    int _salloc_slab118 = 0;
    int _salloc_slab119 = 0;
    int _salloc_slab120 = 0;
    int _salloc_slab121 = 0;
    int _salloc_slab122 = 0;
    int _salloc_slab123 = 0;
    int _salloc_slab124 = 0;
    int _salloc_slab125 = 0;
    int _salloc_slab126 = 0;
    int _salloc_slab127 = 0;
    int _salloc_slab128 = 0;
    int _salloc_slab129 = 0;
    int _salloc_slab130 = 0;
    int _salloc_slab131 = 0;
    int _salloc_slab132 = 0;
    int _salloc_slab133 = 0;
    int _salloc_slab134 = 0;
    int _salloc_slab135 = 0;
    int _salloc_slab136 = 0;
    int _salloc_slab137 = 0;
    int _salloc_slab138 = 0;
    int _salloc_slab139 = 0;
    int _salloc_slab140 = 0;
    int _salloc_slab141 = 0;
    int _salloc_slab142 = 0;
    int _salloc_slab143 = 0;
    int _salloc_slab144 = 0;
    int _salloc_slab145 = 0;
    int _salloc_slab146 = 0;
    int _salloc_slab147 = 0;
    int _salloc_slab148 = 0;
    int _salloc_slab149 = 0;
    int _salloc_slab150 = 0;
    int _salloc_slab151 = 0;
    int _salloc_slab152 = 0;
    int _salloc_slab153 = 0;
    int _salloc_slab154 = 0;
    int _salloc_slab155 = 0;
    int _salloc_slab156 = 0;
    int _salloc_slab157 = 0;
    int _salloc_slab158 = 0;
    int _salloc_slab159 = 0;
    return (player_rumble_datum *)g_rumble_globals + user;
}
static __forceinline long player_rumble_player_index(long user)
{
    long player = -1;
    if (user != -1)
        player = tls_player_mapping_globals()->player_indices[user];
    return player;
}
static __forceinline void player_rumble_set_motors(s_input_globals *input, short controller, unsigned short left, unsigned short right)
{
    input->rumble[controller][0] = left;
    input->rumble[controller][1] = right;
}

// @address 0x1372c0 ?rumble_update@@YAXM@Z
void rumble_update(float elapsed_seconds)
{
    float *scales = 0;
    unsigned char updated[4] = {};
    if (!g_shell_application_paused && !simulation_is_pending_start() && !game_is_cooperative()) {
        long user = 0; if (user >= 0) for (; user < 4; ++user) {
            player_rumble_datum *datum = player_rumble_get(user);
            rumble_data output = rumble_calculate(datum);
            long player = player_rumble_player_index(user);
            scales = datum->scales;
            for (long effect = 0; effect < 8; ++effect)
                scales[effect] += elapsed_seconds;
            if (player != -1) {
                long controller = user_interface_controller_get_rumble_enabled(user);
                if (controller != -1) {
                    player_rumble_set_motors(&input_globals, (short)controller, output.x, output.y);
                    updated[controller] = true;
                }
            }
        }
    }
    long controller = 0; if (controller >= 0) for (; controller < 4; ++controller) {
        if (!updated[controller])
            player_rumble_set_motors(&input_globals, (short)controller, 0, 0);
    }
}
