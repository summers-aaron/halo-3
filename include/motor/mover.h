#ifndef MOTOR_MOVER_H
#define MOTOR_MOVER_H

// Functions defined in tu/objectslibxbox/mover.cpp that other units call.

bool mover_is_in_entering_state(long object_index);

void mover_move(long object_index);
void mover_delete(long object_index);

#endif // MOTOR_MOVER_H
