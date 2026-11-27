#ifndef AI_ACTOR_ITERATOR_NOALIAS_H
#define AI_ACTOR_ITERATOR_NOALIAS_H

struct actor_datum;
struct actor_iterator;

// match: 0x55ca40: 0.0f stays in fr29 across calls; removal reloads per fcmpu, renumbers fr28/r26, swaps __savefpr_28 for stfd/lfd.
__declspec(noalias) actor_datum *actor_iterator_next(actor_iterator *iterator);

#endif // AI_ACTOR_ITERATOR_NOALIAS_H
