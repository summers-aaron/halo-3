// hkdynamics/hkAabbPhantom.obj: from the records 0x64b548 0x64b560 0x64b568 0x64b588 0x64b5a8 0x64b5f0 0x64b6a8 0x64b7e0 0x64b948 0x64bad8 0x64bb58 0x64bc10 0x64bc28 0x64bc58 0x64bcf8 0x64bd70 0x64bf10 0x64bf90 0x64bff8
// @compiler xdk6130
// @flags /TP /O2 /Oc /GF /EHs-c- /GR-
// @address 0x64b548 ?destroy_loaded_box_volume@@YAXPAX@Z
// @address 0x64b560 ?get_type_id@phys_box_volume@@UBA?AW4phys_volume_type@@XZ
// @address 0x64b568 ?compute_bounds@phys_box_volume@@UAAXAAVphys_aabb@@@Z
// @address 0x64b588 ?set_aabb@phys_box_volume@@QAAXABVphys_aabb@@@Z
// @address 0x64b5a8 ?has_overlapping_collider@phys_box_volume@@UAA?AVphys_bool@@PAVphys_collider@@@Z
// @address 0x64b5f0 ?erase_overlapping_collider@phys_box_volume@@UAAXPAVphys_collider@@@Z
// @address 0x64b6a8 ?trace_ray@phys_box_volume@@QBAXABUphys_world_ray_input@@AAVphys_ray_hit_collector@@@Z
// @address 0x64b7e0 ?trace_ray@phys_box_volume@@QBAXABUphys_world_ray_input@@AAUphys_world_ray_output@@@Z
// @address 0x64b948 ?sweep_cast@phys_box_volume@@QBAXQBVphys_collider@@ABUphys_linear_cast_input@@AAVphys_hit_collector@@2@Z
// @address 0x64bad8 ?insert_overlapping_collider@phys_box_volume@@UAAXPAVphys_collider@@@Z
// @address 0x64bb58 ?gather_memory_stats@phys_box_volume@@UBAXPAVphys_statistics_collector@@@Z
// @address 0x64bc10 ?fixup_loaded_box_volume@@YAXPAX@Z
// @address 0x64bc28 ?get_box_volume_vtable@@YAPBXXZ
// @address 0x64bc58 ??0phys_box_volume@@QAA@ABVphys_aabb@@I@Z
// @address 0x64bcf8 ??1phys_box_volume@@UAA@XZ
// @address 0x64bd70 ?clone@phys_box_volume@@UBAPAVphys_volume@@XZ
// @address 0x64bf10 ?free_internal_arrays@phys_box_volume@@UAAXXZ
// @address 0x64bf90 ??0phys_box_volume@@QAA@Vphys_fixup_tag@@@Z
// @address 0x64bff8 ??_Gphys_box_volume@@UAAPAXI@Z
#include "phys/phys_dynamics.h"
#include "phys/phys_type_desc.h"
#include "phys/phys_aabb.h"
#include "phys/phys_dispatcher.h"
#include "phys/phys_statistics_collector.h"
#include "phys/phys_collectors.h"
#include "phys/phys_volume_overlap_listener.h"
phys_box_volume::phys_box_volume( const phys_aabb& aabb, unsigned int collision_filter_info )
: phys_volume( 0 )
{
    m_volume_aabb = aabb;

    m_collider.set_collision_filter_info( collision_filter_info );
}

phys_box_volume::~phys_box_volume()
{
}

phys_volume_type phys_box_volume::get_type_id() const
{
    return PHYS_VOLUME_AABB;
}

phys_volume* phys_box_volume::clone() const
{
    const phys_collider* c = get_collider();
    phys_box_volume* ap = new phys_box_volume( m_volume_aabb, c->get_collision_filter_info() );

    ap->m_overlap_listeners = m_overlap_listeners;
    ap->m_volume_listeners = m_volume_listeners;

    ap->copy_object_props( this );
    return ap;
}

void phys_box_volume::compute_bounds( phys_aabb& aabb )
{
    aabb = m_volume_aabb;
}

void phys_box_volume::set_aabb(const phys_aabb& new_aabb)
{
    m_volume_aabb = new_aabb;

    update_broadphase( m_volume_aabb );
}

void phys_box_volume::trace_ray( const phys_world_ray_input& input, phys_ray_hit_collector& collector ) const
{



    phys_ray_input ray_cast_input;
    ray_cast_input.m_filter_info = input.m_filter_info;

    if (input.m_compound_filter_enabled )
    {
        ray_cast_input.m_ray_compound_filter = m_world->get_collision_filter();
    }
    else
    {
        ray_cast_input.m_ray_compound_filter = 0;
    }

    const phys_collider* const* hit_collider = m_overlapping_colliders.begin();

    for (int i = m_overlapping_colliders.getSize()-1; i >=0 ; i--)
    {
        
        const phys_shape* shape = (*hit_collider)->get_shape();
        if ( shape != 0 )
        {
            const phys_transform& trans = (*hit_collider)->get_transform();
            ray_cast_input.m_ray_from.set_transformed_inverse_pos( trans, input.m_ray_from );
            ray_cast_input.m_ray_to.  set_transformed_inverse_pos( trans, input.m_ray_to );

            const phys_collision_proxy& body = **hit_collider;
                
            shape->trace_ray_with_collector( ray_cast_input, body, collector );
        }
        hit_collider++;
    }
}

void phys_box_volume::trace_ray( const phys_world_ray_input& input, phys_world_ray_output& output ) const
{


    phys_ray_output ray_results;

    phys_ray_input ray_cast_input;
    ray_cast_input.m_filter_info = input.m_filter_info;

    if (input.m_compound_filter_enabled )
    {
        ray_cast_input.m_ray_compound_filter = m_world->get_collision_filter();
    }
    else
    {
        ray_cast_input.m_ray_compound_filter = 0;
    }
    
    const phys_collider* const * hit_collider = m_overlapping_colliders.begin();

    for (int i = m_overlapping_colliders.getSize()-1; i >=0 ; i--)
    {    
        const phys_shape* shape = (*hit_collider)->get_shape();
        if ( shape != 0 )
        {
            const phys_transform& trans = (*hit_collider)->get_transform();
            ray_cast_input.m_ray_from.set_transformed_inverse_pos( trans, input.m_ray_from );
            ray_cast_input.m_ray_to.  set_transformed_inverse_pos( trans, input.m_ray_to );
                
            phys_bool hit = shape->trace_ray( ray_cast_input, output);
            if (hit)
            {
                output.m_root_collider = *hit_collider;
            }
        }
        hit_collider++;
    }
    
    if ( output.m_root_collider != 0 )
    {
        const phys_transform& trans = output.m_root_collider->get_transform();
        output.m_normal.vec_set_rotated_dir( trans.get_rotation(), output.m_normal);
    }
}

void phys_box_volume::sweep_cast(const phys_collider *const to_be_cast, const phys_linear_cast_input &input, phys_hit_collector &cast_hit_collector,
    phys_hit_collector &start_collector) const
{
    
    phys_vec4 path;
    path.set_sub4(input.m_ray_to, to_be_cast->get_transform().get_translation());

    phys_linear_cast_collision_input linear_cast_input;
    {
        linear_cast_input.set( *m_world->get_collision_input() );
        linear_cast_input.set_path_and_tolerance( path, input.m_start_tolerance );
        linear_cast_input.m_max_extra_penetration = input.m_max_extra_penetration;
    }

    const phys_dispatcher* dispatcher    = m_world->get_dispatcher();
    const phys_collision_filter*     filter        = m_world->get_collision_filter();
    
    const int collider_cnt = m_overlapping_colliders.getSize();
    for ( int i = 0; i<collider_cnt; i++ )
    {
        const phys_collider* target = m_overlapping_colliders[i];

        const bool collision_enabled = filter->allows_collision(*to_be_cast, *target);
        const bool distinct_collider = (target != to_be_cast);

        if (target->get_shape() && collision_enabled && distinct_collider)
        {
            const phys_shape_type shape_type_a = to_be_cast->get_shape()->get_type_id();
            const phys_shape_type shape_type_b = target->get_shape()->get_type_id();
            phys_dispatcher::agent_funcs::sweep_cast_fn dispatch_linear_cast_fn = dispatcher->get_linear_cast_fn(shape_type_a, shape_type_b);
            dispatch_linear_cast_fn(*to_be_cast, *target, linear_cast_input, cast_hit_collector, &start_collector);
        }
    }
}

phys_bool phys_box_volume::has_overlapping_collider( phys_collider* collidable )
{
    for( int i = 0; i<m_overlapping_colliders.getSize(); i++ )
    {
        if( m_overlapping_colliders[i] == collidable )
        {
            return true;
        }
    }
    return false;
}

void phys_box_volume::insert_overlapping_collider( phys_collider* collidable )
{
    

    phys_collider_accept accept = notify_collider_added( collidable );

    if ( accept == PHYS_COLLIDER_ACCEPT  )
    {
        m_overlapping_colliders.array_push_back( collidable );
    }
}

void phys_box_volume::erase_overlapping_collider( phys_collider* collidable )
{

    int index = m_overlapping_colliders.array_index_of( collidable );

    notify_collider_removed( collidable, index >= 0 );

    if( index >= 0 )
    {
        m_overlapping_colliders.remove_at( index );
    }

}

void phys_box_volume::gather_memory_stats( phys_statistics_collector* collector ) const 
{

    collector->begin_stats_object("AabbPhantom", collector->stat_category_instance, this);
    phys_volume::gather_memory_stats(collector);
    collector->add_array_stats("OvrlpCollPtr", collector->stat_category_runtime, m_overlapping_colliders);
    collector->end_object();
}

void phys_box_volume::free_internal_arrays()
{

    if (m_overlapping_colliders.getSize() == 0)
    {
        m_overlapping_colliders.clear_and_deallocate();
    }

    phys_volume::free_internal_arrays();
}


void fixup_loaded_box_volume(void *object)
{
    phys_fixup_tag flag;
    flag.m_fixup_finishing = 1;
    new (object) phys_box_volume(flag);
}
void destroy_loaded_box_volume(void *object)
{
    static_cast<phys_box_volume *>(object)->~phys_box_volume();
}
const void *get_box_volume_vtable()
{
    union { __declspec(align(16)) void *ptr; char buffer[sizeof(phys_box_volume)]; } object;
    phys_fixup_tag flag;
    new (object.buffer) phys_box_volume(flag);
    return object.ptr;
}

const phys_type_desc phys_box_volume_type_desc = phys_type_desc("hkAabbPhantom", fixup_loaded_box_volume,
    destroy_loaded_box_volume, get_box_volume_vtable());
