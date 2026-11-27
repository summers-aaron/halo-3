#ifndef OBJECTS_DAMAGE_SECTION_RESPONSE_H
#define OBJECTS_DAMAGE_SECTION_RESPONSE_H

struct s_model_damage_info;
struct s_damage_owner;
struct s_damage_response_data;

extern "C" void damage_handle_section_response(const s_model_damage_info *model_damage_info, long object_index,
    const s_damage_owner *owner, unsigned char reporting_info, long section_index, long response_index,
    s_damage_response_data *response);

#endif
