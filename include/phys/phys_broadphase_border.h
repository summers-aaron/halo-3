#ifndef INCLUDE_PHYS_BROADPHASE_BORDER_H
#define INCLUDE_PHYS_BROADPHASE_BORDER_H

#include "phys/phys_dynamics.h"
#include "phys/phys_volume_overlap_listener.h"
#include "phys/phys_world_deletion_listener.h"

// Virtual interface confirmed by March 0x834e7408..0x834e81f8.
class phys_broadphase_border : public phys_ref_object, protected phys_world_deletion_listener, protected phys_volume_overlap_listener
{
	public:

		PHYS_DECLARE_ALLOCATOR(PHYS_MEM_UTILITIES);

		phys_broadphase_border( phys_world* world, phys_world_desc::broadphase_border_mode type  = phys_world_desc::BROADPHASE_BORDER_ASSERT );
			
		virtual ~phys_broadphase_border();

		virtual void on_max_position_exceeded( phys_entity* body );

		virtual void deactivate();

		virtual void collider_added_callback( const phys_collider_added_event& event	);

		virtual void collider_removed_callback( const phys_collider_removed_event& event );

	public:

		virtual void world_deleted_callback( phys_world* world );

		phys_world* m_world;

		class phys_volume* m_border_volumes[6];

		phys_world_desc::broadphase_border_mode m_type;

};


#endif
