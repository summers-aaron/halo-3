#ifndef UNITS_UNIT_DIALOGUE_DATUM_TYPES_H
#define UNITS_UNIT_DIALOGUE_DATUM_TYPES_H

struct s_unit_dialogue_datum
{
    // Unit object storage preceding the dialogue fields (dialogue_offset matches unit_datum at 0x3fe).
    char prefix[0x3fe];
    short dialogue_offset;
};

#endif // UNITS_UNIT_DIALOGUE_DATUM_TYPES_H
