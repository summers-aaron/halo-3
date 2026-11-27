// interfacelibxbox/c_player_profile_storage.obj: from the records 0x4a4988 0x4a4998 0x4a4a20 0x4a4c80 0x4a4da0 0x4a4e68 0x4a4e78 0x4a4ef0 0x4a4f20 0x4a5018 0x4a5158 0x4a52a8 0x4a55f8 0x4a57b8 0x4a5ce0
// @compiler xdk6130
// @flags /TP /O2 /Oc
#include "interface/controller_types.h"
#include "game/player_types.h"
#include <stddef.h>
#include "cseries/cseries_wrapped.h"
#include "game/game_types.h"
#include "shell/shell.h"
#include <xtl.h>
#include "multithreading/thread_tls.h"
#include <string.h>
#include "multithreading/synchronization.h"
#include "multithreading/synchronization_types.h"
#include "cseries/async_xoverlapped.h"
#include "cseries/async_xoverlapped_memory.h"
#include "interface/c_player_profile_storage.h"
#include "cseries/cseries.h"
#include "interface/user_interface_error_manager.h"
#include "interface/controller_inline.h"
#include "interface/look_sensitivity_types.h"
#include "networking/online/overlapped_allocation_marker_types.h"
#include "cseries/async_xoverlapped_types.h"
#include "interface/player_profile_types.h"
#include "interface/player_model_choice.h"
#include "interface/c_player_profile_storage_scope_lock.h"

typedef char profile_setting_size[(sizeof(void *) != 4 || ((sizeof(XUSER_PROFILE_SETTING) == 40) )) ? 1 : -1];
typedef char profile_setting_id_offset[(offsetof(XUSER_PROFILE_SETTING, dwSettingId) == 16) ? 1 : -1];
typedef char profile_setting_data_offset[(offsetof(XUSER_PROFILE_SETTING, data) == 24) ? 1 : -1];
typedef char profile_settings_result_size[(sizeof(void *) != 4 || ((sizeof(XUSER_READ_PROFILE_SETTING_RESULT) == 8) )) ? 1 : -1];

#include "interface/c_player_profile.h"

// The title-specific profile setting blob (setting 0x63e83fff), version 15.
#pragma pack(push, 4)
struct s_storage_block_version15
{
    long version;
    bool controller_look_inverted;
    bool controller_vibration_disabled;
    bool controller_flight_stick_aircraft_controls;
    bool controller_auto_center_look;
    unsigned char button_preset;
    unsigned char joystick_preset;
    unsigned char unknown_setting_1c;
    long unknown_setting_20;
    long unknown_setting_24;
    long unknown_setting_28;
    long unknown_setting_2c;
    long unknown_setting_30;
    long look_sensitivity;
    long last_campaign_map_played_absolute_index;
    unsigned char maps_completed[2][32];
    unsigned __int64 last_campaign_played_time[2];
    unsigned long campaign_map_unlocked;
    unsigned char campaign_map_completion[32];
    long primary_skulls_found;
    long secondary_skulls_found;
    short unknown_setting_98;
    short unknown_setting_9a;
    long last_network_game_hopper_played;
    long last_custom_multiplayer_map_played;
    long last_variant_game_engine;
    long last_variant_played;
    bool controller_crouch_lock_enabled;
    long primary_change_color;
    long secondary_change_color;
    long tertiary_change_color;
    long player_model_choice;
    s_emblem_info emblem_info;
    wchar_t service_tag_validated[4];
    wchar_t desired_service_tag[4];
    bool service_tag_was_randomly_generated;
    bool service_tag_failed_verification;
    long voice_output_setting;
    long voice_mask;
    long unknown_setting_104;
    long unknown_setting_108;
    char model_customization_selections[2][4];
    s_player_training_profile_data training_data;
    bool unknown_bool_114;
    bool unknown_bool_115;
    bool unknown_bool_116;
    long campaign_difficulty;
    long unknown_setting_c4;
    long reserved;
};
#pragma pack(pop)

#include "cseries/async_xoverlapped_allocation.h"

class c_profile_read_overlapped_task : public c_overlapped_task {
public:
    __forceinline c_profile_read_overlapped_task(const char *file, long line) : c_overlapped_task(file, line) {}
    virtual const char *get_context_string() const;
    virtual unsigned long start(void *overlapped);
    virtual void success(unsigned long return_result);
    virtual void failure(unsigned long calling_result, unsigned long overlapped_error, unsigned long overlapped_extended_error);
    virtual void complete();
    __forceinline void initialize(unsigned long title, int controller, XUSER_READ_PROFILE_SETTING_RESULT *buffer, unsigned long size)
    {
        title_id = title;
        controller_index = controller;
        results = buffer;
        buffer_size = size;
    }
    unsigned long title_id;
    long controller_index;
    XUSER_READ_PROFILE_SETTING_RESULT *results;
    unsigned long buffer_size;
};
class c_profile_write_overlapped_task : public c_overlapped_task {
public:
    __forceinline c_profile_write_overlapped_task(const char *file, long line) : c_overlapped_task(file, line) {}
    virtual const char *get_context_string() const;
    virtual unsigned long start(void *overlapped);
    virtual void failure(unsigned long calling_result, unsigned long overlapped_error, unsigned long overlapped_extended_error);
    virtual void complete();
    long controller_index;
    XUSER_PROFILE_SETTING *settings;
};

struct s_async_profile_io_storage {
    XUSER_READ_PROFILE_SETTING_RESULT *read_results_buffer;
    c_profile_read_overlapped_task *read_overlapped_task;
    c_profile_write_overlapped_task *write_overlapped_task;
    XUSER_PROFILE_SETTING write_request;
    s_storage_block_version15 write_storage;
};
typedef char profile_version15_size[(sizeof(s_storage_block_version15) == 0x130) ? 1 : -1];
typedef char profile_version15_emblem_offset[(offsetof(s_storage_block_version15, emblem_info) == 0xcc) ? 1 : -1];
typedef char profile_version15_voice_output_offset[(offsetof(s_storage_block_version15, voice_output_setting) == 0xe8) ? 1 : -1];
typedef char profile_version15_unknown_setting_c4_offset[(offsetof(s_storage_block_version15, unknown_setting_c4) == 0x128) ? 1 : -1];
typedef char profile_io_write_request_offset[(sizeof(void *) != 4 || ((offsetof(s_async_profile_io_storage, write_request) == 0x10))) ? 1 : -1];
typedef char profile_io_write_storage_offset[(sizeof(void *) != 4 || ((offsetof(s_async_profile_io_storage, write_storage) == 0x38))) ? 1 : -1];
typedef char profile_io_storage_size[(sizeof(void *) != 4 || ((sizeof(s_async_profile_io_storage) == 0x168))) ? 1 : -1];

// @address 0x24684 g_xenon_profile_setting_ids
// @address 0x897860 g_async_profile_io_storage
extern "C" {
s_async_profile_io_storage g_async_profile_io_storage[k_number_of_controllers];
}
const char *c_profile_read_overlapped_task::get_context_string() const
{
    return "XUserReadProfileSettings";
}
unsigned long c_profile_read_overlapped_task::start(void *overlapped)
{
    unsigned long result;
    if (controller_get((e_controller_index)controller_index)->is_signed_in_to_machine())
        result = XUserReadProfileSettings(title_id, controller_index, 14, g_xenon_profile_setting_ids,
            &buffer_size, results, (XOVERLAPPED *)overlapped);
    else
        result = 0x80004005;
    return result;
}
namespace {
static __forceinline s_async_profile_io_storage &profile_io_storage(e_controller_index controller_index)
{
    return g_async_profile_io_storage[controller_index];
}
static __forceinline unsigned long get_profile_read_buffer_size(e_controller_index controller_index,
    unsigned long count, const unsigned long *settings)
{
    unsigned long buffer_size = 0;
    unsigned long title_id = 0;
    unsigned long result = XUserReadProfileSettings(title_id, controller_index, count,
        settings, &buffer_size, 0, 0);
    if (result != ERROR_INSUFFICIENT_BUFFER) buffer_size = 0;
    return buffer_size;
}
}
// @address 0x4a4a20 ?success@c_profile_read_overlapped_task@@UAAXK@Z
void c_profile_read_overlapped_task::success(unsigned long return_result)
{
    c_player_profile_storage_scope_lock lock;
    XUSER_READ_PROFILE_SETTING_RESULT *results
        = (XUSER_READ_PROFILE_SETTING_RESULT *)profile_io_storage((e_controller_index)controller_index).read_results_buffer;
    profile_io_storage((e_controller_index)controller_index).read_results_buffer = 0;
    c_player_profile_interface *profile = controller_get((e_controller_index)controller_index)->get_player_profile_interface();
    const s_storage_block_version15 *block = 0;
    int blob_size = 0;
    bool no_settings = results->dwSettingsLen == 0;
    bool valid_settings = results->dwSettingsLen >= 13;
    bool found_blob = false;
    bool valid_blob = false;
    bool previously_retrieved = profile->get_title_specific_settings_previously_retrieved();
    if (valid_settings)
        profile->set_unsafe_to_save(false);
    for (unsigned int index = 0; index < results->dwSettingsLen; ++index) {
        XUSER_PROFILE_SETTING *setting = &results->pSettings[index];
        if (setting->dwSettingId == 0x63e83fff && setting->source == XSOURCE_TITLE && setting->data.type == XUSER_DATA_TYPE_BINARY) {
            found_blob = true;
            blob_size = setting->data.binary.cbData;
            if (blob_size > 0) {
                block = (const s_storage_block_version15 *)setting->data.binary.pbData;
                if (block != 0)
                    valid_blob = true;
                else
                    blob_size = 0;
            }
            break;
        }
    }
    if (previously_retrieved && (!found_blob || !valid_blob))
        profile->set_unsafe_to_save(true);
    if (found_blob && !valid_blob)
        profile->set_unsafe_to_save(true);
    if (valid_settings) {
        bool initialized = false;
        if (block) {
            if ((unsigned int)blob_size >= sizeof(block->version) && block->version == 15 && (unsigned int)blob_size == sizeof(*block)) {
                initialize_interface_with_xenon_global_gamercard_settings(block, profile);
                initialized = true;
            } else {
                profile->set_unsafe_to_save(true);
            }
        }
        const unsigned long *setting_ids = (const unsigned long *)g_xenon_profile_setting_ids;
        if (initialized) {
            profile->set_title_specific_settings_previously_retrieved(true);
        } else {
            player_profile_interface_apply_global_settings(profile, (const start_menu_settings_result *)results, setting_ids, 14);
        }
        player_profile_interface_apply_settings(profile, results, setting_ids);
    } else if (no_settings) {
        profile->set_unsafe_to_save(true);
    } else {
        profile->set_unsafe_to_save(true);
    }
    overlapped_free(results);
}

// @address 0x4a4c80 ?failure@c_profile_read_overlapped_task@@UAAXKKK@Z
void c_profile_read_overlapped_task::failure(unsigned long calling_result, unsigned long overlapped_error, unsigned long overlapped_extended_error)
{
    long error_name;
    switch (controller_index)
    {
    case _controller0:
        error_name = _player_profile_storage_string_player_profile_read_failed_controller0;
        break;
    case _controller1:
        error_name = _player_profile_storage_string_player_profile_read_failed_controller1;
        break;
    case _controller2:
        error_name = _player_profile_storage_string_player_profile_read_failed_controller2;
        break;
    case _controller3:
        error_name = _player_profile_storage_string_player_profile_read_failed_controller3;
        break;
    default:
        error_name = _player_profile_storage_string_player_profile_read_failed_controller0;
        break;
    }
    g_gui_error_manager.post_error_with_custom_message(error_name, 0, 0, (e_controller_index)controller_index, false, true);
    c_player_profile_storage_scope_lock lock;
    XUSER_READ_PROFILE_SETTING_RESULT *read_results = profile_io_storage((e_controller_index)controller_index).read_results_buffer;
    profile_io_storage((e_controller_index)controller_index).read_results_buffer = 0;
    c_controller_interface *controller = controller_get((e_controller_index)controller_index);
    c_player_profile_interface *profile = controller->get_player_profile_interface();
    profile->set_unsafe_to_save(true);
    overlapped_free(read_results);
}

const char *c_profile_write_overlapped_task::get_context_string() const
{
    return "XUserWriteProfileSettings";
}
unsigned long c_profile_write_overlapped_task::start(void *overlapped)
{
    unsigned long result;
    if (controller_get((e_controller_index)controller_index)->is_signed_in_to_machine())
        result = XUserWriteProfileSettings(controller_index, 1, settings, (XOVERLAPPED *)overlapped);
    else
        result = 0x80004005;
    return result;
}

// Empty body: the base failure is empty and March's event block compiles out in release.
// @address 0x5bec98 ?failure@c_profile_write_overlapped_task@@UAAXKKK@Z
void c_profile_write_overlapped_task::failure(unsigned long, unsigned long, unsigned long)
{
}

// @address 0x4a4ef0 ?complete@c_profile_write_overlapped_task@@UAAXXZ
void c_profile_write_overlapped_task::complete()
{
    c_player_profile_interface *profile = controller_get((e_controller_index)controller_index)->get_player_profile_interface();
    profile->synchronization_with_storage_complete();
    profile->propagate_settings();
}

// @address 0x4a4da0 ?complete@c_profile_read_overlapped_task@@UAAXXZ
void c_profile_read_overlapped_task::complete()
{
    c_player_profile_storage_scope_lock lock;
    if (profile_io_storage((e_controller_index)controller_index).read_results_buffer) {
        overlapped_free(profile_io_storage((e_controller_index)controller_index).read_results_buffer);
        profile_io_storage((e_controller_index)controller_index).read_results_buffer = 0;
    }
    c_player_profile_interface *profile = controller_get((e_controller_index)controller_index)->get_player_profile_interface();
    profile->synchronization_with_storage_complete();
    profile->propagate_settings();
}

void xenon_player_profile_storage_initialize()
{
    c_player_profile_storage_scope_lock lock;
    memset(g_async_profile_io_storage, 0, k_number_of_controllers * sizeof(s_async_profile_io_storage));
    for (unsigned long controller_index = 0; controller_index < k_number_of_controllers; controller_index++) {
        profile_io_storage((e_controller_index)controller_index).read_overlapped_task =
            new ("..\\interface\\c_player_profile_storage.cpp", 870, _overlapped_allocation_marker)
            c_profile_read_overlapped_task("..\\interface\\c_player_profile_storage.cpp", 870);
        profile_io_storage((e_controller_index)controller_index).write_overlapped_task =
            new ("..\\interface\\c_player_profile_storage.cpp", 872, _overlapped_allocation_marker)
            c_profile_write_overlapped_task("..\\interface\\c_player_profile_storage.cpp", 872);
    }
}
void xenon_player_profile_storage_dispose()
{
    for (e_controller_index controller_index = first_controller(); controller_index != k_no_controller;
        controller_index = next_controller(controller_index)) {
        c_player_profile_storage_inline_scope_lock lock;
        if (profile_io_storage(controller_index).read_results_buffer) {
            overlapped_free(profile_io_storage(controller_index).read_results_buffer);
            profile_io_storage(controller_index).read_results_buffer = 0;
        }
        if (profile_io_storage(controller_index).read_overlapped_task) {
            overlapped_track_delete(profile_io_storage(controller_index).read_overlapped_task);
            profile_io_storage(controller_index).read_overlapped_task = 0;
        }
        if (profile_io_storage(controller_index).write_overlapped_task) {
            overlapped_track_delete(profile_io_storage(controller_index).write_overlapped_task);
            profile_io_storage(controller_index).write_overlapped_task = 0;
        }
    }
}
void read_player_profile_interface_from_xenon_profile(e_controller_index controller_index,
    c_player_profile_interface *profile)
{
    if (controller_index >= 0 && controller_index < 4) {
        if (!profile_io_storage(controller_index).read_results_buffer) {
            unsigned long title_id = 0;
            unsigned long buffer_size = get_profile_read_buffer_size(controller_index, 14, g_xenon_profile_setting_ids);
            if (buffer_size > 0) {
                c_player_profile_storage_scope_lock lock;
                profile->set_async_io_busy(true);
                profile_io_storage(controller_index).read_results_buffer =
                    (XUSER_READ_PROFILE_SETTING_RESULT *)::overlapped_malloc_tracked(buffer_size, (char const *)(0), 0);
                if (profile_io_storage(controller_index).read_results_buffer) {
                    memset(profile_io_storage(controller_index).read_results_buffer, 0, buffer_size);
                    profile_io_storage(controller_index).read_overlapped_task->initialize(title_id, controller_index,
                        profile_io_storage(controller_index).read_results_buffer, buffer_size);
                    if (!overlapped_task_start_internal(profile_io_storage(controller_index).read_overlapped_task,
                        "..\\interface\\c_player_profile_storage.cpp", 1342)) {
                        overlapped_free(profile_io_storage(controller_index).read_results_buffer);
                        profile_io_storage(controller_index).read_results_buffer = 0;
                        profile->set_async_io_busy(false);
                    }
                } else {
                    profile->set_unsafe_to_save(true);
                    profile->set_async_io_busy(false);
                }
            }
        }
    }
}

// @address 0x4a55f8 player_profile_interface_apply_settings
void player_profile_interface_apply_settings(c_player_profile_interface *profile, XUSER_READ_PROFILE_SETTING_RESULT *results, const unsigned long *setting_ids)
{
    for (unsigned int index = 0; index < results->dwSettingsLen; ++index) {
        XUSER_PROFILE_SETTING *setting = &results->pSettings[index];
        if (setting->source == XSOURCE_DEFAULT) {
            switch (g_xenon_profile_setting_ids[index]) {
            case XPROFILE_GAMERCARD_ZONE:
                if (setting->data.type == XUSER_DATA_TYPE_INT32) {
                    int value = 1;
                    switch (setting->data.nData) {
                    case 0: value = 0; break;
                    case 1: value = 1; break;
                    case 2: value = 2; break;
                    case 3: value = 3; break;
                    case 4: value = 4; break;
                    }
                    profile->gamer_zone = value;
                }
                break;
            case XPROFILE_GAMERCARD_REGION:
                if (setting->data.type == XUSER_DATA_TYPE_INT32)
                    profile->gamer_region = get_dense_index_from_xbox_value(setting->data.nData);
                break;
            case XPROFILE_OPTION_VOICE_MUTED:
                if (setting->data.type == XUSER_DATA_TYPE_INT32)
                    profile->voice.xbox_voice_muted = setting->data.nData != 0;
                break;
            case XPROFILE_OPTION_VOICE_THRU_SPEAKERS:
                if (setting->data.type == XUSER_DATA_TYPE_INT32) {
                    switch (setting->data.nData) {
                    case 0: profile->voice.xbox_voice_output_setting = 0; break;
                    case 1: profile->voice.xbox_voice_output_setting = 1; break;
                    case 2: profile->voice.xbox_voice_output_setting = 2; break;
                    }
                }
                break;
            }
        }
    }
}

// @address 0x4a5ce0 ?initialize_saved_game_block_from_profile_interface@@YAXPBVc_player_profile_interface@@PAUs_storage_block_version15@@@Z
void initialize_saved_game_block_from_profile_interface(const c_player_profile_interface *profile,
    s_storage_block_version15 *block)
{
    memset(block, 0, sizeof(*block));
    block->version = 15;
    block->controller_look_inverted = (bool)(profile->flags & 1);
    block->controller_vibration_disabled = (bool)(_rotl(profile->flags, 31) & 1);
    block->controller_flight_stick_aircraft_controls = (bool)(_rotl(profile->flags, 30) & 1);
    block->controller_auto_center_look = (bool)(_rotl(profile->flags, 29) & 1);
    block->button_preset = (unsigned char)profile->controls.button_preset;
    block->joystick_preset = (unsigned char)profile->controls.joystick_preset;
    block->unknown_setting_1c = (unsigned char)profile->controls.unknown_setting_1c;
    block->unknown_setting_20 = profile->camera.unknown_setting_20;
    block->unknown_setting_24 = profile->camera.unknown_setting_24;
    block->unknown_setting_28 = profile->camera.unknown_setting_28;
    block->unknown_setting_2c = profile->camera.unknown_setting_2c;
    block->unknown_setting_30 = profile->camera.unknown_setting_30;
    block->look_sensitivity = profile->campaign.look_sensitivity;
    block->last_campaign_map_played_absolute_index = profile->campaign.last_campaign_map_played_absolute_index;
    for (long game_mode = 0; game_mode < 2; ++game_mode)
    {
        for (long map_index = 0; map_index < 32; ++map_index)
        {
            for (long difficulty = 0; difficulty < 4; ++difficulty)
            {
                if (profile->get_unknown_level_flag_40_inline(game_mode, map_index, (e_campaign_difficulty_level)difficulty))
                    block->maps_completed[game_mode][map_index] |= 1 << difficulty;
                else
                    block->maps_completed[game_mode][map_index] &= ~(1 << difficulty);
            }
        }
        block->last_campaign_played_time[game_mode] = profile->campaign.last_campaign_played_time[game_mode];
    }
    for (long map_index = 0; map_index < 32; ++map_index)
    {
        if (profile->get_unknown_flag_9c((short)map_index))
            block->campaign_map_unlocked |= 1 << map_index;
        else
            block->campaign_map_unlocked &= ~(1 << map_index);
        for (long difficulty = 0; difficulty < 4; ++difficulty)
        {
            if (profile->get_map_completed_at_difficulty_level_inline((short)map_index, difficulty))
                block->campaign_map_completion[map_index] |= 1 << difficulty;
            else
                block->campaign_map_completion[map_index] &= ~(1 << difficulty);
        }
    }
    block->primary_skulls_found = profile->campaign.primary_skulls_found;
    block->secondary_skulls_found = profile->campaign.secondary_skulls_found;
    block->unknown_setting_98 = profile->campaign.unknown_setting_98;
    block->unknown_setting_9a = profile->campaign.unknown_setting_9a;
    block->last_network_game_hopper_played = (unsigned short)profile->last_played.last_network_game_hopper_played;
    profile->get_last_variant_played(&block->last_variant_game_engine, &block->last_variant_played);
    block->last_custom_multiplayer_map_played = profile->last_played.last_custom_multiplayer_map_played;
    block->controller_crouch_lock_enabled = (bool)(_rotl(profile->flags, 28) & 1);
    block->primary_change_color = profile->customization.primary_change_color;
    block->secondary_change_color = profile->customization.secondary_change_color;
    block->tertiary_change_color = profile->customization.tertiary_change_color;
    block->player_model_choice = profile->get_player_model_choice();
    block->emblem_info = profile->get_emblem_info();
    memcpy(block->service_tag_validated, profile->get_service_tag_validated(), sizeof(block->service_tag_validated));
    memcpy(block->desired_service_tag, profile->get_desired_service_tag(), sizeof(block->desired_service_tag));
    block->service_tag_was_randomly_generated = profile->customization.service_tag_was_randomly_generated;
    block->service_tag_failed_verification = profile->customization.service_tag_failed_verification;
    for (unsigned long model = 0; model < 2; ++model)
    {
        for (unsigned long selection = 0; selection < 4; ++selection)
            block->model_customization_selections[model][selection] = profile->get_model_customization_selection_inline(model, selection);
    }
    block->voice_mask = profile->voice.voice_mask;
    block->unknown_setting_108 = profile->voice.unknown_setting_108;
    memcpy(&block->training_data, &profile->training_data, sizeof(block->training_data));
    block->unknown_bool_114 = profile->unknown_bool_114;
    block->unknown_bool_115 = profile->unknown_bool_115;
    block->unknown_bool_116 = profile->unknown_bool_116;
    block->campaign_difficulty = profile->last_played.campaign_difficulty;
    block->unknown_setting_c4 = profile->last_played.unknown_setting_c4;
}


// @address 0x4a52a8 ?write_player_profile_interface_to_xenon_profile@@YAXW4e_controller_index@@PAVc_player_profile_interface@@@Z
void write_player_profile_interface_to_xenon_profile(e_controller_index controller_index, c_player_profile_interface *profile)
{
    c_player_profile_storage_scope_lock lock;
    XUSER_PROFILE_SETTING *request = &profile_io_storage(controller_index).write_request;
    s_storage_block_version15 *block = &profile_io_storage(controller_index).write_storage;
    initialize_saved_game_block_from_profile_interface(profile, block);
    profile->set_async_io_busy(true);
    request->source = XSOURCE_TITLE;
    request->user.dwUserIndex = controller_index;
    request->dwSettingId = 0x63e83fff;
    request->data.type = XUSER_DATA_TYPE_BINARY;
    request->data.binary.cbData = 0x130;
    request->data.binary.pbData = (unsigned char *)block;
    c_profile_write_overlapped_task *write_task = profile_io_storage(controller_index).write_overlapped_task;
    write_task->controller_index = controller_index;
    write_task->settings = request;
    if (!overlapped_task_start_internal(profile_io_storage(controller_index).write_overlapped_task, "..\\interface\\c_player_profile_storage.cpp", 1404))
        profile->set_async_io_busy(false);
}

// @address 0x4a57b8 initialize_interface_with_xenon_global_gamercard_settings
void initialize_interface_with_xenon_global_gamercard_settings(const s_storage_block_version15 *block, c_player_profile_interface *profile)
{
    profile->set_controller_look_inverted(block->controller_look_inverted, false);
    bool vibration_disabled = block->controller_vibration_disabled;
    profile->dirty_or_from_gamercard(false);
    if (vibration_disabled)
        profile->flags |= 2;
    else
        profile->flags &= ~2;
    profile->set_controller_flight_inverted(block->controller_flight_stick_aircraft_controls, false);
    profile->set_controller_auto_center_look(block->controller_auto_center_look, false);
    int button_preset = block->button_preset;
    if (button_preset >= 0)
        profile->set_button_preset((e_button_presets)button_preset, false);
    int joystick_preset = block->joystick_preset;
    if (joystick_preset >= 0)
        profile->set_joystick_preset((e_joystick_presets)joystick_preset, false);
    int unknown_setting_1c = block->unknown_setting_1c;
    if (unknown_setting_1c >= 0)
        profile->set_unknown_setting_1c(unknown_setting_1c, false);
    if (block->unknown_setting_20 >= 0)
        profile->set_unknown_setting_20(block->unknown_setting_20, false);
    if (block->unknown_setting_24 >= 0)
        profile->set_unknown_setting_24(block->unknown_setting_24, false);
    if (block->unknown_setting_28 >= 0)
        profile->set_unknown_setting_28(block->unknown_setting_28, false);
    if (block->unknown_setting_2c >= 0)
        profile->set_unknown_setting_2c(block->unknown_setting_2c, false);
    if (block->unknown_setting_30 >= 0)
        profile->set_unknown_setting_30(block->unknown_setting_30, false);
    if (block->look_sensitivity >= 0)
        profile->set_look_sensitivity((e_look_sensitivity)block->look_sensitivity, false);
    profile->set_last_campaign_map_played_absolute_index((short)block->last_campaign_map_played_absolute_index, false);
    for (long mode = 0; mode < 2; mode++) {
        for (long level = 0; level < 32; level++) {
            for (long difficulty = 0; difficulty < 4; difficulty++) {
                profile->set_unknown_level_flag_40(mode, level, (e_campaign_difficulty_level)difficulty,
                    ((block->maps_completed[mode][level] & (1 << difficulty)) != 0), false);
            }
        }
        profile->set_last_campaign_played_time((e_campaign_game_mode)mode, block->last_campaign_played_time[mode], false);
    }
    profile->dirty_or_from_gamercard(false);
    memset(profile->campaign.campaign_map_unlocked, 0, sizeof(profile->campaign.campaign_map_unlocked));
    memset(profile->campaign.campaign_map_completion, 0, sizeof(profile->campaign.campaign_map_completion));
    for (long map = 0; map < 32; map++) {
        short short_map = (short)map;
        profile->set_unknown_flag_9c(short_map, (((1 << map) & block->campaign_map_unlocked) != 0), false);
        for (long difficulty = 0; difficulty < 4; difficulty++) {
            short short_difficulty = (short)difficulty;
            profile->set_map_completed_at_difficulty_level(short_map, short_difficulty,
                (((1 << difficulty) & block->campaign_map_completion[map]) != 0), false);
        }
    }
    long primary_skulls_found = block->primary_skulls_found;
    profile->dirty_or_from_gamercard(false);
    profile->campaign.primary_skulls_found = primary_skulls_found;
    long secondary_skulls_found = block->secondary_skulls_found;
    profile->dirty_or_from_gamercard(false);
    profile->campaign.secondary_skulls_found = secondary_skulls_found;
    short unknown_setting_98 = block->unknown_setting_98;
    profile->dirty_or_from_gamercard(false);
    profile->campaign.unknown_setting_98 = unknown_setting_98;
    short unknown_setting_9a = block->unknown_setting_9a;
    profile->dirty_or_from_gamercard(false);
    profile->campaign.unknown_setting_9a = unknown_setting_9a;
    unsigned short last_network_game_hopper_played = (unsigned short)block->last_network_game_hopper_played;
    profile->dirty_or_from_gamercard(false);
    profile->last_played.last_network_game_hopper_played = last_network_game_hopper_played;
    long last_variant_game_engine = block->last_variant_game_engine;
    long last_variant_played = block->last_variant_played;
    if ((unsigned long)last_variant_game_engine <= 10) {
        profile->dirty_or_from_gamercard((profile->last_played.last_variant_played - last_variant_played) != 0);
        profile->last_played.last_variant_game_engine = last_variant_game_engine;
        profile->last_played.last_variant_played = last_variant_played;
    }
    long last_custom_multiplayer_map_played = block->last_custom_multiplayer_map_played;
    if (last_custom_multiplayer_map_played == -1 || last_custom_multiplayer_map_played >= 0) {
        profile->dirty_or_from_gamercard(false);
        profile->last_played.last_custom_multiplayer_map_played = last_custom_multiplayer_map_played;
    }
    profile->set_controller_crouch_lock_enabled(block->controller_crouch_lock_enabled, false);
    long primary_change_color = block->primary_change_color;
    if (primary_change_color >= 0 && primary_change_color < 32) {
        profile->dirty_or_from_gamercard(false);
        profile->customization.primary_change_color = (e_player_color_index)primary_change_color;
    }
    long secondary_change_color = block->secondary_change_color;
    if (secondary_change_color >= 0 && secondary_change_color < 32) {
        profile->dirty_or_from_gamercard(false);
        profile->customization.secondary_change_color = (e_player_color_index)secondary_change_color;
    }
    long tertiary_change_color = block->tertiary_change_color;
    if (tertiary_change_color >= 0 && tertiary_change_color < 32) {
        profile->dirty_or_from_gamercard(false);
        profile->customization.tertiary_change_color = (e_player_color_index)tertiary_change_color;
    }
    profile->set_player_model_choice_inline((e_player_model_choice)block->player_model_choice, false);
    profile->set_emblem_info(&block->emblem_info, false);
    profile->set_desired_service_tag(block->desired_service_tag, false);
    profile->set_service_tag_validated(block->service_tag_validated, false);
    bool service_tag_was_randomly_generated = block->service_tag_was_randomly_generated;
    profile->dirty_or_from_gamercard(false);
    profile->customization.service_tag_was_randomly_generated = service_tag_was_randomly_generated;
    bool service_tag_failed_verification = block->service_tag_failed_verification;
    profile->dirty_or_from_gamercard(false);
    profile->customization.service_tag_failed_verification = service_tag_failed_verification;
    for (unsigned long model = 0; model < 2; model++) {
        for (unsigned long part = 0; part < 4; part++)
            profile->set_model_customization_selection((e_player_model_choice)model, part, block->model_customization_selections[model][part], false);
    }
    long voice_mask = block->voice_mask;
    if (voice_mask >= 0 && voice_mask < 2) {
        profile->dirty_or_from_gamercard(false);
        profile->voice.voice_mask = (e_voice_mask)voice_mask;
    }
    long unknown_setting_108 = block->unknown_setting_108;
    if (unknown_setting_108 >= 0 && unknown_setting_108 < 2) {
        profile->dirty_or_from_gamercard(false);
        profile->voice.unknown_setting_108 = unknown_setting_108;
    }
    profile->set_training_data(&block->training_data, false);
    for (long index = 0; index < 3; index++) {
        bool value = (&block->unknown_bool_114)[index];
        if (index >= 0 && index < 3) {
            profile->dirty_or_from_gamercard(false);
            (&profile->unknown_bool_114)[index] = value;
        }
    }
    long campaign_difficulty = block->campaign_difficulty;
    if (campaign_difficulty >= 0 && campaign_difficulty < 5) {
        profile->dirty_or_from_gamercard(false);
        profile->last_played.campaign_difficulty = (e_campaign_difficulty_level)campaign_difficulty;
    }
    long unknown_setting_c4 = block->unknown_setting_c4;
    profile->dirty_or_from_gamercard(false);
    profile->last_played.unknown_setting_c4 = unknown_setting_c4;
}
