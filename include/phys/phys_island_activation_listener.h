#ifndef INCLUDE_PHYS_ISLAND_ACTIVATION_LISTENER_H
#define INCLUDE_PHYS_ISLAND_ACTIVATION_LISTENER_H
class phys_island;
class phys_island_activation_listener {
public:
    virtual ~phys_island_activation_listener() {}
    virtual void on_island_activated(phys_island *island) = 0;
    virtual void on_island_deactivated(phys_island *island) = 0;
};
#endif
