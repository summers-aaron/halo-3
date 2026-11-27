#ifndef INCLUDE_PHYS_PHYSICS_SYSTEM_H
#define INCLUDE_PHYS_PHYSICS_SYSTEM_H

#include "phys/phys_base.h"

class phys_rigid_body;
class phys_constraint;
class phys_action;
class phys_volume;

class phys_physics_system : public phys_ref_object
{
	public:

		PHYS_DECLARE_ALLOCATOR(PHYS_MEM_WORLD);

		phys_physics_system();

		virtual ~phys_physics_system();

		phys_physics_system* clone() const;

		void copy(const phys_physics_system& to_copy);

		void add_system_rigid_body( phys_rigid_body* rb );

		void add_volume(  phys_volume* p );

		void add_world_constraint( phys_constraint* c );

		void add_world_action( phys_action* a );

		void remove_rigid_body( int i );

		void remove_volume( int i );

		void remove_constraint( int i );

		void remove_action( int i );

		inline const phys_array<phys_rigid_body*>& get_rigid_bodies() const;

		inline const phys_array<phys_volume*>& get_volumes() const;

		inline const phys_array<phys_constraint*>& get_system_constraints() const;

		inline const phys_array<phys_action*>& get_actions() const;

		inline const char* get_action_name() const;

		inline void set_action_name(const char* name);

		inline unsigned long get_user_data() const;

		inline void set_user_data(unsigned long d);

		inline phys_bool is_body_active() const;

		inline void system_set_active(phys_bool active);

		virtual phys_bool has_contacts() { return false; }

	protected:

		phys_array<class phys_rigid_body*> m_system_rigid_bodies;

		phys_array<class phys_constraint*> m_component_constraints;

		phys_array<class phys_action*> m_actions;

		phys_array<class phys_volume*> m_border_volumes;

		const char* m_name;

		unsigned long	m_user_data;

		phys_bool m_active;

	public:

		phys_physics_system( phys_fixup_tag f ) : m_system_rigid_bodies(f), m_component_constraints(f), m_actions(f), m_border_volumes(f) { }
};


inline const phys_array<phys_rigid_body*>& phys_physics_system::get_rigid_bodies() const
{
	return m_system_rigid_bodies;
}

inline const phys_array<phys_constraint*>& phys_physics_system::get_system_constraints() const
{
	return m_component_constraints;
}

inline const phys_array<phys_action*>& phys_physics_system::get_actions() const
{
	return m_actions;
}

inline const phys_array<phys_volume*>& phys_physics_system::get_volumes() const
{
	return m_border_volumes;
}

inline const char* phys_physics_system::get_action_name() const
{
	return m_name;
}

inline void phys_physics_system::set_action_name(const char* name)
{
	m_name = name ;
}

inline unsigned long phys_physics_system::get_user_data() const
{
	return m_user_data;
}

inline void phys_physics_system::set_user_data(unsigned long d)
{
	m_user_data = d;
}

inline phys_bool phys_physics_system::is_body_active() const
{
	return m_active;
}

inline void phys_physics_system::system_set_active(phys_bool active)
{
	m_active = active;
}



typedef char phys_physics_system_size[(sizeof(void *) != 4 || sizeof(phys_physics_system) == 0x44) ? 1 : -1];

#endif
