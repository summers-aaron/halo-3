// blamlibxbox/material_effects.obj: from the records 0xf64d0 0xf6900 0x25e2d8 0x25e3b0 0x25e4c0
// @flags /TP /O2 /Oc
#include "math/real_math_types.h"
#include "tag_files/tag_types.h"
#include "game/game_globals.h"
#include "game/materials.h"
#include "cseries/cseries.h"
// @address 0x25e2d8 ?material_effect_find_material_internal@@YAPAUs_material_effect_material@@PBUs_material_effect@@W4e_material_effect_search@@Vc_global_material_type@@@Z
// @address 0x25e3b0 material_effect_resolve_effect_type
// @address 0xf6900 ?material_effect_find_material@@YAPAUs_material_effect_material@@JW4e_effect_type@@W4e_material_effect_search@@Vc_global_material_type@@@Z

#include "effects/material_effects_definition_types.h"
#include "effects/material_effect_search_types.h"
#include "effects/material_effect_types.h"
#include "effects/material_effects.h"
#include "game/materials_definitions.h"
#include "camera/observer_proximity.h"
#include "scenario/scenario.h"

#include "tag_files/tag_groups.h"

static __forceinline long material_effects_fallback_effect_type(long effect_type)
{
    long fallback_type = NONE;
    switch (effect_type)
    {
    case _effect_type_fizzle:
        fallback_type = _effect_type_impact;
        break;
    case _effect_type_overpenetrate:
        fallback_type = _effect_type_impact;
        break;
    case _effect_type_bounce:
        fallback_type = _effect_type_fizzle;
        break;
    case _effect_type_ricochet:
        fallback_type = _effect_type_bounce;
        break;
    case _effect_type_grinding:
        fallback_type = _effect_type_rolling;
        break;
    case _effect_type_biped_bodyfall:
        fallback_type = _effect_type_biped_jump_land;
        break;
    }
    return fallback_type;
}
static __forceinline bool material_effects_type_has_effects(long material_effects_index, e_material_effect_search search, long effect_type)
{
    bool result = false;
    s_material_effects_definition *definition = TAG_GET('foot', s_material_effects_definition, material_effects_index);
    if (effect_type < definition->material_effects.count)
    {
        s_material_effect *material_effect = TAG_BLOCK_GET_ELEMENT(
            &definition->material_effects, effect_type, s_material_effect);
        if (search == _material_effect_search_effects)
        {
            if (material_effect->first_materials.count > 0)
                result = true;
        }
        else
        {
            if (material_effect->second_materials.count > 0)
                result = true;
        }
    }
    return result;
}

s_material_effect_material *material_effect_find_material_internal(
    const s_material_effect *effect, e_material_effect_search search, c_global_material_type material_type)
{
    bool search_first = (_material_effect_search_first == search);
    s_material_effect_material *result = NULL;
    long count = search_first ? effect->first_materials.count : effect->second_materials.count;
    c_global_material_type current_material_type = material_type;
    while (!result && current_material_type.get_material(global_game_globals))
    {
        s_global_material_definition *global_material = current_material_type.get_material(global_game_globals);
        for (long material_index = 0; material_index < count; material_index++)
        {
            s_material_effect_material *material;
            if (search_first)
                material = TAG_BLOCK_GET_ELEMENT(&effect->first_materials, material_index, s_material_effect_material);
            else
                material = TAG_BLOCK_GET_ELEMENT(&effect->second_materials, material_index, s_material_effect_material);
            if (material->global_material_type == current_material_type)
            {
                result = material;
                break;
            }
        }
        current_material_type = global_material->parent_material_type;
    }
    return result;
}

long material_effect_resolve_effect_type(long material_effects_index, e_material_effect_search search, long effect_type)
{
    long result = NONE;
    for (long type = effect_type; type != NONE; type = material_effects_fallback_effect_type(type))
    {
        if (material_effects_type_has_effects(material_effects_index, search, type))
        {
            result = type;
            break;
        }
    }
    return result;
}

s_material_effect_material *material_effect_find_material(
    long material_effects_index, e_effect_type effect_type, e_material_effect_search search, c_global_material_type material_type)
{
 s_material_effects_definition *definition=TAG_GET('foot', s_material_effects_definition, material_effects_index);
 long resolved_effect_type=material_effect_resolve_effect_type(material_effects_index,search,effect_type);
 s_material_effect_material *result=NULL;
 if(resolved_effect_type!=NONE) {
  s_material_effect *effect=TAG_BLOCK_GET_ELEMENT(&definition->material_effects,resolved_effect_type,s_material_effect);
  result=material_effect_find_material_internal(effect,search,material_type);
 }
 return result;
}

// @address 0x25e4c0 ?global_material_material_effect_find_material_recursive@@YAPAUs_material_effect_material@@Vc_global_material_type@@W4e_effect_type@@W4e_material_effect_search@@0@Z
s_material_effect_material *global_material_material_effect_find_material_recursive(c_global_material_type material_type,
    e_effect_type effect_type, e_material_effect_search search, c_global_material_type target_material_type)
{
    c_global_material_type current_material_type = material_type;
    s_material_effect_material *result = NULL;
    while (!result && current_material_type.get_material(global_game_globals))
    {
        s_global_material_definition *material = current_material_type.get_material(global_game_globals);
        if (material->material_effects.index != NONE)
            result = material_effect_find_material(material->material_effects.index, effect_type, search, target_material_type);
        current_material_type = material->parent_material_type;
    }
    return result;
}


static __forceinline bool material_effects_effect_type_is_looping(e_effect_type effect_type)
{
    bool result = false;
    switch (effect_type)
    {
    case _effect_type_grinding:
    case _effect_type_rolling:
        result = true;
        break;
    }
    return result;
}

// tmp/probe (material audio): HALO3_AUDIO_TRACE=1 logs every material effect lookup ("game: mfx") and dumps the
// globals material table once ("game: matg").
extern "C" int port_audio_trace_enabled(void);
extern "C" unsigned port_audio_trace_time_ms(void);
extern "C" void port_audio_log(const char *text);
static int probe_mfx_enabled()
{
    static int enabled = -1;
    if (enabled < 0) {
        enabled = port_audio_trace_enabled();
        if (enabled) {
            for (long i = 0; i < global_game_globals->materials.count(); i++) {
                s_global_material_definition *m = TAG_BLOCK_GET_ELEMENT(&global_game_globals->materials, i, s_global_material_definition);
                char text[600];
                long n = 0;
                n += strlen(csnzprintf(text, sizeof(text), "game: matg %ld id=%08lx parent=%d flags=%x foot=%d snd=", i,
                    (unsigned long)m->material_id, (int)m->parent_material_type.m_index, (unsigned)m->flags, (int)(m->material_effects.index & 0xffff)));
                for (long s = 0; s < 17; s++)
                    n += strlen(csnzprintf(text + n, sizeof(text) - n, "%d%s", m->sounds[s].index == NONE ? -1 : (int)(m->sounds[s].index & 0xffff), s < 16 ? "," : ""));
                csnzprintf(text + n, sizeof(text) - n, " inherit=%lx\n", (unsigned long)m->sweetener_inheritance_flags);
                port_audio_log(text);
            }
        }
    }
    return enabled;
}
static void probe_mfx_trace(bool skip, const real_point3d *point, long foot, c_global_material_type material_type,
    c_global_material_type target, bool underwater, long effect_type, long sweetener_size, const long *out,
    const s_material_effect_material *em, const s_material_effect_material *sm)
{
    if (!probe_mfx_enabled())
        return;
    char text[400];
    csnzprintf(text, sizeof(text),
        "game: mfx %u type=%ld sw=%ld foot=%d mat=%d target=%d uw=%d skip=%d pos=%.2f/%.2f/%.2f em=%d/%d sm=%d/%d out=%d,%d,%d,%d,%d,%d,%d\n",
        port_audio_trace_time_ms(), effect_type, sweetener_size, foot == NONE ? -1 : (int)(foot & 0xffff), (int)material_type.m_index,
        (int)target.m_index, (int)underwater, (int)skip, point->x, point->y, point->z,
        em ? (int)em->global_material_type.m_index : -9, em ? (int)em->sweetener_mode : -9,
        sm ? (int)sm->global_material_type.m_index : -9, sm ? (int)sm->sweetener_mode : -9,
        (int)(out[0] == NONE ? -1 : out[0] & 0xffff), (int)(out[1] == NONE ? -1 : out[1] & 0xffff), (int)(out[2] == NONE ? -1 : out[2] & 0xffff),
        (int)(out[3] == NONE ? -1 : out[3] & 0xffff), (int)(out[4] == NONE ? -1 : out[4] & 0xffff), (int)(out[5] == NONE ? -1 : out[5] & 0xffff),
        (int)(out[6] == NONE ? -1 : out[6] & 0xffff));
    port_audio_log(text);
}

// @address 0xf64d0 ?material_effect_find_effects@@YAX_NPBTreal_point3d@@JVc_global_material_type@@20W4e_effect_type@@W4e_sweetener_size@@PAJ@Z
void material_effect_find_effects(bool skip_proximity_check, const real_point3d *point, long material_effects_index,
    c_global_material_type material_type, c_global_material_type target_material_type, bool underwater, e_effect_type effect_type,
    e_sweetener_size sweetener_size, long *out_values)
{
    e_effect_type resolved_effect_type;
    if (effect_type == _effect_type_grinding)
        resolved_effect_type = _effect_type_rolling;
    else
        resolved_effect_type = effect_type;
    for (long i = 0; i < 7; i++)
        out_values[i] = NONE;
    if (!skip_proximity_check)
    {
        float radius = 10.0f;
        switch (effect_type)
        {
        case _effect_type_collision_large:
            radius = 40.0f;
            break;
        case _effect_type_grinding:
        case _effect_type_rolling:
            radius = 15.0f;
            break;
        case _effect_type_impact:
        case _effect_type_overpenetrate:
            radius = 70.0f;
            break;
        case _effect_type_obstacle_smash:
        case _effect_type_fizzle:
        case _effect_type_attach:
        case _effect_type_bounce:
        case _effect_type_ricochet:
        case _effect_type_collision_damage:
        case _effect_type_melee:
            radius = 20.0f;
            break;
        case _effect_type_walk:
        case _effect_type_run:
        case _effect_type_biped_sliding:
        case _effect_type_biped_shuffle:
        case _effect_type_biped_jump:
        case _effect_type_biped_jump_land:
        case _effect_type_biped_sprint:
        case _effect_type_biped_jump_land_hard:
        case _effect_type_biped_bodyfall:
        case _effect_type_biped_unused_2:
        case _effect_type_particle_impact:
            radius = 10.0f;
        }
        if (!active_user_close_to_point_not_deterministic(point, radius, _adjust_for_fov_scale_none))
            return;
    }
    s_material_effect_material *effect_material = NULL;
    s_material_effect_material *sweetener_material = NULL;
    if (underwater)
        target_material_type = global_material_resolve_underwater_material_proxy(target_material_type, point);
    s_global_material_definition *material_definition = target_material_type.get_material(global_game_globals);
    if (material_effects_index != NONE)
    {
        effect_material = material_effect_find_material(material_effects_index, resolved_effect_type, _material_effect_search_first, target_material_type);
        sweetener_material = material_effect_find_material(material_effects_index, effect_type, _material_effect_search_second, target_material_type);
    }
    if (!effect_material)
        effect_material = global_material_material_effect_find_material_recursive(
            material_type, resolved_effect_type, _material_effect_search_first, target_material_type);
    if (!sweetener_material)
        sweetener_material = global_material_material_effect_find_material_recursive(
            material_type, effect_type, _material_effect_search_second, target_material_type);
    if (effect_material)
    {
        if (effect_material->sweetener_mode == _material_effect_sweetener_mode_enabled && sweetener_size != NONE)
        {
            if (material_effects_effect_type_is_looping(effect_type))
            {
                switch (effect_type)
                {
                case _effect_type_grinding:
                case _effect_type_rolling:
                    out_values[1] = material_definition->effect_looping_sound.index;
                    break;
                }
            }
            else
                out_values[1] = material_definition->effect_sounds[sweetener_size].index;
        }
        long effect_index = effect_material->effect.index;
        long second_effect_index = effect_material->second_effect.index;
        out_values[2] = second_effect_index;
        out_values[0] = effect_index;
    }
    if (sweetener_material)
    {
        if (sweetener_material->sweetener_mode == _material_effect_sweetener_mode_enabled && sweetener_size != NONE)
        {
            if (material_effects_effect_type_is_looping(effect_type))
            {
                switch (effect_type)
                {
                case _effect_type_grinding:
                    out_values[4] = material_definition->grinding_looping_sound.index;
                    break;
                case _effect_type_rolling:
                    out_values[4] = material_definition->rolling_looping_sound.index;
                    break;
                }
            }
            else
                out_values[4] = material_definition->sweetener_sounds[sweetener_size].index;
        }
        long sweetener_effect_index = sweetener_material->effect.index;
        long sweetener_second_effect_index = sweetener_material->second_effect.index;
        out_values[5] = sweetener_second_effect_index;
        out_values[3] = sweetener_effect_index;
    }
    if (material_definition && sweetener_size != NONE)
        out_values[6] = material_definition->default_sounds[sweetener_size].index;
    probe_mfx_trace(skip_proximity_check, point, material_effects_index, material_type, target_material_type, underwater,
        (long)effect_type, (long)sweetener_size, out_values, effect_material, sweetener_material);
}
