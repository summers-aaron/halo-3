#ifndef TAG_FILES_TAG_RESOURCE_INTERNALS_H
#define TAG_FILES_TAG_RESOURCE_INTERNALS_H

// Functions defined in tu/blamlibxbox/tag_resource_internals.cpp that other units call.

struct s_tag_resource_interop_fixup_setup;
struct s_tag_resource_fixup_setup;
template<typename t_type> class c_basic_buffer;

void tag_resource_fixup_cache_data_references(s_tag_resource_fixup_setup *setup, c_basic_buffer<void> *optional_data);

void tag_resource_fixup_interops(s_tag_resource_interop_fixup_setup *setup);
void tag_resource_teardown_interops(s_tag_resource_interop_fixup_setup *setup);

#endif // TAG_FILES_TAG_RESOURCE_INTERNALS_H
