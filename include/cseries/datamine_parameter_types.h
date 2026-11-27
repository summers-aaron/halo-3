#ifndef CSERIES_DATAMINE_PARAMETER_TYPES_H
#define CSERIES_DATAMINE_PARAMETER_TYPES_H

// March c_datamine::fill_and_get_parameter copies a 32-byte name, then stores
// the parameter type at +32. add_long supplies the 40-byte integer form.
struct s_datamine_parameter
{
    char name[32];
    long type;
    long value;
};

typedef char datamine_parameter_size[(sizeof(s_datamine_parameter) == 40) ? 1 : -1];

#endif
