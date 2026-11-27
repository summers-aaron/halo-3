// ailibxbox/joint_behavior.obj: from the records 0xb6060 0xc4650 0xc4968 0xc82c0 0x5541e8 0x558138 0x5581d8 0x558210 0x558338 0x558478 0x5584f0 0x558718 0x558808 0x5588a8 0x558950 0x5589a8 0x558a98 0x558b38 0x558c28 0x558c80 0x558db0 0x558e88 0x558f60 0x559018 0x559190 0x559218 0x5592f0 0x559548 0x559620 0x5596a0
// @address 0x11df6b4 g_joint_data
// @compiler xdk6130
// @flags /TP /O2 /Oc
// @address 0x559018 ?joint_accept@@YAJJPAUinvitation_data@@PAVc_joint_behavior_data@@@Z
// @address 0x559190 ?joint_decline@@YA_NPAUinvitation_data@@@Z

// @address 0x5541e8 ?__tls_set_g_joint_data_allocator@@YAXPAX@Z
// @address 0x558210 ??0Joint_mutex_group@@QAA@FJP6AFJPBUs_behavior_input@@PBUs_behavior_arguments@@@ZP6AFJPAVc_behavior_state@@3@ZP6AJJ3@ZP6AXJ3PAUjoint_state_datum@@@ZFP6AFJF36@ZFPAUchild_entry@@W4e_joint_type@@P6AFJJ3@ZFFMP6AXJ3J@ZP6AXJ3J@ZP6AXJ3J@ZP6AXJ3@ZP6AXJ3_N@Z7P6AX3@Z@Z
// @address 0x559218 joint_reject

#include "memory/tls_globals.h"
#include "multithreading/thread_tls.h"
#include "ai/clump.h"
#include "ai/clump_importance_types.h"
#include "ai/joint_data.h"
#include "math/real_math_types.h"
#include "math/real_math.h"
#include "memory/data_types.h"
#include "ai/joint_behavior.h"
#include "game/game_time_types.h"
#include <math.h>
#include "cache/restricted_memory_regions.h"
#include "objects/objects.h"
#include "ai/actor_dynamic_firing_position.h"
#include "ai/props.h"
#include "memory/data.h"
#include "cseries/maximum.h"
#include "ai/joint_behavior_types.h"
#include "ai/actor_datum.h"
#include "ai/clump_datum.h"
#include "ai/behavior.h"
#include "ai/behavior_info.h"
#include "ai/actor_type_grunt_types.h"
#include "ai/prop_datum.h"
#include "game/game_time_inline.h"
#include "ai/prop_ref_datum.h"
#include "ai/prop_state.h"
#include "cache/restricted_memory_regions_inline.h"
#include "cache/restricted_memory_registration.h"
#include "ai/joint_behavior_allocators.h"
#include "ai/prop_view_inline.h"

// TLS slot 0x831df6b4, tagged at the top of this file.
__declspec(thread) s_data_array *g_joint_data;

static __forceinline invitation_data *actor_get_invitation(long actor_index, short invitation_index)
{
  actor_datum *actor = datum_get<actor_datum>(tls_actor_data(), actor_index);
  return &actor->invitations[invitation_index];
}
static __forceinline bool invitation_valid(const invitation_data *invitation)
{
  return invitation->type != -1 && invitation->handle != -1;
}

namespace {
static __forceinline joint_state_datum *joint_get(long index)
{
    return datum_get<joint_state_datum>(joint_state_data(), index);
}
static __forceinline void invitation_clear(invitation_data *invitation)
{
    invitation->type = -1;
    invitation->time = 0;
    invitation->handle = -1;
    invitation->slot = -1;
    invitation->mode = 0;
}
}
// @address 0xb6060 ?actor_invitations_update@@YAXJ@Z
void actor_invitations_update(long actor_index)
{
    for (short index = 0; index < 4; index++) {
        invitation_data *invitation = actor_get_invitation(actor_index, index);
        if (invitation_valid(invitation) && invitation->time != -1 && invitation->time < game_time_get()) {
            if (invitation->mode == 1)
                joint_decline(invitation);
            invitation_clear(invitation);
        }
    }
}
// @address 0xc4968 ?actor_invited@@YA_NJF@Z
bool actor_invited(long actor_index, short invitation_type)
{
    bool invited = false;
    for (short index = 0; index < 4; index++) {
        invitation_data *invitation = actor_get_invitation(actor_index, index);
        if (invitation->type == invitation_type && invitation->time > game_time_get()) {
            invited = true;
            break;
        }
    }
    return invited;
}
// @address 0xc82c0 ?joint_invitation_iterator_next@@YAPAUinvitation_data@@PAUjoint_invitation_iterator@@@Z
invitation_data *joint_invitation_iterator_next(joint_invitation_iterator *iterator)
{
    invitation_data *result = 0;
    if (!iterator->done) {
        if (iterator->index < 4) {
            if (iterator->index == -1) iterator->index = 0;
            else iterator->index++;
            while (iterator->index < 4) {
                invitation_data *invitation = actor_get_invitation(iterator->actor_index, iterator->index);
                if (invitation_valid(invitation) && invitation->type == iterator->type) {
                    result = invitation;
                    break;
                } else {
                    iterator->index++;
                    continue;
                }
            }
        }
        if (result == 0) {
            iterator->index = -1;
            iterator->done = true;
            result = joint_invitation_iterator_next(iterator);
        }
    } else if (iterator->index < 10) {
        actor_datum *actor = datum_get<actor_datum>(tls_actor_data(), iterator->actor_index);
        if (actor->clump_index != -1) {
            clump_datum *clump = datum_get<clump_datum>(tls_clump_data(), actor->clump_index);
            if (iterator->index == -1) iterator->index = 0;
            else iterator->index++;
            while (iterator->index < 10) {
                invitation_data *invitation = &clump->invitations[iterator->index];
                if (invitation_valid(invitation) && invitation->type == iterator->type) {
                    result = invitation;
                    break;
                } else {
                    iterator->index++;
                    continue;
                }
            }
        }
    }
    return result;
}


static __forceinline void joint_behavior_invitation_iterator_new(long actor_index, short type, joint_invitation_iterator *iterator)
{
    iterator->actor_index = actor_index;
    iterator->type = type;
    iterator->index = -1;
    iterator->done = false;
}
// 0xc4650
// @address 0xc4650 ?behavior_surprise_get_desire@@YAFJPBUs_behavior_input@@PBUs_behavior_arguments@@@Z
short behavior_surprise_get_desire(long actor_index, const s_behavior_input *, const s_behavior_arguments *)
{
    short desire = 0;
    actor_datum *actor = datum_get<actor_datum>(tls_actor_data(), actor_index);
    prop_ref_datum *prop_ref = 0;
    prop_ref_datum *target = 0;
    if (actor->target_prop_index != -1)
        target = datum_get<prop_ref_datum>(tls_prop_ref_data(), actor->target_prop_index);
    if (!actor->suppress_combat) {
        if (actor_invited(actor_index, 29)) {
            joint_invitation_iterator iterator;
            joint_behavior_invitation_iterator_new(actor_index, 29, &iterator);
            invitation_data *invitation;
            while ((invitation = joint_invitation_iterator_next(&iterator)) != 0) {
                joint_state_datum *joint = datum_get<joint_state_datum>(joint_state_data(), actor->invitations[iterator.index].handle);
                if (target && joint->object_index == target->object_index) {
                    prop_ref = target;
                    desire = joint->participants[actor->invitations[iterator.index].slot].desire;
                    break;
                }
                if (actor->combat_status < 4) {
                    long prop_ref_index = prop_ref_get_by_object_index(actor_index, joint->object_index, false, _clump_importance_none);
                    if (prop_ref_index != -1) {
                        prop_ref = datum_get<prop_ref_datum>(tls_prop_ref_data(), prop_ref_index);
                        desire = joint->participants[actor->invitations[iterator.index].slot].desire;
                        break;
                    }
                }
                if (invitation->mode == 1)
                    joint_decline(invitation);
            }
        } else if (actor->clump_index != -1 && target) {
            desire = 3;
            prop_ref = datum_get<prop_ref_datum>(tls_prop_ref_data(), actor->target_prop_index);
            if (prop_ref->awareness < 2)
                desire = 0;
        }
    }
    if (desire > 0) {
        prop_state *state = prop_state_get(prop_ref);
        prop_view *view = prop_view_get(prop_ref);
        real_vector3d velocity;
        object_get_velocities(prop_ref->object_index, &velocity, 0);
        if (!view || prop_ref->distance <= 1.6f || magnitude3d(&velocity) >= 1.0f || view->vitality >= 0.31f ||
            actor->situation.best_prop_class >= 9 || prop_ref->distance > 15.0f || state->dead || pref_unreachable(actor->target_prop_index))
            desire = 0;
    } else {
        desire = 0;
    }
    return desire;
}
void __tls_set_g_joint_data_allocator(void *allocator)
{
  data_set_new_base_address(&clump_data, (s_data_array *)allocator);
}
// @address 0x558c80 ?actor_find_empty_invitation_index@@YAFJF@Z
short actor_find_empty_invitation_index(long actor_index, short invitation_type)
{
  short invitation_index = -1;
  short lowest_invitation_type = 8;
  short lowest_invitation_index = -1;
  for (short i = 0; i < 4; i++) {
    invitation_data *invitation = actor_get_invitation(actor_index, i);
    if (!invitation_valid(invitation)) {
      invitation_index = i;
      break;
    }
    joint_state_datum *jstate = datum_get<joint_state_datum>(joint_state_data(), invitation->handle);
    if (jstate->participants[invitation->slot].desire < lowest_invitation_type) {
      lowest_invitation_type = jstate->participants[invitation->slot].desire;
      lowest_invitation_index = i;
    }
  }
  if (invitation_index == -1 && lowest_invitation_type < invitation_type && lowest_invitation_index != -1) {
    invitation_data *invitation = actor_get_invitation(actor_index, lowest_invitation_index);
    if (invitation->mode == 1) joint_decline(invitation);
    invitation_index = lowest_invitation_index;
  }
  return invitation_index;
}

// @address 0x558db0 ?invite_actor@@YA_NJJJFM@Z
bool invite_actor(long inviter_index, long actor_index, long joint_index, short invitation_type, float weight)
{
    bool success = false;
    short invitation_index = actor_find_empty_invitation_index(actor_index, 3);
    if (invitation_index != -1) {
        joint_state_datum *joint = joint_get(joint_index);
        if (joint->joint_type == 1) {
            short participant;
            for (participant = 0; participant < 10 && joint->participants[participant].actor_index != -1; participant++) {}
            if (participant < 10) {
                joint->participants[participant].actor_index = actor_index;
                joint->participants[participant].weight = weight;
                joint->participants[participant].desire = 3;
                success = true;
            }
        }
    }
    return success;
}
// @address 0x558e88 ?joint_submit_invitation_all@@YAXJPAUinvitation_data@@@Z
void joint_submit_invitation_all(long actor_index, invitation_data *invitation)
{
    // match: 0x558e88: pointer __restrict qualifier prevents alias reload
    invitation_data *__restrict source = invitation;
    joint_state_datum *joint = joint_get(invitation->handle);
    for (short participant = 0; participant < 10; participant++) {
        long participant_actor = joint->participants[participant].actor_index;
        if (participant_actor != -1 && participant_actor != actor_index) {
            actor_datum *actor = datum_get<actor_datum>(tls_actor_data(), participant_actor);
            short invitation_index = actor_find_empty_invitation_index(participant_actor, joint->participants[participant].desire);
            if (invitation_index != -1) {
                actor->invitations[invitation_index] = *source;
                actor->invitations[invitation_index].slot = participant;
            }
        }
    }
}
namespace {
// match: 0x5584f0: __restrict keeps the invitation copy as 3 bulk loads then 3 stores; without it each word copies as an interleaved lwz/stw pair
__forceinline void joint_behavior_clump_post_invitation(long index, invitation_data *__restrict invitation) {
    clump_datum *clump = datum_get<clump_datum>(tls_clump_data(), index);
    invitation_data *entry = 0;
    for (short i = 0; i < 10; i++) {
        entry = &clump->invitations[i];
        if (!invitation_valid(entry)) { invitation_data copy = *invitation; *entry = copy; break; }
    }
}
}
static __forceinline short joint_behavior_num_undecided(joint_state_datum *state)
{
    short count = 0;
    for (short i = 0; i < 10; ++i)
        if (state->participants[i].actor_index != -1 && state->participants[i].status == 0)
            ++count;
    return count;
}
// @address 0x558338 ?joint_state_update@@YAXJ@Z
void joint_state_update(long i)
{
    joint_state_datum *state = joint_get(i);
    short minimum, maximum;
    bool limits_success = joint_behavior_get_participant_limits(state->behavior_index, &minimum, &maximum);
    switch (state->status) {
    case 0:
        --state->invitation_timer;
        if (state->invitation_timer <= 0 || (state->joint_type == 1 && joint_behavior_num_undecided(state) == 0) || (state->joint_type == 2 &&
            state->participant_count >= minimum)) {
            if (state->participant_count >= minimum) {
                if (state->joint_type == 1 && state->participant_count >= maximum)
                    choose_participants(i, minimum, maximum);
                state->status = 1;
            } else
                state->status = 3;
        }
        break;
    }
}

// @address 0x558478 actor_joint_state_update
extern "C" bool actor_joint_state_update(long actor_index, c_joint_behavior_data *data, long joint_index, short minimum, short maximum)
{
    joint_state_datum *jstate = joint_get(joint_index);
    bool result = true;
    if (jstate->status == 3 || jstate->status == 2)
        result = false;
    else if (jstate->joint_type == 1 && jstate->participants[data->slot].status == 2)
        result = false;
    data->jstatus = jstate->status;
    return result;
}

// @address 0x5584f0 joint_post_invitation
extern "C" bool joint_post_invitation(long actor_index, c_joint_behavior_data *data, long joint_index, long type, short minimum,
    float seconds, short (*count)(long, long, c_behavior_state *))
{
    bool success = false;
    joint_state_datum *joint = joint_get(joint_index);
    long duration = 0;
    bool infinite = false;
    if (seconds < 0.0f) infinite = true;
    else duration = maximum<long>(game_seconds_to_ticks_round(seconds), 2);
    invitation_data invitation;
    invitation.handle = joint_index;
    invitation.type = data->behavior_index;
    invitation.time = infinite ? -1 : game_time_get() + duration;
    invitation.slot = 0;
    invitation.mode = type;
    joint->joint_type = type;
    joint->invitation_timer = (short)duration;
    switch (type) {
    case 1: {
        short participants = count(actor_index, joint_index, data);
        success = participants + 1 >= minimum;
        if (success) joint_submit_invitation_all(actor_index, &invitation);
        break;
    }
    case 2: {
        actor_datum *actor = datum_get<actor_datum>(tls_actor_data(), actor_index);
        if (actor->clump_index != -1) {
            joint_behavior_clump_post_invitation(actor->clump_index, &invitation);
            joint->status = 1;
            joint->clump_index = actor->clump_index;
            success = true;
        }
        break;
    }
    }
    return success;
}

namespace {
static __forceinline long joint_new_internal(long actor_index, short behavior_index)
{
    long joint_index = ::datum_new(joint_state_data());
    if (joint_index != -1) {
        joint_state_datum *joint = joint_get(joint_index);
        for (short participant = 1; participant < 10; participant++) {
            joint->participants[participant].actor_index = -1;
            joint->participants[participant].status = 0;
        }
        joint->behavior_index = behavior_index;
        joint->joint_type = 0;
        joint->status = 0;
        joint->participants[0].actor_index = actor_index;
        joint->participants[0].status = 3;
        joint->participant_count = 1;
        joint->clump_index = -1;
        joint->invitation_timer = 0;
    }
    return joint_index;
}
}
// @address 0x558f60 ?joint_new@@YAJJFPAVc_joint_behavior_data@@@Z
long joint_new(long actor_index, short behavior_index, c_joint_behavior_data *joint_behavior)
{
    long joint_index = joint_new_internal(actor_index, behavior_index);
    if (joint_index != -1) {
        joint_behavior->initiator = 1;
        joint_behavior->joint_index = joint_index;
        joint_behavior->slot = 0;
        joint_behavior->jstatus = 0;
    }
    return joint_index;
}
long joint_accept(long actor_index, invitation_data *invitation, c_joint_behavior_data *joint_behavior)
{
    int result = -1;
    bool has_participant_limit = false;
    short participant_limit;
    joint_state_datum *element = datum_get<joint_state_datum>(joint_state_data(), invitation->handle);
    Behavior_header *type_definition = global_behavior_info[element->behavior_index].header;
    int kind = (short)type_definition->type;
    if (kind == 2 || kind == 1) {
        if (static_cast<Behavior *>(type_definition)->joint != 0) {
            if (kind == 2) {
                participant_limit = ((Joint_primitive_behavior *)type_definition)->maximum_participants;
                has_participant_limit = true;
            } else if (kind == 1) {
                participant_limit = ((Joint_primitive_behavior *)type_definition)->maximum_participants;
                has_participant_limit = true;
            }
        }
    }
    if (has_participant_limit) {
    if (participant_limit == -1 || (element->joint_type == 2 && element->participant_count < participant_limit) || (element->joint_type == 1 &&
        element->status == 0)) {
        if (invitation->handle != -1) {
            joint_behavior->initiator = 0;
            joint_behavior->joint_index = invitation->handle;
            element->participant_count++;
            result = invitation->handle;
            if (invitation->mode == 1) {
                element->participants[invitation->slot].status = 3;
                joint_behavior->slot = invitation->slot;
                invitation->type = -1;
                invitation->time = 0;
                invitation->handle = -1;
                invitation->slot = -1;
                invitation->mode = 0;
            } else {
                joint_behavior->slot = -1;
            }
        }
    }
    }
    return result;
}

bool joint_decline(invitation_data *invitation)
{
    joint_state_datum *element = datum_get<joint_state_datum>(joint_state_data(), invitation->handle);
    element->participants[invitation->slot].actor_index = -1;
    element->participants[invitation->slot].status = 1;
    invitation->type = -1;
    invitation->time = 0;
    invitation->handle = -1;
    invitation->slot = -1;
    invitation->mode = 0;
    return true;
}


void joint_reject(long index, short slot_index)
{
  joint_member *participant = &datum_get<joint_state_datum>(joint_state_data(), index)->participants[slot_index];
  if (participant->status == 0) {
    actor_datum *player = datum_get<actor_datum>(tls_actor_data(), participant->actor_index);
    for (short i = 0; i < 4; i++) {
      invitation_data *ref = &player->invitations[i];
      if ((unsigned short)ref->type != 0xffff && ref->handle == index) {
        if (ref->mode == 1) joint_decline(ref);
        break;
      }
    }
  }
  participant->status = 2;
}
// @address 0x559548 ?choose_participants@@YAXJFF@Z
void choose_participants(long joint_index, short minimum_participants, short maximum_participants)
{
    joint_state_datum *joint = joint_get(joint_index);
    short count = joint->participant_count;
    while (count > maximum_participants) {
        float lowest_weight = 3.402823466e+38F;
        short lowest_participant = -1;
        for (short participant = 1; participant < 10; participant++) {
            joint_member *slot = &joint->participants[participant];
            if (slot->actor_index != -1 && slot->status == 3 && slot->weight < lowest_weight) {
                lowest_weight = slot->weight;
                lowest_participant = participant;
            }
        }
        if (lowest_participant != -1) {
            joint_reject(joint_index, lowest_participant);
            count--;
            continue;
        } else break;
    }
}

// 0x558138
template<> void *t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_joint_data_allocator>::allocate(unsigned int size, const char *name)
{
    m_member_index = restricted_region_register_member(1, name, "", size, 0, __tls_set_g_joint_data_allocator, 0, 0);
    m_thread_id = get_current_thread_index();
    return restricted_region_get_member_address(1, m_member_index);
}

// @address 0x5581d8 ?deallocate@?$t_restricted_allocation_manager@$00$0A@$0A@$1?__tls_set_g_joint_data_allocator@@YAXPAX@Z@@UAAXPAX@Z
template<> void t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_joint_data_allocator>::deallocate(void *memory)
{
    clump_data = 0;
    restricted_region_free_member(1, m_member_index);
}

// @address 0x5592f0 joint_withdraw
static __forceinline short grow_find_invitation(long actor_index, long joint_index)
{
    actor_datum *actor = datum_get<actor_datum>(tls_actor_data(), actor_index);
    short result = -1;
    for (short index = 0; index < 4; ++index) {
        if (actor->invitations[index].type != -1 && actor->invitations[index].handle == joint_index) {
            result = index;
            break;
        }
    }
    return result;
}
extern "C" void joint_withdraw(long actor_index, c_joint_behavior_data *data)
{
    long joint_index = data->joint_index;
    joint_state_datum *jstate = datum_get<joint_state_datum>(joint_state_data(), joint_index);
    if (jstate->joint_type == 1) {
        jstate->participants[data->slot].actor_index = -1;
        jstate->participants[data->slot].status = 1;
    }
    --jstate->participant_count;
    if (jstate->participant_count <= 0) {
        switch (jstate->joint_type) {
        case 1:
            for (short slot = 0; slot < 10; ++slot) {
                if (jstate->participants[slot].actor_index != -1 && jstate->participants[slot].status == 0) {
                    long participant_index = jstate->participants[slot].actor_index;
                    actor_datum *actor = (actor_datum *)datum_try_and_get(tls_actor_data(), participant_index);
                    if (actor) {
                        short invitation_index = grow_find_invitation(participant_index, data->joint_index);
                        if (invitation_index != -1) {
                            invitation_clear(actor_get_invitation(participant_index, invitation_index));
                        }
                    }
                }
            }
            break;
        case 2:
            if (jstate->clump_index != -1) {
                long clump_index = jstate->clump_index;
                clump_datum *clump = datum_get<clump_datum>(tls_clump_data(), clump_index);
                for (short index = 0; index < 10; ++index) {
                    invitation_data *inv = &clump->invitations[index];
                    if (invitation_valid(inv) && inv->handle == joint_index) {
                        invitation_clear(inv);
                        break;
                    }
                }
            }
            break;
        }
        ::datum_delete(joint_state_data(), joint_index);
    }
}

// @address 0x558718 ?joint_primitive_behavior_activate@@YA_NJPAVc_behavior_state@@@Z
bool joint_primitive_behavior_activate(long a, c_behavior_state *state)
{
    c_joint_behavior_data *thing = (c_joint_behavior_data *)state;
    bool result = true;
    long index = -1;
    Joint_primitive_behavior *definition =
        (Joint_primitive_behavior *)global_behavior_info[thing->behavior_index].header;
    if (definition->activate)
        index = definition->activate(a, thing);
    if (index == -1) {
        result = false;
    } else if (thing->initiator) {
        result = joint_post_invitation(a, thing, index, definition->joint_type, definition->minimum_participants, definition->weight, definition->count);
        if (!result)
            joint_primitive_behavior_deactivate(a, thing);
    }
    if (result)
        thing->jstatus = datum_get<joint_state_datum>(joint_state_data(), index)->status;
    return result;
}

// @address 0x558808 ?joint_primitive_behavior_deactivate@@YAXJPAVc_behavior_state@@@Z
void joint_primitive_behavior_deactivate(long a, c_behavior_state *state)
{
    c_joint_behavior_data *thing = (c_joint_behavior_data *)state;
    joint_state_datum *elem = datum_get<joint_state_datum>(joint_state_data(), thing->joint_index);
    if (elem) {
        void (*handler)(long, c_behavior_state *, joint_state_datum *) =
            static_cast<Joint_primitive_behavior *>(global_behavior_info[thing->behavior_index].header)->deactivate;
        if (handler)
            handler(a, thing, elem);
        joint_withdraw(a, thing);
    }
}

// @address 0x558950 ?joint_primitive_behavior_control@@YAXJPAVc_behavior_state@@@Z
void joint_primitive_behavior_control(long a, c_behavior_state *state)
{
    c_joint_behavior_data *thing = static_cast<c_joint_behavior_data *>(state);
    void (*handler)(long, c_behavior_state *, joint_state_datum *) =
            static_cast<Joint_primitive_behavior *>(global_behavior_info[thing->behavior_index].header)->update;
    if (handler)
        handler(a, thing, joint_get(thing->joint_index));
}

// @address 0x5589a8 ?joint_mutex_group_activate@@YA_NJPAVc_behavior_state@@@Z
bool joint_mutex_group_activate(long a, c_behavior_state *state)
{
    c_joint_behavior_data *thing = (c_joint_behavior_data *)state;
    bool result = true;
    long index = -1;
    Joint_mutex_group *definition =
        (Joint_mutex_group *)global_behavior_info[thing->behavior_index].header;
    if (definition->activate)
        index = definition->activate(a, thing);
    if (index == -1) {
        result = false;
    } else if (thing->initiator) {
        result = joint_post_invitation(a, thing, index, definition->joint_type, definition->minimum_participants, definition->weight, definition->count);
        if (!result)
            joint_mutex_group_deactivate(a, thing);
    }
    if (result)
        thing->jstatus = datum_get<joint_state_datum>(joint_state_data(), index)->status;
    return result;
}

// @address 0x558a98 ?joint_mutex_group_deactivate@@YAXJPAVc_behavior_state@@@Z
void joint_mutex_group_deactivate(long a, c_behavior_state *state)
{
    c_joint_behavior_data *thing = (c_joint_behavior_data *)state;
    joint_state_datum *elem = datum_get<joint_state_datum>(joint_state_data(), thing->joint_index);
    if (elem) {
        void (*handler)(long, c_behavior_state *, joint_state_datum *) = ((Joint_mutex_group *)global_behavior_info[thing->behavior_index].header)->deactivate;
        if (handler)
            handler(a, thing, elem);
        joint_withdraw(a, thing);
    }
}

// @address 0x558c28 ?joint_mutex_group_debug@@YAXJPAVc_behavior_state@@@Z
void joint_mutex_group_debug(long a, c_behavior_state *state)
{
    c_joint_behavior_data *thing = static_cast<c_joint_behavior_data *>(state);
    void (*handler)(long, c_behavior_state *, joint_state_datum *) = ((Joint_mutex_group *)global_behavior_info[thing->behavior_index].header)->deactivate;
    if (handler)
        handler(a, thing, joint_get(thing->joint_index));
}

// @address 0x559620 ?joint_behavior_get_participant_limits@@YA_NFPAF0@Z
bool joint_behavior_get_participant_limits(short behavior_index, short *minimum_participants, short *maximum_participants)
{
    Behavior_header *definition = global_behavior_info[behavior_index].header;
    bool success = false;

    if (((short)definition->type == 2 || (short)definition->type == 1) && static_cast<Behavior *>(definition)->joint)
    {
        if ((short)definition->type == 2)
        {
            Joint_mutex_group *group = (Joint_mutex_group *)definition;
            if (minimum_participants)
                *minimum_participants = group->minimum_participants;
            if (maximum_participants)
                *maximum_participants = group->maximum_participants;
            success = true;
        }
        else if ((short)definition->type == 1)
        {
            Joint_primitive_behavior *primitive = (Joint_primitive_behavior *)definition;
            if (minimum_participants)
                *minimum_participants = primitive->minimum_participants;
            if (maximum_participants)
                *maximum_participants = primitive->maximum_participants;
            success = true;
        }
    }

    return success;
}

// @address 0x5588a8 ?joint_primitive_behavior_perform@@YA_NJPAVc_behavior_state@@@Z
bool joint_primitive_behavior_perform(long actor, c_behavior_state *state)
{
    c_joint_behavior_data *data = (c_joint_behavior_data *)state;
    joint_state_datum *j = joint_get(data->joint_index);
    bool result = false;
    Joint_primitive_behavior *definition =
        (Joint_primitive_behavior *)global_behavior_info[data->behavior_index].header;
    if (actor_joint_state_update(actor, data, data->joint_index, definition->minimum_participants, definition->maximum_participants)) {
        if (definition->perform) result = definition->perform(actor, state, j);
        else result = true;
    }
    return result;
}
static __forceinline c_joint_behavior_data *grow_actor_behavior_state_get(actor_datum *actor, short layer)
{
    c_joint_behavior_data *result = 0;
    if (layer >= 0 && layer < 8)
        result = (c_joint_behavior_data *)(actor->behavior_state_data + actor->state_offsets[layer]);
    return result;
}
// @address 0x558b38 ?joint_mutex_group_decide@@YAFJFPAVc_behavior_state@@@Z
short joint_mutex_group_decide(long index, short layer, c_behavior_state *state)
{
    short result = -1;
    actor_datum *actor = datum_get<actor_datum>(tls_actor_data(), index);
    c_joint_behavior_data *data = grow_actor_behavior_state_get(actor, layer);
    Joint_mutex_group *definition = (Joint_mutex_group *)global_behavior_info[data->behavior_index].header;
    if (actor_joint_state_update(index, data, data->joint_index, definition->minimum_participants, definition->maximum_participants)) {
        joint_state_datum *j = joint_get(data->joint_index);
        result = definition->decide(index, layer, state, j);
    }
    return result;
}
Joint_mutex_group::Joint_mutex_group(
    short behavior_index_arg,
    long flags,
    short (*get_desire_fn)(long, const s_behavior_input *, const s_behavior_arguments *),
    short (*next_fn)(long, c_behavior_state *, c_behavior_state *),
    long (*activate_fn)(long, c_behavior_state *),
    void (*deactivate_fn)(long, c_behavior_state *, joint_state_datum *),
    short layer_behavior_index_arg,
    short (*decide_fn)(long, short, c_behavior_state *, joint_state_datum *),
    short child_count,
    child_entry *children_arg,
    e_joint_type joint_type,
    short (*count_fn)(long, long, c_behavior_state *),
    short min_participants,
    short max_participants,
    float weight_arg,
    void (*flush_object_fn)(long, c_behavior_state *, long),
    void (*flush_prop_fn)(long, c_behavior_state *, long),
    void (*flush_firing_pos_fn)(long, c_behavior_state *, long),
    void (*flush_struct_fn)(long, c_behavior_state *),
    void (*flush_firing_position_fn)(long, c_behavior_state *, bool),
    void (*update_fn)(long, c_behavior_state *, joint_state_datum *),
    void (*debug_fn)(c_behavior_state *))
{
    // match: 0x558210: this constructor initializes presearch_behavior exclusively.
    __assume(this == &presearch_behavior);
    (void)behavior_index_arg;
    (void)flags;
    (void)get_desire_fn;
    (void)next_fn;
    (void)activate_fn;
    (void)deactivate_fn;
    (void)layer_behavior_index_arg;
    (void)decide_fn;
    (void)child_count;
    (void)children_arg;
    (void)joint_type;
    (void)count_fn;
    (void)min_participants;
    (void)max_participants;
    (void)weight_arg;
    (void)flush_object_fn;
    (void)flush_prop_fn;
    (void)flush_firing_pos_fn;
    (void)flush_struct_fn;
    (void)flush_firing_position_fn;
    (void)update_fn;
    (void)debug_fn;
    Joint_mutex_group *definition = &presearch_behavior;
    definition->weight = 5e+01f;
    definition->index = 46;
    definition->type = 1;
    definition->flags = 0x3ff01ffc;
    definition->kind = -2;
    definition->get_desire = behavior_presearch_get_desire;
    definition->next = behavior_presearch_next;
    definition->Behavior::activate = joint_mutex_group_activate;
    definition->Behavior::deactivate = joint_mutex_group_deactivate;
    definition->layer_behavior_index = -1;
    definition->clear_non_deterministic_state = 0;
    definition->on_object_deleted = 0;
    definition->flush_prop = behavior_presearch_flush_prop;
    definition->handle_deleted_object = 0;
    definition->flush_structure = 0;
    definition->flush_firing_position = 0;
    definition->debug = joint_mutex_group_debug;
    definition->Mutex_group::decide = joint_mutex_group_decide;
    definition->child_count = 6;
    definition->children = behavior_presearch_children;
    definition->activate = behavior_presearch_activate;
    definition->deactivate = behavior_presearch_deactivate;
    definition->decide = behavior_presearch_decide;
    definition->count = 0;
    definition->minimum_participants = 1;
    definition->maximum_participants = 10;
    definition->joint = true;
    definition->joint_type = 2;
    definition->update = 0;
}

// Registers a dynamic-firing-set member in region 1 and publishes its base through the joint data slot (the
// thread-local g_joint_data, not the allocator's own slot); the allocation records the member index and thread index.
// @address 0x5596a0 ?allocate@?$t_restricted_allocation_manager@$00$0A@$0A@$1?__tls_set_g_clump_data_allocator@@YAXPAX@Z@@UAAPAXIPBD@Z
template<> void *t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_clump_data_allocator>::allocate(unsigned int size, const char *name)
{
    long member_index = restricted_memory_register_member(&g_restricted_regions[1], name, "", size, 0,
        __tls_set_g_dynamic_firing_set_allocator, 0, 0);
    void *address = restricted_region_get_member_address(1, member_index);
    data_set_new_base_address(&joint_state_data(), (s_data_array *)address);
    m_member_index = member_index;
    m_thread_id = get_current_thread_index();
    return restricted_region_get_member_address(1, member_index);
}

extern "C" {
joint_clump_allocation_manager g_ai_clumps_allocator;
joint_state_allocation_manager g_ai_joint_state_allocator;
}


// @address 0x558c28 ?joint_primitive_behavior_debug@@YAXJPAVc_behavior_state@@@Z
void joint_primitive_behavior_debug(long actor_index, c_behavior_state *state)
{
    c_joint_behavior_data *data = static_cast<c_joint_behavior_data *>(state);
    Joint_primitive_behavior *definition = static_cast<Joint_primitive_behavior *>(global_behavior_info[data->behavior_index].header);
    if (definition->debug)
        definition->debug(actor_index, data, joint_get(data->joint_index));
}
