// blamlibxbox/content_catalogue.obj: from the records 0x8aa78 0x198158 0x1e5090 0x1e9ae0 0x282838 0x282898 0x282970 0x282a38 0x282c80 0x282ce8 0x282e18 0x282f70 0x283260 0x283548 0x283730 0x283800 0x2838b8 0x283a98 0x283ce0 0x283d78 0x283f08 0x283f88 0x284128 0x2841f8 0x2842e0 0x284458 0x284598 0x2848e0 0x284a10 0x284aa0 0x284b30 0x284cc0 0x284ef0 0x284ff0 0x285110 0x285260 0x285448 0x2854a0 0x2856e0 0x285770 0x285818 0x285958 0x46b0e8 0x46f830 0x4ada98 0x7084f8 0x70bef0
// @flags /TP /O2
#include "saved_games/content_catalogue_externs.h"
#include "saved_games/content_catalogue_enumeration.h"
#include "interface/controller_state_flags.h"
#include "saved_games/content_item_status_types.h"
#include "memory/data_types.h"
#include "multithreading/thread_tls.h"
#include "cseries/async_helpers.h"
#include "shell/shell.h"
#include "game/game_types.h"
#include "memory/static_arrays.h"
#include <string.h>
#include "multithreading/synchronization.h"
#include "game/game.h"
#include "simulation/simulation.h"
#include "text/unicode.h"
#include "cseries/async_xoverlapped.h"
#include "interface/c_player_profile_storage.h"
#include "tag_files/files.h"
#include "cseries/async_xoverlapped_memory.h"
#include "saved_games/content_catalogue.h"
#include "game/game_engine_group.h"
#include "cseries/cseries.h"
#include "cseries/async.h"
#include "saved_games/content_item.h"
#include "saved_games/saved_game_files.h"
#include "interface/controller_inline.h"
#include "interface/user_interface_error_manager.h"
#include "saved_games/content_synchronization_globals_types.h"
#include "game/game_engine_settings_definitions.h"
#include "scenario/scenario_tag.h"
#include "networking/online/online_globals_types.h"
#include "saved_games/content_critical_section_types.h"
#include "saved_games/content_item_task_types.h"
#include "networking/logic/network_session_interface.h"
#include "saved_games/saved_game_item_metadata_packed.h"
#include "saved_games/game_content_types.h"
#include "memory/data.h"
#include "cseries/crt_xdk.h"
#include "scenario/scenario.h"
#include "saved_games/content_catalogue_globals.h"
#include "saved_games/game_content.h"
#include "cseries/async_block.h"
#include "saved_games/content_catalogue_task_types.h"
#include "saved_games/content_item_types.h"
#include "game/game_globals_storage.h"
#include "memory/tls_globals.h"
#include "interface/gui_selected_item_metadata.h"
#include "main/levels_globals.h"
#include <ppcintrinsics.h>
#include <stdlib.h>
#include "cseries/async_xoverlapped_types.h"
#include "saved_games/storage_device.h"
#include "saved_games/content_catalogue_types.h"
#include "networking/online/online_presence_accessors.h"
#include "multithreading/synchronized_value.h"
#include "tag_files/file_reference.h"
#include "saved_games/saved_game_scope_lock.h"
#include "saved_games/content_item_valid.h"
#include "interface/user_interface_content_globals.h"
#include "saved_games/saved_game_file_types.h"
#include "interface/gui_selected_items_game_variant.h"
#include "game/game_engine_variant_lookup.h"
class c_content_catalogue_item_iterator {
    s_content_item *m_datum;
    s_data_iterator m_iterator;
public:
    __forceinline c_content_catalogue_item_iterator(s_data_array *data)
    {
        m_iterator.data = data;
        m_iterator.absolute_index = -1;
        m_iterator.index = -1;
    }
    __forceinline bool next() { m_datum = (s_content_item *)data_iterator_next(&m_iterator); return m_datum != 0; }
    __forceinline s_content_item *get_datum() const { return m_datum; }
    __forceinline long get_index() const { return m_iterator.index; }
};
struct s_content_catalogue_enumeration_task
{
    long state;
    long controller_index;
    long content_item_index;
#ifdef HALO3_64BIT
    char data[512 - 12];
#else
    char data[0x114];
#endif
};
// @address 0x282f70 ?content_catalogue_get_saved_game_file_type_from_container_name@@YA?AW4e_game_content_type@@PBD@Z
// @address 0x2838b8 ?dispose@c_content_catalogue@@QAAXXZ
// @address 0x285260 ?content_enumeration_success@c_content_catalogue@@IAAXPAU_XCONTENT_DATA@@JJ@Z
// @address 0x2854a0 ?try_and_get_item_by_file_reference@c_content_catalogue@@QBAPAUs_content_item@@PBUs_file_reference@@PAJ@Z
// @address 0x283d78 ?try_and_get_item_by_type_and_display_name@c_content_catalogue@@QAAJPB_W@Z
// @address 0x2841f8 ?try_and_get_personal_storage_item@c_content_catalogue@@QAAJPB_WW4e_content_item_status@@@Z
// @address 0x285110 content_catalogue_close_unlisted_map_items
// @address 0x283730 ??0c_content_catalogue@@QAA@XZ
// @address 0x282970 ??1c_content_catalogue@@QAA@XZ
// @address 0x2f7258 ?failure@c_content_item_overlapped_task@@UAAXKKK@Z
// @address 0x2f76e0 ?failure@c_content_enumeration_overlapped_task@@UAAXKKK@Z
// @address 0x2f76e8 ?complete@c_content_enumeration_overlapped_task@@UAAXXZ
struct utf8;
// @address 0x119a430 ?g_content_catalogue_name_counter@@3JA
long g_content_catalogue_name_counter;
class c_storage_device;
static __forceinline c_storage_device *content_catalogue_storage_device_get(long controller_index)
{
    return (unsigned long)controller_index <= 3 ? &g_content_catalogue_globals.storage_devices[controller_index] : 0;
}
class c_content_catalogue;
// the scenario's campaign type (0 campaign, 1 multiplayer, 2 main menu) and map id
// @address 0x282ce8 ?content_catalogue_display_device_selection_guide_interface@@YAXW4e_controller_index@@@Z
void content_catalogue_display_device_selection_guide_interface(e_controller_index controller_index)
{
    c_controller_interface *controller = controller_get(controller_index);
    if (controller->is_signed_in_to_machine() && !controller->is_unsigned_in_user() && !controller->is_guest()) {
        bool device_selector_active = false;
        for (e_controller_index index = first_controller(); index != k_no_controller; index = next_controller(index)) {
            if (g_content_catalogue_globals.storage_devices[index].is_being_selected()) {
                device_selector_active = true;
                break;
            }
        }
        if (!device_selector_active && !online_guide_is_active())
            g_content_catalogue_globals.storage_devices[controller_index].select_device(0xa00000);
    }
}
const char *content_catalogue_build_new_saved_game_file_type_container_name(e_game_content_type type, char *buffer, long buffer_size)
{
    // match: 0x282e18: every caller passes 42; without it buffer_size stays live in r30 (savegprlr_29 frame) instead of li r4,42 at each csnzprintf
    __assume(buffer_size == 42);
    __int64 time = _time64(0);
    ++g_content_catalogue_name_counter;
    switch (type)
    {
    case _game_content_type_map:
        csnzprintf(buffer, buffer_size, "usermap%016lX%lX", time, g_content_catalogue_name_counter);
        break;
    case _game_content_type_personal:
        csnzprintf(buffer, buffer_size, "personal%016lX%lX", time, g_content_catalogue_name_counter);
        break;
    case _game_content_type_ctf:
        csnzprintf(buffer, buffer_size, "ctf%016lX%lX", time, g_content_catalogue_name_counter);
        break;
    case _game_content_type_slayer:
        csnzprintf(buffer, buffer_size, "slayer%016lX%lX", time, g_content_catalogue_name_counter);
        break;
    case _game_content_type_oddball:
        csnzprintf(buffer, buffer_size, "oddball%016lX%lX", time, g_content_catalogue_name_counter);
        break;
    case _game_content_type_koth:
        csnzprintf(buffer, buffer_size, "koth%016lX%lX", time, g_content_catalogue_name_counter);
        break;
    case _game_content_type_jugg:
        csnzprintf(buffer, buffer_size, "juggernaut%016lX%lX", time, g_content_catalogue_name_counter);
        break;
    case _game_content_type_terries:
        csnzprintf(buffer, buffer_size, "territories%016lX%lX", time, g_content_catalogue_name_counter);
        break;
    case _game_content_type_assault:
        csnzprintf(buffer, buffer_size, "assault%016lX%lX", time, g_content_catalogue_name_counter);
        break;
    case _game_content_type_vip:
        csnzprintf(buffer, buffer_size, "vip%016lX%lX", time, g_content_catalogue_name_counter);
        break;
    case _game_content_type_zombiez:
        csnzprintf(buffer, buffer_size, "infection%016lX%lX", time, g_content_catalogue_name_counter);
        break;
    case _game_content_type_film:
        csnzprintf(buffer, buffer_size, "film%016lX%lX", time, g_content_catalogue_name_counter);
        break;
    case _game_content_type_clip:
        csnzprintf(buffer, buffer_size, "clip%016lX%lX", time, g_content_catalogue_name_counter);
        break;
    case _game_content_type_shot:
        csnzprintf(buffer, buffer_size, "screenshot%016lX%lX", time, g_content_catalogue_name_counter);
        break;
    default:
        // match: 0x282e18: without it the switch gains a cmplwi cr6,r10,13 / bgt range check before the jump table
        __assume(0);
    }
    return buffer;
}
e_game_content_type content_catalogue_get_saved_game_file_type_from_container_name(const char *container_name)
{
    if (strncmp(container_name, "usermap", 7) == 0)
        return _game_content_type_map;
    if (strncmp(container_name, "personal", 8) == 0)
        return _game_content_type_personal;
    if (strncmp(container_name, "ctf", 3) == 0)
        return _game_content_type_ctf;
    if (strncmp(container_name, "slayer", 6) == 0)
        return _game_content_type_slayer;
    if (strncmp(container_name, "oddball", 7) == 0)
        return _game_content_type_oddball;
    if (strncmp(container_name, "koth", 4) == 0)
        return _game_content_type_koth;
    if (strncmp(container_name, "juggernaut", 10) == 0)
        return _game_content_type_jugg;
    if (strncmp(container_name, "territories", 11) == 0)
        return _game_content_type_terries;
    if (strncmp(container_name, "assault", 7) == 0)
        return _game_content_type_assault;
    if (strncmp(container_name, "vip", 3) == 0)
        return _game_content_type_vip;
    if (strncmp(container_name, "infection", 9) == 0)
        return _game_content_type_zombiez;
    if (strncmp(container_name, "film", 4) == 0)
        return _game_content_type_film;
    if (strncmp(container_name, "clip", 4) == 0)
        return _game_content_type_clip;
    return strncmp(container_name, "screenshot", 10) == 0 ? _game_content_type_shot : _game_content_type_none;
}


class c_content_catalogue;

// match: saved_game_files.cpp still declares this C bridge with an extra argument.

struct s_synchronization_globals;

class c_content_catalogue;



c_content_catalogue::c_content_catalogue() :
    m_controller_index(-1),
    m_data(0),
    m_items(0),
    m_content_item_task(__FILE__, __LINE__),
    m_enumeration_task(__FILE__, __LINE__),
    last_content_index(-1),
    m_lock(0)
{
    m_flags.value = 0;
}

// @address 0x284ef0 content_catalogue_block_until_finished
void content_catalogue_block_until_finished(c_content_catalogue *catalogue, const s_content_item *item, bool first, bool second)
{
    if (first) {
        if (catalogue->m_enumeration_task.busy())
            overlapped_task_block_until_finished(&catalogue->m_enumeration_task);
        if (catalogue->last_content_index != -1) {
            async_block_until_done(&catalogue->m_lock, simple_yield_function, 0, 1, 0, 0);
            catalogue->last_content_index = -1;
        }
    }
    if (catalogue->m_content_item_task.busy() && (item == 0 || item == catalogue->m_content_item_task.get_content_item()))
        overlapped_task_block_until_finished(&catalogue->m_content_item_task);
}

// @address 0x283800 content_catalogue_reset_controller_state
void content_catalogue_reset_controller_state(c_content_catalogue *catalogue, long controller_index)
{
    bool bit2 = catalogue->m_flags.test(2);
    bool bit3 = catalogue->m_flags.test(3);
    catalogue->m_controller_index = controller_index;
    memset(&catalogue->m_item_for_deletion, 0, sizeof(catalogue->m_item_for_deletion));
    catalogue->last_content_index = -1;
    catalogue->m_lock.set(0);
    catalogue->m_flags.value = 0;
    catalogue->m_flags.set(2, bit2);
    catalogue->m_flags.set(3, bit3);
}

static __forceinline void content_catalogue_delete_item_array(s_content_item *items)
{
    unsigned __int64 *allocation = (unsigned __int64 *)items - 1;
    for (long index = 0; index < *allocation; ++index)
        items[index].close(true);
    overlapped_free(allocation);
}

// @address 0x285770 ?dispose_content_item_array@c_content_catalogue@@IAAXXZ
void c_content_catalogue::dispose_content_item_array()
{
    c_saved_game_scope_lock lock;
    if (m_items != 0) {
        content_catalogue_delete_item_array(m_items);
        m_items = 0;
    }
}


void c_content_catalogue::dispose()
{
    if (m_flags.test(4))
        return;
    m_flags.set(4, true);
    bool bit2 = m_flags.test(2);
    bool bit3 = m_flags.test(3);
    content_catalogue_block_until_finished(this, 0, true, true);
    m_content_item_task.reset();
    m_enumeration_task.reset();
    c_saved_game_scope_lock lock;
    last_content_index = -1;
    m_lock.set(0);
    if (m_data) {
        if (!(m_data->flags & 2))
            data_disconnect(m_data);
        data_dispose(m_data);
        m_data = 0;
    }
    if (m_items != 0)
        dispose_content_item_array();
    c_storage_device *storage_device = content_catalogue_storage_device_get(m_controller_index);
    if (storage_device != 0)
        storage_device->release();
    m_items = 0;
    memset(&m_item_for_deletion, 0, sizeof(m_item_for_deletion));
    m_flags.set(4, false);
    m_flags.value = 0;
    m_flags.set(2, bit2);
    m_flags.set(3, bit3);
}

static __forceinline long content_catalogue_enumeration_range(long index, long normal_count, long total_count)
{
    if (index >= 0 && index < normal_count)
        return 0;
    if (normal_count <= index && index < total_count)
        return 1;
    return -1;
}

void c_content_catalogue::content_enumeration_success(_XCONTENT_DATA *content_data, long content_count, long other_content_count)
{
    c_saved_game_scope_lock lock;
    // the content data was enumerated into the item array: items are built over it from the back
    m_items = (s_content_item *)content_data;
    if (m_data != 0) {
        if (!(m_data->flags & 2))
            data_disconnect(m_data);
    } else {
        m_data = (s_data_array *)data_initialize_disconnected(
            "content items", 151, sizeof(s_content_item), 0, &g_overlapped_allocation);
    }
    if (content_catalogue_has_storage_device(this) && m_data != 0) {
        long total_count = content_count + other_content_count;
        if (total_count > 0) {
            for (long index = total_count - 1; index >= 0; --index) {
                const _XCONTENT_DATA *data = &content_data[index];
                s_content_item *item = &m_items[index];
                switch (content_catalogue_enumeration_range(index, content_count, total_count)) {
                case 0: {
                    e_game_content_type type = content_catalogue_get_saved_game_file_type_from_container_name(data->szFileName);
                    if ((unsigned long)type <= _game_content_type_shot)
                        item->create_from_enumeration(type, this, data);
                    break;
                }
                case 1:
                    item->create_from_enumeration(_game_content_type_savegame, this, data);
                    break;
                }
                item->identifier = -1;
            }
            long remaining_count = 151 - total_count;
            if (remaining_count > 0)
                memset(&m_items[total_count], 0, remaining_count * sizeof(s_content_item));
        }
        if (m_data != 0)
            ::data_connect(m_data, 151, (void *)(m_items));
    }
}

static __forceinline unsigned long content_catalogue_storage_device_id(long controller_index)
{
    c_storage_device *storage_device = content_catalogue_storage_device_get(controller_index);
    return storage_device ? storage_device->device_id() : 0xcacacaca;
}

// @address 0x1e5090 content_catalogue_has_storage_device
// match: 0x284ff0: inline keeps the device check folded into begin_enumeration; out of line it adds one instruction
#if defined(_XBOX) && !defined(__clang__)
extern "C" inline bool content_catalogue_has_storage_device(const c_content_catalogue *catalogue)
#else
extern "C" bool content_catalogue_has_storage_device(const c_content_catalogue *catalogue)
#endif
{
    return content_catalogue_storage_device_id(catalogue->m_controller_index) != 0xcacacaca;
}


// @address 0x282838 content_catalogue_enumeration_is_complete
// match: 0x285448: inline keeps the busy and valid tests folded into try_and_get_item; out of line it loses three instructions
#if defined(_XBOX) && !defined(__clang__)
extern "C" inline bool content_catalogue_enumeration_is_complete(const c_content_catalogue *catalogue)
#else
extern "C" bool content_catalogue_enumeration_is_complete(const c_content_catalogue *catalogue)
#endif
{
    const c_overlapped_task *task = &catalogue->m_enumeration_task;
    return !task->busy() && catalogue->m_data != 0 && catalogue->m_data->valid;
}

c_content_catalogue::~c_content_catalogue()
{
}

typedef c_flags_no_init<e_controller_state_flags, short, 13> c_controller_state_flags;
static __forceinline c_content_catalogue *content_catalogue_get(e_controller_index controller_index)
{
    return &g_content_catalogue_globals.catalogues[controller_index];
}

// @address 0x282a38 content_catalogue_close_items_for_scenario
extern "C" void content_catalogue_close_items_for_scenario()
{
    char single_player_types[15] = {0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
    char multiplayer_types[15] = {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1};
    char all_types[15] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
    const char *types;
    struct scenario *scenario = global_scenario;
    if (scenario == 0)
        return;
    switch (scenario->type) {
    case 0: types = single_player_types; break;
    case 1: types = multiplayer_types; break;
    case 2: types = all_types; break;
    default: return;
    }
    bool cooperative = game_is_cooperative();
    for (e_controller_index controller_index = first_controller(); controller_index != k_no_controller;
        controller_index = next_controller(controller_index))
    {
        for (long type = 0; type < 15; ++type) {
            if (cooperative) {
                if (type == _game_content_type_film)
                    continue;
                if (type == _game_content_type_clip)
                    continue;
            }
            if (types[type] == 0)
                continue;
            c_content_catalogue *catalogue = content_catalogue_get(controller_index);
            if (content_catalogue_storage_device_id(catalogue->m_controller_index) == 0xcacacaca)
                continue;
            c_saved_game_scope_lock lock;
            long index = content_catalogue_get_first_item_index(catalogue);
            if (index != -1) {
                do {
                    if (content_catalogue_get_game_content_type(catalogue, index) == type)
                        content_catalogue_close_item(catalogue, index, true);
                    index = content_catalogue_get_next_item_index(catalogue, index);
                } while (index != -1);
            }
        }
    }
}

// @address 0x282c80 content_catalogue_dispose_all_on_content_change
extern "C" void content_catalogue_dispose_all_on_content_change()
{
    for (e_controller_index controller_index = first_controller(); controller_index != k_no_controller;
        controller_index = next_controller(controller_index))
    {
        c_content_catalogue *catalogue = content_catalogue_get(controller_index);
        c_controller_state_flags *state_flags = (c_controller_state_flags *)controller_get(controller_index);
        state_flags->set(_controller_state_storage_device_selected, false);
        catalogue->dispose();
    }
}


// @address 0x285958 content_catalogue_trim_trailing_whitespace
extern "C" void content_catalogue_trim_trailing_whitespace(unsigned short *string)
{
    int length = ((const c_static_wchar_string<256> *)string)->length();
    if (length < 256) {
        unsigned short *character = string + length - 1;
        do {
            unsigned short c = *character;
            if (c != 32 && c != 13 && c != 10)
                return;
            *character = 0;
            --length;
            --character;
        } while (256 > length);
    }
}

// @address 0x283ce0 content_catalogue_get_next_item_index
extern "C" int content_catalogue_get_next_item_index(c_content_catalogue *catalogue, int index)
{
    int result = -1;
    c_saved_game_scope_lock lock;
    if (content_catalogue_has_storage_device(catalogue) && content_catalogue_enumeration_is_complete(catalogue))
        result = ::data_next_index(catalogue->m_data, index);
    return result;
}

static __forceinline c_content_catalogue *content_catalogue_get_from_controller(const c_controller_interface *controller)
{
    return content_catalogue_get(controller->get_controller_index());
}


// @address 0x283260 content_catalogue_make_ready_for_path
extern "C" bool content_catalogue_make_ready_for_path(const char *path)
{
    bool result = false;
    char root_path[256];
    strncpy(root_path, path, sizeof(root_path));
    root_path[255] = 0;
    while (true) {
        long length = (long)csstrnlen(root_path, sizeof(root_path));
        long index = (long)strcspn(root_path, ":\\");
        if (index >= length)
            break;
        root_path[index] = 0;
    }
    long length = (long)csstrnlen(root_path, sizeof(root_path));
    strncpy(&root_path[length], ":\\", sizeof(root_path) - length);
    root_path[255] = 0;
    s_file_reference directory;
    file_reference_create_from_path(&directory, root_path, true);
    for (e_controller_index controller_index = first_controller(); controller_index != k_no_controller;
        controller_index = next_controller(controller_index))
    {
        if (result)
            break;
        c_controller_interface *controller = controller_get(controller_index);
        bool signed_in = controller->is_signed_in_to_machine() || controller->is_unsigned_in_user();
        if (!signed_in)
            continue;
        bool skip = false;
        short user_index = ((const s_controller *)controller)->user_index;
        if (user_index >= _controller0 && user_index < k_number_of_controllers) {
            unsigned short state = (online_presence_user_slot(user_index).xuid >> 48);
            skip = (state & 15) == 9 && (state & 0xc0) != 0;
        }
        if (skip)
            continue;
        bool success = false;
        c_content_catalogue *catalogue = content_catalogue_get_from_controller(controller);
        c_saved_game_scope_lock lock;
        if (content_catalogue_has_storage_device(catalogue)) {
            long content_item_index;
            s_content_item *item = catalogue->try_and_get_item_by_file_reference(&directory, &content_item_index);
            if (item != 0 && item->game_content_type == 0)
                success = ((s_content_item *)catalogue)->make_ready_for_access(content_item_index, true);
        }
            result = success;
    }
    return result;
}

// @address 0x283548 content_catalogue_update_active_map_items
extern "C" void content_catalogue_update_active_map_items(bool active)
{
    e_map_id map_ids[4];
    e_campaign_id campaign_ids[4];
    long count = 0;
    if (active) {
        if (simulation_starting_up()) {
            count = 1;
            game_globals_storage *globals = game_globals;
            e_campaign_id campaign = globals->options.campaign_id;
            e_map_id map = globals->options.map_id;
            campaign_ids[0] = campaign;
            map_ids[0] = map;
        }
        if (((scenario *)global_scenario) != 0) {
            if ((unsigned short)((scenario *)global_scenario)->type != 0)
                campaign_ids[count] = _campaign_id_none;
            else
                campaign_ids[count] = _campaign_id_pre_game_fallback;
            map_ids[count] = (e_map_id)((scenario *)global_scenario)->map_id;
            ++count;
        }
        e_campaign_id campaign_id = _campaign_id_none;
        e_map_id map_id = _map_id_none;
        if (network_life_cycle_session_get_map(&campaign_id, &map_id)) {
            campaign_ids[count] = campaign_id;
            map_ids[count] = map_id;
            ++count;
        }
    }
    for (e_controller_index controller_index = first_controller(); controller_index != k_no_controller;
        controller_index = next_controller(controller_index))
    {
        c_controller_interface *controller = controller_get(controller_index);
        bool signed_in = controller->is_signed_in_to_machine() || controller->is_unsigned_in_user();
        if (!signed_in)
            continue;
        bool skip = false;
        short user_index = ((const s_controller *)controller)->user_index;
        if (user_index >= _controller0 && user_index < k_number_of_controllers) {
            unsigned short state = (online_presence_user_slot(user_index).xuid >> 48);
            skip = (state & 15) == 9 && (state & 0xc0) != 0;
        }
        if (!skip)
            content_catalogue_close_unlisted_map_items(content_catalogue_get_from_controller(controller), (int *)campaign_ids, (int *)map_ids, count);
    }
}


// @address 0x4ada98 content_catalogue_trim_whitespace
extern "C" void content_catalogue_trim_whitespace(unsigned short *string)
{
    int count = 0;
    int length = ((const c_static_wchar_string<256> *)string)->length();
    if (length > 0) {
        unsigned short *character = string;
        while (*character == ' ' || *character == '\t' || *character == '\r' || *character == '\n') {
            ++count;
            ++character;
            if (count >= length)
                break;
        }
        if (count != 0) {
            memmove(string, string + count, (length - count) * sizeof(unsigned short));
            string[length - count] = 0;
        }
    }
    content_catalogue_trim_trailing_whitespace(string);
}






// @address 0x1e9ae0 gui_selected_items_unpack_metadata
extern "C" void gui_selected_items_unpack_metadata(s_gui_selected_items_block *metadata, const s_saved_game_item_metadata_packed *packed)
{
    memset(metadata, 0, sizeof(*metadata));
    metadata->unique_id = packed->unique_id;
    ustrnzcpy_inline(metadata->name, packed->name, 16);
    ::utf8_string_to_wchar_string((utf8 const *)packed->description, (wchar_t *)((unsigned short *)metadata->description), 128, (long *)(0));
    csstrnzcpy(metadata->author, packed->author, 16);
    metadata->file_type = packed->file_type;
    metadata->author_is_online = packed->author_is_online;
    metadata->author_id = packed->author_id;
    metadata->size_in_bytes = packed->size_in_bytes;
    metadata->date = packed->date;
    metadata->length_seconds = packed->length_seconds;
    metadata->campaign_id = packed->campaign_id;
    metadata->map_id = packed->map_id;
    metadata->game_engine_type = packed->game_engine_type;
    metadata->campaign_difficulty = packed->campaign_difficulty;
    metadata->campaign_insertion_point = packed->campaign_insertion_point;
    metadata->campaign_survival_enabled = packed->campaign_survival_enabled;
    metadata->game_id = packed->game_id;
}


// @address 0x285448 content_catalogue_try_and_get_item
extern "C" s_content_item *content_catalogue_try_and_get_item(const c_content_catalogue *catalogue, long content_item_index)
{
    if (content_catalogue_enumeration_is_complete(catalogue))
        return (s_content_item *)datum_try_and_get(catalogue->m_data, content_item_index);
    return 0;
}




static __forceinline bool content_catalogue_file_references_equal(const s_file_reference *a, const s_file_reference *b)
{
    bool equal = false;
    if (a->location == b->location && strcmp(a->path, b->path) == 0)
        equal = true;
    return equal;
}


s_content_item *c_content_catalogue::try_and_get_item_by_file_reference(const s_file_reference *file, long *content_item_index) const
{
    s_content_item *result = 0;
    bool compare_paths = (file->flags & 1) == 0;
    *content_item_index = -1;
    if (content_catalogue_has_storage_device(this) && content_catalogue_enumeration_is_complete(this)) {
        char file_name[256];
        file_reference_get_name(file, 13, file_name, sizeof(file_name));
        c_content_catalogue_item_iterator iterator(m_data);
        while (iterator.next()) {
            s_content_item *item = iterator.get_datum();
            s_file_reference directory;
            if (!item->is_valid() || !item->get_directory_reference(&directory))
                continue;
            if (compare_paths) {
                if (!content_catalogue_file_references_equal(&directory, file))
                    continue;
            } else {
                char directory_name[256];
                file_reference_get_name(&directory, 13, directory_name, sizeof(directory_name));
                if (::csstrnicmp((char const *)(directory_name), (char const *)(file_name),
                        csstrnlen(directory_name, sizeof(directory_name))) != 0)
                    continue;
            }
            result = item;
            if (result != 0) {
                *content_item_index = iterator.get_index();
                break;
            }
        }
    }
    return result;
}


// @address 0x283f08 content_catalogue_find_item_by_file_reference
extern "C" long content_catalogue_find_item_by_file_reference(const c_content_catalogue *catalogue, const s_file_reference *file)
{
    long content_item_index;
    {
        c_saved_game_scope_lock lock;
        catalogue->try_and_get_item_by_file_reference(file, &content_item_index);
    }
    return content_item_index;
}


// callers (saved_game_files.cpp) always pass synchronous = true
// match: this accepted caller supplies the legacy synchronous argument.
// @address 0x284458 content_catalogue_delete_item
extern "C" void content_catalogue_delete_item(c_content_catalogue *catalogue, long content_item_index, bool synchronous)
{
    if (content_catalogue_has_storage_device(catalogue)) {
        if (catalogue->m_item_for_deletion.is_valid() && catalogue->m_item_for_deletion.flags.test_bit(3))
            content_catalogue_block_until_finished(catalogue, &catalogue->m_item_for_deletion, false, true);
        c_saved_game_scope_lock lock;
        if (content_catalogue_enumeration_is_complete(catalogue)) {
            s_content_item *item = content_catalogue_try_and_get_item(catalogue, content_item_index);
            if (item != 0) {
                memcpy(&catalogue->m_item_for_deletion, item, sizeof(s_content_item));
                catalogue->m_item_for_deletion.flags.set_bit(3, false);
                content_item_delete_container(&catalogue->m_item_for_deletion, true);
                datum_delete(catalogue->m_data, content_item_index);
            }
        }
    }
}

// @address 0x284598 content_catalogue_flush_item
extern "C" void content_catalogue_flush_item(c_content_catalogue *catalogue, long content_item_index, bool)
{
    if (content_catalogue_has_storage_device(catalogue)) {
        c_saved_game_scope_lock lock;
        s_content_item *item = content_catalogue_try_and_get_item(catalogue, content_item_index);
        if (item != 0)
            item->flush(true);
    }
}

// @address 0x2848e0 content_catalogue_get_item_display_name
extern "C" bool content_catalogue_get_item_display_name(const c_content_catalogue *catalogue, long content_item_index, wchar_t *display_name, long size)
{
    bool result = false;
    c_saved_game_scope_lock lock;
    if (content_catalogue_has_storage_device(catalogue)) {
        s_content_item *item = content_catalogue_try_and_get_item(catalogue, content_item_index);
        if (item != 0) {
            *display_name = 0;
            if (item->is_valid()) {
                c_saved_game_scope_lock item_lock;
                ustrnzcpy(display_name, item->metadata.name, size);
            }
            result = *display_name != 0;
        }
    }
    return result;
}

// @address 0x284a10 content_catalogue_is_item_corrupt
extern "C" bool content_catalogue_is_item_corrupt(const c_content_catalogue *catalogue, long content_item_index)
{
    bool result = true;
    c_saved_game_scope_lock lock;
    if (content_catalogue_has_storage_device(catalogue)) {
        s_content_item *item = content_catalogue_try_and_get_item(catalogue, content_item_index);
        if (item != 0)
            result = item->flags.test_bit(1);
    }
    return result;
}

// @address 0x284aa0 content_catalogue_is_item_created
extern "C" bool content_catalogue_is_item_created(const c_content_catalogue *catalogue, long content_item_index)
{
    bool result = true;
    c_saved_game_scope_lock lock;
    if (content_catalogue_has_storage_device(catalogue)) {
        s_content_item *item = content_catalogue_try_and_get_item(catalogue, content_item_index);
        if (item != 0)
            result = item->flags.test_bit(6);
    }
    return result;
}

// @address 0x284cc0 content_catalogue_get_item_container_path
extern "C" bool content_catalogue_get_item_container_path(const c_content_catalogue *catalogue, long content_item_index, char *buffer)
{
    bool result = false;
    *buffer = 0;
    c_saved_game_scope_lock lock;
    if (content_catalogue_has_storage_device(catalogue)) {
        s_content_item *item = content_catalogue_try_and_get_item(catalogue, content_item_index);
        if (item != 0) {
            item->get_container_full_path(buffer, 0x100, true);
            result = *buffer != 0;
        }
    }
    return result;
}

// @address 0x284ff0 ?begin_enumeration@c_content_catalogue@@QAA_NXZ
bool c_content_catalogue::begin_enumeration()
{
    bool result = false;
    m_flags.value = m_flags.value & -3;
    if (content_catalogue_has_storage_device(this)) {
        content_catalogue_block_until_finished(this, 0, true, true);
        c_saved_game_scope_lock lock;
        dispose_content_item_array();
        long controller_index = m_controller_index;
        unsigned long device_id;
        if ((unsigned long)controller_index <= 3) {
            c_storage_device *device = &g_content_catalogue_globals.storage_devices[controller_index];
            if (device != 0)
                device_id = device->device_id();
            else
                device_id = 0xcacacaca;
        } else {
            device_id = 0xcacacaca;
        }
        m_enumeration_task.initialize(this, controller_index, device_id);
        result = overlapped_task_start_internal(&m_enumeration_task, "..\\saved_games\\content_catalogue.cpp", 0x7a1);
    }
    return result;
}



long c_content_catalogue::try_and_get_item_by_type_and_display_name(const wchar_t *display_name)
{
    long index = content_catalogue_get_first_item_index(this);
    if (index != -1) {
        do {
            if (content_catalogue_get_game_content_type(this, index) == 2) {
                bool match = false;
                wchar_t item_name[128];
                s_content_critical_section *section = &((s_content_synchronization_globals *)(&g_synch_globals))->critical_sections[21];
                RtlEnterCriticalSection((_RTL_CRITICAL_SECTION *)section->native_lock);
                _InterlockedExchange(&section->owner_thread, get_current_thread_index());
                _InterlockedIncrement(&section->reference_count);
                if (content_catalogue_storage_device_id(m_controller_index) != 0xcacacaca && content_catalogue_enumeration_is_complete(this)) {
                    void *datum = datum_try_and_get(m_data, index);
                    if (datum != 0) {
                        ((s_content_item *)datum)->get_display_name(item_name, 128);
                        match = item_name[0] != 0;
                    }
                }
                _InterlockedDecrement(&section->reference_count);
                RtlLeaveCriticalSection((_RTL_CRITICAL_SECTION *)section->native_lock);
                if (match && _wcsnicmp(display_name, item_name, 128) == 0)
                    return index;
            }
            index = content_catalogue_get_next_item_index(this, index);
        } while (index != -1);
    }
    return index;
}

long c_content_catalogue::try_and_get_personal_storage_item(const wchar_t *name, e_content_item_status content_item_status)
{
    long content_item_index = -1;
    c_saved_game_scope_lock lock;
    if ((unsigned int)content_item_status <= 14) {
        long index = content_catalogue_get_first_item_index(this);
        if (index != -1) {
            do {
                wchar_t item_name[128];
                if (content_catalogue_get_game_content_type(this, index) == content_item_status &&
                    content_catalogue_get_item_display_name(this, index, item_name, 128) && _wcsnicmp(name, item_name, 128) == 0)
                {
                    content_item_index = index;
                    break;
                }
                index = content_catalogue_get_next_item_index(this, index);
            } while (index != -1);
        }
    }
    return content_item_index;
}

extern "C" void content_catalogue_close_unlisted_map_items(c_content_catalogue *catalogue, int *left, int *right, long count)
{
    c_saved_game_scope_lock lock;
    if (content_catalogue_has_storage_device(catalogue) && content_catalogue_enumeration_is_complete(catalogue)) {
        c_content_catalogue_item_iterator iterator(catalogue->m_data);
        while (iterator.next()) {
            s_content_item *item = iterator.get_datum();
            if (item->game_content_type != 0)
                continue;
            bool found = false;
            char container_name[256];
            item->get_container_full_path(container_name, 0x100, false);
            for (long index = 0; index < count; index++) {
                char name[256];
                levels_get_map_scenario_path(left[index], right[index], name, 0x100);
                if (strstr(name, container_name) != 0) {
                    found = true;
                    break;
                }
            }
            if (!found)
                content_catalogue_close_item(catalogue, iterator.get_index(), true);
        }
    }
}


// match: this accepted caller supplies the legacy synchronous argument.

// @address 0x2856e0 content_catalogue_update_item_metadata
extern "C" unsigned char content_catalogue_update_item_metadata(c_content_catalogue *catalogue, int content_item_index)
{
    bool result = false;
    internal_critical_section_enter(21);
    if (content_catalogue_has_storage_device(catalogue))
    {
        s_content_item *item = content_catalogue_try_and_get_item(catalogue, content_item_index);
        if (item != 0)
            result = content_item_load_metadata(item, false);
    }
    _InterlockedDecrement(&((s_content_synchronization_globals *)&g_synch_globals)->critical_sections[21].reference_count);
    RtlLeaveCriticalSection((_RTL_CRITICAL_SECTION *)((s_content_synchronization_globals *)&g_synch_globals)->critical_sections[21].native_lock);
    return result;
}


// @address 0x285818 content_catalogue_handle_enumeration_step
extern "C" int content_catalogue_handle_enumeration_step(s_content_catalogue_enumeration_task *enumeration_task)
{
    int finished = 1;
    c_content_catalogue *catalogue = &g_content_catalogue_globals.catalogues[enumeration_task->controller_index];
    if (content_catalogue_has_storage_device(catalogue)) {
        if (content_catalogue_has_pending_tasks(catalogue)) {
            finished = 0;
        } else {
            long content_item_index = enumeration_task->content_item_index;
            finished = (content_item_index + 1) == 0;
            if ((unsigned int)finished == 0) {
                if (enumeration_task->state == 0) {
                    if (((s_content_item *)catalogue)->make_ready_for_access(content_item_index, false)) {
                        enumeration_task->state = 1;
                    } else {
                        enumeration_task->content_item_index = content_catalogue_get_next_item_index(
                            catalogue, enumeration_task->content_item_index);
                    }
                } else if (enumeration_task->state == 1) {
                    e_game_content_type type = content_catalogue_get_game_content_type(catalogue, content_item_index);
                    if (type != _game_content_type_savegame && type != _game_content_type_personal
                        && !content_catalogue_update_item_metadata(catalogue, enumeration_task->content_item_index)) {
                        wchar_t item_display_name[128];
                        content_catalogue_get_item_display_name(catalogue, enumeration_task->content_item_index,
                            item_display_name, 128);
                    }
                    content_catalogue_close_item(catalogue, enumeration_task->content_item_index, false);
                    enumeration_task->content_item_index = content_catalogue_get_next_item_index(
                        catalogue, enumeration_task->content_item_index);
                    enumeration_task->state = 0;
                }
            }
        }
    }
    return (unsigned char)finished != 0;
}



// @address 0x8aa78 content_catalogue_update_enumeration_task
extern "C" void content_catalogue_update_enumeration_task(c_content_catalogue *catalogue)
{
    if (catalogue->m_flags.test(0))
    {
        bool can_enumerate = false;
        catalogue->m_flags.set(0, false);
        if (catalogue->last_content_index != -1)
        {
            async_block_until_done(&catalogue->m_lock, simple_yield_function, 0, 1, 0, 0);
            catalogue->m_lock.set(-1);
        }
        if (content_catalogue_enumeration_is_complete(catalogue))
            can_enumerate = catalogue->m_data->actual_count > 0;
        if (can_enumerate)
        {
            catalogue->m_lock.set(0);
            s_content_catalogue_enumeration_task task;
            long controller_index = catalogue->m_controller_index;
            task.state = 0;
            memset(task.data, 0, sizeof(task.data));
            task.controller_index = controller_index;
            task.content_item_index = content_catalogue_get_first_item_index(content_catalogue_get((e_controller_index)controller_index));
            catalogue->m_lock.set(0);
            catalogue->last_content_index = async_task_add_ex(_async_priority_content_enumeration, (s_async_task *)&task,
                _async_category_saved_games, (e_async_completion (*)(s_async_task *))content_catalogue_handle_enumeration_step,
                &catalogue->m_lock, true);
            if (catalogue->last_content_index == -1)
                catalogue->m_flags.set(0, true);
        }
        else
        {
            catalogue->m_flags.set(1, true);
        }
    }
    if (catalogue->last_content_index != -1 && catalogue->m_lock.peek() == 1)
    {
        catalogue->m_lock.set(0);
        catalogue->last_content_index = -1;
        catalogue->m_flags.set(1, true);
    }
}



// @address 0x284b30 content_catalogue_item_metadata_matches
extern "C" bool content_catalogue_item_metadata_matches(
    const c_content_catalogue *catalogue, long content_item_index, s_saved_game_item_metadata_packed *metadata)
{
    bool result = false;
    internal_critical_section_enter(21);
    if (content_catalogue_has_storage_device(catalogue))
    {
        s_content_item *item = content_catalogue_try_and_get_item(catalogue, content_item_index);
        if (item != 0)
            result = content_item_try_and_get_metadata(item, metadata) && saved_game_metadata_is_valid(metadata);
    }
    _InterlockedDecrement(&((s_content_synchronization_globals *)&g_synch_globals)->critical_sections[21].reference_count);
    RtlLeaveCriticalSection((_RTL_CRITICAL_SECTION *)((s_content_synchronization_globals *)&g_synch_globals)->critical_sections[21].native_lock);
    return result;
}

// @address 0x46f830 content_catalogue_dispose_all
extern "C" void content_catalogue_dispose_all(void)
{
    for (e_controller_index controller_index = first_controller(); controller_index != k_no_controller;
        controller_index = next_controller(controller_index))
    {
    }
    xenon_player_profile_storage_dispose();
    c_saved_game_scope_lock lock;
    for (e_controller_index controller_index = first_controller(); controller_index != k_no_controller;
        controller_index = next_controller(controller_index))
    {
        c_content_catalogue *catalogue = &g_content_catalogue_globals.catalogues[controller_index];
        catalogue->dispose();
    }
    g_content_catalogue_globals.initialized = false;
}



// @address 0x198158 ??0?$c_static_wchar_string@$0BAA@@@QAA@XZ
template c_static_wchar_string<256>::c_static_wchar_string();

// @address 0x46b0e8 ??1s_user_interface_content_globals@@QAA@XZ
s_user_interface_content_globals::~s_user_interface_content_globals()
{
    // match: 0x46b0e8: load the singleton address before the reverse task destruction loop instead of deriving the array from r3.
    __assume(this == &g_user_interface_content_globals);
}

// @address 0x42380 ??_7c_storage_device_selector_overlapped_task@@6B@

// @address 0x282898 ??0s_content_catalogues@@QAA@XZ
s_content_catalogues::s_content_catalogues() : s_content_catalogues_base(this)
{
}

s_content_catalogues g_content_catalogue_globals;




static __forceinline bool content_catalogue_personal_available(c_content_catalogue *catalogue, const wchar_t *name, long content_type, bool check_personal)
{
    bool available_result;
    if (check_personal && catalogue->try_and_get_personal_storage_item(name, (e_content_item_status)content_type) == -1)
        available_result = true;
    else
        available_result = false;
    return available_result;
}
// @address 0x283f88 content_catalogue_is_name_available
extern "C" int content_catalogue_is_name_available(c_content_catalogue *catalogue, const wchar_t *name, long content_type)
{
    bool check_personal = true;
    long unique_ids[50];
    long count;
    levels_map_datum level;
    e_saved_game_file_type file_type = game_content_type_to_saved_game_file_type((e_game_content_type)content_type);
    if (file_type == _saved_game_file_type_map) {
        bool found = false;
        count = 50;
        levels_get_multiplayer_map_ids(unique_ids, &count);
        long index = 0;
        if (count > 0) {
            long *unique_id = unique_ids;
            do {
                if (levels_try_and_get_datum(g_level_globals.multiplayer_levels, *unique_id, &level) && _wcsicmp(name, level.display_name) == 0) {
                    found = true;
                    break;
                }
                ++index;
                ++unique_id;
            } while (index < count);
        }
        check_personal = !found;
    } else if ((unsigned int)(file_type - 1) <= 8) {
        bool found = false;
        e_game_engine_type engine_type = game_content_type_to_game_engine_type((e_game_content_type)content_type);
        if (engine_type != _game_engine_type_none && game_engine_settings_try_and_get() != 0) {
            long item_count = c_gui_game_variant_selected_item::get_game_engine_item_count(engine_type);
            long index = 0;
            if (item_count > 0) {
                do {
                    c_static_wchar_string<48> variant_name;
                    c_static_wchar_string<256> variant_description;
                    if (game_engine_tag_defined_variant_get_strings(engine_type, index, &variant_name, &variant_description) &&
                        _wcsicmp(name, variant_name.m_string) == 0)
                    {
                        found = true;
                        break;
                    }
                    ++index;
                } while (index < item_count);
            }
        }
        check_personal = !found;
    }
    bool missing = content_catalogue_personal_available(catalogue, name, content_type, check_personal);
    return (unsigned char)missing;
}


// @address 0x283a98 content_catalogue_get_remaining_slots
extern "C" long content_catalogue_get_remaining_slots(c_content_catalogue *catalogue, long game_content_type, long unused)
{
    c_saved_game_scope_lock lock;
    long remaining = game_content_type != _game_content_type_shot ? 100 : 50;
    if (content_catalogue_has_storage_device(catalogue) && catalogue->m_flags.test(1) && catalogue->m_data != 0 && catalogue->m_data->valid) {
        for (long index = content_catalogue_get_first_item_index(catalogue); index != -1; index = content_catalogue_get_next_item_index(catalogue, index)) {
            s_saved_game_item_metadata_packed metadata;
            if (!content_catalogue_item_metadata_matches(catalogue, index, &metadata))
                continue;
            if (game_content_type == _game_content_type_shot) {
                if (metadata.file_type == _game_content_type_clip)
                    --remaining;
            } else if ((unsigned long)(game_content_type - 1) <= 13 && metadata.file_type != _game_content_type_clip) {
                --remaining;
            }
        }
    }
    if (remaining <= 0) {
        if (game_content_type != _game_content_type_shot)
            g_gui_error_manager.post_error_with_custom_message(0x2008e, 0, 0, (e_controller_index)catalogue->m_controller_index, false, true);
        else
            g_gui_error_manager.post_error_with_custom_message(0x2008f, 0, 0, (e_controller_index)catalogue->m_controller_index, false, true);
    } else if (remaining <= 6) {
        if (game_content_type != _game_content_type_shot)
            g_gui_error_manager.post_error_with_custom_message(0x20090, 0, 0, (e_controller_index)catalogue->m_controller_index, false, true);
        else
            g_gui_error_manager.post_error_with_custom_message(0x20091, 0, 0, (e_controller_index)catalogue->m_controller_index, false, true);
    }
    return remaining;
}

// @address 0x284128 content_catalogue_is_item_name_valid
extern "C" bool content_catalogue_is_item_name_valid(
    c_content_catalogue *catalogue, e_controller_index controller_index, const wchar_t *name, long content_type)
{
    bool result = false;
    c_static_wchar_string<256> trimmed_name(name);
    content_catalogue_trim_trailing_whitespace((unsigned short *)trimmed_name.m_string);
    if (trimmed_name.length() == 0) {
        g_gui_error_manager.post_error_with_custom_message(0x20005, 0, 0, controller_index, false, true);
    } else if (!(unsigned char)content_catalogue_is_name_available(catalogue, trimmed_name.m_string, content_type)) {
        g_gui_error_manager.post_error_with_custom_message(0x20088, 0, 0, controller_index, false, true);
    } else {
        result = true;
    }
    return result;
}


// @address 0x2842e0 content_catalogue_new_item
extern "C" long content_catalogue_new_item(c_content_catalogue *catalogue, const wchar_t *name, const wchar_t *display_name,
    e_game_content_type content_type, bool open_existing, long maximum_size, bool synchronous)
{
    long item_index = -1;
    if (!synchronous)
        content_catalogue_block_until_finished(catalogue, 0, true, true);
    c_saved_game_scope_lock lock;
    if (content_catalogue_enumeration_is_complete(catalogue)) {
        item_index = catalogue->try_and_get_personal_storage_item(name, (e_content_item_status)content_type);
        if (item_index == -1 && content_catalogue_get_remaining_slots(catalogue, content_type, 1) > 0) {
            item_index = datum_new(catalogue->m_data);
            if (item_index != -1) {
                s_content_item *item = datum_get<s_content_item>(catalogue->m_data, item_index);
                item->flags.set(0);
                long creation_flags = 1 | (open_existing ? 4 : 0);
                if (!content_item_create(item, content_type, catalogue, name, display_name, creation_flags, maximum_size, synchronous)) {
                    datum_delete(catalogue->m_data, item_index);
                    item_index = -1;
                }
            }
        }
    }
    return item_index;
}
