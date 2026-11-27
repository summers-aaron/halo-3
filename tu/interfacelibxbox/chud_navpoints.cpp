// interfacelibxbox/chud_navpoints.obj: from the records 0x132cd0 0x133240 0x133368 0x2902a0 0x2f2908 0x491a20 0x49de08 0x132640 0x132de8
// @flags /TP /O2
#include "camera/observer_globals_types.h"
// @address 0x11df8cc ?g_observer_globals@@3PAUs_player_globals@@A
#include "text/font_cache_types.h"
#include "game/player_mapping_types.h"
#include "units/unit_types.h"
#include "interface/chud/chud_globals_definitions.h"
#include "cseries/cseries_unicode.h"
#include "text/font_id_types.h"
#include "interface/chud/chud_user_data_types.h"
#include "scenario/scenario.h"
// @address 0x2902a0 ??_Gc_draw_string@@UAAPAXI@Z
#include "math/real_math_types.h"
#include <string.h>
#include "game/game_globals_storage.h"
#include "math/real_math.h"
#include "objects/object_iterator.h"
// @address 0x133240 ?submit_navpoint@c_chud_navpoint_manager@@QAAXPAUs_chud_navpoint@@@Z
#include "memory/data.h"
#include "game/game_time_types.h"
#include "game/game_time_inline.h"
#include "game/game.h"
#include "objects/objects.h"
#include "text/unicode.h"
#include "units/units.h"
#include "game/aim_assist.h"
#include "game/game_engine_util.h"
#include "networking/online/voice_xenon.h"
#include "text/draw_string.h"
#include "interface/chud/chud.h"
#include "interface/chud/chud_draw.h"
#include "game/game_engine_infection.h"
#include "interface/chud/chud_navpoints.h"
#include "game/player_in_game_iterator.h"
#include "interface/chud/utf32_types.h"
#include "game/game_engine_area_types.h"
#include "rasterizer/rasterizer_text_glyph_cache_types.h"
#include "game/players.h"
#include "memory/tls_globals.h"
#include "game/game_engine_scoring_globals.h"
#include "scenario/scenario_tag.h"
#include "interface/chud/chud_navpoint_types.h"
#include "scenario/scenario_cutscene_flag.h"
#include "game/game_engine_default.h"
#include "game/game_engine_area_set_types.h"
#include "game/game_engine_infection_globals.h"
#include "objects/object_header.h"
#include "text/font_cache_lock.h"
#include "game/game_engine_list.h"
#include <ppcintrinsics.h>
#include "game/game_engine_infection_types.h"
#include "text/font_character_types.h"
#include "interface/chud/chud_draw_string.h"
#include "interface/chud/chud_navpoints_runtime.h"
struct biped_datum;

#include "game/player_mapping_inlines.h"

void c_chud_navpoint_manager::submit_scripted_navpoints(long user_index)
{
    long player_index = player_index_from_user_index_render(user_index);
    if (player_index != -1) {
        for (long scripted_navpoint_index = 0; scripted_navpoint_index < scripted_navpoint_count; scripted_navpoint_index++) {
            long cutscene_flag_index = scripted_navpoint_cutscene_flag_indices[scripted_navpoint_index];
            if (cutscene_flag_index != -1) {
                const scenario_cutscene_flag *cutscene_flag =
                    &global_scenario->cutscene_flags[cutscene_flag_index];
                const real_point3d *position = &cutscene_flag->position;

                s_chud_navpoint navpoint;
                const real_vector3d up = { 0.0f, 0.0f, 1.0f };
                point_from_line3d(position, &up, scripted_navpoint_offsets[scripted_navpoint_index], &navpoint.position);
                navpoint.object_index = scripted_navpoint_index;
                navpoint.type = 2;
                navpoint.icon = -1;
                navpoint.state = 0;
                navpoint.flags = 193;
                navpoint.scale = 1.0f;
                navpoint.name[0] = 0;

                submit_navpoint(&navpoint);
            }
        }
    }
}

// Stores key in the cache: refreshes the entry with the same id when there
// is one, else takes the first free slot, else evicts the oldestIndex entry.
void c_chud_navpoint_manager::submit_navpoint(s_chud_navpoint *navpoint)
{
  c_chud_navpoint_manager *manager = this;
  game_time_globals_definition *timeGlobals;
  int oldestIndex;
  int oldestTime;
  int entryIndex;
  c_chud_navpoint_manager::s_managed_navpoint *entry;

  for (entryIndex = 0; entryIndex < 20; entryIndex++) {
    if (manager->navpoints[entryIndex].valid && manager->navpoints[entryIndex].id == navpoint->object_index) {
      if (entryIndex != -1) {
        memcpy(&manager->navpoints[entryIndex], navpoint, sizeof(*navpoint));
        manager->navpoints[entryIndex].time = tls_game_time_globals()->time;
        return;
      }
      break;
    }
  }
  oldestIndex = -1;
  timeGlobals = tls_game_time_globals();
  oldestTime = timeGlobals->time;
  int newIndex = -1;
  for (entryIndex = 0; entryIndex < 20; entryIndex++) {
    if (!manager->navpoints[entryIndex].valid) {
      newIndex = entryIndex;
      break;
    }
    if (manager->navpoints[entryIndex].time < oldestTime) {
      oldestTime = manager->navpoints[entryIndex].time;
      oldestIndex = entryIndex;
    }
  }
  if (newIndex == -1) {
    newIndex = oldestIndex;
    if (newIndex == manager->selected_index) {
      manager->selected_player_index = -1;
      manager->selected_index = -1;
    }
  }
  entry = &manager->navpoints[newIndex];
  memcpy(entry, navpoint, sizeof(*navpoint));
  entry->state = -1;
  entry->valid = true;
  entry->count = 0;
  entry->time = timeGlobals->time;
}


long c_chud_navpoint_manager::determine_distanced_navpoint(long user_index)
{
    real_vector3d cameraForward = observer_get_camera(user_index)->forward;
    real_point3d cameraPosition = observer_get_camera(user_index)->position;
    float maximumAngle = 0.017453292f * 10.0f;
    long closestIndex = -1;
    float closestAngle = maximumAngle;
    for (long index = 0; index < 20; index++) {
        c_chud_navpoint_manager::s_managed_navpoint *entry = &navpoints[index];
        if (entry->valid && ((long)entry->flags & 128) != 0) {
            real_vector3d direction;
            vector_from_points3d(&cameraPosition, &entry->position, &direction);
            normalize3d(&direction);
            float angle = angle_between_vectors3d(&direction, &cameraForward);
            if (angle < closestAngle) {
                closestAngle = angle;
                closestIndex = index;
            }
        }
    }
    return closestIndex;
}

// @address 0x2f2908 ?submit_nav_points@c_infection_engine@@UBAXJJ@Z
// The direct debug body is 82e0d8e0; the automatic campaign-navpoints pairing is a false lead.


extern "C" {

}

void c_infection_engine::submit_nav_points(long user_index, long target_player_index) const
{
    long player_index = player_index_from_user_index_render(user_index);
    c_game_engine::submit_nav_points(user_index, target_player_index);
    if (!player_is_zombie(player_index))
    {
        c_area *selected_area = ((game_engine_infection_engine_globals *)tls_game_engine_globals())->infection.safe_havens.find_selected_area();
        if (selected_area)
        {
            s_chud_navpoint navpoint;
            navpoint.flags = 1;
            navpoint.object_index = 0;
            navpoint.type = 2;
            navpoint.icon = 4;
            navpoint.state = 0;
            navpoint.scale = 1.0f;
            navpoint.name[0] = 0;
            selected_area->get_center(&navpoint.position);
            if (user_index >= 0 && user_index < 4)
                chud_persistent_user_data()[user_index].m_navpoint_manager.submit_navpoint(&navpoint);
        }
    }
}

// Removes the navpoint with this id from every user's list (the lists are kept packed: the last entry moves into the hole).


// @address 0x491a20 chud_navpoints_delete_navpoint
extern "C" void chud_navpoints_delete_navpoint(short user_index, short navpoint_id)
{
  for (long list_index = 0; list_index < 4; list_index++) {
    c_chud_navpoint_manager *list = &chud_persistent_user_data()[list_index].m_navpoint_manager;
    if (user_index != -1) {
      if (navpoint_id != -1) {
        for (long index = 0; index < list->scripted_navpoint_count; index++) {
          if ((short)list->scripted_navpoint_cutscene_flag_indices[index] == navpoint_id) {
            if (index + 1 < list->scripted_navpoint_count) {
              list->scripted_navpoint_cutscene_flag_indices[index] = list->scripted_navpoint_cutscene_flag_indices[list->scripted_navpoint_count - 1];
              list->scripted_navpoint_offsets[index] = list->scripted_navpoint_offsets[list->scripted_navpoint_count - 1];
            }
            list->scripted_navpoint_count = list->scripted_navpoint_count - 1;
          }
        }
      }
    }
  }
}
// 0x49de08 chud_build_text_geometry: measures a string into a widget geometry.
extern "C" {
}
// @address 0x7bbad8 g_rasterizer_text_glyph_cache
typedef char navpoints_draw_string_size[(sizeof(void *) != 4 || ((sizeof(c_draw_string) == 0x100))) ? 1 : -1];

// @address 0x49de08 chud_compute_text_geometry
extern "C" bool chud_compute_text_geometry(chud_navpoints_widget_geometry *geometry, const wchar_t *string, long font_index,
    unsigned char centered, float scale, const real_rectangle2d *clip)
{
    geometry->character_count = 0;
    bool result = false;
    c_chud_draw_string draw;
    c_font_cache cache;
    rectangle2d bounds;
    bounds.x0 = 0;
    bounds.y0 = 0;
    bounds.x1 = 1000;
    bounds.y1 = 1000;
    long font = (font_index > 0 ? font_index : 0) < 10 ? (font_index > 0 ? font_index : 0) : 10;
    draw.m_font_id = (e_font_id)font;
    draw.m_font_header = font_get_header(font);
    draw.m_style = _text_style_none;
    draw.m_justification = centered ? _text_justification_center : _text_justification_left;
    draw.m_geometry = geometry;
    draw.set_bounds(&bounds);
    draw.m_scale = scale;
    if (clip != 0) {
        draw.m_clip = *clip;
    }
    if (draw.precache_wchar_string_for_font(&cache, string, (e_font_id)font_index)) {
        rectangle2d text_bounds;
        draw.compute_wchar_string_bounds(&cache, string, &bounds, &text_bounds);
        draw.c_draw_string::draw(&cache, string);
        geometry->bounds.x0 = (float)(double)bounds.x0;
        geometry->bounds.x1 = (float)(double)bounds.x1;
        geometry->bounds.y0 = (float)(double)bounds.y0;
        geometry->bounds.y1 = (float)(double)bounds.y1;
        long invalid_texture;
        const long *active_font;
        const s_hardware_character_cache_globals *glyph_globals = &g_rasterizer_text_glyph_cache;
        if (glyph_globals->initialized)
            active_font = &glyph_globals->handle;
        else {
            invalid_texture = -1;
            active_font = &invalid_texture;
        }
        long texture_handle = *active_font;
        geometry->render_type = 3;
        geometry->texture_handle = texture_handle;
        result = true;
    }

    return result;
}

extern "C" {
// @address 0x119a558 g_chud_navpoint_companion_search_timer
long g_chud_navpoint_companion_search_timer;
}

// @address 0x132de8 chud_navpoint_manager_submit_player_navpoints
extern "C" void chud_navpoint_manager_submit_player_navpoints(c_chud_navpoint_manager *manager, long user_index, long target_player_index)
{
    long player_index = player_index_from_user_index_render(user_index);
    if (tls_game_globals()->options.game_mode == 1 && !game_campaign_is_multiplayer() && manager->campaign_navpoints_enabled) {
        long object_index = manager->campaign_object_index;
        if (object_index == -1 || !object_try_and_get_and_verify_type(object_index, 0x1003) || !players_is_object_campaign_companion(object_index))
            manager->campaign_object_index = -1;
        if (manager->campaign_object_index == -1) {
            if (!g_chud_navpoint_companion_search_timer) {
                c_object_iterator<biped_datum> objects;
                objects.begin(1, 1);
                while (objects.next()) {
                    long candidate = objects.get_index();
                    if (players_is_object_campaign_companion(candidate)) {
                        manager->campaign_object_index = candidate;
                        break;
                    }
                }
                g_chud_navpoint_companion_search_timer = tls_game_time_globals()->ticks_per_second;
            } else {
                --g_chud_navpoint_companion_search_timer;
            }
        }
    } else {
        manager->campaign_object_index = -1;
    }
    if (player_index != -1) {
        c_player_in_game_iterator players;
        players.begin();
        long team = (*datum_get<player_datum>(tls_player_data(), player_index)).configuration.host.team_index;
        long waypoint_index = 0;
        while (players.next()) {
            long other_player_index = players.m_iterator.m_iterator.index;
            if (other_player_index != player_index) {
                player_datum *player = players.m_iterator.m_datum;
                if (team == player->configuration.host.team_index) {
                    bool valid = false;
                    s_chud_navpoint navpoint;
                    navpoint.scale = 1.0f;
                    navpoint.object_index = other_player_index;
                    navpoint.type = 0;
                    navpoint.icon = -1;
                    navpoint.state = 0;
                    navpoint.name[0] = 0;
                    s_game_engine_player_waypoint *waypoint = &chud_navpoint_globals()->waypoints[waypoint_index];
                    c_static_wchar_string<4> name;
                    navpoint.flags = (other_player_index == target_player_index) ? 32 : 0;
                    ustrnzcpy_inline(name.get_buffer(), player->configuration.client.appearance.service_tag, 4);
                    ustrnzcpy(navpoint.name, name.get_string(), 4);
                    if (player->unit_index != -1) {
                        unit_get_head_position(player->unit_index, &navpoint.position);
                        const real_vector3d up = { 0.0f, 0.0f, 1.0f };
            point_from_line3d(&navpoint.position, &up, 0.075f, &navpoint.position);
                        valid = true;
                        if (voice_lifecycle_in_game() && voice_player_is_talking((unsigned short)other_player_index)) {
                            navpoint.flags |= 16;
                        } else {
                            switch ((signed char)waypoint->state) {
                            case 1: navpoint.flags |= 4; break;
                            case 2: navpoint.flags |= 16; break;
                            case 3: navpoint.flags |= 8; break;
                            }
                        }
                    } else {
                        if (waypoint->active && waypoint->ticks) {
                            navpoint.position = waypoint->position;
                            valid = true;
                        } else if (player->dead_unit_index != -1 &&
                            game_time_get() - ((unit_datum *)object_get(player->dead_unit_index))->unit.last_active_time < game_seconds_to_ticks_integer(3))
                            {
                            unit_get_head_position(player->dead_unit_index, &navpoint.position);
                            const real_vector3d up = { 0.0f, 0.0f, 1.0f };
                            point_from_line3d(&navpoint.position, &up, 0.075f, &navpoint.position);
                            valid = true;
                        }
                        navpoint.icon = 1;
                        navpoint.state = 3;
                    }
                    if (valid) manager->submit_navpoint(&navpoint);
                }
            }
            ++waypoint_index;
        }
        if (manager->campaign_object_index != -1) {
            s_chud_navpoint navpoint;
            navpoint.scale = 1.0f;
            navpoint.object_index = manager->campaign_object_index;
            navpoint.type = 0;
            navpoint.icon = -1;
            navpoint.state = 0;
            navpoint.flags = 0;
            navpoint.name[0] = 0;
            unit_get_head_position(manager->campaign_object_index, &navpoint.position);
            const real_vector3d up = { 0.0f, 0.0f, 1.0f };
                        point_from_line3d(&navpoint.position, &up, 0.075f, &navpoint.position);
            manager->submit_navpoint(&navpoint);
        }
    }
}

// ---- 0x132640 c_chud_navpoint_manager::update ----

extern "C" {
// @address 0x1328 k_two_string_format
}
// @address 0x21308 k_minus_sign_string
// @rdata 0x82021308..0x8202130c
const wchar_t k_minus_sign_string[] = L"-";
// @rdata-end


// @address 0x132640 ?update@c_chud_navpoint_manager@@QAAXJ@Z
void c_chud_navpoint_manager::update(long user_index)
{
    long game_time = game_time_get();
    long target = -1;
    long player_index = player_index_from_user_index_render(user_index);
    if (player_index != -1) {
        long object_index = (*datum_get<player_datum>(tls_player_data(), player_index)).unit_index;
        if (object_index != -1) {
            unit_datum *object = (unit_datum *)object_get(object_index);
            if (object->unit.targeting.target_object_index != -1) {
                long candidate = object->unit.targeting.target_player_index;
                if (candidate != -1) {
                    target = candidate;
                }
            }
        }
    }
    if (target == -1)
        target = player_aim_assist_get_player_under_crosshair(user_index);
    long engine_index = tls_game_engine_globals()->game_engine_index;
    int invalid_engine_index = 0;
    if (engine_index < 1)
        invalid_engine_index = 1;
    else if (engine_index > 10)
        invalid_engine_index = 1;
    if (!invalid_engine_index && game_engines[engine_index] &&
        player_index_from_user_index_render(user_index) != -1)
        current_game_engine()->submit_nav_points(user_index, target);
    submit_scripted_navpoints(user_index);
    if (tls_game_globals()->options.game_mode == 1)
        chud_navpoint_manager_submit_player_navpoints(this, user_index, target);
    long distanced = determine_distanced_navpoint(user_index);
    long first = -1;
    if (target != -1) {
        long scan_index = 0;
        c_chud_navpoint_manager::s_managed_navpoint *scan = navpoints;
        do {
            if (scan->valid && ((long)scan->flags & 0x20) != 0) {
                first = scan_index;
                break;
            }
            scan_index++;
            scan++;
        } while (scan_index < 20);
    }
    float fade_out_time = 0.25f;
    float label_fade_in_fast_time = 0.25f;
    float label_fade_in_slow_time = 0.125f;
    float label_fade_out_fast_time = 0.5f;
    float label_fade_out_slow_time = 0.25f;
    for (long i = 0; i < 20; i++) {
        c_chud_navpoint_manager::s_managed_navpoint *entry = &navpoints[i];
        const float &entry_z = entry->position.z;
        if (entry->valid) {
            bool fresh = game_time - entry->time <= 0;
            if (!fresh) {
                float tick = game_tick_length();
                entry->progress = entry->progress - tick / fade_out_time;
                if (entry->progress <= 0.0f) {
                    entry->valid = false;
                    if (i == selected_index) {
                        selected_player_index = -1;
                        selected_index = -1;
                    }
                }
            } else {
                entry->progress = 1.0f;
            }
        }
        if (entry->valid) {
            if (target != -1 && i == first && selected_index == -1) {
                unsigned short sort = entry->text[0];
                player_datum *player = datum_get<player_datum>(tls_player_data(), target);
                const wchar_t *alt = sort == 0 ? L"" : k_minus_sign_string;
                usnzprintf(text, 17, k_two_string_format, player->configuration.host.name, alt);
                float width = chud_draw_get_string_width(6, text);
                text_width = width;
                if (width > 0.0f) {
                    text_progress = 0.0f;
                    selected_player_index = target;
                    selected_index = i;
                }
            }
            if (i == selected_index) {
                if (first == i) {
                    if (entry->text[0] == 0)
                        text_progress += game_tick_length() / label_fade_in_slow_time;
                    else
                        text_progress += game_tick_length() / label_fade_in_fast_time;
                } else if (first == -1) {
                    text_progress -= game_tick_length() / label_fade_out_fast_time;
                } else {
                    text_progress -= game_tick_length() / label_fade_out_slow_time;
                }
                text_progress = PIN(text_progress, 0.0f, 1.0f);
                if (text_progress == 0.0f) {
                    selected_player_index = -1;
                    selected_index = -1;
                }
            }
        }
        if (entry->valid) {
            if (i == distanced && distance_index == -1.0f) {
                distance_progress = 0.0f;
                distance_index = (float)i;
            }
            if ((float)i == distance_index) {
                if (distanced == i) {
                    if (entry->text[0] == 0)
                        distance_progress += game_tick_length() / label_fade_in_slow_time;
                    else
                        distance_progress += game_tick_length() / label_fade_in_fast_time;
                } else if (distanced == -1) {
                    distance_progress -= game_tick_length() / label_fade_out_fast_time;
                } else {
                    distance_progress -= game_tick_length() / label_fade_out_slow_time;
                }
                distance_progress = PIN(distance_progress, 0.0f, 1.0f);
                {
                    const real_point3d *camera = &g_observer_globals->blocks[user_index].result.position;
                    float dy = camera->y - entry->position.y;
                    float dx = camera->x - entry->position.x;
                    float dz = camera->z - entry_z;
                    double dist_feet = (double)(__fsqrts(dx * dx + dz * dz + dy * dy) * 3.048f);
                    usnzprintf(distance_text, 10, L"%.1fm", dist_feet);
                    float width = chud_draw_get_string_width(6, distance_text);
                    distance_width = width;
                    if (distance_progress == 0.0f)
                        distance_index = -1.0f;
                }
            }
        }
        if (entry->valid) {
            const chud_table_row &limits = chud_globals->rows[chud_get_user_skin_index(user_index)];
            float dist = distance3d(&g_observer_globals->blocks[user_index].result.position, &entry->position);
            float min_scale = limits.navpoint_minimum_scale;
            float max_scale = limits.navpoint_maximum_scale;
            if (dist > 0.0001f)
                entry->scale = PIN(20.0f / dist, min_scale, max_scale);
            else
                entry->scale = max_scale;
            long flags = (long)entry->flags;
            long icon;
            if ((flags & 0x10) != 0)
                icon = 4;
            else if ((flags & 4) != 0)
                icon = 2;
            else if ((flags & 8) != 0)
                icon = 3;
            else if ((flags & 2) != 0)
                icon = 1;
            else
                icon = -1;
            if (icon != -1) {
                entry->state = icon;
                entry->count = tls_game_time_globals()->ticks_per_second / 2;
            }
            else if (entry->count > 0 && --entry->count == 0)
                entry->state = -1;
            if (entry->state == 4 && entry->icon_index == -1)
                entry->icon_index = 0;
        }
    }
}
