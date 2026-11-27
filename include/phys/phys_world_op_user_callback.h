#ifndef INCLUDE_PHYS_WORLD_OP_USER_CALLBACK_H
#define INCLUDE_PHYS_WORLD_OP_USER_CALLBACK_H
#include "phys/phys_object.h"
class phys_world_op_user_callback : public phys_ref_object {
public:
    virtual void world_op_user_callback(unsigned long action_user_data) = 0;
};
#endif
