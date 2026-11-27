#ifndef INTERFACE_CHUD_CHUD_UPDATE_USER_DATA_TYPES_H
#define INTERFACE_CHUD_CHUD_UPDATE_USER_DATA_TYPES_H

#include "interface/chud/chud.h"
#include "interface/chud/chud_definitions.h"
#include "interface/chud/chud_user_data_types.h"
#include "game/player_configuration.h"
#include "game/game_engine_interface_state_types.h"
#include "math/real_math_types.h"
#include <stddef.h>

struct s_aim_assist_targeting_result;

struct chud_saved_film_progress
{
    float duration;
    float position;
    long marker_count;
    float progress;
    float buffered;
    float rate;
    bool active;
    float markers[10];
};

struct weapon_state
{
    unsigned short active_flags_0;
    unsigned short active_flags_2;
    unsigned short active_flags_4;
    unsigned short active_flags_6;
    unsigned short flashingFlags;
    float heat;
    unsigned int : 32;
    float age;
    float analog_value;
    float overheated;
    float charge;
    float rounds_loaded;
    float rounds_inventory;
    float rounds_total;
    float magazine;
    unsigned char magazine_active;
    unsigned char control_flag_1;
    unsigned char tracking;
    unsigned char locking;
    unsigned char locked;
    float lock_fraction;
    real_point3d target_position;
};
typedef char chud_weapon_state_size[(sizeof(weapon_state) == 0x4c) ? 1 : -1];
typedef char chud_weapon_state_heat_offset[(offsetof(weapon_state, heat) == 0xc) ? 1 : -1];
typedef char chud_weapon_state_age_offset[(offsetof(weapon_state, age) == 0x14) ? 1 : -1];
typedef char chud_weapon_state_locked_offset[(offsetof(weapon_state, locked) == 0x38) ? 1 : -1];
typedef char chud_weapon_state_target_offset[(offsetof(weapon_state, target_position) == 0x40) ? 1 : -1];
struct definition_info { long index; long definition_index; weapon_state weapon; };

struct chud_metagame_state
{
    long player_count;
    long scores[4];
    bool local[4];
    union
    {
        s_emblem_info emblems[4];
        s_chud_color_labels color_labels[4];
    };
    short teams[4];
    float time;
    float multiplier;
    float score_scale;
    long survival_time;
    bool bonus_round;
    long bonus_round_index;
    bool bonus_round_timer;
};
typedef char chud_metagame_state_size[(sizeof(chud_metagame_state) == 0x5c) ? 1 : -1];
typedef char chud_metagame_state_local_offset[(offsetof(chud_metagame_state, local) == 0x14) ? 1 : -1];
typedef char chud_metagame_state_color_labels_offset[(offsetof(chud_metagame_state, color_labels) == 0x18) ? 1 : -1];
typedef char chud_metagame_state_teams_offset[(offsetof(chud_metagame_state, teams) == 0x38) ? 1 : -1];
typedef char chud_metagame_state_time_offset[(offsetof(chud_metagame_state, time) == 0x40) ? 1 : -1];
typedef char chud_metagame_state_bonus_round_offset[(offsetof(chud_metagame_state, bonus_round) == 0x50) ? 1 : -1];
typedef char chud_metagame_state_bonus_round_timer_offset[(offsetof(chud_metagame_state, bonus_round_timer) == 0x58) ? 1 : -1];
class c_chud_update_user_data;
class c_chud_update_user_data {
 friend c_chud_update_user_data *::chud_update_user_data_build(c_chud_update_user_data *update_data, long user_index, long last_unit_index, bool force);
public:
 long user_index;
 long player_index;
 long unit_index;
 unsigned int : 32;
 bool unit_changed;

 definition_info definitions[12];
 unsigned short active_flags[12];
 long flashingFlags;
 unsigned short flags;
 unsigned short : 16;
 bool unit_valid; float zoom_level; float secondary_zoom_level; float body_vitality; float shield_vitality;
 long selected_grenade_index;
 long grenade_counts[4];
 bool primary_weapon;
 bool alternate_state;

 long autoaim_target_type;
 bool seat_flag;

 long frag_grenade_count;
 long plasma_grenade_count;
 unsigned long state_flags;
 bool zoom_unavailable;
 unsigned char can_show_action_prompt;

 float energy_fraction; bool has_shield;
 long autoaim_flags; bool weapon_ready_sound; bool weapon_overheat_sound;  long highlight_index; long autoaim_target_index;
 bool hud_changed;
 bool hud_reset;

 game_engine_interface_state hud_state;
 chud_metagame_state metagame_state;
 chud_saved_film_progress saved_film_progress;
 union { struct { long external_count_6dc; long external_types_6e0[3]; }; long network_status[4]; };
protected:
 void study_autoaim(long unit_index);
 void compute_weapon_update(long weapon_index, long slot, const s_aim_assist_targeting_result *targeting);
public:
 static bool widget_definition_should_be_impulse(long user_index, s_chud_widget_base *, s_chud_widget_collection *, long object_index, long *count);
 __forceinline weapon_state *find_weapon_state(long index) {
  weapon_state *result=0;
  if(index != -1) for(long i=0;i<=4;i++) if(definitions[i].index == index) { result=&definitions[i].weapon; break; }
  return result;
 }
 bool widget_definition_should_be_active(s_chud_widget_base*,s_chud_widget_collection*,long);
 bool widget_definition_should_be_flashing(s_chud_widget_base*,s_chud_widget_collection*,long);
 bool widget_definition_should_be_hidden(s_chud_widget_base*,s_chud_widget_collection*,long);
 __forceinline bool widget_definition_type_is_impulse(s_chud_widget_base *widget, s_chud_widget_collection *collection, long index) {
  state *widget_state = widget->states.count() ? widget->states.begin() : 0;
  state *collection_state = collection->states.count() ? collection->states.begin() : 0;
  bool impulse = false;
  if (widget_state || collection_state) {
   for (long i = 0; i < 2; ++i) {
    state *candidate = i ? widget_state : collection_state;
    if (candidate && (candidate->impulse_grenade_flags || candidate->impulse_ammo_flags)) {
     impulse = true;
     break;
    }
   }
  }
  return impulse;
 }
 long compute_desired_widget_state(long user_index, long collection_tag_index, long collection_index, long widget_index, long object_index, bool *hidden);
 long compute_actual_widget_state(long, long, long, long, long, long, long);
 unsigned long get_sound_flags();
 float evaluate_external_input(long input_type, long weapon_index, long state_index, float value);
 __forceinline bool has_primary_weapons() const
 {
  bool result = false;
  if (definitions[0].definition_index != -1 && definitions[2].definition_index != -1)
   result = true;
  return result;
 }
 __forceinline bool has_no_secondary_weapons() const
 {
  bool result = true;
  if (definitions[3].index != -1 || definitions[4].index != -1 || definitions[6].definition_index != -1)
   result = false;
  return result;
 }
 __forceinline float get_score(long count, long index) const { return count > index ? saved_film_progress.markers[index] : -1.0f; }
 __forceinline bool contains_chud_definition_index(long definition_index)
 {
  bool result = false;
  for (long i = 0; i < 12; i++)
  {
   if (definitions[i].definition_index == definition_index)
   {
    result = true;
    break;
   }
  }
  return result;
 }
 __forceinline long get_next_definition_info_index(long index)
 {
  do
   index++;
  while (index < 12 && definitions[index].definition_index == -1);
  if (index == 12)
   index = -1;
  return index;
 }
 __forceinline definition_info *get_definition_info(long index) { return &definitions[index]; }
};
typedef char chud_update_user_data_size[(sizeof(c_chud_update_user_data) == 0x6ec) ? 1 : -1];
typedef char chud_update_user_data_unit_changed_offset[(offsetof(c_chud_update_user_data, unit_changed) == 0x10) ? 1 : -1];
typedef char chud_update_user_data_unit_valid_offset[(offsetof(c_chud_update_user_data, unit_valid) == 0x424) ? 1 : -1];
typedef char chud_update_user_data_hud_state_offset[(offsetof(c_chud_update_user_data, hud_state) == 0x484) ? 1 : -1];
typedef char c_chud_update_user_data_metagame_state_offset[(offsetof(c_chud_update_user_data, metagame_state) == 0x63c) ? 1 : -1];
typedef char c_chud_update_user_data_network_status_offset[(offsetof(c_chud_update_user_data, network_status) == 0x6dc) ? 1 : -1];
typedef char chud_update_user_data_film_progress_offset[(offsetof(c_chud_update_user_data, saved_film_progress) == 0x698) ? 1 : -1];

#endif
