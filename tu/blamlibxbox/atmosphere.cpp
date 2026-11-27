// blamlibxbox/atmosphere.obj: from the records 0x16c730 0x16c7d8 0x16ca48 0x16fbf8 0x180618 0x23d218 0x23d268
// @compiler xdk6130
// @flags /TP /O2 /Oc
#include "objects/object_reference_types.h"
#include "tag_files/tag_index.h"
#include "objects/object_types.h"
#include "memory/data.h"
#include <xtl.h>
#include "math/matrix_math.h"
#include "math/real_math.h"
#include "math/real_math_constant_storage.h"
#include "tag_files/tag_groups.h"
#include "effects/effects.h"
#include "sky_atm/atmosphere.h"
#include "sky_atm/atmosphere_render_interface.h"
#include "sky_atm/real_linear_rgb_color_types.h"
#include "scenario/scenario.h"
#include "scenario/scenario_tag.h"
#include "effects/effect_vector_types.h"
#include "structures/cluster_bit_vectors.h"
#include "rasterizer/rasterizer_main_globals.h"
#include "sky_atm/atmosphere_fog_interface.h"
#include "structures/structure_bsp_tag.h"
#include "scenario/scenario_structure_bsp_reference.h"
#include "game/player_mapping_types.h"
#include "effects/effect_render_globals.h"
#include "rasterizer/rasterizer.h"
#include "math/real_math_trigonometry.h"
#include "sky_atm/weighted_atmosphere_parameters.h"
#include "sky_atm/sky_atm_parameters_types.h"
#include "sky_atm/atmosphere_external.h"
#include "game/game_globals_storage.h"
#include "memory/tls_globals.h"
#include "models/model_types.h"
#include "models/render_model_definitions.h"
#include "objects/object_definitions.h"
#include "scenario/scenario_definitions.h"

// @address 0x7ea858 ?m_default_parameters@c_atmosphere_fog_interface@@2Us_weighted_atmosphere_parameters@@A
s_weighted_atmosphere_parameters c_atmosphere_fog_interface::m_default_parameters;

enum
{
    k_atmosphere_vertex_constant_register = 234,
    k_atmosphere_disabled = -3
};

struct s_atmosphere_constant_tag
{
    UINT64 mask;
    DWORD index;
};

typedef char atmosphere_setting_size[(sizeof(c_atmosphere_setting) == 0xa4) ? 1 : -1];
typedef char atmosphere_weather_index_offset[(offsetof(c_atmosphere_setting, weather_effect.index) == 152) ? 1 : -1];

typedef real_linear_rgb_color c_color_RGB;

class c_color_XYZ
{
public:
    float x;
    float y;
    float z;
};

class c_color_xyY
{
public:
    float x;
    float y;
    float luminance;
};

struct s_atmosphere_sky_index
{
    signed char index;
    unsigned char pad;
};

__forceinline s_atmosphere_constant_tag &atmosphere_vertex_constant_tag()
{
    static s_atmosphere_constant_tag tag = { D3DTag_ShaderConstantMask(k_atmosphere_vertex_constant_register, 1), D3DTag_Index(D3DTAG_VERTEXSHADERCONSTANTS) };
    return tag;
}

// the atmosphere whose constants the device holds
long atmosphere_constants_index = -1;

// @address 0x16c730 ?populate_atmosphere_parameters@c_atmosphere_fog_interface@@SAXUs_cluster_reference@@PAUs_weighted_atmosphere_parameters@@@Z
void c_atmosphere_fog_interface::populate_atmosphere_parameters(s_cluster_reference cluster, s_weighted_atmosphere_parameters *parameters)
{
    if (tls_effect_render_globals()->camera_relative)
    {
        datum_index sky_index = global_scenario->sky_parameters.index;
        if (sky_index != NONE)
        {
            const s_sky_atm_parameters_definition *definition = TAG_GET('skya', const s_sky_atm_parameters_definition, sky_index);
            if (definition)
            {
                for (long setting_index = 0; setting_index < definition->atmosphere_settings.count; setting_index++)
                {
                    const c_atmosphere_setting *setting = TAG_BLOCK_GET_ELEMENT(&definition->atmosphere_settings, setting_index, const c_atmosphere_setting);
                    render_objects_accumulate_atmosphere(cluster, setting, parameters, setting->weight);
                }
            }
        }
    }
}

static __forceinline void atmosphere_convert_RGB_to_XYZ(const c_color_RGB &rgb, c_color_XYZ &xyz)
{
    xyz.x = 0.412424f * rgb.red + 0.357579f * rgb.green + 0.180464f * rgb.blue;
    xyz.y = 0.212656f * rgb.red + 0.715158f * rgb.green + 0.0721856f * rgb.blue;
    xyz.z = 0.0193324f * rgb.red + 0.119193f * rgb.green + 0.950444f * rgb.blue;
}

static __forceinline void atmosphere_convert_XYZ_to_xyY(const c_color_XYZ &xyz, c_color_xyY &xyY)
{
    float sum = xyz.x + xyz.y + xyz.z;
    float x;
    float y;
    if (sum != 0.0f)
    {
        x = xyz.x / sum;
        y = xyz.y / sum;
    }
    else
    {
        x = 0.0f;
        y = 0.0f;
    }
    xyY.x = x;
    xyY.luminance = xyz.y;
    xyY.y = y;
}

static __forceinline void atmosphere_convert_RGB_to_xyY(const c_color_RGB &rgb, c_color_xyY &xyY)
{
    c_color_XYZ xyz;
    atmosphere_convert_RGB_to_XYZ(rgb, xyz);
    atmosphere_convert_XYZ_to_xyY(xyz, xyY);
}

static __forceinline void atmosphere_convert_xyY_to_XYZ(const c_color_xyY &xyY, float intensity, c_color_XYZ &xyz)
{
    float x;
    float y;
    float z;
    if (xyY.y != 0.0f)
    {
        float one_minus_x = 1.0f - xyY.x;
        float ratio = intensity / xyY.y;
        float z_fraction = one_minus_x - xyY.y;
        x = ratio * xyY.x;
        y = intensity;
        z = z_fraction * ratio;
    }
    else
    {
        x = 0.0f;
        y = 0.0f;
        z = 0.0f;
    }
    xyz.x = x;
    xyz.y = y;
    xyz.z = z;
}

static __forceinline void atmosphere_convert_XYZ_to_RGB(const c_color_XYZ &xyz, c_color_RGB &rgb)
{
    rgb.red = 3.240479f * xyz.x - 1.53715f * xyz.y - 0.498535f * xyz.z;
    rgb.green = -0.969256f * xyz.x + 1.875991f * xyz.y + 0.041556f * xyz.z;
    rgb.blue = 0.055648f * xyz.x - 0.204043f * xyz.y + 1.057311f * xyz.z;
}

static __forceinline void atmosphere_convert_xyY_to_RGB(const c_color_xyY &xyY, float intensity, c_color_RGB &rgb)
{
    c_color_XYZ xyz;
    atmosphere_convert_xyY_to_XYZ(xyY, intensity, xyz);
    atmosphere_convert_XYZ_to_RGB(xyz, rgb);
}

// @address 0x16c7d8 atmosphere_get_sun_parameters
extern "C" void atmosphere_get_sun_parameters(s_atmosphere_sky_index sky_index, const c_atmosphere_setting *setting,
    // match: 0x16c7d8: __restrict keeps xyz in fprs across the color stores (removal adds an lfs xyz reload and recolors the fmuls chain)
    real_linear_rgb_color *__restrict color, real_vector3d *__restrict direction)
{
    if (TEST_BIT(setting->flags, 1))
    {
        c_color_RGB rgb;
        rgb.red = setting->color.red;
        rgb.green = setting->color.green;
        rgb.blue = setting->color.blue;
        c_color_xyY xyY;
        atmosphere_convert_RGB_to_xyY(rgb, xyY);
        float intensity = setting->intensity;
        c_color_RGB converted;
        atmosphere_convert_xyY_to_RGB(xyY, intensity, converted);
        color->red = converted.red;
        color->green = converted.green;
        color->blue = converted.blue;
        float pitch_sine = sine((setting->pitch * 0.0055555557f) * 3.1415927f);
        float yaw_cosine = cosine((setting->yaw * 0.0055555557f) * 3.1415927f);
        float next_pitch_angle = setting->pitch * 0.0055555557f;
        direction->i = pitch_sine * yaw_cosine;
        direction->j = sine(next_pitch_angle * 3.1415927f) * (float)sin((double)((setting->yaw * 0.0055555557f) * 3.1415927f));
        direction->k = cosine((setting->pitch * 0.0055555557f) * 3.1415927f);
    }
    else
    {
        if (sky_index.index != -1)
            atmosphere_query_sky_sun_parameters(
                (long)((scenario_structure_bsp_reference *)global_scenario->structure_bsps.address)[sky_index.index].default_sky_index,
                color, direction);
    }
}

// @address 0x16ca48 ?set_constant@c_atmosphere_fog_interface@@SAXPBUs_weighted_atmosphere_parameters@@_N1@Z
void c_atmosphere_fog_interface::set_constant(const s_weighted_atmosphere_parameters *parameters, bool enabled, bool vertex_shader)
{
    if (parameters && tls_effect_render_globals()->camera_relative)
    {
        enabled = parameters && parameters->valid;
        if (!enabled)
            disable_atmosphere();
        else
        {
            real_vector3d sun_direction = parameters->sun_direction;
            normalize3d_with_default(&sun_direction, &k_negative_identity4x3.up);
            real_vector4d constants[7];
            float mie_scale = 1.0f - parameters->mie_anisotropy * parameters->mie_anisotropy;
            constants[0].x = sun_direction.i;
            constants[0].w = parameters->sun_intensity;
            constants[0].y = sun_direction.j;
            float red_scattering = parameters->rayleigh_scattering.red + parameters->mie_scattering.red;
            constants[0].z = sun_direction.k;
            float green_scattering;
            float blue_scattering;
            if (!(red_scattering <= k_real_epsilon) &&
                !((green_scattering = parameters->mie_scattering.green + parameters->rayleigh_scattering.green) <= k_real_epsilon) &&
                !((blue_scattering = parameters->mie_scattering.blue + parameters->rayleigh_scattering.blue) <= k_real_epsilon))
            {
                constants[1].x = parameters->sun_color.red / red_scattering;
                constants[1].y = parameters->sun_color.green / green_scattering;
                constants[1].z = parameters->sun_color.blue / blue_scattering;
            }
            else
            {
                constants[1].x = 0.0f;
                constants[1].y = 0.0f;
                constants[1].z = 0.0f;
            }
            constants[1].w = parameters->distance_bias < 0.1f ? 0.1f : parameters->distance_bias;
            constants[2].x = parameters->rayleigh_scattering.red * 1.442695f;
            constants[2].y = parameters->rayleigh_scattering.green * 1.442695f;
            constants[2].z = parameters->rayleigh_scattering.blue * 1.442695f;
            constants[2].w = parameters->mie_anisotropy + 1.0f;
            constants[3].x = parameters->mie_scattering.red * 1.442695f;
            constants[3].y = parameters->mie_scattering.green * 1.442695f;
            constants[3].z = parameters->mie_scattering.blue * 1.442695f;
            constants[3].w = parameters->rayleigh_height;
            constants[4].x = parameters->fog_color.red;
            constants[4].y = parameters->fog_color.green;
            constants[4].z = parameters->fog_color.blue;
            constants[4].w = parameters->fog_intensity;
            constants[5].x = parameters->mie_color.red * mie_scale;
            constants[5].y = parameters->mie_color.green * mie_scale;
            constants[5].z = parameters->mie_color.blue * mie_scale;
            constants[5].w = parameters->mie_height;
            constants[6].x = constants[6].y = constants[6].z = constants[6].w = parameters->mie_anisotropy * 2.0f;
            if (vertex_shader)
            {
                c_rasterizer::get_device()->SetVertexShaderConstantF(233, &constants[0].x, 6);
                c_rasterizer::get_device()->SetVertexShaderConstantF(15, &constants[6].x, 1);
            }
            else
            {
                c_rasterizer::get_device()->SetPixelShaderConstantF(233, &constants[0].x, 6);
                c_rasterizer::get_device()->SetPixelShaderConstantF(239, &constants[6].x, 1);
            }
        }
    }
    else
        disable_atmosphere();
}

// @address 0x16fbf8 atmosphere_query_sky_sun_parameters
extern "C" void atmosphere_query_sky_sun_parameters(long index, real_linear_rgb_color *color, real_vector3d *direction)
{
    color->red = color->green = color->blue = 1.0f;
    *direction = *global_down3d;

    const s_tag_block *skies = &global_scenario->skies;
    if (!VALID_INDEX(index, skies->count))
        return;

    // match: 0x16fc60: Byte-scaled sky lookup puts the block base in the first add operand and the scaled index in the second.
    const long &sky_offset = index * static_cast<long>(sizeof(scenario_sky_reference));
    const char *const &sky_data = static_cast<const char *>(skies->address);
    const scenario_sky_reference *sky = reinterpret_cast<const scenario_sky_reference *>(sky_data + sky_offset);
    const datum_index sky_object_index = sky->object.index;
    if (sky_object_index == NONE)
        return;

    const s_object_definition *object = TAG_GET('scen', const s_object_definition, sky_object_index);
    if (object->model.index == NONE)
        return;

    const s_model_definition *model = TAG_GET('hlmt', const s_model_definition, object->model.index);
    if (model->render_model_index == NONE)
        return;

    const render_model_definition *render_model = TAG_GET('mode', const render_model_definition, model->render_model_index);
    const s_tag_block *lights = &render_model->lightgen_lights;
    if (lights->count <= 0)
        return;

    const s_sky_gen_light *light = TAG_BLOCK_GET_ELEMENT(lights, lights->count - 1, const s_sky_gen_light);
    color->red = (light->color.red * light->intensity) * 0.2f;
    color->green = (light->color.green * light->intensity) * 0.2f;
    color->blue = (light->color.blue * light->intensity) * 0.2f;
    *direction = light->direction;
}

// @address 0x180618 ?disable_atmosphere@c_atmosphere_fog_interface@@SAXXZ
void c_atmosphere_fog_interface::disable_atmosphere()
{
    D3DDevice *device = c_rasterizer::get_device();
    if (device != NULL && atmosphere_constants_index != k_atmosphere_disabled)
    {
        s_atmosphere_constant_tag &tag = atmosphere_vertex_constant_tag();
        D3DVECTOR4 *constant = &device->m_Constants.VertexShaderF[k_atmosphere_vertex_constant_register];
        constant->x = 0.0f;
        constant->y = 0.0f;
        constant->z = 0.0f;
        atmosphere_constants_index = k_atmosphere_disabled;
        constant->w = -1.0f;
        c_rasterizer::get_device()->m_Pending.m_Mask[tag.index] |= tag.mask;
    }
}

// The March release map folds these empty game-system callbacks into the shared blr at 0x5bec98.
// @address 0x5bec98 ?initialize@c_atmosphere_fog_interface@@SAXXZ
void c_atmosphere_fog_interface::initialize()
{
}

// @address 0x5bec98 ?dispose@c_atmosphere_fog_interface@@SAXXZ
void c_atmosphere_fog_interface::dispose()
{
}

// @address 0x5bec98 ?dispose_from_old_map@c_atmosphere_fog_interface@@SAXXZ
void c_atmosphere_fog_interface::dispose_from_old_map()
{
}

// @address 0x5bec98 ?dispose_from_old_structure_bsp@c_atmosphere_fog_interface@@SAXK@Z
void c_atmosphere_fog_interface::dispose_from_old_structure_bsp(unsigned long)
{
}

// @address 0x23d218 ?initialize_for_new_structure_bsp@c_atmosphere_fog_interface@@SAXK@Z
void c_atmosphere_fog_interface::initialize_for_new_structure_bsp(unsigned long)
{
    s_game_cluster_bit_vectors old_pvs;
    memset(&old_pvs, 0, sizeof(old_pvs));
    change_pvs(&old_pvs, (const s_game_cluster_bit_vectors *)&tls_game_globals()->cluster_pvs_local, true);
}

// @address 0x23d268 ?change_pvs@c_atmosphere_fog_interface@@SAXPBUs_game_cluster_bit_vectors@@0_N@Z
void c_atmosphere_fog_interface::change_pvs(const s_game_cluster_bit_vectors *, const s_game_cluster_bit_vectors *new_pvs, bool force)
{
    if (force != 1)
        return;
    scenario *sky_scenario = global_scenario;
    if (!sky_scenario)
        return;
    if (tls_player_mapping_globals()->local_player_count != 1)
        return;
    datum_index sky_index = sky_scenario->sky_parameters.index;
    if (sky_index == NONE)
        return;
    c_static_flags<128> used;
    used.clear();
    const s_sky_atm_parameters_definition *sky = TAG_GET('skya', s_sky_atm_parameters_definition, sky_index);
    long active_mask = g_active_structure_bsp_mask;
    for (long bsp_index = 0; bsp_index < sky_scenario->structure_bsps.count; bsp_index++)
    {
        if (((1 << bsp_index) & active_mask) == 0)
            continue;
        const structure_bsp *bsp = TAG_GET('sbsp', structure_bsp,
            ((const scenario_structure_bsp_reference *)global_scenario->structure_bsps.address)[bsp_index].structure_bsp.index);
        for (short cluster_index = 0; cluster_index < bsp->clusters.count; cluster_index++)
        {
            if (!new_pvs->bit_vectors[bsp_index].test(cluster_index))
                continue;
            const structure_cluster *cluster = TAG_BLOCK_GET_ELEMENT(&bsp->clusters, cluster_index, structure_cluster);
            signed char palette = cluster->atmosphere_index;
            if (palette == -1)
                continue;
            const structure_atmosphere_palette_entry *palette_entry =
                TAG_BLOCK_GET_ELEMENT(&bsp->atmosphere_palette, palette, structure_atmosphere_palette_entry);
            if (!VALID_INDEX(palette_entry->atmosphere_setting_index, sky->atmosphere_settings.count))
                continue;
            used.set(palette_entry->atmosphere_setting_index, true);
            const c_atmosphere_setting *weather =
                TAG_BLOCK_GET_ELEMENT(&sky->atmosphere_settings, palette_entry->atmosphere_setting_index, c_atmosphere_setting);
            c_tag_index definition_index(weather->weather_effect.index);
            if (definition_index == -1)
                continue;
            if (tls_effect_render_globals()->cached_indices[palette_entry->atmosphere_setting_index] == -1)
            {
                long new_effect_index = effect_new_weather(definition_index);
                tls_effect_render_globals()->cached_indices[palette_entry->atmosphere_setting_index] = new_effect_index;
            }
            long effect_index = tls_effect_render_globals()->cached_indices[palette_entry->atmosphere_setting_index];
            if (effect_index != -1)
            {
                union { s_cluster_reference reference; unsigned short combined; } cluster_ref;
                cluster_ref.reference.bsp_index = (signed char)bsp_index;
                cluster_ref.reference.cluster_index = (unsigned char)cluster_index;
                effect_refresh_location(effect_index, &cluster_ref.reference);
            }
        }
    }
    for (long palette_index = 0; palette_index < sky->atmosphere_settings.count; palette_index++)
    {
        if (!used.test(palette_index))
        {
            long effect_index = tls_effect_render_globals()->cached_indices[palette_index];
            if (effect_index != -1)
            {
                effect_handle_deleted_atmosphere(effect_index);
                tls_effect_render_globals()->cached_indices[palette_index] = -1;
            }
        }
    }
}
