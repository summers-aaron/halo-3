#ifndef INCLUDE_PHYS_CONSTRAINT_H
#define INCLUDE_PHYS_CONSTRAINT_H

#include <stddef.h>
#include "phys/phys_constraint_data.h"

class phys_constraint_data;
class phys_entity;
class phys_rigid_body;
class phys_constraint_owner;
class phys_island;
extern const phys_class_desc phys_constraint_rtti;

typedef void phys_constraint_runtime;

class phys_constraint : public phys_ref_object
{

    public:

        PHYS_DECLARE_ALLOCATOR( PHYS_MEM_CONSTRAINT );

        enum constraint_priority
        {

            CONSTRAINT_PRIORITY_INVALID,

            PRIORITY_PSI,

            CONSTRAINT_PRIORITY_TOI,

            CONSTRAINT_PRIORITY_TOI_HIGHER,

            CONSTRAINT_PRIORITY_TOI_FORCED,
        };

        enum constraint_instance_kind
        {

            CONSTRAINT_KIND_NORMAL,

            CONSTRAINT_KIND_CHAIN
        };

        enum add_ref_policy
        {
            CLONE_SKIP_REFS,
            CLONE_ADD_REFS
        };

        enum cloning_mode
        {
            CONSTRAINT_CLONE_SHALLOW,
            CONSTRAINT_CLONE_DATA_AND_MOTORS
        };

        phys_constraint(phys_entity* listener_entity_a, phys_entity* listener_entity_b, phys_constraint_data* data,
            constraint_priority priority = PRIORITY_PSI);

        ~phys_constraint();

        phys_constraint* clone(phys_entity* new_entity_a, phys_entity* new_entity_b) const;

    protected:

        phys_constraint(constraint_priority priority);

    public:

        inline phys_entity* get_entity_a() const;

        inline phys_entity* get_constraint_entity( int index ) const;

        inline phys_entity* get_entity_b() const;

        inline phys_rigid_body* get_rigid_body_a() const;

        inline phys_rigid_body* get_rigid_body_b() const;

        inline phys_entity* get_other_entity( const phys_entity* entity );

        phys_island* get_island();

        inline phys_constraint_data* get_chain_data() const;

        inline phys_constraint_runtime* get_runtime() const;

        inline void set_want_runtime( phys_bool b );

        inline phys_bool get_want_runtime() const ;

        inline unsigned long get_user_data() const;

        inline void set_user_data( unsigned long data );

        inline constraint_priority get_priority() const;

        void set_constraint_priority( constraint_priority priority );

        inline const char* get_action_name() const;

        inline void set_action_name( const char* name );

    public:

        inline phys_constraint_owner* get_collider_owner() const;

        inline phys_entity* get_master_entity() const;

        inline phys_entity* get_slave_entity() const;

        virtual void on_entity_added(phys_entity* entity);

        virtual void on_entity_removed(phys_entity* entity);

        virtual void on_entity_deleted( phys_entity* entity );

        inline void set_collider_owner( phys_constraint_owner* island );

        inline struct phys_constraint_internal* get_internal();

        void bind_nulls_to_fixed_body();

        virtual constraint_instance_kind get_type_id() const { return CONSTRAINT_KIND_NORMAL; }

    protected:
        friend class phys_simple_contact_data;
        friend class phys_island;

        class phys_constraint_owner* m_owner;

        class phys_constraint_data*  m_data;

        struct phys_modifier_atom* m_constraint_modifiers;

        class phys_entity* m_island_entities[2];

    public:
        phys_enum<constraint_priority,unsigned char> m_priority;

    protected:

        phys_bool                m_want_runtime;

        const char* m_name;

    public:

        unsigned long m_user_data;

        struct phys_constraint_internal* m_constraint_internal;

    public:

        phys_constraint(phys_fixup_tag f) { }

    private:

        friend class phys_world_constraint_util;
        friend class phys_simple_contact_manager;
        friend class phys_save_contact_points_util;

        phys_constraint(){}
};

__declspec(align(16)) struct phys_constraint_internal
{
    PHYS_DECLARE_NONVIRTUAL_ALLOCATOR( PHYS_MEM_DYNAMICS, phys_constraint_internal );

    inline phys_entity* get_other_entity( const phys_entity* entity ) const;
    inline phys_entity* get_master_entity() const;
    inline phys_entity* get_slave_entity() const;

    inline struct phys_atom* get_atoms() const { return m_data_atoms; }
    inline unsigned short get_atoms_size() const { return m_atoms_size; }

    inline void clear_constraint_info( );

    public:

        class phys_constraint* m_contact_constraint;

        class phys_entity*      m_island_entities[2];

    protected:
        friend class phys_world_constraint_util;

        struct phys_atom* m_data_atoms;

        unsigned short m_atoms_size;

    public:
        unsigned char    m_callback_request;

        phys_enum<phys_constraint::constraint_priority,unsigned char> m_priority;

        unsigned short m_schemas_size;
        unsigned short m_solver_result_count;
        unsigned short m_solver_temp_count;

        inline void get_constraint_info( phys_constraint_info& info ) const;
        inline void add_constraint_info( const phys_constraint_info& delta );
        inline void sub_constraint_info( const phys_constraint_info& delta );

        unsigned char    m_master_body;

        phys_bool m_normal_type;

        phys_constraint_runtime*    m_solver_runtime;
        unsigned short            m_runtime_size;

        unsigned short        m_slave_idx;

        friend class phys_save_contact_points_util;
};

inline phys_entity* phys_constraint::get_entity_a() const
{
    return m_island_entities[0];
}

inline phys_entity* phys_constraint::get_constraint_entity( int index ) const
{
    return m_island_entities[index];
}

inline phys_entity* phys_constraint::get_entity_b() const
{
    return m_island_entities[1];
}

inline phys_rigid_body* phys_constraint::get_rigid_body_a() const
{
    return reinterpret_cast<phys_rigid_body*>(m_island_entities[0]);
}

inline phys_rigid_body* phys_constraint::get_rigid_body_b() const
{
    return reinterpret_cast<phys_rigid_body*>(m_island_entities[1]);
}

inline phys_entity* phys_constraint_internal::get_master_entity() const
{
    return m_island_entities[m_master_body];
}

inline phys_entity* phys_constraint_internal::get_slave_entity() const
{
    return m_island_entities[1-m_master_body];
}

inline phys_entity* phys_constraint::get_master_entity() const
{
    return m_constraint_internal->get_master_entity();
}

inline phys_entity* phys_constraint::get_slave_entity() const
{
    return m_constraint_internal->get_slave_entity();
}

inline void phys_constraint::set_want_runtime( phys_bool b )
{
    m_want_runtime = b;
}

inline phys_bool phys_constraint::get_want_runtime() const
{
    return m_want_runtime;
}

inline phys_entity* phys_constraint_internal::get_other_entity( const phys_entity* entity ) const
{
    uintptr_t a = reinterpret_cast<uintptr_t>(m_island_entities[0]);
    uintptr_t b = reinterpret_cast<uintptr_t>(m_island_entities[1]);
    uintptr_t c = reinterpret_cast<uintptr_t>(entity);
    return reinterpret_cast<phys_entity*>( a^b^c );
}

inline phys_constraint_owner* phys_constraint::get_collider_owner() const
{
    return m_owner;
}

void phys_constraint::set_collider_owner( phys_constraint_owner* owner )
{
    m_owner = owner;
}

inline phys_constraint_data* phys_constraint::get_chain_data() const
{
    return m_data;
}

inline phys_constraint_internal* phys_constraint::get_internal()
{
    return m_constraint_internal;
}

inline phys_constraint_runtime* phys_constraint::get_runtime() const
{
    return m_constraint_internal->m_solver_runtime;
}

inline unsigned long phys_constraint::get_user_data() const
{
    return m_user_data;
}

inline void phys_constraint::set_user_data( unsigned long data )
{

    m_user_data = data;
}

inline const char* phys_constraint::get_action_name() const
{
    return m_name;
}

inline void phys_constraint::set_action_name( const char* name )
{
    m_name = name;
}

inline phys_constraint::constraint_priority phys_constraint::get_priority() const
{
    return m_priority;
}

inline phys_entity* phys_constraint::get_other_entity( const phys_entity* entity )
{
    return phys_select_other( const_cast<phys_entity*>(entity), m_island_entities[0], m_island_entities[1]);
}

void phys_constraint_internal::get_constraint_info( phys_constraint_info& info ) const
{
    info.m_max_schema_size     = m_schemas_size;
    info.m_schemas_size       = m_schemas_size;
    info.m_solver_result_count    = m_solver_result_count;
    info.m_solver_temp_count  = m_solver_temp_count;
}

void phys_constraint_internal::clear_constraint_info( )
{
    this->m_solver_result_count = 0;
    this->m_solver_temp_count = 0;
    this->m_schemas_size    = 0;
}

void phys_constraint_internal::add_constraint_info( const phys_constraint_info& delta)
{
    const unsigned int solver_result_count   = this->m_solver_result_count   + delta.m_solver_result_count;
    const unsigned int solver_elem_temp_count = this->m_solver_temp_count + delta.m_solver_temp_count;
    const unsigned int schemas_size      = this->m_schemas_size      + delta.m_schemas_size;

    this->m_solver_result_count   = static_cast<unsigned short>(solver_result_count);
    this->m_solver_temp_count = static_cast<unsigned short>(solver_elem_temp_count);
    this->m_schemas_size      = static_cast<unsigned short>(schemas_size);
}

void phys_constraint_internal::sub_constraint_info( const phys_constraint_info& delta)
{
    const unsigned int solver_result_count   = this->m_solver_result_count   - delta.m_solver_result_count;
    const unsigned int solver_elem_temp_count = this->m_solver_temp_count - delta.m_solver_temp_count;
    const unsigned int schemas_size      = this->m_schemas_size      - delta.m_schemas_size;

    this->m_solver_result_count = static_cast<unsigned short>(solver_result_count);
    this->m_solver_temp_count = static_cast<unsigned short>(solver_elem_temp_count);
    this->m_schemas_size    = static_cast<unsigned short>(schemas_size);
}


#endif // INCLUDE_HAVOK_HK_CONSTRAINT_INSTANCE_H
