#ifndef INTERFACE_CHUD_CHUD_IMPULSE_MANAGER_H
#define INTERFACE_CHUD_CHUD_IMPULSE_MANAGER_H

struct chud_ammo_impulse { long definition_index; long count; };
struct chud_ammo_impulses { chud_ammo_impulse impulses[3]; long count; };

struct c_chud_impulse_manager
{
    chud_ammo_impulses ammo;
    long grenade_counts[4];

    static c_chud_impulse_manager *get(long user_index);
    void picked_up_grenade(long grenade_index);
    long get_grenade_pickup_count(long grenade_index);
    long get_ammo_pickup_count(long object_index);
};

#endif
