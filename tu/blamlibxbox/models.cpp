// blamlibxbox/models.obj: from the records 0x10b140 0x262f60 0x262fc8 0x263118 0x2631c8 0x2635a8 0x263810 0x263870 0x2638d0 0x263af8
// @flags /TP /O2 /Oc
#include "math/matrix_math_types.h"
#include "memory/tls_globals.h"
#include "math/random_math.h"
#include "tag_files/tag_types.h"
#include "effects/effects.h"
#include "effects/effect_creation_data.h"
#include "models/models.h"
#include "models/model_variant_types.h"
#include "models/render_model_definitions.h"
#include "math/matrix_math.h"
#include "objects/object_header.h"
// @address 0x10b140 ?model_adjust_node_matrices_children_recursive@@YAXPBUrender_model_definition@@PBUreal_matrix4x3@@JJPAU2@@Z
// @address 0x262f60 ?model_find_region_by_name@@YAJJJ@Z
// @address 0x262fc8 ?model_instance_group_choose_instances@@YAXPBUs_model_definition@@JPAK@Z
// @address 0x263118 ?model_instance_group_calculate_all_possible_instances@@YAXJJPAK@Z
// @address 0x2631c8 ?model_select_initial_permutation@@YAXJJPADPAUobject_region_information@@JPAUs_model_customization_region@@J@Z
// @address 0x2635a8 ?model_set_state_or_property_per_region@@YA_NJJJJJ_NJFPADPAUobject_region_information@@@Z
// @address 0x263810 ?model_find_region_by_physics_region_index@@YAJJJ@Z
// @address 0x263870 ?model_find_node@@YAJJJ@Z
// @address 0x2638d0 ?model_find_permutation_possibilities@@YAXPBUs_model_variant_region@@JJJ_NPAJJPAUs_model_permutation_possibility@@@Z
// @address 0x263af8 ?model_select_permutation_possibility@@YAJJPBUs_model_permutation_possibility@@@Z


#include "models/model_types.h"
#include "tag_files/tag_groups.h"
#include "math/random_math_range.h"
extern "C" {
}







void model_adjust_node_matrices_children_recursive(const render_model_definition *render_model, const real_matrix4x3 *matrix,
    long node_index, long node_count, real_matrix4x3 *node_matrices)
{
    unsigned char stack[256];
    long depth = -1;
    const render_model_node *node = (const render_model_node *)render_model->nodes.address + node_index;
    long child_index = node->first_child_node_index;
    while (child_index != -1) {
        node = (const render_model_node *)render_model->nodes.address + child_index;
        matrix4x3_compute_product(matrix, &node_matrices[child_index], &node_matrices[child_index]);
        long next_index = node->next_sibling_node_index;
        if (next_index != -1) {
            if (node->first_child_node_index != -1)
                stack[++depth] = (unsigned char)node->first_child_node_index;
            child_index = next_index;
        } else {
            child_index = node->first_child_node_index;
            if (child_index == -1 && depth >= 0)
                child_index = stack[depth--];
        }
    }
}

long model_find_region_by_name(long model_index, long name)
{
    long result = -1;
    if (model_index != -1 && name != 0) {
        s_model_definition *model = TAG_GET('hlmt', s_model_definition, model_index);
        for (long region_index = 0; region_index < model->regions.count; region_index++) {
            s_model_region *region = (s_model_region *)tag_block_get_element_with_size(&model->regions, region_index, sizeof(s_model_region));
            if (region->name == name) {
                result = region_index;
                break;
            }
        }
    }
    return result;
}

void model_instance_group_choose_instances(const s_model_definition *model, long group_index, unsigned long *instances)
{
    const s_model_instance_group *group = (const s_model_instance_group *)tag_block_get_element_with_size(
        &model->groups, group_index, sizeof(s_model_instance_group));
    float random = _real_random_range(get_local_random_seed_address(), 0.0f, group->weight);
    bool choose_all = group->mode == 1;
    for (long member_index = 0; member_index < group->members.count; ++member_index) {
        const s_model_instance_group_member *member = (const s_model_instance_group_member *)tag_block_get_element_with_size(
            &group->members, member_index, sizeof(s_model_instance_group_member));
        if (choose_all || random < member->weight) {
            if (member->subgroup != -1) {
                unsigned long subgroup_instances[2] = {0, 0};
                model_instance_group_choose_instances(model, member->subgroup, subgroup_instances);
                instances[0] |= subgroup_instances[0];
                instances[1] |= subgroup_instances[1];
            }
            for (long word = 0; word < 2; ++word)
                instances[word] |= member->instances[word];
            if (!choose_all)
                break;
        } else {
            random -= member->weight;
        }
    }
}

void model_instance_group_calculate_all_possible_instances(long model_index, long group_index, unsigned long *instances)
{
    s_model_definition *model = TAG_GET('hlmt', s_model_definition, model_index);
    s_model_instance_group *group = (s_model_instance_group *)tag_block_get_element_with_size(&model->groups, group_index, sizeof(s_model_instance_group));
    for (long member_index = 0; member_index < group->members.count; ++member_index) {
        s_model_instance_group_member *member = (s_model_instance_group_member *)tag_block_get_element_with_size(
            &group->members, member_index, sizeof(s_model_instance_group_member));
        if (member->subgroup != -1)
            model_instance_group_calculate_all_possible_instances(model_index, member->subgroup, instances);
        for (long word = 0; word < 2; ++word)
            instances[word] |= member->instances[word];
    }
}

struct s_model_variant_region;

static __forceinline long models_find_variant_region_by_region_index(long model_index, long variant_index, long region_index)
{
    long result = -1;
    if (model_index != -1 && variant_index != -1 && region_index != -1) {
        s_model_definition *model = TAG_GET('hlmt', s_model_definition, model_index);
        s_model_variant *variant = (s_model_variant *)tag_block_get_element_with_size(&model->variants, variant_index, sizeof(s_model_variant));
        result = variant->region_indices[region_index];
    }
    return result;
}
static __forceinline void models_initialize_permutation_possibility(long permutation_index, s_model_permutation_possibility *possibility)
{
    possibility->permutation_index = permutation_index;
    possibility->variant_index = -1;
    possibility->type = 0;
    possibility->property = 0;
    possibility->effect = -1;
    possibility->flags = 0;
    possibility->weight = 1.0f;
}

void model_select_initial_permutation(long model_index, long variant_index, char *region_permutation_indices,
    object_region_information *region_information, long region_mask, s_model_customization_region *customization_regions,
    long customization_region_count)
{
    s_model_definition *model = TAG_GET('hlmt', s_model_definition, model_index);
    long region_index;
    for (region_index = 0; region_index < model->regions.count; ++region_index) {
        region_permutation_indices[region_index] = -1;
        region_information[region_index].variant_index = -1;
        region_information[region_index].state = 0;
        region_information[region_index].properties = 0;
        region_information[region_index].effect = -1;
    }
    for (region_index = 0; region_index < model->regions.count; ++region_index) {
        s_model_region *region = (s_model_region *)tag_block_get_element_with_size(&model->regions, region_index, sizeof(s_model_region));
        long possibility_count = 0;
        s_model_permutation_possibility possibilities[32];
        if (variant_index != -1) {
            s_model_variant *variant = (s_model_variant *)tag_block_get_element_with_size(&model->variants, variant_index, sizeof(s_model_variant));
            long variant_region_index = models_find_variant_region_by_region_index(model_index, variant_index, region_index);
            if (variant_region_index != -1) {
                const s_model_variant_region *variant_region = (const s_model_variant_region *)tag_block_get_element_with_size(
                    &variant->regions, variant_region_index, sizeof(s_model_variant_region));
                model_find_permutation_possibilities(variant_region, -1, 0, 0, true, &possibility_count, 32, possibilities);
            }
        }
        if (possibility_count == 0) {
            for (long permutation_index = 0; permutation_index < region->permutations.count; ++permutation_index) {
                s_model_region_permutation *permutation = (s_model_region_permutation *)tag_block_get_element_with_size(
                    &region->permutations, permutation_index, sizeof(s_model_region_permutation));
                if ((permutation->flags & 1) == 0) {
                    models_initialize_permutation_possibility(permutation_index, &possibilities[possibility_count]);
                    ++possibility_count;
                }
            }
            if (possibility_count == 0 && region->permutations.count > 0) {
                models_initialize_permutation_possibility(0, &possibilities[possibility_count]);
                ++possibility_count;
            }
        }
        if ((region_mask & (1 << region_index)) == 0) {
            long chosen_possibility_index = -1;
            for (long customization_index = 0; customization_index < customization_region_count; ++customization_index) {
                if (region->name == customization_regions[customization_index].region_name) {
                    for (long possibility_index = 0; possibility_index < possibility_count; ++possibility_index) {
                        long permutation_index = possibilities[possibility_index].permutation_index;
                        if (permutation_index != -1) {
                            s_model_region_permutation *permutation = (s_model_region_permutation *)tag_block_get_element_with_size(
                                &region->permutations, permutation_index, sizeof(s_model_region_permutation));
                            if (permutation->name == customization_regions[customization_index].permutation_name) {
                                chosen_possibility_index = possibility_index;
                                break;
                            }
                        }
                    }
                    break;
                }
            }
            if (chosen_possibility_index == -1)
                chosen_possibility_index = model_select_permutation_possibility(possibility_count, possibilities);
            if (chosen_possibility_index != -1) {
                s_model_permutation_possibility *chosen_possibility = &possibilities[chosen_possibility_index];
                region_permutation_indices[region_index] = (char)chosen_possibility->permutation_index;
                region_information[region_index].variant_index = (char)chosen_possibility->variant_index;
                region_information[region_index].properties = (unsigned char)chosen_possibility->property;
                region_information[region_index].state = (signed char)chosen_possibility->type;
            }
        }
    }
}

struct s_effect_creation_data;

bool model_set_state_or_property_per_region(long object_index, long model_index, long variant_index, long requested_region,
    long model_state, bool force, long property_flag, short region_mask, char *region_permutation_indices,
    object_region_information *region_information)
{
    s_model_definition *model = TAG_GET('hlmt', s_model_definition, model_index);
    bool result = false;
    for (long region_index = 0; region_index < model->regions.count; region_index++) {
        object_region_information *region = &region_information[region_index];
        long state = model_state == -1 ? region->state : model_state;
        long properties = region->properties;
        if (property_flag != -1) {
            if (region_mask & (1 << region_index)) properties |= 1 << property_flag;
            else properties &= ~(1 << property_flag);
        }
        if ((state >= region->state || force) && ((unsigned char)region_permutation_indices[region_index] != 255 || force) &&
            (requested_region == -1 || requested_region == region_index))
        {
            long variant_region_index = models_find_variant_region_by_region_index(model_index, variant_index, region_index);
            long possibility_count = 0;
            s_model_permutation_possibility possibilities[32];
            if (variant_region_index != -1) {
                s_model_variant *variant = (s_model_variant *)tag_block_get_element_with_size(&model->variants, variant_index, sizeof(s_model_variant));
                const s_model_variant_region *variant_region = (const s_model_variant_region *)tag_block_get_element_with_size(
                    &variant->regions, variant_region_index, sizeof(s_model_variant_region));
                model_find_permutation_possibilities(
                    variant_region, (signed char)region->variant_index, state, properties, false, &possibility_count, 32, possibilities);
            }
            long possibility_index = model_select_permutation_possibility(possibility_count, possibilities);
            if (possibility_index != -1) {
                const s_model_permutation_possibility *possibility = &possibilities[possibility_index];
                region_permutation_indices[region_index] = (char)possibility->permutation_index;
                region->variant_index = (char)possibility->variant_index;
                if (region->state != possibility->type) {
                    if (region->effect != -1) effect_stop(region->effect, true);
                    if (possibility->effect != -1) {
                        s_effect_creation_data creation;
                        creation.flags = 0;
                        effect_creation_data_initialize(&creation);
                        creation.definition_index = possibility->effect;
                        creation.structure_bsp_index = object_index;
                        creation.lightprobe_slot_index = 574;
                        creation.user_mask = 574;
                        creation.parent_effect_index = possibility->flags;
                        creation.flags = 3;
                        region->effect = effect_new_from_creation_data(&creation);
                    }
                }
                region->state = (signed char)possibility->type;
                region->properties = (unsigned char)possibility->property;
                result = true;
            }
        }
    }
    return result;
}

long model_find_region_by_physics_region_index(long model_index, long physics_region_index)
{
    s_model_definition *model = TAG_GET('hlmt', s_model_definition, model_index);
    long result = -1;
    for (long region_index = 0; region_index < model->regions.count; region_index++) {
        s_model_region *region = (s_model_region *)tag_block_get_element_with_size(&model->regions, region_index, sizeof(s_model_region));
        if (region->physics_region_index == physics_region_index) {
            result = region_index;
            break;
        }
    }
    return result;
}

long model_find_node(long model_index, long name)
{
    long result = -1;
    if (model_index != -1) {
        s_model_definition *model = TAG_GET('hlmt', s_model_definition, model_index);
        for (long node_index = 0; node_index < model->nodes.count; node_index++) {
            s_model_node *node = (s_model_node *)tag_block_get_element_with_size(&model->nodes, node_index, sizeof(s_model_node));
            if (node->name == name) {
                result = node_index;
                break;
            }
        }
    }
    return result;
}



void model_find_permutation_possibilities(const s_model_variant_region *variant_region, long chosen_variant, long type, long property,
    // match: 0x2638d0: keeps *possibility_count in a register across the possibilities stores instead of an lwz reload before each store
    bool use_random, long *__restrict possibility_count_parameter, long maximum_possibility_count,
    s_model_permutation_possibility *possibilities)
{
    // match: 0x2638d0: folds the count guard into cmpwi against 32 instead of a cmpw against the loaded parameter
    __assume(maximum_possibility_count == 32);
    long *possibility_count = possibility_count_parameter;
    long passes = 1;
    while (passes-- != 0) {
        for (long variant_index = 0; variant_index < variant_region->permutations.count; variant_index++) {
            if (chosen_variant == -1 || chosen_variant == variant_index) {
                const s_model_permutation *permutation = (const s_model_permutation *)tag_block_get_element_with_size(
                    &variant_region->permutations, variant_index, sizeof(s_model_permutation));
                bool found = false;
                long permutation_index = -1;
                long effect = -1;
                long flags = 0;
                long selected_type = type;
                unsigned char selected_property = (unsigned char)property;
                if (use_random && permutation->states.count > 0) {
                    float random_value = _real_random(tls_random_seed(), __FILE__, __FUNCTION__, __LINE__);
                    float accumulated_weight = 0.0f;
                    for (long state_index = 0; state_index < permutation->states.count; state_index++) {
                        const s_model_permutation_state *state = (const s_model_permutation_state *)tag_block_get_element_with_size(
                            &permutation->states, state_index, sizeof(s_model_permutation_state));
                        if (state->weight + accumulated_weight > random_value) {
                            permutation_index = state->permutation_index;
                            effect = state->looping_effect.index;
                            flags = state->flags;
                            selected_type = state->type;
                            selected_property = state->property;
                            found = true;
                            break;
                        }
                        accumulated_weight += state->weight;
                    }
                    if (!found) {
                        permutation_index = permutation->index;
                        found = true;
                    }
                } else if (type == 0 && property == 0) {
                    permutation_index = permutation->index;
                    found = true;
                } else {
                    for (long state_index = 0; state_index < permutation->states.count; state_index++) {
                        const s_model_permutation_state *state = (const s_model_permutation_state *)tag_block_get_element_with_size(
                            &permutation->states, state_index, sizeof(s_model_permutation_state));
                        if (state->type == type && state->property == property) {
                            permutation_index = state->permutation_index;
                            effect = state->looping_effect.index;
                            flags = state->flags;
                            found = true;
                            break;
                        }
                    }
                }
                if (found && *possibility_count < maximum_possibility_count) {
                    possibilities[*possibility_count].permutation_index = permutation_index;
                    possibilities[*possibility_count].variant_index = variant_index;
                    possibilities[*possibility_count].type = selected_type;
                    possibilities[*possibility_count].property = selected_property;
                    possibilities[*possibility_count].effect = effect;
                    possibilities[*possibility_count].flags = flags;
                    possibilities[*possibility_count].weight = permutation->weight;
                    ++*possibility_count;
                }
            }
        }
        if (*possibility_count == 0 && property != 0) {
            ++passes;
            property = 0;
        }
    }
}

long model_select_permutation_possibility(long possibility_count, const s_model_permutation_possibility *possibilities)
{
    // match: 0x263af8: shifts load temporaries so both weight sums keep the total in the first fadds operand.
    __assume(possibilities != 0);
    long result = -1;
    if (possibility_count == 0) {
        result = -1;
    } else if (possibility_count == 1) {
        result = 0;
    } else if (possibility_count > 1) {
        // match: 0x263af8: same temporary shift for the remainder loop of the total weight sum.
        __assume(possibility_count > 1);
        float total_weight = 0.0f;
        long index;
        for (index = 0; index < possibility_count; index++)
            total_weight += possibilities[index].weight;
        float random_weight = _real_random_range(tls_random_seed(), 0.0f, total_weight);
        float cumulative_weight = 0.0f;
        for (index = 0; index < possibility_count; index++) {
            cumulative_weight += possibilities[index].weight;
            if (random_weight <= cumulative_weight || index == possibility_count - 1) {
                result = index;
                break;
            }
        }
    }
    return result;
}
