#ifndef TAG_FILES_TAG_VERTEX_BUFFERS_H
#define TAG_FILES_TAG_VERTEX_BUFFERS_H

// Functions defined in tu/blamlibxbox/tag_vertex_buffers.cpp.

struct s_tag_interop;
class c_tag_interop_verifier;

bool tag_d3d_vertex_buffer_fixup(s_tag_interop *value, c_tag_interop_verifier *verifier);
bool tag_d3d_index_buffer_fixup(s_tag_interop *value, c_tag_interop_verifier *verifier);

#endif // TAG_FILES_TAG_VERTEX_BUFFERS_H
