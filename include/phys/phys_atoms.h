#ifndef INCLUDE_PHYS_ATOMS_H
#define INCLUDE_PHYS_ATOMS_H
#include "phys/phys_constraint_data.h"
#include "phys/phys_contact_point.h"

class phys_constraint_query_in;
class phys_constraint_query_out;
class phys_contact_point;
class phys_contact_point_props;
struct phys_simple_contact_atom;
class phys_simple_contact_data_info
{
	public:
		PHYS_DECLARE_NONVIRTUAL_ALLOCATOR( PHYS_MEM_CONSTRAINT_SOLVER, phys_simple_contact_data_info );

		enum {	PHYS_FLAG_OK = 0x00, PHYS_FLAG_POINT_REMOVED = 0x01, PHYS_FLAG_AREA_CHANGED = 0x04 };

		void init()
		{
			m_flags = PHYS_FLAG_OK;
			m_index = 3;
			for(int i = 0; i < 7; ++i)
			{
				m_data[i] = 0;
			}
		}

		phys_simple_contact_data_info()
		{
			init();
		}

		__declspec(align(16)) unsigned short m_flags;
		// Retain the packed words used by init(), with the solver's typed fields at the same offsets.
		union { unsigned short m_index; unsigned short m_binormal_axis; };
		union {
			unsigned int m_data[7];
			struct {
				float m_contact_radius;
				unsigned int m_rolling_friction_mul;
				float m_internal_data1;
				unsigned int m_payload4;
				float m_internal_data2;
				unsigned int m_payload6;
				float m_rhs_rolling;
			};
		};

		phys_simple_contact_data_info(phys_fixup_tag f) {}
};

extern phys_real PHYS_CONTACT_LIN_ERROR_RECOVERY_VEL;
extern phys_real PHYS_CONTACT_EXP_ERROR_RECOVERY_VEL;

extern "C"
{

	void PHYS_CALL phys_simple_contact_build_jacobian( phys_simple_contact_atom* atom, const phys_constraint_query_in& in,
		phys_bool write_header_schema, phys_constraint_query_out& out );

	void PHYS_CALL phys_simple_contact_add_viscosity( phys_simple_contact_data_info* info );

}

class phys_contact_point;
class phys_motor;
class phys_contact_props_stream;

struct phys_atom
{
		PHYS_DECLARE_NONVIRTUAL_ALLOCATOR( PHYS_MEM_CONSTRAINT, phys_atom );

	public:
		enum atom_kind
		{
			ATOM_INVALID = 0,

			ATOM_BRIDGE,

			ATOM_SET_LOCAL_TRANSFORMS,
			ATOM_SET_LOCAL_TRANSLATIONS,
			ATOM_SET_LOCAL_ROTATIONS,

			ATOM_BALL_SOCKET,
			ATOM_STIFF_SPRING,

			ATOM_LIN,
			ATOM_LIN_SOFT,
			ATOM_LIN_LIMIT,
			ATOM_LIN_FRICTION,
			ATOM_LIN_MOTOR,

			ATOM_2D_ANG,

			ATOM_ANG,
			ATOM_ANG_LIMIT,
			ATOM_TWIST_LIMIT,
			ATOM_CONE_LIMIT,
			ATOM_ANG_FRICTION,
			ATOM_ANG_MOTOR,

			ATOM_RAGDOLL_MOTOR,

			ATOM_PULLEY,
			ATOM_OVERWRITE_PIVOT,

			ATOM_CONTACT,

			ATOM_MOD_SOFT_CONTACT,
			ATOM_MOD_MASS_CHANGER,
			ATOM_MOD_VISCOUS_SURFACE,
			ATOM_MOD_MOVING_SURFACE,

			CONTACT_MANAGER_TYPE_COUNT
		};

		enum callback_request
		{
			ATOM_CALLBACK_NONE = 0,
			ATOM_CALLBACK_NEW_CONTACT = 1,
			ATOM_CALLBACK_SETUP_HOST_ONLY = 2,
			ATOM_CALLBACK_SETUP = 4
		};

	public:
		__forceinline enum atom_kind get_type_id() const { return m_type; }

		__forceinline int is_modifier_type() const { return m_type >= ATOM_MOD_SOFT_CONTACT; }

	protected:
		phys_atom(enum atom_kind type) : m_type(type) {}

	private:

		phys_atom();

	public:
		phys_enum<atom_kind,unsigned short> m_type;

		phys_atom(phys_fixup_tag f) {}
};

typedef void (PHYS_CALL *phys_atom_build_jacobian_fn) ( class phys_constraint_data* m_constraint_data, const phys_constraint_query_in &in,
	phys_constraint_query_out &out );

struct phys_bridge_atom: public phys_atom
{
	PHYS_DECLARE_NONVIRTUAL_ALLOCATOR( PHYS_MEM_CONSTRAINT, phys_bridge_atom );

	phys_bridge_atom(  ): phys_atom( ATOM_BRIDGE ){ }

	void init (class phys_constraint_data* m_constraint_data);

	__forceinline phys_atom* next() const { return const_cast<phys_atom*>( static_cast<const phys_atom*>(this+1) ); }

	__forceinline int solver_result_count() const    { return 100000; }

	phys_atom_build_jacobian_fn       m_build_jacobian_func;

	class phys_constraint_data* m_constraint_data;

	phys_bridge_atom(phys_fixup_tag f) : phys_atom(f) { init(m_constraint_data); }
};

struct phys_bridge_atoms
{
	PHYS_DECLARE_NONVIRTUAL_ALLOCATOR( PHYS_MEM_CONSTRAINT, phys_bridge_atoms );

	struct phys_bridge_atom m_bridge_atom;

	phys_bridge_atoms(){}

	const phys_atom* get_atoms() const { return &m_bridge_atom; }

	int get_size_of_all_atoms() const               { return phys_get_byte_offset_int(this, &m_bridge_atom+1); }

	phys_bridge_atoms(phys_fixup_tag f) : m_bridge_atom(f) {}
};

struct phys_simple_contact_atom : public phys_atom
{

		PHYS_DECLARE_NONVIRTUAL_ALLOCATOR( PHYS_MEM_CONSTRAINT, phys_simple_contact_atom );

	public:

		phys_simple_contact_atom() : phys_atom(ATOM_CONTACT) {}

		__forceinline phys_contact_point* get_contact_points() const {
			return const_cast<phys_contact_point*>( reinterpret_cast<const phys_contact_point*>( this+1 ) ); }
		__forceinline phys_contact_point_props *get_contact_point_props() const {
			return const_cast<phys_contact_point_props *>(
				reinterpret_cast<const phys_contact_point_props *>(
					phys_add_byte_offset_const(get_contact_points(), sizeof(phys_contact_point) * m_reserved_contact_count)));
		}
		__forceinline int get_contact_props_stride() const;
		__forceinline phys_contact_props_stream* get_contact_props_stream() const
		{
			return const_cast<phys_contact_props_stream*>(
				reinterpret_cast<const phys_contact_props_stream*>(
					phys_add_byte_offset_const( get_contact_points(), sizeof(phys_contact_point) * m_reserved_contact_count ) ) );
		}
		__forceinline phys_contact_props_stream* get_contact_props_stream(int i) const;

	public:
		__forceinline phys_atom* next() const {  return 0; }
		__forceinline int solver_result_count() const    { return m_contact_point_count+3; }

		__forceinline void add_to_constraint_info(phys_constraint_info& out_constraint_info) const
		{
			int size = m_contact_point_count;

			out_constraint_info.m_schemas_size    += PHYS_SIZE_OF_HEADER_SCHEMA
										+ (size >> 1) * PHYS_SIZE_OF_PAIR_CONTACT_SCHEMA
				                        + (size & 1) * PHYS_SIZE_OF_SINGLE_CONTACT_SCHEMA
										+ PHYS_SIZE_OF_2D_FRICTION_SCHEMA;
			out_constraint_info.m_max_schema_size = out_constraint_info.m_schemas_size + (PHYS_SIZE_OF_3D_FRICTION_SCHEMA - PHYS_SIZE_OF_2D_FRICTION_SCHEMA);
			out_constraint_info.m_solver_result_count   += size + 2;
			out_constraint_info.m_solver_temp_count += size + (2 + 1);
			if ( size >= 2 )
			{
				out_constraint_info.m_schemas_size   += PHYS_SIZE_OF_3D_FRICTION_SCHEMA - PHYS_SIZE_OF_2D_FRICTION_SCHEMA;
				out_constraint_info.m_solver_result_count   += 1;
				out_constraint_info.m_solver_temp_count += 1 + 0;
			}
		}

	public:
		unsigned short m_all_atoms_size;
		unsigned short m_contact_point_count;
		unsigned short m_reserved_contact_count;

		unsigned char m_user_data_count_a;

		unsigned char m_user_data_count_b;

		unsigned char m_contact_props_stride;

		unsigned short m_max_contact_points;

		__declspec(align(16)) class phys_simple_contact_data_info m_bvh_info;

	public:
		phys_simple_contact_atom(phys_fixup_tag f);
};

struct phys_ball_socket_atom : public phys_atom
{
	PHYS_DECLARE_NONVIRTUAL_ALLOCATOR( PHYS_MEM_CONSTRAINT, phys_ball_socket_atom );

public:
	phys_ball_socket_atom() : phys_atom(ATOM_BALL_SOCKET) {}

	__forceinline phys_atom* next() const { return const_cast<phys_atom*>( static_cast<const phys_atom*>(this+1) ); }

	__forceinline int solver_result_count() const    { return 3; }

	__forceinline void add_to_constraint_info(phys_constraint_info& out_constraint_info) const {
		out_constraint_info.add( 3 * PHYS_SIZE_OF_1D_BILATERAL_SCHEMA, 3, 3 ); }

	phys_ball_socket_atom(phys_fixup_tag f) : phys_atom(f) {}
};

struct phys_stiff_spring_atom : public phys_atom
{
	PHYS_DECLARE_NONVIRTUAL_ALLOCATOR( PHYS_MEM_CONSTRAINT, phys_stiff_spring_atom );

public:
	phys_stiff_spring_atom() : phys_atom(ATOM_STIFF_SPRING) {}
	__forceinline phys_atom* next() const { return const_cast<phys_atom*>( static_cast<const phys_atom*>(this+1) ); }
	__forceinline int solver_result_count() const    { return 1; }
	__forceinline void add_to_constraint_info(phys_constraint_info& out_constraint_info) const {
		out_constraint_info.add( PHYS_SIZE_OF_1D_BILATERAL_SCHEMA, 1, 1 ); }

	phys_real m_length;

	phys_stiff_spring_atom(phys_fixup_tag f) : phys_atom(f) {}
};

struct phys_set_local_transforms_atom : public phys_atom
{
	PHYS_DECLARE_NONVIRTUAL_ALLOCATOR( PHYS_MEM_CONSTRAINT, phys_set_local_transforms_atom );

	phys_set_local_transforms_atom() : phys_atom(ATOM_SET_LOCAL_TRANSFORMS) {}
	__forceinline phys_atom* next() const { return const_cast<phys_atom*>( static_cast<const phys_atom*>(this+1) ); }
	__forceinline int solver_result_count() const    { return 0; }

	phys_transform m_transform_a;

	phys_transform m_transform_b;

	phys_set_local_transforms_atom(phys_fixup_tag f) : phys_atom(f) {}
};

struct phys_set_local_translations_atom : public phys_atom
{
	PHYS_DECLARE_NONVIRTUAL_ALLOCATOR( PHYS_MEM_CONSTRAINT, phys_set_local_translations_atom );

	phys_set_local_translations_atom() : phys_atom(ATOM_SET_LOCAL_TRANSLATIONS) {}
	__forceinline phys_atom* next() const { return const_cast<phys_atom*>( static_cast<const phys_atom*>(this+1) ); }
	__forceinline int solver_result_count() const    { return 0; }

	phys_vec4 m_translation_a;

	phys_vec4 m_translation_b;

	phys_set_local_translations_atom(phys_fixup_tag f) : phys_atom(f) {}
};

struct phys_set_local_rotations_atom : public phys_atom
{
	PHYS_DECLARE_NONVIRTUAL_ALLOCATOR( PHYS_MEM_CONSTRAINT, phys_set_local_rotations_atom );

	phys_set_local_rotations_atom() : phys_atom(ATOM_SET_LOCAL_ROTATIONS) {}
	__forceinline phys_atom* next() const { return const_cast<phys_atom*>( static_cast<const phys_atom*>(this+1) ); }
	__forceinline int solver_result_count() const    { return 0; }

	phys_rotation m_rotation_a;

	phys_rotation m_rotation_b;

	phys_set_local_rotations_atom(phys_fixup_tag f) : phys_atom(f) {}
};

struct phys_overwrite_pivot_atom : public phys_atom
{
	PHYS_DECLARE_NONVIRTUAL_ALLOCATOR( PHYS_MEM_CONSTRAINT, phys_overwrite_pivot_atom );

	phys_overwrite_pivot_atom() : phys_atom(ATOM_OVERWRITE_PIVOT), m_copy_pivot_a_to_b(true) { }
	__forceinline phys_atom* next() const { return const_cast<phys_atom*>( static_cast<const phys_atom*>(this+1) ); }
	__forceinline int solver_result_count() const    { return 0; }

	unsigned char m_copy_pivot_a_to_b;

	phys_overwrite_pivot_atom(phys_fixup_tag f) : phys_atom(f) {}
};

struct phys_lin_atom : public phys_atom
{
	PHYS_DECLARE_NONVIRTUAL_ALLOCATOR( PHYS_MEM_CONSTRAINT, phys_lin_atom );

	phys_lin_atom() : phys_atom(ATOM_LIN) {}
	__forceinline phys_atom* next() const { return const_cast<phys_atom*>( static_cast<const phys_atom*>(this+1) ); }
	__forceinline int solver_result_count() const    { return 1; }
	__forceinline void add_to_constraint_info(phys_constraint_info& out_constraint_info) const {
		out_constraint_info.add( PHYS_SIZE_OF_1D_BILATERAL_SCHEMA, 1, 1 ); }

	unsigned char m_axis_index;

	phys_lin_atom(phys_fixup_tag f) : phys_atom(f) {}
};

struct phys_lin_soft_atom : public phys_atom
{
	PHYS_DECLARE_NONVIRTUAL_ALLOCATOR( PHYS_MEM_CONSTRAINT, phys_lin_soft_atom );

	phys_lin_soft_atom() : phys_atom(ATOM_LIN_SOFT) {}
	__forceinline phys_atom* next() const { return const_cast<phys_atom*>( static_cast<const phys_atom*>(this+1) ); }
	__forceinline int solver_result_count() const    { return 1; }
	__forceinline void add_to_constraint_info(phys_constraint_info& out_constraint_info) const {
		out_constraint_info.add( PHYS_SIZE_OF_1D_BILATERAL_USER_TAU_SCHEMA, 1, 1 ); }

	unsigned char m_axis_index;

	phys_real m_query_tau;

	phys_real m_solver_damping;

	phys_lin_soft_atom(phys_fixup_tag f) : phys_atom(f) {}
};

struct phys_lin_limit_atom : public phys_atom
{
	PHYS_DECLARE_NONVIRTUAL_ALLOCATOR( PHYS_MEM_CONSTRAINT, phys_lin_limit_atom );

	phys_lin_limit_atom() : phys_atom(ATOM_LIN_LIMIT) {}
	__forceinline phys_atom* next() const { return const_cast<phys_atom*>( static_cast<const phys_atom*>(this+1) ); }
	__forceinline int solver_result_count() const    { return 1; }
	__forceinline void add_to_constraint_info(phys_constraint_info& out_constraint_info) const {
		out_constraint_info.add( PHYS_SIZE_OF_1D_LIN_LIMIT_SCHEMA, 1, 1 ); }

	unsigned char m_axis_index;

	phys_real m_min_limit;

	phys_real m_max_limit;

	phys_lin_limit_atom(phys_fixup_tag f) : phys_atom(f) {}
};

struct phys_2d_ang_atom : public phys_atom
{
	PHYS_DECLARE_NONVIRTUAL_ALLOCATOR( PHYS_MEM_CONSTRAINT, phys_2d_ang_atom );

	phys_2d_ang_atom() : phys_atom(ATOM_2D_ANG) {}
	__forceinline phys_atom* next() const { return const_cast<phys_atom*>( static_cast<const phys_atom*>(this+1) ); }
	__forceinline int solver_result_count() const    { return 2; }
	__forceinline void add_to_constraint_info(phys_constraint_info& out_constraint_info) const {
		out_constraint_info.add( 2 * PHYS_SIZE_OF_1D_BILATERAL_SCHEMA, 2, 2 ); }

	unsigned char m_free_rot_axis;

	phys_2d_ang_atom(phys_fixup_tag f) : phys_atom(f) {}
};
typedef phys_2d_ang_atom phys_2d_ang_atom_alias;

struct phys_ang_atom : public phys_atom
{
	PHYS_DECLARE_NONVIRTUAL_ALLOCATOR( PHYS_MEM_CONSTRAINT, phys_ang_atom );

	phys_ang_atom() : phys_atom(ATOM_ANG) {}
	__forceinline phys_atom* next() const { return const_cast<phys_atom*>( static_cast<const phys_atom*>(this+1) ); }
	__forceinline int solver_result_count() const    { return m_constrained_axis_count; }
	__forceinline void add_to_constraint_info(phys_constraint_info& out_constraint_info) const
	{
		out_constraint_info.add( m_constrained_axis_count * PHYS_SIZE_OF_1D_BILATERAL_SCHEMA, m_constrained_axis_count, m_constrained_axis_count );
	}

	unsigned char m_first_locked_axis;

	unsigned char m_constrained_axis_count;

	phys_ang_atom(phys_fixup_tag f) : phys_atom(f) {}
};

struct phys_ang_limit_atom : public phys_atom
{
	PHYS_DECLARE_NONVIRTUAL_ALLOCATOR( PHYS_MEM_CONSTRAINT, phys_ang_limit_atom );

	phys_ang_limit_atom() : phys_atom(ATOM_ANG_LIMIT), m_limit_enabled(true) {}
	__forceinline phys_atom* next() const { return const_cast<phys_atom*>( static_cast<const phys_atom*>(this+1) ); }
	__forceinline int solver_result_count() const    { return 1; }
	__forceinline void add_to_constraint_info(phys_constraint_info& out_constraint_info) const {
		out_constraint_info.add( PHYS_SIZE_OF_1D_ANG_LIMITS_SCHEMA, 1, 1 ); }

	unsigned char m_limit_enabled;

	unsigned char m_limit_axis;

	phys_real m_min_angle;

	phys_real m_atom_max_angle;

	phys_real m_ang_limits_tau_factor;

	phys_ang_limit_atom(phys_fixup_tag f) : phys_atom(f) {}
};

struct phys_twist_limit_atom : public phys_atom
{
	PHYS_DECLARE_NONVIRTUAL_ALLOCATOR( PHYS_MEM_CONSTRAINT, phys_twist_limit_atom );

	phys_twist_limit_atom() : phys_atom(ATOM_TWIST_LIMIT), m_limit_enabled(true) {}
	__forceinline phys_atom* next() const { return const_cast<phys_atom*>( static_cast<const phys_atom*>(this+1) ); }
	__forceinline int solver_result_count() const    { return 1; }
	__forceinline void add_to_constraint_info(phys_constraint_info& out_constraint_info) const {
		out_constraint_info.add( PHYS_SIZE_OF_1D_ANG_LIMITS_SCHEMA, 1, 1 ); }

	unsigned char m_limit_enabled;

	unsigned char m_twist_axis;

	unsigned char m_ref_axis;

	phys_real m_min_angle;

	phys_real m_atom_max_angle;

	phys_real m_ang_limits_tau_factor;

	phys_twist_limit_atom(phys_fixup_tag f) : phys_atom(f) {}
};

struct phys_cone_limit_atom : public phys_atom
{
	PHYS_DECLARE_NONVIRTUAL_ALLOCATOR( PHYS_MEM_CONSTRAINT, phys_cone_limit_atom );

	phys_cone_limit_atom() : phys_atom(ATOM_CONE_LIMIT), m_limit_enabled(true) {}
	__forceinline phys_atom* next() const { return const_cast<phys_atom*>( static_cast<const phys_atom*>(this+1) ); }
	__forceinline int solver_result_count() const    { return 1; }
	__forceinline void add_to_constraint_info(phys_constraint_info& out_constraint_info) const {
		out_constraint_info.add( PHYS_SIZE_OF_1D_ANG_LIMITS_SCHEMA, 1, 1 ); }

	unsigned char m_limit_enabled;

	unsigned char m_twist_axis_in_a;

	unsigned char m_ref_axis_in_b;

	enum measurement_mode
	{

		ANG_ZERO_WHEN_ALIGNED = 0,

		ANG_ZERO_WHEN_PERPENDICULAR = 1
	};

	phys_enum<measurement_mode, unsigned char> m_angle_measure_mode;

	unsigned char m_mem_to_angle_offset;

	phys_real m_min_angle;

	phys_real m_atom_max_angle;

	phys_real m_ang_limits_tau_factor;

	phys_cone_limit_atom(phys_fixup_tag f) : phys_atom(f) {}
};

struct phys_ang_friction_atom : public phys_atom
{
	PHYS_DECLARE_NONVIRTUAL_ALLOCATOR( PHYS_MEM_CONSTRAINT, phys_ang_friction_atom );

	phys_ang_friction_atom() : phys_atom(ATOM_ANG_FRICTION), m_limit_enabled(true), m_friction_axis_count(1) {}
	__forceinline phys_atom* next() const { return const_cast<phys_atom*>( static_cast<const phys_atom*>(this+1) ); }
	__forceinline int solver_result_count() const    { return m_friction_axis_count; }
	__forceinline void add_to_constraint_info(phys_constraint_info& out_constraint_info) const
	{
		out_constraint_info.add( m_friction_axis_count * PHYS_SIZE_OF_1D_ANG_FRICTION_SCHEMA, m_friction_axis_count, (1+1) * m_friction_axis_count );
	}

	unsigned char m_limit_enabled;

	unsigned char m_first_friction_axis;

	unsigned char m_friction_axis_count;

	phys_real m_max_friction_torque;

	phys_ang_friction_atom(phys_fixup_tag f) : phys_atom(f) {}
};

struct phys_ang_motor_atom : public phys_atom
{
	PHYS_DECLARE_NONVIRTUAL_ALLOCATOR( PHYS_MEM_CONSTRAINT, phys_ang_motor_atom );

	phys_ang_motor_atom() : phys_atom(ATOM_ANG_MOTOR) { m_limit_enabled = true; m_initialized_offset = 0; m_prev_target_angle_offset = 0; }
	__forceinline phys_atom* next() const { return const_cast<phys_atom*>( static_cast<const phys_atom*>(this+1) ); }
	__forceinline int solver_result_count() const    { return 1; }
	__forceinline void add_to_constraint_info(phys_constraint_info& out_constraint_info) const {
		out_constraint_info.add( PHYS_SIZE_OF_1D_ANG_MOTOR_SCHEMA, 1, (1+2) ); }

	phys_bool m_limit_enabled;

	unsigned char m_motor_axis;

	short m_initialized_offset;

	short m_prev_target_angle_offset;

	short m_ang_limit_result_offset;

	phys_real m_target_angle;

	class phys_motor* m_schema_motor;

	phys_ang_motor_atom(phys_fixup_tag f) : phys_atom(f) {}
};

struct phys_ragdoll_motor_atom : public phys_atom
{
	PHYS_DECLARE_NONVIRTUAL_ALLOCATOR( PHYS_MEM_CONSTRAINT, phys_ragdoll_motor_atom );

	phys_ragdoll_motor_atom() : phys_atom(ATOM_RAGDOLL_MOTOR) { m_limit_enabled = true; m_initialized_offset = 0; m_prev_target_angles_offset = 0; }
	__forceinline phys_atom* next() const { return const_cast<phys_atom*>( static_cast<const phys_atom*>(this+1) ); }
	__forceinline int solver_result_count() const    { return 3; }
	__forceinline void add_to_constraint_info(phys_constraint_info& out_constraint_info) const {
		out_constraint_info.add( 3 * PHYS_SIZE_OF_1D_ANG_MOTOR_SCHEMA, 3, (1+2) * 3 ); }

	phys_bool m_limit_enabled;

	short m_initialized_offset;

	short m_prev_target_angles_offset;

	phys_mat3 m_target_frame_a_in_b;

	class phys_motor* m_axis_motors[3];

	phys_ragdoll_motor_atom(phys_fixup_tag f) : phys_atom(f) {}
};

struct phys_lin_friction_atom : public phys_atom
{
	PHYS_DECLARE_NONVIRTUAL_ALLOCATOR( PHYS_MEM_CONSTRAINT, phys_lin_friction_atom );

	phys_lin_friction_atom() : phys_atom(ATOM_LIN_FRICTION), m_limit_enabled(true) {}
	__forceinline phys_atom* next() const { return const_cast<phys_atom*>( static_cast<const phys_atom*>(this+1) ); }
	__forceinline int solver_result_count() const    { return 1; }
	__forceinline void add_to_constraint_info(phys_constraint_info& out_constraint_info) const {
		out_constraint_info.add( PHYS_SIZE_OF_1D_FRICTION_SCHEMA, 1, (1+1) ); }

	unsigned char m_limit_enabled;

	unsigned char m_friction_axis;

	phys_real m_max_friction_force;

	phys_lin_friction_atom(phys_fixup_tag f) : phys_atom(f) {}
};

struct phys_lin_motor_atom : public phys_atom
{
	PHYS_DECLARE_NONVIRTUAL_ALLOCATOR( PHYS_MEM_CONSTRAINT, phys_lin_motor_atom );

	phys_lin_motor_atom() : phys_atom(ATOM_LIN_MOTOR) { m_limit_enabled = true; m_initialized_offset = 0; m_prev_target_pos_offset = 0; }
	__forceinline phys_atom* next() const { return const_cast<phys_atom*>( static_cast<const phys_atom*>(this+1) ); }
	__forceinline int solver_result_count() const    { return 1; }
	__forceinline void add_to_constraint_info(phys_constraint_info& out_constraint_info) const {
		out_constraint_info.add( PHYS_SIZE_OF_1D_LIN_MOTOR_SCHEMA, 1, (1+2) ); }

	phys_bool  m_limit_enabled;

	unsigned char m_motor_axis;

	short m_initialized_offset;

	short m_prev_target_pos_offset;

	phys_real m_target_pos;

	class phys_motor* m_schema_motor;

	phys_lin_motor_atom(phys_fixup_tag f) : phys_atom(f) {}
};

struct phys_pulley_atom : public phys_atom
{
	PHYS_DECLARE_NONVIRTUAL_ALLOCATOR( PHYS_MEM_CONSTRAINT, phys_pulley_atom );

	phys_pulley_atom() : phys_atom(ATOM_PULLEY) { }
	__forceinline phys_atom* next() const { return const_cast<phys_atom*>( static_cast<const phys_atom*>(this+1) ); }
	__forceinline int solver_result_count() const    { return 1; }
	__forceinline void add_to_constraint_info(phys_constraint_info& out_constraint_info) const {
		out_constraint_info.add( PHYS_SIZE_OF_1D_PULLEY_SCHEMA, 1, 1 ); }

	phys_vec4 m_fixed_pivot_a_world;

	phys_vec4 m_fixed_pivot_b_world;

	phys_real m_rope_length;

	phys_real m_leverage_on_b;

	phys_pulley_atom(phys_fixup_tag f) : phys_atom(f) {}
};

struct phys_modifier_atom : public phys_atom
{
		PHYS_DECLARE_NONVIRTUAL_ALLOCATOR(PHYS_MEM_CONSTRAINT, phys_modifier_atom );

	public:

		int add_modifier_info( phys_constraint_info& cinfo ) const;

		static int PHYS_CALL add_all_modifier_info( phys_modifier_atom* first_modifier_atom, phys_constraint_info& cinfo );

	protected:

		phys_modifier_atom(enum atom_kind type, int size) : phys_atom(type), m_modifier_atom_size(static_cast<unsigned short>(size)) {}

	public:

		__declspec(align(16)) unsigned short		  m_modifier_atom_size;
		unsigned short                      m_child_size;
		phys_atom* m_child_shape;
		unsigned int m_pad[2];

		phys_modifier_atom(phys_fixup_tag f) : phys_atom(f) {}
};

struct phys_soft_contact_modifier_atom : public phys_modifier_atom
{
		__forceinline phys_atom* next() const { return const_cast<phys_atom*>( static_cast<const phys_atom*>(this+1) ); }
		__forceinline int solver_result_count() const { return 0; }
		PHYS_DECLARE_NONVIRTUAL_ALLOCATOR(PHYS_MEM_CONSTRAINT, phys_soft_contact_modifier_atom );

	public:

		phys_soft_contact_modifier_atom() : phys_modifier_atom(ATOM_MOD_SOFT_CONTACT, sizeof(phys_soft_contact_modifier_atom)),
			m_query_tau(0.1f), m_max_accel( 20.0f)
		{
			}

		void collision_response_begin_callback( const phys_contact_point& cp, struct phys_simple_constraint_init_input& init_input_a,
			struct phys_body_vel& body_vel_a,
			phys_simple_constraint_init_input& init_input_b, phys_body_vel& body_vel_b);
		void collision_response_end_callback(   const phys_contact_point& cp, phys_real applied_impulse, struct phys_simple_constraint_init_input& init_input_a,
			struct phys_body_vel& body_vel_a, phys_simple_constraint_init_input& init_input_b, phys_body_vel& body_vel_b);

		int get_constraint_info( phys_constraint_info& info ) const	{		return phys_atom::ATOM_CALLBACK_SETUP_HOST_ONLY;		}

	public:

		phys_real m_query_tau;

		phys_real m_max_accel;

		phys_soft_contact_modifier_atom(phys_fixup_tag f) : phys_modifier_atom(f) {}
};

struct phys_mass_changer_atom : public phys_modifier_atom
{
		__forceinline phys_atom* next() const { return const_cast<phys_atom*>( static_cast<const phys_atom*>(this+1) ); }
		__forceinline int solver_result_count() const { return 0; }
		PHYS_DECLARE_NONVIRTUAL_ALLOCATOR(PHYS_MEM_CONSTRAINT, phys_mass_changer_atom );

	public:

		phys_mass_changer_atom() : phys_modifier_atom(ATOM_MOD_MASS_CHANGER, sizeof(phys_mass_changer_atom)) { }


		void collision_response_begin_callback( const phys_contact_point& cp, struct phys_simple_constraint_init_input& init_input_a,
			struct phys_body_vel& body_vel_a,
			phys_simple_constraint_init_input& init_input_b, phys_body_vel& body_vel_b);
		void collision_response_end_callback(   const phys_contact_point& cp, phys_real applied_impulse, struct phys_simple_constraint_init_input& init_input_a,
			struct phys_body_vel& body_vel_a, phys_simple_constraint_init_input& init_input_b, phys_body_vel& body_vel_b);

		int get_constraint_info( phys_constraint_info& info ) const
		{
			info.m_schemas_size   += 2 * PHYS_SIZE_OF_SET_MASS_SCHEMA  + PHYS_SIZE_OF_HEADER_SCHEMA;
			return phys_atom::ATOM_CALLBACK_NONE;
		}

	public:

		phys_real m_factor_a;
		phys_real m_factor_b;
		phys_real m_atom_pad16[2];

		phys_mass_changer_atom(phys_fixup_tag f) : phys_modifier_atom(f) {}
};

struct phys_viscous_surface_atom : public phys_modifier_atom
{
		__forceinline phys_atom* next() const { return const_cast<phys_atom*>( static_cast<const phys_atom*>(this+1) ); }
		__forceinline int solver_result_count() const { return 0; }
		PHYS_DECLARE_NONVIRTUAL_ALLOCATOR(PHYS_MEM_CONSTRAINT, phys_viscous_surface_atom );

	public:

		phys_viscous_surface_atom() : phys_modifier_atom(ATOM_MOD_VISCOUS_SURFACE, sizeof(phys_viscous_surface_atom)) { }

		int get_constraint_info( phys_constraint_info& info ) const	{		return phys_atom::ATOM_CALLBACK_SETUP_HOST_ONLY;		}


	public:

		phys_viscous_surface_atom(phys_fixup_tag f) : phys_modifier_atom(f) {}
};

struct phys_moving_surface_atom : public phys_modifier_atom
{
		__forceinline phys_atom* next() const { return const_cast<phys_atom*>( static_cast<const phys_atom*>(this+1) ); }
		__forceinline int solver_result_count() const { return 0; }
		PHYS_DECLARE_NONVIRTUAL_ALLOCATOR(PHYS_MEM_CONSTRAINT, phys_moving_surface_atom );

	public:

		phys_moving_surface_atom() : phys_modifier_atom(ATOM_MOD_MOVING_SURFACE, sizeof(phys_moving_surface_atom)) { }


		int get_constraint_info( phys_constraint_info& info ) const
		{
			info.m_schemas_size   += 2 * PHYS_SIZE_OF_ADD_VEL_SCHEMA + PHYS_SIZE_OF_HEADER_SCHEMA;
			return phys_atom::ATOM_CALLBACK_NONE;
		}

		void collision_response_begin_callback( const phys_contact_point& cp, struct phys_simple_constraint_init_input& init_input_a,
			struct phys_body_vel& body_vel_a,
			phys_simple_constraint_init_input& init_input_b, phys_body_vel& body_vel_b);
		void collision_response_end_callback(   const phys_contact_point& cp, phys_real applied_impulse, struct phys_simple_constraint_init_input& init_input_a,
			struct phys_body_vel& body_vel_a, phys_simple_constraint_init_input& init_input_b, phys_body_vel& body_vel_b);

		phys_vec4& get_atom_velocity() { return m_velocity; }
		const phys_vec4& get_atom_velocity() const { return m_velocity; }

	public:

		phys_vec4 m_velocity;

		phys_moving_surface_atom(phys_fixup_tag f) : phys_modifier_atom(f) {}
};

#endif // INCLUDE_HAVOK_HK_CONSTRAINT_ATOMS_H
