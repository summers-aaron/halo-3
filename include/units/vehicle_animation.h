#ifndef UNITS_VEHICLE_ANIMATION_H
#define UNITS_VEHICLE_ANIMATION_H
struct render_model_definition;
class c_animation_channel;
extern "C" void vehicle_impulse_channel_node_preprocess_callback(long object_index, const render_model_definition *model,
	c_animation_channel *channel, float ratio, float pitch, float weight, void *user_data);
#endif
