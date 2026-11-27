// ailibxbox/prop_dialogue.obj: from the records 0xace68 0x564510 0x564570 0x564678 0x564a18 0x564b68 0x564c58 0x564df8 0x564e60 0x564ed8 0x564f90
// @flags /TP /O2 /Oc
#include "ai/ai_information_data.h"
#include "memory/data_types.h"
#include "memory/tls_globals.h"
#include "cseries/cseries.h"
#include "tag_files/tag_types.h"
#include "game/game_time_types.h"
#include "game/game_types.h"
#include "ai/clump_datum.h"
#include "ai/prop_datum.h"
#include "ai/prop_ref_datum.h"
#include "game/game_globals.h"
#include "ai/clump.h"
#include "ai/prop_dialogue.h"
#include "game/game_allegiance.h"
#include "ai/prop_dialogue_event_types.h"
#include "ai/dialogue_state_types.h"
#include "cache/cache_files.h"
#include "scenario/scenario.h"
#include "ai/actor_datum.h"
#include "ai/ai_globals_definition.h"
#include "ai/ai_dialogue.h"
#include "ai/character_definitions.h"
#include "objects/object_datum.h"
#include "memory/data.h"
#include "ai/clump_iterator.h"
#include "ai/actor_prop_ref_iterator.h"
#include "ai/ai_dialogue_definitions.h"
#include "game/game_time_inline.h"
#include "objects/object_header_access.h"
#include "tag_files/tag_groups.h"
#include "math/random_math.h"
#include "math/random_math_range.h"
// @address 0x564510 ?clump_get_first_unit@@YAJJ@Z
// @address 0x564c58 ?clump_dialogue_allowed@@YA_NJJW4e_dialogue_state@@@Z
// @address 0x564df8 ?actor_dialogue_allowed@@YAEJJH@Z

// @address 0x9ca90 ?game_team_is_ally@@YA_NW4e_game_team@@0@Z
#include "ai/prop_ref_flags.h"



struct s_dialogue_object;
#include "ai/dialogue_event.h"

#include "ai/dialogue_option.h"

static __forceinline short prop_dialogue_team_get(short team)
{
    short result = 1;
    if (team == 1 || game_team_is_ally((e_game_team)team, _ai_dialogue_team_player))
        result = 0;
    else
        result = 1;
    return result;
}

// @address 0x564510 ?clump_get_first_unit@@YAJJ@Z
long clump_get_first_unit(long clump_index) {
 long unit_index=-1;
 clump_actor_iterator iterator;
 clump_actor_iterator_new(&iterator,datum_get<clump_datum>(tls_clump_data(),clump_index));
 for(;;) {
  actor_datum *actor=clump_actor_iterator_next(&iterator);
  if(actor) {
   if(actor->unit_index!=-1) {
    unit_index=actor->unit_index;
    break;
   }
   continue;
  }
  break;
 }
 return unit_index;
}

static __forceinline bool prop_dialogue_object_is_special(long object_index) {
 object_datum *object=object_get(object_index);
 return (object->damage_flags & 4)!=0;
}

// @address 0x564678 ?prop_dialogue_event_play_allowed@@YA_NJJW4e_prop_dialogue_event@@_NPAF@Z
bool prop_dialogue_event_play_allowed(long clump_index,long prop_index,e_prop_dialogue_event event,bool flag,short *out) {
 clump_datum *clump=datum_get<clump_datum>(tls_clump_data(),clump_index);
 prop_datum *prop=0;
 short dialogue_index=-1;
 bool allowed=false;
 if(prop_index!=-1) prop=datum_get<prop_datum>(tls_ai_prop_data(),prop_index);
 short team=prop_dialogue_team_get(clump->team);
 bool friendly=team==0;
 bool valid=prop && prop->valid && (friendly || prop->hostile);
 bool special=false;
 if(prop) special=prop_dialogue_object_is_special(prop->object_index);
 switch(event) {
 case 5:
  if(prop) {
   if(!(prop->dialogue_flags&16)) prop_calculate_dialogue_flags(clump_index,prop_index);
   if((prop->dialogue_flags&1) && !(prop->previous_dialogue_flags&1)) {
    dialogue_index=3;
   } else if(clump->dialogue_state==1 || clump->dialogue_state==2) {
    if(prop->type==1) {
     if(!clump->dialogue_ready) dialogue_index=6;
     else if(valid && (!flag || !friendly || clump->count<3)) dialogue_index=4;
    }
   } else if(clump->dialogue_state==4 || clump->dialogue_state==5) {
    if(valid && (!flag || !friendly || clump->count<3)) {
     if(prop->count==0) dialogue_index=4;
     else if(prop->count>1 && !special) {
      if(clump->dialogue_state==5) dialogue_index=12;
      else dialogue_index=11;
     }
    }
   }
  }
  break;
 case 2:
  if(prop && valid && clump->count<3) {
   allowed=true;
   if(clump->dialogue_state==1) dialogue_index=8;
   else dialogue_index=9;
  }
  break;
 case 3:
  if(prop && valid && !special && (flag || (prop->count<7 && (!friendly || clump->count<3)))) dialogue_index=20;
  break;
 case 4:
  if(prop && valid && !special && (!friendly || clump->count<3)) dialogue_index=27;
  break;
 case 1:
  if(clump->count==1) dialogue_index=54;
  break;
 }
 allowed=dialogue_index!=-1;
 if(out) *out=dialogue_index;
 return allowed;
}

// @address 0x564a18 ?clump_dialogue_event_allowed@@YA_NJJ_NW4e_prop_dialogue_event@@PAFPAW4e_dialogue_state@@@Z
bool clump_dialogue_event_allowed(
    long clump_index, long prop_index, bool flag, e_prop_dialogue_event event, short *dialogue_index, e_dialogue_state *dialogue_state)
{
 clump_datum *clump=datum_get<clump_datum>(tls_clump_data(),clump_index);
 e_dialogue_state next_state=dialogue_state_none;
 bool allowed=false;
 switch(event) {
 case prop_dialogue_event_5:
  if(clump->dialogue_state!=dialogue_state_3) next_state=dialogue_state_3;
  allowed=true;
  break;
 case prop_dialogue_event_2:
  if(clump->dialogue_state==dialogue_state_1) {
   next_state=dialogue_state_2;
   allowed=true;
  } else if(clump->dialogue_state==dialogue_state_2 || clump->dialogue_state==dialogue_state_4 || clump->dialogue_state==dialogue_state_5) {
   allowed=true;
  }
  break;
 case prop_dialogue_event_3:
  if(clump->dialogue_state==dialogue_state_3) {
   next_state=dialogue_state_4;
   allowed=true;
  }
  break;
 case prop_dialogue_event_4:
  if(clump->dialogue_state==dialogue_state_4) {
   next_state=dialogue_state_5;
   allowed=true;
  }
  break;
 case prop_dialogue_event_1:
  if(clump->dialogue_state!=dialogue_state_1 && clump->dialogue_state!=dialogue_state_2) {
   next_state=dialogue_state_1;
   allowed=true;
  }
  break;
 case prop_dialogue_event_6:
  if(clump->dialogue_state!=dialogue_state_6) {
   next_state=dialogue_state_6;
   allowed=true;
  }
  break;
 }
 if(allowed) allowed=prop_dialogue_event_play_allowed(clump_index,prop_index,event,flag,dialogue_index);
 if(allowed && dialogue_state) *dialogue_state=next_state;
 return allowed;
}

// @address 0x564b68 ?prop_dialogue_event_internal@@YAXJJW4e_prop_dialogue_event@@FM@Z
void prop_dialogue_event_internal(long actor_index,long prop_index,e_prop_dialogue_event event,short dialogue_index,float delay) {
 actor_datum *actor=datum_get<actor_datum>(tls_actor_data(),actor_index);
 prop_ref_datum *prop_ref=datum_get<prop_ref_datum>(tls_prop_ref_data(),prop_index);
 short allowed_dialogue_index;
 if(clump_dialogue_event_allowed(actor->clump_index,prop_ref->prop_index,true,event,&allowed_dialogue_index,0)) {
  if(event>prop_ref->dialogue_event) {
   short ticks=(short)game_seconds_to_ticks_round(delay);
   if(dialogue_index==-1) dialogue_index=allowed_dialogue_index;
   prop_ref->dialogue_event=(signed char)event;
   prop_ref->dialogue_index=(unsigned char)dialogue_index;
   prop_ref->dialogue_ticks=(unsigned char)ticks;
  }
 }
}

// @address 0x564c58 ?clump_dialogue_allowed@@YA_NJJW4e_dialogue_state@@@Z
bool clump_dialogue_allowed(long clump_index,long prop_index,e_dialogue_state state) {
 bool allowed=false;
 if(state==0) allowed=true;
 else if(clump_index!=-1) {
  clump_datum *clump=datum_get<clump_datum>(tls_clump_data(),clump_index);
  prop_datum *prop=0;
  if(prop_index!=-1) prop=datum_get<prop_datum>(tls_ai_prop_data(),prop_index);
  short team=prop_dialogue_team_get(clump->team);
  bool friendly=team==0;
  bool valid=prop && prop->valid && (friendly || prop->hostile);
  if(clump->dialogue_state==state && (!prop || valid)) {
   allowed=true;
   if(friendly) {
    switch(state) {
    case 1: case 2: case 4: case 5: allowed=clump->count<3; break;
    case 3: allowed=clump->count>=3; break;
    }
   }
  }
 }
 return allowed;
}

unsigned char actor_dialogue_allowed(long actor_index,long prop_ref_index,int state) {
 bool allowed=false;
 actor_datum *actor=datum_get<actor_datum>(tls_actor_data(),actor_index);
 if(actor->clump_index!=-1) {
  long prop_index=-1;
  if(prop_ref_index!=-1) prop_index=datum_get<prop_ref_datum>(tls_prop_ref_data(),prop_ref_index)->prop_index;
  allowed=clump_dialogue_allowed(actor->clump_index,prop_index,(e_dialogue_state)state);
 }
 return allowed;
}
// @address 0x564e60 ?actor_dialogue_event@@YA_NJFJPAUai_information_data@@@Z
bool actor_dialogue_event(long actor_index,short event,long parameter,ai_information_data *information) {
 bool result=0;
 long unit_index=datum_get<actor_datum>(tls_actor_data(),actor_index)->unit_index;
 if(unit_index!=-1) {
  c_dialogue_event dialogue_event(event,unit_index,parameter,-1,-1,0);
  dialogue_event.clump_index=0;
  result=::ai_dialogue_event(&dialogue_event);
 }
 return result;
}
// @address 0x564ed8 ?actor_dialogue_event@@YA_NJFJW4e_dialogue_state@@PAUai_information_data@@@Z
bool actor_dialogue_event(long actor_index,short event,long target,e_dialogue_state state,ai_information_data *information) {
 bool result=false;
 actor_datum *actor=datum_get<actor_datum>(tls_actor_data(),actor_index);
 if(actor->unit_index!=-1) {
  bool allowed;
  if(state!=dialogue_state_none) {
   allowed=false;
   if(actor->clump_index!=-1) allowed=clump_dialogue_allowed(actor->clump_index,-1,state);
  }
  if(state==dialogue_state_none || allowed) {
   c_dialogue_event dialogue_event(event,actor->unit_index,target,-1,-1,0);
   dialogue_event.clump_index=(int)state;
   result=::ai_dialogue_event(&dialogue_event);
  }
 }
 return result;
}
// @address 0x564f90 ?actor_dialogue_prop_event@@YA_NJFJW4e_dialogue_state@@PAUai_information_data@@@Z
bool actor_dialogue_prop_event(long actor_index,short event,long prop_ref_index,e_dialogue_state state,ai_information_data *information) {
 bool result=false;
 actor_datum *actor=datum_get<actor_datum>(tls_actor_data(),actor_index);
 if(actor->unit_index!=-1 && actor_dialogue_allowed(actor_index,prop_ref_index,state)) {
  prop_ref_datum *prop_ref=datum_get<prop_ref_datum>(tls_prop_ref_data(),prop_ref_index);
  c_dialogue_event dialogue_event(event,actor->unit_index,prop_ref->object_index,-1,-1,0);
  dialogue_event.clump_index=(int)state;
  result=::ai_dialogue_event(&dialogue_event);
 }
 return result;
}

// match: 0xace68: the actor list is read one inline level deep, like actor_prop_ref_iterator_new;
// the shared clump_actor_iterator_next (zero levels, needed by behavior.cpp) moves its registers.
static __forceinline actor_datum *prop_dialogue_actor_iterator_next(clump_actor_iterator *iterator) {
 actor_datum *actor=0;
 if(iterator->next_actor_index!=-1) {
  actor=datum_get<actor_datum>(tls_actor_data(),iterator->next_actor_index);
  iterator->actor_index=iterator->next_actor_index;
  iterator->next_actor_index=actor->next_clump_actor_index;
 }
 return actor;
}
// @address 0xace68 ?clump_dialogue_update@@YAXJ@Z
void clump_dialogue_update(long clump_index) {
 ai_dialogue_definitions *globals=0;
 if(global_game_globals && global_game_globals->ai_globals.count()>0) {
  long index=global_game_globals->ai_globals.begin()->dialogue_globals;
  if(index!=-1) globals=TAG_GET('adlg',ai_dialogue_definitions,index);
 }
 if(!globals) return;
 clump_datum *clump=datum_get<clump_datum>(tls_clump_data(),clump_index);
 e_prop_dialogue_event event=prop_dialogue_event_none;
 e_dialogue_state next_state=dialogue_state_none;
 clump_actor_iterator actors;
 clump_actor_iterator_new(&actors,datum_get<clump_datum>(tls_clump_data(),clump_index));
 while(actor_datum *actor=prop_dialogue_actor_iterator_next(&actors)) {
  actor_prop_ref_iterator refs;
  actor_prop_ref_iterator_new(&refs,actors.actor_index);
  while(prop_ref_datum *ref=actor_prop_ref_iterator_next(&refs)) {
   if(ref->dialogue_ticks && --ref->dialogue_ticks==0) {
    ref->m_flags.set(prop_ref_dialogue_flag,true);
    if(ref->dialogue_event>event) {
     e_dialogue_state state=dialogue_state_none;
     if(clump_dialogue_event_allowed(clump_index,ref->prop_index,false,(e_prop_dialogue_event)ref->dialogue_event,0,&state)) {
      event=(e_prop_dialogue_event)ref->dialogue_event;
      next_state=state;
     }
    }
   }
  }
 }
 if(event>prop_dialogue_event_none) {
  c_dialogue_option options[100];
  short option_count=0;
  clump_actor_iterator_new(&actors,datum_get<clump_datum>(tls_clump_data(),clump_index));
  while(actor_datum *actor=prop_dialogue_actor_iterator_next(&actors)) {
   if(actor->unit_index!=-1) {
    actor_prop_ref_iterator refs;
    actor_prop_ref_iterator_new(&refs,actors.actor_index);
    while(prop_ref_datum *ref=actor_prop_ref_iterator_next(&refs)) {
     if(ref->dialogue_event==event && clump_dialogue_event_allowed(clump_index,ref->prop_index,false,(e_prop_dialogue_event)ref->dialogue_event,0,0)) {
      ai_information_data information={0};
      ai_information_data *information_pointer=0;
      if(ref->dialogue_event==5) {
       information.values[0]=3;
       information.values[2]=ref->object_index;
       information_pointer=&information;
      }
      if(ref->dialogue_index) {
       c_dialogue_event dialogue_event((signed char)ref->dialogue_index,actor->unit_index,ref->object_index,-1,-1,information_pointer);
       ai_dialogue_event_aggregate_options(&dialogue_event,options,&option_count,100);
      }
      ref->m_flags.set(prop_ref_dialogue_flag,true);
     }
    }
   }
  }
  if(option_count>0) {
   short selected=ai_dialogue_event(options,option_count);
   if(selected!=-1 && event==5) {
    c_dialogue_option *option=&options[selected];
    if(option->event.speaker && option->event.target_index!=-1) {
     long prop_index=clump_get_clump_prop(clump_index,option->event.target_index);
     if(prop_index!=-1) (datum_get<prop_datum>(tls_ai_prop_data(),prop_index))->previous_dialogue_flags|=1;
    }
    clump->dialogue_ready=true;
   }
  }
  if(next_state>0) clump->dialogue_state=next_state;
 }
 clump_actor_iterator_new(&actors,datum_get<clump_datum>(tls_clump_data(),clump_index));
 while(actor_datum *actor=prop_dialogue_actor_iterator_next(&actors)) {
  float *dialogue=0;
  long character_index=actor->character_index;
  while(character_index!=-1) {
   character_definition *character=TAG_GET('char',character_definition,character_index);
   if(character->vocalization_properties.count>0) { dialogue=(float *)character->vocalization_properties.address; break; }
   character_index=character->parent_character.index;
  }
  float probability=0.1f;
  if(dialogue && !(dialogue[0]<0.1f)) probability=dialogue[0];
  actor_prop_ref_iterator refs;
  actor_prop_ref_iterator_new(&refs,actors.actor_index);
  while(prop_ref_datum *ref=actor_prop_ref_iterator_next(&refs)) {
   if(ref->m_flags.test(prop_ref_dialogue_flag)) {
    bool clear=true;
    if(ref->dialogue_index==3) {
     prop_datum *prop=datum_get<prop_datum>(tls_ai_prop_data(),ref->prop_index);
     if(!(prop->previous_dialogue_flags&1)) {
      if(!(_real_random_range(tls_random_seed(),0.0f,1.0f)<probability)) {
       clear=false;
       ref->dialogue_ticks=(unsigned char)game_seconds_to_ticks_round(1.0f);
      }
     }
    }
    if(clear) { ref->dialogue_event=0; ref->dialogue_index=0; }
   }
   ref->m_flags.set(prop_ref_dialogue_flag,false);
  }
 }
 if(clump->first_actor!=-1) {
  actor_datum *actor=datum_get<actor_datum>(tls_actor_data(),clump->first_actor);
  if(actor->unit_index!=-1) {
   clump_prop_iterator props;
   clump_prop_iterator_new(&props,datum_get<clump_datum>(tls_clump_data(),clump_index));
   while(prop_datum *prop=clump_prop_iterator_next(&props)) {
    long index=props.index;
    if(prop->valid) {
     if((prop->dialogue_flags&2) && !(prop->previous_dialogue_flags&2) && (prop->flags&128) && prop->count>=8 && prop->count<=11) {
      if(prop->lost_ticks>0) --prop->lost_ticks;
      if(prop->lost_ticks==0) {
       if(clump_dialogue_allowed(clump_index,index,dialogue_state_3)) {
        c_dialogue_event dialogue_event(89,actor->unit_index,prop->object_index,-1,-1,0);
        ai_dialogue_event(&dialogue_event);
        prop->previous_dialogue_flags|=2;
       } else prop->lost_ticks=(short)game_seconds_to_ticks_round(3.0f);
      }
     }
     if(prop->count>=7) {
      bool allowed=true;
      if(prop->actor_index!=-1) allowed=(datum_get<actor_datum>(tls_actor_data(),prop->actor_index))->combat_status>=3;
      if(allowed) {
       if(prop->comment_ticks>0) --prop->comment_ticks;
       if(prop->comment_ticks==0) {
        if(clump_dialogue_allowed(clump_index,index,dialogue_state_3)) {
         c_dialogue_event dialogue_event(81,actor->unit_index,prop->object_index,-1,-1,0);
         ai_dialogue_event(&dialogue_event);
        }
        float seconds=_real_random_range(tls_random_seed(),globals->strike_delay_minimum,globals->strike_delay_maximum);
        if(!(seconds<=0.0f)) prop->comment_ticks=(short)game_seconds_to_ticks_round(seconds);
        else prop->comment_ticks=-1;
       }
      }
     }
    }
   }
  }
 }
}


// @address 0x564570 ?prop_calculate_dialogue_flags@@YAXJJ@Z
void prop_calculate_dialogue_flags(long clump_index, long prop_index) {
 prop_datum *prop=datum_get<prop_datum>(tls_ai_prop_data(),prop_index);
 prop->dialogue_flags=0;
 long unit_index=clump_get_first_unit(clump_index);
 if(unit_index!=-1) {
  c_dialogue_event visible_event(3,unit_index,prop->object_index,-1,-1,0);
  c_dialogue_event hostile_event(89,unit_index,prop->object_index,-1,-1,0);
  if(ai_dialogue_event_has_valid_pattern(&visible_event)) prop->dialogue_flags|=1;
  else prop->dialogue_flags&=~1;
  if(ai_dialogue_event_has_valid_pattern(&hostile_event)) prop->dialogue_flags|=2;
  else prop->dialogue_flags&=~2;
 }
 prop->dialogue_flags|=16;
}
