// blamlibxbox/collision_instanced_geometry_definitions.obj: from the records 0xb09f8 0xb0a50 0x25f618 0x260340 0x262eb0
// @flags /TP /O2 /Oc
#include "phys/phys_collide.h"
#include "tag_files/tag_groups.h"
#include "physics/collision_instanced_geometry_definitions.h"
#include "physics/havok_mopp_code_definitions.h"
#include "physics/collision_instanced_geometry_definition_types.h"
#include <stddef.h>
#include <new>
#include "memory/datum_index.h"
#include "structures/structure_bsp_tag.h"
#ifdef HALO3_64BIT
#include "physics/havok_tag_unpack_64.h"
#endif
// @address 0x25f618 ??_Gphys_compound_shape@@UAAPAXI@Z
// @address 0x260340 ??_Ephys_compound_shape@@WBA@AAPAXI@Z
// @address 0xb09f8 ?get_child_shape@c_cluster_collision_instanced_geometry_shape@@UBAPBVphys_shape@@IAAY0CAA@D@Z
// @address 0xb0a50 ?trace_ray_with_collector@c_instanced_geometry_shape@@UBAXABUphys_ray_input@@ABVphys_collision_proxy@@AAVphys_ray_hit_collector@@@Z

// @address 0x22f4 ??_7c_instanced_geometry_shape@@6B@
// @address 0x1c0038 ??_Gc_instanced_geometry_shape@@UAAPAXI@Z
// @address 0x1c0038 ??_Ec_instanced_geometry_shape@@UAAPAXI@Z
class c_instanced_geometry_shape : public phys_shape {
public:
    __forceinline c_instanced_geometry_shape(const structure_bsp *structure, long instance_index);
    virtual __forceinline ~c_instanced_geometry_shape();
protected:
    virtual void get_aabb_impl(const phys_transform &, float, phys_aabb &) const;
    virtual phys_bool trace_ray_impl(const phys_ray_input &, phys_ray_output &) const;
public:
    virtual float get_max_projection(const phys_vec4 &) const;
    virtual void trace_ray_with_collector(const phys_ray_input &, const phys_collision_proxy &, phys_ray_hit_collector &) const;
    const structure_bsp *m_structure_bsp;
    long m_instance_index;
};
class c_cluster_collision_instanced_geometry_shape : public phys_compound_shape {
public:
    virtual __forceinline ~c_cluster_collision_instanced_geometry_shape();
    virtual int get_child_shape_count() const;
    virtual unsigned int get_first_key() const;
    virtual unsigned int get_next_key(unsigned int) const;
    virtual const phys_shape *get_child_shape(unsigned int key, char (&buffer)[512]) const;
    __forceinline c_cluster_collision_instanced_geometry_shape(phys_fixup_tag flag);
    s_tag_block m_instances;
    long m_structure_bsp_index;
    long m_cluster_index;
protected:
    virtual phys_bool trace_ray_impl(const phys_ray_input &, phys_ray_output &) const;
    virtual void trace_ray_with_collector(const phys_ray_input &, const phys_collision_proxy &, phys_ray_hit_collector &) const;
    virtual void get_aabb_impl(const phys_transform &, float, phys_aabb &) const;
    virtual float get_max_projection(const phys_vec4 &) const;
};
class c_cluster_collision_instanced_geometry_mopp_bv_tree_shape : public phys_bvh_tree_shape {
public:
    virtual __forceinline ~c_cluster_collision_instanced_geometry_mopp_bv_tree_shape();
    __forceinline c_cluster_collision_instanced_geometry_mopp_bv_tree_shape(const phys_compound_shape *collection, const phys_bvh_bytecode *code);
};

static __forceinline void collision_instanced_geometry_definitions_update_shape(phys_shape *shape, long user_data) {
    shape->m_object_ref_count = 128;
    shape->m_user_data = user_data;
    shape->m_mem_size_and_flags = 0;
}
__forceinline c_instanced_geometry_shape::~c_instanced_geometry_shape() {}
__forceinline c_instanced_geometry_shape::c_instanced_geometry_shape(const structure_bsp *structure, long instance_index)
    : phys_shape(PHYS_SHAPE_INVALID), m_structure_bsp(structure), m_instance_index(instance_index) {}
__forceinline c_cluster_collision_instanced_geometry_shape::~c_cluster_collision_instanced_geometry_shape() {}
__forceinline c_cluster_collision_instanced_geometry_shape::c_cluster_collision_instanced_geometry_shape(phys_fixup_tag flag)
    : phys_compound_shape(flag) {
    collision_instanced_geometry_definitions_update_shape(this, 0);
}
__forceinline c_cluster_collision_instanced_geometry_mopp_bv_tree_shape::~c_cluster_collision_instanced_geometry_mopp_bv_tree_shape() {}
__forceinline c_cluster_collision_instanced_geometry_mopp_bv_tree_shape::c_cluster_collision_instanced_geometry_mopp_bv_tree_shape(
    const phys_compound_shape *collection, const phys_bvh_bytecode *code) : phys_bvh_tree_shape(collection, code) {
    collision_instanced_geometry_definitions_update_shape(this, 0);
}

const phys_shape *c_cluster_collision_instanced_geometry_shape::get_child_shape(unsigned int key, char (&buffer)[512]) const
{
    const structure_bsp *structure = (const structure_bsp *)
        global_tag_instances[datum_index_to_absolute_index(m_structure_bsp_index)].definition;
    c_instanced_geometry_shape *child = (c_instanced_geometry_shape *)buffer;
    if (child != 0) {
        new (child) c_instanced_geometry_shape(structure, key);
        return (const phys_shape *)child;
    }
    return 0;
}

void c_instanced_geometry_shape::trace_ray_with_collector(const phys_ray_input &input, const phys_collision_proxy &body,
    phys_ray_hit_collector &collector) const
{
    phys_ray_output output;
    collector.add_ray_hit(body, output);
}

static __forceinline phys_bvh_bytecode *collision_instanced_geometry_definitions_get_mopp_code(const s_tag_block *block, long index) {
    return (phys_bvh_bytecode *)tag_block_get_element_with_size(block, index, sizeof(s_havok_mopp_code_definition));
}
// @address 0x262eb0 ?collision_instanced_geometry_definition_fixup@@YAXPAUs_collision_instanced_geometry_definition@@JJ@Z
void collision_instanced_geometry_definition_fixup(s_collision_instanced_geometry_definition *definition, long structure_bsp_index, long instance_index)
{
    ::havok_mopp_code_definitions_fixup(&definition->m_mopp_codes);
#ifdef HALO3_64BIT
    if (*(unsigned long *)definition->m_cluster_shape == k_tag_block_unpacked_64_sentinel)
        return;

    if (definition->m_mopp_codes.count > 0) {
        struct s_cluster_instanced_geometry_shapes_64 {
            c_cluster_collision_instanced_geometry_shape cluster_shape;
            c_cluster_collision_instanced_geometry_mopp_bv_tree_shape mopp_shape;
        };

        s_tag_block instances;
        memcpy(&instances, definition->m_cluster_shape + 0x18, sizeof(s_tag_block));
        long cluster_index;
        memcpy(&cluster_index, definition->m_cluster_shape + 0x28, sizeof(long));

        s_cluster_instanced_geometry_shapes_64 *pair = (s_cluster_instanced_geometry_shapes_64 *)
            havok_unpack_alloc_64(sizeof(s_cluster_instanced_geometry_shapes_64));
        c_cluster_collision_instanced_geometry_shape *cluster_shape = new (&pair->cluster_shape)
            c_cluster_collision_instanced_geometry_shape(phys_fixup_tag());
        cluster_shape->m_instances = instances;
        cluster_shape->m_structure_bsp_index = structure_bsp_index;
        cluster_shape->m_cluster_index = cluster_index;

        c_cluster_collision_instanced_geometry_mopp_bv_tree_shape *mopp_shape = new (&pair->mopp_shape)
            c_cluster_collision_instanced_geometry_mopp_bv_tree_shape(&pair->cluster_shape,
                collision_instanced_geometry_definitions_get_mopp_code(&definition->m_mopp_codes, 0));

        const phys_shape *p = mopp_shape;
        memcpy(definition->m_mopp_shape, &p, sizeof(p));
        *(unsigned long *)definition->m_cluster_shape = k_tag_block_unpacked_64_sentinel;
    }
#else
    if (definition->m_mopp_codes.count > 0) {
        c_cluster_collision_instanced_geometry_shape *cluster_shape = new (definition->m_cluster_shape)
            c_cluster_collision_instanced_geometry_shape(phys_fixup_tag());
        c_cluster_collision_instanced_geometry_mopp_bv_tree_shape *mopp_shape = new (definition->m_mopp_shape)
            c_cluster_collision_instanced_geometry_mopp_bv_tree_shape((const phys_compound_shape *)definition,
                collision_instanced_geometry_definitions_get_mopp_code(&definition->m_mopp_codes, 0));
        cluster_shape->m_structure_bsp_index = structure_bsp_index;
    }
#endif
}

// The cluster shape answers child queries through its collection interface only; these trap if reached.
// @address 0x1b65a8 ?get_child_shape_count@c_cluster_collision_instanced_geometry_shape@@UBAHXZ
int c_cluster_collision_instanced_geometry_shape::get_child_shape_count() const
{
    __debugbreak();
    // match: 0x1b65a8: unreachable hint keeps the body a single trap word
    __assume(0);
}

// The instanced shape likewise traps the queries its collection interface never sends;
// all three share bytes with the traps at 0x1b65a8.
// @address 0x1b65a8 ?get_aabb_impl@c_instanced_geometry_shape@@MBAXABVphys_transform@@MAAVphys_aabb@@@Z
void c_instanced_geometry_shape::get_aabb_impl(const phys_transform &, float, phys_aabb &) const
{
    __debugbreak();
    // match: 0x1b65a8: __assume(0) omits the trailing blr after __debugbreak
    __assume(0);
}

// @address 0x1b65a8 ?trace_ray_impl@c_instanced_geometry_shape@@MBA?AVphys_bool@@ABUphys_ray_input@@AAUphys_ray_output@@@Z
phys_bool c_instanced_geometry_shape::trace_ray_impl(const phys_ray_input &, phys_ray_output &) const
{
    __debugbreak();
    // match: 0x1b65a8: __assume(0) omits the hkBool return epilog after __debugbreak
    __assume(0);
}

// @address 0x1b65a8 ?get_max_projection@c_instanced_geometry_shape@@UBAMABVphys_vec4@@@Z
float c_instanced_geometry_shape::get_max_projection(const phys_vec4 &) const
{
    __debugbreak();
    // match: 0x1b65a8: __assume(0) omits the float return epilog after __debugbreak
    __assume(0);
}

// @address 0x1b65a8 ?get_first_key@c_cluster_collision_instanced_geometry_shape@@UBAIXZ
unsigned int c_cluster_collision_instanced_geometry_shape::get_first_key() const
{
    __debugbreak();
    // match: 0x1b65a8: unreachable hint keeps the body a single trap word
    __assume(0);
}

// @address 0x1b65a8 ?get_next_key@c_cluster_collision_instanced_geometry_shape@@UBAII@Z
unsigned int c_cluster_collision_instanced_geometry_shape::get_next_key(unsigned int) const
{
    __debugbreak();
    // match: 0x1b65a8: unreachable hint keeps the body a single trap word
    __assume(0);
}

// @address 0x1b65a8 ?get_max_projection@c_cluster_collision_instanced_geometry_shape@@MBAMABVphys_vec4@@@Z
float c_cluster_collision_instanced_geometry_shape::get_max_projection(const phys_vec4 &) const
{
    __debugbreak();
    // match: 0x1b65a8: unreachable hint keeps the body a single trap word
    __assume(0);
}

// The cluster shape likewise traps the ray/aabb queries its collection interface never sends;
// all three share bytes with the traps at 0x1b65a8.
// @address 0x1b65a8 ?trace_ray_impl@c_cluster_collision_instanced_geometry_shape@@MBA?AVphys_bool@@ABUphys_ray_input@@AAUphys_ray_output@@@Z
phys_bool c_cluster_collision_instanced_geometry_shape::trace_ray_impl(const phys_ray_input &, phys_ray_output &) const
{
    __debugbreak();
    // match: 0x1b65a8: __assume(0) omits the hkBool return epilog after __debugbreak
    __assume(0);
}

// @address 0x1b65a8 ?trace_ray_with_collector@c_cluster_collision_instanced_geometry_shape@@MBAXABUphys_ray_input@@ABVphys_collision_proxy@@AAVphys_ray_hit_collector@@@Z
void c_cluster_collision_instanced_geometry_shape::trace_ray_with_collector(const phys_ray_input &, const phys_collision_proxy &,
    phys_ray_hit_collector &) const
{
    __debugbreak();
    // match: 0x1b65a8: __assume(0) omits the trailing blr after __debugbreak
    __assume(0);
}

// @address 0x1b65a8 ?get_aabb_impl@c_cluster_collision_instanced_geometry_shape@@MBAXABVphys_transform@@MAAVphys_aabb@@@Z
void c_cluster_collision_instanced_geometry_shape::get_aabb_impl(const phys_transform &, float, phys_aabb &) const
{
    __debugbreak();
    // match: 0x1b65a8: __assume(0) omits the trailing blr after __debugbreak
    __assume(0);
}
