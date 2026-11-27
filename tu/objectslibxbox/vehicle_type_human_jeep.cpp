// objectslibxbox/vehicle_type_human_jeep.obj: from the records 0x3705b0 0x370c30 0x370e88 0x370690
// @flags /TP /O2 /Oc
#include "units/vehicle_type_human_jeep.h"
#include "units/vehicle_definitions.h"
#include "models/model_types.h"
#include "models/render_model_definitions.h"
#include "objects/object_datum.h"
#include "objects/object_header.h"
#include "tag_files/tag_groups.h"
#include "cseries/cseries.h"
#include "math/real_math.h"
#include <math.h>
#include "animations/animation_id.h"
#include "math/matrix_math.h"
#include "units/vehicle_engine.h"
#include "units/vehicle_utilities.h"
#include "units/vehicles.h"
#include "animations/animation_manager.h"
#include "units/vehicle_datum.h"
#include "physics/havok_vehicle_physics.h"
#include "animations/mixing_board/channels/animation_channel.h"
#include "math/real_math_types.h"
#include "physics/physics_variable_types.h"

static __forceinline float human_jeep_game_tick_length() { return tls_game_time_globals()->tick_length; }
static __forceinline vehicle_datum *human_jeep_physics_vehicle_get(long index)
{
    return (vehicle_datum *)object_get_and_verify_type(index, _object_mask_vehicle);
}
static __forceinline void human_jeep_matrix4x3_rotation_from_quaternion(real_matrix4x3 *matrix,
    const real_quaternion *quaternion) {
 matrix3x3_rotation_from_quaternion(&matrix->rotation,quaternion);
 matrix->position.x=0.0f; matrix->position.y=0.0f; matrix->position.z=0.0f;
 matrix->scale=1.0f;
}
typedef void (*animation_callback)(long,const render_model_definition *,c_animation_channel *,float,float,float,void *);
static __forceinline vehicle_datum *human_jeep_datum_get(long index,
    unsigned long mask) { return (vehicle_datum *)object_get_and_verify_type(index, mask); }
static __forceinline const s_vehicle_human_jeep_definition *human_jeep_definition_get(long index) { return TAG_GET('vehi',
    vehicle_definition, index)->jeep; }
static __forceinline const s_vehicle_human_jeep_definition *human_jeep_vehicle_definition_get(long index)
{
    return ((vehicle_definition *)tag_get(0, index))->jeep;
}
static __forceinline void *human_jeep_object_header_block_get(long index,
    const short *reference) { return (char *)human_jeep_datum_get(index, -1)+*reference; }
// @address 0x3705b0 ?update_control@c_vehicle_type_human_jeep@@QAAXJ@Z
void c_vehicle_type_human_jeep::update_control(long index) {
 vehicle_datum *v=human_jeep_datum_get(index,2);
 const s_vehicle_human_jeep_definition *definition=human_jeep_definition_get(v->object.definition_index);
 real_vector3d control; scale_vector3d(&v->unit.throttle,v->unit.throttle_scale,&control);
 vehicle_update_boost(index,0);
 ::vehicle_update_control_classic(index, &v->object.forward, &v->object.up, control.i, control.j,
     &definition->steering_control, &definition->turning_control,
     0, 0, engine.gear == 0, false, &turning_value,
     (float *)(0), (float *)(0));
 e_braking=(human_jeep_datum_get(index,2)->unit.control_flags&0x800)!=0;
}
static __forceinline float human_jeep_safe_divide(float numerator,float denominator) {
 float result=0.0f;
 if(fabsf(denominator)>0.001f) result=numerator/denominator;
 return result;
}
static __forceinline float human_jeep_gear_ratio(const s_vehicle_engine *engine,
    const s_vehicle_engine_definition *definition, short gear_index) {
 float result=0.0f;
 if(gear_index!=-1) {
  if(gear_index>=0 && gear_index<definition->gears.count) {
   const s_vehicle_gear_definition *gear_definition=&((const s_vehicle_gear_definition *)definition->gears.address)[gear_index];
   result=gear_definition->ratio;
  }
 } else {
  result=0.0f;
 }
 return result;
}
static __forceinline float human_jeep_wheel_ground_speed(const s_vehicle_engine *engine,
    const s_vehicle_engine_definition *definition) {
 return engine->rpm*human_jeep_gear_ratio(engine,definition,engine->gear);
}
static __forceinline float human_jeep_normalize3d(real_vector3d *vector) {
 float length=sqrtf(magnitude_squared3d(vector));
 if(!(fabsf(length-0.0f)<k_real_epsilon)) scale_vector3d(vector,1.0f/length,vector);
 else length=0.0f;
 return length;
}
// @address 0x370690 ?update_physics@c_vehicle_type_human_jeep@@QAAXJPAUs_havok_vehicle_physics_instance@@@Z
void c_vehicle_type_human_jeep::update_physics(long vehicle_index,s_havok_vehicle_physics_instance *instance) {
 float tick_length=human_jeep_game_tick_length();
 vehicle_datum *vehicle=human_jeep_physics_vehicle_get(vehicle_index);
 const s_vehicle_human_jeep_definition *definition=human_jeep_vehicle_definition_get(vehicle->object.definition_index);
 float half_turning=turning_value*0.5f;
 float cosine=(float)cos(half_turning);
 float sine=(float)sin(half_turning);
 float throttle=vehicle_compute_throttle1d(vehicle_index);
 float abs_throttle=throttle>=0.0f?throttle:-throttle;
 braking_counter.update(vehicle_index);
 bool braking=braking_counter.is_braking(vehicle_index);
 short desired_gear=vehicle_engine_get_desired_gear(&engine, &definition->engine, throttle, braking,
     vehicle->unit.driver_object_index!=-1, false);
 vehicle_engine_update(vehicle_index,&engine,&definition->engine,abs_throttle,desired_gear,braking);
 float wheel_speed=braking?0.0f:human_jeep_wheel_ground_speed(&engine,&definition->engine);
 real_quaternion quaternion;
 for(long point_index=0;point_index<instance->definition->friction_points.count;point_index++) {
  const s_havok_vehicle_friction_point_definition *point_definition=
      &((const s_havok_vehicle_friction_point_definition *)
          instance->definition->friction_points.address)[point_index];
  bool positive_rotation=(point_definition->flags>>2)&1;
  bool negative_rotation=(point_definition->flags>>3)&1;
  instance->friction_points[point_index].forward_velocity=wheel_speed;
  if(braking) instance->friction_points[point_index].braking=true;
  if(positive_rotation) {
   quaternion.v.i=0.0f; quaternion.v.j=0.0f; quaternion.v.k=sine; quaternion.w=cosine;
   human_jeep_matrix4x3_rotation_from_quaternion(&instance->friction_points[point_index].matrix,&quaternion);
   matrix4x3_compute_product(&instance->world_matrix, &instance->friction_points[point_index].matrix,
       &instance->friction_points[point_index].matrix);
  } else if(negative_rotation) {
   quaternion.v.i=0.0f; quaternion.v.j=0.0f; quaternion.v.k=-sine; quaternion.w=cosine;
   human_jeep_matrix4x3_rotation_from_quaternion(&instance->friction_points[point_index].matrix,&quaternion);
   matrix4x3_compute_product(&instance->world_matrix, &instance->friction_points[point_index].matrix,
       &instance->friction_points[point_index].matrix);
  } else {
   instance->friction_points[point_index].matrix.rotation=instance->world_matrix.rotation;
  }
 }
 for(long wheel_index=0;wheel_index<instance->definition->anti_gravity_points.count;wheel_index++)
  instance->anti_gravity_points[wheel_index].strength=vehicle->unit.throttle_scale;
 float stop_fraction=stop_counter.calculate_fraction(vehicle_index);
 havok_vehicle_physics_update(vehicle_index,instance,stop_fraction,true,0,0,0);
 engine.load_torque=instance->load_torque;
 bool any_active=false;
 for(long point_index=0;point_index<instance->definition->friction_points.count;point_index++) {
  if(instance->friction_points[point_index].grounded) {
   any_active=true;
   break;
  }
 }
 if(!any_active) engine.up_ticks=0;
 stop_counter.update(vehicle_index,abs_throttle,braking);
 steering_value+=wheel_speed*tick_length;
 steering_value=definition->wheel_circumference>0.001f?(float)fmod(steering_value,definition->wheel_circumference):0.0f;
 if(steering_value<0.0f) steering_value+=definition->wheel_circumference;
 if(!e_braking) {
  velocity_value+=wheel_speed*tick_length;
  velocity_value=definition->wheel_circumference>0.001f?(float)fmod(velocity_value,
      definition->wheel_circumference):0.0f;
  if(velocity_value<0.0f) velocity_value+=definition->wheel_circumference;
 }
 real_point3d positions[3];
 long point_count=0;
 up=instance->world_matrix.rotation.up;
 for(long point_index=0;point_index<instance->definition->friction_points.count;point_index++) {
  if(instance->friction_points[point_index].grounded) {
   positions[point_count]=instance->friction_points[point_index].position;
   point_count++;
  }
  if(point_count==3) {
   real_vector3d first,second,normal;
   vector_from_points3d(&positions[0],&positions[1],&first);
   vector_from_points3d(&positions[0],&positions[2],&second);
   cross_product3d(&first,&second,&normal);
   if(human_jeep_normalize3d(&normal)!=0.0f) {
    if(dot_product3d(&normal,global_up3d)<0.0f) scale_vector3d(&normal,-1.0f,&normal);
    up=normal;
   }
   break;
  }
 }
}
// @address 0x370c30 ?process_animation_channels@c_vehicle_type_human_jeep@@QAAXJP6AXJPBUrender_model_definition@@PAVc_animation_channel@@MMMPAX@Z21_N@Z
void c_vehicle_type_human_jeep::process_animation_channels(long vehicle_index, animation_callback callback,
    void *context, c_animation_channel *channel, bool find_animations)
{
 vehicle_datum *vehicle=human_jeep_datum_get(vehicle_index,2);
 const vehicle_definition *definition=(vehicle_definition *)tag_get(0,vehicle->object.definition_index);
 const s_vehicle_human_jeep_definition *jeep=human_jeep_vehicle_definition_get(vehicle->object.definition_index);
 const s_model_definition *model=(const s_model_definition *)tag_get(0,definition->model.index);
 const render_model_definition *render_model=(render_model_definition *)tag_get(0,model->render_model_index);
 c_animation_manager *manager=(c_animation_manager *)human_jeep_object_header_block_get(vehicle_index,
     &vehicle->object.animation.offset);
 if(find_animations) {
  steering=manager->find_parallel_impulse_animation_id(226,226,129);
  velocity=manager->find_parallel_impulse_animation_id(226,226,614);
  turning=manager->find_parallel_impulse_animation_id(226,226,124);
 }
 if(steering.valid() && manager->setup_animation_channel_by_index(channel,steering,0)) {
  float value=jeep->wheel_circumference>0.0f ? steering_value/jeep->wheel_circumference : 0.0f;
  vehicle_impulse_channel_node_preprocess_callback(vehicle_index, render_model, (vehicles_animation_channel *)channel,
      value, 0.0f, 1.0f, context);
 }
 if(velocity.valid() && manager->setup_animation_channel_by_index(channel,velocity,0)) {
  float value=jeep->wheel_circumference>0.0f ? velocity_value/jeep->wheel_circumference : 0.0f;
  vehicle_impulse_channel_node_preprocess_callback(vehicle_index, render_model, (vehicles_animation_channel *)channel,
      value, 0.0f, 1.0f, context);
 }
 if (turning.valid() && manager->setup_animation_channel_by_index(channel, turning, 0))
     vehicle_impulse_channel_node_preprocess_callback(vehicle_index, render_model,
         (vehicles_animation_channel *)channel, turning_value, 0.0f, 1.0f, context);
}
// @address 0x370e88 ?compute_function_value@c_vehicle_type_human_jeep@@QAA_NJJPAMPA_N@Z
bool c_vehicle_type_human_jeep::compute_function_value(long vehicle_index, long function_name, float *value,
    bool *active) {
 // match: 0x370e88: pointer __restrict qualifier prevents alias reload
 vehicle_datum *__restrict vehicle=(vehicle_datum *)object_get_and_verify_type(vehicle_index,_object_mask_vehicle);
 const s_vehicle_human_jeep_definition *definition=TAG_GET('vehi', vehicle_definition,
     vehicle->object.definition_index)->jeep;
 bool result=true;
 switch(function_name) {
 case _string_id_engine_power:
  *value=human_jeep_safe_divide(engine.rpm,definition->engine.maximum_rpm);
  *value=*value*vehicle_engine_get_rpm_function_scale(&engine);
  if(vehicle->unit.throttle_scale<=0.0f) {
   *value=0.0f;
   *active=false;
  } else {
   *active=true;
  }
  break;
 case _string_id_speed_absolute: {
  float maximum_ground_speed=human_jeep_gear_ratio(&engine, &definition->engine,
      (short)(definition->engine.gears.count-1))*definition->engine.maximum_rpm;
  float ground_speed=human_jeep_wheel_ground_speed(&engine,&definition->engine);
  *value=human_jeep_safe_divide(fabsf(ground_speed),fabsf(maximum_ground_speed));
  break;
 }
 case _string_id_engine_rpm:
  *value=human_jeep_safe_divide(engine.rpm,definition->engine.maximum_rpm);
  *value=*value*vehicle_engine_get_rpm_function_scale(&engine);
  break;
 case _string_id_engine_cruising:
  *value=PIN(1.0f-engine.throttle,0.0f,1.0f)*human_jeep_safe_divide(engine.rpm,definition->engine.maximum_rpm);
  break;
 case _string_id_jump:
  *value=braking_counter.is_braking(vehicle_index) || (vehicle->unit.control_flags&2)!=0 || e_braking ? 1.0f : 0.0f;
  break;
 case _string_id_front_left_tire_position: case _string_id_front_right_tire_position:
  *value=human_jeep_safe_divide(steering_value,definition->wheel_circumference);
  break;
 case _string_id_back_left_tire_position: case _string_id_back_right_tire_position:
  *value=human_jeep_safe_divide(velocity_value,definition->wheel_circumference);
  break;
 case _string_id_front_left_tire_velocity: case _string_id_front_right_tire_velocity:
  if(braking_counter.is_braking(vehicle_index) || (vehicle->unit.control_flags&2)!=0)
   *value=0.0f;
  else
   *value=human_jeep_safe_divide(fabsf(engine.rpm),fabsf(definition->engine.maximum_rpm));
  break;
 case _string_id_back_left_tire_velocity: case _string_id_back_right_tire_velocity:
  if(braking_counter.is_braking(vehicle_index) || (vehicle->unit.control_flags&2)!=0 || e_braking)
   *value=0.0f;
  else
   *value=human_jeep_safe_divide(fabsf(engine.rpm),fabsf(definition->engine.maximum_rpm));
  break;
 default:
  result=false;
  break;
 }
 return result;
}
