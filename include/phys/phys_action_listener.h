#ifndef INCLUDE_PHYS_ACTION_LISTENER_H
#define INCLUDE_PHYS_ACTION_LISTENER_H
class phys_action;
class phys_action_listener {
public:
    virtual ~phys_action_listener() {}
    virtual void on_action_added(phys_action *action) {}
    virtual void on_action_removed(phys_action *action) {}
};
#endif
