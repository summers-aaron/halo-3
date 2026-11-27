// hkbase/hkBaseSystem.obj: from the records 0x260b00 0x60a810 0x60a850 0x60a860 0x60a8b8 0x60a908 0x60aa80 0x60abc8 0x60ad20 0x60adf0 0x60ae80 0x60aec8 0x60aee0 0x60b238 0x60b280 0x60b290 0x60b2a0 0x60b2d0 0x60b360 0x60b688 0x60b7a0 0x60b828 0x60b840 0x60b858 0x60b888 0x60b890 0x60b930 0x60ba30 0x60ba90 0x60bc00 0x60bc20 0x60bc70 0x60bc78 0x60bda0 0x60bdb0 0x60be20 0x60be98
// @flags /TP /O2
// @address 0x60a810 ?init_thread_memory@phys_system@@YA?AW4phys_result@@PAVphys_thread_memory@@@Z
// @address 0x60a850 ?system_is_initialized@phys_system@@YA?AVphys_bool@@XZ
// @address 0x60a860 ?release_thread_resources@phys_system@@YA?AW4phys_result@@XZ
// @address 0x60a8b8 ?phys_dummy_singleton_create@@YAPAVphys_ref_object@@XZ
// @address 0x60ad20 ?quit_system@phys_system@@YA?AW4phys_result@@XZ
// @address 0x60ae80 ?set_stream_mark@phys_seekable_reader@@UAA?AW4phys_result@@H@Z
// @address 0x60aec8 ?rewind_to_mark@phys_seekable_reader@@UAA?AW4phys_result@@XZ
// @address 0x60b238 ?read@phys_stdio_reader@@UAAHPAXH@Z
// @address 0x60b280 ?is_stream_ok@phys_stdio_reader@@UBA?AVphys_bool@@XZ
// @address 0x60b2a0 ?seek@phys_stdio_reader@@UAA?AW4phys_result@@HW4seek_whence@phys_reader@@@Z
// @address 0x60b2d0 ?tell@phys_stdio_reader@@UBAHXZ
// @address 0x60b688 ?open_reader@phys_default_streambuf_factory@@UAAPAVphys_reader@@PBD@Z
// @address 0x60b828 ?flush@phys_stdio_writer@@UAAXXZ
// @address 0x60b840 ?is_stream_ok@phys_stdio_writer@@UBA?AVphys_bool@@XZ
// @address 0x60b858 ?seek@phys_stdio_writer@@UAA?AW4phys_result@@HW4seek_whence@phys_writer@@@Z
// @address 0x60b888 ?tell@phys_stdio_writer@@UBAHXZ
// @address 0x60b930 ?open_writer@phys_default_streambuf_factory@@UAAPAVphys_writer@@PBD@Z
// @address 0x60bc00 ?error_set_enabled@phys_default_error@@UAAXHVphys_bool@@@Z
// @address 0x60bc20 ?is_error_enabled@phys_default_error@@UAA?AVphys_bool@@H@Z
// @address 0x60bc70 ?enable_all_reports@phys_default_error@@UAAXXZ
// @address 0x60bc78 ?message@phys_default_error@@UAAHW4error_message@phys_error@@HPBD1H@Z
#include "cseries/crt_xdk.h"
#include "physics/havok_halo_external.h"
#include "phys/phys_math.h"
#include "phys/phys_memory.h"
#include "phys/phys_system.h"
#include "phys/phys_error.h"
#include "phys/phys_ptr_map_base.h"
#include "phys/phys_out_stream.h"
#include "phys/phys_buffered_writer.h"
#include "phys/phys_buffered_reader.h"
#include "phys/phys_stack_tracer.h"
#include "phys/phys_streambuf_factory.h"
#include "phys/phys_base_types.h"
#include "phys/phys_string.h"
#include "phys/phys_monitor_stream.h"
#include "phys/phys_socket.h"

// @address 0x60b890 ??_Gphys_stdio_writer@@UAAPAXI@Z
// @address 0x60aa80 ?init@phys_system@@YA?AW4phys_result@@PAVphys_memory@@PAVphys_thread_memory@@P6AXPBDPAX@Z3@Z
// @address 0x60bda0 ?end_section@phys_default_error@@UAAXXZ
// @address 0x60b7a0 ?write@phys_stdio_writer@@UAAHPBXH@Z
// @address 0x60aee0 ??0phys_stdio_reader@@QAA@PBD@Z
// @address 0x60bdb0 ?begin_section@phys_default_error@@UAAXHPBD@Z
// @address 0x60ba30 ??0phys_default_error@@QAA@P6AXPBDPAX@Z1@Z
// @address 0x260b00 ??_Gphys_dummy_singleton@@UAAPAXI@Z
// @address 0x5bec98 ?force_linkage@phys_dummy_singleton@@UAAXXZ
// @address 0x60b290 ?mark_supported@phys_seekable_reader@@UBA?AVphys_bool@@XZ
// @address 0x60b290 ?seek_tell_supported@phys_seekable_reader@@UBA?AVphys_bool@@XZ
// @address 0x60be98 ??_Gphys_default_error@@UAAPAXI@Z

class phys_default_error : public phys_error {
  phys_ptr_map_base<unsigned long> m_disabled_ids;
  phys_array<int> m_section_id_list;  // following the virtual table and header
public:
  phys_default_error(phys_error_report_func error_report_fn, void *error_report_context);
protected:
  void show_error_message(const char *what, int id, const char *desc, const char *file, int line, phys_bool with_stack_trace);
public:
  phys_error_report_func m_error_func;
  void *m_error_ctx;
  virtual int message(phys_error::error_message type, int id, const char *description, const char *file, int line);
  virtual void error_set_enabled(int id, phys_bool enabled);
  virtual phys_bool is_error_enabled(int id);
  virtual void enable_all_reports();
  virtual void end_section();
  virtual void begin_section(int begin_section_id, const char *section_name);
};

class phys_seekable_reader : public phys_reader {
public:
    phys_seekable_reader() : m_mark_pos(-1) {}
    int m_mark_pos;
    virtual phys_bool mark_supported() const;
    virtual phys_result set_stream_mark(int size);
    virtual phys_result rewind_to_mark();
    virtual phys_bool seek_tell_supported() const;
};
class phys_stdio_reader : public phys_seekable_reader {
public:
    phys_stdio_reader(const char *name);
    FILE *m_handle;
    phys_bool m_is_ok;
    virtual int read(void *destination, int size);
    virtual phys_bool is_stream_ok() const;
    virtual phys_result seek(int offset, seek_whence seek_origin);
    virtual int tell() const;
};

class phys_stdio_writer : public phys_writer {
public:
    __forceinline phys_stdio_writer(const char *name);
    void open(const char *file_name_in, const char *mode);
    virtual ~phys_stdio_writer()
    {
        if (m_handle && m_is_owned)
            fclose(m_handle);
        m_handle = 0;
    }
    virtual int write(const void *buffer, int size);
    static __forceinline void operator delete(void *object)
    {
        phys_thread_memory::get_singleton_instance().free_chunk(
            object, static_cast<phys_ref_object *>(object)->m_mem_size_and_flags, PHYS_MEM_STREAM);
    }
    static __forceinline void *operator new(size_t, void *address) { return address; }
    FILE *m_handle;
    phys_bool m_is_owned;
    virtual void flush();
    virtual phys_bool is_stream_ok() const;
    virtual phys_result seek(int offset, seek_whence seek_origin);
    virtual int tell() const;
};

class phys_default_streambuf_factory : public phys_streambuf_factory {
public:
    virtual ~phys_default_streambuf_factory() {}
    virtual phys_reader *open_reader(const char *name);
    virtual phys_writer *open_writer(const char *name);
};

class phys_dummy_singleton : public phys_ref_object {
public:
    PHYS_DECLARE_ALLOCATOR(PHYS_MEM_SINGLETON);
    static __forceinline phys_dummy_singleton &get_singleton_instance() { return *phys_dummy_singleton_instance; }
    phys_dummy_singleton() { m_object_ref_count = 1; }
    virtual void force_linkage() {}
};

// @address 0x7ce564 ?phys_system_init_verbose@@3Vphys_bool@@A
phys_bool phys_system_init_verbose;
// @address 0x7ce565 ?phys_system_is_initialized@@3Vphys_bool@@A
phys_bool phys_system_is_initialized;
// @address 0x7ce580 ?phys_singleton_init_list@@3PAUphys_singleton_init_node@@A
phys_singleton_init_node *phys_singleton_init_list;
// @address 0x7ce584 ?s_instance@?$phys_singleton@Vphys_streambuf_factory@@@@2PAVphys_streambuf_factory@@A
template<>
phys_streambuf_factory *phys_singleton<phys_streambuf_factory>::s_instance = 0;
extern "C" {
// @address 0x7ce560 phys_dummy_singleton_instance
phys_dummy_singleton *phys_dummy_singleton_instance;
// @address 0x14900 k_phys_error_stack_trace_header
const char k_phys_error_stack_trace_header[] = "Stack trace is:\n";
// @address 0x14914 k_phys_error_location_separator
const char k_phys_error_location_separator[] = "): [";
// @address 0x1491c k_phys_error_severity_separator
const char k_phys_error_severity_separator[] = "] ";
// @address 0x14920 k_phys_error_description_separator
const char k_phys_error_description_separator[] = " : '";
// @address 0x14928 k_phys_error_line_suffix
const char k_phys_error_line_suffix[] = "'\n";
// @address 0x1492c k_phys_error_id_format
const char k_phys_error_id_format[] = "0x%x";
}

__forceinline phys_stdio_writer::phys_stdio_writer(const char *name) : m_is_owned(true)
{
    if (name) open(name, "wb");
}

static __forceinline void *alloc_stream_buf(int size, phys_memory_class mem_class)
{
    phys_ref_object *object =
        static_cast<phys_ref_object *>((&phys_thread_memory::get_singleton_instance())->allocate_chunk(size, mem_class));
    object->m_mem_size_and_flags = static_cast<unsigned short>(size);
    return object;
}

// 0x60a810
phys_result phys_system::init_thread_memory(phys_thread_memory *memory)
{
    phys_thread_memory::init();
    phys_thread_memory::replace_singleton_instance(memory);
    phys_monitor_stream::init();
    return PHYS_SUCCESS;
}

// 0x60a850
phys_bool phys_system::system_is_initialized()
{
    return phys_system_is_initialized;
}

// 0x60a860
phys_result phys_system::release_thread_resources()
{
    phys_monitor_stream::get_singleton_instance().quit_system();
    phys_thread_memory &memory = phys_thread_memory::get_singleton_instance();
    memory.release_cached_memory();
    phys_thread_memory::replace_singleton_instance(0);
    phys_thread_memory::quit_system();
    return PHYS_SUCCESS;
}

// 0x60a8b8
phys_ref_object *phys_dummy_singleton_create()
{
    phys_dummy_singleton *dummy_singleton =
        static_cast<phys_dummy_singleton *>(alloc_stream_buf(sizeof(phys_dummy_singleton), PHYS_MEM_SINGLETON));
    return new (dummy_singleton) phys_dummy_singleton;
}

phys_singleton_init_node phys_singleton_reg_dummy = phys_singleton_init_node(
    phys_dummy_singleton_create, static_cast<void **>(static_cast<void *>(&phys_dummy_singleton_instance)));

// @address 0x60a908 ?init_singletons@phys_system@@YAXXZ
void phys_system::init_singletons()
{
  phys_singleton_init_node *n;
  phys_singleton_init_node **link;
  phys_ref_object *r;
  int i;
  phys_array<phys_singleton_init_node *> pending_init_nodes;

  link = (phys_singleton_init_node **)&phys_singleton_init_list;
  n = *link;
  if (n != 0) {
    do {
      if (*n->m_value == 0) {
        r = n->m_singleton_create_fn();
        if (r != 0) {
          *n->m_value = r;
          link = &n->m_next;
          n = n->m_next;
          continue;
        }
        if (pending_init_nodes.m_size == (pending_init_nodes.m_capacity_and_flags & 0x3fffffff)) {
          phys_array_util::grow_array_storage(&pending_init_nodes, (int)sizeof(pending_init_nodes.m_data[0]));
        }
        pending_init_nodes.m_data[pending_init_nodes.m_size] = n;
        pending_init_nodes.m_size++;
        n = n->m_next;
        (*link)->m_next = 0;
        *link = n;
      } else {
        link = &n->m_next;
        n = n->m_next;
      }
    } while (n != 0);
    while (pending_init_nodes.m_size != 0) {
      for (i = pending_init_nodes.m_size - 1; i >= 0; i--) {
          n = pending_init_nodes.m_data[i];
          r = n->m_singleton_create_fn();
          if (r != 0) {
            *n->m_value = r;
            *link = n;
            link = &n->m_next;
            pending_init_nodes.m_size--;
            pending_init_nodes.m_data[i] = pending_init_nodes.m_data[pending_init_nodes.m_size];
          }
      }
    }
  }

}

static __forceinline void init_thread_inline(phys_thread_memory *system_thread_memory)
{
    phys_thread_memory::init();
    phys_thread_memory::replace_singleton_instance(system_thread_memory);
    phys_monitor_stream::init();
}
// 0x60aa80
phys_result phys_system::init(
    phys_memory *memory_manager, phys_thread_memory *system_thread_memory, phys_error_report_func error_report_fn, void *error_report_context)
{
    if (phys_system_is_initialized == false) {
        if (memory_manager) {
            phys_memory::replace_singleton_instance(memory_manager);
            if (system_thread_memory == 0) {
                system_thread_memory = new phys_thread_memory(memory_manager, 16);
                init_thread_inline(system_thread_memory);
                system_thread_memory->object_remove_ref();
            } else {
                init_thread_inline(system_thread_memory);
            }
        } else {
            halo_external_breakpoint();
            return PHYS_FAILURE;
        }
        phys_singleton<phys_streambuf_factory>::replace_singleton_instance(
            new (alloc_stream_buf(sizeof(phys_default_streambuf_factory), PHYS_MEM_SINGLETON)) phys_default_streambuf_factory);
        phys_singleton<phys_error>::replace_singleton_instance(
            new (alloc_stream_buf(sizeof(phys_default_error), PHYS_MEM_SINGLETON))
                phys_default_error(error_report_fn, error_report_context));
        phys_system::init_singletons();
        phys_dummy_singleton::get_singleton_instance().force_linkage();
        phys_system_is_initialized = true;
    }
    return PHYS_SUCCESS;
}

// @address 0x60abc8 ?quit_singletons@phys_system@@YAXXZ
void phys_system::quit_singletons()
{
    phys_inline_array<phys_singleton_init_node *, 128> a;
    for (phys_singleton_init_node *n = phys_singleton_init_list; n != 0; n = n->m_next) a.array_push_back(n);
    for (int i = a.m_size - 1; i >= 0; i--) {
        phys_ref_object *o = static_cast<phys_ref_object *>(*a[i]->m_value);
        if (o->m_mem_size_and_flags != 0) {
            short r = o->m_object_ref_count - 1;
            o->m_object_ref_count = r;
            if (r == 0) {
                if (halo_managed_memory_address((const phys_ref_object *)o)) o->m_object_ref_count = 1;
                else delete o;
            }
        }
        *a[i]->m_value = 0;
    }
}

namespace {
static __forceinline phys_result phys_system_clear_thread_resources_inline()
{
    phys_monitor_stream::get_singleton_instance().quit_system();
    phys_thread_memory &memory = phys_thread_memory::get_singleton_instance();
    memory.release_cached_memory();
    phys_thread_memory::replace_singleton_instance(0);
    phys_thread_memory::quit_system();
    return PHYS_SUCCESS;
}
}

// 0x60ad20
phys_result phys_system::quit_system()
{
    if (phys_system_is_initialized == true) {
        phys_system::quit_singletons();
        if (phys_socket::s_platformNetInitialized != false && phys_socket::s_platformNetQuit != 0) {
            phys_socket::s_platformNetQuit();
            phys_socket::s_platformNetInitialized = 0;
        }
        phys_singleton<phys_error>::replace_singleton_instance(0);
        phys_singleton<phys_streambuf_factory>::replace_singleton_instance(0);
        phys_system_clear_thread_resources_inline();
        phys_memory::replace_singleton_instance(0);
        phys_system_is_initialized = false;
    }
    return PHYS_SUCCESS;
}

// 0x60ae80
phys_result phys_seekable_reader::set_stream_mark(int size)
{
    m_mark_pos = tell();
    return m_mark_pos == -1 ? PHYS_FAILURE : PHYS_SUCCESS;
}

// 0x60aec8
phys_result phys_seekable_reader::rewind_to_mark()
{
    return seek(m_mark_pos, SEEK_FROM_START);
}

phys_stdio_reader::phys_stdio_reader(const char *name) : m_is_ok(true)
{
    phys_string path(name);
    path = path.replace("/", "\\", phys_string::STRING_REPLACE_ALL);
    int index = 0;
    while (path[index] == '.' || path[index] == '\\') ++index;
    if (phys_string::str_chr(path.string_c_str(), ':') == 0) {
        path = phys_string("D:") + phys_string(path[index] == '\\' ? "" : "\\") + (path.string_c_str() + index);
    }
    m_handle = fopen(path.string_c_str(), "rb");
    m_is_ok = m_handle != 0;
}

// @address 0x60b360 ?open@phys_stdio_writer@@QAAXPBD0@Z
void phys_stdio_writer::open(const char *file_name_in, const char *mode)
{
    phys_string path(file_name_in);
    path = path.replace("/", "\\", phys_string::STRING_REPLACE_ALL);
    int index = 0;
    while (path[index] == '.' || path[index] == '\\') ++index;
    if (phys_string::str_chr(path.string_c_str(), ':') == 0) {
        path = phys_string("D:") + phys_string(path[index] == '\\' ? "" : "\\") + (path.string_c_str() + index);
    }
    m_handle = fopen(path.string_c_str(), mode);
}

// 0x60b238
int phys_stdio_reader::read(void *destination, int size)
{
    int read_byte_cnt = static_cast<int>(fread(destination, 1, size, m_handle));
    if (read_byte_cnt <= 0) m_is_ok = false;
    return read_byte_cnt;
}

// 0x60b280
phys_bool phys_stdio_reader::is_stream_ok() const
{
    return m_is_ok;
}

// 0x60b290
phys_bool phys_seekable_reader::mark_supported() const
{
    return true;
}

phys_bool phys_seekable_reader::seek_tell_supported() const
{
    return true;
}

// 0x60b2a0
phys_result phys_stdio_reader::seek(int offset, seek_whence seek_origin)
{
    return fseek(m_handle, offset, seek_origin) != 0 ? PHYS_FAILURE : PHYS_SUCCESS;
}

// 0x60b2d0
int phys_stdio_reader::tell() const
{
    return ftell(m_handle);
}

// 0x60b688
phys_reader *phys_default_streambuf_factory::open_reader(const char *name)
{
    phys_stdio_reader *reader =
        new (alloc_stream_buf(sizeof(phys_stdio_reader), PHYS_MEM_STREAM)) phys_stdio_reader(name);
    if (!reader->mark_supported()) {
        phys_reader *buffered =
            new (alloc_stream_buf(sizeof(phys_buffered_reader), PHYS_MEM_STREAM)) phys_buffered_reader(reader, 4096);
        reader->object_remove_ref();
        return buffered;
    } else {
        return reader;
    }
}

// 0x60b7a0
int phys_stdio_writer::write(const void *buffer, int size)
{
    if (m_handle != 0) {
        int n = static_cast<int>(fwrite(buffer, 1, size, m_handle));
        if (n <= 0) {
            if (m_handle != 0 && m_is_owned) fclose(m_handle);
            m_handle = 0;
        }
        return n;
    }
    return 0;
}

// 0x60b828
void phys_stdio_writer::flush()
{
    if (m_handle != 0) fflush(m_handle);
}

// 0x60b840
phys_bool phys_stdio_writer::is_stream_ok() const
{
    return m_handle != 0;
}

// 0x60b858
phys_result phys_stdio_writer::seek(int offset, seek_whence seek_origin)
{
    return fseek(m_handle, offset, seek_origin) != 0 ? PHYS_FAILURE : PHYS_SUCCESS;
}

// 0x60b888
int phys_stdio_writer::tell() const
{
    return ftell(m_handle);
}

// 0x60b930
phys_writer *phys_default_streambuf_factory::open_writer(const char *name)
{
    phys_stdio_writer *writer =
        new (alloc_stream_buf(sizeof(phys_stdio_writer), PHYS_MEM_STREAM)) phys_stdio_writer(name);
    phys_writer *buffered =
        new (alloc_stream_buf(sizeof(phys_buffered_writer), PHYS_MEM_STREAM)) phys_buffered_writer(writer, 4096);
    writer->object_remove_ref();
    return buffered;
}

// 0x60ba30
phys_default_error::phys_default_error(phys_error_report_func error_report_fn, void *error_report_context)
    : m_disabled_ids(), m_section_id_list(), m_error_func(error_report_fn), m_error_ctx(error_report_context)
{
}

// @address 0x60ba90 ?show_error_message@phys_default_error@@IAAXPBDH00HVphys_bool@@@Z
void phys_default_error::show_error_message(
    const char *what, int id, const char *desc, const char *file, int line, phys_bool with_stack_trace)
{
    char buf[16];
    phys_string::sprintf(buf, k_phys_error_id_format, id);
    char msg_buf[512];
    phys_buffered_writer writer(msg_buf, 512, true);
    phys_out_stream os(&writer);
    os << file << '(' << line << k_phys_error_location_separator << buf
       << k_phys_error_severity_separator << what << k_phys_error_description_separator
       << desc << k_phys_error_line_suffix;
    this->m_error_func(msg_buf, this->m_error_ctx);
    if (with_stack_trace) {
        phys_stack_tracer tracer;
        unsigned long trace[20];
        int trace_depth = tracer.get_stack_trace(trace, 20);
        if (trace_depth > 2) {
            this->m_error_func(k_phys_error_stack_trace_header, this->m_error_ctx);
            tracer.dump_stack_trace(trace + 2, trace_depth - 2, this->m_error_func, this->m_error_ctx);
        }
    }
}

// 0x60bc00
void phys_default_error::error_set_enabled(int id, phys_bool enabled)
{
    if (enabled) m_disabled_ids.remove(id);
    else m_disabled_ids.insert(id, 1);
}

// 0x60bc20
phys_bool phys_default_error::is_error_enabled(int id)
{
    return m_disabled_ids.get_with_default(id, 0) == 0;
}

// 0x60bc70
void phys_default_error::enable_all_reports()
{
    m_disabled_ids.clear();
}

// 0x60bc78
int phys_default_error::message(phys_error::error_message type, int id, const char *description, const char *file, int line)
{
    if (id == -1 && m_section_id_list.getSize() != 0)
        id = m_section_id_list.back();
    if (!is_error_enabled(id))
        return 0;
    const char *text = "";
    phys_bool with_stack_trace(false);
    switch (type) {
    case MESSAGE_REPORT: text = "Report"; break;
    case MESSAGE_WARNING: text = "Warning"; break;
    case MESSAGE_ASSERT: text = "Assert"; with_stack_trace = true; break;
    case MESSAGE_ERROR: text = "Error"; with_stack_trace = true; break;
    }
    show_error_message(text, id, description, file, line, with_stack_trace);
    return type == MESSAGE_ASSERT || type == MESSAGE_ERROR;
}

// 0x60bda0
void phys_default_error::end_section()
{
  m_section_id_list.drop_back();
}

// 0x60bdb0
void phys_default_error::begin_section(int begin_section_id, const char *section_name)
{
  m_section_id_list.array_push_back(begin_section_id);
}

// @address 0x60adf0 ?replace_singleton_instance@?$phys_singleton@Vphys_streambuf_factory@@@@SAXPAVphys_streambuf_factory@@@Z
template<>
void phys_singleton<phys_streambuf_factory>::replace_singleton_instance(phys_streambuf_factory *instance)
{
    if (s_instance != 0)
        s_instance->object_remove_ref();
    s_instance = instance;
}

// @address 0x60be20 ??1phys_default_error@@UAA@XZ

