#ifndef AI_SQUAD_ITERATOR_TYPES_H
#define AI_SQUAD_ITERATOR_TYPES_H

struct squad_group_datum;

struct squad_group_squad_iterator
{
    squad_group_datum *squad_group;
    squad_group_datum *root_squad_group;
    long squad_index;
    long next_squad_index;
    long previous_squad_index;
};

struct ai_index_squad_iterator
{
    squad_group_squad_iterator squad_group_iterator;
    long squad_group_index;
    long squad_index;
    long next_squad_index;
    short task_index;
    short objective_index;
    bool objective_squads_started;
};

struct squad_actor_iterator
{
    long squad_index;
    long index;
    long next_index;
    bool active_only;
};

struct ai_index_actor_iterator
{
    long actor_index;
    bool single;
    bool done;
    ai_index_squad_iterator squad_iterator;
    squad_actor_iterator actor_iterator;
};

#if defined(_M_PPCBE) || defined(_XBOX)
typedef char squad_group_squad_iterator_size[(sizeof(void *) != 4 || sizeof(squad_group_squad_iterator) == 0x14) ? 1 : -1];
typedef char ai_index_squad_iterator_size[(sizeof(void *) != 4 || sizeof(ai_index_squad_iterator) == 0x28) ? 1 : -1];
typedef char squad_actor_iterator_size[(sizeof(void *) != 4 || sizeof(squad_actor_iterator) == 0x10) ? 1 : -1];
typedef char ai_index_actor_iterator_size[(sizeof(void *) != 4 || sizeof(ai_index_actor_iterator) == 0x40) ? 1 : -1];
#endif

#endif // AI_SQUAD_ITERATOR_TYPES_H
