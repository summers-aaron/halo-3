// ailibxbox/swarm_behavior.obj: from the records 0x570680 0x570728 0x570778 0x570930 0x570968 0x5709a0 0x5709d8 0x570a10 0x570b18 0x570b60 0x570ba8 0x570be0 0x570cb0
// @flags /TP /O2 /Oc
#include "ai/behavior_info.h"
#include "ai/swarms.h"
// @address 0x570680 ?behavior_swarm_scatter_get_desire@@YAFJPBUs_behavior_input@@PBUs_behavior_arguments@@@Z
// @address 0x570728 ?behavior_swarm_scatter_deactivate@@YAXJPAVc_behavior_state@@@Z
// @address 0x570b60 ?behavior_support_attack_next@@YAFJPAVc_behavior_state@@0@Z
// @address 0x570cb0 ?infect_get_impulse@@YAFJPAVc_behavior_state@@@Z
// @address 0x5501d8 ?ai_resurrect_get_marker@@YA_NJPAUobject_marker@@PA_N@Z
// @address 0x9a080 ?object_get_origin@@YAPATreal_point3d@@JPAT1@@Z
// @address 0xae5e0 ?actor_executing_layer@@YAFJF@Z
// Hand-drafted from the records: support_attackers and prop_ref_infectable.

#include <stdlib.h>
#include <math.h>
#include <float.h>
#include "math/real_math.h"
#include "game/game_time_types.h"
#include "game/game_time_conversion.h"
#include "memory/tls_globals.h"
#include "objects/object_datum.h"
#include "objects/object_header.h"
#include "objects/object_placement.h"
#include "ai/actor_datum.h"
#include "ai/actor_globals.h"
#include "ai/behavior.h"
#include "ai/clump_datum.h"
#include "ai/prop_ref_datum.h"
#include "ai/prop_datum.h"
#include "ai/prop_state.h"
#include "objects/objects.h"
#include "ai/ai_actions.h"
#include "ai/props.h"
#include "ai/character_definitions.h"
#include "ai/swarm_behavior.h"
#include "ai/actor_prop_ref_iterator.h"
#include "ai/clump.h"
#include "math/real_math_types.h"
#include "ai/swarm_creature_iterator_types.h"
#include "cache/cache_files.h"
#include "tag_files/tag_groups.h"
#include "creatures/creature_datum.h"
#include "units/biped_definitions.h"

struct s_behavior_input;
struct s_behavior_arguments;

// behavior_swarm_scatter's data after the layer header
struct swarm_scatter_state : public c_behavior_state { bool scattering; char pad25; short timer; };

static __forceinline void swarm_clump_actor_iterator_new(clump_actor_iterator *iterator, long index)
{
    clump_datum *clump = datum_get<clump_datum>(tls_clump_data(), index);
    iterator->next_actor_index = clump->first_actor_index;
}
static __forceinline actor_datum *swarm_clump_actor_iterator_next(clump_actor_iterator *iterator)
{
    actor_datum *result = 0;
    if (iterator->next_actor_index != -1)
    {
        result = datum_get<actor_datum>(tls_actor_data(), iterator->next_actor_index);
        iterator->actor_index = iterator->next_actor_index;
        iterator->next_actor_index = result->next_clump_actor_index;
    }
    return result;
}
static __forceinline bool swarm_actor_executing(long index,short behavior) { return actor_executing_layer(index,behavior)!=-1; }

short behavior_swarm_scatter_get_desire(long actor_index, const s_behavior_input *, const s_behavior_arguments *)
{
 short result=0;
 character_swarm_properties *properties=::character_definition_swarm_multi_properties_get(actor_index);
 if(properties) {
  swarm_datum *swarm=datum_get<swarm_datum>(tls_swarm_data(), datum_get<actor_datum>(tls_actor_data(), actor_index)->swarm_index);
  if(swarm->death_count>properties->scatter_killed_count) result=3;
  else if(_rotl(datum_get<actor_datum>(tls_actor_data(), actor_index)->stimulus_flags,12)&1) result=3;
 }
 return result;
}

void behavior_swarm_scatter_deactivate(long actor_index, c_behavior_state *)
{
 actor_datum *actor=datum_get<actor_datum>(tls_actor_data(), actor_index);
 if(actor->swarm_index!=-1) {
  swarm_datum *swarm=datum_get<swarm_datum>(tls_swarm_data(), actor->swarm_index);
  swarm->death_count=0;
 }
}

static __forceinline void swarm_behavior_creature_iterator_new(long index, swarm_creature_iterator *i)
{
    swarm_datum *s = datum_get<swarm_datum>(tls_swarm_data(), index);
    i->next_object_index = s->first_object_index;
    i->current_object_index = -1;
}
static __forceinline swarm_creature_state *swarm_behavior_creature_state_get(creature_datum *creature)
{
    return (swarm_creature_state *)object_header_block_get(creature, &creature->creature.creature_reference);
}
// @address 0x570778 ?behavior_swarm_scatter_next@@YAFJPAVc_behavior_state@@0@Z
short behavior_swarm_scatter_next(long index,c_behavior_state *behavior_state,c_behavior_state *next_state)
{
 swarm_scatter_state *state=(swarm_scatter_state*)behavior_state;
 actor_datum *a=datum_get<actor_datum>(tls_actor_data(), index);
 long prop_index=a->target_prop_index;
 long swarm_index;
 short result=-1;
 if(prop_index!=-1 && (swarm_index=a->swarm_index)!=-1) {
  character_swarm_properties *p=::character_definition_swarm_multi_properties_get(index);
  if(p) {
   result=-2;
   unsigned char suppress=(a->stimulus_flags>>20)&1;
   if(!suppress) {
    if(state->scattering) {
     state->timer++;
     if(game_ticks_to_seconds((float)state->timer)>=p->scatter_time) result=-1;
    } else {
     prop_state *prop=(prop_state*)prop_state_get_by_index(prop_index);
     bool clear=true;
     float radius=0.75f*p->scatter_distance;
     swarm_creature_iterator iter; swarm_behavior_creature_iterator_new(swarm_index,&iter);
     creature_datum *c;
     while((c=swarm_creature_iterator_next(&iter))!=0) {
      swarm_creature_state *s=c->creature.link_type==0 ? swarm_behavior_creature_state_get(c) : 0;
      if(s) {
       float distance=distance3d(&s->position,&prop->position);
       if(distance<radius) { clear=false; break; }
      }
     }
     if(clear) state->scattering=true;
    }
   }
  }
 }
 return result;
}

// @address 0x570930 ?behavior_swarm_scatter_control@@YAXJPAVc_behavior_state@@@Z
void behavior_swarm_scatter_control(long index, c_behavior_state *)
{
 actor_datum *actor=datum_get<actor_datum>(tls_actor_data(), index);
 actor->secondary_control_mode=6;
 actor->secondary_control_flags=2;
 actor->secondary_control_target=actor->target_prop_index;
}

// @address 0x570968 ?behavior_swarm_attack_get_desire@@YAFJPBUs_behavior_input@@PBUs_behavior_arguments@@@Z
short behavior_swarm_attack_get_desire(long index, const s_behavior_input *, const s_behavior_arguments *)
{
 short result=0;
 actor_datum *actor=datum_get<actor_datum>(tls_actor_data(), index);
 if(actor->target_prop_index!=-1) result=3;
 return result;
}

// @address 0x5709a0 ?behavior_swarm_attack_next@@YAFJPAVc_behavior_state@@0@Z
short behavior_swarm_attack_next(long index, c_behavior_state *, c_behavior_state *)
{
 short result=-1;
 actor_datum *actor=datum_get<actor_datum>(tls_actor_data(), index);
 if(actor->target_prop_index!=-1) result=-2;
 return result;
}

// @address 0x5709d8 ?behavior_swarm_attack_control@@YAXJPAVc_behavior_state@@@Z
void behavior_swarm_attack_control(long index, c_behavior_state *)
{
 actor_datum *actor=datum_get<actor_datum>(tls_actor_data(), index);
 actor->secondary_control_mode=1;
 actor->secondary_control_flags=2;
 actor->secondary_control_target=actor->target_prop_index;
}

bool support_attackers(long actor_index,float threshold)
{
 bool result=false;
 actor_datum *actor=datum_get<actor_datum>(tls_actor_data(), actor_index);
 if(actor->target_prop_index!=-1 && actor->clump_index!=-1) {
  prop_ref_datum *prop=datum_get<prop_ref_datum>(tls_prop_ref_data(), actor->target_prop_index);
  clump_actor_iterator iterator;
  swarm_clump_actor_iterator_new(&iterator,actor->clump_index);
  actor_datum *other;
  while((other=swarm_clump_actor_iterator_next(&iterator))!=0) {
   if(other!=actor && !other->swarm && other->target_prop_index!=-1) {
    prop_ref_datum *other_prop=datum_get<prop_ref_datum>(tls_prop_ref_data(), other->target_prop_index);
    if(other_prop->prop_index==prop->prop_index && swarm_actor_executing(iterator.actor_index,25) && other_prop->distance<threshold) result=true;
   }
  }
 }
 return result;
}

// @address 0x570b18 ?behavior_support_attack_get_desire@@YAFJPBUs_behavior_input@@PBUs_behavior_arguments@@@Z
short behavior_support_attack_get_desire(long index, const s_behavior_input *, const s_behavior_arguments *)
{
 short result=0;
 if(support_attackers(index,4.0f)) result=3;
 return result;
}

short behavior_support_attack_next(long actor_index, c_behavior_state *, c_behavior_state *)
{
 short result=-2;
 if(!support_attackers(actor_index,6.0f)) result=-1;
 return result;
}

// @address 0x570ba8 ?behavior_support_attack_control@@YAXJPAVc_behavior_state@@@Z
void behavior_support_attack_control(long index, c_behavior_state *)
{
 actor_datum *actor=datum_get<actor_datum>(tls_actor_data(), index);
 long prop=actor->target_prop_index;
 actor->secondary_control_flags=2;
 actor->secondary_control_mode=3;
 actor->secondary_control_target=prop;
}

bool prop_ref_infectable(long prop_index)
{
    bool result = false;
    prop_ref_datum *prop_ref = datum_get<prop_ref_datum>(tls_prop_ref_data(), prop_index);
    if (prop_ref->knowledge_state >= 1 && prop_ref->knowledge_state <= 2) {
        if (prop_ref->type == 7 || prop_ref->type == 6) {
            object_datum *object = datum_get<object_header_datum>(tls_object_header_data(), prop_ref->object_index)->datum;
            unsigned char flag = (object->damage_flags >> 2) & 1;
            if (flag && object->object_type == 0
                && ((biped_definition *)tag_get('bipd', object->definition_index))->reanimation_character.index != -1) {
                unsigned char bits = object->inhibited_flags;
                if ((bits & 1) == 0 || (bits & 2) == 0)
                    result = true;
            }
        }
    }
    return result;
}

static __forceinline bool swarm_infect_available(void)
{
    bool result = false;
    if (tls_actor_globals()->enabled && tls_actor_globals()->active_actor_count <= 25
        && tls_actor_globals()->scripted_unknown_time < tls_game_time_globals()->time)
        result = true;
    return result;
}

short infect_get_impulse(long actor_index, c_behavior_state *)
{
 if(swarm_infect_available()) {
  float best_distance=FLT_MAX;
  long best_prop_ref=-1;
  short best_up;
  actor_datum *actor=datum_get<actor_datum>(tls_actor_data(), actor_index);
  actor_prop_ref_iterator prop_iterator;
  actor_prop_ref_iterator_new(&prop_iterator,actor_index);
  prop_ref_datum *prop_ref;
  while((prop_ref=actor_prop_ref_iterator_next(&prop_iterator))!=0) {
   if(prop_ref_infectable(prop_iterator.index)) {
    prop_datum *prop=datum_get<prop_datum>(tls_ai_prop_data(), prop_ref->prop_index);
    object_marker marker;
    bool up;
    if((prop->flags&32)==0 && ai_resurrect_get_marker(prop_ref->object_index,&marker,&up)) {
     float distance=distance_squared3d(&actor->position.body,&marker.matrix.position);
     if(distance<best_distance) { best_distance=distance; best_prop_ref=prop_iterator.index; best_up=up; }
    }
   }
  }
  if(best_prop_ref!=-1) {
   prop_ref_datum *best=datum_get<prop_ref_datum>(tls_prop_ref_data(), best_prop_ref);
   prop_datum *best_prop=datum_get<prop_datum>(tls_ai_prop_data(), best->prop_index);
   long best_object=-1;
   float best_object_distance=FLT_MAX;
   swarm_creature_iterator creature_iterator;
   swarm_behavior_creature_iterator_new(actor->swarm_index,&creature_iterator);
   real_point3d origin;
   object_get_origin(best->object_index,&origin);
   creature_datum *creature;
   while((creature=swarm_creature_iterator_next(&creature_iterator))!=0) {
    swarm_creature_state *creature_state=creature->creature.link_type==0 ? swarm_behavior_creature_state_get(creature) : 0;
    if (creature_state && creature->object.parent_object_index == -1 && creature_state->prop_ref_index == -1 && creature->creature.physics.m_mode == 1 &&
        !(bool)((creature->creature.flags >> 5) & 1))
    {
     real_point3d creature_origin;
     object_get_origin(creature_iterator.current_object_index,&creature_origin);
     float distance=distance_squared3d(&creature_origin,&origin);
     if(distance<best_object_distance) { best_object_distance=distance; best_object=creature_iterator.current_object_index; }
    }
   }
   if(best_object!=-1) {
    creature_datum *target=(creature_datum *)datum_get<object_header_datum>(tls_object_header_data(), best_object)->datum;
    swarm_creature_state *creature_state=target->creature.link_type==0 ? swarm_behavior_creature_state_get(target) : 0;
    creature_state->prop_ref_index=best_prop_ref;
    creature_state->resurrect_type=(best_up==0)+3;
    best_prop->flags|=32;
   }
  }
 }
 return -1;
}

// @address 0x570fd0 ?behavior_swarm_root_decide@@YAFJFPAVc_behavior_state@@@Z
short behavior_swarm_root_decide(long actor_index, short layer, c_behavior_state *state)
{
    return behavior_decide_prioritized(actor_index, layer, state);
}

// @address 0x42d380 ?behavior_swarm_root_get_desire@@YAFJPBUs_behavior_input@@PBUs_behavior_arguments@@@Z
short behavior_swarm_root_get_desire(long, const s_behavior_input *, const s_behavior_arguments *)
{
    return 1;
}

// @address 0x72ce98 ?behavior_swarm_root_children@@3PAUchild_entry@@A
child_entry behavior_swarm_root_children[6] = {
    { 4, 0, true, 0, -1, 0.0f },
    { 145, 0, true, 1, 0, 1.0f },
    { 146, 0, true, 0, -1, 0.0f },
    { 144, 0, true, 0, -1, 0.0f },
    { 143, 0, true, 0, -1, 0.0f },
    { 5, 0, true, 0, -1, 0.0f },
};

// @address 0x7c56c0 ?swarm_root_behavior@@3VMutex_group@@A
Mutex_group swarm_root_behavior = Mutex_group(142, -1, behavior_swarm_root_get_desire, 0, 0, 0, -1,
    behavior_swarm_root_decide, 6, behavior_swarm_root_children);

// @address 0x7c5610 ?swarm_attack_behavior@@3VPrimitive_behavior@@A
Primitive_behavior swarm_attack_behavior = Primitive_behavior(143, 0x3ff007f8, behavior_swarm_attack_get_desire,
    behavior_swarm_attack_next, 0, 0, -1, 0, behavior_swarm_attack_control);

// @address 0x7c5660 ?support_attack_behavior@@3VPrimitive_behavior@@A
Primitive_behavior support_attack_behavior = Primitive_behavior(144, 0x3ff007f8,
    behavior_support_attack_get_desire, behavior_support_attack_next, 0, 0, -1, 0,
    behavior_support_attack_control);

// @address 0x7c56ac ?infect_impulse@@3VBehavior_impulse@@A
Behavior_impulse infect_impulse = Behavior_impulse(145, 0x3ff007f8, infect_get_impulse);

// @address 0x7c55c0 ?swarm_scatter_behavior@@3VPrimitive_behavior@@A
Primitive_behavior swarm_scatter_behavior = Primitive_behavior(146, 0x3ff007f8,
    behavior_swarm_scatter_get_desire, behavior_swarm_scatter_next, 0, behavior_swarm_scatter_deactivate, -1, 0,
    behavior_swarm_scatter_control);
