#ifndef INTERFACE_CHUD_CHUD_MOTION_SENSOR_H
#define INTERFACE_CHUD_CHUD_MOTION_SENSOR_H

// Functions defined in tu/interfacelibxbox/chud_motion_sensor.cpp that other units call.

bool object_velocity_exceeds_threshold(long object_index);

#include "math/real_math_types.h"

class c_chud_persistent_user_data;
struct s_widget_geometry_quad_fancy;
extern "C" void chud_persistent_user_data_initialize(c_chud_persistent_user_data *data, long user_index);

class c_chud_motion_sensor {
    friend void ::chud_persistent_user_data_initialize(c_chud_persistent_user_data *data, long user_index);
    enum e_blip_type { _blip_none, _blip_self, _blip_friendly, _blip_enemy, _blip_empty };
    e_blip_type object_get_blip_type(long object_index);
    struct s_tracked_entity { long object_index; float age; long delay; };
    struct s_motion_sensor_blip { union { float x; long x_bits; }; union { float y; long y_bits; }; long : 32;
        unsigned char intensity; unsigned char size; unsigned char shape; };
    s_tracked_entity m_tracked_entities[20];
    struct s_motion_sensor_contact { int active; s_motion_sensor_blip blips[16]; };
    s_motion_sensor_contact m_contacts[10];
    s_motion_sensor_blip m_custom_blips[20];
    float m_navpoint_data[20];
    unsigned long m_tracking_flags;
    long m_custom_blip_count;
    long m_user_index;
    __forceinline bool get_sensor_origin(real_point3d *origin);
    __forceinline void invalidate_tracked_entity(long entity_index) { m_tracked_entities[entity_index].object_index = -1; }
    void update_tracked_entities(bool);
    void update_tracked_entity(long entity_index, const real_point3d *position);
    bool object_should_be_tracked(long object_index, const real_point3d *position);
    long new_tracked_entity(long object_index);
    long m_valid_index;
    float m_tracking_distance;
    float m_sensor_range;
    float m_sensor_scale;
    enum e_blip_size { _blip_size_none };
    __forceinline e_blip_size object_get_blip_size(long object_index);
    __forceinline bool object_is_moving(long object_index);
    bool entity_create_blip(long entity_index, s_motion_sensor_blip *blip, real_point3d *origin, real_point2d *override_position,
        float forward_x, float forward_y);
    bool create_custom_blip(const real_point3d *origin, s_motion_sensor_blip *blip, real_point3d *point, float forward_x, float forward_y);
    void build_new_history(class c_chud_navpoint_manager *chud_navpoint_manager);
    bool tracking_flag_test(long bit) const { return (m_tracking_flags >> bit) & 1; }
    __forceinline void set_radar_distance(float distance) { m_sensor_range = distance; m_sensor_scale = distance > 0.0f ? 1.0f / distance : 0.0f; }
    __forceinline void set_tracking_distance(float distance) { m_tracking_distance = distance; }
    void update_user_properties();
public:
    __forceinline void reset() { invalidate(); }
    __forceinline void update(bool update_entities, c_chud_navpoint_manager *navpoint_manager)
    {
        update_user_properties();
        update_tracked_entities(update_entities);
        build_new_history(navpoint_manager);
    }
    void draw();
    bool enemies_nearby();
    bool slow_moving_boardable_enemy_vehicles_nearby();
private:
    __forceinline void invalidate()
    {
        for (long i = 0; i < 20; ++i)
            invalidate_tracked_entity(i);
        for (long i = 0; i < 10; ++i)
            m_contacts[i].active = 0;
        m_valid_index = 0;
    }
    long history_build_geometry(long history_index, float fraction, s_widget_geometry_quad_fancy *quad, long);
    void blip_build_geometry(s_widget_geometry_quad_fancy *quad, s_motion_sensor_blip *blip, unsigned long alpha, float forward_x,
        float forward_y, real_point2d *position, float scale);
};

typedef char chud_motion_sensor_size_check[(sizeof(c_chud_motion_sensor) == 0xcc4) ? 1 : -1];

#endif // INTERFACE_CHUD_CHUD_MOTION_SENSOR_H
