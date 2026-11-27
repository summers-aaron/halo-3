// tmp/probe/extra (GAME-CHANGES F48): rendering functions retail has out of line that the source only declares.
// prebuilt.tsv lists 0x16fbf8 and 0x16fd28 as "between library functions" (LTCG placed them among d3d9 state.obj's
// routines), so the decomp skipped them as XDK code; 0x22d440 / 0x230060 are decals.obj members the source names by
// address. Decompiled from the retail listing (recomp oracle sub_8216FBF8, sub_8216FD28, sub_8222D440, sub_82230060).
#include <xtl.h>
#include "math/real_math_types.h"
#include "sky_atm/real_linear_rgb_color_types.h"
#include "rasterizer/rasterizer_main_runtime_globals.h"
#include "math/real_math_constant_storage.h"
#include "rasterizer/rasterizer.h"
#include "render/render_objects_globals.h"
#include "render/views/player_view_types.h"
#include "scenario/scenario.h"
#include "scenario/scenario_tag.h"
#include "scenario/scenario_definitions.h"
#include "tag_files/tag_groups.h"
#include "objects/object_definitions.h"
#include "models/model_types.h"
#include "models/render_model_definitions.h"
#include "effects/decal_types.h"
#include "effects/decal_system_types.h"
#include "effects/decal_definition_types.h"

// ---- 0x16fbf8 atmosphere_query_sky_sun_parameters (atmosphere.cpp's caller: the sky's sun for the fog) ----
// The sun of a sky is the last lightgen light of the sky object's render model: color = rgb * intensity * 0.2,
// direction as authored. Without a sky (index out of range, or a missing tag on the way) the color is white and the
// direction straight down (k_negative_identity4x3.up).
struct s_render_model_lightgen_light
{
    real_vector3d direction;
    real_linear_rgb_color color;
    float intensity;
};
typedef char render_model_lightgen_light_size[(sizeof(s_render_model_lightgen_light) == 28) ? 1 : -1];

extern "C" void atmosphere_query_sky_sun_parameters(long sky_index, real_linear_rgb_color *color, real_vector3d *direction)
{
    const float k_sun_intensity_scale = 0.2f; // 0x8203385c
    color->red = color->green = color->blue = 1.0f;
    *direction = k_negative_identity4x3.up;
    if (sky_index < 0 || sky_index >= global_scenario->skies.count)
        return;
    const scenario_sky_reference *sky = (const scenario_sky_reference *)global_scenario->skies.address + sky_index;
    if (sky->object.index == -1)
        return;
    const s_object_definition *object = (const s_object_definition *)tag_get('scen', sky->object.index);
    if (object->model.index == -1)
        return;
    const s_model_definition *model = (const s_model_definition *)tag_get('hlmt', object->model.index);
    if (model->render_model_index == -1)
        return;
    const render_model_definition *render_model = (const render_model_definition *)tag_get('mode', model->render_model_index);
    if (render_model->lightgen_lights.count <= 0)
        return;
    const s_render_model_lightgen_light *sun =
        (const s_render_model_lightgen_light *)render_model->lightgen_lights.address + (render_model->lightgen_lights.count - 1);
    color->red = sun->color.red * sun->intensity * k_sun_intensity_scale;
    color->green = sun->color.green * sun->intensity * k_sun_intensity_scale;
    color->blue = sun->color.blue * sun->intensity * k_sun_intensity_scale;
    *direction = sun->direction;
}

// ---- 0x16fd28 rasterizer_set_alpha_test_enabled == rasterizer_set_mesh_part_state (one function, two source names) ----
// Alpha-tested geometry (foliage, decorators, mesh parts with an alpha-test material): enabling forces opaque blending
// (unless the decorator albedo pass owns the blend state) and sets alpha test GREATEREQUAL 128; disabling clears it.
// The byte at 0x828c446f selects alpha-to-mask instead; it is .bss that nothing writes (a debug switch), so false.
// The second parameter of the source declarations is not read (r4 is overwritten before any use).
static bool g_rasterizer_alpha_to_mask_enabled = false; // 0x828c446f

static void rasterizer_alpha_test_set(unsigned char enable)
{
    D3DDevice *device = g_rasterizer_device; // ?g_device@c_rasterizer (one datum, F19 alias)
    if (enable) {
        if (!g_decorator_albedo_active && g_rasterizer_alpha_blend_mode != 0) {
            g_rasterizer_alpha_blend_mode = 0; // c_rasterizer::g_current_alpha_blend_mode: _alpha_blend_opaque
            D3DDevice_SetRenderState_AlphaBlendEnable(device, 0);
        }
        if (g_rasterizer_alpha_to_mask_enabled) {
            D3DDevice_SetRenderState_AlphaToMaskEnable(device, 1);
            D3DDevice_SetRenderState_AlphaToMaskOffsets(device, 0xaa);
        } else {
            D3DDevice_SetRenderState_AlphaTestEnable(device, 1);
            D3DDevice_SetRenderState_AlphaFunc(device, D3DCMP_GREATEREQUAL);
            D3DDevice_SetRenderState_AlphaRef(device, 128);
        }
    } else if (g_rasterizer_alpha_to_mask_enabled) {
        D3DDevice_SetRenderState_AlphaToMaskEnable(device, 0);
    } else {
        D3DDevice_SetRenderState_AlphaTestEnable(device, 0);
    }
}

extern "C" void rasterizer_set_alpha_test_enabled(unsigned char enable, int) { rasterizer_alpha_test_set(enable); }
extern "C" void rasterizer_set_mesh_part_state(unsigned char enable, int) { rasterizer_alpha_test_set(enable); }

// ---- 0x22d440 c_decal::get_fade (decals_alias_22d440): the remaining life over the fade time, clamped to [0, 1] ----
extern "C" float decals_alias_22d440(const c_decal *decal)
{
    const float fade = (1.0f / decal->m_fade_time) * (decal->m_lifespan - decal->m_age);
    if (fade > 0.0f && fade >= 1.0f)
        return 1.0f;
    if (fade <= 0.0f)
        return 0.0f;
    return fade;
}

// ---- 0x230060 decal system distance fade (decals_alias_230060) ----
// The camera's distance to the system's center, scaled by g_decal_distance_inverse, faded out between the
// definition's near and far distances: (far - d) / (far - near), clamped to [0, 1].
extern "C" float decals_alias_230060(const c_decal_system *system)
{
    const real_point3d *camera = &c_player_view::get_current()->get_rasterizer_camera()->position;
    const real_point3d *center = &system->m_projection.position;
    const float dx = camera->x - center->x, dy = camera->y - center->y, dz = camera->z - center->z;
    const float distance = sqrtf(dx * dx + dy * dy + dz * dz) * g_decal_distance_inverse;
    const s_decals_system_definition *definition = s_decals_system_definition::get(system->m_definition_index);
    const float fade = (definition->far_distance - distance) / (definition->far_distance - definition->near_distance);
    if (fade > 0.0f && fade >= 1.0f)
        return 1.0f;
    if (fade <= 0.0f)
        return 0.0f;
    return fade;
}
