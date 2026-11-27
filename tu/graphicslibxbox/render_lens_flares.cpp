// graphicslibxbox/render_lens_flares.obj: from the records 0x12b7d0 0x12b898 0x1472a8 0x147430 0x178c38 0x1806b8 0x1835e0 0x45aa78 0x45ace0 0x45ad00 0x45b2d8 0x45b500 0x45bcc0 0x45be20 0x45bf50 0x45bfb0 0x45c0b0
#include "rasterizer/rasterizer_main_globals.h"
#include <xtl.h>
#include "memory/tls_globals.h"
#include "tag_files/tag_types.h"
#include "tag_files/tag_groups.h"
#include "game/game_globals_storage.h"
#include "math/real_math.h"
#include "objects/objects.h"
#include "math/periodic_functions.h"
#include "rasterizer/dx9/rasterizer_dx9_dynamic_geometry.h"
#include "memory/crc.h"
#include "render/render_lens_flares.h"
#include "game/game_time_types.h"
#include "math/rotate_vector2d.h"
#include "rasterizer/occlusion_query_types.h"
#include "render/shader_reference_types.h"
#include "rasterizer/rasterizer_state_globals.h"
#include "rasterizer/rasterizer_tiling_types.h"
#include "rasterizer/rasterizer_tiling_globals.h"
#include "cache/cache_files.h"
#include "rasterizer/rasterizer_occlusion_queries.h"
#include "render/render_visibility_collection.h"
#include "render/render_visibility_collection_globals.h"
#include "rasterizer/lens_flare_entry_types.h"
#include "rasterizer/lens_flare_globals.h"
#include "rasterizer/rasterizer_device_globals.h"
#include "render/render_player_view_globals.h"
#include "render/render_water_globals.h"
#include "bitmaps/bitmap_data.h"
#include "bitmaps/bitmap_group_definitions.h"
#include "math/function_definitions.h"
#include "effects/lens_flare_attachment_types.h"
#include "rasterizer/rasterizer_vertex_screen_types.h"
#include "rasterizer/simple_hash_table.h"
#include "rasterizer/rasterizer_vertex_definitions.h"
#include "render_methods/render_method_definitions.h"
#include "rasterizer/rasterizer.h"
#include "math/matrix_math.h"
#include "render/render_camera_types.h"
#include "render/lens_flare_types.h"
#include "visibility/visibility_render_objects.h"
#include <string.h>
#include "effects/lens_flare_definitions.h"
#include "render/views/player_view_types.h"
#include "render/render_view.h"
#include "effects/decal_types.h"

typedef c_simple_hash_table<s_lens_flare, 256, 3307, 3, 337> c_lens_flare_hash_table;
struct s_render_visibility_collection_globals;
struct rasterizer_vertex_world;
struct s_lens_flare_render_parameters
{
    float alpha;
    float rotation;
    float size;
    real_vector3d incident_direction;
    real_vector3d camera_forward;
    real_vector3d camera_up;
    real_vector3d camera_left;
    real_vector4d color;
};
// @flags /TP /O2 /Oc
// @address 0x12b7d0 ?valid_real_vector3d_axes2@@YA_NPBTreal_vector3d@@0@Z
// @address 0x12b898 ?valid_real_normal3d@@YA_NPBTreal_vector3d@@@Z
// @address 0x147430 ?iterator_next@?$c_simple_hash_table@Us_lens_flare@@$0BAA@$0MOL@$02$0BFB@@@QAAJJ@Z
// @address 0x1472a8 lens_flares_update
// @address 0x178c38 lens_flares_submit_occlusions
// @address 0x1806b8 ?find_entry@?$c_simple_hash_table@Us_lens_flare@@$0BAA@$0MOL@$02$0BFB@@@QAAJJJJ@Z
// @address 0x1835e0 ?lens_flares_render@@YAXJ@Z
// @address 0x45aa78 lens_flares_create_internal
// @address 0x45b2d8 lens_flares_evaluate_corona_rotation_function
// @address 0x45bf50 ?lens_flares_reset_rasterizer_parameters@@YAXXZ
// @address 0x45bfb0 ?lens_flare_get_bitmap_aspect_ratio@@YAXJJPATreal_vector2d@@@Z
// @address 0x45c0b0 ?add_entry@?$c_simple_hash_table@Us_lens_flare@@$0BAA@$0MOL@$02$0BFB@@@QAAJJJJ@Z


static __forceinline long calculate_lens_flare_hash(long key0, long key1, long key2)
{
  return abs(key0 * 3307 + key1 * 3 + key2 * 337) % 256;
}

// match: 0x12b7d0: inline lets valid_real_vector3d_axes2 inline the body; out of line it calls and saves r29-r31/fr30-fr31
#if defined(_XBOX) && !defined(__clang__)
inline bool valid_real_normal3d(const real_vector3d *vector)
#else
bool valid_real_normal3d(const real_vector3d *vector)
#endif
{
  return valid_realcmp(magnitude_squared3d(vector), 1.0f);
}

bool valid_real_vector3d_axes2(const real_vector3d *vector, const real_vector3d *vector2)
{
  return valid_real_normal3d(vector) && valid_real_normal3d(vector2) && valid_realcmp(dot_product3d(vector, vector2), 0.0f);
}

template<>
inline long c_lens_flare_hash_table::iterator_next(long index)
{
    if (entry_count == 0)
        index = -1;
    else
    {
        bool searching = true;
        while (searching)
        {
            if (index == 255)
            {
                searching = false;
                index = -1;
            }
            else
            {
                index++;
                searching = original_hash_index[index] == -1;
            }
        }
    }
    return index;
}

template<>
inline long c_lens_flare_hash_table::find_entry(long key0, long key1, long key2)
{
  long original_hash = calculate_lens_flare_hash(key0, key1, key2);
  long probe = original_hash;
  long original_count = hash_entry_count[original_hash];
  long result = -1;
  long iterations = 0;
  while (original_count > 0 && result == -1) {
    if (original_hash_index[probe] == original_hash) {
      c_lens_flare_hash_table::s_entry *entry = &data[probe];
      --original_count;
      if (entry->key0 == key0 && entry->key1 == key1 && entry->key2 == key2)
        result = probe;
    }
    probe = (probe + 1) % 256;
    if (iterations++ > 256) break;
  }
  return result;
}



static __forceinline bitmap_data *render_lens_flares_bitmap_get(const s_tag_block *block, long index)
{
    return &((bitmap_data *)block->address)[index];
}

extern "C" float lens_flares_evaluate_corona_rotation_function(int rotation_function, float rotation_scale,
    const real_point3d *position, const real_vector3d *direction)
{
    float cosine = 1.0f;
    float sine = 0.0f;
    float rotation = 0.0f;
    const render_projection *projection = &c_player_view::get_current()->render_projection_state;
    const render_camera *camera = &c_player_view::get_current()->render_camera_state;
    real_vector3d incident_direction;
    real_vector3d projected_direction;
    switch (rotation_function)
    {
    case 1:
        cross_product3d(direction, &projection->view_to_world.forward, &projected_direction);
        cross_product3d(&projected_direction, direction, &projected_direction);
        incident_direction = camera->forward;
        sine = dot_product3d(&incident_direction, &projected_direction);
        cosine = -dot_product3d(&incident_direction, direction);
        break;
    case 2:
        negate_vector3d(direction, &incident_direction);
        sine = dot_product3d(&incident_direction, &projection->view_to_world.forward);
        cosine = -dot_product3d(&incident_direction, &projection->view_to_world.up);
        break;
    case 3:
        cross_product3d(direction, &projection->view_to_world.forward, &projected_direction);
        cross_product3d(&projected_direction, direction, &projected_direction);
        vector_from_points3d(&camera->position, position, &incident_direction);
        sine = dot_product3d(&incident_direction, &projected_direction);
        cosine = -dot_product3d(&incident_direction, direction);
        break;
    case 4:
        vector_from_points3d(&camera->position, position, &incident_direction);
        sine = dot_product3d(&incident_direction, &projection->view_to_world.forward);
        cosine = -dot_product3d(&incident_direction, &projection->view_to_world.up);
        break;
    case 0:
        break;
    default:
        return 0;
    }
    if (rotation_function != 0 && sine != 0.0f)
        rotation = (float)atan2((double)sine, (double)cosine) * rotation_scale * 0.31830987f;
    return rotation;
}

void lens_flare_get_bitmap_aspect_ratio(long bitmap_group_index, long bitmap_index, real_vector2d *aspect_ratio)
{
    set_real_vector2d(aspect_ratio, 1.0f, 1.0f);
    if (bitmap_group_index != -1)
    {
        bitmap_group *group = TAG_GET('bitm', bitmap_group, bitmap_group_index);
        if (bitmap_index >= 0 && bitmap_index < group->bitmaps.count)
        {
            bitmap_data *bitmap = render_lens_flares_bitmap_get(&group->bitmaps, bitmap_index);
            if ((float)bitmap->height != 0.0f && bitmap->width > bitmap->height)
                aspect_ratio->j = (float)(bitmap->width / bitmap->height);
            else if ((float)bitmap->width != 0.0f && bitmap->height > bitmap->width)
                aspect_ratio->i = (float)(bitmap->height / bitmap->width);
        }
    }
}

template<>
inline long c_lens_flare_hash_table::add_entry(long key0, long key1, long key2)
{
  long result = -1;
  if (entry_count < 256) {
    long original = calculate_hash(key0, key1, key2);
    result = find_new_hash_entry(original);
    if (result != -1) {
      c_lens_flare_hash_table::s_entry &entry = data[result];
      entry.key0 = key0;
      entry.key1 = key1;
      entry.key2 = key2;
      original_hash_index[result] = static_cast<short>(original);
      ++hash_entry_count[original];
    }
    ++entry_count;
  }
  return result;
}

extern "C" {
// @address 0x836ec8 g_lens_flare_globals
#if !defined(_XBOX) || defined(__clang__)
lens_flare_globals g_lens_flare_globals;
#endif

// @address 0x8c46ad g_rasterizer_in_ui_shell
// @address 0x8c4874 g_frame_texture_flags
unsigned long g_frame_texture_flags;
// @address 0x8c48b8 g_lens_flare_previous_frame_texture_flags
int g_lens_flare_previous_frame_texture_flags;
}


extern "C" void lens_flares_update(void)
{
    int hash_index;
    game_globals_storage *game_globals;
    int flare_index;
    int current_frame;
    unsigned char use_game_mode;
    rasterizer_main_memory_globals *memory = rasterizer_memory_get_globals();
    memory->GPU_frontend_remaining = 98304.0f;
    memory->current_offset = 0;
    memory->maximum_offset = 0;
    memory->allocation_bytes = 0;
    memset(&render_visibility_collection_globals, 0, sizeof(render_visibility_collection_globals));
    current_frame = g_lens_flare_globals.frame;
    g_lens_flare_previous_frame_texture_flags = g_frame_texture_flags;
    g_frame_texture_flags = 0;
    flare_index = ((c_lens_flare_hash_table *)&g_lens_flare_globals)->iterator_next(-1);
    if (!((flare_index == -1))) {
        while ((flare_index != -1)) {
            if (current_frame - g_lens_flare_globals.entries[flare_index].frame > 1) {
                g_lens_flare_globals.buckets[g_lens_flare_globals.links[flare_index]]--;
                g_lens_flare_globals.links[flare_index] = -1;
                g_lens_flare_globals.entry_count--;
            }
            flare_index = ((c_lens_flare_hash_table *)&g_lens_flare_globals)->iterator_next(flare_index);
        }
        current_frame = g_lens_flare_globals.frame;
    }
    g_lens_flare_globals.frame = (current_frame + 1);
    game_globals = tls_game_globals();
    if (game_globals != 0 && (game_globals->initializing || game_globals->map_active))
        use_game_mode = 1;
    else
        use_game_mode = 0;
    if (use_game_mode != 0) {
        g_rasterizer_in_ui_shell = game_globals->options.game_mode == 3;
        c_rasterizer::update_tiling_configuration();
        return;
    }
    g_rasterizer_in_ui_shell = 0;
    c_rasterizer::update_tiling_configuration();
    return;
}


void lens_flares_reset_rasterizer_parameters(void)
{
    if (c_rasterizer::g_current_alpha_blend_mode != c_rasterizer::_alpha_blend_opaque) {
        c_rasterizer::g_current_alpha_blend_mode = c_rasterizer::_alpha_blend_opaque;
        D3DDevice_SetRenderState_AlphaBlendEnable(c_rasterizer::get_device(), 0);
    }
    D3DDevice_SetRenderState_CullMode(c_rasterizer::get_device(), 2);
    ::c_rasterizer::set_z_buffer_mode((c_rasterizer::e_z_buffer_mode)0);
}


static __forceinline float render_lens_flares_max(float a, float b)
{
    return a > b ? a : b;
}

static __forceinline real_rgb_color *render_lens_flares_scale_rgb_color(const real_rgb_color *a, float scale, real_rgb_color *out)
{
    out->red = scale * a->red;
    out->green = scale * a->green;
    out->blue = scale * a->blue;
    return out;
}

static __forceinline unsigned long render_lens_flares_rgb_color_to_pixel32(const real_rgb_color *color)
{
    unsigned long red = (long)(color->red * 255.0f);
    unsigned long green = (long)(color->green * 255.0f);
    unsigned long blue = (long)(color->blue * 255.0f);
    return ((((0xff00 | red) << 8) | green) << 8) | blue;
}

extern "C" long lens_flares_create_internal(long definition_index, e_lens_flare_attachment_types type, long key1, unsigned short key2,
    const real_point3d *position, const real_vector3d *direction, const real_rgb_color *color, float scale, float intensity, float rotation,
    bool flag, long unknown)
{
    c_lens_flare_hash_table *table = (c_lens_flare_hash_table *)&g_lens_flare_globals;
    lens_flare_globals *globals = &g_lens_flare_globals;
    long index = table->find_entry(type, key1, key2);
    if (index == -1) {
        index = table->add_entry(type, key1, key2);
        if (index != -1) {
            lens_flare_entry *added = &globals->entries[index];
            added->tag_index = definition_index;
            added->age = 1;
            added->flags = 0;
        }
    }
    if (index != -1) {
        lens_flare_entry *flare = &globals->entries[index];
        real_rgb_color actual_color = *color;
        s_lens_flare_definition *definition = (s_lens_flare_definition *)tag_get('lens', flare->tag_index);
        float max_color = actual_color.red > render_lens_flares_max(actual_color.green, actual_color.blue)
            ? actual_color.red
            : render_lens_flares_max(actual_color.green, actual_color.blue);
        if (max_color > 1.0f) {
            render_lens_flares_scale_rgb_color(&actual_color, 1.0f / max_color, &actual_color);
            intensity *= max_color;
        }
        if (type == _lens_flare_attachment_type_default && !(definition->occlusion_flags & 128))
            render_lens_flares_scale_rgb_color(&actual_color, 1.0f - scale, &actual_color);
        flare->color = render_lens_flares_rgb_color_to_pixel32(&actual_color);
        flare->position = *position;
        flare->direction = *direction;
        flare->scale = scale;
        flare->intensity = intensity;
        flare->rotation = rotation;
        flare->frame = globals->frame;
        flare->element_type = type;
        flare->element_key1 = key1;
        flare->element_key2 = unknown;
        if (flag)
            flare->flags |= 1;
        else
            flare->flags &= ~1;
    }
    return index;
}

// @address 0x45ace0 ?lens_flare_render_occlusion@@YAXJ@Z
void lens_flare_render_occlusion(long index)
{
    lens_flare_render(-1, index, 1.0f, -1, 0);
}

extern "C" void lens_flares_submit_occlusions(long user_index, int query_type)
{
    c_lens_flare_hash_table *table = (c_lens_flare_hash_table *)&g_lens_flare_globals;
    lens_flare_globals *globals = &g_lens_flare_globals;
    c_rasterizer::set_explicit_shaders(17, _vertex_type_world, _transfer_vector_vertex_none, _entry_point_default);
    g_render_method_submit_cache = k_default_render_method_submit_cache;
    D3DDevice_SetPixelShader(c_rasterizer::get_device(), 0);
    if (c_rasterizer::g_current_alpha_blend_mode != c_rasterizer::_alpha_blend_opaque) {
        c_rasterizer::g_current_alpha_blend_mode = c_rasterizer::_alpha_blend_opaque;
        D3DDevice_SetRenderState_AlphaBlendEnable(c_rasterizer::get_device(), 0);
    }
    ::c_rasterizer::set_z_buffer_mode((c_rasterizer::e_z_buffer_mode)1);
    D3DDevice_SetRenderState_CullMode(c_rasterizer::get_device(), 6);
    for (long index = table->iterator_next(-1); index != -1; index = table->iterator_next(index)) {
        int frame = globals->frame;
        lens_flare_entry * flare = &globals->entries[index];
        if (frame - flare->frame == 0) {
            s_lens_flare_definition *definition = (s_lens_flare_definition *)tag_get('lens', flare->tag_index);
            if (!(definition->occlusion_flags & 2)) {
                if (!query_type)
                    rasterizer_occlusion_submit(_occlusion_query_type_player, 1, index, user_index, lens_flare_render_occlusion);
                else
                    rasterizer_occlusion_submit(_occlusion_query_type_player, 2, index, user_index, lens_flare_render_occlusion);
            }
        }
    }
    lens_flares_reset_rasterizer_parameters();
}

// @address 0x45be20 ?safe_generate_orthogonal_vector_up@@YAXPBTreal_vector3d@@PAT1@0@Z
void safe_generate_orthogonal_vector_up(const real_vector3d *forward, real_vector3d *up, const real_vector3d *reference)
{
    if (!reference)
        reference = global_left3d;
    float alignment = dot_product3d(reference, forward);
    if (fabsf(alignment) < 0.9f)
        cross_product3d(forward, reference, up);
    else
        cross_product3d(forward, global_backward3d, up);
    normalize3d(up);
    if (sqrtf(magnitude_squared3d(up)) <= 0.995f || !valid_real_vector3d_axes2(forward, up))
        *up = *global_up3d;
}

// @address 0x1835e0 ?lens_flares_render@@YAXJ@Z
void lens_flares_render(long user_index)
{
    c_lens_flare_hash_table *table = (c_lens_flare_hash_table *)&g_lens_flare_globals;
    lens_flare_globals *globals = &g_lens_flare_globals;
    ::c_rasterizer::set_z_buffer_mode((c_rasterizer::e_z_buffer_mode)1);
    c_rasterizer::set_explicit_shaders(46, _vertex_type_screen, _transfer_vector_vertex_none, _entry_point_default);
    for (long index = table->iterator_next(-1); index != -1; index = table->iterator_next(index)) {
        long occlusion[2];
        occlusion[0] = 0;
        occlusion[1] = -1;
        lens_flare_entry *flare = &globals->entries[index];
        bool current = globals->frame - flare->frame == 0;
        s_lens_flare_definition *definition = (s_lens_flare_definition *)tag_get('lens', flare->tag_index);
        float ratio = 1.0f;
        if ((definition->occlusion_flags & 2) == 0) {
            rasterizer_occlusions_get_result(_occlusion_query_type_player, index, user_index, &occlusion[0], &occlusion[1]);
            ratio = (float)occlusion[0] / (float)flare->age;
            ratio = PIN(ratio, 0.0f, 1.0f);
        }
        if (current && ratio > 0.05f)
            lens_flare_render(user_index, index, ratio, occlusion[1], 1);
    }
    if (c_rasterizer::g_current_alpha_blend_mode != c_rasterizer::_alpha_blend_opaque) {
        c_rasterizer::g_current_alpha_blend_mode = c_rasterizer::_alpha_blend_opaque;
        D3DDevice_SetRenderState_AlphaBlendEnable(c_rasterizer::get_device(), 0);
    }
    D3DDevice_SetRenderState_CullMode(c_rasterizer::get_device(), 2);
    ::c_rasterizer::set_z_buffer_mode((c_rasterizer::e_z_buffer_mode)0);
}




static __forceinline float render_lens_flares_game_time_get_safe_in_seconds(void)
{
    if (game_time_globals != 0 && game_time_globals->initialized)
        return (float)game_time_globals->time * game_time_globals->tick_length;
    return 0.0f;
}

static __forceinline real_rgb_color *render_lens_flares_pixel32_to_real_rgb_color(unsigned long pixel, real_rgb_color *color)
{
    const float scale = 1.0f / 255.0f;
    color->red = (float)((pixel >> 16) & 255) * scale;
    color->green = (float)((pixel >> 8) & 255) * scale;
    color->blue = (float)(pixel & 255) * scale;
    return color;
}

// @address 0x45b500 ?lens_flare_evaluate_parameters@@YA_NJJMPAUs_lens_flare_render_parameters@@@Z
bool lens_flare_evaluate_parameters(long lens_flare_index, long definition_index, float occlusion_factor, s_lens_flare_render_parameters *parameters)
{
    bool valid = false;
    lens_flare_entry *flare = &g_lens_flare_globals.entries[lens_flare_index];
    const s_lens_flare_definition *definition = (const s_lens_flare_definition *)tag_get('lens', definition_index);
    const render_camera *camera = &c_player_view::get_current()->render_camera_state;
    const render_projection *projection =
        &c_player_view::get_current()->render_projection_state;

    real_vector3d camera_forward = camera->forward;
    real_point3d camera_position = camera->position;
    float current_time = render_lens_flares_game_time_get_safe_in_seconds();
    float unknown_scale = 1.0f;
    real_rgb_color modulation_color;
    modulation_color.red = 1.0f;
    modulation_color.green = 1.0f;
    modulation_color.blue = 1.0f;
    float incident_intensity = 1.0f;

    real_vector3d flare_offset;
    {
        const real_point3d *camera_point = &camera_position;
        const real_point3d *flare_point = &flare->position;
        flare_offset.n[0] = flare_point->x - camera_point->x;
        flare_offset.n[1] = flare_point->y - camera_point->y;
        flare_offset.n[2] = flare_point->z - camera_point->z;
    }
    float incident_distance = dot_product3d(&camera_forward, &flare_offset);

    real_vector3d reflected_offset;
    scale_vector3d(&camera_forward, incident_distance, &reflected_offset);
    subtract_vectors3d(&reflected_offset, &flare_offset, &reflected_offset);
    scale_vector3d(&reflected_offset, 2.0f, &reflected_offset);

    parameters->camera_forward = camera->forward;
    parameters->camera_up = camera->up;
    cross_product3d(&camera->up, &camera->forward, &parameters->camera_left);
    normalize3d(&parameters->camera_left);

    negate_vector3d(&flare_offset, &parameters->incident_direction);
    normalize3d(&parameters->incident_direction);

    if (definition->far_fade_begin != definition->far_fade_end)
    {
        float distance_factor = (incident_distance - definition->far_fade_end) / (definition->far_fade_begin - definition->far_fade_end);
        incident_intensity *= PIN(distance_factor, 0.0f, 1.0f);
    }

    if (definition->near_fade_begin != definition->near_fade_end && incident_distance < definition->near_fade_end)
    {
        float distance_factor = (incident_distance - definition->near_fade_begin) / (definition->near_fade_end - definition->near_fade_begin);
        incident_intensity *= PIN(distance_factor, 0.0f, 1.0f);
    }

    if (definition->falloff_angle != definition->cutoff_angle)
    {
        float cos_angle = PIN(dot_product3d(&flare->direction, &parameters->incident_direction), -1.0f, 1.0f);
        float angle = acosf(cos_angle);
        if (angle > definition->falloff_angle)
        {
            incident_intensity *= 1.0f - PIN((angle - definition->falloff_angle) / (definition->cutoff_angle - definition->falloff_angle), 0.0f, 1.0f);
        }
    }

    incident_intensity *= PIN(unknown_scale, 0.0f, 1.0f);
    incident_intensity *= occlusion_factor;

    if (!TEST_BIT(definition->flags, 0))
    {
        dword seed = lens_flare_index;
        seed >>= 5;
        dword crc = 0xffffffff;
        crc_checksum_buffer(crc, &seed, sizeof(seed));
        seed = crc;
        current_time += (float)(seed & 0xff) / 256.0f;
        current_time += (float)((seed >> 8) & 0xff);
    }

    if (definition->time_brightness.count > 0)
    {
        const c_function_definition *function = (const c_function_definition *)tag_block_get_range_with_size(&definition->time_brightness, 0, 1,
            sizeof(c_function_definition));
        incident_intensity *= function->evaluate_scalar(current_time, 0.0f);
    }

    if (definition->age_brightness.count > 0)
    {
        const c_function_definition *function = (const c_function_definition *)tag_block_get_range_with_size(&definition->age_brightness, 0, 1,
            sizeof(c_function_definition));
        incident_intensity *= function->evaluate_scalar(flare->scale, 0.0f);
    }

    if (incident_intensity > 0.0f)
    {
        float corona_rotation = lens_flares_evaluate_corona_rotation_function(
            definition->rotation_function, definition->rotation_function_scale, &flare->position, &flare->direction);
        float horizon_angle = (float)atan2((double)dot_product3d(&flare_offset, &projection->view_to_world.forward),
            (double)dot_product3d(&flare_offset, &projection->view_to_world.left));

        if (definition->time_color.count > 0)
        {
            const s_lens_flare_color_function *color_function =
                (const s_lens_flare_color_function *)tag_block_get_range_with_size(&definition->time_color, 0, 1, sizeof(s_lens_flare_color_function));
            const c_function_definition *function = &color_function->function;
            real_rgb_color color;
            float scalar = function->evaluate_scalar(current_time, 0.0f);
            if (flare->element_type == 2 && flare->element_key1 != NONE && color_function->input_variable != _string_id_empty_string)
            {
                scalar = object_get_function_value_simple(flare->element_key1, color_function->input_variable, flare->tag_index);
            }
            function->evaluate_color(scalar, 1.0f, &color);
            modulation_color.red *= color.red;
            modulation_color.green *= color.green;
            modulation_color.blue *= color.blue;
        }

        if (definition->age_color.count > 0)
        {
            const s_lens_flare_color_function *color_function =
                (const s_lens_flare_color_function *)tag_block_get_range_with_size(&definition->age_color, 0, 1, sizeof(s_lens_flare_color_function));
            const c_function_definition *function = &color_function->function;
            real_rgb_color color;
            float scalar = function->evaluate_scalar(flare->scale, 0.0f);
            if (flare->element_type == 2 && flare->element_key1 != NONE && color_function->input_variable != _string_id_empty_string)
            {
                scalar = object_get_function_value_simple(flare->element_key1, color_function->input_variable, flare->tag_index);
            }
            function->evaluate_color(scalar, 1.0f, &color);
            modulation_color.red *= color.red;
            modulation_color.green *= color.green;
            modulation_color.blue *= color.blue;
        }

        if (definition->time_rotation.count > 0)
        {
            const c_function_definition *function = (const c_function_definition *)tag_block_get_range_with_size(&definition->time_rotation, 0,
                1, sizeof(c_function_definition));
            corona_rotation += function->evaluate_scalar(current_time, 0.0f) * k_pi;
        }

        if (definition->age_rotation.count > 0)
        {
            const c_function_definition *function = (const c_function_definition *)tag_block_get_range_with_size(&definition->age_rotation, 0, 1,
                sizeof(c_function_definition));
            corona_rotation += function->evaluate_scalar(flare->scale, 0.0f) * k_pi;
        }

        const s_lens_flare_reflection *reflection = (const s_lens_flare_reflection *)tag_block_get_range_with_size(&definition->reflections, 0,
            1, sizeof(s_lens_flare_reflection));
        float alpha = 1.0f;
        float size = flare->rotation;
        float interpolant = flare->intensity;

        alpha *= PIN(reflection->brightness_min + (reflection->brightness_max - reflection->brightness_min) * interpolant, 0.0f, 1.0f);
        size *= reflection->radius_min + (reflection->radius_max - reflection->radius_min) * interpolant;
        alpha *= incident_intensity;

        if (alpha > 0.0f && size > 0.0f)
        {
            real_rgb_color reflection_color = modulation_color;
            float rotation = reflection->rotation_offset * k_degrees_to_radians;
            rotation += corona_rotation;
            const real_rgb_color *tint_color = &reflection->tint_color;
            reflection_color.red *= tint_color->red;
            reflection_color.green *= tint_color->green;
            reflection_color.blue *= tint_color->blue;

            if (!TEST_BIT(reflection->flags, 4))
            {
                real_rgb_color flare_color;
                render_lens_flares_pixel32_to_real_rgb_color(flare->color, &flare_color);
                reflection_color.red *= flare_color.n[0];
                reflection_color.green *= flare_color.n[1];
                reflection_color.blue *= flare_color.n[2];
            }

            if (TEST_BIT(reflection->flags, 0))
            {
                rotation += horizon_angle;
            }

            if (TEST_BIT(reflection->flags, 2))
            {
                size *= PIN(occlusion_factor * 0.5f + 0.5f, 0.0f, 1.0f);
            }

            if (TEST_BIT(reflection->flags, 1))
            {
                size *= incident_distance;
            }

            real_vector4d *out_color = &parameters->color;
            parameters->rotation = rotation;
            parameters->size = size;
            out_color->i = reflection_color.red;
            out_color->j = reflection_color.green;
            out_color->k = reflection_color.blue;
            out_color->l = 1.0f;
            parameters->alpha = alpha;
            valid = true;
        }
    }

    return valid;
}

// @address 0x73a244 g_render_far_plane
extern "C" {
float g_render_far_plane = 10240.0f;
}

// @address 0x45bcc0 ?lens_flare_project_billboard@@YA_NPBTreal_point3d@@MPBTreal_vector2d@@PATreal_vector4d@@PAT2@@Z
// match: 0x45bcc0: restrict on size keeps the tail fdivs ahead of the fmuls chain with the 0Ch(r30) store sourced from fr13
bool lens_flare_project_billboard(const real_point3d *point, float radius, const real_vector2d *__restrict size,
    // match: 0x45bcc0: restrict on projected_point keeps the mid-body lfs/fmuls/fmadds schedule and float coloring with fr30 spilled
    real_vector4d *__restrict projected_point, real_vector2d *projected_size)
{
    bool result = false;
    const render_projection *projection = &c_player_view::get_current()->render_projection_state;
    real_point3d view_point;
    matrix4x3_transform_point(&projection->world_to_view, point, &view_point);
    float clip_x = projection->projection_matrix[0][0] * view_point.x + projection->projection_matrix[1][0] * view_point.y +
        projection->projection_matrix[2][0] * view_point.z + projection->projection_matrix[3][0];
    float clip_y = projection->projection_matrix[0][1] * view_point.x + projection->projection_matrix[1][1] * view_point.y +
        projection->projection_matrix[2][1] * view_point.z + projection->projection_matrix[3][1];
    float clip_z = projection->projection_matrix[0][2] * view_point.x + projection->projection_matrix[1][2] * view_point.y +
        projection->projection_matrix[2][2] * view_point.z + projection->projection_matrix[3][2];
    float clip_w = projection->projection_matrix[0][3] * view_point.x + projection->projection_matrix[1][3] * view_point.y +
        projection->projection_matrix[2][3] * view_point.z + projection->projection_matrix[3][3];
    float scale_x = projection->projection_matrix[0][0];
    float scale_y = projection->projection_matrix[1][1];
    if (clip_z > 0.0f)
    {
        float inverse_w = 1.0f / clip_w;
        float far_plane = g_render_far_plane;
        projected_point->i = clip_x * inverse_w;
        projected_point->j = clip_y * inverse_w;
        projected_point->k = MIN(clip_z * inverse_w, 1.0f) * far_plane;
        projected_point->l = far_plane * clip_w / far_plane;
        projected_size->i = scale_x * inverse_w * size->i;
        projected_size->j = scale_y * inverse_w * size->j;
        result = true;
    }
    return result;
}

extern "C" {
// @address 0x4b7a8 k_screen_bitmap_quad_vertices
}


static __forceinline short render_lens_flares_rectangle_width(const rectangle2d *bounds)
{
    return bounds->x1 - bounds->x0;
}

static __forceinline short render_lens_flares_rectangle_height(const rectangle2d *bounds)
{
    return bounds->y1 - bounds->y0;
}

static __forceinline real_point2d *render_lens_flares_set_point2d(real_point2d *point, float x, float y)
{
    point->x = x;
    point->y = y;
    return point;
}

static __forceinline rectangle2d *render_lens_flares_set_rectangle2d(rectangle2d *bounds, short x0, short y0, short x1, short y1)
{
    bounds->x0 = x0;
    bounds->y0 = y0;
    bounds->x1 = x1;
    bounds->y1 = y1;
    return bounds;
}

static inline float render_lens_flares_normalized_to_pixel(float coordinate, short origin, short extent)
{
    return coordinate * (float)extent * 0.5f + (float)origin - 0.5f;
}

// match: 0x45ad00: precise float_control keeps the fr0/fr11/fr12 coloring and fadds order of the half-pixel-extent blend math
#pragma float_control(precise, on, push)
static __forceinline float render_lens_flares_half_pixel_extent(short extent, float size)
{
    return (float)extent * size * 0.5f;
}
// match: 0x45ad00: restore default float control after the half-pixel-extent helper
#pragma float_control(pop)

// @address 0x45ad00 lens_flare_render
extern "C" void lens_flare_render(int user_index, int lens_flare_index, float occlusion_factor, int query, int screen_space)
{
    lens_flare_entry *flare = &g_lens_flare_globals.entries[lens_flare_index];
    long definition_index = flare->tag_index;
    const s_lens_flare_definition *definition = (const s_lens_flare_definition *)tag_get('lens', definition_index);
    if (definition->reflections.count > 0)
    {
        const s_lens_flare_reflection *reflection = (const s_lens_flare_reflection *)definition->reflections.address;
        bool matches_user = user_index == -1 || user_index == flare->element_key2;
        bool visible = (!(definition->occlusion_flags & 4) && !(definition->occlusion_flags & 8)) ||
            ((definition->occlusion_flags & 4) && (flare->flags & 1)) ||
            ((definition->occlusion_flags & 8) && !(flare->flags & 1));
        occlusion_factor = transition_function_evaluate(definition->falloff_function, occlusion_factor);
        if (matches_user && visible && occlusion_factor > 0.0001f)
        {
            real_vector2d aspect_ratio;
            lens_flare_get_bitmap_aspect_ratio(definition->bitmap.index, reflection->bitmap_index, &aspect_ratio);
            real_vector4d projected_point;
            real_vector2d projected_size;
            s_lens_flare_render_parameters parameters;
            if (lens_flare_project_billboard(&flare->position, 1.0f, &aspect_ratio, &projected_point, &projected_size) &&
                lens_flare_evaluate_parameters(lens_flare_index, definition_index, occlusion_factor, &parameters))
            {
                if ((unsigned char)screen_space)
                {
                    if (reflection->flags & 4)
                        parameters.size *= occlusion_factor;
                    real_vector4d position;
                    position.i = projected_point.i;
                    position.j = projected_point.j;
                    position.k = parameters.rotation;
                    ((D3DDevice *)c_rasterizer::get_device())->SetVertexShaderConstantF(240, position.n, 1);
                    real_vector4d size;
                    size.i = projected_size.i;
                    size.j = projected_size.j;
                    size.k = parameters.size;
                    ((D3DDevice *)c_rasterizer::get_device())->SetVertexShaderConstantF(241, size.n, 1);
                    ((D3DDevice *)c_rasterizer::get_device())->SetPixelShaderConstantF(51, parameters.color.n, 1);
                    float animation_speed = !(reflection->animation_speed > 0.0001f) ? 4.0f : reflection->animation_speed;
                    if (query != -1)
                        D3DDevice_BeginConditionalRendering((D3DDevice *)c_rasterizer::get_device(), query);
                    rasterizer_draw_screen_bitmap(k_screen_bitmap_quad_vertices, definition->bitmap.index, reflection->bitmap_index,
                        parameters.alpha, reflection->tint_modulation_factor, animation_speed);
                    if (query != -1)
                        D3DDevice_EndConditionalRendering((D3DDevice *)c_rasterizer::get_device());
                }
                else
                {
                    float cosine = (float)cos((double)parameters.rotation);
                    float sine = (float)sin((double)parameters.rotation);
                    parameters.size /= pow(2.0f, (float)definition->exposure);
                    if (!rasterizer_tiling_get_state()->enabled)
                    {
                        rectangle2d window = c_player_view::get_current()->render_camera_state.window_pixel_bounds;
                        short window_width = render_lens_flares_rectangle_width(&window);
                        short window_height = render_lens_flares_rectangle_height(&window);
                        real_point2d center;
                        render_lens_flares_set_point2d(&center,
                            render_lens_flares_normalized_to_pixel(projected_point.i + 1.0f, window.x0, window_width),
                            render_lens_flares_normalized_to_pixel(1.0f - projected_point.j, window.y0, window_height));
                        real_vector2d size;
                        set_real_vector2d(&size,
                            render_lens_flares_half_pixel_extent(window_width, projected_size.i) * parameters.size,
                            render_lens_flares_half_pixel_extent(window_height, projected_size.j) * parameters.size);
                        rectangle2d bounds;
                        render_lens_flares_set_rectangle2d(&bounds,
                            (short)(center.x - size.i), (short)(center.y - size.j),
                            (short)(center.x + size.i), (short)(center.y + size.j));
                        long area = render_lens_flares_rectangle_width(&bounds) * render_lens_flares_rectangle_height(&bounds);
                        flare->age = MAX(area, 1);
                    }
                    real_vector3d up;
                    safe_generate_orthogonal_vector_up(&parameters.incident_direction, &up, &parameters.camera_left);
                    real_matrix4x3 matrix;
                    matrix4x3_rotation_from_vectors(&matrix, &parameters.incident_direction, &up);
                    matrix.position = flare->position;
                    float width = aspect_ratio.i * parameters.size;
                    float height = aspect_ratio.j * parameters.size;
                    real_point3d points[4];
                    points[0].x = 0.0f; points[0].y = -width; points[0].z = -height;
                    points[1].x = 0.0f; points[1].y = -width; points[1].z = height;
                    points[2].x = 0.0f; points[2].y = width; points[2].z = height;
                    points[3].x = 0.0f; points[3].y = width; points[3].z = -height;
                    rasterizer_vertex_world vertices[4];
                    for (unsigned long index = 0; index < 4; ++index)
                    {
                        rotate_vector2d(&points[index].yz, sine, cosine, &points[index].yz);
                        matrix4x3_transform_point(&matrix, &points[index], &vertices[index].position);
                        if (definition->orientation != 2)
                        {
                            real_vector3d direction = definition->orientation == 0 ? parameters.incident_direction : flare->direction;
                            point_from_line3d(&vertices[index].position, &direction,
                                definition->direction_offset, &vertices[index].position);
                        }
                    }
                    c_rasterizer::draw_worldspace_polygon(vertices, 4);
                }
            }
        }
    }
}
