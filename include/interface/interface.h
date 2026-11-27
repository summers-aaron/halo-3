#ifndef INTERFACE_INTERFACE_H
#define INTERFACE_INTERFACE_H

// Functions defined in tu/interfacelibxbox/interface.cpp that other units call.

void interface_splitscreen_render(void);

void interface_initialize(void);
void interface_dispose(void);
void interface_initialize_for_new_map(void);
void interface_dispose_from_old_map(void);

#endif // INTERFACE_INTERFACE_H
