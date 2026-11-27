// blamlibxbox/recorded_animations.obj: from the records 0xd3d40 0x241838 0x241c10 0x241c28 0x241c70 0x241c88 0x241ca0 0x241e68 0x241f08 0x241f98 0x242038 0x463810
#include "cutscene/recorded_animation_globals.h"
#include "cutscene/recorded_animation_playback_v1.h"
// @flags /TP /O2
#include "memory/tls_data_arrays.h"
#include "memory/tls_globals.h"
#include "gpu_particle/contrail_chain_types.h"
#include "memory/data_types.h"
#include "units/unit_control.h"
#include "objects/object_header.h"
#include "tag_files/tag_groups.h"
// @address 0x463810 ?__tls_set_g_animation_threads_allocator@@YAXPAX@Z
#include "scenario/scenario_tag.h"
#include "scenario/scenario.h"
#include "cache/restricted_memory_regions.h"
#include "objects/objects.h"
#include "units/units.h"
#include "cutscene/recorded_animations.h"
#include "units/vehicles.h"
#include "cache/restricted_memory.h"
#include "memory/data.h"

#include "units/unit_types.h"
#include "scenario/scenario_access.h"
#include "cache/restricted_memory_layout.h"
#include "cache/restricted_memory_registration.h"
#include "cache/restricted_memory_regions_inline.h"
#include "multithreading/thread_tls.h"

// @address 0x11df844 animation_threads
__declspec(thread) s_data_array *animation_threads;

struct s_recorded_animation_codec
{
    void (*initialize)(animation_playback_controller *controller, unit_control_data *control, unsigned char **event_stream,
        unsigned char unit_control_data_version);
    bool (*update)(animation_playback_controller *controller, unit_control_data *control, long *relative_ticks,
        unsigned char **event_stream);
};




// @address 0x241c10 ?__tls_set_g_animation_threads_data_allocator@@YAXPAX@Z
void __tls_set_g_animation_threads_data_allocator(void *allocation)
{
    data_set_new_base_address(&animation_threads_data(), (s_data_array *)allocation);
}

// @address 0x241c28 ?recorded_animations_initialize@@YAXXZ
void recorded_animations_initialize()
{
    animation_threads_data() = data_new("recorded animations", 1, sizeof(animation_thread), 0, (c_allocation_base *)&g_animation_threads_allocator);
}

// @address 0x5bec98 ?recorded_animations_dispose@@YAXXZ
void recorded_animations_dispose(void)
{
}

// @address 0x241c70 ?recorded_animations_initialize_for_new_map@@YAXXZ
void recorded_animations_initialize_for_new_map()
{
    data_make_valid(animation_threads_data());
}

// @address 0x241c88 ?recorded_animations_dispose_from_old_map@@YAXXZ
void recorded_animations_dispose_from_old_map()
{
    animation_threads_data()->valid = false;
}

extern "C" {
// @address 0x745b30 k_recorded_animation_codec_definitions
s_recorded_animation_codec k_recorded_animation_codec_definitions[2] =
{
    {
        (void (*)(animation_playback_controller *, unit_control_data *, unsigned char **, unsigned char))recorded_animation_initialize_event_stream,
        (bool (*)(animation_playback_controller *, unit_control_data *, long *, unsigned char **))recorded_animation_apply_event_stream
    },
    {
        (void (*)(animation_playback_controller *, unit_control_data *, unsigned char **, unsigned char))recorded_animation_initialize_event_stream_v1,
        (bool (*)(animation_playback_controller *, unit_control_data *, long *, unsigned char **))recorded_animation_apply_event_stream_v1
    }
};
}

extern "C" {
// @address 0x745b40 k_recorded_animation_codecs
s_recorded_animation_codec *k_recorded_animation_codecs[4] =
{
    &k_recorded_animation_codec_definitions[1],
    &k_recorded_animation_codec_definitions[1],
    &k_recorded_animation_codec_definitions[1],
    &k_recorded_animation_codec_definitions[0]
};
}


static __forceinline bool recorded_animations_unit_controllable(long unit_index)
{
    unit_datum *unit = OBJECT_GET(unit_index, unit_datum, _object_mask_unit);
    return (unit->unit.unit_flags & 1) != 0;
}
static __forceinline void recorded_animations_unit_set_controllable(long unit_index, bool controllable)
{
    unit_datum *unit = OBJECT_GET(unit_index, unit_datum, _object_mask_unit);
    if (controllable)
        unit->unit.unit_flags |= 1;
    else
        unit->unit.unit_flags &= ~1;
}
static __forceinline void recorded_animations_unit_set_possessed(long unit_index, bool possessed)
{
    unit_datum *unit = OBJECT_GET(unit_index, unit_datum, _object_mask_unit);
    if (possessed)
        unit->unit.unit_flags |= 0x200000;
    else
        unit->unit.unit_flags &= ~0x200000;
}

// @address 0x241ca0 ?recorded_animation_play_internal@@YA_NJFG@Z
bool recorded_animation_play_internal(long unit_index, short animation_index, unsigned short play_flags)
{
    bool result = false;
    if (unit_index != -1)
    {
        if (animation_index != -1 && animation_index < global_scenario_get()->recorded_animations.count)
        {
            long thread_index;
            animation_thread *thread = get_controlling_thread(unit_index, &thread_index);
            recorded_animation_definition *definition = TAG_BLOCK_GET_ELEMENT(&global_scenario_get()->recorded_animations, animation_index,
                recorded_animation_definition);
            if (recorded_animation_controlling_unit(unit_index) == 0)
            {
                if (thread == 0)
                {
                    thread_index = ::datum_new(animation_threads_data());
                    if (thread_index != -1)
                        thread = datum_get<animation_thread>(animation_threads_data(), thread_index);
                }
                if (thread != 0)
                {
                thread->unit_index = unit_index;
                thread->relative_ticks = 0;
                thread->ticks = definition->length_of_animation;
                thread->event_stream = (unsigned char *)tag_data_get_pointer(&definition->recorded_animation_event_stream, 0,
                    definition->recorded_animation_event_stream.size);
                thread->codec_index = definition->version - 1;
                thread->flags &= ~1;
                k_recorded_animation_codecs[thread->codec_index]->initialize(
                    &thread->playback_controller, &thread->control_data, &thread->event_stream, definition->unit_control_data_version);
                unit_set_actively_controlled(unit_index, true);
                if (recorded_animations_unit_controllable(unit_index))
                    thread->flags |= 4;
                else
                    thread->flags &= ~4;
                recorded_animations_unit_set_controllable(unit_index, false);
                recorded_animations_unit_set_possessed(unit_index, true);
                thread->flags = thread->flags | play_flags;
                result = true;
                }
            }
        }
    }
    return result;
}

// @address 0x241e68 ?recorded_animation_controlling_unit@@YA_NJ@Z
bool recorded_animation_controlling_unit(long unit_index)
{
    bool result = false;
    c_data_iterator<animation_thread> iterator;
    iterator.begin(animation_threads_data());
    while (iterator.next())
    {
        animation_thread *thread = iterator.get_datum();
        if (thread->unit_index == unit_index && (thread->flags & 1) == 0)
        {
            result = true;
            break;
        }
    }
    return result;
}


// @address 0x241f08 ?get_controlling_thread@@YAPAUanimation_thread@@JPAJ@Z
animation_thread *get_controlling_thread(long unit_index, long *thread_index)
{
    animation_thread *result = 0;
    long result_index = -1;
    c_data_iterator<animation_thread> iterator;
    iterator.begin(animation_threads_data());
    while (iterator.next())
    {
        animation_thread *thread = iterator.get_datum();
        if (thread->unit_index == unit_index)
        {
            result_index = iterator.get_index();
            result = thread;
            break;
        }
    }
    if (thread_index)
        *thread_index = result_index;
    return result;
}

// @address 0xd3d40 ?recorded_animations_update@@YAXXZ
void recorded_animations_update()
{
    c_data_iterator<animation_thread> iterator;
    iterator.begin(animation_threads_data());
    while (iterator.next())
    {
        animation_thread *thread = iterator.get_datum();
        unit_datum *unit = (unit_datum *)object_try_and_get_and_verify_type_inline(thread->unit_index, _object_mask_unit);
        if (unit != 0)
        {
            if ((thread->flags & 1) == 0)
            {
                bool finished;
                unit_control_data *control = &thread->control_data;
                thread->ticks = thread->ticks - 1;
                finished = k_recorded_animation_codecs[thread->codec_index]->update(
                               &thread->playback_controller, control, &thread->relative_ticks, &thread->event_stream) == 0;
                thread->relative_ticks = thread->relative_ticks + 1;
                unit_control(thread->unit_index, control);
                if (finished)
                    thread->flags |= 1;
                else
                    thread->flags &= ~1;
            }
            else
            {
                recorded_animations_unit_set_controllable(thread->unit_index, (thread->flags & 4) != 0);
                recorded_animations_unit_set_possessed(thread->unit_index, false);
                unit_set_actively_controlled(thread->unit_index, false);
                if ((thread->flags & 8) != 0)
                {
                    long unit_index = thread->unit_index;
                    if (unit_index != -1 && !object_is_or_contains_player(unit_index))
                        object_delete(unit_index);
                }
                if ((thread->flags & 16) != 0)
                    vehicle_hover(thread->unit_index, true);
                ::datum_delete(animation_threads_data(), iterator.get_index());
            }
        }
        else
        {
            ::datum_delete(animation_threads_data(), iterator.get_index());
        }
    }
}

namespace {
static __forceinline long animation_threads_add_member(const char *name, const char *type, unsigned int size, long alignment_bits)
{
    long member_index = restricted_memory_register_member(&g_restricted_regions[3], name, type, size,
        alignment_bits, __tls_set_g_animation_threads_data_allocator, 0, 0);
    void *address = restricted_region_get_member_address(3, member_index);
    data_set_new_base_address(&animation_threads_data(), (s_data_array *)address);
    return member_index;
}

}
template<> __forceinline void *t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_animation_threads_allocator>::reserve_memory(const char *name, const char *type, unsigned int size,
    long alignment_bits)
{
    m_member_index = animation_threads_add_member(name, type, size, alignment_bits);
    m_thread_id = get_current_thread_index();
    return restricted_region_get_member_address(3, m_member_index);
}
// @address 0x241838 ?allocate@?$t_restricted_allocation_manager@$02$0A@$0A@$1?__tls_set_g_cinematic_light_globals_allocator@@YAXPAX@Z@@UAAPAXIPBD@Z
template<>
void *t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_cinematic_light_globals_allocator>::allocate(unsigned int size, const char *name)
{
    return reserve_memory(name, "", size, 0);
}
// @address 0x241f98 ?allocate@?$t_restricted_allocation_manager@$02$0A@$0A@$1?__tls_set_g_animation_threads_allocator@@YAXPAX@Z@@UAAPAXIPBD@Z
template<>
void *t_restricted_allocation_manager<3,static_cast<void (*)(void *)>(0),static_cast<void (*)(void *)>(0),
    __tls_set_g_animation_threads_allocator>::allocate(unsigned int size, const char *name)
{
    return reserve_memory(name, "", size, 0);
}
// @address 0x242038 ?deallocate@?$t_restricted_allocation_manager@$02$0A@$0A@$1?__tls_set_g_animation_threads_allocator@@YAXPAX@Z@@UAAXPAX@Z
template<>
void t_restricted_allocation_manager<3,static_cast<void (*)(void *)>(0),static_cast<void (*)(void *)>(0),
    __tls_set_g_animation_threads_allocator>::deallocate(void *allocation)
{
    animation_threads_data() = 0;
    restricted_region_free_member(3, m_member_index);
}
void __tls_set_g_animation_threads_allocator(void *object)
{
    data_set_new_base_address(&g_contrail_chain_data, (s_data_array *)object);
}


extern "C" {
t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_cinematic_light_globals_allocator> g_cinematic_light_globals_allocator;
}
t_restricted_allocation_manager<3,static_cast<void (*)(void *)>(0),static_cast<void (*)(void *)>(0),
    __tls_set_g_animation_threads_allocator> g_animation_threads_allocator;

