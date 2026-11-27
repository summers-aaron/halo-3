// graphicslibxbox/render_lights.obj: from the records 0x16c2a0 0x179480 0x45c490 0x45c4c0 0x45c9b0 0x45ca80 0x45cb08 0x45d018 0x45d248 0x45d2d8 0x45d440 0x45d5b8 0x45d708
// @flags /TP /O2 /Oc
#include "memory/data.h"
#include "memory/datum_index.h"
#include "cutscene/cinematic_new_globals_types.h"
#include "visibility/visible_items_types.h"
#include "visibility/visible_items.h"
#include "objects/light_globals.h"
#include "render/render_object_storage.h"
#include "render/render_simple_lights.h"
#include "render/render_object_globals_types.h"
#include "rasterizer/rasterizer_viewport_state.h"
#include "cutscene/cinematic_globals_types.h"
#include "rasterizer/rasterizer_main_globals.h"
#include "tag_files/tag_types.h"
#include <xtl.h>
#include <d3dx9math.h>
#include <string.h>
#include <math.h>
#include "math/matrix_math.h"
#include "math/color_math.h"
#include "objects/object_header.h"
#include "objects/lights.h"
#include "render/render.h"
#include "render/render_lights.h"
#include "render/render_structure.h"
#include "visibility/visibility_projection_types.h"
#include "render_methods/render_method_definitions.h"
#include "objects/light_intermediate_geometry_types.h"
#include "objects/light_geometry_types.h"
#include "cache/cache_files.h"
#include "rasterizer/rasterizer_render_globals_types.h"
#include "visibility/visibility_collection_types.h"
#include "render/render_occlusions.h"
#include "rasterizer/rasterizer.h"
#include "render/render_camera_types.h"
#include "render/object_renderer.h"
#include "render/structure_renderer.h"
#include "objects/light_datum.h"
#include "render/render_view.h"
#include "render/views/world_view_types.h"
#include "render/views/lightmap_shadows_view.h"
#include "objects/light_definitions.h"
#include "visibility/visibility_collection.h"
#include "visibility/visibility_globals.h"
#include "render/render_structure_globals_types.h"
#include "camera/cinematic_scene_types.h"
#include "render/views/player_view_types.h"
#include "render/render_structure_lighting.h"
#include "render/render_view_stack_globals.h"

// @address 0x179480 ?render_lights_submit_simple_light_draw_list_to_shader@@YAXPBMJ@Z
// @address 0x45ca80 ?render_setup@c_lights_view@@UAAXXZ
// @address 0x45d018 ?setup_clip_planes@c_lights_view@@QAAXXZ
// @address 0x45d440 render_lights_insert_sorted
// @address 0x45d5b8 render_lights_add_simple_light
// @address 0x45d708 render_lights_set_single_light


// @address 0x152178 ?set_scissor_rect@c_rasterizer@@SAXPBTrectangle2d@@@Z

class c_lights_view;
typedef char render_lights_count_offset[(sizeof(void *) != 4 || ((offsetof(c_lights_view, light_count) == 0x1034))) ? 1 : -1];
typedef char render_lights_single_offset[(sizeof(void *) != 4 || ((offsetof(c_lights_view, single_light) == 0x1038))) ? 1 : -1];
typedef char render_lights_list_offset[(sizeof(void *) != 4 || ((offsetof(c_lights_view, lights) == 0x1088))) ? 1 : -1];


static __forceinline float render_lights_cos(float angle) { return (float)cos((double)angle); }
static __forceinline float render_lights_pow(float base,float exponent) { return (float)pow((double)base,(double)exponent); }
extern "C" {
// @address 0x7c86c8 k_render_light_clip_plane_normals
const real_vector3d k_render_light_clip_plane_normals[6] = {
    { 1.0f, 0.0f, 0.0f },
    { -1.0f, 0.0f, 0.0f },
    { 0.0f, 1.0f, 0.0f },
    { 0.0f, -1.0f, 0.0f },
    { 0.0f, 0.0f, 1.0f },
    { 0.0f, 0.0f, -1.0f },
};
// @address 0x7c8710 k_render_light_clip_plane_distances
const float k_render_light_clip_plane_distances[6] = { 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, -1.0f };
}

__forceinline real_vector3d *render_lights_transform_normal(const real_matrix4x3 *m,const real_vector3d *n,real_vector3d *out) {
 float i=n->i,j=n->j,k=n->k;
 out->i=i*m->forward.i+j*m->left.i+k*m->up.i;
 out->j=i*m->forward.j+j*m->left.j+k*m->up.j;
 out->k=i*m->forward.k+j*m->left.k+k*m->up.k;
 return out;
}
__forceinline real_plane3d *render_lights_transform_plane(const real_matrix4x3 *m,const real_plane3d *p,real_plane3d *out) {
 render_lights_transform_normal(m,&p->normal,&out->normal);
 out->distance=m->scale*p->distance+dot_product3d(&m->position,&out->normal);
 return out;
}

void render_lights_submit_simple_light_draw_list_to_shader(const float *lights,long count) {
    int light_counts[4]={count,0,5,0};
    c_rasterizer::get_device()->SetPixelShaderConstantI(0,light_counts,1);
    float light_count_float[4];
    light_count_float[0]=light_count_float[1]=light_count_float[2]=light_count_float[3]=(float)count;
    c_rasterizer::get_device()->SetPixelShaderConstantF(17,light_count_float,1);
    if(count>0) c_rasterizer::get_device()->SetPixelShaderConstantF(18,lights,80);
}

void c_lights_view::render_setup() {
    rectangle2d bounds;
    bounds.y0 = 0;
    bounds.x0 = 0;
    bounds.x1 = (short)width;
    bounds.y1 = (short)height;
    rectangle2d scissor = bounds;
    rectangle2d viewport = bounds;
    c_rasterizer::set_viewport(viewport, 0.0f, 1.0f);
    g_rasterizer_last_rectangles.viewport = bounds;
    c_rasterizer::set_scissor_rect(&scissor);
    g_rasterizer_last_rectangles.scissor = bounds;
}

void c_lights_view::setup_clip_planes(void) {
 c_rasterizer::get_device()->SetRenderState(D3DRS_CLIPPLANEENABLE,63);
 D3DXMATRIX player_matrix;
 memcpy(&player_matrix,&c_player_view::get_current()->get_rasterizer_projection()->projection_matrix,sizeof(player_matrix));
 D3DXMatrixTranspose(&player_matrix, &player_matrix);
 D3DXMatrixInverse(&player_matrix, 0, &player_matrix);
 const render_projection *rasterizer=&rasterizer_projection;
 D3DXMATRIX gel_matrix;
 memcpy(&gel_matrix,texture_matrix,sizeof(gel_matrix));
 D3DXMatrixInverse(&gel_matrix, 0, &gel_matrix);
 D3DXMatrixTranspose(&gel_matrix, &gel_matrix);
 D3DXMatrixInverse(&gel_matrix, 0, &gel_matrix);
 for(long plane_index=0;plane_index<6;plane_index++) {
  real_plane3d clip;
  clip.normal=k_render_light_clip_plane_normals[plane_index]; clip.distance=-k_render_light_clip_plane_distances[plane_index];
  real_plane3d transformed,world,player,final_plane;
  D3DXPlaneTransform((D3DXPLANE *)&transformed, (const D3DXPLANE *)&clip,
      &gel_matrix);
  transformed.distance=-transformed.distance;
  render_lights_transform_plane(&rasterizer->view_to_world,&transformed,&world);
  render_lights_transform_plane(&c_player_view::get_current()->get_rasterizer_projection()->world_to_view,&world,&player);
  player.distance=-player.distance;
  D3DXPlaneTransform((D3DXPLANE *)&final_plane, (const D3DXPLANE *)&player,
      &player_matrix);
  c_rasterizer::get_device()->SetClipPlane(plane_index,(const float*)&final_plane);
 }
}

// @address 0x45d248 render_lights_render_light
extern "C" void render_lights_render_light(long light_index, long unused, float scale, c_lights_view *view) {
 light_geometry geometry;
 if (light_build_render_geometry(light_index, &geometry)) {
  c_visible_items::push_marker();
  view->compute_visibility(light_index, &geometry);
  view->submit_visibility_and_render(light_index, scale, &geometry);
  c_rasterizer::clear_sampler_textures(0xffff);
  c_visible_items::pop_marker();
 }
}

// @address 0x45d2d8 ?initialize_simple_light@c_lights_view@@SAXPAUsimple_light@@Treal_point3d@@Treal_rgb_color@@MMTreal_vector3d@@MMM@Z
void c_lights_view::initialize_simple_light(simple_light *light, real_point3d position, real_rgb_color color, float intensity, float radius,
    real_vector3d direction, float angle, float exponent, float diffuse)
{
 light->position=position;
 light->intensity=intensity;
 light->direction.i=-direction.i;
 light->direction.j=-direction.j;
 light->direction.k=-direction.k;
 light->diffuse=diffuse;
 real_rgb_color_scale(&color,1.0f/(intensity>0.000001f ? intensity : 0.000001f),&light->color);
 light->exponent=exponent;
 float radius_squared=radius*radius;
 if(angle<0.0001f) {
  light->angle_scale=0.0f;
  light->angle_offset=1.0f;
 } else {
  float cosine=render_lights_cos(angle/2.0f);
  light->angle_scale=render_lights_pow(1.0f-diffuse,1.0f/exponent)/(1.0f-cosine);
  light->angle_offset=-light->angle_scale*cosine;
 }
 light->radius_scale=intensity*(intensity+radius_squared)/radius_squared;
 light->radius_offset=-intensity/radius_squared;
 light->radius_squared=radius_squared;
}

// match: 0x45d440: __restrict on items keeps the carried values[] element in fr13 across shift-loop iterations (spills to fr0 + reorders stores without it)
extern "C" int render_lights_insert_sorted(int item,float value,float * values,int *__restrict items,int *count) {
 int list_count=*count;
 int insert_index=-1;
 if(list_count<16) {
  insert_index=list_count;
  *count=list_count+1;
 } else if(values[list_count-1]<value) {
  insert_index=list_count-1;
 }
 if(insert_index!=-1) {
  for(;insert_index>0;insert_index--) {
   if(values[insert_index-1]>=value) break;
   values[insert_index]=values[insert_index-1];
   items[insert_index]=items[insert_index-1];
  }
  values[insert_index]=value;
  items[insert_index]=item;
 }
 return insert_index;
}

static const float render_lights_half = 0.5f;
static const float render_lights_zero = 0.0f;
static const float render_lights_one = 1.0f;
static const float render_lights_five = 5.0f;
extern "C" void render_lights_add_simple_light(int light_index, int view_index, float intensity_scale, void *context)
{
    c_lights_view *state = static_cast<c_lights_view *>(context);
    light_datum *datum = datum_get<light_datum>(light_data, light_index);
    real_rgb_color color;
    color.red = datum->color.red * intensity_scale;
    color.green = datum->color.green * intensity_scale;
    color.blue = datum->color.blue * intensity_scale;
    const light_definition *definition = (const light_definition *)global_tag_instances[datum_index_to_absolute_index(datum->definition_index)].definition;

    float diffuse;
    if (definition->type == 0) diffuse = render_lights_one;
    else diffuse = definition->simple_light_diffuse;
    float exponent = definition->simple_light_exponent;
    float angle = definition->field_of_view;
    float radius = definition->radius;
    int count = state->light_count;
    float intensity = definition->simple_light_intensity;
    real_vector3d direction = datum->forward;
    real_point3d position = datum->position;
    if (count < 16) {
        if (radius <= render_lights_zero) radius = render_lights_five;
        if (angle <= render_lights_zero) angle = render_lights_half;
        if (exponent <= render_lights_zero) exponent = render_lights_one;
        state->light_count = count + 1;
        c_lights_view::initialize_simple_light(&state->lights[count], position, color, intensity, radius, direction, angle, exponent,
            diffuse);
    }
}

extern "C" void render_lights_set_single_light(c_lights_view *state, real_point3d position, real_rgb_color color, float intensity,
    float radius, real_vector3d direction, float angle, float exponent, float diffuse)
{
 if(radius<=0.0f) radius=5.0f;
 if(angle<=0.0f) angle=0.5f;
 if(exponent<=0.0f) exponent=1.0f;
 simple_light *light=&state->single_light;
 c_lights_view::initialize_simple_light(light,position,color,intensity,radius,direction,angle,exponent,diffuse);
 render_lights_submit_simple_light_draw_list_to_shader((const float *)light,1);
}


// @address 0x45c490 ?render_submit_visibility@c_lights_view@@UAAXXZ
void c_lights_view::render_submit_visibility()
{
  ::c_structure_renderer::submit_visibility(0);
  ::c_object_renderer::submit_visibility(0);
}

#if !defined(_XBOX) || defined(__clang__)
// GAME-CHANGES F118: the Xbox c_lightmap_shadows_view vtable 0x82000e30 slot 3 is 0x17c680, which submits the objects
// in pass 6; 0x45c490 (pass 0) is c_lights_view's slot 3, where the Xbox build places this symbol.
void c_lightmap_shadows_view::render_submit_visibility()
{
  ::c_structure_renderer::submit_visibility(0);
  ::c_object_renderer::submit_visibility(6);
}
#else
// @address 0x45c490 ?render_submit_visibility@c_lightmap_shadows_view@@UAAXXZ
void c_lightmap_shadows_view::render_submit_visibility()
{
  ::c_structure_renderer::submit_visibility(0);
  ::c_object_renderer::submit_visibility(0);
}
#endif


// match: 0x45c4c0: tuned against the old cross_product3d (one more local, i); 1 word differs without it
static __forceinline real_vector3d *render_lights_cross_product3d(const real_vector3d *a, const real_vector3d *b, real_vector3d *result)
{
    float i = a->j * b->k - a->k * b->j;
    float k = a->i * b->j - a->j * b->i;
    float j = a->k * b->i - a->i * b->k;
    result->i = i;
    result->j = j;
    result->k = k;
    return result;
}

// @address 0x45c4c0 ?setup_shadow_camera_and_projection@c_lights_view@@QAA_NPAUlight_state@@PAUlight_geometry@@JJ@Z
bool c_lights_view::setup_shadow_camera_and_projection(light_state *state, light_geometry *geometry, long width, long height)
{
    c_lights_view *view = this;
    render_projection *projection = &view->rasterizer_projection;
    render_camera *camera = &view->rasterizer_camera;
    bool result = false;
    if (geometry->field_of_view < 3.1414928f && (short)geometry->type != 0)
    {
    float zoom;
    float aspect = 1.0f / geometry->intermediate.frustum.distance;
    view->width = width; view->height = height;
    view->near_size = geometry->near_distance;
    view->near_aspect = geometry->intermediate.frustum.distance * geometry->near_distance;
    view->orthographic = geometry->field_of_view < 0.0001f;
    float distance = 0.0f;
    if (!view->orthographic) { zoom = 1.0f / (float)tan(geometry->field_of_view * 0.5f); distance = 0.5f * view->near_size * zoom; }
    real_vector3d right, up, direction;
    render_lights_cross_product3d(&state->forward, &state->up, &right);
    render_lights_cross_product3d(&right, &state->forward, &up);
    negate_vector3d(&state->forward, &direction);
    normalize3d(&right); normalize3d(&up); normalize3d(&direction);
    projection->view_to_world.forward = right; projection->view_to_world.left = up; projection->view_to_world.up = direction;
    point_from_line3d(&state->position, &state->forward, -distance, &projection->view_to_world.position);
    projection->view_to_world.scale = 1.0f;
    matrix4x3_inverse(&projection->view_to_world, &projection->world_to_view);
    camera->position = projection->view_to_world.position; camera->forward = state->forward; camera->up = state->up;
    memset(projection->projection_matrix, 0, sizeof(projection->projection_matrix));
    real_vector3d transformed;
    matrix4x3_transform_vector(&projection->world_to_view, &geometry->intermediate.frustum.direction, &transformed);
    float x = -transformed.i / transformed.k;
    float y = -transformed.j / transformed.k;
    if (view->orthographic) {
        float near_plane = 0.0f;
        float far_distance = geometry->intermediate.frustum.far_offset;
        float factor = 2.0f / view->near_size, scaled = factor * aspect;
        view->near_plane = near_plane; view->far_plane = far_distance;
        projection->projection_matrix[0][0] = factor;
        projection->projection_matrix[1][1] = scaled;
        projection->projection_matrix[2][0] = factor * x;
        projection->projection_matrix[2][1] = scaled * y;
        projection->projection_matrix[3][0] = (float)(-1.0 / (float)width);
        projection->projection_matrix[3][1] = (float)(1.0 / (float)height);
        projection->projection_matrix[2][2] = -1.0f / (far_distance - near_plane);
        projection->projection_matrix[3][2] = near_plane / (far_distance - near_plane);
        projection->projection_matrix[3][3] = 1.0f;
        memcpy(view->texture_matrix, projection->projection_matrix, sizeof(view->texture_matrix));
        view->texture_matrix[0] *= 0.5f; view->texture_matrix[5] *= -0.5f;
        view->texture_matrix[8] *= 0.5f; view->texture_matrix[9] *= -0.5f;
        view->texture_matrix[12] = 0.5f; view->texture_matrix[13] = 0.5f;
    } else {
        float xscale = zoom;
        float factor = xscale * aspect;
        float far_plane = geometry->intermediate.frustum.far_offset + distance;
        float near_plane = MAX(distance, far_plane * 0.01f);
        float dx = x * xscale, dy = factor * y;
        view->near_plane = near_plane; view->far_plane = far_plane;
        projection->projection_matrix[2][3] = -1.0f; projection->projection_matrix[0][0] = xscale;
            projection->projection_matrix[1][1] = factor;
        projection->projection_matrix[3][0] = dx*distance; projection->projection_matrix[3][1] = dy*distance;
        projection->projection_matrix[2][2] = -(far_plane/(far_plane-near_plane));
            projection->projection_matrix[3][2] = -(near_plane*far_plane/(far_plane-near_plane));
        projection->projection_matrix[2][0] = 1.0f/(float)width + dx; projection->projection_matrix[2][1] = dy - 1.0f/(float)height;
        memcpy(view->texture_matrix, projection->projection_matrix, sizeof(view->texture_matrix));
        view->texture_matrix[0] *= 0.5f; view->texture_matrix[5] *= -0.5f;
        view->texture_matrix[8] = dx*0.5f - 0.5f; view->texture_matrix[9] = dy*-0.5f - 0.5f;
        view->texture_matrix[12] = dx*distance*0.5f; view->texture_matrix[13] = dy*distance*-0.5f;
    }
    memcpy(&view->render_camera_state, camera, sizeof(view->render_camera_state));
    memcpy(&view->render_projection_state, projection, sizeof(view->render_projection_state));
    result = true;
    }
    return result;
}


static __forceinline c_visibility_collection *render_lights_get_global_camera_collection()
{ return (c_visibility_collection *)&g_visibility_globals.collection_input; }
static __forceinline void render_lights_compute_collection(c_visibility_collection collection,
    const visibility_projection *projections, long count,
    const real_point3d *center, float radius, long region_index, s_cluster_reference cluster, long option0, long option1)
{
    collection.compute(_collection_type_light, 0, projections, count, center, radius, region_index, cluster, option0, option1);
    c_visibility_collection::postprocess_filter_unlight_non_visible_items();
}

// @address 0x45c9b0 ?compute_visibility@c_lights_view@@QAAXJPAUlight_geometry@@@Z
void c_lights_view::compute_visibility(long light_index, light_geometry *geometry)
{
    light_datum *light = datum_get<light_datum>(light_data, light_index);
    light_build_visibility_projections(&light->forward, &light->up, geometry, m_visibility_projections, &m_visibility_projection_count);
    c_visibility_collection *camera_collection = render_lights_get_global_camera_collection();
    s_visibility_input input;
    render_lights_compute_collection(c_visibility_collection(&input), m_visibility_projections, m_visibility_projection_count,
        &light->bounding_sphere_center, light->bounding_sphere_radius,
        camera_collection->get_input()->region_index, light->location.cluster_reference, camera_collection->get_input()->option0,
            camera_collection->get_input()->option1);
}

// @address 0x5bec98 ?compute_visibility@c_lights_view@@UAAXXZ
void c_lights_view::compute_visibility()
{
}

extern "C" {
// @address 0x730e68 g_current_light_gel_index
long g_current_light_gel_index = -1;
// @address 0x81dc04 g_view_stack
}
static __forceinline void render_lights_push_structure_marker()
{
    g_render_structure_globals.depth++;
    g_render_structure_globals.cluster_markers[g_render_structure_globals.depth] = g_render_structure_globals.current_a;
    g_render_structure_globals.instance_markers[g_render_structure_globals.depth] = g_render_structure_globals.current_b;
}
static __forceinline void render_lights_push_object_marker()
{
    s_render_object_globals *globals = g_render_object_globals;
    long *marker_depth = &globals->marker_depth;
    (*marker_depth)++;
    globals->marker_object_counts[*marker_depth] = globals->object_count;
    globals->marker_mesh_part_counts[*marker_depth] = globals->mesh_part_count;
}
// match: 0x45cb08, 0x16c2a0: calling the shared cinematic_in_progress() lets the optimizer merge the repeated tests (-36B, -28B)
static __forceinline bool render_lights_cinematic_in_progress()
{
    bool result = false;
    if (cinematic_globals)
        result = cinematic_globals->in_progress;
    return result;
}

// @address 0x45cb08 ?submit_visibility_and_render@c_lights_view@@QAAXJMPAUlight_geometry@@@Z
void c_lights_view::submit_visibility_and_render(long light_index, float scale, light_geometry *geometry)
{
    light_datum *light = datum_get<light_datum>(light_data, light_index);
    const light_definition *definition = (const light_definition *)global_tag_instances[datum_index_to_absolute_index(light->definition_index)].definition;
    long target = c_rasterizer::render_globals.shadow_size;
    if (!setup_shadow_camera_and_projection(&light->placement, geometry, target, target))
        return;

    c_lights_view *state = this;
    render_lights_push_structure_marker();
    render_lights_push_object_marker();
    render_submit_visibility();
    D3DDevice_FlushHiZStencil(c_rasterizer::get_device(), (D3DFHZS_FLUSHTYPE)1);
    bool shadows = (definition->flags & 2) != 0 && (short)geometry->type != 0;
    if (shadows) {
        c_view::begin(this);
        render_setup_window((const render_camera *)&state->rasterizer_camera, (const render_projection *)&state->rasterizer_projection);
        render_occlusions_setup_depth_target(1, c_rasterizer::render_globals.shadow_size, c_rasterizer::render_globals.shadow_size);
        c_object_renderer::render_static_lighting((c_rasterizer::e_z_buffer_mode)4);
        structure_renderer_render_static_lighting((c_rasterizer::e_z_buffer_mode)4);
        c_rasterizer::resolve_surface((c_rasterizer::e_surface)2, 4, 0, 0, 0);
        rasterizer_set_light_render_targets(state->render_target0, state->render_target1, state->depth_stencil);
        c_view::end();
    } else {
        rasterizer_set_light_render_targets(state->render_target0, state->render_target1, state->depth_stencil);
    }
    D3DDevice_FlushHiZStencil(c_rasterizer::get_device(), (D3DFHZS_FLUSHTYPE)1);
    rasterizer_set_light_render_targets(state->render_target0, state->render_target1, state->depth_stencil);
    if (c_rasterizer::g_current_alpha_blend_mode != c_rasterizer::_alpha_blend_additive) {
        c_rasterizer::g_current_alpha_blend_mode = c_rasterizer::_alpha_blend_additive;
        D3DDevice *device = c_rasterizer::get_device();
        D3DDevice_SetRenderState_AlphaBlendEnable(device, 1);
        D3DDevice_SetRenderState_BlendOp(device, 0);
        D3DDevice_SetRenderState_SrcBlend(device, 1);
        D3DDevice_SetRenderState_DestBlend(device, 1);
    }
    c_rasterizer::set_z_buffer_mode((c_rasterizer::e_z_buffer_mode)5);
    c_rasterizer::set_viewport(g_rasterizer_last_rectangles.viewport, 0.0f, 1.0f);
    c_rasterizer::set_scissor_rect(&g_rasterizer_last_rectangles.scissor);

    real_rgb_color color;
    color.red = light->color.red * scale;
    color.green = light->color.green * scale;
    color.blue = light->color.blue * scale;
    render_lights_set_single_light(this, geometry->origin, color, definition->simple_light_intensity,
        geometry->far_width, geometry->intermediate.frustum.direction,
        geometry->field_of_view, definition->simple_light_exponent, definition->simple_light_diffuse);
    setup_clip_planes();
    float combined[4][4];
    combine_projection_and_view_matrix(&state->rasterizer_projection.world_to_view, (const float (*)[4])state->texture_matrix,
        &combined[0][0]);
    c_rasterizer::set_vertex_shader_constant(240, 4, (const real_vector4d *)combined);
    c_rasterizer::set_pixel_shader_constant(1, 1, (const real_vector4d *)&combined[2]);

    real_vector4d direction;
    direction.i = -light->forward.i;
    direction.j = -light->forward.j;
    direction.k = -light->forward.k;
    normalize3d(&direction.xyz);
    float width = state->near_size / (float)state->width;
    float height = state->near_aspect / (float)state->height;
    float depth_scale = state->orthographic ? 1.0f : state->near_plane;
    direction.l = sqrtf(height * height + width * width) * 0.5f / (state->far_plane - state->near_plane) / depth_scale;
    c_rasterizer::set_pixel_shader_constant(2, 1, &direction);
    real_vector4d gel_transform;
    gel_transform.i = gel_transform.j = 1.0f;
    gel_transform.k = light->gel_offset_u;
    gel_transform.l = light->gel_offset_v;
    c_rasterizer::set_pixel_shader_constant(5, 1, &gel_transform);
    g_current_light_gel_index = definition->gel_index;
    BOOL has_shadows = shadows != false;
    D3DDevice_SetPixelShaderConstantB(c_rasterizer::get_device(), 13, &has_shadows, 1);
    structure_renderer_render_shadows_generate((e_entry_point)(render_lights_cinematic_in_progress() ? 17 : 9));
    c_object_renderer::render_shadows_generate((e_entry_point)(render_lights_cinematic_in_progress() ? 17 : 9));
    D3DDevice_SetRenderState_ClipPlaneEnable(c_rasterizer::get_device(), 0);
    g_current_light_gel_index = -1;
    c_structure_renderer::pop_marker();
    c_object_renderer::pop_marker();
    c_rasterizer::begin_high_quality_blend();
}

struct light_datum;
// @address 0x16c2a0 render_lights_iterate_lights
extern "C" void render_lights_iterate_lights(void *context, long unused_user_index, long view_index, long maximum, unsigned char direct,
    unsigned char indirect, float scale, void (*callback)(int, int, float, void *))
{
    int count = 0;
    float values[16];
    memset(values, 0, sizeof(values));
    int items[16];
    long first_index = c_visible_items::get_light_starting_index();
    long last_index = c_visible_items::m_items.objects_to_render.count;
    for (long i = first_index; i < last_index; ++i) {
        int id = c_visible_items::m_items.objects_to_render.entries[i];
        light_datum *light = datum_get<light_datum>(light_data, id);
        const light_definition *definition = (const light_definition *)global_tag_instances[datum_index_to_absolute_index(light->definition_index)].definition;
        bool static_light = (definition->flags & 1) != 0;
        bool render = (static_light && direct) || (!static_light && indirect);
        if (render) {
            s_nondeterministic_light_data *entry = &g_nondeterministic_light_data[datum_index_to_absolute_index(id)];
            if (light_adjust_position_from_attachment_last_minute((light_datum *)light,
                c_player_view::get_current()->get_rasterizer_camera())) {
                if (definition->flags & 0x100) {
                    callback(id, view_index, entry->specular_fade * entry->specular_fade * scale, context);
                    --maximum;
                } else if (render_lights_insert_sorted(id, entry->intensity, values, items, &count) == -1) {
                    entry->players[view_index].fade = 0;
                    entry->players[view_index].cooldown = 60;
                }
            }
        }
    }
    if (render_lights_cinematic_in_progress()) {
        if (render_lights_cinematic_in_progress() && cinematic_new_globals && cinematic_new_globals->current_scene != -1 &&
            ((const s_cinematic_scene *)global_tag_instances[datum_index_to_absolute_index(cinematic_new_globals->current_scene)].definition)->type == 0)
        {
            for (int i = 0; i < count; ++i) {
                s_nondeterministic_light_player_state *state =
                    &g_nondeterministic_light_data[datum_index_to_absolute_index(items[i])].players[view_index];
                state->cooldown = 0;
                state->fade = 255;
                callback(items[i], view_index, scale, context);
            }
            return;
        }
    }
    float threshold = 0.0f;
    if (count > maximum)
        threshold = values[MIN(MAX(maximum, 0), count - 1)] + 0.01f;
    for (int i = 0; i < count; ++i) {
        float value = values[i];
        s_nondeterministic_light_data *entry = &g_nondeterministic_light_data[datum_index_to_absolute_index(items[i])];
        s_nondeterministic_light_player_state *player = &entry->players[view_index];
        if (!(value < threshold) && maximum > 0) {
            if (player->cooldown)
                player->cooldown--;
            else {
                long increased = player->fade + 1;
                if (increased > 255)
                    increased = 255;
                player->fade = increased;
            }
        } else {
            int decrement = (int)(36.0f - value * 2.0f);
            player->fade = MAX(player->fade - MAX(decrement, 1), 0);
            if (player->fade < 100)
                player->cooldown = 60;
        }
        unsigned long amount = player->fade;
        if (amount) {
            if (amount > 100)
                amount = 100;
            float factor = entry->specular_fade * (float)(long)amount * 0.01f;
            callback(items[i], view_index, factor * factor * scale, context);
        }
    }
}

// @address 0x8c46dc ?x_current_player_view@c_player_view@@1PAV1@A
// @address 0x81da20 g_rasterizer_last_rectangles
