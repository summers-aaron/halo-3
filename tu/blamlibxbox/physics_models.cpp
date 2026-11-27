// blamlibxbox/physics_models.obj: from the records 0x2de920
// @flags /TP /O2 /Oc
#include "math/matrix_math_types.h"
#include "tag_files/tag_types.h"
#include "objects/object_datum.h"
#include "physics/physics_models.h"
#include "cache/cache_files.h"
#include "objects/object_header.h"
#include "tag_files/tag_groups.h"
#include "physics/physics_model_instance.h"
#include "objects/object_material_types.h"

bool physics_model_instance_new(s_physics_model_instance *out,long index) {
 bool result=false;
 object_datum *object=object_get(index);
 s_object_material_definition *definition=(s_object_material_definition *)tag_get('obje',object->definition);
 if (definition->model.index!=-1) {
  s_model_definition *model=(s_model_definition *)tag_get('hlmt',definition->model.index);
  if (model->physics_model_index!=-1) {
   s_physics_model_definition *physics=(s_physics_model_definition *)tag_get('phmo',model->physics_model_index);
   long region_count,node_count;
   char *permutations;
   {
    object_datum *region_object=object_get(index);
    long size=region_object->regions.size;
    char *base=(char *)region_object+region_object->regions.offset;
    if ((bool)((region_object->flags>>11)&1)) size-=16;
    region_count=size/10;
    permutations=base+region_count*8;
   }
   object_datum *node_object=object_get(index);
   node_count=node_object->nodes.size/(long)sizeof(real_matrix4x3);
   real_matrix4x3 *nodes=(real_matrix4x3 *)((char *)node_object+node_object->nodes.offset);
   out->object_index=index;
   out->component_index=object->component;
   object_get_world_matrix(index,&out->world_matrix);
   out->model_index=definition->model.index;
   out->physics_model_index=model->physics_model_index;
   out->model=model;
   out->physics_model=physics;
   out->regions=permutations;
   out->node_matrices=nodes;
   result=true;
  }
 }
 return result;
}
