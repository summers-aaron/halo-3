#ifndef INCLUDE_PHYS_CONSTRAINT_LISTENER_H
#define INCLUDE_PHYS_CONSTRAINT_LISTENER_H
class phys_constraint;
class phys_constraint_listener {
public:
    virtual ~phys_constraint_listener() {}
    virtual void constraint_added_callback(phys_constraint *constraint) {}
    virtual void constraint_removed_callback(phys_constraint *constraint) {}
};
#endif
