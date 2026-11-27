#ifndef SIMULATION_SIMULATION_ABORT_H
#define SIMULATION_SIMULATION_ABORT_H

enum e_simulation_abort_reason
{
    _simulation_abort_reason_exiting_in_game = 0,
    _simulation_abort_reason_leaving_match = 1,
    _simulation_abort_reason_2 = 2,
    _simulation_abort_reason_fatal_error = 5,
    _simulation_abort_reason_connection_lost = 6,
    _simulation_abort_reason_7 = 7,
    _simulation_abort_reason_film_request_pending = 9
};

void simulation_abort_immediate(e_simulation_abort_reason reason);

#endif // SIMULATION_SIMULATION_ABORT_H
