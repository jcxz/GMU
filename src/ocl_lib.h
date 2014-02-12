#ifndef OCL_LIB_H
#define OCL_LIB_H

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX               // disable min, max macros from Windows headers

#include "ogl_lib.h"
#include "utils.h"

#include <CL/cl.hpp>
#include <stdexcept>
#include <iostream>
#include <unordered_map>
#include <ostream>
#include <memory>



namespace ocl {

///////////////////////////////////////////////////////////////////////////////
// Error handling

const char *errorToStr(cl_int err);

/** OpenCL exception class */
struct Exception : std::runtime_error
{
  explicit Exception(cl_int err) throw()
    : std::runtime_error(errorToStr(err))
  {
  }

  explicit Exception(const std::string & msg, cl_int err) throw()
    : std::runtime_error(msg + ": " + errorToStr(err))
  {
  }
};


///////////////////////////////////////////////////////////////////////////////
// Device and platform management

/**
 * This function will attempt to determine the device and platform id of the device
 * that is used by OpenGL. This function should be called only after a valid OpenGL
 * context is created.
 *
 * @param device a pointer to cl_device_id variable that will contain the selected device
 * @param platform a pointer to cl_platform_id variable that will contain the selected platform
 *
 * @return true on success, false in case no interoperable device is found.
 * Note however that false will be returned even in cases, when necessary extensions are
 * not supported by OpenCL implementation, so this does not necesarilly mean that no
 * device is capable of sharing between OpenGL and OpenCL is present.
 */
bool selectGLDeviceAndPlatform(cl_device_id *device, cl_platform_id *platform);

/**
 * A function to select appropriate platform and device id-s
 *
 * @param device a pointer to cl_device_id variable that will contain the selected device
 * @param platform a pointer to cl_platform_id variable that will contain the selected platform
 * @param dev_type the preferred type of device
 *
 * @return true on success, false otherwise
 */
bool selectPlatformAndDevice(cl_device_id *device, cl_platform_id *platform,
                             cl_device_type dev_type = CL_DEVICE_TYPE_GPU);

///////////////////////////////////////////////////////////////////////////////
// Kernel and program management

/**
 * A function to create an OpenCL program object from an array of OpenCL
 * program file paths.
 *
 * @param program_files an array of pointers to OpenCL program filenames
 * @param num the number of file paths
 *
 * @return a newly created Program or a NULL program on Error
 */
cl_program buildProgram(const cl_context ctx,
                        const char * const program_files[],
                        const unsigned int num,
                        const char *built_options = nullptr);

/**
 * A class to initialize kernel arguments
 */
class KernelArgs
{
  public:
    KernelArgs(cl::Kernel kernel, const char *kernel_name = "")
      : m_kernel(kernel()), m_next_arg(0),
        m_err(CL_SUCCESS), m_kernel_name(kernel_name)
    {
      assert(m_kernel != nullptr);
    }

    KernelArgs(cl_kernel kernel, const char *kernel_name = "")
      : m_kernel(kernel), m_next_arg(0),
        m_err(CL_SUCCESS), m_kernel_name(kernel_name)
    {
      assert(m_kernel != nullptr);
    }

    template <typename T>
    KernelArgs & arg(T param)
    { return setArgNext(sizeof(T), &param); }

    template <typename T>
    KernelArgs & arg(T param, cl_uint index)
    { return setArgIndex(sizeof(T), &param, , index); }

    template <typename T>
    KernelArgs & arg(cl::LocalSpaceArg param)
    { return setArgNext(param.size_, nullptr); }
    
    template <typename T>
    KernelArgs & arg(cl::LocalSpaceArg param, cl_uint index)
    { return setArgIndex(param.size_, nullptr, index); }

    bool hasError(void) const { return m_err != CL_SUCCESS; }
    cl_int error(void) const { return m_err; }
    const char *errorString(void) const { return errorToStr(m_err); }
    operator bool(void) const { return m_err == CL_SUCCESS; }

    cl_int argIndex(void) const { return m_next_arg - 1; }

  private:
    KernelArgs & setArgNext(size_t size, const void *value)
    {
      if (m_err != CL_SUCCESS) return *this;
      m_err = clSetKernelArg(m_kernel, m_next_arg++, size, value);
      if (m_err != CL_SUCCESS)
      {
        std::cerr << "Failed to set argument number "
                  << (m_next_arg - 1) << "of " << m_kernel_name
                  << "kernel : " << errorToStr(m_err)
                  << std::endl;
      }
      return *this;
    }

    KernelArgs & setArgIndex(size_t size, const void *value, cl_uint index)
    {
      if (m_err != CL_SUCCESS) return *this;
      m_next_arg = index;
      m_err = clSetKernelArg(m_kernel, m_next_arg++, size, value);
      if (m_err != CL_SUCCESS)
      {
        std::cerr << "Failed to set argument number "
                  << (m_next_arg - 1) << "of " << m_kernel_name
                  << "kernel : " << errorToStr(m_err)
                  << std::endl;
      }
      return *this;
    }

  private:
    cl_kernel m_kernel;          // the OpenCL kernel to be setup
    cl_uint m_next_arg;          // the number of the next argument that will be initialized
    cl_int m_err;                // whether any error occured while processing arguments
    const char *m_kernel_name;   // kernel's string name (for better diagnostic messages)
};

///////////////////////////////////////////////////////////////////////////////
// OpenGL interoperability

/** Class representing a buffer that is shared between OpenCL and OpenGL */
class GLBuffer
{
  public:
    enum AccessType {
      READ_ONLY  = CL_MEM_READ_ONLY,
      WRITE_ONLY = CL_MEM_WRITE_ONLY,
      READ_WRITE = CL_MEM_READ_WRITE
    };

  public:
    explicit GLBuffer(cl_context ctx = nullptr)
      : m_vbo(0),
        m_mem(nullptr),
        m_ctx(ctx)
    {
      glGenBuffers(1, &m_vbo);
      GLenum err = glGetError();
      if (err != GL_NO_ERROR) throw Exception("Failed to construct GLBuffer", err);
    }
    
    GLBuffer(GLBuffer && other)
    {
      m_vbo = other.m_vbo;
      other.m_vbo = 0;
      m_mem = other.m_mem;
      other.m_mem = nullptr;
    }

    ~GLBuffer(void)
    {
      clReleaseMemObject(m_mem);
      glDeleteBuffers(1, &m_vbo);
    }

    GLBuffer & operator=(GLBuffer && other)
    {
      GLuint tmp_vbo = other.m_vbo;
      other.m_vbo = m_vbo;
      m_vbo = tmp_vbo;

      cl_mem tmp_mem = other.m_mem;
      other.m_mem = m_mem;
      m_mem = tmp_mem;

      return *this;
    }

    GLuint getGLID(void) const
    {
      return m_vbo;
    }

    cl_mem getCLID(void) const
    {
      return m_mem;
    }

    void setCLContext(cl_context ctx)
    {
      m_ctx = ctx;
    }

    bool bufferData(const GLvoid *data,               // a pointer to data that will be stored in the buffer
                    GLsizeiptr size,                  // the size of the data that will be stored (in bytes)
                    AccessType at = READ_WRITE,       // the access type of OpenCL's memory object
                    GLenum usage = GL_DYNAMIC_DRAW);  // the way the buffer will be utilized (assume, that the contents
                                                      // of the buffer will be used in rendering, but that they will be
                                                      // changed often by an OpenCL kernel)

  private:
    GLBuffer(const GLBuffer & );
    GLBuffer & operator=(const GLBuffer & );

  private:
    GLuint m_vbo;      /// vertex buffer object of OpenGL
    cl_mem m_mem;      /// OpenCL memory object
    cl_context m_ctx;  /// OpenCL context to which the buffer is bound (not owned by this class)
};

/**
 * A class to synchronize OpenGL and OpenCL memory objects
 */
class GLSyncHandler
{
  public:
#if 0
    GLSyncHandler(cl_command_queue queue, cl_mem buffer)
      : m_mem(buffer), m_queue(queue), m_num_buffers(1),
        m_buffers(&m_mem), m_err(CL_SUCCESS)
    {
      acquireGLObjects();
    }
#endif
    GLSyncHandler(cl_command_queue queue, cl_uint num_buffers, const cl_mem *buffers)
      : m_queue(queue), m_num_buffers(num_buffers),
        m_buffers(buffers), m_err(CL_SUCCESS)
    {
      acquireGLObjects();
    }

    ~GLSyncHandler(void)
    {
      /* unlock the vertex buffer object, so that OpenGL can continue using it */
      m_err = clEnqueueReleaseGLObjects(m_queue, m_num_buffers, m_buffers, 0, nullptr, nullptr);
      if (m_err != CL_SUCCESS)
      {
        std::cerr << "Failed to release an exclusive access to one of OpenGL's vertex buffer objects: "
                  << errorToStr(m_err) << std::endl;
      }
      
      /* wait for OpenCL to finish processing */
      clFinish(m_queue);
    }

    bool hasError(void) const { return m_err != CL_SUCCESS; }
    operator bool(void) const { return m_err == CL_SUCCESS; }

  private:
    void acquireGLObjects(void)
    {
      assert(m_queue != nullptr);
      assert(m_buffers != nullptr);

      /* wait for OpenGL to finish rendering */
      glFinish();

      /* acquire access to the shared vertex buffer object */
      m_err = clEnqueueAcquireGLObjects(m_queue, m_num_buffers, m_buffers, 0, nullptr, nullptr);
      if (m_err != CL_SUCCESS)
      {
        std::cerr << "Failed to acquire an exclusive access to one of OpenGL's vertex buffer objects: "
                  << errorToStr(m_err) << std::endl;
        return;
      }

      return;
    }

  private:
    // just disable copying for now
    GLSyncHandler(const GLSyncHandler & );
    GLSyncHandler & operator=(const GLSyncHandler & );

  private:
    //cl_mem m_mem;
    cl_command_queue m_queue;
    cl_uint m_num_buffers;
    const cl_mem *m_buffers;
    cl_int m_err;
};

///////////////////////////////////////////////////////////////////////////////
// Performance countersand event handlers

class Event
{
  public:
    Event(cl_event event = nullptr) : m_event(event) { }
    Event(Event && other) : m_event(other.m_event) { other.m_event = nullptr; }
    ~Event(void) { clReleaseEvent(m_event); }

    Event & operator=(Event && other) { std::swap(m_event, other.m_event); }

    void setCallback(void (CL_CALLBACK * cb)(cl_event, cl_int, void *),
                     void *data = nullptr,
                     cl_int status = CL_COMPLETE)
    {
      if (m_event != nullptr)
      {
        cl_int err = clSetEventCallback(m_event, status, cb, (void *) data);
        if (err != CL_SUCCESS)
        {
          std::cerr << "WARNING: Failed to set event callback: "
                    << errorToStr(err) << " (" << err << ")"
                    << std::endl;
        }
      }
    }

    operator cl_event (void) const { return m_event; }
    operator cl_event * (void) { return &m_event; }

  private:
    Event(const Event & );
    Event & operator=(const Event & );

  private:
    cl_event m_event;
};

class PerfStatsRecord
{
  public:
    PerfStatsRecord(void)
      : m_count(0)
      , m_queue_time()
      , m_submit_time()
      , m_exec_time()
    {
    }
   
    void add(cl_ulong queue_time, cl_ulong submit_time, cl_ulong exec_time)
    {
      m_queue_time.add(queue_time, m_count);
      m_submit_time.add(submit_time, m_count);
      m_exec_time.add(exec_time, m_count);
      m_count++;
    }

    void print(const std::string & name, std::ostream & os);

  private:
    unsigned int m_count;                              /// the number of times this record has been recorded
    utils::stats::Aggregator<cl_ulong> m_queue_time;   /// aggregated statistical records for in queue time
    utils::stats::Aggregator<cl_ulong> m_submit_time;  /// aggregated statistical records for submit time
    utils::stats::Aggregator<cl_ulong> m_exec_time;    /// aggregated statistical records for execution time
};

class PerfStats
{
  private:
    struct EventProxy
    {
      PerfStatsRecord *m_stats_rec;
      cl_event m_event;

      EventProxy(PerfStatsRecord *stats_rec)
        : m_stats_rec(stats_rec), m_event(nullptr)
      {
      }

      ~EventProxy(void)
      {
        if (m_event != nullptr)
        {
          clSetEventCallback(m_event, CL_COMPLETE, cl_callback, (void *) m_stats_rec);
        }
      }

      operator cl_event * (void) { return &m_event; }
    };

    struct EventProxyRef
    {
      PerfStatsRecord *m_stats_rec;
      Event & m_event;

      EventProxyRef(Event & event, PerfStatsRecord *stats_rec)
        : m_stats_rec(stats_rec), m_event(event)
      {
      }

      ~EventProxyRef(void) { m_event.setCallback(cl_callback, m_stats_rec); }
      operator cl_event * (void) { return m_event; }
    };

    static void CL_CALLBACK cl_callback(cl_event ev, cl_int status, void *stats_rec);

  private:
    typedef std::unordered_map<std::string, std::unique_ptr<PerfStatsRecord>> tContainer;

  public:
    PerfStats(void) : m_stats() { }

    void clear(void) { return m_stats.clear(); }
    
    EventProxyRef event(Event & event, const std::string & name = std::string())
    { return EventProxyRef(event, insertStat(name)); }
    
    EventProxy event(const std::string & name = std::string())
    { return EventProxy(insertStat(name)); }

    friend std::ostream & operator<<(std::ostream & os, const PerfStats & stats)
    {
      for (auto & it : stats.m_stats)
      {
        it.second->print(it.first, os);
        os << std::endl;
      }
      return os;
    }

  private:
    PerfStatsRecord *insertStat(const std::string & name)
    {
      tContainer::iterator it = m_stats.find(name);
      if (it == m_stats.end())
      {
        PerfStatsRecord *ptr = new PerfStatsRecord;
        m_stats.insert(std::make_pair(name, std::unique_ptr<PerfStatsRecord>(ptr)));
        return ptr;
      }
      else
      {
        return it->second.get();
      }
    }

  private:
    tContainer m_stats;
};

}

#endif