#ifndef MODELS_MODELS_H
#define MODELS_MODELS_H

// Functions defined in tu/blamlibxbox/models.cpp.

struct object_region_information;
struct real_matrix4x3;
struct render_model_definition;
struct s_model_customization_region;
struct s_model_definition;

void model_adjust_node_matrices_children_recursive(const render_model_definition *render_model, const real_matrix4x3 *matrix, long node_index,
    long node_count, real_matrix4x3 *node_matrices);
long model_find_region_by_name(long model_index, long name);
void model_instance_group_choose_instances(const s_model_definition *model, long group_index, unsigned long *instances);
void model_instance_group_calculate_all_possible_instances(long model_index, long group_index, unsigned long *instances);
void model_select_initial_permutation(long model_index, long variant_index, char *region_permutation_indices, object_region_information *region_information,
    long region_mask, s_model_customization_region *customization_regions, long customization_region_count);
bool model_set_state_or_property_per_region(long object_index, long model_index, long variant_index, long requested_region, long model_state, bool force,
    long property_flag, short region_mask, char *region_permutation_indices, object_region_information *region_information);
long model_find_region_by_physics_region_index(long model_index, long physics_region_index);
long model_find_node(long model_index, long name);

struct s_model_permutation_possibility;

long model_select_permutation_possibility(long possibility_count, const s_model_permutation_possibility *possibilities);

struct s_model_variant_region;

void model_find_permutation_possibilities(const s_model_variant_region *variant_region, long chosen_variant, long type, long property, bool use_random,
    long *possibility_count_parameter, long maximum_possibility_count, s_model_permutation_possibility *possibilities);

#endif // MODELS_MODELS_H
