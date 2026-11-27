// ailibxbox/character_definitions.obj: from the records 0xadcf0 0xae150 0xafb40 0xafbc8 0xafe20 0xb4630 0xb6668 0xbbff0 0xbefb0 0xbf010 0xbf098 0xc0430 0xc1cd0 0xc3490 0xc3870 0xc8f30 0xcb3c0 0xd2588 0xe9a90 0x518ed8 0x518f20 0x519000 0x5190e0 0x5191c0
// @flags /TP /O2
#include "game/game_time_types.h"
#include "ai/character_behavior_properties.h"
#include "ai/character_weapon_properties.h"
#include "game/game_globals.h"
#include "memory/tls_globals.h"
#include "game/game_globals_storage.h"
#include "objects/object_header.h"
// @address 0xadcf0 ?character_definition_general_properties_get@@YAPAUcharacter_general_properties@@J@Z
// @address 0xae150 ?character_definition_movement_properties_get@@YAPAUcharacter_movement_properties@@J@Z
// @address 0xb4630 ?character_definition_charge_multi_properties_get@@YAPAUcharacter_charge_properties@@J@Z
// @address 0xb6668 ?character_vehicle_properties_get@@YAPAUcharacter_vehicle_properties@@JJ@Z
// @address 0xbefb0 ?actor_grenade_properties_get@@YAPAUcharacter_grenade_properties@@J@Z
// @address 0xbf010 ?character_grenade_properties_get@@YAPAUcharacter_grenade_properties@@JF@Z
// @address 0xbf098 ?character_definition_engage_multi_properties_get@@YAPAUcharacter_engage_properties@@J@Z
// @address 0xc1cd0 ?actor_firing_pattern_properties_get@@YAPAUcharacter_firing_pattern_properties@@J@Z
// @address 0xc3870 ?character_definition_cover_multi_properties_get@@YAPAUcharacter_cover_properties@@J@Z
// @address 0xc8f30 ?character_definition_retreat_multi_properties_get@@YAPAUcharacter_retreat_properties@@J@Z
// @address 0xcb3c0 ?character_definition_presearch_multi_properties_get@@YAPAUcharacter_presearch_properties@@J@Z
// @address 0xd2588 ?character_definition_search_multi_properties_get@@YAPAUcharacter_search_properties@@J@Z
// @address 0xe9a90 ?character_definition_vitality_properties_get@@YAPAUcharacter_vitality_properties@@J@Z
// @address 0x518ed8 ?character_definition_morph_properties_get@@YAPAUcharacter_morph_properties@@J@Z
// @address 0x518f20 ?character_definition_swarm_multi_properties_get@@YAPAUcharacter_swarm_properties@@J@Z
// @address 0x519000 ?character_definition_ready_multi_properties_get@@YAPAUcharacter_ready_properties@@J@Z
// @address 0x5190e0 ?character_definition_combatform_multi_properties_get@@YAPAUcharacter_combatform_properties@@J@Z
// @address 0x5191c0 character_definition_charge_entry_get
// @address 0xb5108 exp
#include "ai/actors.h"
#include "ai/actor_datum.h"
#include "ai/character_definitions.h"
#include "ai/character_properties_types.h"
#include "ai/styles.h"
#include "tag_files/tag_types.h"
#include "ai/perception_mode_types.h"
#include "ai/actor_perception.h"
#include "cache/cache_files.h"
#include "tag_files/tag_groups.h"
#include "memory/data.h"
#include "game/game.h"
#include "game/game_time_inline.h"
#include "scenario/scenario.h"
#include "ai/character_morph_properties.h"

#include "ai/character_firing_pattern_properties.h"
#include "ai/character_perception.h"
#include "ai/actor_type_flood_types.h"
#include "ai/ai_globals_definition.h"
#include "ai/character_idle_properties.h"
#include "ai/character_evasion_properties.h"
#include "units/unit_types.h"
#include <math.h>


character_general_properties *character_definition_general_properties_get(long tag_index)
{
  character_general_properties *result = 0;
  while (tag_index != -1) {
    character_definition *definition = TAG_GET('char', character_definition, tag_index);
    if (definition->general_properties.count > 0) {
      result = (character_general_properties *)definition->general_properties.address;
      break;
    }
    tag_index = definition->parent_character.index;
  }
  return result;
}

character_movement_properties *character_definition_movement_properties_get(long tag_index)
{
    character_movement_properties *result = 0;
    while (tag_index != -1) {
        character_definition *definition = TAG_GET('char', character_definition, tag_index);
        if (definition->movement_properties.count > 0) {
            result = (character_movement_properties *)definition->movement_properties.address;
            break;
        }
        tag_index = definition->parent_character.index;
    }
    return result;
}

character_charge_properties *character_definition_charge_multi_properties_get(long player_index)
{
  character_charge_properties *result = 0;
  short variant = 0;
  long definition_index = datum_get<actor_datum>(tls_actor_data(), player_index)->character_index;
  style_definition *representation = (style_definition *)actor_get_task_style(player_index);
  if (representation) {
    variant = representation->attitude;
  }
  while (definition_index != -1) {
    character_definition *definition = TAG_GET('char', character_definition, definition_index);
    if (definition->charge_properties.count > 0) {
      if (variant < definition->charge_properties.count) {
        result = &((character_charge_properties *)definition->charge_properties.address)[variant];
      } else {
        result = (character_charge_properties *)definition->charge_properties.address;
      }
      break;
    }
    definition_index = definition->parent_character.index;
  }
  return result;
}

static __forceinline character_vehicle_properties *character_find_entry(character_definition *definition, long key)
{
    for (short i = 0; i < definition->vehicle_properties.count; i++) {
        character_vehicle_properties *entry = &((character_vehicle_properties *)definition->vehicle_properties.address)[i];
        if (entry->key == key) {
            return entry;
        }
    }
    return 0;
}

static __forceinline character_grenade_properties *character_find_grenade_entry(character_definition *definition, short identifier)
{
    for (short i = 0; i < definition->grenades_properties.count; i++) {
        character_grenade_properties *element = &((character_grenade_properties *)definition->grenades_properties.address)[i];
        if (element->identifier == identifier) {
            return element;
        }
    }
    return 0;
}

static __forceinline character_weapon_properties *character_find_weapon_entry(character_definition *definition, long weapon_definition_index)
{
    for (short i = 0; i < definition->weapons_properties.count; i++) {
        character_weapon_properties *properties = &((character_weapon_properties *)definition->weapons_properties.address)[i];
        if (properties->weapon_definition_index == weapon_definition_index) {
            return properties;
        }
    }
    return 0;
}

static __forceinline character_firing_pattern_properties *character_find_firing_pattern_entry(character_definition *definition, long target)
{
    for (short i = 0; i < definition->firing_pattern_properties.count; i++) {
        character_firing_pattern_properties *entry = &((character_firing_pattern_properties *)definition->firing_pattern_properties.address)[i];
        if (entry->weapon.index == target) {
            return entry;
        }
    }
    return 0;
}

character_vehicle_properties *character_vehicle_properties_get(long actor_index, long key)
{
    character_vehicle_properties *result = 0;
    actor_datum *actor = datum_get<actor_datum>(tls_actor_data(), actor_index);
    int tag_index = actor->character_index;
    while (tag_index != -1) {
        character_definition *definition = TAG_GET('char', character_definition, tag_index);
        result = character_find_entry(definition, key);
        if (result) {
            break;
        }
        tag_index = definition->parent_character.index;
    }
    return result;
}

// 0xbbff0
character_idle_properties *character_definition_idle_multi_properties_get(long actor_index)
{
  actor_datum *actor = datum_get<actor_datum>(tls_actor_data(), actor_index);
  long definition_index = actor->character_index;
  character_idle_properties *result = 0;
  short variant = 0;
  style_definition *style = (style_definition *)actor_get_task_style(actor_index);
  if (style)
    variant = style->attitude;
  while (definition_index != -1) {
    character_definition *definition = TAG_GET('char', character_definition, definition_index);
    if (definition->idle_properties.count > 0) {
      if (variant < definition->idle_properties.count)
        result = &((character_idle_properties *)definition->idle_properties.address)[variant];
      else
        result = (character_idle_properties *)definition->idle_properties.address;
      break;
    }
    definition_index = definition->parent_character.index;
  }
  return result;
}

character_grenade_properties *actor_grenade_properties_get(long player_index)
{
  character_grenade_properties *result = 0;
  actor_datum *player = datum_get<actor_datum>(tls_actor_data(), player_index);
  if (player->unit_index != -1) {
    short weapon_index = OBJECT_GET(player->unit_index, unit_datum, _object_mask_unit)->unit.current_grenade_index;
    if (weapon_index != -1)
      result = character_grenade_properties_get(player->character_index, weapon_index);
  }
  return result;
}

character_grenade_properties *character_grenade_properties_get(long tag_index, short identifier)
{
    character_grenade_properties *result = 0;

    if (tag_index != -1) {
        do {
            character_definition *definition = TAG_GET('char', character_definition, tag_index);
            result = character_find_grenade_entry(definition, identifier);
            if (result) {
                break;
            }
            tag_index = definition->parent_character.index;
        } while (tag_index != -1);
    }
    return result;
}

character_engage_properties *character_definition_engage_multi_properties_get(long player_index)
{
  character_engage_properties *result = 0;
  short variant = 0;
  long definition_index = datum_get<actor_datum>(tls_actor_data(), player_index)->character_index;
  style_definition *representation = (style_definition *)actor_get_task_style(player_index);
  if (representation) {
    variant = representation->attitude;
  }
  while (definition_index != -1) {
    character_definition *definition = TAG_GET('char', character_definition, definition_index);
    if (definition->engage_properties.count > 0) {
      if (variant < definition->engage_properties.count) {
        result = &((character_engage_properties *)definition->engage_properties.address)[variant];
      } else {
        result = (character_engage_properties *)definition->engage_properties.address;
      }
      break;
    }
    definition_index = definition->parent_character.index;
  }
  return result;
}

// 0xc0430
character_weapon_properties *character_weapon_properties_get(long definition_index, long weapon_definition_index)
{
  character_weapon_properties *result = 0;
  while (definition_index != -1) {
    character_definition *definition = TAG_GET('char', character_definition, definition_index);
    result = character_find_weapon_entry(definition, weapon_definition_index);
    if (result) {
      break;
    }
    definition_index = definition->parent_character.index;
  }
  return result;
}

character_firing_pattern_properties *actor_firing_pattern_properties_get(long player_index)
{
  character_firing_pattern_properties *result = 0;
  long weapon = actor_get_weapon(player_index);
  if (weapon != -1) {
    actor_datum *player = datum_get<actor_datum>(tls_actor_data(), player_index);
    object_datum *object = object_get(weapon);
    character_firing_pattern_properties *found = 0;
    long base = player->character_index;
    long target = object->definition_index;
    if (base != -1) {
      do {
        character_definition *definition = TAG_GET('char', character_definition, base);
        found = character_find_firing_pattern_entry(definition, target);
        if (found) {
          break;
        }
        base = definition->parent_character.index;
      } while (base != -1);
    }
    result = found;
  }
  return result;
}

// 0xc3490
character_evasion_properties *character_definition_evasion_multi_properties_get(long actor_index)
{
  actor_datum *actor = datum_get<actor_datum>(tls_actor_data(), actor_index);
  long definition_index = actor->character_index;
  character_evasion_properties *result = 0;
  short variant = 0;
  style_definition *style = (style_definition *)actor_get_task_style(actor_index);
  if (style)
    variant = style->attitude;
  while (definition_index != -1) {
    character_definition *definition = TAG_GET('char', character_definition, definition_index);
    if (definition->evasion_properties.count > 0) {
      if (variant < definition->evasion_properties.count)
        result = &((character_evasion_properties *)definition->evasion_properties.address)[variant];
      else
        result = (character_evasion_properties *)definition->evasion_properties.address;
      break;
    }
    definition_index = definition->parent_character.index;
  }
  return result;
}

character_cover_properties *character_definition_cover_multi_properties_get(long player_index)
{
  character_cover_properties *result = 0;
  short variant = 0;
  long tag_index = datum_get<actor_datum>(tls_actor_data(), player_index)->character_index;
  style_definition *variant_def = (style_definition *)actor_get_task_style(player_index);
  if (variant_def)
    variant = variant_def->attitude;
  while (tag_index != -1) {
    character_definition *definition = TAG_GET('char', character_definition, tag_index);
    if (definition->cover_properties.count > 0) {
      if (variant < definition->cover_properties.count)
        result = &((character_cover_properties *)definition->cover_properties.address)[variant];
      else
        result = (character_cover_properties *)definition->cover_properties.address;
      break;
    }
    tag_index = definition->parent_character.index;
  }
  return result;
}

character_retreat_properties *character_definition_retreat_multi_properties_get(long player_index)
{
  character_retreat_properties *result = 0;
  short variant = 0;
  long definition_index = datum_get<actor_datum>(tls_actor_data(), player_index)->character_index;
  style_definition *representation = (style_definition *)actor_get_task_style(player_index);
  if (representation) {
    variant = representation->attitude;
  }
  while (definition_index != -1) {
    character_definition *definition = TAG_GET('char', character_definition, definition_index);
    if (definition->retreat_properties.count > 0) {
      if (variant < definition->retreat_properties.count) {
        result = &((character_retreat_properties *)definition->retreat_properties.address)[variant];
      } else {
        result = (character_retreat_properties *)definition->retreat_properties.address;
      }
      break;
    }
    definition_index = definition->parent_character.index;
  }
  return result;
}

character_presearch_properties *character_definition_presearch_multi_properties_get(long player_index)
{
  character_presearch_properties *result = 0;
  short variant = 0;
  long definition_index = datum_get<actor_datum>(tls_actor_data(), player_index)->character_index;
  style_definition *representation = (style_definition *)actor_get_task_style(player_index);
  if (representation) {
    variant = representation->attitude;
  }
  while (definition_index != -1) {
    character_definition *definition = TAG_GET('char', character_definition, definition_index);
    if (definition->presearch_properties.count > 0) {
      if (variant < definition->presearch_properties.count) {
        result = &((character_presearch_properties *)definition->presearch_properties.address)[variant];
      } else {
        result = (character_presearch_properties *)definition->presearch_properties.address;
      }
      break;
    }
    definition_index = definition->parent_character.index;
  }
  return result;
}

character_search_properties *character_definition_search_multi_properties_get(long player_index)
{
  character_search_properties *result = 0;
  short index = 0;
  long tag_index = datum_get<actor_datum>(tls_actor_data(), player_index)->character_index;
  style_definition *variant = (style_definition *)actor_get_task_style(player_index);
  if (variant)
    index = variant->attitude;
  while (tag_index != -1) {
    character_definition *definition = TAG_GET('char', character_definition, tag_index);
    if (definition->search_properties.count > 0) {
      if (index < definition->search_properties.count)
        result = &((character_search_properties *)definition->search_properties.address)[index];
      else
        result = (character_search_properties *)definition->search_properties.address;
      break;
    }
    tag_index = definition->parent_character.index;
  }
  return result;
}


// @address 0xafbc8 actor_update_emotion
extern "C" void actor_update_emotion(unsigned short actor_index)
{
  actor_datum *actor = datum_get<actor_datum>(tls_actor_data(), actor_index);
  short level = actor->situation.best_prop_class;
  actor->activity_scariness = 0.0f;
  s_game_globals *globals = global_game_globals;
  if (globals != 0 && globals->ai_globals.s_tag_block::count > 0) {
    s_ai_globals_definition *ai = (s_ai_globals_definition *)globals->ai_globals.address;
    if (level >= 13) {
      float first = 0.25f;
      float second = 0.25f;
      character_vitality_properties *vitality = character_definition_vitality_properties_get(actor->character_index);
      if (vitality != 0) {
        if (vitality->shield_damage_threshold > 0.0f)
          first = vitality->shield_damage_threshold;
        if (vitality->recoverable_damage_threshold > 0.0f)
          second = vitality->recoverable_damage_threshold;
      }
      if (actor->body_damage > first)
        actor->activity_scariness = ai->danger_extended_body_damage;
      else if (actor->body_damage > 0.0f)
        actor->activity_scariness = ai->danger_body_damage;
      else if (actor->shield_damage > second)
        actor->activity_scariness = ai->danger_extended_shield_damage;
      else
        actor->activity_scariness = ai->danger_shield_damage;
    } else if (level >= 12) {
      actor->activity_scariness = ai->danger_extremely_close;
    } else if (level >= 11) {
      actor->activity_scariness = ai->danger_shooting_at;
    } else if (level >= 10) {
      actor->activity_scariness = ai->danger_shooting_near;
    } else if (level >= 9) {
      actor->activity_scariness = ai->danger_broadly_facing;
    } else {
      actor->activity_scariness = 0.0f;
    }
  }
  float decay;
  if (actor->activity_scariness > actor->danger) {
    long ticks = game_seconds_to_ticks_round(0.25f);
    decay = (float)exp((double)(-0.6931472f / (float)ticks));
  } else {
    long ticks = game_seconds_to_ticks_round(1.0f);
    decay = (float)exp((double)(-0.6931472f / (float)ticks));
  }
  actor->danger = actor->danger + (actor->activity_scariness - actor->danger) * (1.0f - decay);
  if (actor->combat_status >= 2)
    actor->behavior_mode = 4;
  else if (actor->combat_status >= 1)
    actor->behavior_mode = 3;
  else
    actor->behavior_mode = 1;
}

character_vitality_properties *character_definition_vitality_properties_get(long tag_index)
{
  character_vitality_properties *result = 0;
  while (tag_index != -1) {
    character_definition *definition = TAG_GET('char', character_definition, tag_index);
    if (definition->vitality_properties.count > 0) {
      result = (character_vitality_properties *)definition->vitality_properties.address;
      break;
    }
    tag_index = definition->parent_character.index;
  }
  return result;
}

character_morph_properties *character_definition_morph_properties_get(long tag_index)
{
  character_morph_properties *result = 0;

  while (tag_index != -1) {
    character_definition *definition = TAG_GET('char', character_definition, tag_index);
    if (definition->morph_properties.count > 0) {
      result = (character_morph_properties *)definition->morph_properties.address;
      break;
    }
    tag_index = definition->parent_character.index;
  }
  return result;
}

character_swarm_properties *character_definition_swarm_multi_properties_get(long player_index)
{
  character_swarm_properties *result = 0;
  short variant = 0;
  long definition_index = datum_get<actor_datum>(tls_actor_data(), player_index)->character_index;
  style_definition *representation = (style_definition *)actor_get_task_style(player_index);
  if (representation) {
    variant = representation->attitude;
  }
  while (definition_index != -1) {
    character_definition *definition = TAG_GET('char', character_definition, definition_index);
    if (definition->swarm_properties.count > 0) {
      if (variant < definition->swarm_properties.count) {
        result = &((character_swarm_properties *)definition->swarm_properties.address)[variant];
      } else {
        result = (character_swarm_properties *)definition->swarm_properties.address;
      }
      break;
    }
    definition_index = definition->parent_character.index;
  }
  return result;
}

character_ready_properties *character_definition_ready_multi_properties_get(long player_index)
{
  character_ready_properties *result = 0;
  long definition_index = datum_get<actor_datum>(tls_actor_data(), player_index)->character_index;
  short entry_index = 0;
  style_definition *selector = (style_definition *)actor_get_task_style(player_index);
  if (selector)
    entry_index = selector->attitude;
  while (definition_index != -1) {
    character_definition *definition = TAG_GET('char', character_definition, definition_index);
    if (definition->ready_properties.count > 0) {
      if (entry_index < definition->ready_properties.count)
        result = &((character_ready_properties *)definition->ready_properties.address)[entry_index];
      else
        result = (character_ready_properties *)definition->ready_properties.address;
      break;
    }
    definition_index = definition->parent_character.index;
  }
  return result;
}

character_combatform_properties *character_definition_combatform_multi_properties_get(long player_index)
{
  character_combatform_properties *result = 0;
  long definition_index = datum_get<actor_datum>(tls_actor_data(), player_index)->character_index;
  short entry_index = 0;
  style_definition *selector = (style_definition *)actor_get_task_style(player_index);
  if (selector)
    entry_index = selector->attitude;
  while (definition_index != -1) {
    character_definition *definition = TAG_GET('char', character_definition, definition_index);
    if (definition->combatform_properties.count > 0) {
      if (entry_index < definition->combatform_properties.count)
        result = &((character_combatform_properties *)definition->combatform_properties.address)[entry_index];
      else
        result = (character_combatform_properties *)definition->combatform_properties.address;
      break;
    }
    definition_index = definition->parent_character.index;
  }
  return result;
}

s_character_charge_entry *character_definition_charge_entry_get(long player_index)
{
  character_charge_properties *charge = character_definition_charge_multi_properties_get(player_index);
  s_character_charge_entry *result = 0;
  if (charge && charge->entry_count > 0) {
    short difficulty = tls_game_globals()->options.game_mode == 1 ? tls_game_globals()->options.campaign_difficulty : 1;
    if (difficulty < charge->entry_count)
      return &charge->entries[difficulty];
    result = charge->entries;
  }
  return result;
}

// @address 0xafe20 actor_perception_properties_get
s_mode_entry *actor_perception_properties_get(int actor_index)
{
  int definition_index = datum_get<actor_datum>(tls_actor_data(), actor_index)->character_index;
  int mode = actor_get_perception_mode(actor_index);
  s_mode_entry *result = 0;
  if (definition_index != -1) {
    do {
      character_definition *definition = TAG_GET('char', character_definition, definition_index);
      if (mode < definition->perception_properties.count)
        return &((s_mode_entry *)definition->perception_properties.address)[mode];
      if (definition->perception_properties.count > 0) {
        result = (s_mode_entry *)definition->perception_properties.address;
        break;
      }
      definition_index = definition->parent_character.index;
    } while (definition_index != -1);
  }
  return result;
}

// @address 0xafb40 ?actor_weapon_properties_get@@YAPAUcharacter_weapon_properties@@J@Z
character_weapon_properties *actor_weapon_properties_get(long actor_index)
{
    character_weapon_properties *result = 0;
    long weapon_index = actor_get_weapon(actor_index);
    if (weapon_index != -1) {
        actor_datum *actor = datum_get<actor_datum>(tls_actor_data(), actor_index);
        object_datum *weapon = object_get(weapon_index);
        result = character_weapon_properties_get(actor->character_index, weapon->definition_index);
    }
    return result;
}
