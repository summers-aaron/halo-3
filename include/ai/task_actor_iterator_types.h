#ifndef AI_TASK_ACTOR_ITERATOR_TYPES_H
#define AI_TASK_ACTOR_ITERATOR_TYPES_H

struct task_actor_iterator {
 long actor_index;
 long next_actor_index;
 long squad_index;
 long next_squad_index;
 short objective_index;
 short task_index;
 bool done;
};

#endif // AI_TASK_ACTOR_ITERATOR_TYPES_H
