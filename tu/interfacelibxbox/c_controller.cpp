// interfacelibxbox/c_controller.obj: from the records 0x89640 0x89910 0x89ae8 0x8a238 0x8a2c8 0x8a4c8 0x8a698 0x8ac48 0x8af98 0x8b050 0x8ea58 0x968c8 0x132060 0x47ba48 0x47bab8 0x47bb78 0x47bcf8 0x47bd98 0x47be10 0x47bf18 0x47c0d0 0x47c1c8 0x4a8ad0 0x4b09e0 0x4c1ec0 0x4c1f50 0x580b90 0x70a4c8 0x70cbe0
#include "interface/controller_runtime_storage.h"
#include "interface/user_interface_clock.h"
#include "shell/utility_drive.h"
// @address 0x806460 g_online_cached_users
// @address 0x8a1ead g_utility_drive_mounted
#include "interface/controller_marketplace_types.h"
#include "interface/user_interface_controller_globals.h"
#include "networking/online/online_cached_user.h"
// @flags /TP /O2
// @split_bss
#include "interface/controller_state_flags.h"
#include "interface/c_player_profile.h"
// @shared_bss 0x82806458..0x828086b8
// @shared_bss_zero_gaps
#include "cseries/cseries_wrapped.h"
#include "game/player_types.h"
#include "shell/shell.h"
#include "game/game_types.h"
#include "game/game_globals_storage.h"
#include "interface/user_interface_messages.h"
// @address 0x8ad68 ?saved_game_files_update_controller@@YAHPAUs_controller_update@@PAUc_controller_interface@@H@Z
// @address 0x8a698 ?is_guest@c_controller_interface@@QBA_NXZ
// @address 0x8b050 ?is_silver_or_gold_live@c_controller_interface@@QBA_NXZ
// @address 0x8ea58 controller_update_signin_cache
// @address 0x968c8 ?have_users_changed@c_signed_in_users_watcher@@QAA_NXZ
// @address 0x8a2c8 ?get_player_identifier@c_controller_interface@@QBAPBUs_player_identifier@@PAU2@@Z
// @address 0x8a4c8 ?is_signed_in_to_machine@c_controller_interface@@QBA_NXZ
// @address 0x132060 ?get_hash_bits@c_controller_interface@@QAA_KJ@Z
// @address 0x47ba48 ?controller_get_first_signed_in_controller@@YA?AW4e_controller_index@@XZ
// @address 0x47bd98 ?set_as_unsigned_in_user@c_controller_interface@@QAAX_N@Z
// @address 0x47bf18 ?sign_in_controller@c_controller_interface@@AAAXPBUs_player_identifier@@_N@Z
// @address 0x4a8ad0 controller_get_signed_in_player_identifiers
// @address 0x4c1ec0 controller_get_signed_in_mask1
// @address 0x4c1f50 controller_get_signed_in_mask
// @address 0x580b90 ?start@c_marketplace_request_consume_assets_task@@UAAKPAX@Z
#include <ppcintrinsics.h>
#include "memory/data.h"
#include "interface/user_interface_error_manager.h"
#include "networking/network_time_types.h"
#include "networking/session/network_session_interface_types.h"
#include "interface/controller_types.h"
// @address 0x11401e8 g_controller_globals
#include "input/input.h"
#include "input/input_globals.h"
#include "interface/c_signed_in_users_watcher.h"
#include "networking/online/voice_interface_types.h"
#include "cseries/data_mining_event_types.h"
#include "memory/data_types.h"
#include <xtl.h>
#include "memory/tls_globals.h"
#include "interface/c_controller.h"
#include "networking/logic/logic_managed_user.h"
#include "saved_games/saved_game_files.h"
#include "interface/achievement_globals_types.h"
#include "networking/online/online_globals_types.h"
#include "interface/controller_achievements_types.h"
#include "networking/logic/network_session_interface.h"
#include "saved_games/content_catalogue.h"
#include "networking/online/voice_xenon.h"
#include "interface/c_player_marketplace.h"
#include "interface/user_interface_controller.h"
#include "networking/network_utilities.h"
#include "networking/online/online_xenon.h"
#include "interface/user_interface_memory.h"
#include <xaudio.h>
#include <xhv.h>
#include <string.h>
#include <wchar.h>
// @address 0x58f5b0 wcsncmp
#include <stdlib.h>
#include "interface/controller_signin_cache.h"
#include "cseries/async_xoverlapped_types.h"
#include "cseries/async_xoverlapped_memory.h"
#include "interface/c_controller_externs.h"
#include "saved_games/content_catalogue_externs.h"
#include "text/unicode_constants.h"
// @address 0xa74 k_empty_wide_string
struct s_89910_storage_task;
struct s_89910_storage_device;
struct s_89910_catalogues;
struct s_89910_error;
struct s_89910_error_manager;



#define TEST_BIT16(flags, bit) (_rotl((flags), 32 - (bit)) & 1)
#if defined(_XBOX) && !defined(__clang__)
class c_achievements_enumerate_overlapped_task;
extern "C" {
// @address 0x808684 g_achievement_enumerate_task
static c_achievements_enumerate_overlapped_task *g_achievement_enumerate_task;
}
#endif



// 0x8ea58 (no dev name): keeps each controller's cache entry in step with its signed-in state. A controller
// that becomes valid takes its key and info key from the sign-in info; one that stops being valid is cleared.
#include "main/player_profile_summary_types.h"


#include "networking/logic/logic_managed_user_types.h"


#include "saved_games/content_catalogue_types.h"
// @address 0x6bccc8 XMarketplaceConsumeAssets

#include "networking/online/voice_engine_xenon.h"
#include "interface/controller_inline.h"
#include "interface/marketplace_consume_assets_task.h"
#include "saved_games/storage_device.h"

#include "networking/network_time_access.h"

static __forceinline s_achievement_globals &c_controller_achievement_storage()
{
#if defined(_XBOX) && !defined(__clang__)
    // match: 0x47bb78: a unit-local object here hoists its address (r27) ahead of g_controller_globals (r28)
    return *reinterpret_cast<s_achievement_globals *>(&g_achievement_enumerate_task);
#else
    return g_achievement_globals;
#endif
}

// @address 0x119ae98 g_controller_silver_or_gold_live_cache
bool g_controller_silver_or_gold_live_cache[4];

static __forceinline long controller_window_value(c_enum<e_window_index, long, 0, 5> window)
{
    return window;
}

// @address 0x89640 controller_update_all
extern "C" void controller_update_all(unsigned long milliseconds)
{
    game_globals_storage *game_globals = tls_game_globals();
    bool game_active = false;
    if (game_globals && game_globals->game_in_progress &&
        !game_globals->initializing && game_globals->map_active)
        game_active = true;

    controller_handle_xenon_messages();
    for (long controller_index = 0; controller_index < sizeof(controller_globals().controllers)
        / sizeof(controller_globals().controllers[0]); ++controller_index)
    {
        ((c_controller_interface *)&controller_globals().controllers[controller_index])->update(milliseconds);
        controller_globals().controller_windows[controller_index] = 4;
    }

    if (game_active)
    {
        long window_index = 0;
        for (long user_index = 0; user_index < 4; ++user_index)
        {
            long controller_index = user_interface_controller_get_rumble_enabled(user_index);
            if (controller_index != -1)
            {
                c_controller_interface *controller = (c_controller_interface *)&controller_globals().controllers[controller_index];
                bool in_use = controller->is_signed_in_to_machine() ||
                    TEST_BIT16(((const c_controller_interface *)controller)->flags, 2);
                controller_globals().controller_windows[controller_index] = in_use ? controller_window_value((e_window_index)window_index++) : 4;
            }
        }
    }
    controller_globals().controller_event_pending = false;
}

// @address 0x8a238 ?voice_device_is_connected@@YA_NW4e_controller_index@@@Z
bool voice_device_is_connected(e_controller_index controller_index)
{
    bool connected = false;
    if (g_voice_interface_globals.initialized &&
        g_voice_interface_globals.controller_states[controller_index].status_flags.test(_voice_controller_valid))
        connected = g_voice_engine.local_user_voice_device_connected(controller_index);
    return connected;
}

// The owner's slot key (zeroed when the slot index is out of range or the slot is unused).
// Without __restrict, update_controller_properties also reorders its presence/purchase_content stores.
// match: 0x89ae8: without __restrict the inlined copy colours its zero register r14 instead of r24
struct s_player_identifier const * c_controller_interface::get_player_identifier(struct s_player_identifier *__restrict key) const
{
  const c_controller_interface *owner = this;
  short index = owner->user_index;
  bool found = index >= 0 && index < 4 && network_session_interface_get_local_user_identifier(owner->user_index, key, true);
  if (!found)
    memset(key, 0, sizeof(*key));
  return key;
}

bool c_controller_interface::is_signed_in_to_machine() const
{
  const c_controller_interface *controller = (const c_controller_interface *)this;
  bool signed_in = controller->user_index >= 0 && controller->user_index < 4 &&
                   (long)g_network_session_interface_local_users[controller->user_index].state == 1 &&
                   !TEST_BIT16(controller->flags, 2);
  long state = XUserGetSigninState(controller - controller_globals().controllers);
  bool connected = state == 1 || state == 2;
  return signed_in & connected;
}

static __forceinline unsigned __int64 network_session_interface_get_local_user_xuid(long user_index)
{
    return g_network_session_interface_local_users[user_index].xuid;
}

static __forceinline bool online_xuid_is_guest_account(unsigned __int64 xuid)
{
    unsigned int account_flags = (unsigned int)(xuid >> 48);
    bool result = (account_flags & 15) == 9 && (account_flags & 192) != 0;
    return result;
}

bool c_controller_interface::is_guest() const
{
    bool result = false;
    if (get_user_index() >= 0 && get_user_index() < 4)
        result = online_xuid_is_guest_account(network_session_interface_get_local_user_xuid(get_user_index()));
    return result;
}

// @address 0x8af98 ?is_signed_in_to_live@c_controller_interface@@QBA_NXZ
bool c_controller_interface::is_signed_in_to_live() const
{
    bool result = false;
    if (get_user_index() >= 0 && get_user_index() < 4) {
        result = is_signed_in_to_machine() && is_silver_or_gold_live();
        long state = XUserGetSigninState(get_controller_index());
        result &= state == 2;
    }
    return result;
}

static __forceinline bool user_interface_xbox_guide_is_active()
{
    return g_message_globals.flag;
}

static __forceinline bool c_controller_online_is_silver_or_gold_live(long controller_index)
{
    bool result = g_online_cached_users[controller_index].silver_or_gold;
    return result;
}

bool c_controller_interface::is_silver_or_gold_live() const
{
    bool result = false;
    if (get_user_index() >= 0 && get_user_index() < 4)
    {
        e_controller_index controller_index = get_controller_index();
        if (user_interface_xbox_guide_is_active())
            result = g_controller_silver_or_gold_live_cache[controller_index];
        else
        {
            result = c_controller_online_is_silver_or_gold_live(controller_index);
            g_controller_silver_or_gold_live_cache[controller_index] = result;
        }
    }
    return result;
}


bool c_signed_in_users_watcher::have_users_changed()
{
    bool changed = false;
    long index = 0;
    do {
        const c_controller_interface *controller = (const c_controller_interface *)&controller_globals().controllers[index];
        s_player_identifier identifier;
        memset(&identifier, 0, sizeof(identifier));
        if (signin_state != 1 || controller->is_signed_in_to_live())
            (controller)->get_player_identifier(&identifier);
        if (memcmp(&identifier, &users[index], sizeof(identifier)) != 0) {
            changed = true;
            memcpy(&users[index], &identifier, sizeof(users[index]));
        }
        ++index;
    } while (index < 4);
    return changed;
}

unsigned __int64 c_controller_interface::get_hash_bits(long)
{
    c_controller_interface *object = this;
    game_globals_storage *globals = tls_game_globals();
    unsigned __int64 key;
    bool active = globals && (globals->initializing || globals->map_active);
    unsigned __int64 blockKey;
    if (active && &globals->options)
        blockKey = globals->options.game_instance;
    else
        blockKey = 0;
    key = blockKey;
    if (object->hash_bits == 0 || key != object->hash_key) {
        unsigned __int64 slotKey = 0;
        object->hash_key = key;
        short index = object->user_index;
        if (index >= 0 && index < 4)
            slotKey = g_network_session_interface_local_users[index].xuid;
        unsigned __int64 slotKeyCopy = slotKey;
        int hashContext[24];
        int slotDigest[8];
        int blockDigest[8];
        XHashMemoryBegin((XHASH_STATE *)hashContext);
        XHashMemoryUpdate((XHASH_STATE *)hashContext, &slotKeyCopy, 8);
        XHashMemoryEnd((XHASH_STATE *)hashContext, (unsigned char *)slotDigest);
        XHashMemoryBegin((XHASH_STATE *)hashContext);
        XHashMemoryUpdate((XHASH_STATE *)hashContext, &blockKey, 8);
        XHashMemoryEnd((XHASH_STATE *)hashContext, (unsigned char *)blockDigest);
        object->hash_bits = (unsigned int)((blockDigest[0] << 16) & 0x3fff0000) | ((unsigned __int64)(unsigned int)slotDigest[0] & 0xffffffff0000ffffui64);
    }
    return object->hash_bits;
}

extern "C" {
// @address 0x9e7e60 g_controller_signin_cache


}

extern "C" void controller_update_signin_cache(void)
{
    for (e_controller_index index = first_controller(); index != _controller_none; index = next_controller(index))
    {
        c_controller_interface *controller = controller_get(index);
        bool valid = false;
        if (controller != 0 && controller->is_signed_in_to_machine() && ((c_controller_interface *)controller)->user_index != -1)
            valid = true;
        s_controller_signin_cache_entry *entry = &g_controller_signin_cache[index];
        if (valid != entry->valid)
        {
            if (valid)
            {
                s_player_profile_summary info;
                if (user_interface_session_get_player_profile_summary(index, &info))
                {
                    entry->key = network_configuration_get_player_class(&info);
                    entry->info_key = info.global_rank;
                    entry->valid = 1;
                }
            }
            else
                entry->valid = 0;
        }
    }
}

e_controller_index controller_get_first_signed_in_controller()
{
    e_controller_index index;
    for (index = first_controller(); index != _controller_none; index = next_controller(index))
    {
        if (controller_get(index)->is_signed_in_to_machine())
            break;
    }
    return index;
}

// March has no pairing or decorated name for this added controller query.
// @address 0x47bab8 controller_get_first_signed_in_non_guest
extern "C" e_controller_index controller_get_first_signed_in_non_guest()
{
    e_controller_index index;
    for (index = first_controller(); index != _controller_none; index = next_controller(index))
    {
        c_controller_interface *controller = controller_get(index);
        if (controller->is_signed_in_to_machine() && !controller->is_guest())
            break;
    }
    return index;
}

// March has no pairing or decorated name for this lazy user-slot allocation.
// @address 0x47bcf8 controller_get_or_allocate_user_index
extern "C" short controller_get_or_allocate_user_index(c_controller_interface *controller)
{
    long user_index = controller->get_user_index();
    if (user_index == -1)
    {
        bool used[4] = { false };
        long controller_index = 0;
        do
        {
            c_controller_interface *other = &controller_globals().controllers[controller_index];
            if (other->user_index != -1)
                used[other->user_index] = true;
        } while (++controller_index < 4);
        for (unsigned long index = 0; index < 4; ++index)
        {
            if (!used[index])
            {
                user_index = index;
                break;
            }
        }
        ((c_controller_interface *)controller)->user_index = (short)user_index;
    }
    return (short)user_index;
}




void c_controller_interface::set_as_unsigned_in_user(bool unsigned_in)
{
    c_controller_interface *record = (c_controller_interface *)this;
    if (unsigned_in)
        generate_random_display_name();
    else
        memset(record->display_name, 0, sizeof(record->display_name));
    flags.set(_controller_state_unsigned_in_user, unsigned_in);
}



static __forceinline s_network_session_interface_user_record *network_session_interface_get_local_user(long user_index)
{
    return (s_network_session_interface_user_record *)&g_network_session_interface_local_users[user_index].state;
}

static __forceinline void network_session_interface_add_local_user(long user_index, const s_player_identifier *identifier)
{
    s_network_session_interface_user_record *user = network_session_interface_get_local_user(user_index);
    for (unsigned long index = 0; index < 4; ++index)
    {
        if (network_session_interface_get_local_user(index)->state == 12)
            network_session_interface_get_local_user(index)->state = 0;
    }
    memset(user, 0, sizeof(*user));
    user->state = 1;
    memcpy(user->key, identifier, sizeof(*identifier));
}

class c_content_catalogue;
__forceinline void c_content_catalogue::initialize(long controller_index)
    {
        bool bit2 = m_flags.test(2);
        bool bit3 = m_flags.test(3);
        content_catalogue_reset_controller_state(this, controller_index);
        m_data = (s_data_array *)data_initialize_disconnected("content items", 151, sizeof(s_content_item), 0, &g_overlapped_allocation);
        m_flags.set(2, bit2);
        m_flags.set(3, bit3);
    }

static __forceinline void content_catalogue_controller_sign_in(e_controller_index controller_index)
{
    ((c_content_catalogue *)&g_content_catalogue_globals.catalogues[controller_index])->initialize(controller_index);
}

void c_controller_interface::sign_in_controller(const s_player_identifier *player_identifier, bool unsigned_in)
{
    network_session_interface_add_local_user(controller_get_or_allocate_user_index(this), player_identifier);
    flags.set(_controller_state_unsigned_in_user, unsigned_in);
    e_controller_index controller_index = get_controller_index();
    user_interface_controller_update_network_properties(controller_index);
    this->hash_bits = 0;
    c_datamine event(0, "controller-sign-in", 2, "ui", "controller");
    content_catalogue_controller_sign_in(controller_index);
}

__forceinline void c_controller_interface::controller_reset_achievements()
{
    memset(&achievements.get_writeable_bits_direct()[0], 0, sizeof(unsigned long));
    memset(&achievements.get_writeable_bits_direct()[1], 0, sizeof(unsigned long));
}

// @address 0x47c1c8 ?sign_out_controller@c_controller_interface@@AAAX_N@Z
void c_controller_interface::sign_out_controller(bool)
{
    e_controller_index controller_index = get_controller_index();
    memset(&managed_user_globals.users[controller_index], 0, sizeof(managed_user_globals.users[controller_index]));
    short user_index = this->user_index;
    if (user_index != -1)
        network_session_interface_remove_local_user(user_index);
    c_player_profile_interface *profile = get_player_profile_interface();
    profile->flags = 0;
    profile->profile_read_retry_count = 0;
    profile->profile_read_retry_time = 0;
    profile->initialize_to_default_settings();
    ((c_content_catalogue *)&g_content_catalogue_globals.catalogues[controller_index])->dispose();
    flags.set(_controller_state_storage_device_selected, false);
    user_interface_controller_reset(controller_index);
    memset(&this->hash_bits, 0, sizeof(this->hash_bits));
    controller_reset_achievements();
    flags.set(_controller_state_storage_device_selected, false);
}

static __forceinline void c_controller_copy_display_name(wchar_t *dest, const wchar_t *source, long size)
{
    const wchar_t *s = source;
    for (long i = 0; i <= size - 1; ++i)
    {
        wchar_t c = *s;
        dest[i] = c;
        if (c == 0)
            break;
        ++s;
    }
    dest[size - 1] = 0;
}

// @address 0x47c0d0 ?generate_random_display_name@c_controller_interface@@AAAXXZ
void c_controller_interface::generate_random_display_name()
{
    wchar_t name[16];
    name[0] = *(const wchar_t *)k_empty_wide_string;
    memset(name + 1, 0, sizeof(name) - sizeof(name[0]));
    for (long attempt = 0; attempt < 20; ++attempt)
    {
        bool unique = true;
        if (network_get_random_player_name(name, 16))
        {
            for (unsigned long index = 0; index < 4; ++index)
            {
                const c_controller_interface *display = (const c_controller_interface *)&controller_globals().controllers[index];
                if (wcsncmp(name, display->display_name, 16) == 0)
                {
                    unique = false;
                    break;
                }
            }
            if (unique)
                break;
        }
    }
    c_controller_interface *self = (c_controller_interface *)this;
    memset(self->display_name, 0, sizeof(self->display_name));
    c_controller_copy_display_name(self->display_name, name, 16);
}

// @address 0x119aff0 g_controller_live_gold_nonguest_cache
bool g_controller_live_gold_nonguest_cache[4];

static __forceinline bool c_controller_online_enabled(e_controller_index controller_index)
{
    const online_xenon_user_state &user = g_online_cached_users[controller_index];
    bool result = user.online_enabled;
    return result;
}

static __forceinline bool c_controller_online_restricted(e_controller_index controller_index)
{
    const online_xenon_user_state &user = g_online_cached_users[controller_index];
    bool result = user.online_restricted;
    return result;
}

// @address 0x47be10 controller_is_live_gold_nonguest
extern "C" bool controller_is_live_gold_nonguest(c_controller_interface *controller)
{
    bool result = false;
    if (controller->get_user_index() >= 0 && controller->get_user_index() < 4)
    {
        e_controller_index controller_index = controller->get_controller_index();
        if (user_interface_xbox_guide_is_active())
            result = g_controller_live_gold_nonguest_cache[controller_index];
        else
        {
            result = controller->is_silver_or_gold_live() &&
                c_controller_online_enabled(controller->get_controller_index()) &&
                !controller->is_guest() &&
                !c_controller_online_restricted(controller->get_controller_index());
            g_controller_live_gold_nonguest_cache[controller_index] = result;
        }
    }
    return result;
}

extern "C" void controller_get_signed_in_player_identifiers(void *unused, s_player_identifier *identifiers)
{
    for (long index = 0; index < 4; ++index)
    {
        c_controller_interface *controller = controller_get((e_controller_index)index);
        bool valid = controller->is_signed_in_to_machine() ||
                     controller->flags.test(_controller_state_unsigned_in_user);
        if (valid)
            (controller)->get_player_identifier(&identifiers[index]);
        else
            memset(&identifiers[index], 0, sizeof(identifiers[index]));
    }
}

// @address 0x4b09e0 controller_is_live_content_available
extern "C" bool controller_is_live_content_available(e_controller_index controller_index, long *out_fallback)
{
    bool result = false;
    if (g_utility_drive_mounted != 0)
    {
        c_controller_interface *controller = controller_get(controller_index);
        result = controller->is_signed_in_to_live() && !controller->is_guest();
        if (out_fallback)
            *out_fallback = 0;
    }
    else
    {
        if (out_fallback)
            *out_fallback = 1;
    }
    return result;
}

// 0x4c1ec0
extern "C" unsigned long controller_get_signed_in_mask1()
{
    unsigned long result = 0;
    for (e_controller_index index = first_controller(); index != _controller_none; index = next_controller(index))
    {
        c_controller_interface *controller = controller_get(index);
        bool valid = controller->is_signed_in_to_machine() ||
                     controller->flags.test(_controller_state_unsigned_in_user);
        if (valid)
            result |= controller->player_profile.campaign.primary_skulls_found;
    }
    return result;
}

// 0x4c1f50
extern "C" unsigned long controller_get_signed_in_mask()
{
    unsigned long result = 0;
    for (e_controller_index index = first_controller(); index != _controller_none; index = next_controller(index))
    {
        c_controller_interface *controller = controller_get(index);
        bool valid = controller->is_signed_in_to_machine() ||
                     controller->flags.test(_controller_state_unsigned_in_user);
        if (valid)
            result |= controller->player_profile.campaign.secondary_skulls_found;
    }
    return result;
}

unsigned long c_marketplace_request_consume_assets_task::start(void *overlapped)
{
    unsigned long result = E_FAIL;
    if (controller_get(controller_index)->is_signed_in_to_live())
    {
        result = XMarketplaceConsumeAssets(controller_index, 1, &asset, static_cast<XOVERLAPPED *>(overlapped));
    }
    return result;
}

// @address 0x47bb78 controller_handle_xenon_messages
extern "C" void controller_handle_xenon_messages()
{
    const c_message *message = 0;
    if (!user_interface_messaging_get_next_message(-1, k_any_controller, k_any_window, &message))
        return;
    do
    {
        if (message->get_type() == _ui_xenon)
        {
            long xenon_type = ((const c_xenon_message *)message)->get_xenon_message_type();
            bool handled = false;
            switch (xenon_type)
            {
            case 4:
            {
                long controllers = ((const c_xenon_message *)message)->get_event_value();
                for (e_controller_index index = first_controller(); index != _controller_none; index = next_controller(index))
                {
                    if (((1 << index) & controllers) != 0)
                        controller_get(index)->is_signed_in_to_machine();
                }
                handled = true;
                break;
            }
            case 6:
                handled = controller_globals().controller_event_pending = true;
                break;
            case 5:
            {
                long controllers = ((const c_xenon_message *)message)->get_event_value();
                unsigned char *enumeration_pending = c_controller_achievement_storage().enumeration_pending;
                for (e_controller_index index = first_controller(); index != _controller_none; index = next_controller(index))
                {
                    if (((1 << index) & controllers) != 0)
                    {
                        enumeration_pending[index] = true;
                        controller_globals().controllers[index].player_profile.mark_for_update_from_storage();
                    }
                }
                handled = true;
                break;
            }
            }
            if (handled)
            {
                user_interface_messaging_pop(message);
                message->~c_message();
                ::user_interface_free((void const *)(message));
                message = 0;
            }
        }
    } while (user_interface_messaging_get_next_message(-1, k_any_controller, k_any_window, &message));
}

extern "C" {
// @address 0x8c494c g_marketplace_active_controller_count
// @address 0x7c33f0 g_marketplace_asset_consume_task
}

// @address 0x8ac48 controller_update_marketplace_state
extern "C" void controller_update_marketplace_state(s_controller_update *update)
{
    c_controller_interface *controller = controller_get(update->controller_index);
    long old_state = update->state;
    long new_state = saved_game_files_update_controller(update, controller, (update->state));
    if (update->state != new_state)
        player_marketplace_handle_request_state((s_player_marketplace_user *)update, controller, update->state, new_state);
    if (old_state == 0)
    {
        if (update->state != 0)
            ++g_marketplace_active_controller_count;
    }
    else if (update->state == 0)
        --g_marketplace_active_controller_count;
    if (g_marketplace_asset_consume_task.get_task_state() == 4)
    {
        if (g_marketplace_asset_consume_task.result == 0)
            g_gui_error_manager.post_error_with_custom_message(0x2008c, 0, 0, g_marketplace_asset_consume_task.controller_index, false, true);
        g_marketplace_asset_consume_task.c_overlapped_task::reset();
        update->consume_pending = true;
    }
    if (update->consume_pending && update->marketplace_state == 2 && update->state_10 == 2)
    {
        update->marketplace_state = 0;
        update->state_10 = 0;
        update->consume_pending = false;
    }
}

static __forceinline bool user_interface_get_storage_device_selection_deferred()
{
    unsigned long clock_bits = g_user_interface_globals.flags.peek();
    return TEST_BIT16(clock_bits, 3) != 0;
}


void gamer_achievements_begin_retrieval(e_controller_index controller_index)
{
    c_controller_achievement_storage().enumeration_pending[controller_index] = true;
}

static __forceinline bool network_session_interface_get_is_user_signed_in(long user_index)
{
    bool signed_in = false;
    const long state = network_session_interface_get_local_user(user_index)->state;
    if (state > 0 && state <= 10)
        signed_in = true;
    return signed_in;
}

static __forceinline void network_session_interface_set_local_user_properties(long user_index, unsigned __int64 xuid,
    bool silver_or_gold, bool online_enabled, bool online_restricted, bool can_use_file_share, bool can_use_friends_user_created_content)
{
    s_network_session_interface_user_record *user = network_session_interface_get_local_user(user_index);
    if (xuid != user->configuration.client.xuid || silver_or_gold != user->configuration.client.online || online_enabled != user->configuration.client.silver ||
        online_restricted != user->configuration.client.is_free_live_gold_account || can_use_file_share != user->configuration.client.guest ||
        can_use_friends_user_created_content != user->configuration.client.bot)
    {
        user->configuration.client.xuid = xuid;
        user->configuration.client.online = silver_or_gold;
        user->configuration.client.silver = online_enabled;
        user->configuration.client.is_free_live_gold_account = online_restricted;
        user->configuration.client.guest = can_use_file_share;
        user->configuration.client.bot = can_use_friends_user_created_content;
        user->change_count++;
    }
}

static __forceinline void user_interface_controller_attached(e_controller_index controller_index)
{
    g_user_interface_controller_globals.error_posted[controller_index] = false;
    g_gui_error_manager.clear_error(0x20014, controller_index, 0);
    user_interface_controller_update_network_properties(controller_index);
}

// @address 0x89ae8 ?update_controller_properties@c_controller_interface@@AAAXXZ
void c_controller_interface::update_controller_properties()
{
    // match: 0x89ae8: member-direct access folds address CSE and shrinks the function by 36B
    c_flags<e_controller_state_flags, short, 13> *flags = &this->flags;
    c_controller_interface *record = (c_controller_interface *)this;
    e_controller_index controller_index = get_controller_index();
    bool was_attached = flags->test(_controller_state_attached);
    bool has_gamepad = input_has_gamepad((short)controller_index);
    bool online_signed_in = online_has_signed_in_user(controller_index);
    bool session_signed_in = (get_user_index() >= 0 && get_user_index() < 4)
        ? network_session_interface_get_is_user_signed_in(get_user_index()) : false;
    if (!session_signed_in && get_user_index() != -1 && network_session_interface_get_local_user(get_user_index())->state == 0)
        this->user_index = -1;

    bool voice_device_connected = has_gamepad && voice_device_is_connected(controller_index);
    bool communications = false;
    bool communications_friends_only = false;
    bool profile_viewing = false;
    bool profile_viewing_friends_only = false;
    bool purchase_content = false;
    bool presence = false;
    bool presence_friends_only = false;

    if (online_signed_in || flags->test(_controller_state_unsigned_in_user))
    {
        bool sign_in_required = !session_signed_in;
        unsigned __int64 current_identifier = 0;
        unsigned __int64 xuid = 0;
        bool silver_or_gold = false;
        bool online_enabled = false;
        bool online_restricted = false;
        if (session_signed_in)
            get_player_identifier((s_player_identifier *)&current_identifier);
        if (online_signed_in)
        {
            xuid = online_user_get_xuid(controller_index);
            silver_or_gold = online_local_xuid_is_silver_or_gold_live(controller_index);
            online_enabled = online_xenon_user_is_online_enabled(controller_index);
            online_restricted = online_local_xuid_is_online_restricted(controller_index);
        }
        bool can_use_file_share = online_xenon_user_can_use_file_share(controller_index);
        bool can_use_friends_user_created_content = online_xenon_user_get_unknown5(controller_index);
        unsigned __int64 player_identifier = online_user_get_player_identifier(controller_index);
        if (sign_in_required || current_identifier != player_identifier)
        {
            ((c_content_catalogue *)&g_content_catalogue_globals.catalogues[controller_index])->dispose();
            flags->set(_controller_state_storage_device_selected, false);
            if (session_signed_in)
                sign_out_controller(true);
            memset(&managed_user_globals.users[controller_index], 0, sizeof(managed_user_globals.users[controller_index]));
            if (!flags->test(_controller_state_unsigned_in_user))
            {
                (get_player_profile_interface())->mark_for_update_from_storage();
                gamer_achievements_begin_retrieval(controller_index);
            }
            sign_in_controller((const s_player_identifier *)&player_identifier, flags->test(_controller_state_unsigned_in_user));
        }
        if (!flags->test(_controller_state_unsigned_in_user))
        {
            memset(record->display_name, 0, sizeof(record->display_name));
            const wchar_t *name = online_user_get_name(controller_index);
            if (name)
                c_controller_copy_display_name(record->display_name, name, 16);
        }
        network_session_interface_set_local_user_properties(get_user_index(), xuid,
            silver_or_gold, online_enabled, online_restricted, can_use_file_share, can_use_friends_user_created_content);
        if (online_signed_in)
        {
            BOOL privilege_result;
            if ((long)XUserCheckPrivilege(controller_index, XPRIVILEGE_COMMUNICATIONS, &privilege_result) == ERROR_SUCCESS && privilege_result)
                communications = true;
            else if ((long)XUserCheckPrivilege(controller_index, XPRIVILEGE_COMMUNICATIONS_FRIENDS_ONLY, &privilege_result) ==
                    ERROR_SUCCESS &&
                privilege_result)
                communications_friends_only = true;
            if ((long)XUserCheckPrivilege(controller_index, XPRIVILEGE_PROFILE_VIEWING, &privilege_result) == ERROR_SUCCESS && privilege_result)
                profile_viewing = true;
            else if ((long)XUserCheckPrivilege(controller_index, XPRIVILEGE_PROFILE_VIEWING_FRIENDS_ONLY, &privilege_result) ==
                    ERROR_SUCCESS &&
                privilege_result)
                profile_viewing_friends_only = true;
            if ((long)XUserCheckPrivilege(controller_index, XPRIVILEGE_PURCHASE_CONTENT, &privilege_result) == ERROR_SUCCESS && privilege_result)
                purchase_content = true;
            if ((long)XUserCheckPrivilege(controller_index, XPRIVILEGE_PRESENCE, &privilege_result) == ERROR_SUCCESS && privilege_result)
                presence = true;
            else if ((long)XUserCheckPrivilege(controller_index, XPRIVILEGE_PRESENCE_FRIENDS_ONLY, &privilege_result) == ERROR_SUCCESS && privilege_result)
                presence_friends_only = true;
        }
        flags->set(_controller_state_voice_device_connected, voice_device_connected);
        flags->set(_controller_state_privilege_communications, communications);
        flags->set(_controller_state_privilege_communications_friends_only, communications_friends_only);
        flags->set(_controller_state_privilege_profile_viewing, profile_viewing);
        flags->set(_controller_state_privilege_profile_viewing_friends_only, profile_viewing_friends_only);
        flags->set(_controller_state_privilege_purchase_content, purchase_content);
        flags->set(_controller_state_privilege_presence, presence);
        flags->set(_controller_state_privilege_presence_friends_only, presence_friends_only);
        record->sign_out_time = 0;
    }
    else if (session_signed_in)
    {
        unsigned long sign_out_time = record->sign_out_time;
        if (sign_out_time == 0)
        {
            record->sign_out_time = network_time_get();
        }
        else if ((long)(network_time_get() - sign_out_time) > 1000)
        {
            sign_out_controller(false);
            flags->clear();
            memset(record->display_name, 0, sizeof(record->display_name));
            record->sign_out_time = 0;
        }
    }

    flags->set(_controller_state_attached, has_gamepad);
    if (has_gamepad != was_attached)
    {
        if (has_gamepad)
        {
            if (in_use())
                user_interface_controller_attached(controller_index);
        }
        else
        {
            if (in_use())
                user_interface_controller_update_profile_error(controller_index);
        }
    }
}

// @address 0x89910 ?update@c_controller_interface@@QAAXK@Z
void c_controller_interface::update(unsigned long)
{
    update_controller_properties();
    if (is_signed_in_to_machine()) {
        if (!user_interface_get_storage_device_selection_deferred()) {
        e_controller_index controller_index = get_controller_index();
        const c_content_catalogue *catalogue = (const c_content_catalogue *)&content_catalogues().catalogues[controller_index];
        if (!content_catalogue_has_storage_device(catalogue) && !TEST_BIT16(this->flags, 3)) {
            if (!content_catalogues().storage_devices[controller_index].is_being_selected()) {
                if (!user_interface_error_manager_get()->any_error_active_for_controller(controller_index))
                    content_catalogue_display_device_selection_guide_interface(controller_index);
            }
        }
        }
    }
    bool in_use = is_signed_in_to_machine() || TEST_BIT16(this->flags, 2);
    if (in_use) {
        c_player_profile_interface *profile = get_player_profile_interface();
        profile->update(get_controller_index());
    }
    content_catalogue_update_enumeration_task(&content_catalogues().catalogues[get_controller_index()]);
    controller_update_marketplace_state(static_cast<s_controller_update *>(m_user));
}

__forceinline c_controller_interface::c_controller_interface() :
    flags(0),
    user_index(-1)
{
    memset(display_name, 0, sizeof(display_name));
    hash_bits = 0;
    hash_key = 0;
    achievements.clear();
    achievements.clear();
}

__forceinline c_controller_interface::~c_controller_interface()
{
}

__forceinline s_controller_globals::s_controller_globals()
{
}

__forceinline s_controller_globals::~s_controller_globals()
{
}

extern "C" {
s_controller_globals g_controller_globals;
}

