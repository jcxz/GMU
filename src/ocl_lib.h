#ifndef OCL_LIB_H
#define OCL_LIB_H

#include "ogl_lib.h"

#include <CL/cl.hpp>
#include <stdexcept>



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
// Bufer management

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

}

#endif