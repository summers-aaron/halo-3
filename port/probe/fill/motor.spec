# motor_programs.cpp: the 16 c_motor_program_definition objects g_program_definition casts (0x8279dd18..), 0x48 each
# (16 callbacks, tasks, short task_count) followed by unrelated pooled constants up to the next one; and the motor task
# definitions / task pointer arrays / s_action_definition records they and g_action_definitions point into (all words).
datum retail_79dd18 0x79dd18 p16 p h2 a2
datum retail_79dd68 0x79dd68 p16 p h2 a4
datum retail_79ddc0 0x79ddc0 p16 p h2
datum retail_79de08 0x79de08 p16 p h2
datum retail_79de50 0x79de50 p16 p h2 a4
datum retail_79dea8 0x79dea8 p16 p h2 a4
datum retail_79df00 0x79df00 p16 p h2
datum retail_79df48 0x79df48 p16 p h2 a4
datum retail_79dfa0 0x79dfa0 p16 p h2 a4
datum retail_79dff8 0x79dff8 p16 p h2
datum retail_79e040 0x79e040 p16 p h2 a4
datum retail_79e098 0x79e098 p16 p h2
datum retail_79e0e0 0x79e0e0 p16 p h2
datum retail_79e128 0x79e128 p16 p h2
datum retail_79e170 0x79e170 p16 p h2
datum retail_79e1b8 0x79e1b8 p16 p h2
datum retail_73bb68 0x73bb68 w p2 w p4 w p2 w p4 w p2 w p4 w p2 w p4 w p2 w p4 w p2 w p4 w p2 w p4 w p2 w p4 w p2 w p4 w p2 w p4 w p2 w p4 w p2 w p4 w p2 w p4 w p2 w p4 w p2 w p4 w p2 w p4 w p2 w p4 w p2 w p4 w p2 w p4 w p2 w p4 w p2 w p4 w p2 w p4 w p2 w p7 w p2 w p6 w p2 w p4 w p2 w p4 w p2 w p4 w p2 w p9 w p2 w p4 w p2 w p4 w p2 w p4 w p2 w p4 w p2 w p4 w p2 w p4 w p2 w p4 w6 p12 w p2 w p12 w p2 w p4 w12 p3 w p2 w p4 w p2 w p16 w p2 w p4 w p2 w p13 w p2 w p4 w p2 w p4 w p2 w p4 w p2 w p4 w p2 w p14 w p2 w p16 w p2 w p4 w p2 w p4 w p2 w p14 w p2 w p4 w p2 w p4 w p2 w p4 w p2 w p4 w p2 w p10 w p2 w p4 w p2 w p16 w5 p2 w p4 w p2 w p4 w p2 w p4 w p2 w p4 w p2 w p4 w p2 w p4 w p2 w p4 w p2 w p4 w p2 w p4 w p2 w p4 w p2 w p318
datum g_action_definitions 0x73cc38 p84
# GAME-CHANGES F52: the tables' static submit functions, through the probe units' external wrappers
name 0x3825a0 ?probe_ground_bunker_close_submit@@YA?AW4e_motor_result@@JPBVc_motor_request@@@Z
name 0x3826f8 ?probe_ground_bunker_open_submit@@YA?AW4e_motor_result@@JPBVc_motor_request@@@Z
name 0x382850 ?probe_ground_animation_task_submit@@YA?AW4e_motor_result@@JPBVc_motor_request@@@Z
name 0x38bb18 ?probe_perch_bunker_close_submit@@YA?AW4e_motor_result@@JPBVc_motor_request@@@Z
name 0x38bc88 ?probe_perch_bunker_open_submit@@YA?AW4e_motor_result@@JPBVc_motor_request@@@Z
name 0x3954f8 ?probe_common_animation_task_submit@@YA?AW4e_motor_result@@JPBVc_motor_request@@@Z
# The move slot (+0x14) of 0x79e040 and 0x79e1b8: retail's folded `li r3, -1; blr` (0x1c1258). retail-names.tsv
# names it after __thiscall members of the fold, which pop their argument; the slot is called cdecl
# (port/probe/extra/motor_program_move_none.cpp).
name 0x1c1258 ?probe_motor_program_move_none@@YA?AW4e_motor_program@@JPAVc_motor_request@@@Z
