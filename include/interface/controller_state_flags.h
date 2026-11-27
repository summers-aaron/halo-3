#ifndef INTERFACE_CONTROLLER_STATE_FLAGS_H
#define INTERFACE_CONTROLLER_STATE_FLAGS_H

enum e_controller_state_flags
{
    _controller_state_attached = 0,
    _controller_state_voice_device_connected = 1,
    _controller_state_unsigned_in_user = 2,
    _controller_state_storage_device_selected = 3,
    _controller_state_privilege_communications = 5,
    _controller_state_privilege_communications_friends_only = 6,
    _controller_state_privilege_profile_viewing = 7,
    _controller_state_privilege_profile_viewing_friends_only = 8,
    _controller_state_privilege_purchase_content = 9,
    _controller_state_privilege_presence = 10,
    _controller_state_privilege_presence_friends_only = 11
};

#endif // INTERFACE_CONTROLLER_STATE_FLAGS_H
