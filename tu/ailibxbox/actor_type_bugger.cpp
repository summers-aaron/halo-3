// ailibxbox/actor_type_bugger.obj: from the records 0x570408 0x570508 0x570610
// @flags /TP /O2 /Oc
#include "ai/behavior_info.h"
#include "ai/actor_datum.h"
#include "ai/clump_datum.h"
#include "ai/behavior.h"
// @address 0x570610 ?behavior_bugger_group_uncover_next@@YAFJPAVc_behavior_state@@0@Z
// Hand-drafted from the records: behavior_bugger_group_uncover_get_desire and behavior_bugger_group_uncover_activate.
#include "memory/tls_globals.h"

#include "ai/clump.h"
struct s_behavior_input;
struct s_behavior_arguments;
// behavior_bugger_group_uncover's data after the layer header
class c_bugger_group_uncover_data : public c_behavior_state {
public:
    short pad24;
    short timer;
};


static __forceinline void *bugger_datum_get(s_data_array *array,long index,long size) { return array->data+(index&0xffff)*size; }
static __forceinline void bugger_clump_actor_iterator_new(clump_actor_iterator *iterator, clump_datum *clump)
{
    iterator->next_actor_index = clump->first_actor_index;
}
static __forceinline actor_datum *bugger_clump_actor_iterator_next(clump_actor_iterator *iterator) {
 actor_datum *result=0;
 if(iterator->next_actor_index!=-1) {
  result=(actor_datum*)bugger_datum_get(tls_actor_data(),iterator->next_actor_index,sizeof(actor_datum));
  iterator->actor_index=iterator->next_actor_index;
  iterator->next_actor_index=result->next_clump_actor_index;
 }
 return result;
}

short behavior_bugger_group_uncover_get_desire(long actor_index,const s_behavior_input *input,const s_behavior_arguments *arguments) {
 actor_datum *actor=(actor_datum*)bugger_datum_get(tls_actor_data(),actor_index,sizeof(actor_datum));
 short result=0;
 if(actor->clump_index!=-1) {
  clump_datum *clump=(clump_datum*)bugger_datum_get(tls_clump_data(),actor->clump_index,sizeof(clump_datum));
  short count=0;
  short executing_count=0;
  clump_actor_iterator iterator;
  bugger_clump_actor_iterator_new(&iterator,clump);
  actor_datum *other;
  while((other=bugger_clump_actor_iterator_next(&iterator))!=0) {
   if(other->type==actor->type) {
    ++count;
    if(actor_executing_layer(iterator.actor_index,46)!=-1) ++executing_count;
   }
  }
  if((float)executing_count/(float)count>0.6f) result=3;
 }
 return result;
}

bool behavior_bugger_group_uncover_activate(long actor_index,c_behavior_state *behavior_state) {
 c_bugger_group_uncover_data *state=(c_bugger_group_uncover_data*)behavior_state;
 bool result=false;
 actor_datum *actor=(actor_datum*)bugger_datum_get(tls_actor_data(),actor_index,sizeof(actor_datum));
 if(actor->clump_index!=-1) {
  state->timer=0;
  clump_datum *clump=(clump_datum*)bugger_datum_get(tls_clump_data(),actor->clump_index,sizeof(clump_datum));
  clump_actor_iterator iterator;
  bugger_clump_actor_iterator_new(&iterator,clump);
  actor_datum *other;
  while((other=bugger_clump_actor_iterator_next(&iterator))!=0) {
   if(other!=actor) {
    short layer=actor_executing_layer(iterator.actor_index,140);
    if(layer>=0&&layer<=other->layer_count) {
     c_bugger_group_uncover_data *other_state=0;
     if(layer<8) other_state=(c_bugger_group_uncover_data*)(other->behavior_state_data+other->state_offsets[layer]);
     short timer=(short)other_state->timer;
     if(timer>(short)state->timer) state->timer=timer;
    }
   }
  }
  result=true;
 }
 return result;
}

short behavior_bugger_group_uncover_next(long actor_index,c_behavior_state *behavior_state,c_behavior_state *other_state) {
 c_bugger_group_uncover_data *state=(c_bugger_group_uncover_data*)behavior_state;
 state->timer=(short)(state->timer+1);
 if(state->timer<=(int)(tls_game_time_globals()->ticks_per_second*1.5f+0.5f)) return -2;
 return -3;
}

// @address 0x7c5570 ?bugger_group_uncover_behavior@@3VPrimitive_behavior@@A
Primitive_behavior bugger_group_uncover_behavior = Primitive_behavior(140, 0x3ff007ff,
    behavior_bugger_group_uncover_get_desire, behavior_bugger_group_uncover_next,
    behavior_bugger_group_uncover_activate, 0, -1, 0, 0);
