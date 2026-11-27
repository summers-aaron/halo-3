#ifndef CACHE_PREDICTED_RESOURCES_H
#define CACHE_PREDICTED_RESOURCES_H

#include "objects/cluster_reference.h"

// Functions defined in tu/blamlibxbox/predicted_resources.cpp.

struct c_visitor;
class c_predicted_resources_index_visitor;
class c_global_material_type;
class c_tag_resource_collector;
struct s_index_array;
struct s_render_geometry;
struct s_tag_block;

extern "C" void predicted_resources_find_material_index(long material_id, s_index_array array, long *count);
extern "C" long predicted_resources_get_shader_materials(long tag_index, s_index_array array);
extern "C" void predicted_resources_add_resources_from_render_geometry_instances(c_visitor *visitor,
    const s_render_geometry *render_geometry, s_tag_block *tag_block, long geometry_mesh_index);

void predict_structure_cluster_resources(c_tag_resource_collector *, long, const s_cluster_reference &);

extern "C" void predicted_resources_iterate_object_instance(c_visitor *visitor, long object_index);
extern "C" void predicted_resources_iterate_object_definition(c_visitor *visitor, long object_index);
extern "C" void predicted_resources_add_resources_from_render_geometry_parts(c_visitor *visitor,
    const s_render_geometry *definition, s_tag_block *instance, long group_index);
extern "C" void predicted_resources_iterate_model_permutation(c_visitor *visitor, long tag_index, long region_index, long permutation_index);
extern "C" void predicted_resources_iterate_geometry_group(c_predicted_resources_index_visitor *visitor,
    const void *geometry, const void *auxiliary, long group_index);
void predicted_resources_iterate_bsp_materials(c_visitor *visitor, long tag_index, long tag_datum, s_cluster_reference key);
extern "C" void predicted_resources_iterate_material_resources(c_visitor *visitor, long unused, c_global_material_type material_type);
extern "C" void predicted_resources_iterate_bsp_permutation(c_predicted_resources_index_visitor *visitor,
    long tag_index, s_cluster_reference key);

#endif // CACHE_PREDICTED_RESOURCES_H
