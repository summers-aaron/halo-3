#ifndef NETWORKING_NETWORK_OBSERVER_OWNER_TYPES_H
#define NETWORKING_NETWORK_OBSERVER_OWNER_TYPES_H

enum e_network_observer_owner
{
    _network_observer_owner_none = -1,

    _network_observer_owner_squad_one = 0,
    _network_observer_owner_squad_two,
    _network_observer_owner_group,
    _network_observer_owner_simulation,

    k_network_observer_owner_count
};

#endif // NETWORKING_NETWORK_OBSERVER_OWNER_TYPES_H
