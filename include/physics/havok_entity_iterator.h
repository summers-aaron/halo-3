#ifndef PHYSICS_HAVOK_ENTITY_ITERATOR_H
#define PHYSICS_HAVOK_ENTITY_ITERATOR_H

class phys_world;
class phys_entity;

// March's asserts locate this class in physics/havok_entity_iterator.cpp.
class c_havok_entity_iterator
{
public:
    enum e_iteration_type
    {
        _iterate_active,
        _iterate_inactive,
        _iterate_all
    };

    explicit c_havok_entity_iterator(phys_world *world);
    c_havok_entity_iterator(phys_world *world, e_iteration_type iteration_type);
    void begin();
    bool next();
    phys_entity *get_entity();
    void end();

private:
    bool m_iterating;
    bool m_first_collections_done;
    long m_collection_index;
    long m_entity_index;
    phys_world *m_world;
    e_iteration_type m_iteration_type;
};

typedef char havok_entity_iterator_size[(sizeof(void *) != 4 || ((sizeof(c_havok_entity_iterator) == 0x14) )) ? 1 : -1];

#endif // PHYSICS_HAVOK_ENTITY_ITERATOR_H
