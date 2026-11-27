#ifndef INCLUDE_PHYS_CONSTRAINT_DATA_H
#define INCLUDE_PHYS_CONSTRAINT_DATA_H
#include "phys/phys_base.h"
struct phys_atom;
class phys_solver_results
{
    public:
        PHYS_DECLARE_NONVIRTUAL_ALLOCATOR( PHYS_MEM_CONSTRAINT_SOLVER, phys_solver_results );

        inline phys_real get_applied_impulse() const;
        inline phys_real get_internal_solver_data() const { return m_internal_solver_data; }

        inline phys_solver_results();

        inline void init();

    public:

        phys_real m_applied_impulse;

        phys_real m_internal_solver_data;
};

inline void phys_solver_results::init()
{
    m_applied_impulse = 0.0f;
    m_internal_solver_data = 0.0f;
}

inline phys_solver_results::phys_solver_results()
{
    init();
}

inline phys_real phys_solver_results::get_applied_impulse() const
{
    return m_applied_impulse;
}

#define PHYS_SIZE_OF_GOTO_SCHEMA 16
#define PHYS_SIZE_OF_SHIFT_SOLVER_RESULTS_SCHEMA 16
#define PHYS_SIZE_OF_1D_BILATERAL_SCHEMA 48
#define PHYS_SIZE_OF_1D_BILATERAL_USER_TAU_SCHEMA 64
#define PHYS_SIZE_OF_1D_LIN_LIMIT_SCHEMA 64
#define PHYS_SIZE_OF_1D_FRICTION_SCHEMA 64
#define PHYS_SIZE_OF_1D_LIN_MOTOR_SCHEMA 80
#define PHYS_SIZE_OF_1D_PULLEY_SCHEMA 64
#define PHYS_SIZE_OF_1D_ANG_SCHEMA 32
#define PHYS_SIZE_OF_1D_ANG_LIMITS_SCHEMA 48
#define PHYS_SIZE_OF_1D_ANG_FRICTION_SCHEMA 48
#define PHYS_SIZE_OF_1D_ANG_MOTOR_SCHEMA 64
#define PHYS_SIZE_OF_SINGLE_CONTACT_SCHEMA 48
#define PHYS_SIZE_OF_PAIR_CONTACT_SCHEMA 112
#define PHYS_SIZE_OF_2D_FRICTION_SCHEMA 112
#define PHYS_SIZE_OF_3D_FRICTION_SCHEMA 144
#define PHYS_SIZE_OF_SET_MASS_SCHEMA 32
#define PHYS_SIZE_OF_ADD_VEL_SCHEMA 32
#define PHYS_SIZE_OF_STIFF_SPRING_CHAIN_SCHEMA 16
#define PHYS_SIZE_OF_BALL_SOCKET_CHAIN_SCHEMA 16
#define PHYS_SIZE_OF_POWERED_CHAIN_SCHEMA 32
#include "phys/phys_constraint_info.h"

class phys_constraint_query_in;
class phys_constraint_query_out;
class phys_constraint_owner;
class phys_constraint;

extern const phys_class_desc phys_constraint_data_rtti;
extern const phys_class_desc phys_constraint_info_rtti;

typedef void phys_constraint_runtime;

class phys_constraint_data;

class phys_constraint_data : public phys_ref_object
{
    public:

        PHYS_DECLARE_ALLOCATOR( PHYS_MEM_CONSTRAINT);

        phys_constraint_data(phys_fixup_tag f) {}

        enum constraint_kind
        {
            CONSTRAINT_KIND_BALL_SOCKET = 0,
            CONSTRAINT_KIND_HINGE = 1,
            CONSTRAINT_KIND_LIMITED_HINGE = 2,
            CONSTRAINT_KIND_POINT_TO_PATH = 3,
            CONSTRAINT_KIND_PRISMATIC = 6,
            CONSTRAINT_KIND_RAGDOLL = 7,
            CONSTRAINT_KIND_STIFF_SPRING = 8,
            CONSTRAINT_KIND_WHEEL = 9,
            CONSTRAINT_KIND_GENERIC = 10,
            CONSTRAINT_KIND_CONTACT = 11,
            CONSTRAINT_KIND_BREAKABLE = 12,
            CONSTRAINT_KIND_MALLEABLE = 13,
            CONSTRAINT_KIND_POINT_TO_PLANE = 14,

            CONSTRAINT_KIND_PULLEY = 15,

            CONSTRAINT_KIND_ROTATIONAL = 16,

            CONSTRAINT_KIND_HINGE_LIMITS = 18,
            CONSTRAINT_KIND_RAGDOLL_LIMITS = 19,

            CONSTRAINT_KIND_CHAIN_FIRST = 100,
            CONSTRAINT_KIND_STIFF_SPRING_CHAIN = 100,
            CONSTRAINT_KIND_BALL_SOCKET_CHAIN = 101,
            CONSTRAINT_KIND_POWERED_CHAIN = 102
        };

        inline phys_constraint_data();

        inline virtual ~phys_constraint_data();

        inline unsigned long get_user_data() const;

        inline void set_user_data( unsigned long data );

        virtual phys_bool is_valid_aabb() const = 0;

        virtual int get_type_id() const = 0;

        struct runtime_info
        {
            PHYS_DECLARE_NONVIRTUAL_ALLOCATOR( PHYS_MEM_DYNAMICS, phys_constraint_data::runtime_info );

            int m_external_runtime_size;
            int m_solver_result_count;
        };

        virtual void get_runtime_info( phys_bool want_runtime, runtime_info& out_constraint_info ) const = 0;

        virtual phys_solver_results* get_solver_results( phys_constraint_runtime* runtime );

        virtual void add_constraint_instance( phys_constraint* constraint, phys_constraint_runtime* runtime, int runtime_size ) const;

        inline void remove_constraint_instance( phys_constraint* constraint, phys_constraint_runtime* runtime, int runtime_size ) const;

    protected:

        friend class phys_solver_setup;

    public:

        virtual void build_jacobian( const phys_constraint_query_in &in, phys_constraint_query_out &out ) {  }

        virtual phys_bool needs_jacobian_callback();

        virtual void build_jacobian_callback( const phys_constraint_query_in &in ) {  }

        struct constraint_info : public phys_constraint_info
        {
            PHYS_DECLARE_NONVIRTUAL_ALLOCATOR( PHYS_MEM_DYNAMICS, phys_constraint_data::constraint_info );

            struct phys_atom* m_data_atoms;
            unsigned int          m_all_atoms_size;
        };

        virtual void get_constraint_info( constraint_info& out_constraint_info ) const = 0;

        static void PHYS_CALL gather_constraint_info( const phys_atom* constraint_atoms, int all_atoms_size, constraint_info& out_constraint_info );

    public:

        unsigned long m_user_data;
};

inline phys_constraint_data::phys_constraint_data()
{

    m_user_data = 0;
}

phys_constraint_data::~phys_constraint_data()
{

}

inline unsigned long phys_constraint_data::get_user_data() const
{
    return m_user_data;
}

inline void phys_constraint_data::set_user_data( unsigned long data )
{
    m_user_data = data;
}

inline void phys_constraint_data::remove_constraint_instance( phys_constraint* constraint, phys_constraint_runtime* runtime, int runtime_size ) const
{
}

class phys_constraint_data;
#endif // INCLUDE_HAVOK_HK_CONSTRAINT_DATA_H
