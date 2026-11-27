// hkdynamics/hkConstraintData.obj: from the records 0x65dc40 0x65dc60 0x65dc68
// @compiler xdk6130
// @flags /TP /O2 /Oc /GF /EHs-c- /GR-
// @address 0x65dc60 ?get_solver_results@phys_constraint_data@@UAAPAVphys_solver_results@@PAX@Z
// @address 0x65dc40 ?add_constraint_instance@phys_constraint_data@@UBAXPAVphys_constraint@@PAXH@Z
// @address 0x65dc68 ?gather_constraint_info@phys_constraint_data@@SAXPBUphys_atom@@HAAUconstraint_info@1@@Z
#define PHYS_CONFIG_SIMD 1
#include "phys/phys_solver.h"

#include "phys/phys_atoms.h"
void phys_constraint_data::add_constraint_instance( phys_constraint* constraint, phys_constraint_runtime* runtime, int runtime_size ) const
{
	if ( runtime )
	{
		phys_string::mem_fill( runtime, 0, runtime_size );
	}
}

phys_solver_results* phys_constraint_data::get_solver_results( phys_constraint_runtime* runtime )
{
	return static_cast<phys_solver_results*>(runtime);
}

#define PHYS_SKIP_ATOM_BY_TYPE(constraint_atom_type, atom_class_name) \
	{case phys_atom::constraint_atom_type: { \
		const atom_class_name* atom = static_cast<const atom_class_name*>(cur_atom); \
		cur_atom = atom->next(); \
	} break;}

#define PHYS_GET_CONSTRAINT_INFO_FROM_ATOM_BY_TYPE(constraint_atom_type, atom_class_name) \
	{case phys_atom::constraint_atom_type: { \
		const atom_class_name* atom = static_cast<const atom_class_name*>(cur_atom); \
		atom->add_to_constraint_info(out_constraint_info); \
		cur_atom = atom->next(); \
	} break;}

void phys_constraint_data::gather_constraint_info( const phys_atom* constraint_atoms, int all_atoms_size,
	phys_constraint_data::constraint_info& out_constraint_info)
{
	out_constraint_info.m_data_atoms = const_cast<phys_atom*>(constraint_atoms);
	out_constraint_info.m_all_atoms_size = all_atoms_size;
	out_constraint_info.clear();
	out_constraint_info.add_header_size();

	if (constraint_atoms->m_type == phys_atom::ATOM_CONTACT)
	{
		phys_simple_contact_atom* contact_atom = static_cast<phys_simple_contact_atom*>( const_cast<phys_atom*>(constraint_atoms) );
		contact_atom->add_to_constraint_info(out_constraint_info);
	}
	else
	{
		const phys_atom* atoms_end    = phys_add_byte_offset_const<const phys_atom>( constraint_atoms, all_atoms_size );
		for( const phys_atom* cur_atom = constraint_atoms; cur_atom < atoms_end; )
		{
			for (;;)
			{
				switch(cur_atom->m_type)
				{
				case phys_atom::ATOM_INVALID:
					{

						cur_atom = reinterpret_cast<phys_atom*>( ((reinterpret_cast<uintptr_t>(cur_atom) + 15) & ~static_cast<uintptr_t>(15)) );
						continue;
					}

					PHYS_GET_CONSTRAINT_INFO_FROM_ATOM_BY_TYPE( ATOM_BALL_SOCKET,				phys_ball_socket_atom      );
					PHYS_GET_CONSTRAINT_INFO_FROM_ATOM_BY_TYPE( ATOM_STIFF_SPRING,			phys_stiff_spring_atom		);

					PHYS_GET_CONSTRAINT_INFO_FROM_ATOM_BY_TYPE( ATOM_LIN,						phys_lin_atom             );
					PHYS_GET_CONSTRAINT_INFO_FROM_ATOM_BY_TYPE( ATOM_LIN_SOFT,				phys_lin_soft_atom         );
					PHYS_GET_CONSTRAINT_INFO_FROM_ATOM_BY_TYPE( ATOM_LIN_LIMIT,				phys_lin_limit_atom        );
					PHYS_GET_CONSTRAINT_INFO_FROM_ATOM_BY_TYPE( ATOM_LIN_FRICTION,			phys_lin_friction_atom		);
					PHYS_GET_CONSTRAINT_INFO_FROM_ATOM_BY_TYPE( ATOM_LIN_MOTOR,				phys_lin_motor_atom		);

					PHYS_GET_CONSTRAINT_INFO_FROM_ATOM_BY_TYPE( ATOM_2D_ANG,					phys_2d_ang_atom_alias			);

					PHYS_GET_CONSTRAINT_INFO_FROM_ATOM_BY_TYPE( ATOM_ANG,						phys_ang_atom				);
					PHYS_GET_CONSTRAINT_INFO_FROM_ATOM_BY_TYPE( ATOM_ANG_LIMIT,				phys_ang_limit_atom        );
					PHYS_GET_CONSTRAINT_INFO_FROM_ATOM_BY_TYPE( ATOM_CONE_LIMIT,				phys_cone_limit_atom		);
					PHYS_GET_CONSTRAINT_INFO_FROM_ATOM_BY_TYPE( ATOM_TWIST_LIMIT,				phys_twist_limit_atom		);
					PHYS_GET_CONSTRAINT_INFO_FROM_ATOM_BY_TYPE( ATOM_ANG_FRICTION,			phys_ang_friction_atom     );
					PHYS_GET_CONSTRAINT_INFO_FROM_ATOM_BY_TYPE( ATOM_ANG_MOTOR,				phys_ang_motor_atom        );

					PHYS_GET_CONSTRAINT_INFO_FROM_ATOM_BY_TYPE( ATOM_RAGDOLL_MOTOR,			phys_ragdoll_motor_atom	);
					PHYS_GET_CONSTRAINT_INFO_FROM_ATOM_BY_TYPE( ATOM_PULLEY,					phys_pulley_atom			);

					PHYS_SKIP_ATOM_BY_TYPE( ATOM_SET_LOCAL_TRANSFORMS,	phys_set_local_transforms_atom		);
					PHYS_SKIP_ATOM_BY_TYPE( ATOM_SET_LOCAL_ROTATIONS,		phys_set_local_rotations_atom		);
					PHYS_SKIP_ATOM_BY_TYPE( ATOM_SET_LOCAL_TRANSLATIONS,	phys_set_local_translations_atom	);

				case phys_atom::ATOM_BRIDGE:
				case phys_atom::ATOM_MOD_SOFT_CONTACT:
				case phys_atom::ATOM_MOD_MASS_CHANGER:
				case phys_atom::ATOM_MOD_VISCOUS_SURFACE:
				case phys_atom::ATOM_MOD_MOVING_SURFACE:
					{

						cur_atom = atoms_end;
						break;
					}

				default:
					break;
				}
			break;
			}
		}
	}
}

#undef PHYS_SKIP_ATOM_BY_TYPE
#undef PHYS_GET_CONSTRAINT_INFOR_FROM_ATOM_BY_TYPE


