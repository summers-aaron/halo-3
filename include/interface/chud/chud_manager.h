#ifndef INCLUDE_INTERFACE_CHUD_CHUD_MANAGER_H
#define INCLUDE_INTERFACE_CHUD_CHUD_MANAGER_H

class c_chud_persistent_user_data;
class c_chud_manager
{
public:
    static void initialize();
    static __declspec(thread) c_chud_persistent_user_data *x_persistent_user_data;
};


// March 2007 TLS slot 0x831df6dc.


#endif // INCLUDE_INTERFACE_CHUD_CHUD_MANAGER_H
