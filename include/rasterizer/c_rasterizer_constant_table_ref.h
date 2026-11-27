#ifndef RASTERIZER_C_RASTERIZER_CONSTANT_TABLE_REF_H
#define RASTERIZER_C_RASTERIZER_CONSTANT_TABLE_REF_H

struct s_entry_table;
class c_rasterizer_constant_desc;

class c_rasterizer_constant_table_ref
{
public:
    s_entry_table *table;
    void set_constants(const c_rasterizer_constant_desc *constant_desc, const void *constants) const;
};

typedef char rasterizer_constant_table_ref_size[(sizeof(void *) != 4 || sizeof(c_rasterizer_constant_table_ref) == 4) ? 1 : -1];

#endif // RASTERIZER_C_RASTERIZER_CONSTANT_TABLE_REF_H
