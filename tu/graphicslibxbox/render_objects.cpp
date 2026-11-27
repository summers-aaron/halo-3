// graphicslibxbox/render_objects.obj: from the records 0x1638f8 0x1639e0 0x163b78 0x163df0 0x1641f8 0x1643b8 0x164458 0x1644b8 0x164540 0x164f90 0x165218 0x165350 0x165520 0x1655f0 0x165860 0x165998 0x16bd70 0x170880 0x170958 0x170a18 0x170aa8 0x170c98 0x172c48 0x172f50 0x1741d0 0x176110 0x17ec20 0x17f970 0x17fa98 0x180598 0x202a40 0x23d168 0x23d1f8 0x4470b0 0x447b60 0x447b78 0x468ef0 0x468f00 0x468f70 0x468fd0 0x469228 0x469290 0x4695d0 0x469638 0x4696b0 0x469b78 0x469d60 0x469e98 0x469fe0 0x46a1d8 0x46a278 0x46a290
#include "cseries/string_literal_pool.h"
#include "cseries/pointer_utilities.h"
#include "rasterizer/rasterizer_device_state.h"
#include "rasterizer/rasterizer_sampler_filter_state.h"
#include "camera/observer_gamestate_tls.h"
#include "main/main_render_fence.h"
#include "rasterizer/xenon/rasterizer_xenon_memory.h"
#include "math/d3dx_matrix.h"
#include "cseries/placement_new.h"
#include "cutscene/cinematic_new_globals_types.h"
#include "interface/chud/chud_manager.h"
#include "visibility/visible_items_types.h"
#include "effects/effect_array_globals.h"
#include "visibility/visible_items.h"
#include "render/render_object_storage.h"
#include "render/render_simple_lights.h"
#include "rasterizer/rasterizer_device_globals.h"
#include "structures/structure_bsp_tag.h"
#include "rasterizer/xenon/rasterizer_xenon_device.h"
#include "rasterizer/xenon/rasterizer_xenon_d3d.h"
#include "render/render_objects_submission_globals.h"
#include "render/render_objects_globals.h"
#include "render_methods/render_method_data.h"
#include "rasterizer/rasterizer_implicit_geometry_types.h"
#include "game/game_engine_scoring_globals.h"
#include "render/render_player_view_globals.h"
#include "render/render_water_globals.h"
#include "render_methods/render_method_submit_globals.h"
#include "rasterizer/rasterizer_viewport_state.h"
#include "rasterizer/rasterizer_distortion_state.h"
#include "effects/lens_flare_attachment_types.h"
#include "rasterizer/rasterizer_main_globals.h"
#include "render/render_camera_types.h"
#include "objects/object_reference_types.h"
#include "game/game_globals_storage.h"
#include "game/game_state_accessors.h"
#include "memory/data_types.h"
#include "memory/data.h"
#include "memory/datum_index.h"
#include "math/real_math_types.h"
#include "math/real_math.h"
#include "math/matrix_math_types.h"
#include "tag_files/tag_types.h"
#include "objects/object_datum.h"
#include "objects/object_placement.h"
#include "objects/widgets/widget_types.h"
#include "render/render_object_globals_types.h"
#include "structures/structure_globals.h"
#include "structures/structure_seam_cluster_stack.h"
#include "models/model_skinning_types.h"
#include "models/render_model_definitions.h"
#include "scenario/scenario_structure_bsp_reference.h"
#include "structures/structure_seams.h"
#include "rasterizer/rasterizer_hdr_targets.h"
#include "game/game_engine_list.h"
#include <ppcintrinsics.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "cache/restricted_memory_regions.h"
#include "objects/objects.h"
#include "effects/player_effects.h"
#include "game/game_engine_display.h"
#include "game/game_engine_util.h"
#include "game/game_time.h"
#include "game/game_time_types.h"
#include "interface/chud/chud.h"
#include "interface/chud/chud_draw.h"
#include "structures/structure_cluster_portal.h"
#include "scenario/scenario_tag.h"
#include "geometry/render_geometry_types.h"
#include "geometry/compression_info_types.h"
#include "cache/restricted_memory_layout.h"
#include "cache/restricted_memory.h"
#include "interface/interface_constants.h"
#include "models/model_skinning.h"
#include "objects/widgets/widgets.h"
#include "rasterizer/rasterizer_occlusion_queries.h"
#include "render/render.h"
#include "render/render_mesh.h"
#include "render/render_objects.h"
#include "render/render_objects_static_lighting.h"
#include "render_methods/render_method_submit.h"
#include "structures/structures.h"
#include "units/vehicle_type_component.h"
#include "visibility/visibility_render_objects.h"
#include "effects/decals.h"
#include "effects/effects.h"
#include "interface/user_interface.h"
#include "rasterizer/dx9/rasterizer_dx9_dynamic_geometry.h"
#include "rasterizer/rasterizer_implicit_geometry.h"
#include "rasterizer/rasterizer_render_targets.h"
#include "rasterizer/xenon/rasterizer_xenon_main.h"
#include "render/render_water.h"
#include "rasterizer/rasterizer_states.h"
#include "render/render_lens_flares.h"
#include "render/render_lights.h"
#include "render/views/render_player_view.h"
#include "math/matrix_math.h"
#include "game/game_engine_default.h"
#include "game/player_masks_types.h"
#include "effects/transparent_sort_layer_types.h"
#include "cache/cache_files.h"
#include "scenario/scenario.h"
#include "sky_atm/atmosphere.h"
#include "game/player_mapping_types.h"
#include "render/render_transparents.h"
#include "models/model_skinning_matrix_types.h"
#include "render/lighting_interface.h"
#include "rasterizer/rasterizer_tiling_state_header.h"
#include "memory/tls_data_arrays.h"
#include "render/render_object_info.h"
#include "objects/object_material_types.h"
#include "models/model_types.h"
#include "sky_atm/sky_atm_parameters_types.h"
#include "effects/effect_render_globals.h"
#include "rasterizer/xenon/rasterizer_device_types.h"
#include "rasterizer/rasterizer_vertex_definitions.h"
#include "render_methods/render_method_definitions.h"
#include "rasterizer/rasterizer.h"
#include "rasterizer/rasterizer_stencil.h"
#include "geometry/geometry_material_types.h"
#include "geometry/render_geometry_parts.h"
#include "geometry/render_geometry_mesh_types.h"
#include "render/object_renderer.h"
#include "rasterizer/rasterizer_main.h"
#include "render/render_view.h"
#include "camera/director_globals_types.h"
#include "camera/saved_film_director_storage.h"
#include "render/render_first_person_globals_types.h"
#include "cache/restricted_memory_region_members.h"
#include "sky_atm/weighted_atmosphere_parameters.h"
#include "render/render_objects_dependencies.h"
#include "render/render_lighting_types.h"
#include "render/views/player_view_types.h"
#include "render/render_visibility_collection_imports.h"
#include "rasterizer/rasterizer_skinning_memory_types.h"
#include "math/matrix_math_functions.h"
#include "objects/object_marker_globals_types.h"
#include "visibility/visibility_collection.h"
#include "render/render_water_xenon.h"
#include "objects/object_definitions.h"
#include "objects/object_attachment_types.h"
#include "effects/lens_flares.h"
#include "geometry/transparent_sorting_node_types.h"
#include "render_methods/render_method.h"
#include "render/shader_extern_info_types.h"
#include "rasterizer/rasterizer_global_definitions.h"
#include "game/game_globals.h"
#include "rasterizer/rasterizer_state_globals.h"
#include "render/water_renderer.h"
#include "sky_atm/atmosphere_fog_interface.h"
#include "render/screen_postprocess.h"
#include "rasterizer/rasterizer_render_target_types.h"
#include "effects/effect_camera.h"
#include "rasterizer/rasterizer_stipple.h"
#include "objects/object_header.h"
#include "objects/object_markers_inline.h"
#include "objects/object_node_matrices_inline.h"
#include "tag_files/tag_groups.h"
#include "interface/chud/chud_user_data_types.h"

struct s_shield_impact_color
{
    real_rgb_color color;
    float intensity;
};

struct s_shield_impact_definition
{
    s_tag_reference noise_texture1;
    s_tag_reference noise_texture2;
    float vertex_scale;
    real_vector2d noise_scale;
    real_vector3d noise_offset1;
    real_vector3d noise_offset2;
    s_shield_impact_color colors[6];
};


// @address 0x8c482e g_render_method_submit_cache_bypass
unsigned char g_render_method_submit_cache_bypass;
// @address 0x8c482f g_render_method_submit_cached_state_enabled
// @address 0x11df70c g_director_globals
// @address 0x8c46dc ?x_current_player_view@c_player_view@@1PAV1@A
// @address 0x16f0a8 ?rasterizer_set_packed_blend_state@@YAXPAUD3DDevice@@KUblend_state_value@@@Z
// @address 0x11df8c0 g_cinematic_light_globals
// @address 0x7f4310 g_object_marker_globals
// @address 0x73e318 g_widget_types
// @address 0x8a1ecc ?g_active_structure_bsp_mask@@3KA
// @address 0x8c4694 g_rasterizer_device
// @address 0x81dab8 g_rasterizer_alpha_blend_mode
// @address 0x7c6178 g_rasterizer_sampler_filter_modes
// @address 0x195e88 rasterizer_device_set_sampler_filter_modes
// @flags /TP /O2
// @address 0x11df8e4 g_render_object_globals
extern "C" {
__declspec(thread) s_render_object_globals *g_render_object_globals;
}
// @address 0x5a5420 D3DDevice_SetRenderState_ClipPlaneEnable
// @address 0x5a6730 D3DDevice_SetClipPlane
struct s_data_array;

struct s_data_array;

// @address 0x172f50 ?pop_marker@c_object_renderer@@SAXXZ
// @address 0x202a40 ?__tls_set_g_render_object_globals_allocator@@YAXPAX@Z
// @address 0x468ef0 ?__tls_set_g_render_object_globals@@YAXPAX@Z
// @address 0x468f00 ?add_object_render_context@c_object_renderer@@SAJXZ

struct s_mesh_part_counter;
struct s_render_object_info;
// @address 0x7374fc g_first_person_object_index
// @address 0x737500 g_first_person_physics_object_index

struct s_mesh_part_counter;

// 0x1643b8

// @address 0x164458 object_get_render_model_instance_bitfield_visible
extern "C" bool object_get_render_model_instance_bitfield_visible(long object_index, void **out_address)
{
  object_datum *object = object_get(object_index);
  if ((bool)(_rotl(object->flags, 21) & 1)) {
    short *block_offset = &object->block_offset;
    *out_address = (char *)object + (block_offset[-1] + *block_offset) - 8;
    return true;
  }
  return false;
}

struct s_shield_impact_definition;

typedef s_cluster_reference s_render_objects_bsp_reference;


static __forceinline bool render_objects_bsp_is_active(long bsp_index)
{
  return (unsigned long)bsp_index <= 15 && ((1 << bsp_index) & g_active_structure_bsp_mask) != 0;
}

static __forceinline int render_objects_thread_index()
{
  return g_registered_thread_index;
}
static __forceinline s_structure_globals *render_objects_structure_globals_get()
{
  int current_thread = render_objects_thread_index();
  return &thread_structure_globals[current_thread - 1];
}
static __forceinline void render_objects_cluster_mark(s_render_objects_bsp_reference reference)
{
  s_structure_globals *globals = render_objects_structure_globals_get();
  globals->cluster_markers[reference.bsp_index][reference.cluster_index] = globals->cluster_marker_sequence;
}
static __forceinline bool render_objects_cluster_unmarked(s_render_objects_bsp_reference reference)
{
  s_structure_globals *globals = render_objects_structure_globals_get();
  return globals->cluster_markers[reference.bsp_index][reference.cluster_index] != globals->cluster_marker_sequence;
}
static __forceinline structure_bsp *render_objects_bsp_get(long bsp_index)
{
  return (structure_bsp *)tag_get(0, ((scenario_structure_bsp_reference *)global_scenario->structure_bsps.address)[bsp_index].structure_bsp.index);
}

// @address 0x165520 render_objects_compute_portal_distance
extern "C" float render_objects_compute_portal_distance(const real_point3d *point, s_render_objects_bsp_reference reference)
{
  float result = 3.4028234663852886e+38f;
  structure_bsp *bsp = (structure_bsp *)tag_get(0, 
      ((scenario_structure_bsp_reference *)global_scenario->structure_bsps.address)[reference.bsp_index].structure_bsp.index);
  structure_cluster *cluster = &((structure_cluster *)bsp->clusters.address)[reference.cluster_index];
  for (long portal_index = 0; portal_index < cluster->portal_indices.count; portal_index++) {
    short portal = (short)((unsigned short *)cluster->portal_indices.address)[portal_index];
    float distance = structure_distance_to_portal((const structure_bsp *)bsp,
        (const cluster_portal *)((char *)bsp->cluster_portals.address + portal * 40), point);
    if (!(result < distance))
      result = distance;
  }
  return result;
}

// @address 0x165350 render_objects_compute_cluster_distance
extern "C" float render_objects_compute_cluster_distance(s_render_objects_bsp_reference source, const real_point3d *point,
                              long *cluster_indices, s_render_objects_bsp_reference reference)
{
  float result;
  if (source.bsp_index == reference.bsp_index && source.cluster_index == reference.cluster_index) {
    result = 0.0f;
  } else {
    result = render_objects_compute_portal_distance(point, reference);
  }
  long packed_reference = (reference.bsp_index << 8) + reference.cluster_index;
  render_objects_cluster_mark(reference);
  structure_bsp *bsp = render_objects_bsp_get(reference.bsp_index);
  structure_cluster *cluster = &((structure_cluster *)bsp->clusters.address)[reference.cluster_index];
  if (cluster->seam_indices.count > 0) {
    s_cluster_stack connected;
    connected.m_count = 0;
    if (structure_seams_connected_cluster_references_get(global_scenario_index, reference, false, &connected)) {
      for (long i = 0; i < connected.m_count; i++) {
        s_render_objects_bsp_reference adjacent = connected.m_elements[i];
        if (render_objects_cluster_unmarked(adjacent)) {
          long adjacent_packed = (adjacent.bsp_index << 8) + adjacent.cluster_index;
          cluster_indices[adjacent_packed] = cluster_indices[packed_reference];
          float distance = render_objects_compute_cluster_distance(source, point, cluster_indices, adjacent);
          if (distance < result)
            result = distance;
        }
      }
    }
  }
  return result;
}

static __forceinline long render_objects_cluster_reference_index(s_render_objects_bsp_reference reference)
{
  return (reference.bsp_index << 8) + reference.cluster_index;
}

// @address 0x165218 render_objects_compute_cluster_distances
extern "C" void render_objects_compute_cluster_distances(s_render_objects_bsp_reference source, const real_point3d *point,
                             long count, const s_render_objects_bsp_reference *references,
                             float *distances)
{
  int current_thread = render_objects_thread_index();
  s_structure_globals *globals_end = thread_structure_globals + current_thread;
  s_structure_globals *globals = globals_end - 1;
  globals->cluster_marker_sequence++;
  globals->cluster_marker_initialized = true;
  long cluster_indices[4096];
  for (long i = 0; i < count; i++) {
    s_render_objects_bsp_reference reference = references[i];
    if (globals->cluster_markers[reference.bsp_index][reference.cluster_index] != globals->cluster_marker_sequence) {
      cluster_indices[render_objects_cluster_reference_index(references[i])] = i;
      distances[i] = render_objects_compute_cluster_distance(source, point, cluster_indices, reference);
    } else {
      distances[i] = distances[cluster_indices[render_objects_cluster_reference_index(references[i])]];
    }
  }
  globals_end[-1].cluster_marker_initialized = false;
}

// @address 0x165860 render_objects_get_cluster_element
extern "C" void *render_objects_get_cluster_element(s_render_objects_bsp_reference reference, const char **out_name)
{
  void *result = 0;
  if (out_name != 0)
    *out_name = 0;
  if (g_effect_render_globals->camera_relative != 0) {
    long bsp_index = reference.bsp_index;
    if (bsp_index != -1 && render_objects_bsp_is_active(bsp_index)) {
      ::scenario *scenario = (::scenario *)global_scenario;
      if (scenario->sky_parameters.index != -1) {
        s_sky_atm_parameters_definition *element_tag =
            (s_sky_atm_parameters_definition *)tag_get(0, scenario->sky_parameters.index);
        if (element_tag != 0) {
          structure_bsp *bsp_tag =
              (structure_bsp *)tag_get(0, ((scenario_structure_bsp_reference *)scenario->structure_bsps.address)[bsp_index].structure_bsp.index);
          signed char element_index = ((structure_cluster *)bsp_tag->clusters.address)[reference.cluster_index].atmosphere_index;
          if (element_index != -1) {
            unsigned short element =
                ((structure_atmosphere_palette_entry *)bsp_tag->atmosphere_palette.address)[element_index].atmosphere_setting_index;
            if (element < element_tag->atmosphere_settings.count)
              result = &((c_atmosphere_setting *)element_tag->atmosphere_settings.address)[element];
          } else if (element_tag->atmosphere_settings.count > 0) {
            result = element_tag->atmosphere_settings.address;
            if (out_name != 0)
              *out_name = "global_default";
          }
        }
      }
    }
  }
  return result;
}

static __forceinline void render_objects_clear_element_weights(s_sky_atm_parameters_definition *element_tag)
{
  for (long i = 0; i < element_tag->atmosphere_settings.count; i++) {
    c_atmosphere_setting *element = (c_atmosphere_setting *)(&((c_atmosphere_setting *)element_tag->atmosphere_settings.address)[i]);
    element->weight = 0.0f;
    element->cluster_weight = 0.0f;
  }
}

static __forceinline void render_objects_element_add_weight(float *value, float weight)
{
  *value += weight;
}

static __forceinline bool render_objects_cluster_references_equal(s_cluster_reference a, s_cluster_reference b)
{
  return a.value() == b.value();
}

// @address 0x164f90 render_objects_update_element_weights
extern "C" void render_objects_update_element_weights(s_cluster_reference cluster_reference, const real_point3d *point)
{
  if (g_effect_render_globals->camera_relative == 0)
    return;
  ::scenario *scenario = (::scenario *)global_scenario;
  if (scenario->sky_parameters.index == -1)
    return;
  s_sky_atm_parameters_definition *element_tag =
      (s_sky_atm_parameters_definition *)tag_get(0, scenario->sky_parameters.index);

  if (game_globals->options.game_mode == 2 && scenario->atmosphere.count <= 1) {
    render_objects_clear_element_weights(element_tag);
    const char *name;
    c_atmosphere_setting *element = (c_atmosphere_setting *)render_objects_get_cluster_element(cluster_reference, &name);
    if (element != 0) {
      element->weight = 1.0f;
      element->cluster_weight = 1.0f;
    }
    return;
  }

  float falloff_distance = element_tag->falloff_distance;
  float falloff_start = element_tag->falloff_start;
  float falloff_exponent = element_tag->falloff_exponent;
  float radius = falloff_distance * 3.0f;
  s_cluster_reference cluster_references[100];
  float distances[100];
  long cluster_count = structure_clusters_in_sphere(cluster_reference, point, radius < 20.0f ? 20.0f : radius, 100, cluster_references, 0);
  render_objects_compute_cluster_distances(cluster_reference, point, cluster_count, cluster_references, distances);

  float total_weight = 0.0f;
  for (long i = 0; i < cluster_count; i++)
    total_weight += render_objects_compute_falloff_weight(distances[i], falloff_start, falloff_exponent, falloff_distance);

  render_objects_clear_element_weights(element_tag);
  for (long i = 0; i < cluster_count; i++) {
    const char *name = 0;
    c_atmosphere_setting *element = (c_atmosphere_setting *)render_objects_get_cluster_element(cluster_references[i], &name);
    if (element != 0) {
      float weight = render_objects_compute_falloff_weight(distances[i], falloff_start, falloff_exponent, falloff_distance) / total_weight;
      render_objects_element_add_weight(&element->weight, weight);
      if (element_tag->flags & 1) {
        if (render_objects_cluster_references_equal(cluster_references[i], cluster_reference))
          element->cluster_weight = 1.0f;
      } else {
        render_objects_element_add_weight(&element->cluster_weight, weight);
      }
    }
  }
}

// @address 0x170958 render_objects_set_hiz_stencil_state
extern "C" void render_objects_set_hiz_stencil_state(void *memory)
{
  g_rasterizer_device->m_ControlPacket.HiControl.dword |= 1;
  g_rasterizer_device->m_ControlPacket.HiControl.dword |= 2;
  g_rasterizer_device->m_ControlPacket.HiControl.dword &= ~4;
  g_rasterizer_device->m_ControlPacket.HiControl.dword &= ~8;
  g_rasterizer_device->m_ControlPacket.HiControl.dword |= 0x10;
  g_rasterizer_device->m_ControlPacket.HiControl.dword |= 0x20;
  g_rasterizer_device->m_ControlPacket.HiControl.bytes[2] = 0;
  g_rasterizer_device->m_ControlPacket.HiControl.dword &= 0x1ffff;
  g_rasterizer_device->m_Pending.m_Mask[2] |= 0x100;
  g_rasterizer_device->m_ControlPacket.EdramModeControl = (g_rasterizer_device->m_ControlPacket.EdramModeControl & ~7) | 4;
  g_rasterizer_device->m_Pending.m_Mask[2] |= 8;
  D3DDevice_FlushHiZStencil(g_rasterizer_device, 0);
}

typedef char render_objects_material_size[(sizeof(geometry_material) == 36) ? 1 : -1];

typedef char render_objects_mesh_size[(sizeof(s_mesh) == 0x4c) ? 1 : -1];
typedef char render_objects_mesh_type_offset[(offsetof(s_mesh, vertex_type) == 0x2e) ? 1 : -1];
struct s_part;
// @address 0x1643b8 render_objects_add_instance_mesh_part
extern "C" void render_objects_add_instance_mesh_part(s_mesh_part_counter *counter, s_render_object_info *object_info, unsigned long flags)
{
  render_model_definition *definition =
      (render_model_definition *)tag_get(0, object_info->definition_index);
  void *address;
  if (definition->instance_starting_mesh_index != -1 && definition->instance_placements.count > 0 &&
      object_get_render_model_instance_bitfield_visible(object_info->object_index, &address)) {
    ::c_object_renderer::add_context_mesh_part(counter, object_info,
        (unsigned long *)((unsigned long *)address), definition->instance_starting_mesh_index, -1, -1, -1, flags | 8);
  }
}
// @address 0x1644b8 ?add_context_mesh_part@c_object_renderer@@SAJPAUs_mesh_part_counter@@PAUs_render_object_info@@PAKJJJJJ@Z
long c_object_renderer::add_context_mesh_part(struct s_mesh_part_counter *counter, struct s_render_object_info *_sym_p1,
    unsigned long *data, long mesh_index, long part_index, long region_index, long render_method_index, long _sym_p7)
{
  s_render_object_globals *globals = g_render_object_globals;
  long result = -1;
  if (globals->mesh_part_count < 8192) {
    result = globals->mesh_part_count;
    long index = globals->mesh_part_count++;
    s_context_mesh_part *part = &globals->mesh_parts[index];
    part->flags = ((unsigned long)_sym_p7);
    part->object_info = _sym_p1;
    part->data = data;
    part->part_index = part_index;
    part->mesh_index = mesh_index;
    part->region_index = region_index;
    part->render_method_index = render_method_index;
    part->shield_impact = 0;
    counter->count++;
  }
  return result;
}

// @address 0x8c484e g_render_static_lighting_active

// 0x468ef0 natural source
void __tls_set_g_render_object_globals(void *globals)
{
  g_render_object_globals = (s_render_object_globals *)globals;
}

static __forceinline long render_objects_add_object_render_context()
{
  s_render_object_globals *globals = g_render_object_globals;
  long context_index = -1;
  globals->contexts_added = true;
  if (globals->object_count < 1024) {
    s_object_render_context *context = &globals->contexts[globals->object_count];
    context->mesh_parts.upper = globals->mesh_part_count;
    context->mesh_parts.count = 0;
    context->lighting = 0;
    context->shader_extern_info = 0;
    context_index = globals->object_count++;
  }
  return context_index;
}

long c_object_renderer::add_object_render_context(void)
{
  return render_objects_add_object_render_context();
}

// @address 0x170a18 ?render_object_contexts@c_object_renderer@@SAXW4e_entry_point@@J@Z
void c_object_renderer::render_object_contexts(e_entry_point entry_point, long p1)
{
  s_render_object_globals *globals = g_render_object_globals;
  long index = globals->marker_object_counts[globals->marker_depth];
  if (index < globals->object_count) {
    do {
      render_objects_render_object_context(index, entry_point, p1, 0);
      index++;
    } while (index < globals->object_count);
  }
  if (g_render_static_lighting_active == 0)
    c_rasterizer::set_stencil_mode(c_rasterizer::_stencil_mode_off);
}

static __forceinline s_object_visibility *render_objects_visible_list_next(simple_list<s_object_visibility, 896> *list, long *index)
{
  if (*index < (long)list->count - 1) {
    (*index)++;
    return &list->entries[*index];
  }
  *index = -1;
  return 0;
}

static __forceinline s_object_visibility *render_objects_visible_list_new(simple_list<s_object_visibility, 896> *list, long *index, long start)
{
  *index = start - 1;
  return render_objects_visible_list_next(list, index);
}

// @address 0x172c48 ?render_object_widgets@c_object_renderer@@SAXW4e_entry_point@@@Z
void c_object_renderer::render_object_widgets(e_entry_point entry_point)
{
  long index;
  s_object_visibility *object = render_objects_visible_list_new(&c_visible_items::m_items.objects, &index,
      c_visible_items::get_objects_starting_index());
  for (; object != 0; object = render_objects_visible_list_next(&c_visible_items::m_items.objects, &index)) {
    if ((object->render_setup.flags & 2) != 0)
      widgets_render_opaque(object->render_setup.object_index, entry_point);
  }
}

static __forceinline s_object_hierarchy *render_objects_visible_hierarchy_next(simple_list<s_object_hierarchy, 768> *list, long *index)
{
  if (*index < (long)list->count - 1) {
    (*index)++;
    return &list->entries[*index];
  }
  *index = -1;
  return 0;
}
static __forceinline s_object_hierarchy *render_objects_visible_hierarchy_new(simple_list<s_object_hierarchy, 768> *list, long *index, long start)
{
  *index = start - 1;
  return render_objects_visible_hierarchy_next(list, index);
}

// @address 0x1638f8 ?submit_visibility@c_object_renderer@@SAXJ@Z
void c_object_renderer::submit_visibility(long render)
{
  long index;
  simple_list<s_object_hierarchy, 768> *hierarchies = &c_visible_items::m_items.hierarchies;
  s_object_hierarchy *hierarchy = render_objects_visible_hierarchy_new(hierarchies, &index, c_visible_items::get_root_objects_starting_index());
  for (; hierarchy != 0; hierarchy = render_objects_visible_hierarchy_next(hierarchies, &index)) {
    if ((hierarchy->flags & 8) != 0) {
      if (hierarchy->object_count != 0) {
        render_objects_submit_visible_hierarchy(hierarchy->first_object, hierarchy->object_count, hierarchy->levels_word,
            hierarchy->flags & 3, render);
      }
    }
  }
}

void c_object_renderer::pop_marker(void)
{
  s_render_object_globals *globals = g_render_object_globals;
  globals->marker_depth--;
  globals->object_count = globals->marker_object_counts[globals->marker_depth + 1];
  globals->mesh_part_count = globals->marker_mesh_part_counts[globals->marker_depth + 1];
}

void __tls_set_g_render_object_globals_allocator(void *allocator)
{
    g_observer_gamestate_globals = allocator;
}

const float k_render_objects_pow_one = 1.0f;

static __forceinline float render_objects_clamp_unit_interval(float t)
{
    if (t > k_render_objects_pow_one)
        return k_render_objects_pow_one;
    if (t > 0.0f)
        return t <= k_render_objects_pow_one ? t : k_render_objects_pow_one;
    return 0.0f;
}

// @address 0x23d168 render_objects_compute_falloff_weight
extern "C" float render_objects_compute_falloff_weight(float p0, float p1, float p2, float p3)
{
    if (p3 <= p1)
        return k_render_objects_pow_one;
    float span = p3 - p0;
    float t = render_objects_clamp_unit_interval(span / (p3 - p1));
    return (float)pow((double)t, (double)p2);
}

// @address 0x23d1f8 render_objects_reset_cached_indices
extern "C" void render_objects_reset_cached_indices(void)
{
    memset(g_effect_render_globals->cached_indices, -1, sizeof(g_effect_render_globals->cached_indices));
}

// @address 0x4470b0 ?__tls_set_g_effect_render_globals@@YAXPAX@Z
void __tls_set_g_effect_render_globals(void *value)
{
    g_effect_render_globals = (s_effect_render_globals *)value;
}

// @address 0x447b60 ?allocate@?$t_restricted_allocation_manager@$02$0A@$0A@$1?__tls_set_g_effect_render_globals@@YAXPAX@Z@@UAAPAXIPBD@Z
template<>
void *t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_effect_render_globals>::allocate(unsigned int size, const char *name)
{
    return reserve_memory(name, "", size, 0);
}

// @address 0x447b78 ?deallocate@?$t_restricted_allocation_manager@$02$0A@$0A@$1?__tls_set_g_effect_render_globals@@YAXPAX@Z@@UAAXPAX@Z
template<>
void t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_effect_render_globals>::deallocate(void *allocation)
{
    int value = m_member_index;
    g_effect_render_globals = 0;
    restricted_region_free_member(3, value);
}

static __forceinline s_widget_type *widget_definition_get(short widget_type)
{
    return &g_widget_types[widget_type];
}
static __forceinline long widget_type_get_flags(short widget_type)
{
    s_widget_type *definition = widget_definition_get(widget_type);
    return definition->flags;
}
static __forceinline void widgets_render_unattached(long entry_point)
{
    for (long widget_type = 0; widget_type < 4; ++widget_type) {
        s_widget_type *definition = widget_definition_get((short)widget_type);
        if ((widget_type_get_flags((short)widget_type) & 4) != 0 && definition->render_unattached)
            definition->render_unattached(entry_point);
    }
}

// @address 0x468f70 render_objects_get_lighting_definition
extern "C" void *render_objects_get_lighting_definition(long tag_index, unsigned char flag)
{
    void *result = 0;
    s_object_material_definition *definition = (s_object_material_definition *)tag_get(0, tag_index);
    long model_index = definition->model.index;
    if (model_index != -1) {
        s_model_definition *model = (s_model_definition *)tag_get(0, model_index);
        long child_index;
        if (flag != 0) {
            child_index = model->tag_index_184;
        } else {
            child_index = model->tag_index_174;
        }
        if (child_index != -1) {
            result = tag_get(0, child_index);
        }
    }
    return result;
}

// @address 0x469228 render_objects_is_first_person_object
extern "C" bool render_objects_is_first_person_object(long object_index)
{
    long ultimate_parent = -1;
    if (object_index != -1) {
            do {
            ultimate_parent = object_index;
            object_index = object_get(ultimate_parent)->parent_object_index;
        } while (object_index != -1);
    }
    return ultimate_parent == g_first_person_object_index || ultimate_parent == g_first_person_physics_object_index;
}

// @address 0x17ec20 ?render_static_lighting@c_object_renderer@@SAXW4e_z_buffer_mode@c_rasterizer@@@Z
void c_object_renderer::render_static_lighting(c_rasterizer::e_z_buffer_mode z_buffer_mode)
{
  c_rasterizer::set_z_buffer_mode(z_buffer_mode);
  D3DDevice_SetRenderState_CullMode(g_rasterizer_device, 2);
  D3DDevice_SetRenderState_ColorWriteEnable(g_rasterizer_device, 0);
  c_object_renderer::render_object_contexts(_entry_point_shadow_generate, 2);
  c_object_renderer::render_object_widgets(_entry_point_shadow_generate);
  c_object_renderer::render_unattached_widgets(_entry_point_shadow_generate);
}

// @address 0x4695d0 ?render_shadows_generate@c_object_renderer@@SAXW4e_entry_point@@@Z
void c_object_renderer::render_shadows_generate(e_entry_point entry_point)
{
  D3DDevice_SetRenderState_CullMode(g_rasterizer_device, 2);
  D3DDevice_SetRenderState_ColorWriteEnable(g_rasterizer_device, 15);
  c_object_renderer::render_object_contexts(entry_point, 1);
  c_object_renderer::render_object_widgets(entry_point);
  c_object_renderer::render_unattached_widgets(entry_point);
}

// @address 0x469638 ?render_unattached_widgets@c_object_renderer@@SAXW4e_entry_point@@@Z
void c_object_renderer::render_unattached_widgets(e_entry_point entry_point)
{
    if (c_view::get_current_stack_level() == 0)
        widgets_render_unattached(entry_point);
}


// @address 0x8c482d g_decorator_albedo_active

// @address 0x10f5d70 g_rasterizer_xenon_memory


// @address 0x170880 ?render_albedo@c_object_renderer@@SAXJ@Z
void c_object_renderer::render_albedo(long p1)
{
  c_rasterizer::set_z_buffer_mode(c_rasterizer::_z_buffer_mode_normal);
  if ((unsigned int)g_render_hiz_stencil_enabled != 0) {
    if (g_rasterizer_alpha_blend_mode != 0) {
      g_rasterizer_alpha_blend_mode = 0;
      D3DDevice_SetRenderState_AlphaBlendEnable(g_rasterizer_device, 0);
    }
    render_objects_set_hiz_stencil_state(&g_rasterizer_xenon_memory);
  }
  D3DDevice_SetRenderState_CullMode(g_rasterizer_device, 2);
  D3DDevice_SetRenderState_ColorWriteEnable(g_rasterizer_device, 15);
  D3DDevice_SetRenderState_ColorWriteEnable1(g_rasterizer_device, 15);
  g_render_method_submit_cached_state_enabled = g_render_method_submit_cache_bypass == 0;
  c_object_renderer::render_object_contexts(_entry_point_albedo, p1);
  render_method_submit_reset_cached_state();
  c_object_renderer::render_object_widgets(_entry_point_albedo);
  c_object_renderer::render_unattached_widgets(_entry_point_albedo);
}

static __forceinline void render_objects_first_person_matrix_from_point_and_quaternion(
    real_matrix4x3 *matrix, const real_point3d *point, const real_quaternion *quaternion)
{
    matrix3x3_rotation_from_quaternion(&matrix->rotation, quaternion);
    matrix->scale = 1.0f;
    matrix->position = *point;
}

// @address 0x469fe0 ?first_person_get_markers_by_string_id@@YAFJJPAUobject_marker@@J@Z
short first_person_get_markers_by_string_id(long user_index, long marker_name, object_marker *markers, long maximum_marker_count)
{
    long count = 0;
    s_render_object_first_person_globals *globals = &render_object_globals;
    if (user_index != -1 && g_director_globals->slots[user_index].perspective == 0) {
        for (long instance_index = 0; instance_index < globals->model_count - 1; instance_index++) {
            render_first_person_model *instance = &globals->models[instance_index];
            if (instance->render_model_index != -1) {
                render_model_definition *definition = (render_model_definition *)tag_get(0, instance->render_model_index);
                for (long group_index = 0; group_index < definition->marker_groups.count; group_index++) {
                    render_model_marker_group *group = &((render_model_marker_group *)definition->marker_groups.address)[group_index];
                    if (group->name == marker_name) {
                        for (long marker_index = 0; marker_index < group->markers.count; marker_index++) {
                            if (count < 16) {
                                render_model_marker *marker = &((render_model_marker *)group->markers.address)[marker_index];
                                render_model_node *node = &((render_model_node *)definition->nodes.address)[marker->node_index];
                                real_matrix4x3 node_matrix;
                                memcpy(&node_matrix, &instance->nodes[marker->node_index], sizeof(node_matrix));
                                real_matrix4x3 inverse;
                                matrix4x3_inverse(&node->default_inverse_matrix, &inverse);
                                real_matrix4x3 unused;
                                matrix4x3_compute_product(&node_matrix, &inverse, &unused);
                                real_matrix4x3 marker_matrix;
                                render_objects_first_person_matrix_from_point_and_quaternion(&marker_matrix, &marker->translation, &marker->rotation);
                                matrix4x3_compute_product(&node_matrix, &marker_matrix, &marker_matrix);
                                markers[count].node_index = marker->node_index;
                                memcpy(&markers[count].matrix, &marker_matrix, sizeof(real_matrix4x3));
                                memcpy(&markers[count].node_matrix, &node_matrix, sizeof(real_matrix4x3));
                                markers[count].scale = 1.0f;
                                count++;
                            }
                        }
                    }
                }
            }
        }
    }
    return (short)count;
}

// @address 0x46a1d8 ?reserve_memory@?$t_restricted_allocation_manager@$03$0A@$0A@$1?__tls_set_g_render_object_globals_allocator@@YAXPAX@Z@@QAAPAXPBD0IJ@Z
template<>
void *t_restricted_allocation_manager<4, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_render_object_globals_allocator>::reserve_memory(
    const char *name, const char *type, unsigned int size, long alignment_bits)
{
    int member_index = g_restricted_regions[4].add_member(name, type, size, 0, __tls_set_g_render_object_globals, 0, 0);
    unsigned int offset = g_restricted_regions[4].get_member_offset(member_index);
    void *address = g_restricted_address[4] + offset;
    int owner = g_registered_thread_index;
    m_thread_id = owner;
    m_member_index = member_index;
    g_render_object_globals = (s_render_object_globals *)address;
    return g_restricted_address[4] + g_restricted_regions[4].get_member_offset(member_index);
}

// @address 0x46a290 ?deallocate@?$t_restricted_allocation_manager@$03$0A@$0A@$1?__tls_set_g_render_object_globals_allocator@@YAXPAX@Z@@UAAXPAX@Z
template <>
void t_restricted_allocation_manager<4, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_render_object_globals_allocator>::deallocate(void *memory)
{
    long value = m_member_index;
    g_render_object_globals = 0;
    restricted_region_free_member(4, value);
}

// @address 0x46a278 ?allocate@?$t_restricted_allocation_manager@$03$0A@$0A@$1?__tls_set_g_render_object_globals_allocator@@YAXPAX@Z@@UAAPAXIPBD@Z
template <>
void *t_restricted_allocation_manager<4, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_render_object_globals_allocator>::allocate(unsigned int size, const char *name)
{
    return (void *)reserve_memory(name, "", size, 0);
}

// @address 0x1655f0 render_objects_accumulate_atmosphere

extern "C" void render_objects_accumulate_atmosphere(s_cluster_reference cluster, const c_atmosphere_setting *setting,
                             // match: 0x1655f0: pointer __restrict qualifier prevents alias reload
                             s_weighted_atmosphere_parameters *__restrict parameters, float weight)
{
  if ((setting->flags & 1) != 0) {
    if (weight > 0.0f)
      parameters->valid = true;
    real_rgb_color color;
    real_vector3d direction;
    atmosphere_get_sun_parameters(cluster, setting, &color, &direction);
    parameters->sun_color.red = color.red * weight + parameters->sun_color.red;
    parameters->sun_color.green = color.green * weight + parameters->sun_color.green;
    parameters->sun_color.blue = color.blue * weight + parameters->sun_color.blue;
    parameters->sun_direction.i = direction.i * weight + parameters->sun_direction.i;
    parameters->sun_direction.j = direction.j * weight + parameters->sun_direction.j;
    parameters->sun_direction.k = direction.k * weight + parameters->sun_direction.k;
    parameters->sun_intensity = setting->distance_bias * weight + parameters->sun_intensity;
    parameters->distance_bias = setting->max_fog_thickness * weight + parameters->distance_bias;
    parameters->rayleigh_scattering.red = setting->beta_m.i * weight + parameters->rayleigh_scattering.red;
    parameters->rayleigh_scattering.green = setting->beta_m.j * weight + parameters->rayleigh_scattering.green;
    parameters->rayleigh_scattering.blue = setting->beta_m.k * weight + parameters->rayleigh_scattering.blue;
    parameters->mie_scattering.red = setting->beta_p.i * weight + parameters->mie_scattering.red;
    parameters->mie_scattering.green = setting->beta_p.j * weight + parameters->mie_scattering.green;
    parameters->mie_scattering.blue = setting->beta_p.k * weight + parameters->mie_scattering.blue;
    parameters->fog_color.red = setting->beta_m_theta_prefix.i * weight + parameters->fog_color.red;
    parameters->fog_color.green = setting->beta_m_theta_prefix.j * weight + parameters->fog_color.green;
    parameters->fog_color.blue = setting->beta_m_theta_prefix.k * weight + parameters->fog_color.blue;
    parameters->mie_color.red = setting->beta_p_theta_prefix.i * weight + parameters->mie_color.red;
    parameters->mie_color.green = setting->beta_p_theta_prefix.j * weight + parameters->mie_color.green;
    parameters->mie_color.blue = setting->beta_p_theta_prefix.k * weight + parameters->mie_color.blue;
    parameters->mie_anisotropy = setting->sun_phase_function * weight + parameters->mie_anisotropy;
    parameters->rayleigh_height = setting->sea_level * weight + parameters->rayleigh_height;
    parameters->fog_intensity = setting->mie_height_scale * weight + parameters->fog_intensity;
    parameters->mie_height = setting->rayleigh_height_scale * weight + parameters->mie_height;
    if ((setting->flags & 4) != 0) {
      parameters->patchy_density = setting->sheet_density * weight + parameters->patchy_density;
      parameters->patchy_start_distance = setting->full_intensity_height * weight + parameters->patchy_start_distance;
      parameters->patchy_end_distance = setting->half_intensity_height * weight + parameters->patchy_end_distance;
      parameters->patchy_velocity.i = setting->wind_direction.i * weight + parameters->patchy_velocity.i;
      parameters->patchy_velocity.j = setting->wind_direction.j * weight + parameters->patchy_velocity.j;
      parameters->patchy_velocity.k = setting->wind_direction.k * weight + parameters->patchy_velocity.k;
    }
  }
}

typedef char render_objects_lighting_probe_offset[(offsetof(render_lighting, sample.red) == 0x18) ? 1 : -1];
typedef char render_objects_lighting_direction_offset[(offsetof(render_lighting, sample.dominant_light_direction) == 0x1e0) ? 1 : -1];
typedef char render_objects_lighting_scale_offset[(offsetof(render_lighting, sample.light_scale) == 0x200) ? 1 : -1];
typedef char render_objects_lighting_index_offset[(offsetof(render_lighting, unknown) == 0x214) ? 1 : -1];

// @address 0x8c483c g_lighting_debug_mode


static __forceinline float render_objects_lighting_luminance_sum(const float *red, const float *green, const float *blue)
{
    float sum = 0.0f;
    unsigned long remaining = 4;
    long i = 0;
    do {
        sum = (float)fabs(green[i]) * 0.715158f + (float)fabs(red[i]) * 0.212656f + (float)fabs(blue[i]) * 0.0721856f + sum;
        ++i;
    } while (--remaining != 0);
    return sum;
}


// @address 0x825da0 g_render_clip_planes
extern "C" {
D3DXPLANE g_render_clip_planes[32];
}

static __forceinline void render_objects_clip_plane_from_entry(const s_cinematics_new_group *entry, real_plane3d *plane)
{
    float y = entry->start.j;
    float x = entry->start.i;
    float z = entry->start.k;
    const real_vector3d *normal = &entry->end;
    plane->normal = *normal;
    plane->distance = z * plane->normal.k + (x * plane->normal.i + y * plane->normal.j);
}

// @address 0x16bd70 render_objects_update_clip_planes
extern "C" void render_objects_update_clip_planes(void *view)
{
    const s_cinematics_new_group *entries = cinematic_new_globals->groups;
    long count = cinematic_new_globals->group_count;
    D3DXMATRIX projection;
    memcpy(&projection, &c_player_view::get_current()->get_rasterizer_projection()->projection_matrix, sizeof(projection));
    D3DXMatrixTranspose(&projection, &projection);
    D3DXMatrixInverse(&projection, 0, &projection);
    long time = game_time_globals->time;
    if (count > 0)
    {
        for (long index = 0; index < count; ++index)
        {
            const s_cinematics_new_group *entry = &entries[index];
            real_plane3d plane;
            D3DXPLANE zero_plane;
            union
            {
                real_plane3d engine;
                D3DXPLANE d3d;
            } transformed_plane;
            D3DXPLANE projected_plane;
            if (time >= entry->frame0 && time < entry->frame1)
            {
                render_objects_clip_plane_from_entry(entry, &plane);
                matrix4x3_transform_plane(&c_player_view::get_current()->get_rasterizer_projection()->world_to_view,
                    &plane, new (&transformed_plane.engine) real_plane3d);
                const real_plane3d view_plane = transformed_plane.engine;
                transformed_plane.d3d.a = view_plane.normal.i;
                transformed_plane.d3d.b = view_plane.normal.j;
                transformed_plane.d3d.c = view_plane.normal.k;
                transformed_plane.d3d.d = -view_plane.distance;
                D3DXPlaneTransform(&projected_plane, &transformed_plane.d3d, &projection);
                g_render_clip_planes[index] = projected_plane;
            }
            else
            {
                zero_plane.d = 0.0f;
                zero_plane.a = 0.0f;
                zero_plane.b = 0.0f;
                zero_plane.c = 0.0f;
                g_render_clip_planes[index] = zero_plane;
            }
        }
    }
    D3DDevice_SetRenderState_ClipPlaneEnable(g_rasterizer_device, 0);
}


// @address 0x81dacc g_last_skinning_matrix_count

static __forceinline s_model_skinning *render_objects_get_skinning(long designator);
static __forceinline const s_model_skinning_matrix *render_objects_skinning_region_matrices(
    const s_model_skinning *skinning, long region_index, long *matrix_count)
{
  *matrix_count = skinning->regions[region_index].node_matrix_count;
  return &skinning->base_node_matrices[(short)skinning->regions[region_index].first_node_matrix];
}
static __forceinline const s_model_skinning_matrix *render_objects_skinning_rigid_matrix(const s_model_skinning *skinning, long region_index)
{
  return &skinning->base_node_matrices[(short)skinning->regions[region_index].first_node_matrix];
}

// @address 0x469d60 ?render_object_context_mesh_part@c_object_renderer@@SAXPAUs_context_mesh_part@@W4e_entry_point@@@Z
void c_object_renderer::render_object_context_mesh_part(s_context_mesh_part *mesh_part, e_entry_point entry_point)
{
  render_model_definition *model = (render_model_definition *)tag_get(0, 
      mesh_part->object_info->definition_index);
  s_render_geometry *geometry = &model->render_geometry;
  s_mesh *mesh = &((s_mesh *)geometry->meshes.address)[mesh_part->mesh_index];
  render_mesh_submit_compression((const s_compression_info *)geometry->compression_info.address);
  long matrix_count = 1;
  s_model_skinning *skinning = render_objects_get_skinning(mesh_part->object_info->skinning_memory_designator);
  const s_model_skinning_matrix *matrices;
  if (mesh->vertex_type == 2) {
    matrices = render_objects_skinning_region_matrices(skinning, mesh_part->region_index, &matrix_count);
  } else {
    matrices = render_objects_skinning_rigid_matrix(skinning, mesh_part->region_index);
  }
  if ((unsigned char)entry_point == 0 || g_last_skinning_matrix_address != matrices ||
      g_last_skinning_matrix_count != matrix_count)
    render_mesh_submit_skinning(matrices, matrix_count);
  render_method_submit_set_extern_info(31 - _CountLeadingZeros(-((long)g_active_structure_bsp_mask) & ((long)g_active_structure_bsp_mask)),
      mesh_part->object_info->shader_extern_info);
}

// @address 0x469e98 render_objects_render_clipped_mesh_part
extern "C" void render_objects_render_clipped_mesh_part(s_context_mesh_part *part, long p1, long p2, long entry_point)
{
  render_model_definition *definition =
      (render_model_definition *)tag_get(0, part->object_info->definition_index);
  s_render_geometry *entries = &definition->render_geometry;
  s_mesh *entry = &((s_mesh *)entries->meshes.address)[part->mesh_index];
  c_object_renderer::render_object_context_mesh_part(part, (e_entry_point)entry_point);
  if (part->object_info->cinematic_clip_plane_mask != 0) {
    D3DXPLANE *plane = g_render_clip_planes;
    long plane_index = 0;
    unsigned long enabled_planes = 0;
    long bit = 0;
    do {
      if ((((1 << bit) & part->object_info->cinematic_clip_plane_mask) != 0) && (plane_index < 6)) {
        D3DDevice_SetClipPlane(g_rasterizer_device, plane_index, &plane->a);
        enabled_planes |= 1 << plane_index;
        plane_index++;
      }
      bit++;
      plane++;
    } while (bit < 32);
    D3DDevice_SetRenderState_ClipPlaneEnable(g_rasterizer_device, enabled_planes);
  }
  if (definition->materials.count > 0) {
    unsigned __int64 high = ((unsigned __int64)(short)part->part_index << 16) + (unsigned __int64)(short)part->mesh_index;
    // The cache key adds the sign-extended 32-bit geometry address.
    const int geometry_address = static_cast<int>(address_from_pointer(entries));
    unsigned __int64 packed = (high << 32) + (unsigned __int64)geometry_address;
    render_method_submit_draw_mesh_part(packed, entries, entry,
      &((s_part *)entry->parts.address)[part->part_index], p1, 0, (void *)definition->materials.address);
  }
  D3DDevice_SetRenderState_ClipPlaneEnable(g_rasterizer_device, 0);
  c_render_method_data::m_valid = false;
  render_method_submit_valid();
}

// @address 0x170c98 render_objects_set_lighting_constants
extern "C" long render_objects_set_lighting_constants(const render_lighting *lighting, long result, long type)
{
    if (type <= 8 || type == 13)
    {
        s_cinematic_light_globals *globals = g_cinematic_light_globals;
        const render_lighting *entry = lighting;
        if (lighting)
        {
            long index = lighting->unknown;
            if (index >= 0 && index < globals->object_entry_count)
                entry = &globals->object_entries[index].lighting;
            if (entry)
            {
                float scale = 1.0f;
                if (lighting->sample.light_scale > 0.0f && g_lighting_debug_mode == 0)
                {
                    const float *red = entry->sample.red;
                    const float *green = entry->sample.green;
                    const float *blue = entry->sample.blue;
                    float sum = render_objects_lighting_luminance_sum(red, green, blue);
                    if (sum <= 0.001f)
                        sum = 0.001f;
                    float requested = lighting->sample.light_scale / sum;
                    if (requested > scale)
                        scale = requested;
                }
                c_lighting_interface::calculate_and_set_ravi_constants(entry->sample.red, entry->sample.green,
                    entry->sample.blue, scale);
                c_lighting_interface::set_object_dominant_light_direction_and_intensity(
                    &entry->sample.dominant_light_direction, &entry->sample.dominant_light_color);
                result = 1;
            }
        }
    }
    return result;
}

// @address 0x180598 render_objects_render_transparent_part
extern "C" void render_objects_render_transparent_part(s_object_render_context *pass, long part_index)
{
  s_render_object_globals *globals = g_render_object_globals;
  render_objects_set_lighting_constants(pass->lighting, 0, 5);
  rasterizer_set_shadow_stencil_ref(pass->stencil_index, 0, 1, 0);
  render_objects_render_clipped_mesh_part(&globals->mesh_parts[part_index], 8, 4, 0);
  if (g_render_static_lighting_active == 0)
    c_rasterizer::set_stencil_mode(c_rasterizer::_stencil_mode_off);
}

// @address 0x170aa8 render_objects_render_object_context
extern "C" void render_objects_render_object_context(long object_index, long entry_point, long p2, char p3)
{
  s_render_object_globals *globals = g_render_object_globals;
  s_object_render_context *context = &globals->contexts[object_index];

  if (entry_point >= 5)
    render_objects_set_lighting_constants(context->lighting, 0, entry_point);

  if (g_decorator_albedo_active != 0) {
    if ((context->mesh_parts.flag & 1) != 0) {
      if (g_rasterizer_alpha_blend_mode != 12) {
        blend_state_value blend_state = { 0xe0001 };
        g_rasterizer_alpha_blend_mode = 12;
        D3DDevice_SetRenderState_BlendFactor(g_rasterizer_device, -1);
        rasterizer_set_packed_blend_state(g_rasterizer_device, 0, blend_state);
      }
    }
  }

  int stencil_index = context->stencil_index;
  bool entry_flag = (((unsigned int)p2 >> 10) & 1) != 0;
  unsigned long first = context->mesh_parts.upper;
  int current_stencil = 0;
  rasterizer_set_shadow_stencil_ref(stencil_index, 0, 1, entry_flag);

  bool first_part = true;
  unsigned long index = first;
  if (first < first + context->mesh_parts.count) {
    unsigned char apply_blend = (unsigned char)p3;
    s_context_mesh_part *part = &globals->mesh_parts[first];
    do {
      if (apply_blend != 0 && g_rasterizer_alpha_blend_mode != 0) {
        g_rasterizer_alpha_blend_mode = 0;
        rasterizer_set_device_alpha_blend_mode(g_rasterizer_device, 0);
      }
      int part_stencil = ((unsigned long)part->flags >> 4) & 1;
      if (part_stencil != current_stencil) {
        current_stencil = part_stencil;
        rasterizer_set_shadow_stencil_ref(stencil_index, part_stencil, 1, entry_flag);
      }
      if ((part->flags & p2) != 0) {
        if ((part->flags & 8) != 0) {
          c_object_renderer::render_object_context_instances(part, (e_entry_point)entry_point, part->data, context->parameters, p2);
        } else {
          render_objects_render_clipped_mesh_part(part, entry_point, p2, first_part == 0);
          first_part = false;
        }
      }
      index++;
      part++;
    } while (index < first + context->mesh_parts.count);
  }

  if (g_decorator_albedo_active != 0 && (context->mesh_parts.flag & 1) != 0 && g_rasterizer_alpha_blend_mode != 11) {
    g_rasterizer_alpha_blend_mode = 11;
    rasterizer_set_device_alpha_blend_mode(g_rasterizer_device, 11);
  }
}

static __forceinline s_model_skinning *render_objects_get_skinning(long designator)
{
    s_model_skinning *skinning = 0;
    if (designator != -1 && (designator & (long)0x80000000) == 0)
        skinning = (s_model_skinning *)&rasterizer_memory_get_globals()->buffer[designator & 0x0fffffff];
    return skinning;
}
// match: noalias declaration preserves caller live registers
// match: noalias declaration preserves caller live registers

// @address 0x1641f8 ?add_transparent_mesh_part@c_object_renderer@@SA_NPBUs_render_geometry@@PBUs_mesh@@PBUs_part@@JJPBUs_object_render_context@@JPATreal_point3d@@_NEP6AXPBXJ@Z@Z
bool c_object_renderer::add_transparent_mesh_part(
    const s_render_geometry *render_geometry, const s_mesh *mesh, const s_part *part,
    long p3, long p4, const s_object_render_context *context, long p6,
    real_point3d *point, bool p8, unsigned char p9, void (*p10)(const void *, long))
{
    if ((mesh->vertex_type) != 2 && (mesh->vertex_type) != 1)
        return false;
    s_model_skinning *data = 0;
    if (p4 != -1 && (p4 & (int)0x80000000) == 0)
        data = (s_model_skinning *)&rasterizer_memory_get_globals()->buffer[p4 & 0xFFFFFFF];
    const s_model_skinning_matrix *skinning_matrix;
    long matrix_count;
    if ((mesh->vertex_type) == 2)
        skinning_matrix = render_objects_skinning_region_matrices(data, p3, &matrix_count);
    else
        skinning_matrix = render_objects_skinning_rigid_matrix(data, p3);
    const float *m = &skinning_matrix->rows[0][0];
    float length = sqrtf((m[4] * m[4] + m[0] * m[0]) + m[8] * m[8]);
    float divisor = length > 0.0001f ? length : 0.0001f;
    float normal = 1.0f / divisor;
    real_point3d transformed;
    real_matrix4x3 matrix;
    matrix.scale = length;
    matrix.rotation.forward.i = m[0] * normal;
    matrix.rotation.forward.j = m[4] * normal;
    matrix.rotation.forward.k = m[8] * normal;
    matrix.rotation.left.i = m[1] * normal;
    matrix.rotation.left.j = m[5] * normal;
    matrix.rotation.left.k = m[9] * normal;
    matrix.rotation.up.i = m[2] * normal;
    matrix.position.x = m[3];
    matrix.rotation.up.j = m[6] * normal;
    matrix.position.y = m[7];
    matrix.rotation.up.k = m[10] * normal;
    matrix.position.z = m[11];
    matrix4x3_transform_point(&matrix, point, &transformed);
    float sort_value = (float)p3 * 0.1f;
    if (p8)
        sort_value = sort_value + 100000.0f;
    return render_objects_add_transparent_element(&transformed, 0, sort_value, (e_transparent_sort_layer)p9, p10, context, p6, 0.0f);
}

struct D3DSurface;
static __forceinline real_point3d *render_objects_fog_point_from_line3d(
    const real_point3d *point, const real_vector3d *direction, float distance, real_point3d *result)
{
    real_vector3d scaled;
    scaled.i = direction->i * distance;
    scaled.j = direction->j * distance;
    scaled.k = direction->k * distance;
    result->x = point->x + scaled.i;
    result->y = point->y + scaled.j;
    result->z = point->z + scaled.k;
    return result;
}
// @address 0x17f970 render_objects_submit_patchy_fog
extern "C" void render_objects_submit_patchy_fog(c_player_view *view)
{
    const s_sky_atm_parameters_definition *parameters = 0;
    render_camera *camera = &view->render_camera_state;
    if (g_effect_render_globals->camera_relative)
    {
        long index = global_scenario->sky_parameters.index;
        if (index != -1)
            parameters = (const s_sky_atm_parameters_definition *)tag_get(0, index);
    }
    if (camera && parameters)
    {
        real_point3d point;
        real_plane3d plane;
        float distance = parameters->transparent_sort_distance;
        long sort_layer = parameters->transparent_sort_layer;
        render_objects_fog_point_from_line3d(&camera->position, &camera->forward, distance, &point);
        scale_vector3d(&camera->forward, -1.0f, &plane.normal);
        plane.distance = point.x * plane.normal.i + (point.y * plane.normal.j + point.z * plane.normal.k);
        if (sort_layer == 0)
            sort_layer = 2;
        render_objects_add_transparent_element(&point, &plane, 0.0f, (e_transparent_sort_layer)sort_layer,
            c_player_view::get_patchy_fog_callback(), view, 0, distance);
    }
}

static __forceinline s_object_marker_globals *render_objects_get_subpart_state()
{
    return &g_object_marker_globals[render_objects_thread_index() - 1];
}

// @address 0x176110 render_objects_update_visible_subparts
extern "C" void render_objects_update_visible_subparts(long render)
{
    if (c_visible_items::m_items.skies.count != 0)
    {
        render_objects_static_lighting_hierarchy_levels levels = {{255, 255, 255, 255}};
        s_object_marker_globals *state = render_objects_get_subpart_state();
        state->active = true;
        ++state->mark;
        render_objects_add_visible_hierarchy(c_visible_items::m_items.skies.entries[0], global_origin3d, 999999.0f, render,
            true, true, true, -1, levels, false, false, -1);
        c_visibility_collection::render_objects_generate_visible_subparts();
        render_objects_get_subpart_state()->active = false;
    }
}


// @address 0x86531c g_active_camo_texture_bounds
real_rectangle2d g_active_camo_texture_bounds;
// @address 0x86532c g_active_camo_resolve_bounds
rectangle2d g_active_camo_resolve_bounds;
// @address 0x8c4843 g_active_camo_capture_pending
bool g_active_camo_capture_pending;

// @address 0x8c4848 g_active_camo_resolve_pending
bool g_active_camo_resolve_pending;

// @address 0x72cb03 g_render_hiz_stencil_enabled
bool g_render_hiz_stencil_enabled = true;


// @address 0x17fa98 render_objects_render_transparent_pass
extern "C" void render_objects_render_transparent_pass(c_player_view *view)
{
  rasterizer_set_global_shader_parameter(3);
  rasterizer_set_global_shader_parameter(45);
  c_lights_view *light_block = &view->lights_view;
  render_lights_submit_simple_light_draw_list_to_shader(light_block->simple_light_data, light_block->simple_light_count);
  bool use_second_target = c_screen_postprocess::x_editable_settings.resolve_hdr || rasterizer_tiling_get_state_header()->enabled;
  rasterizer_main_setup_hdr_render_targets(use_second_target, true);
  if (g_active_camo_capture_pending) {
    g_active_camo_resolve_bounds = view->get_rasterizer_camera()->window_pixel_bounds;
    g_active_camo_resolve_pending = true;
    g_active_camo_capture_pending = false;
    g_active_camo_texture_bounds.x0 = 0.0f;
    g_active_camo_texture_bounds.x1 = 0.0f;
    g_active_camo_texture_bounds.y0 = 1.0f;
    g_active_camo_texture_bounds.y1 = 1.0f;
  }
  g_rasterizer_using_albedo_sampler = false;
  int pixel_constant = 1;
  D3DDevice_SetPixelShaderConstantB(g_rasterizer_device, 0, &pixel_constant, 1);
  long user_index = view->user_index;
  unsigned short saved_count = (unsigned short)c_transparency_renderer::m_total_transparent_count;
  long transparency_depth = ++c_transparency_renderer::m_current_marker_index;
  {
    s_render_object_globals *globals = g_render_object_globals;
    globals->marker_depth++;
    globals->marker_object_counts[globals->marker_depth] = globals->object_count;
    globals->marker_mesh_part_counts[globals->marker_depth] = globals->mesh_part_count;
  }
  c_transparency_renderer::m_markers[transparency_depth].first_transparent_index = saved_count;
  c_transparency_renderer::transparent_sorted_order.set_count(saved_count);
  c_visible_items::push_marker();
  render_objects_update_visible_subparts(user_index);
  c_object_renderer::submit_visibility(true);
  render_player_view_sort_transparents();
  c_transparency_renderer::render(true);
  c_visible_items::pop_marker();
  c_transparency_renderer::pop_marker();
  {
    s_render_object_globals *globals = g_render_object_globals;
    globals->marker_depth--;
    globals->object_count = globals->marker_object_counts[globals->marker_depth + 1];
    globals->mesh_part_count = globals->marker_mesh_part_counts[globals->marker_depth + 1];
  }
  s_implicit_geometry_globals *submit_list = (s_implicit_geometry_globals *)g_rasterizer_implicit_geometry_data;
  for (long index = 0; index < submit_list->count; index++) {
    s_implicit_geometry_primitive *entry = &submit_list->primitives[index];
    render_objects_add_transparent_element(&entry->transform.position, 0, 0.0f, _transparent_sort_layer_normal,
        implicit_geometry_render_primitive, entry, -1, 0.0f);
  }
  effects_render_pass(view->view_index, 1);
  render_player_view_sort_transparents();
  c_transparency_renderer::render(true);
  bool using_albedo_sampler = rasterizer_is_surface_resolved((c_rasterizer::e_surface)17);
  g_rasterizer_using_albedo_sampler = using_albedo_sampler;
  int sampler_value = (using_albedo_sampler == 0);
  D3DDevice_SetPixelShaderConstantB(g_rasterizer_device, 0, &sampler_value, 1);
}


// @address 0x737504 g_tiled_render_x_scale
// @address 0x737508 g_tiled_render_y_scale


static __forceinline void render_objects_set_pending_mask(D3DTAGCOLLECTION *pending, unsigned long index, unsigned long long mask)
{
    pending->m_Mask[index] |= mask;
}

__forceinline void D3DDevice::SetPixelShaderConstantF(unsigned int start_register, const float *constant_data, unsigned long count)
{
    if (__IsIntConst((int)count) && count == 1)
    {
        real_vector4d *constants = m_Constants.PixelShaderF;
        constants[start_register].x = constant_data[0];
        constants[start_register].y = constant_data[1];
        constants[start_register].z = constant_data[2];
        constants[start_register].w = constant_data[3];
        render_objects_set_pending_mask(&m_Pending, 1, 1ui64 << (63 - start_register / 4));
    }
    else
        D3DDevice_SetPixelShaderConstantFN(this, start_register, constant_data, count,
            (~0ui64 >> (start_register / 4)) & (~0ui64 << (63 - (start_register + count - 1) / 4)));
}

// @address 0x469b78
void c_object_renderer::render_active_camo_object_context(const void *context, long active_camo_value)
{
    const long context_index = static_cast<long>(reinterpret_cast<intptr_t>(context));
    if (g_active_camo_resolve_pending)
    {
        c_rasterizer::g_HDR_alpha_blend = false;
        c_rasterizer::resolve_surface((c_rasterizer::e_surface)13, 0, &g_active_camo_resolve_bounds, g_active_camo_resolve_bounds.x0,
            g_active_camo_resolve_bounds.y0);
        g_active_camo_resolve_pending = false;
    }
    s_render_object_globals *globals = g_render_object_globals;
    const s_object_render_context *object_context = &globals->contexts[context_index];
    g_rasterizer_device->SetPixelShaderConstantF(233, (const float *)&g_active_camo_texture_bounds, 1);
    render_objects_set_lighting_constants(object_context->lighting, 0, 5);
    D3DDevice_SetRenderState_ColorWriteEnable(g_rasterizer_device, 15);
    D3DDevice_SetRenderState_ColorWriteEnable1(g_rasterizer_device, 15);
    if (active_camo_value < 255)
    {
        c_rasterizer::set_z_buffer_mode(c_rasterizer::_z_buffer_mode_normal);
        render_objects_render_object_context(context_index, 8, 4, 1);
        c_rasterizer::set_z_buffer_mode(c_rasterizer::_z_buffer_mode_read_only);
    }
    else
        c_rasterizer::set_z_buffer_mode(c_rasterizer::_z_buffer_mode_normal);
    real_vector4d constants;
    constants.x = (float)active_camo_value / 255.0f;
    constants.y = g_tiled_render_x_scale;
    constants.z = g_tiled_render_y_scale;
    constants.w = 0.0f;
    g_rasterizer_device->SetPixelShaderConstantF(232, (const float *)&constants, 1);
    if (g_rasterizer_alpha_blend_mode != 3)
    {
        D3DDevice *device = g_rasterizer_device;
        g_rasterizer_alpha_blend_mode = 3;
        D3DDevice_SetRenderState_AlphaBlendEnable(device, 1);
        D3DDevice_SetRenderState_BlendOp(device, 0);
        D3DDevice_SetRenderState_SrcBlend(device, 6);
        D3DDevice_SetRenderState_DestBlend(device, 7);
    }
    render_objects_render_object_context(context_index, 12, 4, 0);
    c_rasterizer::set_z_buffer_mode(c_rasterizer::_z_buffer_mode_read_only);
}

// 0x165998 support (kept after all accepted bodies so sibling numbering never shifts)

static __forceinline void *render_objects_object_header_block_get_with_count(
    long object_index, const s_object_block_reference *reference, unsigned int element_size, long *count)
{
    object_datum *object = object_get(object_index);
    *count = reference->size / element_size;
    return (char *)object + reference->offset;
}

static __forceinline long render_objects_object_definition_index(const object_datum *object)
{
    return object->definition_index;
}

static __forceinline const real_rgb_color *render_objects_attachment_white()
{
    return (const real_rgb_color *)&objects_get_default_argb_white()->red;
}

static __forceinline void render_objects_widgets_object_submit_transparents(long object_index)
{
    object_datum *object = object_get(object_index);
    long widget_index = object->first_widget_index;
    while (widget_index != -1)
    {
        s_widget_datum *widget = &((s_widget_datum *)g_widget_data->data)[(unsigned short)widget_index];
        widget_submit_transparent(widget_index);
        widget_index = widget->next_widget;
    }
}

// @address 0x165998 ?submit_object_attachments@c_object_renderer@@SAXJJ@Z
void c_object_renderer::submit_object_attachments(long user_index, long object_index)
{
    object_datum *object = object_get(object_index);
    s_object_definition *definition = (s_object_definition *)tag_get(0, object->definition_index);
    long attachment_count = object->attached_lights.size / sizeof(s_object_datum_attachment);
    s_object_datum_attachment *datum_attachments = (s_object_datum_attachment *)((char *)object + object->attached_lights.offset);
    object_marker markers[16];
    real_rgb_color color;
    float function_value;
    bool deterministic1;
    bool deterministic2;
    for (long attachment_index = 0; attachment_index < attachment_count && attachment_index < definition->attachments.count; attachment_index++)
    {
        s_object_attachment_definition *attachment = &((s_object_attachment_definition *)definition->attachments.address)[attachment_index];
        s_object_datum_attachment *datum_attachment = &datum_attachments[attachment_index];
        if (datum_attachment->type != -1 && datum_attachment->type == 3)
        {
            bool first_person = render_object_is_first_person(user_index, object_index);
            float intensity = 1.0f;
            color = *render_objects_attachment_white();
            long marker_count = 0;
            if (!first_person)
            {
                if (attachment->marker == 0 || attachment->marker == 489)
                {
                    marker_count = 1;
                    memcpy(&markers[0].matrix, object_get_node_matrix(object_index, 0), sizeof(real_matrix4x3));
                }
                else
                {
                    marker_count = object_get_markers_by_string_id(object_index, attachment->marker, markers, 16);
                }
            }
            else if (object->parent_object_index == -1)
            {
                marker_count = first_person_get_markers_by_string_id(user_index, attachment->marker, markers, 16);
            }
            if (attachment->primary_scale != 0 && attachment->primary_scale != -1)
            {
                bool valid = object_get_function_value(
                    object_index, attachment->primary_scale, render_objects_object_definition_index(object), &function_value, &deterministic1);
                intensity *= valid ? function_value : 0.0f;
            }
            if (attachment->secondary_scale != 0 && attachment->secondary_scale != -1)
            {
                bool valid = object_get_function_value(
                    object_index, attachment->secondary_scale, render_objects_object_definition_index(object), &function_value, &deterministic2);
                intensity *= valid ? function_value : 0.0f;
            }
            if (attachment->change_color != 0)
                object_get_change_color(object_index, attachment->change_color, &color);
            if (intensity > 0.0f && attachment->type.group_tag == 'lens')
            {
                for (long marker_index = 0; marker_index < marker_count; marker_index++)
                {
                    lens_flares_create_internal(attachment->type.index, _lens_flare_attachment_type_object, object_index,
                        (unsigned short)(marker_index + attachment_index * 16),
                        &markers[marker_index].matrix.position, &markers[marker_index].matrix.rotation.forward, &color, 0.0f,
                            intensity, markers[marker_index].matrix.scale, first_person, user_index);
                }
            }
        }
    }
    render_objects_widgets_object_submit_transparents(object_index);
}

// @address 0x8c48bc g_scene_texture_flags
unsigned long g_scene_texture_flags;

static __forceinline unsigned long render_objects_get_uses_scene_texture(long index)
{
    if (index != -1)
        return (c_render_method::get(index)->flags >> 2) & 1;
    return 0;
}
// @address 0x468fd0 render_objects_submit_active_camo_parts
extern "C" void render_objects_submit_active_camo_parts(
    s_object_render_context *context, s_render_object_info *object_info, unsigned char transparent, unsigned char lighting_flag)
{
    const render_model_definition *geometry =
        (const render_model_definition *)tag_get(0, object_info->definition_index);
    const s_render_geometry *render_geometry = &geometry->render_geometry;
    if (geometry->materials.count > 0) {
        void *lighting;
        if (transparent)
            lighting = render_objects_get_lighting_definition(object_get(object_info->object_index)->definition_index, lighting_flag);
        const geometry_material *index_entries = (geometry_material *)geometry->materials.address;
        long mesh_slot = 0;
        if (object_info->mesh_count > 0) {
            do {
                long mesh_index = object_info->mesh_indices[mesh_slot];
                if (mesh_index != 255) {
                    s_mesh *mesh = &((s_mesh *)render_geometry->meshes.address)[mesh_index];
                    for (long part_index = 0; part_index < mesh->parts.count; part_index++) {
                        s_part *part = &static_cast<s_part *>(mesh->parts.address)[part_index];
                        long entry_index = part->material_index;
                        if (entry_index != -1) {
                            long part_tag_index = index_entries[entry_index].render_method.index;
                            if (part_tag_index != -1) {
                                const c_render_method *part_tag =
                                    (const c_render_method *)tag_get(0, part_tag_index);
                                g_scene_texture_flags |= (part_tag->flags >> 2) & 1;
                                unsigned long *data = object_info->data;
                                long mesh_part = c_object_renderer::add_context_mesh_part(&context->mesh_parts, object_info, data,
                                                              mesh_index, part_index, mesh_slot, entry_index, 6);
#if !defined(_XBOX) || defined(__clang__)
                                if (mesh_part == -1)
                                    continue;
#endif
                                g_active_camo_capture_pending = true;
                                if (transparent) {
                                    real_point3d position = *global_origin3d;
                                    if (part->transparent_sorting_index != -1 && (mesh->flags & 2) == 0)
                                        position =
                                            ((const s_transparent_sorting_node *)render_geometry->part_sorting_position.address)
                                                [part->transparent_sorting_index].position;
                                    c_object_renderer::add_transparent_mesh_part(
                                        (const s_render_geometry *)render_geometry, (const s_mesh *)mesh, (const s_part *)part,
                                        mesh_slot, object_info->skinning_memory_designator, context, mesh_part, &position, false, 3,
                                        (void (*)(const void *, long))render_objects_render_shield_impact);
                                    g_render_object_globals->mesh_parts[mesh_part].shield_impact = (const s_shield_impact_definition *)lighting;
                                }
                            }
                        }
                    }
                }
                mesh_slot++;
            } while (mesh_slot < object_info->mesh_count);
        }
    }
}

// @address 0x24b5c k_render_object_type_flag_16
#if !defined(_XBOX) || defined(__clang__)
const bool k_render_object_type_flag_16[16] = {
    true, true, true, false, false, false, false, false, false, false, false, true, true, false, false, false,
};
#endif

static __forceinline bool render_objects_submit_part_is_valid(
    const s_part *part,
    const geometry_material *materials)
{
    return part->type != 5 && part->material_index != -1 &&
        materials[part->material_index].render_method.index != -1;
}

// @address 0x163df0 render_objects_submit_object_mesh_parts
extern "C" void render_objects_submit_object_mesh_parts(s_object_render_context *context,
    s_render_object_info * object_info, long flags,
    bool submit_transparent, bool shield_impact, unsigned char lighting_flag)
{
    const render_model_definition *render_model =
        (const render_model_definition *)tag_get(0, object_info->definition_index);
    const s_render_geometry *render_geometry = &render_model->render_geometry;
    if (render_model->materials.count > 0) {
        const geometry_material *materials = (geometry_material *)render_model->materials.address;
        bool first_person = (flags & 64) != 0;
        object_datum *object = object_get(object_info->object_index);
        if (k_render_object_type_flag_16[object->object_type])
            flags |= 16;
        void *lighting;
        if (shield_impact)
            lighting = render_objects_get_lighting_definition(object->definition_index, lighting_flag);
        for (long mesh_slot = 0; mesh_slot < object_info->mesh_count; mesh_slot++) {
            long mesh_index = object_info->mesh_indices[mesh_slot];
            if (mesh_index != 255) {
                s_mesh *mesh = &((s_mesh *)render_geometry->meshes.address)[mesh_index];
                for (long part_index = 0; part_index < mesh->parts.count; part_index++) {
                    s_part *part = &static_cast<s_part *>(mesh->parts.address)[part_index];
                    if (render_objects_submit_part_is_valid(part, materials)) {
                        if (part->material_index != -1 && materials[part->material_index].render_method.index != -1) {
                            bool is_transparent = c_render_method::get_is_transparent(materials[part->material_index].render_method.index);
                            bool is_decal = c_render_method::get_is_decal(materials[part->material_index].render_method.index);
                            const unsigned long &uses_scene_texture = render_objects_get_uses_scene_texture(
                                materials[part->material_index].render_method.index);
                            g_scene_texture_flags |= uses_scene_texture;
                            is_transparent &= !is_decal;
                            long part_flags = is_transparent ? 4 : flags;
                            if (is_decal) {
                                part_flags &= ~1;
                                part_flags |= 32;
                            }
                            long context_part = c_object_renderer::add_context_mesh_part(&context->mesh_parts, object_info, object_info->data,
                                mesh_index, part_index, mesh_slot, part->material_index, part_flags);
#if !defined(_XBOX) || defined(__clang__)
                            if (context_part == -1)
                                continue;
#endif
                            if (submit_transparent && (flags & 1) && (is_transparent || shield_impact)) {
                                real_point3d position = *global_origin3d;
                                if (part->transparent_sorting_index != -1 && (mesh->flags & 2) == 0)
                                    position =
                                            ((const s_transparent_sorting_node *)render_geometry->part_sorting_position.address)
                                                [part->transparent_sorting_index].position;
                                if (is_transparent) {
                                    unsigned char sort_layer =
                                        ((const c_render_method *)
                                                tag_get(0, materials[part->material_index].render_method.index))
                                            ->sort_layer;
                                    c_object_renderer::add_transparent_mesh_part(
                                        (const s_render_geometry *)render_geometry, (const s_mesh *)mesh, (const s_part *)part,
                                        mesh_slot, object_info->skinning_memory_designator, context, context_part, &position,
                                            first_person, sort_layer,
                                        (void (*)(const void *, long))render_objects_render_transparent_part);
                                }
                                if (shield_impact) {
                                    c_object_renderer::add_transparent_mesh_part(
                                        (const s_render_geometry *)render_geometry, (const s_mesh *)mesh, (const s_part *)part,
                                        mesh_slot, object_info->skinning_memory_designator, context, context_part, &position, false, 3,
                                        (void (*)(const void *, long))render_objects_render_shield_impact);
                                    g_render_object_globals->mesh_parts[context_part].shield_impact = (const s_shield_impact_definition *)lighting;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

// @address 0x163b78 render_objects_submit_primary_hierarchy
extern "C" void render_objects_submit_primary_hierarchy(long *visibility_object_index, long *object_count,
    unsigned long flags, s_object_render_context *context, long inherited_active_camo,
    unsigned long submit_flags, unsigned long *special_contexts)
{
    s_object_visibility *visibility = &c_visible_items::m_items.objects.entries[*visibility_object_index];
    s_render_object_info *object_info = (s_render_object_info *)visibility;
    object_datum *object = object_get(object_info->object_index);
    bool submit_transparent = (submit_flags & 1) != 0;
    unsigned char lighting_flag = (submit_flags >> 3) & 1;
    bool shield_impact = submit_transparent && (submit_flags & 4) == 0 &&
        (((visibility->render_setup.flags & 8) != 0 && ((s_shader_extern_info *)object_info->shader_extern_info)->shield_damage) ||
            ((s_shader_extern_info *)object_info->shader_extern_info)->shield);
    long active_camo_value = inherited_active_camo;
    if (((s_shader_extern_info *)object_info->shader_extern_info)->camouflage != 0)
        active_camo_value = ((s_shader_extern_info *)object_info->shader_extern_info)->camouflage;
    if ((visibility->render_setup.flags & 4) != 0 || (submit_flags & 2) != 0)
        active_camo_value = 0;
    bool submit_mesh_parts = false;
    if ((visibility->render_setup.flags & 1) != 0)
    {
        if (active_camo_value > 0)
        {
            submit_mesh_parts = false;
            if (submit_transparent)
            {
                const unsigned __int64 &context_flags = *special_contexts | 1;
                *special_contexts = (unsigned long)context_flags;
            }
        }
        else if ((submit_flags & 4) == 0 &&
            ((s_shader_extern_info *)object_info->shader_extern_info)->instance_index != -1)
        {
            submit_mesh_parts = false;
            *special_contexts |= 2;
        }
        else
        {
            if (((s_shader_extern_info *)object_info->shader_extern_info)->override_material)
            {
                submit_mesh_parts = true;
                flags = (flags & ~1) | 128;
            }
            else
            {
                submit_mesh_parts = true;
            }
        }
        if (shield_impact)
        {
            submit_mesh_parts = false;
            *special_contexts |= 4;
        }
        if ((visibility->render_setup.flags & 16) != 0)
        {
            if ((visibility->render_setup.flags & 32) != 0)
                flags = (flags & ~0x300) | 0x100;
            else
                flags = (flags & ~0x300) | 0x200;
        }
    }
    if (submit_mesh_parts)
    {
        render_objects_submit_object_mesh_parts(context, object_info, flags, submit_transparent, false, lighting_flag);
        if (context->submit_instances)
            render_objects_add_instance_mesh_part(&context->mesh_parts, object_info, flags);
    }
    long child_object_index = object->first_child_object_index;
    ++*visibility_object_index;
    --*object_count;
    while (child_object_index != -1 && *object_count > 0)
    {
        if (c_visible_items::m_items.objects.count > *visibility_object_index)
            render_objects_submit_primary_hierarchy(visibility_object_index, object_count, flags, context,
                active_camo_value, submit_flags, special_contexts);
        child_object_index = object_get(child_object_index)->next_object_index;
    }
}

// @address 0x469290 render_objects_submit_special_hierarchy
extern "C" void render_objects_submit_special_hierarchy(long *visibility_object_index, long *object_count,
    unsigned long flags, const render_lighting *inherited_lighting,
    long context_parameters, long inherited_active_camo, const real_point3d *sort_point,
    unsigned long submit_flags)
{
    s_object_visibility *visibility = &c_visible_items::m_items.objects.entries[*visibility_object_index];
    s_render_object_info *object_info = (s_render_object_info *)visibility;
    object_datum *object = object_get(object_info->object_index);
    const render_lighting *lighting =
        ((s_shader_extern_info *)object_info->shader_extern_info)->instance_index != -1 ?
            &((s_shader_extern_info *)object_info->shader_extern_info)->lighting :
            inherited_lighting;
    bool submit_transparent = (submit_flags & 1) != 0;
    unsigned char lighting_flag = (submit_flags >> 3) & 1;
    bool shield_impact = submit_transparent && (submit_flags & 4) == 0 &&
        (((visibility->render_setup.flags & 8) != 0 && ((s_shader_extern_info *)object_info->shader_extern_info)->shield_damage) ||
            ((s_shader_extern_info *)object_info->shader_extern_info)->shield);
    long active_camo_value = inherited_active_camo;
    if (((s_shader_extern_info *)object_info->shader_extern_info)->camouflage != 0)
        active_camo_value = ((s_shader_extern_info *)object_info->shader_extern_info)->camouflage;
    if ((visibility->render_setup.flags & 4) != 0 || (submit_flags & 2) != 0)
        active_camo_value = 0;
    s_object_render_context *context = 0;
    if ((visibility->render_setup.flags & 1) != 0)
    {
        if ((visibility->render_setup.flags & 16) != 0)
        {
            if ((visibility->render_setup.flags & 32) != 0)
                flags = (flags & ~0x300) | 0x100;
            else
                flags = (flags & ~0x300) | 0x200;
        }
        if (active_camo_value > 0)
        {
            if (submit_transparent)
            {
                long context_index = c_object_renderer::add_object_render_context();
                if (context_index != -1)
                {
                    context = &g_render_object_globals->contexts[context_index];
                    context->lighting = lighting;
                    context->parameters = context_parameters;
                    context->shader_extern_info = (const s_shader_extern_info *)object_info->shader_extern_info;
                    context->mesh_parts.flag = render_objects_is_first_person_object(object_info->object_index);
                    if (sort_point == 0)
                        sort_point = &object_get(object_info->object_index)->bounding_center;
                    render_objects_add_transparent_element(sort_point, 0, 0.0f, _transparent_sort_layer_normal,
                        c_object_renderer::render_active_camo_object_context,
                        reinterpret_cast<const void *>(static_cast<intptr_t>(context_index)), active_camo_value, 0.0f);
                    render_objects_submit_active_camo_parts(context, object_info, shield_impact, lighting_flag);
                    if (context->submit_instances)
                        render_objects_add_instance_mesh_part(&context->mesh_parts, object_info, 6);
                }
                g_render_object_globals->contexts_added = false;
            }
        }
        else if (((submit_flags & 4) == 0 &&
            ((s_shader_extern_info *)object_info->shader_extern_info)->instance_index != -1) || shield_impact)
        {
            long context_index = c_object_renderer::add_object_render_context();
            if (context_index != -1)
            {
                context = &g_render_object_globals->contexts[context_index];
                context->lighting = lighting;
                context->parameters = context_parameters;
                context->shader_extern_info = (const s_shader_extern_info *)object_info->shader_extern_info;
                context->mesh_parts.flag = render_objects_is_first_person_object(object_info->object_index);
                render_objects_submit_object_mesh_parts(context, object_info, flags, submit_transparent, shield_impact, lighting_flag);
                if (context->submit_instances)
                    render_objects_add_instance_mesh_part(&context->mesh_parts, object_info, flags);
            }
            g_render_object_globals->contexts_added = false;
        }
    }
    long child_object_index = object->first_child_object_index;
    ++*visibility_object_index;
    --*object_count;
    while (child_object_index != -1 && *object_count > 0)
    {
        if (c_visible_items::m_items.objects.count > *visibility_object_index)
            render_objects_submit_special_hierarchy(visibility_object_index, object_count, flags, inherited_lighting,
                context_parameters, active_camo_value, sort_point, submit_flags);
        child_object_index = object_get(child_object_index)->next_object_index;
    }
}

// @address 0x1639e0 render_objects_submit_visible_hierarchy
extern "C" void render_objects_submit_visible_hierarchy(long first_object, long object_count, long context_parameters,
    unsigned long flags, unsigned long submit_flags)
{
    s_render_object_info *root_info = (s_render_object_info *)&c_visible_items::m_items.objects.entries[first_object];
    long visibility_object_index = first_object;
    long remaining_count = object_count;
    unsigned long object_flags;
    if (object_get(root_info->object_index)->object_identifier.get_source() == 4)
        object_flags = flags | 64;
    else
        object_flags = flags & ~64;
    while (remaining_count > 0)
    {
        long current_index = visibility_object_index;
        long current_count = remaining_count;
        unsigned long special_contexts = 0;
        s_render_object_info *object_info = (s_render_object_info *)&c_visible_items::m_items.objects.entries[visibility_object_index];
        const render_lighting *lighting =
            &((s_shader_extern_info *)object_info->shader_extern_info)->lighting;
        long context_index = render_objects_add_object_render_context();
        if (context_index == -1)
        {
            g_render_object_globals->contexts_added = false;
            return;
        }
        s_object_render_context *context = &g_render_object_globals->contexts[context_index];
        context->lighting = lighting;
        context->parameters = context_parameters;
        context->mesh_parts.flag = render_objects_is_first_person_object(root_info->object_index);
        render_objects_submit_primary_hierarchy(&current_index, &current_count, object_flags, context, 0, submit_flags, &special_contexts);
        g_render_object_globals->contexts_added = false;
        if (special_contexts != 0)
        {
            current_index = visibility_object_index;
            current_count = remaining_count;
            render_objects_submit_special_hierarchy(&current_index, &current_count, object_flags, lighting, context_parameters, 0, 0, submit_flags);
        }
        visibility_object_index = current_index;
        remaining_count = current_count;
    }
}

static __forceinline void render_objects_shield_color_constant(const s_shield_impact_definition *source, long color_index,
    real_vector4d *constant)
{
    constant->i = source->colors[color_index].color.red * source->colors[color_index].intensity;
    constant->j = source->colors[color_index].color.green * source->colors[color_index].intensity;
    constant->k = source->colors[color_index].color.blue * source->colors[color_index].intensity;
    constant->l = 0.0f;
}
struct s_decals_render_container;

static __forceinline void render_objects_shield_time_constant(const s_object_render_context *context, real_vector4d *constant)
{
    constant->i = game_time_get_safe_in_seconds();
    constant->j = context->shader_extern_info->shield_damage * (1.0f / 255.0f);
    constant->k = context->shader_extern_info->shield * (1.0f / 51.0f);
}

// @address 0x4696b0 render_objects_render_shield_impact
extern "C" void render_objects_render_shield_impact(const void *user_data, long user_context)
{
    s_render_object_globals *globals = g_render_object_globals;
    const void *const *user_data_reference = &user_data;
    const s_object_render_context *context = (const s_object_render_context *)*user_data_reference;
    rasterizer_set_shadow_stencil_ref(context->stencil_index, 0, 1, 0);
    s_context_mesh_part *part = &globals->mesh_parts[user_context];
    const s_shield_impact_definition *shield;
    const s_shield_impact_definition *const *shield_override = &part->shield_impact;
    if (part->shield_impact)
        shield = *shield_override;
    else
    {
        const c_rasterizer_globals *rasterizer_globals =
            global_game_globals->rasterizer_globals_ref.index != -1 ?
            (const c_rasterizer_globals *)tag_get('rasg', global_game_globals->rasterizer_globals_ref.index) : 0;
        long shield_impact_index = rasterizer_globals->m_default_shield_impact.index;
        shield = shield_impact_index != -1 ?
            (const s_shield_impact_definition *)global_tag_instances[datum_index_to_absolute_index(shield_impact_index)].definition : 0;
    }
    if (shield)
    {
        render_model_definition *model =
            (render_model_definition *)tag_get(0, part->object_info->definition_index);
        s_render_geometry *geometry = &model->render_geometry;
        s_mesh *mesh = &((s_mesh *)geometry->meshes.address)[part->mesh_index];
        c_object_renderer::render_object_context_mesh_part(part, _entry_point_default);
        c_rasterizer::set_explicit_shaders(61, static_cast<e_vertex_type>(mesh->vertex_type), _transfer_vector_vertex_none, _entry_point_default);
        render_method_submit_set_texture(0, shield->noise_texture1.index, 0);
        c_rasterizer::set_sampler_address_mode(
            0, c_rasterizer::_sampler_address_wrap, c_rasterizer::_sampler_address_wrap, c_rasterizer::_sampler_address_wrap);
        if (g_rasterizer_sampler_filter_modes[0] != 2)
        {
            g_rasterizer_sampler_filter_modes[0] = 2;
            rasterizer_device_set_sampler_filter_modes(g_rasterizer_device, 0, 2, 2);
        }
        render_method_submit_set_texture(1, shield->noise_texture2.index, 0);
        c_rasterizer::set_sampler_address_mode(
            1, c_rasterizer::_sampler_address_wrap, c_rasterizer::_sampler_address_wrap, c_rasterizer::_sampler_address_wrap);
        if (g_rasterizer_sampler_filter_modes[1] != 2)
        {
            g_rasterizer_sampler_filter_modes[1] = 2;
            rasterizer_device_set_sampler_filter_modes(g_rasterizer_device, 1, 2, 2);
        }
        if (g_rasterizer_alpha_blend_mode != 1)
        {
            g_rasterizer_alpha_blend_mode = 1;
            D3DDevice *device = g_rasterizer_device;
            D3DDevice_SetRenderState_AlphaBlendEnable(device, 1);
            D3DDevice_SetRenderState_BlendOp(device, 0);
            D3DDevice_SetRenderState_SrcBlend(device, 1);
            D3DDevice_SetRenderState_DestBlend(device, 1);
        }
        real_vector4d time_constants;
        render_objects_shield_time_constant(context, &time_constants);
        c_rasterizer::set_pixel_shader_constant(101, 1, &time_constants);
        c_rasterizer::set_pixel_shader_constant(100, 1, &context->shader_extern_info->bounding_sphere);
        real_vector4d noise_constants;
        noise_constants.i = shield->noise_scale.i;
        noise_constants.j = shield->noise_scale.j;
        noise_constants.k = 0.0f;
        noise_constants.l = 0.0f;
        c_rasterizer::set_pixel_shader_constant(102, 1, &noise_constants);
        real_vector4d offset_constants;
        offset_constants.i = shield->noise_offset1.i;
        offset_constants.j = shield->noise_offset1.j;
        offset_constants.k = shield->noise_offset1.k;
        c_rasterizer::set_pixel_shader_constant(103, 1, &offset_constants);
        offset_constants.i = shield->noise_offset2.i;
        offset_constants.j = shield->noise_offset2.j;
        offset_constants.k = shield->noise_offset2.k;
        c_rasterizer::set_pixel_shader_constant(104, 1, &offset_constants);
        real_vector4d color_constants;
        render_objects_shield_color_constant(shield, 0, &color_constants);
        c_rasterizer::set_pixel_shader_constant(105, 1, &color_constants);
        render_objects_shield_color_constant(shield, 1, &color_constants);
        c_rasterizer::set_pixel_shader_constant(106, 1, &color_constants);
        render_objects_shield_color_constant(shield, 2, &color_constants);
        c_rasterizer::set_pixel_shader_constant(107, 1, &color_constants);
        render_objects_shield_color_constant(shield, 3, &color_constants);
        c_rasterizer::set_pixel_shader_constant(108, 1, &color_constants);
        render_objects_shield_color_constant(shield, 4, &color_constants);
        c_rasterizer::set_pixel_shader_constant(109, 1, &color_constants);
        render_objects_shield_color_constant(shield, 5, &color_constants);
        c_rasterizer::set_pixel_shader_constant(110, 1, &color_constants);
        real_vector4d vertex_constants;
        vertex_constants.i = shield->vertex_scale;
        c_rasterizer::set_vertex_shader_constant(228, 1, &vertex_constants);
        render_mesh_part_no_material(geometry, part->mesh_index, part->part_index, part->data);
    }
    if (g_render_static_lighting_active == 0)
        c_rasterizer::set_stencil_mode(c_rasterizer::_stencil_mode_off);
}

static __forceinline void render_objects_instance_skinning_matrix(
    float scale, const real_matrix4x3 *matrix, float forward_i, s_model_skinning_matrix *skinning_matrix)
{
    skinning_matrix->rows[0][0] = scale * forward_i;
    skinning_matrix->rows[0][1] = matrix->left.i * scale;
    skinning_matrix->rows[0][2] = matrix->up.i * scale;
    skinning_matrix->rows[1][0] = matrix->forward.j * scale;
    skinning_matrix->rows[1][1] = matrix->left.j * scale;
    skinning_matrix->rows[1][2] = matrix->up.j * scale;
    skinning_matrix->rows[2][0] = matrix->forward.k * scale;
    skinning_matrix->rows[2][1] = matrix->left.k * scale;
    skinning_matrix->rows[2][2] = matrix->up.k * scale;
    skinning_matrix->rows[0][3] = matrix->position.x;
    skinning_matrix->rows[1][3] = matrix->position.y;
    skinning_matrix->rows[2][3] = matrix->position.z;
}

static __forceinline bool render_objects_instance_part_valid(const s_part *part, const geometry_material *materials)
{
    return part->type != 5 && part->material_index != -1 && materials[part->material_index].render_method.index != -1;
}

// @address 0x1741d0
void c_object_renderer::render_object_context_instances(
    s_context_mesh_part *context_part, e_entry_point entry_point, unsigned long *instance_mask, long stencil_index, long flags)
{
    render_model_definition *model =
        (render_model_definition *)
            tag_get(0, context_part->object_info->definition_index);
    if (model->instance_starting_mesh_index != -1)
    {
        s_render_geometry *geometry = &model->render_geometry;
        s_mesh *mesh = &((s_mesh *)geometry->meshes.address)[model->instance_starting_mesh_index];
        if (entry_point == 8)
            entry_point = _entry_point_static_sh;
        render_mesh_submit_compression((const s_compression_info *)geometry->compression_info.address);
        render_method_submit_set_extern_info(31 - _CountLeadingZeros(-((long)g_active_structure_bsp_mask) & ((long)g_active_structure_bsp_mask)),
            context_part->object_info->shader_extern_info);
        long instance_count = mesh->subparts.count < model->instance_placements.count ? mesh->subparts.count : model->instance_placements.count;
        for (long instance_index = 0; instance_index < instance_count; instance_index++)
        {
            if ((instance_mask[instance_index >> 5] & (1 << (instance_index & 31))) != 0)
            {
                const s_subpart *subpart = &static_cast<s_subpart *>(mesh->subparts.address)[instance_index];
                s_part *part = &static_cast<s_part *>(mesh->parts.address)[subpart->part_index];
                const render_model_instance_placement *instance =
                    &static_cast<const render_model_instance_placement *>(model->instance_placements.address)[instance_index];
                real_matrix4x3 node_matrix;
                real_matrix4x3 instance_matrix;
                s_model_skinning_matrix skinning_matrix;
                model_skinning_get_node_matrix(
                    render_objects_get_skinning(context_part->object_info->skinning_memory_designator),
                    instance->node_index, &node_matrix);
                matrix4x3_compute_product(&node_matrix, &instance->matrix, &instance_matrix);
                render_objects_instance_skinning_matrix(instance_matrix.scale, &instance_matrix, instance_matrix.forward.i, &skinning_matrix);
                render_mesh_submit_skinning(&skinning_matrix, 1);
                unsigned long subpart_mask[2];
                subpart_mask[1] = 0;
                subpart_mask[0] = 0;
                long part_subpart = instance_index - part->first_subpart;
                subpart_mask[part_subpart >> 5] |= 1 << (part_subpart & 31);
                if (instance_matrix.scale < 0.0f)
                    D3DDevice_SetRenderState_CullMode(g_rasterizer_device, 6);
                if (model->materials.count > 0)
                {
                    geometry_material *materials = (geometry_material *)model->materials.address;
                    if (render_objects_instance_part_valid(part, materials))
                    {
                        unsigned __int64 high =
                            ((unsigned __int64)(short)context_part->part_index << 16) + (unsigned __int64)(short)context_part->mesh_index;
                        const int geometry_address = static_cast<int>(address_from_pointer(geometry));
                        unsigned __int64 key = (high << 32) + (unsigned __int64)geometry_address;
                        render_method_submit_draw_mesh_part(key, geometry, mesh, part, entry_point, subpart_mask, materials);
                    }
                }
                if (instance_matrix.scale < 0.0f)
                    D3DDevice_SetRenderState_CullMode(g_rasterizer_device, 2);
            }
        }
    }
    c_render_method_data::m_valid = false;
    render_method_submit_valid();
}

struct s_decals_cluster_request;
struct render_lights_state;
// @address 0x72cd37 g_main_render_fence_active
// @address 0x72cdcb g_water_interaction_enabled
// @address 0x73a15c g_render_method_submit_cached_definition_index
// @address 0x73a184 g_water_interaction_last_time

// @address 0x745cd8 g_decal_distance_inverse
// @address 0x825d8c g_water_camera_in_water
// @address 0x832a84 g_main_render_gpu_fence

// @address 0x8c37d8 g_decal_preplaced_render_counts
// @address 0x8c3fe0 g_decal_runtime_render_counts
// @address 0x8c46b8 g_antialiasing_mode

// @address 0x8c46e5 g_render_water_flag
// @address 0x8c46fc g_depth_render_mode
long g_depth_render_mode;


static __forceinline bool render_objects_water_ripples_active()
{
    return g_water_interaction_last_time != -1 && game_time_globals->time - g_water_interaction_last_time < 300;
}
__forceinline void c_lights_view::build_simple_light_draw_list(long window, const float &scale)
{
    // match: 0x164540: const reference binding preserves the user-index load order.
    const c_lights_view *const &self = this;
    // match: 0x164540: const reference binding preserves the scale reload.
    const float *const &scale_address = &scale;
    render_lights_iterate_lights(this, self->user_index, window,
        g_player_render_tuning.simple_lighting_pass,
        false, true, *scale_address, render_lights_add_simple_light);
}
static __forceinline void render_objects_build_lighting(
    c_lights_view *data, long user, long window, D3DSurface *surface0, D3DSurface *surface1, D3DSurface *surface2)
{
    c_lights_view *lights = data;
    lights->render_target0 = surface0;
    lights->render_target1 = surface1;
    lights->user_index = user;
    lights->depth_stencil = surface2;
    render_lights_iterate_lights(lights, user, window, g_player_render_tuning.lighting_pass, true, false, data->exposure,
        reinterpret_cast<void (*)(int, int, float, void *)>(&render_lights_render_light));
}
static __forceinline D3DSurface *render_objects_get_hdr_surface()
{
    c_rasterizer::g_HDR_alpha_blend = true;
    return g_rasterizer_surface_groups[13].get_specialized_surface((c_rasterizer::e_surface)13)->surface;
}
static __forceinline void render_objects_set_hdr_alpha_blend(bool enabled)
{
    c_rasterizer::g_HDR_alpha_blend = enabled;
}
static __forceinline void render_objects_set_default_atmosphere_constants(s_weighted_atmosphere_parameters *parameters)
{
    memcpy(&c_atmosphere_fog_interface::m_default_parameters, parameters, sizeof(*parameters));
    atmosphere_constants_index = -2;
    c_atmosphere_fog_interface::set_constant(&c_atmosphere_fog_interface::m_default_parameters, true, true);
}
static __forceinline void render_objects_set_atmosphere_constants()
{
    if (atmosphere_constants_index != -2)
    {
        atmosphere_constants_index = -2;
        c_atmosphere_fog_interface::set_constant(&c_atmosphere_fog_interface::m_default_parameters, true, true);
    }
}
static __forceinline bool render_objects_allow_ui()
{
    const volatile s_rasterizer_tiling_state_header &tiling = *rasterizer_tiling_get_state_header();
    return !tiling.enabled || tiling.disabled;
}
static __forceinline bool render_objects_use_second_target()
{
    return c_screen_postprocess::x_editable_settings.resolve_hdr || rasterizer_tiling_get_state_header()->enabled;
}
static __forceinline bool render_objects_tiling_enabled()
{
    return rasterizer_tiling_get_state_header()->enabled;
}
static __forceinline void render_objects_clear_draw_counts()
{
    for (long index = 0; index < 2; ++index)
    {
        g_decal_runtime_render_counts[index] = 0;
        g_decal_preplaced_render_counts[index] = 0;
    }
}
static __forceinline const c_game_engine *get_active_game_engine()
{
    if (game_engine_globals->game_engine_index >= 1 && game_engine_globals->game_engine_index <= 10)
        return game_engines[game_engine_globals->game_engine_index];
    return 0;
}
static __forceinline void render_objects_render_game_engine_player(long player_index)
{
    const c_game_engine *(*const get_engine)() = current_game_engine;
    const c_game_engine *engine = get_engine();
    engine->render(player_index);
}
static __forceinline long get_mapped_player_index(long user_index)
{
    return player_mapping_globals->player_indices[user_index];
}
static __forceinline void render_objects_render_game_engine(long user_index)
{
    if (user_index != -1 && get_active_game_engine())
    {
        render_objects_render_game_engine_player(get_mapped_player_index(user_index));
    }
}
static __forceinline void render_objects_render_game_engine_debug(long user_index)
{
    if (game_in_progress() && user_index != -1)
    {
        const c_game_engine *engine = current_game_engine();
        if (engine)
            engine->render_debug(get_mapped_player_index(user_index));
    }
}
static __forceinline void render_objects_render_hud_damage(long user_index)
{
    if (chud_user_should_draw(user_index) && ((void *)chud_globals) && (unsigned long)user_index <= 3)
    {
        // match: 0x164540: const reference binding preserves the TLS user-data load.
        c_chud_persistent_user_data *const &alpha_users = chud_persistent_user_data();
        float alpha = alpha_users[user_index].m_cinematic_fade.get_fade();
        float fade = chud_get_scoreboard_fade(user_index) * chud_get_cinematic_fade() * alpha;
        if (chud_initialize_draw_state(user_index, fade, true, false))
        {
            // match: 0x164540: const reference binding preserves the damage-user address math.
            c_chud_persistent_user_data *const &damage_users = chud_persistent_user_data();
            const long &damage_offset = user_index * sizeof(c_chud_persistent_user_data);
            c_chud_persistent_user_data *damage_user = (c_chud_persistent_user_data *)((char *)damage_users + damage_offset);
            chud_draw_damage_indicators_and_curvature(damage_user->m_directional_damage.entries, user_index);
        }
    }
}
static __forceinline void end_tiling_if_enabled()
{
    if (c_rasterizer::get_is_tiling_enabled())
    {
        g_rasterizer_device->m_Pending.m_Mask[1] |= 0x1000000000ui64;
        D3DDevice_EndTiling(g_rasterizer_device, 0, 0, 0, 0, 1.0f, 0, 0);
        real_vector4d zero;
        zero.i = 0.0f;
        memset(&zero.j, 0, 3 * sizeof(float));
        g_rasterizer_device->SetPixelShaderConstantF(108, zero.n, 1);
    }
}
static __forceinline bool render_objects_get_render_water_flag()
{
    return g_rasterizer_distortion_visible;
}
static __forceinline void render_objects_set_render_water_flag(bool enabled)
{
    g_render_water_flag = enabled;
}
static __forceinline D3DDevice *render_objects_get_device()
{
    return g_rasterizer_device;
}

// @address 0x164540 ?render@c_player_view@@UAAXXZ
void c_player_view::render()
{
    g_antialiasing_mode = antialiasing_mode;
    g_render_method_submit_cached_definition_index = -1;
    s_cluster_reference starting_cluster;
    get_starting_cluster(&starting_cluster);
    const render_camera *camera = get_rasterizer_camera();
    render_objects_update_element_weights(starting_cluster, &camera->position);
    submit_attachments();
    bool water = false;
    water = c_water_renderer::update_water_part_list();
    if (water && g_water_interaction_enabled && render_objects_water_ripples_active())
    {
        c_water_renderer::ripple_apply();
        c_water_renderer::ripple_slope();
    }
    chud_draw_text_widgets(view_index);
    render_objects_screen_effect_settings screen_effect;
    render_objects_compute_screen_effect(&camera->position, &rasterizer_camera.forward, &screen_effect, view_index);
    setup_camera_fx_parameters(screen_effect.channels[0]);
    render_objects_update_clip_planes(this);
    long light_owner = view_index;
    c_lights_view *lights = &lights_view;
    lights->clear_simple_light_draw_list();
    lights->user_index = light_owner;
    lights->build_simple_light_draw_list(user_index, lights->exposure);
    s_weighted_atmosphere_parameters atmosphere;
    atmosphere.valid = false;
    atmosphere_constants_index = -1;
    memset(&atmosphere.sun_color, 0, sizeof(atmosphere) - offsetof(s_weighted_atmosphere_parameters, sun_color));
    c_atmosphere_fog_interface::populate_atmosphere_parameters(starting_cluster, &atmosphere);
    render_objects_set_default_atmosphere_constants(&atmosphere);
    render_objects_set_atmosphere_constants();
    rasterizer_set_default_render_targets();
    c_rasterizer::begin_tiling();
    rasterizer_stipple_initialize();
    render_objects_clear_draw_counts();
    if (g_player_render_tuning.decal_distance != 0.0f)
    {
        g_decal_distance_inverse = 1.0f / g_player_render_tuning.decal_distance;
        c_data_iterator<s_decals_cluster_request> iterator;
        iterator.m_iterator.absolute_index = -1;
        iterator.m_iterator.index = -1;
        iterator.m_iterator.data = (s_data_array *)g_decal_system_data;
        while (iterator.next())
            decals_add_visible_system_decals(iterator.get_datum());
    }
    bool albedo = render_albedo();
    if (albedo)
    {
        c_rasterizer::set_using_albedo_sampler(true);
        if (!c_rasterizer::get_is_tiling_enabled())
        {
            player_view_render_distortion(this);
            render_objects_set_render_water_flag(render_objects_get_render_water_flag() || water || g_water_camera_in_water != 0);
        }
        submit_occlusion_tests(false, true);
        if (last_view)
        {
            if (g_main_render_fence_active && !(D3DDevice_GetDeviceState(render_objects_get_device()) & 1) && g_main_render_gpu_fence_inserted)
                D3DDevice_BlockOnFence(g_main_render_gpu_fence);
            last_view = false;
        }
        render_static_lighting();
        player_view_render_lightmap_shadows(this);
        D3DSurface *surface0 = render_objects_get_hdr_surface();
        D3DSurface *surface1 = render_objects_get_hdr_surface();
        D3DSurface *surface2 = g_rasterizer_surface_groups[3].get_specialized_surface((c_rasterizer::e_surface)3)->surface;
        render_objects_build_lighting(&lights_view,
            view_index, user_index, surface0, surface1, surface2);
    }
    if (albedo)
    {
        c_rasterizer::setup_targets_static_lighting(exposure, exposure_multiplier, render_objects_use_second_target(),
            c_camera_fx_values::g_HDR_target_stops, false);
        if (g_depth_render_mode == 0)
            player_view_render_first_person_effects(this, false);
    }
    if (c_rasterizer::get_is_tiling_enabled())
        c_screen_postprocess::resolve_frame_buffers(&get_rasterizer_camera()->window_pixel_bounds);
    end_tiling_if_enabled();
    if (c_rasterizer::get_is_tiling_enabled())
    {
        player_view_render_distortion(this);
        render_objects_set_render_water_flag(render_objects_get_render_water_flag() || water || g_water_camera_in_water != 0);
        player_view_draw_static_lighting_composite(this);
    }
    if (albedo)
    {
        if (c_rasterizer::get_is_tiling_enabled())
            player_view_resolve_depth_of_field_source(user_index, &get_rasterizer_camera()->window_pixel_bounds, 0);
        bool use_second_target = render_objects_use_second_target();
        c_rasterizer::setup_targets_static_lighting(exposure, exposure_multiplier, use_second_target, c_camera_fx_values::g_HDR_target_stops, false);
        if (water || g_water_camera_in_water != 0)
        {
            render_objects_set_hdr_alpha_blend(false);
            c_rasterizer::resolve_surface((c_rasterizer::e_surface)13, 0, 0, 0, 0);
            rasterizer_set_global_shader_parameter(45);
            rasterizer_set_global_shader_parameter(3);
            water_renderer_draw_composite();
            rasterizer_render_water(render_objects_tiling_enabled());
            c_water_renderer::render_shading();
        }
        if (g_effect_render_globals->patchy_fog)
            render_objects_submit_patchy_fog(this);
        render_objects_render_transparent_pass(this);
        const long render_user_index = view_index;
        render_objects_render_game_engine(render_user_index);
        c_screen_postprocess::postprocess_player_view(this);
        if (g_depth_render_mode == 0)
        {
            rasterizer_main_setup_hdr_render_targets(render_objects_use_second_target(), true);
            player_view_render_first_person_effects(this, true);
        }
        rasterizer_main_setup_hdr_render_targets(use_second_target, false);
        render_lens_flares();
        submit_occlusion_tests(true, false);
        long hud_user_index = view_index;
        if (chud_draw_damage_indicators(hud_user_index))
        {
            rasterizer_main_setup_hdr_render_targets(false, true);
            render_player_view_set_exposure_constants(1.0f, 1.0f, 1.0f, true);
            c_rasterizer::set_render_target(1, (c_rasterizer::e_surface)0);
            render_objects_render_hud_damage(hud_user_index);
        }
        render_setup_window(get_rasterizer_camera(), get_rasterizer_projection());
        c_rasterizer::setup_targets_static_lighting(exposure, exposure_multiplier, true, c_camera_fx_values::g_HDR_target_stops, false);
        c_screen_postprocess::resolve_frame_buffers(&get_rasterizer_camera()->window_pixel_bounds);
        rasterizer_set_global_shader_parameter(45);
        effect_update_camera(this);
        if (render_objects_allow_ui())
            screen_postprocess_render_view(&camera_settings->values, get_rasterizer_camera(), &screen_effect, antialiasing_mode,
                &observer_flags, view_index);
    }
    if (render_objects_allow_ui())
    {
        long user = view_index;
        c_rasterizer::set_viewport(g_rasterizer_last_rectangles.viewport, 0.0f, 1.0f);
        c_rasterizer::set_scissor_rect(&g_rasterizer_last_rectangles.scissor);
        render_player_view_set_exposure_constants(1.0f, 1.0f, 1.0f, true);
        chud_draw_screen(user);
        game_engine_render_ui(user);
        rectangle2d bounds;
        interface_get_current_window_settings(0, 0, &bounds, 0);
        user_interface_render_windows(controller_index, user, user_index, &bounds, 1, false);
        chud_draw_curved_hud(user);
        player_effect_render(user);
    }
    const long debug_user_index = view_index;
    render_objects_render_game_engine_debug(debug_user_index);
    c_rasterizer::set_viewport(g_rasterizer_last_rectangles.viewport, 0.0f, 1.0f);
    c_rasterizer::set_depth_stencil_surface((c_rasterizer::e_surface)7);
    if (window_count > 1)
    {
        rectangle2d bounds = get_rasterizer_camera()->window_pixel_bounds;
        c_rasterizer::resolve_surface((c_rasterizer::e_surface)window_type, 0, &bounds, bounds.x0, bounds.y0);
        c_rasterizer::set_render_target(0, (c_rasterizer::e_surface)window_type);
    }
}

void c_lights_view::clear_simple_light_draw_list()
{
    simple_light_count = 0;
    memset(simple_light_data, 0, sizeof(simple_light_data));
}

t_restricted_allocation_manager<4, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_render_object_globals_allocator> g_render_object_globals_allocator;
