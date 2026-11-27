// blamlibxbox/scenario_soft_ceilings.obj: from the records 0x2268a8 0x2268f0 0x226918 0x226d10 0x226e80 0x2270d0 0x2272a8 0x227a18 0x227f30 0x227fb0 0x228060 0x228100 0x228118
// @compiler xdk6130
// @flags /TP /O2 /Oc
#include "memory/tls_globals.h"
#include "multithreading/thread_tls.h"
#include "math/real_math_types.h"
#include "math/real_math.h"
#include "tag_files/tag_types.h"
#include "memory/static_arrays.h"
#include "phys/phys_collide.h"
// @address 0x227f30 ?scenario_soft_ceiling_from_name@@YAPAUs_scenario_soft_ceiling@@J@Z
#include <stddef.h>
#include <string.h>
#include <math.h>
#include <float.h>
#include "cseries/cseries.h"
#include "phys/phys_base.h"
#include "cache/restricted_memory_regions.h"
#include "physics/havok_utilities.h"
#include "scenario/scenario_soft_ceilings.h"
#include "scenario/scenario_soft_ceilings_allocator.h"
#include "physics/collision_features.h"
#include "ai/ai_mission_dialogue.h"
#include "phys/phys_bvh_vm.h"
#include "memory/datum_index.h"
#include "memory/bit_vectors.h"
#include "physics/soft_ceiling_triangle_types.h"
#include "physics/soft_ceiling_group_types.h"
#include "cache/cache_files.h"

#include "scenario/scenario_soft_ceiling_state.h"
#include "cache/restricted_memory_regions_inline.h"
#include "phys/phys_bvh_sphere_query.h"
#include "cache/restricted_memory_layout.h"
#include "cache/restricted_memory_registration.h"
#include "physics/collision_feature_types.h"
#include "physics/soft_ceiling_design_types.h"
#include "physics/soft_ceiling_mopp_key.h"
#include "scenario/scenario_tag.h"
#include "scenario/scenario.h"
#include "scenario/scenario_structure_bsp_reference.h"

typedef unsigned int uint;

struct s_soft_ceiling_contact_point {
  long bsp_index;
  unsigned short plane_index;
  unsigned short record_index;
  s_soft_ceiling_triangle *triangle;
  float depth;
  float height;
  real_point3d position;
};

struct s_soft_ceiling_contacts {
  long closest_index;
  float closest_height;
  real_point3d closest_position;
#ifdef HALO3_64BIT
  long count;
  s_soft_ceiling_contact_point contact_points[256];
#else
  c_static_stack<s_soft_ceiling_contact_point, 512> contact_points;
#endif
};

struct s_soft_ceiling_query_hit {
  long bsp_index;
  unsigned short plane_index;
  unsigned short record_index;
  s_soft_ceiling_triangle *plane;
  float plane_distance;
  float clearance;
  real_point3d point;
};

struct s_soft_ceiling_query_results {
  long best_index;
  float best_clearance;
  real_point3d best_point;
  long count;
  s_soft_ceiling_query_hit hits[256];
};

#ifdef HALO3_64BIT
typedef char soft_ceiling_contacts_size_check[(sizeof(s_soft_ceiling_contacts) == sizeof(s_soft_ceiling_query_results)) ? 1 : -1];
typedef char soft_ceiling_contacts_count_offset_check[(offsetof(s_soft_ceiling_contacts, count) == offsetof(s_soft_ceiling_query_results, count)) ? 1 : -1];
typedef char soft_ceiling_contacts_points_offset_check[(offsetof(s_soft_ceiling_contacts, contact_points) == offsetof(s_soft_ceiling_query_results, hits)) ? 1 : -1];
typedef char soft_ceiling_contact_point_size_check[(sizeof(s_soft_ceiling_contact_point) == sizeof(s_soft_ceiling_query_hit)) ? 1 : -1];
typedef char soft_ceiling_contact_point_triangle_offset_check[(offsetof(s_soft_ceiling_contact_point, triangle) == offsetof(s_soft_ceiling_query_hit, plane)) ? 1 : -1];
typedef char soft_ceiling_contact_point_position_offset_check[(offsetof(s_soft_ceiling_contact_point, position) == offsetof(s_soft_ceiling_query_hit, point)) ? 1 : -1];
#endif

// 0x2268a8
void scenario_soft_ceilings_initialize()
{
    TLS_SCENARIO_SOFT_CEILING_STATE = (unsigned long *)(&g_scenario_soft_ceilings_allocator)
                       ->reserve_memory("soft surface globals", 0, 16, 0);
}

// The March release map folds this empty game-system callback into the shared blr at 0x5bec98.
// @address 0x5bec98 ?scenario_soft_ceilings_dispose@@YAXXZ
void scenario_soft_ceilings_dispose()
{
}

// 0x2268f0
void scenario_soft_ceilings_initialize_for_new_map()
{
    memset(TLS_SCENARIO_SOFT_CEILING_STATE, -1, sizeof(unsigned long) * 4);
}

// @address 0x5bec98 ?scenario_soft_ceilings_dispose_from_old_map@@YAXXZ
void scenario_soft_ceilings_dispose_from_old_map()
{
}

// 0x226d10
// @address 0x226d10 scenario_soft_ceilings_add_triangle_collision_features
extern "C" void scenario_soft_ceilings_add_triangle_collision_features(collision_feature_list *list,
                             long instance_index, s_soft_ceiling_triangle *triangle, const real_point3d *unused_point,
                             float unused, float height, float radius)
{
  real_point3d *points = triangle->vertices;
  unsigned long point_count = 3;
  real_point3d *point = points;
  do {
    collision_features_from_point(point, height, radius, -1, -1, instance_index, -1, 0, 0, -1, list);
    point_count--;
    point++;
  } while (point_count != 0);

  long edge_index = 0;
  real_point3d *current = points;
  do {
    edge_index++;
    long next_index = edge_index % 3;
    const real_point3d *next = &triangle->vertices[next_index];
    real_vector3d edge;
    edge.i = next->x - current->x;
    edge.j = next->y - current->y;
    edge.k = next->z - current->z;
    collision_features_from_line(current, &edge, height, radius, -1, -1, instance_index, -1, 0, 0, -1, list);
    current++;
  } while (edge_index < 3);

  collision_features_from_polygon(3, points, &triangle->plane, height, radius, -1, -1, instance_index, -1, 0, 0, -1, list);
}

// @address 0x2270d0 scenario_soft_ceilings_is_contact_adjacent
extern "C" bool scenario_soft_ceilings_is_contact_adjacent(const s_soft_ceiling_contacts *contacts, long first_index, long second_index)
{
  const real_point3d *position = &contacts->contact_points[first_index].position;
  const s_soft_ceiling_triangle *triangle = contacts->contact_points[second_index].triangle;
  const float threshold = 0.05f * 0.05f;
  bool adjacent = first_index == second_index;

  for (long edge = 0; !adjacent && edge < 3; edge++) {
    const real_point3d *a = &triangle->vertices[edge];
    const real_point3d * b = &triangle->vertices[(edge + 1) % 3];

    if (distance_squared3d(position, a) < threshold ||
        distance_squared3d(position, b) < threshold ||
        distance_squared3d(position, &contacts->contact_points[second_index].position) < threshold) {
      adjacent = true;
      break;
    }

    real_vector3d edge_direction;
    vector_from_points3d(a, b, &edge_direction);
    float length = ::normalize3d(&edge_direction);
    if (length != 0.0f) {
      real_vector3d delta;
      vector_from_points3d(a, position, &delta);
      float projection = dot_product3d(&delta, &edge_direction);
      if (projection >= 0.0f && projection <= length) {
        real_vector3d projected;
        real_vector3d perpendicular;
        scale_vector3d(&edge_direction, projection, &projected);
        subtract_vectors3d(&delta, &projected, &perpendicular);
        if (magnitude_squared3d(&perpendicular) < threshold) {
          adjacent = true;
          break;
        }
      }
    }
  }

  return adjacent;
}

static __forceinline float scenario_soft_ceilings_normalize3d(real_vector3d *vector)
{
  float length = magnitude3d(vector);
  if (!(fabs(length - 0.0f) < 0.0001f))
    scale_vector3d(vector, 1.0f / length, vector);
  else
    length = 0.0f;
  return length;
}

// 0x2272a8
// @address 0x2272a8 scenario_soft_ceilings_compute_collision_normal
extern "C" bool scenario_soft_ceilings_compute_collision_normal(const s_soft_ceiling_contacts *contact,
                             const real_point3d *point, c_static_stack<long, 3> *contact_indices,
                             // match: 0x2272a8: pointer __restrict qualifier prevents alias reload
                             real_vector3d *normal, float *height, float *__restrict magnitude_squared, bool try_invert)
{
  const float height_tolerance = 0.1f;
  bool result = false;
  long closest_index = contact->closest_index;
  float closest_height = contact->closest_height;
#ifdef HALO3_64BIT
  long count = contact->count;
#else
  long count = contact->contact_points.count();
#endif
  if (closest_index != -1) {
    bool found_penetrating = false;
    c_static_stack<long, 3> stack;
    for (long i = 0; i < count; i++) {
      if (contact->contact_points[i].height < closest_height + height_tolerance &&
          scenario_soft_ceilings_is_contact_adjacent(contact, closest_index, i) &&
          contact->contact_points[i].depth < 0.0f) {
        found_penetrating = true;
        break;
      }
    }
    if (found_penetrating) {
      real_vector3d offset;
      vector_from_points3d(&contact->contact_points[closest_index].position, point, &offset);
      if (try_invert)
        scale_vector3d(&offset, -1.0f, &offset);
      if (normalize3d(&offset) != 0.0f) {
        real_vector3d direction = offset;
        const real_vector3d *original = &offset;
        for (long round = 0; round < 3; round++) {
          float best_distance = 3.4028234663852886e+38f;
          long best_index = -1;
          for (long i = 0; i < count; i++) {
            if (contact->contact_points[i].height < closest_height + height_tolerance &&
                scenario_soft_ceilings_is_contact_adjacent(contact, closest_index, i))
            {
              const real_vector3d *triangle_normal = &contact->contact_points[i].triangle->plane.normal;
              float distance = dot_product3d(&direction, triangle_normal);
              if (distance < -0.001f && distance < best_distance) {
                best_distance = distance;
                best_index = i;
              }
            }
          }
          if (best_index == -1)
            break;
          stack.push_back(best_index);
          switch (stack.count()) {
          case 1: {
            const real_vector3d *triangle_normal = &contact->contact_points[stack[0]].triangle->plane.normal;
            real_vector3d projected;
            scale_vector3d(triangle_normal, best_distance, &projected);
            subtract_vectors3d(original, &projected, &direction);
            break;
          }
          case 2: {
            const real_vector3d *normal_a = &contact->contact_points[stack[0]].triangle->plane.normal;
            const real_vector3d *normal_b = &contact->contact_points[stack[1]].triangle->plane.normal;
            real_vector3d clip_line_vector;
            cross_product3d(normal_a, normal_b, &clip_line_vector);
            scenario_soft_ceilings_normalize3d(&clip_line_vector);
            float projection = dot_product3d(original, &clip_line_vector);
            scale_vector3d(&clip_line_vector, projection, &direction);
            break;
          }
          case 3:
            break;
          }
        }
        if (stack.count() != 0) {
          long nearest_slot = 0;
          float nearest_distance = 3.4028234663852886e+38f;
          real_vector3d nearest_normal = contact->contact_points[stack[0]].triangle->plane.normal;
          for (long j = 0; j < stack.count(); j++) {
            long index = stack[j];
            if (contact->contact_points[index].height < closest_height + height_tolerance &&
                scenario_soft_ceilings_is_contact_adjacent(contact, closest_index, index))
            {
              float distance = dot_product3d(&offset, &contact->contact_points[index].triangle->plane.normal);
              if (distance < nearest_distance) {
                nearest_normal = contact->contact_points[index].triangle->plane.normal;
                nearest_distance = distance;
                nearest_slot = j;
              }
            }
          }
          *height = contact->contact_points[stack[0]].height;
          if (stack.count() != 1) {
            const real_vector3d *normal_a = &contact->contact_points[stack[0]].triangle->plane.normal;
            const real_vector3d *normal_b = &contact->contact_points[stack[1]].triangle->plane.normal;
            real_vector3d clip_vector;
            cross_product3d(normal_a, normal_b, &clip_vector);
            scenario_soft_ceilings_normalize3d(&clip_vector);
            if (nearest_slot == 0) {
              cross_product3d(&clip_vector, &nearest_normal, normal);
              cross_product3d(normal, &clip_vector, normal);
            } else {
              cross_product3d(&nearest_normal, &clip_vector, normal);
              cross_product3d(&clip_vector, normal, normal);
            }
            if (stack.count() == 3) {
              const real_vector3d *normal_c = &contact->contact_points[stack[2]].triangle->plane.normal;
              float normal_dot = dot_product3d(&offset, normal);
              float third_dot = dot_product3d(&offset, normal_c);
              bool flip = normal_dot < third_dot;
              cross_product3d(normal, normal_c, &clip_vector);
              scenario_soft_ceilings_normalize3d(&clip_vector);
              if (flip) {
                cross_product3d(&clip_vector, normal, normal);
                cross_product3d(normal, &clip_vector, normal);
              } else {
                cross_product3d(normal, &clip_vector, normal);
                cross_product3d(&clip_vector, normal, normal);
              }
            }
          } else {
            *normal = contact->contact_points[stack[0]].triangle->plane.normal;
          }
          if (scenario_soft_ceilings_normalize3d(normal) == 0.0f)
            scale_vector3d(&offset, try_invert ? 1.0f : -1.0f, normal);
          *contact_indices = stack;
          *magnitude_squared = magnitude_squared3d(&direction);
          result = dot_product3d(&offset, normal) < -0.001f;
        }
      }
    }
  }
  return result;
}

// 0x227f30
s_scenario_soft_ceiling *scenario_soft_ceiling_from_name(long name)
{
  scenario *current_scenario = global_scenario;
  s_scenario_soft_ceiling *result = 0;

  for (long soft_ceiling_index = 0; soft_ceiling_index < current_scenario->soft_ceilings.count; soft_ceiling_index++) {
    s_scenario_soft_ceiling *soft_ceiling = &((s_scenario_soft_ceiling *)current_scenario->soft_ceilings.address)[soft_ceiling_index];
    if (soft_ceiling->name == name) {
      unsigned long *flags = TLS_SCENARIO_SOFT_CEILING_STATE;
      if (bit_vector_test(flags, soft_ceiling_index)) {
        result = soft_ceiling;
      }
      break;
    }
  }

  return result;
}

// @address 0x227fb0 scenario_soft_ceilings_is_point_in_triangle
extern "C" bool scenario_soft_ceilings_is_point_in_triangle(const long *count_pointer, const s_soft_ceiling_triangle *shape, const real_point3d *point,
                             // match: 0x227fb0: pointer __restrict qualifier prevents alias reload
                             float tolerance, float *__restrict distance)
{
  long count = *count_pointer;
  float y = point->y;
  float normal_i = shape->plane.normal.i;
  float x = point->x;
  float z = point->z;
  const float *normal = shape->plane.normal.n;
  float plane_distance = z * normal[2] + (x * normal[0] + y * normal[1]) - shape->plane.distance;
  *distance = plane_distance;
  real_point3d position = { x, y, z };
  return (count >= 1 || normal_i * normal_i +
             (shape->plane.normal.j * shape->plane.normal.j + shape->plane.normal.k * shape->plane.normal.k) != 0.0f) &&
         distance3d(&shape->center, &position) - shape->radius < tolerance &&
         fabs(plane_distance) < tolerance;
}

// @address 0x228060 ?reserve_memory@?$t_restricted_allocation_manager@$02$0A@$0A@$1?__tls_set_g_scenario_soft_ceilings_allocator@@YAXPAX@Z@@QAAPAXPBD0IJ@Z
template<>
void *t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_scenario_soft_ceilings_allocator>::reserve_memory(
    const char *name, const char *type, unsigned int size, long alignment_bits)
{
  int member_index = restricted_memory_register_member(&g_restricted_regions[3],
      name, type, size, 0, __tls_set_g_scenes_played_allocator, 0, 0);
  uint offset = restricted_region_get_member_offset(3, member_index);
  void *address = restricted_region_get_base_address(3) + offset;
  int owner = get_current_thread_index();
  m_thread_id = owner;
  m_member_index = member_index;
  TLS_SCENARIO_SOFT_CEILING_STATE = (unsigned long *)address;
  return restricted_region_get_base_address(3) + restricted_region_get_member_offset(3, member_index);
}

// @address 0x228100 ?allocate@?$t_restricted_allocation_manager@$02$0A@$0A@$1?__tls_set_g_scenario_soft_ceilings_allocator@@YAXPAX@Z@@UAAPAXIPBD@Z
template<> void *t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_scenario_soft_ceilings_allocator>::allocate(unsigned int size, const char *name)
{
  return (void *)reserve_memory(name, "", size, 0);
}

// @address 0x228118 ?deallocate@?$t_restricted_allocation_manager@$02$0A@$0A@$1?__tls_set_g_scenario_soft_ceilings_allocator@@YAXPAX@Z@@UAAXPAX@Z
// @address 0x1d6bb0 ?restricted_region_free_member@@YAXJJ@Z
template<> void t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_scenario_soft_ceilings_allocator>::deallocate(void *memory)
{
    int value = m_member_index;
    TLS_SCENARIO_SOFT_CEILING_STATE = 0;
    restricted_region_free_member(3, value);
}

static __forceinline long scenario_soft_ceilings_first_bsp(unsigned long mask)
{
  unsigned long first_bit = mask & -mask;
  return 31 - _CountLeadingZeros(first_bit);
}
static __forceinline long scenario_soft_ceilings_first_active_bsp()
{
  return scenario_soft_ceilings_first_bsp(g_active_structure_bsp_mask);
}
static __forceinline long scenario_soft_ceilings_next_active_bsp(long index)
{
  unsigned long remaining = g_active_structure_bsp_mask & ~((1i64 << (index + 1)) - 1);
  return scenario_soft_ceilings_first_bsp(remaining);
}
static __forceinline const s_soft_ceiling_design *scenario_soft_ceilings_structure_design_get(long structure_bsp_index)
{
  scenario_structure_bsp_reference *bsp =
      &((scenario_structure_bsp_reference *)global_scenario->structure_bsps.address)[structure_bsp_index];
  return bsp->structure_design.index != -1 ?
      (const s_soft_ceiling_design *)global_tag_instances[datum_index_to_absolute_index(bsp->structure_design.index)].definition : 0;
}
static __forceinline float scenario_soft_ceilings_surface_dot(const real_vector3d *a, const real_vector3d *b)
{
  return a->i * b->i + (a->j * b->j + a->k * b->k);
}
static __forceinline float scenario_soft_ceilings_surface_magnitude(const real_vector3d *v)
{
  return sqrtf(v->i * v->i + (v->k * v->k + v->j * v->j));
}
static __forceinline void scenario_soft_ceilings_move_point(real_point3d *point, const real_vector3d *normal, float distance)
{
  point->x = normal->i * distance + point->x;
  point->y = normal->j * distance + point->y;
  point->z = normal->k * distance + point->z;
}
static __forceinline float scenario_soft_ceilings_absolute(float value)
{
  return value >= 0.0f ? value : -value;
}

// @address 0x226918 scenario_soft_ceilings_push_point_out
extern "C" bool scenario_soft_ceilings_push_point_out(real_point3d *point, float height, float radius)
{
  long best_bsp = -1;
  bool result = false;
  float plane_distance;
  phys_vec4 query_center;
  query_center(0) = point->x;
  query_center(1) = point->y;
  query_center(2) = point->z;
  long bsp_index = scenario_soft_ceilings_first_active_bsp();
  float best_distance = FLT_MAX;
  query_center(3) = 0.0f;
  float best_plane_distance;
  float best_angle;
  real_vector3d best_normal;
  real_plane3d best_plane;
  for (; bsp_index != -1; bsp_index = scenario_soft_ceilings_next_active_bsp(bsp_index)) {
    long design_index = ((scenario_structure_bsp_reference *)global_scenario->structure_bsps.address)[bsp_index].structure_design.index;
    if (design_index != -1) {
      s_soft_ceiling_design *design = (s_soft_ceiling_design *)global_tag_instances[datum_index_to_absolute_index(design_index)].definition;
      if (design && design->mopp_size != 0) {
        const phys_bvh_bytecode *code = design->code;
        phys_sphere sphere;
        sphere.m_sphere_center = query_center;
        sphere.m_sphere_center(3) = radius;
        phys_inline_array<phys_bvh_primitive_info, 8192> hits;
        phys_bvh_sphere_vm_query_sphere(code, sphere, &hits);
        for (long hit_index = 0; hit_index < hits.getSize(); ++hit_index) {
          c_soft_ceiling_mopp_key key(hits[hit_index].ID);
          s_soft_ceiling_group *group = &design->groups[key.group_index()];
          if (group->type == 0) {
            s_soft_ceiling_triangle *triangle = &group->triangles[key.triangle_index()];
            if (scenario_soft_ceilings_is_point_in_triangle(
                    &design->plane_count, triangle, point, radius, &plane_distance))
            {
              s_scenario_soft_ceiling *ceiling = scenario_soft_ceiling_from_name(group->name);
              if (ceiling && (ceiling->flags & 4) == 0) {
                real_vector3d normal;
                float distance;
                if (havok_calculate_sphere_vs_triangle_collision(
                        point, radius, &triangle->vertices[0], &triangle->vertices[1], &triangle->vertices[2], &normal, &distance) &&
                    distance - height < 0.0f)
                {
                  float separation = distance + radius - height;
                  if (separation < best_distance) {
                    best_distance = separation;
                    best_bsp = bsp_index;
                    best_plane_distance = plane_distance;
                    float projection = scenario_soft_ceilings_surface_dot(&triangle->plane.normal, &normal);
                    real_vector3d projected;
                    real_vector3d perpendicular;
                    scale_vector3d(&triangle->plane.normal, projection, &projected);
                    subtract_vectors3d(&normal, &projected, &perpendicular);
                    best_angle = (float)atan2(scenario_soft_ceilings_absolute(projection),
                        scenario_soft_ceilings_surface_magnitude(&perpendicular));
                    best_normal = normal;
                    best_plane = triangle->plane;
                  }
                }
              }
            }
          }
        }
      }
    }
  }
  if (best_bsp != -1 && best_plane_distance < 0.0f) {
    if (scenario_soft_ceilings_absolute(best_angle) > 1.483529806f)
      scenario_soft_ceilings_move_point(point, &best_plane.normal, -best_plane_distance);
    else {
      scenario_soft_ceilings_move_point(point, &best_normal, -best_distance);
    }
    result = true;
  }
  return result;
}

// @address 0x226e80 scenario_soft_ceilings_add_collision_features
extern "C" void scenario_soft_ceilings_add_collision_features(
    collision_feature_list *list, const real_point3d *point, float radius, float height, float feature_radius)
{
  phys_vec4 query_center;
  query_center(0) = point->x;
  query_center(1) = point->y;
  query_center(2) = point->z;
  query_center(3) = 0.0f;
  for (long bsp_index = scenario_soft_ceilings_first_active_bsp(); bsp_index != -1;
       bsp_index = scenario_soft_ceilings_next_active_bsp(bsp_index)) {
    s_soft_ceiling_design *design = (s_soft_ceiling_design *)scenario_soft_ceilings_structure_design_get(bsp_index);
    if (design && design->mopp_size != 0) {
        const phys_bvh_bytecode *code = design->code;
        phys_sphere sphere;
        sphere.m_sphere_center = query_center;
        sphere.m_sphere_center(3) = radius;
        phys_inline_array<phys_bvh_primitive_info,8192> hits;
        phys_bvh_sphere_vm_query_sphere(code, sphere, &hits);
        for (long hit_index = 0; hit_index < hits.getSize(); ++hit_index) {
          c_soft_ceiling_mopp_key key(hits[hit_index].ID);
          s_soft_ceiling_group *group = &design->groups[key.group_index()];
          if (group->type == 0) {
            s_soft_ceiling_triangle *triangle = &group->triangles[key.triangle_index()];
            float distance;
            if (scenario_soft_ceilings_is_point_in_triangle(&design->plane_count, triangle, point, radius, &distance)) {
              s_scenario_soft_ceiling *ceiling = scenario_soft_ceiling_from_name(group->name);
              if (ceiling && (ceiling->flags & 4) == 0)
                scenario_soft_ceilings_add_triangle_collision_features(list, bsp_index, triangle, point, radius, height, feature_radius);
            }
          }
        }
      }
  }
}


static __forceinline const s_soft_ceiling_design *scenario_soft_ceilings_design_get(long bsp_index)
{
  long design_index = ((const scenario_structure_bsp_reference *)global_scenario->structure_bsps.address)[bsp_index].structure_design.index;
  return design_index != -1 ?
      (const s_soft_ceiling_design *)global_tag_instances[datum_index_to_absolute_index(design_index)].definition : 0;
}

// @rdata 0x8203385c
static const float k_soft_ceiling_trigger_distance = 0.2f;

// @rdata 0x82022dbc
static const float k_soft_ceiling_clearance_margin = 0.1f;

static __forceinline float scenario_soft_ceilings_query_distance_squared(const real_point3d *a, const real_point3d *b)
{
  real_vector3d delta;
  vector_from_points3d(a, b, &delta);
  return scenario_soft_ceilings_surface_dot(&delta, &delta);
}

static __forceinline float scenario_soft_ceilings_query_plane_distance(const real_point3d *point, const s_soft_ceiling_triangle *plane)
{
  return point->x * plane->plane.normal.i + (point->z * plane->plane.normal.k + point->y * plane->plane.normal.j) - plane->plane.distance;
}

static __forceinline long scenario_soft_ceilings_query_first_bsp()
{
  return 31 - _CountLeadingZeros(-g_active_structure_bsp_mask & g_active_structure_bsp_mask);
}
static __forceinline long scenario_soft_ceilings_query_next_bsp(long index)
{
  unsigned long mask = g_active_structure_bsp_mask & ~(unsigned long)((1i64 << (index + 1)) - 1);
  return 31 - _CountLeadingZeros(-mask & mask);
}

// @address 0x227a18 scenario_soft_ceilings_query_triangles_in_sphere
// match: 0x227a18: pointer __restrict qualifier prevents alias reload
extern "C" void scenario_soft_ceilings_query_triangles_in_sphere(s_soft_ceiling_query_results *results, unsigned long *__restrict hit_mask,
    long flags, bool flag_a, float threshold, const real_point3d *origin, const real_point3d *point, bool flag_b, float max_distance)
{
  phys_vec4 point_quad;
  point_quad.m_simd_quad.v[0] = point->x;
  point_quad.m_simd_quad.v[1] = point->y;
  point_quad.m_simd_quad.v[2] = point->z;
  *hit_mask = 0;
  results->count = 0;
  results->best_clearance = FLT_MAX;
  point_quad.m_simd_quad.v[3] = 0.0f;
  results->best_index = -1;
  float max_dist = max_distance;
    long bsp = scenario_soft_ceilings_query_first_bsp();
  if (bsp != -1) {
    __vector4 query_quad = point_quad.m_simd_quad;
    do {
    const s_soft_ceiling_design *mesh = scenario_soft_ceilings_design_get(bsp);
    if (mesh && mesh->mopp_size) {
        const phys_bvh_bytecode *mopp_code = (const phys_bvh_bytecode *)mesh->code;
        phys_inline_array<phys_bvh_primitive_info, 8192> hits;
        phys_sphere sphere;
        sphere.m_sphere_center.m_simd_quad = query_quad;
        sphere.m_sphere_center(3) = max_distance;
        long last_key_hi = -1;
        long type = -1;
        s_soft_ceiling_group *group;
        float max_d;
        phys_bvh_sphere_vm_query_sphere(mopp_code, sphere, &hits);
        for (long hit_index = 0; hit_index < hits.getSize(); hit_index++) {
          c_soft_ceiling_mopp_key key(hits[hit_index].ID);
          long key_hi = (long)(key.group_index());
          unsigned long key_lo = key.triangle_index();
          if (key_hi != last_key_hi) {
            group = &mesh->groups[key_hi];
            s_scenario_soft_ceiling *ceiling = scenario_soft_ceiling_from_name(group->name);
            if (!ceiling)
              type = -1;
            else {
              type = (short)group->type;
              switch (type) {
              case 0: {
                if (flag_a) {
                  if (ceiling->flags & 8)
                    type = -1;
                } else if ((1 << flags) & 1) {
                  if (ceiling->flags & 1)
                    type = -1;
                } else {
                  if (ceiling->flags & 2)
                    type = -1;
                }
                break;
              }
              case 1:
                max_d = threshold;
                if (((1 << flags) & 0x1003) == 0)
                  type = -1;
                break;
              case 2:
                max_d = k_soft_ceiling_trigger_distance;
                if (!flag_b)
                  type = -1;
                break;
              default:
                type = -1;
                break;
              }
            }
            last_key_hi = key_hi;
          }
          if (type == -1)
            continue;
          s_soft_ceiling_triangle *plane = &group->triangles[key_lo];
          if (type == 0) {
            if (mesh->plane_count < 1) {
              if (plane->plane.normal.i * plane->plane.normal.i +
                  (plane->plane.normal.j * plane->plane.normal.j + plane->plane.normal.k * plane->plane.normal.k) <= 0.0f)
                continue;
            }
            float plane_distance = scenario_soft_ceilings_query_plane_distance(point, plane);
            if (fabs(plane_distance) >= max_dist)
              continue;
            float rr = plane->radius + max_dist;
            if (scenario_soft_ceilings_query_distance_squared(&plane->center, point) >= rr * rr)
              continue;
            real_point3d closest;
            float clearance = real_compute_point_triangle_distance(point, plane->vertices, &closest);
            if (clearance >= max_dist)
              continue;
            const real_point3d *closest_point = &closest;
            long slot = results->count;
            if ((unsigned)slot < 256) {
              results->count = slot + 1;
            }
            else {
              long worst_slot = -1;
              float worst = -FLT_MAX;
              for (long i = 0; i < results->count; i++) {
                if (results->hits[i].clearance > worst) {
                  worst_slot = i;
                  worst = results->hits[i].clearance;
                }
              }
              if (clearance >= worst)
                continue;
              slot = worst_slot;
            }
            if (slot == -1)
              continue;
            s_soft_ceiling_query_hit *hit = &results->hits[slot];
            hit->plane_distance = plane_distance;
            hit->bsp_index = bsp;
            hit->clearance = clearance;
            hit->plane_index = key_lo;
            hit->record_index = (unsigned short)key_hi;
            hit->plane = plane;
            hit->point = *closest_point;
            if (clearance < results->best_clearance) {
              results->best_clearance = clearance;
              results->best_index = slot;
              results->best_point = *closest_point;
              float tightened = clearance + k_soft_ceiling_clearance_margin;
              if (max_dist > tightened)
                max_dist = tightened;
            }
          }
          else {
            if ((*hit_mask & (1UL << type)) == 0) {
              float rr = plane->radius + max_d;
              if (scenario_soft_ceilings_query_distance_squared(&plane->center, point) < rr * rr &&
                  real_compute_point_triangle_distance(point, plane->vertices, 0) < max_d)
                *hit_mask |= 1UL << type;
            }
          }
        }
    }
    bsp = scenario_soft_ceilings_query_next_bsp(bsp);
    } while (bsp != -1);
  }
}

c_scenario_soft_ceilings_allocation_manager g_scenario_soft_ceilings_allocator;
