#ifndef GAME_GAME_ENGINE_CANDY_MONITOR_TYPES_H
#define GAME_GAME_ENGINE_CANDY_MONITOR_TYPES_H

#include "game/game_engine_candy_monitor.h"
#include <string.h>
#include <stdlib.h>

static __forceinline void *candy_csmemset(void *buffer, int value, unsigned long size)
{
    return memset(buffer, value, size);
}

class c_candy_monitor {
public:
    long object_index;
    unsigned short flags;
    unsigned char state;
    unsigned char abandoned;
    unsigned char timers[2];
    short abandonment_time;
    __forceinline long get_spawned_object_index() const { return object_index; }
    __forceinline bool disturbed_from_initial_placement() const { return state != 0; }
    void update();
    __forceinline void initialize(long index) {
        object_index = index;
        flags = 0;
        state = 0;
        abandoned = 0;
        candy_csmemset(timers, 0, sizeof(timers));
        if (index != -1)
            abandonment_time = (short)scenario_object_get_abandonment_time_seconds(index);
        else
            abandonment_time = 10;
    }
    __forceinline void initialize(long index) volatile {
        object_index = index;
        flags = 0;
        state = 0;
        abandoned = 0;
        timers[0] = 0;
        timers[1] = 0;
        abandonment_time = 10;
    }
};
class c_candy_spawner {
    friend void game_engine_candy_monitor_delete_entry(int object_name);
    signed char flags;
    long map_variant_index;
    long last_spawned_object_index;
    unsigned short respawn_timer_seconds;
public:
    void initialize(long object_index);
    bool spawn_object(bool force);
    void update();
    __forceinline void respawn_timer_update();
    __forceinline bool respawn_timer_starts_on_disturbance() const { return (_rotl(flags, 30) & 1) != 0; }
    __forceinline bool respawn_timer_starts_on_death() const { return (_rotl(flags, 29) & 1) != 0; }
    __forceinline bool monitoring_something() const { return map_variant_index != -1; }
    __forceinline short get_map_variant_index() const { return (short)map_variant_index; }
    __forceinline short get_respawn_timer_seconds() const { return respawn_timer_seconds; }
    __forceinline void set_flag(long bit, bool enabled) {
        if (enabled) flags |= 1 << bit;
        else flags &= ~(1 << bit);
    }
    __forceinline long get_last_object_spawned_index() const { return last_spawned_object_index; }
    __forceinline void set_last_spawned_object_index(long index) { last_spawned_object_index = index; }
    __forceinline void initialize() {
        flags = 0;
        map_variant_index = -1;
        last_spawned_object_index = -1;
        respawn_timer_seconds = 0;
    }
};
class c_multiplayer_candy_monitor_manager
{
public:
    c_candy_spawner spawners[512];
    c_candy_monitor monitors[512];
    void initialize_for_new_map();
    void dispose_from_old_round();
    void update();
    void detach_object(long object_index);
    void monitor_object(short quota_index, long object_index);
    __forceinline c_candy_spawner *spawner_try_and_get(short quota_index) {
        c_candy_spawner *result = 0;
        for (long index = 0; index < 512; index++) {
            if (spawners[index].get_map_variant_index() == quota_index) {
                result = &spawners[index];
                break;
            }
        }
        return result;
    }
    bool quotas_permit_spawn(short variant_index, bool *bypass_timer);
    void try_to_spawn_new_object(short variant_index, c_candy_spawner *spawner);
    static long get_candy_monitor_object_definition_index(short object_index);
    __forceinline c_candy_monitor *monitor_try_and_get(long object_index) {
        c_candy_monitor *monitor = 0;
        if (object_index != -1) {
            for (long index = 0; index < 512; ++index) {
                if (monitors[index].get_spawned_object_index() == object_index) {
                    monitor = &monitors[index];
                    break;
                } else { continue; }
            }
        }
        return monitor;
    }
};

typedef char candy_monitor_manager_size[(sizeof(c_multiplayer_candy_monitor_manager) == 0x3800) ? 1 : -1];

#endif
