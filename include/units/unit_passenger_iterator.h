#ifndef UNITS_UNIT_PASSENGER_ITERATOR_H
#define UNITS_UNIT_PASSENGER_ITERATOR_H

struct c_multi_unit_passenger_iterator {
    union {
        long m_root_unit_index;
        long root_unit_index;
    };
    union {
        long m_unit_index;
        long unit_index;
    };
    union {
        long m_next_object_index;
        long next_object_index;
    };
    union {
        long m_parent_index;
        long parent_index;
    };
    union {
        long m_passenger_index;
        long passenger_index;
    };
    union {
        short m_seat_index;
        short seat_index;
    };

    void begin(long unit_index);
    bool next();
};

typedef char unit_pass_iter_size[(sizeof(c_multi_unit_passenger_iterator) == 0x18) ? 1 : -1];

#endif // UNITS_UNIT_PASSENGER_ITERATOR_H
