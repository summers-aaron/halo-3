#ifndef VISIBILITY_VISIBILITY_COLLECTION_STRUCTURE_QUERIES_H
#define VISIBILITY_VISIBILITY_COLLECTION_STRUCTURE_QUERIES_H

class phys_bvh_bytecode;
class phys_sphere;
template<class T> class phys_array;
struct phys_bvh_planes_query_input;
struct s_render_geometry;
struct structure_instanced_geometry_instance;


extern "C" void phys_bvh_float_aabb_vm_query_planes_opt(
    const phys_bvh_bytecode *, const phys_bvh_planes_query_input *, phys_array<unsigned int> *, phys_array<unsigned int> *);
extern "C" void phys_bvh_float_aabb_vm_query_sphere_opt(const phys_bvh_bytecode *, const phys_sphere *, phys_array<unsigned int> *, phys_array<unsigned int> *);
bool convert_part_keys_to_bitvector(const s_render_geometry *, long, unsigned long *, long, long, phys_array<unsigned int> &, phys_array<unsigned int> &);

#endif // VISIBILITY_VISIBILITY_COLLECTION_STRUCTURE_QUERIES_H
