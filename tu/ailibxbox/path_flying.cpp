// ailibxbox/path_flying.obj: from the records 0x56e590 0x56e7f0 0x56e9f8 0x56eac0 0x56ebc8 0x56ed30 0x56ee00 0x56f510
// @flags /TP /O2 /Oc
#include "scenario/scenario_tag.h"
#include "ai/ai_direction_mode_types.h"
#include "memory/tls_globals.h"
#include "memory/data_types.h"
#include "tag_files/tag_groups.h"
#include "objects/object_header.h"
#include "ai/firing_position_ref.h"
#include "physics/collision_types.h"
#include "ai/path_point_types.h"
#include "ai/actor_firing_position.h"
#include "ai/path_flying.h"
#include "math/real_math_types.h"
#include "math/real_math.h"
#include "ai/sector_ref.h"
#include "ai/ai_point3d.h"
#include "ai/ai_direction.h"
#include <string.h>
#include "cseries/cseries_wrapped.h"
#include "memory/static_arrays.h"
#include "ai/ai_reference_frame.h"
#include "objects/object_datum.h"
#include "ai/sector.h"
#include "ai/user_hint_definitions.h"
#include "ai/Zone.h"
#include "scenario/scenario.h"
#include "devices/machine_datum.h"
#include "devices/device_definitions.h"
#include "physics/collision_test_flags.h"
#include "ai/firing_position_definition.h"
#include "physics/collision_test_vector.h"
#include <math.h>
#include "ai/hint_ref.h"
#include "ai/path_result.h"
#include "ai/actor_datum.h"
#include "memory/data.h"
#include "objects/object_type_access.h"
#include "ai/firing_position_ref_inline.h"
// @address 0x56e590 ?flight_evaluation_cache_add_area@@YAXPAUflight_evaluation_cache@@FF@Z
// @address 0x56e7f0 ?flight_test_points@@YA_NPBTreal_point3d@@0JJ@Z
// @address 0x56e9f8 ?test_cache_entry_internal@@YAFPBTreal_point3d@@JFPAUflight_evaluation_cache@@@Z
// @address 0x56eac0 ?flight_hint_any_point_visible@@YA_NJPBTreal_point3d@@JPAUflight_evaluation_cache@@@Z
// @address 0x56ebc8 ?firing_position_path_3d_available@@YA_NPBTreal_point3d@@JUfiring_position_ref@@PAUflight_evaluation_cache@@@Z
// @address 0x56ed30 ?path_3d_available@@YA_NPBTreal_point3d@@PBVc_ai_point3d@@JUfiring_position_ref@@PAUflight_evaluation_cache@@@Z





// Local spelling of distance_squared3d summing j+(k+i); real_math.h sums i+j+k.
static __forceinline float distance_squared3d_inline(const real_point3d *point0,const real_point3d *point1)
{
 real_vector3d vector;
 vector_from_points3d(point0,point1,&vector);
 return vector.j*vector.j+(vector.k*vector.k+vector.i*vector.i);
}
// Inline expansion of collision_test_line; the out-of-line copy lives in collisions.obj.
static __forceinline bool collision_test_line_inline(s_collision_test_flags flags, const real_point3d *point0,
    const real_point3d *point1, long first_ignore_object_index, long second_ignore_object_index, collision_result *collision)
{
 real_vector3d vector;
 vector_from_points3d(point0,point1,&vector);
 return collision_test_vector(flags,false,point0,&vector,first_ignore_object_index,second_ignore_object_index,-1,collision);
}

void flight_evaluation_cache_add_area(flight_evaluation_cache *cache,short zone_index,short area_index) {
 if(cache->count<30) {
  scenario *scenario_view=global_scenario;
  if(zone_index>=0 && zone_index<scenario_view->zones.count) {
   zone_definition *flight_zone=&((zone_definition *)scenario_view->zones.address)[zone_index];
   if(area_index>=0 && area_index<flight_zone->areas.count) {
    area_definition *flight_area=&((area_definition *)flight_zone->areas.address)[area_index]; c_static_flags_no_init<64> visited; visited.clear();
    for(short reference_index=0;reference_index<flight_area->flight_hints.count;reference_index++) {
     s_flight_area_reference *reference=&((s_flight_area_reference *)flight_area->flight_hints.address)[reference_index];
     if(!visited.test(reference->area)) {
      bool already_cached=false; visited.set(reference->area,true);
      for (short point_index = 0; point_index < cache->count; point_index++)
          if (cache->entries[point_index].area == reference->area && cache->entries[point_index].bsp == reference->bsp)
          {
              already_cached = true;
              break;
          }
      if(already_cached) break;
      short active_bsp_mask=(short)g_active_structure_bsp_mask;
      if(active_bsp_mask & (1<<(short)reference->bsp)) {
       s_flight_hints *flight_hints=(global_scenario)->ai_user_hint_data.count>0?(s_flight_hints *)global_scenario->ai_user_hint_data.address:0;
       if(flight_hints && reference->area>=0 && reference->area<flight_hints->count) {
        s_flight_points *flight_points=&flight_hints->entries[reference->area];
        for(short point_index=0;point_index<flight_points->count;point_index++) {
         cache->entries[cache->count].area=reference->area;
         cache->entries[cache->count].bsp=reference->bsp;
         cache->entries[cache->count].point=point_index;
         cache->entries[cache->count].state=0;
         cache->count++;
         if(cache->count>=30) { break; }
        }
        if(cache->count>=30) break;
       }
      }
     }
    }
   }
  }
 }
}

bool flight_test_points(const real_point3d *start,const real_point3d *end,long first_ignore_object_index,long second_ignore_object_index)
{
 collision_result collision;
 short count=0;
 bool again=true;
 bool result=true;
 real_point3d point;
 s_collision_test_flags flags=k_flying_vehicle_collision_flags;
 flags.object_flags&=~4ul;
 while(again && count<4)
 {
  again=false;
  if(collision_test_line_inline(flags,start,end,first_ignore_object_index,second_ignore_object_index,&collision))
  {
   if(collision.t>=1.0f || distance_squared3d_inline(start,end)*((1.0f-collision.t)*(1.0f-collision.t))<0.1f) result=true;
   else
   {
    result=false;
    if(collision.type==4 && ((1<<object_get_type(collision.object_index))&128))
    {
     s_machine_datum *object=(s_machine_datum *)object_get_and_verify_type(collision.object_index, NONE);
     s_machine_definition *definition=(s_machine_definition *)tag_get('mach',object->definition);
     if(definition->type==0 && object->power>0.0f && !(object->machine_flags.test(_machine_flag_0)))
     {
      point=collision.point; start=&point; first_ignore_object_index=collision.object_index; again=true; result=true;
     }
    }
   }
  }
  count++;
 }
 return result;
}

short test_cache_entry_internal(const real_point3d *point,long first_ignore_object_index,short entry_index,flight_evaluation_cache *cache)
{
 s_flight_evaluation_entry *entry=&cache->entries[entry_index];
 short state=entry->state;
 if(state==0)
 {
  scenario *scenario_view=global_scenario;
  s_flight_hints *flight_hints=scenario_view->ai_user_hint_data.count>0?((s_flight_hints *)scenario_view->ai_user_hint_data.address):0;
  state=2;
  if(flight_hints)
  {
   s_flight_points *flight_points=&flight_hints->entries[entry->area];
   real_point3d *end=&(flight_points->data)[entry->point];
   state=(flight_test_points(point,end,first_ignore_object_index,-1)==0)+1;
  }
  entry->state=state;
 }
 return state;
}

bool flight_hint_any_point_visible(long hint_index,const real_point3d *point,long unit_index,flight_evaluation_cache *cache)
{
 bool visible=false;
 bool cached=false;
 for(short i=0;i<cache->count;i++)
 {
  if(cache->entries[i].area==hint_index)
  {
   cached=true;
   if(cache->entries[i].state==1) visible=true;
  }
 }
 if(!visible && cached)
 {
  for(short i=0;i<cache->count;i++)
  {
   if(cache->entries[i].area==hint_index && test_cache_entry_internal(point,unit_index,i,cache)==1) return true;
  }
 }
 return visible;
}

bool firing_position_path_3d_available(const real_point3d *point,long unit_index,firing_position_ref reference,flight_evaluation_cache *cache)
{
 bool result=false;
 short i=0;
 unsigned short zone_index=(unsigned short)reference.block;
 if((zone_index&0x8000)==0 && !fp_ref_equals(reference,FP_REF_CURRENT))
 {
  firing_position_definition *definition=(firing_position_definition *)firing_position_ref_get_definition(reference);
  if(definition)
  {
   short signed_zone=(short)zone_index;
   if(signed_zone>=0 && signed_zone<(global_scenario)->zones.count)
   {
    zone_definition *zone=&((zone_definition *)global_scenario->zones.address)[zone_index];
    if(definition->area>=0 && definition->area<zone->areas.count)
    {
     area_definition *area=&((area_definition *)zone->areas.address)[definition->area];
     c_static_flags_no_init<64> tested; tested.clear();
     for(;i<area->flight_hints.count;i=(short)(i+1))
     {
       s_flight_area_reference *flight=&((s_flight_area_reference *)area->flight_hints.address)[i];
       if(!tested.test(flight->area))
       {
        tested.set(flight->area,true);
        if(flight_hint_any_point_visible(flight->area,point,unit_index,cache)) { result=true; break; }
       }
     }
    }
   }
  }
 }
 return result;
}

bool path_3d_available(const real_point3d *point,const c_ai_point3d *ai_point,long unit_index,firing_position_ref reference,flight_evaluation_cache *cache)
{
 bool result=false;
 real_point3d position;
 ai_point_get_position(ai_point,&position);
 bool blocked=!flight_test_points(point,&position,unit_index,-1);
 if (blocked && cache != 0 && !fp_ref_equals(reference, FP_REF_NONE) && !(reference.block & 0x8000) &&
     !fp_ref_equals(reference, FP_REF_CURRENT))
     blocked = !firing_position_path_3d_available(point, unit_index, reference, cache);
 if(!blocked) result=true;
 return result;
}
static __forceinline const real_point3d *path_flying_hint_point(const s_flight_points *hint, long point_index)
{
  return &(hint->data)[point_index];
}
// @rdata 0x82021694 path_flying_maximum_distance
static const float path_flying_maximum_distance = 3.402823466e38f;
// @address 0x56ee00 ?flight_hint_build_3d_path@@YA_NPBTreal_point3d@@JUfiring_position_ref@@PAUflight_evaluation_cache@@PAUpath_result@@@Z
bool flight_hint_build_3d_path(const real_point3d *start, long unit_index, firing_position_ref reference, flight_evaluation_cache *cache, path_result *result)
{
  float best_distance = path_flying_maximum_distance;
  short best_hint_index = -1;
  short best_structure_bsp_index = -1;
  short best_start_point_index = -1;
  short best_end_point_index = -1;
  bool success = false;
  scenario *scenario = global_scenario;
  unsigned short zone_index = (unsigned short)reference.block;
  if ((zone_index & 0x8000) == 0 && !fp_ref_equals(reference, FP_REF_CURRENT) &&
    (short)zone_index >= 0 && (short)zone_index < scenario->zones.count)
  {
    firing_position_definition *definition = (firing_position_definition *)firing_position_ref_get_definition(reference);
    if (definition)
    {
      zone_definition *zone = &((zone_definition *)scenario->zones.address)[zone_index];
      if (definition->area >= 0 && definition->area < zone->areas.count)
      {
        area_definition *area = &((area_definition *)zone->areas.address)[definition->area];
        c_static_flags<50> hints_visited;
        real_point3d destination;
        ai_point_get_position(&definition->point, &destination);
        hints_visited.clear();
        for (short i = 0; i < area->flight_hints.count; i++)
        {
          s_flight_area_reference *flight_ref = &((s_flight_area_reference *)area->flight_hints.address)[i];
          long hint_index = flight_ref->area;
          s_flight_hints *flight_hints = (global_scenario)->ai_user_hint_data.count > 0 ? ((s_flight_hints *)global_scenario->ai_user_hint_data.address) : 0;
          if (flight_hints && !hints_visited.test(flight_ref->area) && hint_index >= 0 && hint_index < flight_hints->count)
          {
            s_flight_points *hint = &flight_hints->entries[hint_index];
            short first_point_index = flight_ref->point_index;
            short last_point_index = first_point_index;
            short nearest_point_index = -1;
            short cache_index = -1;
            c_static_flags<10> hint_points;
            hints_visited.set(flight_ref->area, true);
            hint_points.clear();
            float nearest_distance = path_flying_maximum_distance;
            for (short j = i; j < area->flight_hints.count; j++)
            {
              s_flight_area_reference *temp_flight_ref = &((s_flight_area_reference *)area->flight_hints.address)[j];
              if (temp_flight_ref->area != hint_index)
                break;
              last_point_index = temp_flight_ref->point_index;
              hint_points.set(last_point_index, true);
              if (last_point_index >= 0 && last_point_index < hint->count)
              {
                float distance = distance_squared3d(&destination, path_flying_hint_point(hint, last_point_index));
                if (distance < nearest_distance)
                {
                  nearest_distance = distance;
                  nearest_point_index = last_point_index;
                }
              }
            }
            if (nearest_point_index != -1)
            {
              for (short k = 0; k < cache->count; k++)
              {
                s_flight_evaluation_entry *entry = &cache->entries[k];
                if (entry->area == hint_index)
                {
                  cache_index = k;
                  break;
                }
              }
              short direction = 1;
              short point_index = nearest_point_index;
              for (short pass = 0; pass < 2; pass++)
              {
                bool found = false;
                short closest_point_index = nearest_point_index;
                const real_point3d *closest_point = path_flying_hint_point(hint, closest_point_index);
                float closest_distance = distance3d(closest_point, &destination);
                while (true)
                {
                  short cache_entry_index = point_index + cache_index;
                  if (point_index < 0 || point_index >= hint->count)
                    break;
                  if (found && ((pass == 0 && point_index > last_point_index) || (pass == 1 && point_index < first_point_index)))
                    break;
                  if (hint_points.test(point_index))
                  {
                    closest_point_index = point_index;
                    closest_point = path_flying_hint_point(hint, closest_point_index);
                    closest_distance = distance3d(closest_point, &destination);
                  }
                  if (cache_entry_index >= 0 && cache_entry_index < cache->count)
                  {
                    s_flight_evaluation_entry *entry = &cache->entries[cache_entry_index];
                    if (entry->state != 2)
                    {
                      short current_point_index = point_index;
                      const real_point3d *current_point = path_flying_hint_point(hint, current_point_index);
                      float distance = distance3d(start, current_point) + distance3d(current_point, closest_point) + closest_distance;
                      if (distance < best_distance && test_cache_entry_internal(start, unit_index, cache_entry_index, cache) == 1)
                      {
                        best_distance = distance;
                        best_hint_index = (short)hint_index;
                        best_structure_bsp_index = entry->bsp;
                        best_start_point_index = current_point_index;
                        best_end_point_index = closest_point_index;
                        found = true;
                      }
                    }
                  }
                  point_index += direction;
                }
                direction = -direction;
                point_index = nearest_point_index + direction;
              }
            }
          }
        }
        if (best_hint_index != -1 && best_structure_bsp_index != -1 && best_start_point_index != -1 && best_end_point_index != -1)
        {
          s_flight_hints *flight_hints = (global_scenario)->ai_user_hint_data.count > 0 ? ((s_flight_hints *)global_scenario->ai_user_hint_data.address) : 0;
          s_flight_points *hint = &flight_hints->entries[best_hint_index];
          short point_index = best_start_point_index;
          short count = 0;
          while (count < 4)
          {
            short step = best_end_point_index > best_start_point_index ? 1 : -1;
            path_point *point = &result->points[count];
            const real_point3d *position = path_flying_hint_point(hint, point_index);
            point->flags = -1;
            point->hint.clear();
            point->point.set(position);
            point->sector.clear();
            point->type = 0;
            count++;
            if (point_index == best_end_point_index)
              break;
            point_index += step;
          }
          if (count < 4)
          {
            path_point *point = &result->points[count];
            point->flags = -1;
            point->hint.clear();
            point->point.set(&destination);
            point->sector.clear();
            point->type = 0;
            count++;
            result->final_point = true;
          }
          else
            result->final_point = false;
          result->count = (char)count;
          success = true;
        }
        else
          success = false;
      }
    }
  }
  return success;
}


// Inline expansion of object_get_ultimate_parent; the out-of-line copy lives in objects.obj.
static __forceinline long object_get_ultimate_parent_inline(long object_index)
{
  long ultimate_index;
  long parent_index = object_index;
  do
  {
    ultimate_index = parent_index;
    parent_index = ((const object_datum *)object_get_and_verify_type(parent_index, NONE))->parent_object_index;
  } while (parent_index != -1);
  return ultimate_index;
}

// @address 0x56f510 ?path_3d_build_path@@YA_NJPAVc_destination_orders@@PBTreal_point3d@@PBVc_path_destination@@Ufiring_position_ref@@PAUflight_evaluation_cache@@JPAUpath_result@@@Z
bool path_3d_build_path(long actor_index, c_destination_orders *destination_orders, const real_point3d *point,
    const c_path_destination *path_destination, firing_position_ref reference, flight_evaluation_cache *cache,
    long second_ignore_object_index, path_result *result)
{
  actor_datum *actor = datum_get<actor_datum>(tls_actor_data(), actor_index);
  long object_index = actor->unit_index;
  long ultimate_parent_index;
  bool append_facing_point = false;
  bool facing_point_available = false;
  bool path_available;
  c_ai_point3d ai_point;
  c_ai_point3d ai_point2;
  if (object_index != -1)
  {
    ultimate_parent_index = object_get_ultimate_parent_inline(object_index);
  }
  else
  {
    ultimate_parent_index = -1;
  }
  memset(result, 0, sizeof(*result));
  if (destination_orders->action.type == 3)
  {
    ai_point = path_destination->point;
    real_vector3d direction;
    if (destination_orders->action.direction.get(actor_index, _ai_direction_mode_vector, 0, &direction, 0, 0))
    {
      ai_point2 = ai_point;
      ai_point2.point.x = ai_point2.point.x + direction.i * 1.5f;
      ai_point2.point.y = ai_point2.point.y + direction.j * 1.5f;
      ai_point2.point.z = ai_point2.point.z + direction.k * 1.5f;
      if (destination_orders->flags.test(_destination_flag_4))
      {
        destination_orders->flags.set(_destination_flag_1, true);
        destination_orders->m_destination_facing = path_destination->vector;
      }
      else
      {
        append_facing_point = true;
      }
      facing_point_available = true;
    }
  }
  if (!facing_point_available)
  {
    ai_point = path_destination->point;
    append_facing_point = false;
  }
  {
    real_point3d position;
    ai_point_get_position(&ai_point, &position);
    result->start_point.set(point);
    result->destination.set(&ai_point, &g_sector_none, 0.0f);
    result->x3f = 0;
    if (flight_test_points(point, &position, ultimate_parent_index, second_ignore_object_index))
    {
      result->points[0].flags = -1;
      result->points[0].point = ai_point;
      result->points[0].sector.clear();
      result->points[0].hint.clear();
      result->count = 1;
      result->final_point = true;
      path_available = true;
    }
    else if (!fp_ref_equals(reference, FP_REF_NONE))
    {
      unsigned short zone_index = (unsigned short)reference.block;
      if ((zone_index & 0x8000) == 0 && !fp_ref_equals(reference, FP_REF_CURRENT))
      {
        flight_evaluation_cache local_cache;
        if (cache == 0)
        {
          local_cache.count = 0;
          cache = &local_cache;
          firing_position_definition *definition = (firing_position_definition *)firing_position_ref_get_definition(reference);
          flight_evaluation_cache_add_area(cache, zone_index, definition->area);
        }
        path_available = flight_hint_build_3d_path(point, ultimate_parent_index, reference, cache, result);
        if (path_available && result->final_point && result->count > 0)
        {
          result->points[result->count - 1].flags = -1;
          result->points[result->count - 1].point = ai_point;
          result->points[result->count - 1].sector.clear();
          result->points[result->count - 1].hint.clear();
        }
      }
      else
      {
        path_available = false;
      }
    }
    else
    {
      path_available = false;
    }
  }
  if (path_available && result->final_point && append_facing_point)
  {
    char count = result->count;
    short index = (short)(count - 1);
    if (count < 4)
    {
      result->points[count] = result->points[count - 1];
      result->count++;
    }
    else
    {
      result->final_point = false;
    }
    path_point *entry = &result->points[index];
    entry->flags = -1;
    entry->hint.clear();
    entry->point = ai_point2;
    entry->sector.clear();
    entry->type = 0;
  }
  result->success = path_available;
  if (!path_available)
  {
    short fail_count = (short)(actor->path_result_time + 1);
    actor->path_result_time = fail_count;
    if (fail_count > 32)
      actor->path_result_time = 0;
  }
  return result->success;
}
