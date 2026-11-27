#ifndef ANIMATIONS_ANIMATION_SEARCH_PATH_ITERATOR_H
#define ANIMATIONS_ANIMATION_SEARCH_PATH_ITERATOR_H

#include <stddef.h>
#include "tag_files/tag_types.h"
#include "math/real_math_types.h"
#include "animations/animation_id.h"
struct animation_aiming_screen_bounds;
#include "animations/animation_definition_types.h"
#include "cseries/cseries.h"

enum e_foot_tracking_state;
class ik_point_iterator;
class c_model_animation_tag_resource_member;
struct animation_graph_node;
struct render_model_definition;
class c_animation_data;
struct real_matrix4x3;

class c_model_animation_graph {
public:
  const c_model_animation *get_animation(c_animation_id reference) const;
  // match: 0x1206a0: noalias keeps default_transition in r15 for a direct epilogue store; without it the value is reloaded from the stack (gprlr_16)
  __declspec(noalias) c_animation_id find_transition(long mode, long weapon_class, long weapon_type, long state, char direction, char region,
      long destination_mode, long destination_state, char destination_direction, char destination_region, char type) const;
  void get_graph_and_animation(c_animation_id reference, const c_model_animation_graph **out_definition, const c_model_animation **out_part) const;
  static __forceinline void get_animation_root_orientation(
      long graph_index, c_animation_id animation_id, float animation_time, const render_model_definition *render_model,
      real_orientation *orientation);
  static void get_animation_root_velocity(long graph_index, c_animation_id animation_id, float animation_time, float playback_rate,
      const render_model_definition *render_model, real_vector3d *velocity);
  __forceinline const c_animation_mode *find_mode(long name) const
  {
    return (const c_animation_mode *)find_named_block_entry_with_size(&m_modes, name, sizeof(c_animation_mode));
  }
  __forceinline const c_model_animation_graph *get_inherited_graph(long index) const;
  __forceinline const c_model_animation *get_local_animation(long index) const;
  __forceinline const animation_aiming_screen_bounds *get_local_blend_screen_bounds(long index) const;
  const c_weapon_class *find_weapon_class(long mode, long weapon_class) const;
  bool find_next_mode_ik_point(ik_point_iterator *iterator) const;
  bool find_next_weapon_ik_point(ik_point_iterator *iterator) const;
  bool animation_contains_movement_data(c_animation_id reference) const;
  __forceinline const c_model_animation_tag_resource_member *get_resource_group_member(c_animation_id reference) const;
  e_foot_tracking_state find_foot_tracking_default_state(long member_index, long mode_name) const;
  // match: 0x290098: without noalias the two find_animation results in reset_facial_animation_channels spill at 84/88 instead of 80/84
  __declspec(noalias) c_animation_id find_animation(long name) const;
  c_animation_id find_action(long mode, long weapon_class, long weapon_type, long action, long *out_mode, long *out_class, long *out_type) const;
  c_animation_id find_overlay(long mode, long weapon_class, long weapon_type, long overlay, long *out_mode, long *out_class, long *out_type) const;
  c_animation_id find_damage_animation(long mode, long weapon_class, long weapon_type, long damage, long direction, long region,
      long *out_mode, long *out_class, long *out_type) const;
  const animation_aiming_screen_bounds *get_blend_screen_bounds(c_animation_id reference) const;
  bool get_animation_data(c_animation_id reference, c_animation_data *out_data) const;
  bool get_local_animation_data(const c_model_animation *animation, c_animation_data *out_data) const;
  // match: 0x2fc8d0: __restrict on matrices keeps the shipped PIAU mangling; plain pointers emit PAU and miss the @address name
  void node_matrices_from_orientations_with_gun_hand_swap(real_matrix4x3 *__restrict matrices,
      // match: 0x2fc8d0: __restrict on orientations/base keeps the shipped PIBU mangling; plain pointers emit PBU and miss the @address name
      const real_orientation *__restrict orientations, const real_matrix4x3 *__restrict base_matrices, long weapon_root_index,
      long opposite_hand_node_index) const;
private:
  const c_model_animation_tag_resource_member *get_local_resource_group_member(long group_index, long member_index) const;
  // match: 0x2fc828: __restrict on matrices keeps the shipped PIAU mangling; plain pointers emit PAU and miss the @address name
  void internal_node_matrices_from_orientations(long start_index, long count, real_matrix4x3 *__restrict matrices,
      // match: 0x2fc828: __restrict on orientations/base keeps the shipped PIBU mangling; plain pointers emit PBU and miss the @address name
      const real_orientation *__restrict orientations, const real_matrix4x3 *__restrict base_matrices) const;
public:
  __forceinline const animation_graph_node *get_node(long node_index) const;
  __forceinline short get_node_count() const { return (short)m_nodes.count; }
  __forceinline void node_matrices_from_orientations(real_matrix4x3 *matrices,
      const real_orientation *orientations, const real_matrix4x3 *base_matrices) const
  {
    internal_node_matrices_from_orientations(0, m_nodes.count, matrices, orientations, base_matrices);
  }
  s_tag_reference m_parent_animation_graph;
  unsigned char m_inheritance_flags;
  unsigned char m_private_flags;
  short m_animation_codec_pack;
  s_tag_block m_nodes;
  s_tag_block m_sound_references;
  s_tag_block m_effect_references;
  s_tag_block m_blend_screens;
  s_tag_block m_foot_tracking;
  s_tag_block m_animations;
  s_tag_block m_modes;
  s_tag_block m_vehicle_suspensions;
  s_tag_block m_object_overlays;
  s_tag_block m_inheritence_list;
  s_tag_block m_weapon_type_remaps;
  c_static_flags<255> node_masks[2];
  s_tag_data m_last_import_results;
  s_tag_block m_additional_node_data;
  s_tag_block m_resource_groups;
};

#if defined(_M_PPCBE) || defined(_XBOX)
typedef char weapon_type_size[(sizeof(c_weapon_type) == 52) ? 1 : -1];
typedef char weapon_class_size[(sizeof(c_weapon_class) == 28) ? 1 : -1];
typedef char animation_mode_size[(sizeof(c_animation_mode) == 40) ? 1 : -1];
typedef char model_animation_size[(sizeof(c_model_animation) == 0x88) ? 1 : -1];
typedef char animation_graph_nodes_offset[(offsetof(c_model_animation_graph, m_nodes) == 0x14) ? 1 : -1];
typedef char animation_graph_modes_offset[(offsetof(c_model_animation_graph, m_modes) == 0x5c) ? 1 : -1];
typedef char animation_graph_inheritence_offset[(offsetof(c_model_animation_graph, m_inheritence_list) == 0x80) ? 1 : -1];
typedef char animation_graph_node_masks_offset[(offsetof(c_model_animation_graph, node_masks) == 0x98) ? 1 : -1];
typedef char animation_graph_additional_node_data_offset[(offsetof(c_model_animation_graph, m_additional_node_data) == 0xec) ? 1 : -1];
typedef char animation_graph_resource_groups_offset[(offsetof(c_model_animation_graph, m_resource_groups) == 0xf8) ? 1 : -1];
typedef char model_animation_flags_offset[(offsetof(c_model_animation, animation_flags) == 0xa) ? 1 : -1];
typedef char animation_search_path_iterator_size[(sizeof(void *) != 4 || (sizeof(c_animation_search_path_iterator) == 28)) ? 1 : -1];
#endif

#endif // ANIMATIONS_ANIMATION_SEARCH_PATH_ITERATOR_H
