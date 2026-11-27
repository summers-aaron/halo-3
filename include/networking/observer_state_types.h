#ifndef NETWORKING_OBSERVER_STATE_TYPES_H
#define NETWORKING_OBSERVER_STATE_TYPES_H

enum e_observer_state
{
    _observer_state_none,
    _observer_state_dead,
    _observer_state_idle,
    _observer_state_securing,
    _observer_state_waiting,
    _observer_state_ready,
    _observer_state_connecting,
    _observer_state_connected,
    _observer_state_reconnecting,
    _observer_state_disconnected
};

#endif // NETWORKING_OBSERVER_STATE_TYPES_H
