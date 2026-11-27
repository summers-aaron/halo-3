#ifndef UNITS_VEHICLE_TYPE_COMPONENT_INTERNALS_H
#define UNITS_VEHICLE_TYPE_COMPONENT_INTERNALS_H

#include "physics/havok_vehicle_physics.h"
#include "physics/physics_variable_types.h"
#include "units/vehicle_definitions.h"
#include "units/vehicle_mantis_definitions.h"
#include "units/vehicle_type_alien_fighter.h"
#include "units/vehicle_type_alien_scout.h"
#include "units/vehicle_type_chopper.h"
#include "units/vehicle_type_guardian.h"
#include "units/vehicle_type_human_jeep.h"
#include "units/vehicle_type_human_plane.h"
#include "units/vehicle_type_human_tank.h"
#include "units/vehicle_type_mantis.h"
#include "units/vehicle_type_turret.h"
#include "units/vehicle_type_vtol.h"
#include "units/vehicle_type_component.h"

struct vehicle_type_component_control_definition
{
    s_vehicle_steering_control_definition steering;
    char turning_control_storage[0x68 - 0x08];
    s_vehicle_steering_animation_definition steering_animation;
};

#endif // UNITS_VEHICLE_TYPE_COMPONENT_INTERNALS_H
