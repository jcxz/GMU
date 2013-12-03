#include "ocl_lib.h"
#include "global.h"
#include "debug.h"
#include "utils.h"



namespace ocl {

///////////////////////////////////////////////////////////////////////////////
// Error handling

const char *errorToStr(cl_int err)
{
  switch (err)
  {
    case CL_SUCCESS: return "Success!";
    case CL_DEVICE_NOT_FOUND: return "Device not found.";
    case CL_DEVICE_NOT_AVAILABLE: return "Device not available";
    case CL_COMPILER_NOT_AVAILABLE: return "Compiler not available";
    case CL_MEM_OBJECT_ALLOCATION_FAILURE: return "Memory object allocation failure";
    case CL_OUT_OF_RESOURCES: return "Out of resources";
    case CL_OUT_OF_HOST_MEMORY: return "Out of host memory";
    case CL_PROFILING_INFO_NOT_AVAILABLE: return "Profiling information not available";
    case CL_MEM_COPY_OVERLAP: return "Memory copy overlap";
    case CL_IMAGE_FORMAT_MISMATCH: return "Image format mismatch";
    case CL_IMAGE_FORMAT_NOT_SUPPORTED: return "Image format not supported";
    case CL_BUILD_PROGRAM_FAILURE: return "Program build failure";
    case CL_MAP_FAILURE: return "Map failure";
    case CL_INVALID_VALUE: return "Invalid value";
    case CL_INVALID_DEVICE_TYPE: return "Invalid device type";
    case CL_INVALID_PLATFORM: return "Invalid platform";
    case CL_INVALID_DEVICE: return "Invalid device";
    case CL_INVALID_CONTEXT: return "Invalid context";
    case CL_INVALID_QUEUE_PROPERTIES: return "Invalid queue properties";
    case CL_INVALID_COMMAND_QUEUE: return "Invalid command queue";
    case CL_INVALID_HOST_PTR: return "Invalid host pointer";
    case CL_INVALID_MEM_OBJECT: return "Invalid memory object";
    case CL_INVALID_IMAGE_FORMAT_DESCRIPTOR: return "Invalid image format descriptor";
    case CL_INVALID_IMAGE_SIZE: return "Invalid image size";
    case CL_INVALID_SAMPLER: return "Invalid sampler";
    case CL_INVALID_BINARY: return "Invalid binary";
    case CL_INVALID_BUILD_OPTIONS: return "Invalid build options";
    case CL_INVALID_PROGRAM: return "Invalid program";
    case CL_INVALID_PROGRAM_EXECUTABLE: return "Invalid program executable";
    case CL_INVALID_KERNEL_NAME: return "Invalid kernel name";
    case CL_INVALID_KERNEL_DEFINITION: return "Invalid kernel definition";
    case CL_INVALID_KERNEL: return "Invalid kernel";
    case CL_INVALID_ARG_INDEX: return "Invalid argument index";
    case CL_INVALID_ARG_VALUE: return "Invalid argument value";
    case CL_INVALID_ARG_SIZE: return "Invalid argument size";
    case CL_INVALID_KERNEL_ARGS: return "Invalid kernel arguments";
    case CL_INVALID_WORK_DIMENSION: return "Invalid work dimension";
    case CL_INVALID_WORK_GROUP_SIZE: return "Invalid work group size";
    case CL_INVALID_WORK_ITEM_SIZE: return "Invalid work item size";
    case CL_INVALID_GLOBAL_OFFSET: return "Invalid global offset";
    case CL_INVALID_EVENT_WAIT_LIST: return "Invalid event wait list";
    case CL_INVALID_EVENT: return "Invalid event";
    case CL_INVALID_OPERATION: return "Invalid operation";
    case CL_INVALID_GL_OBJECT: return "Invalid OpenGL object";
    case CL_INVALID_BUFFER_SIZE: return "Invalid buffer size";
    case CL_INVALID_MIP_LEVEL: return "Invalid mip-map level";
  }

  return "Unknown OpenCL error";
}


///////////////////////////////////////////////////////////////////////////////
// Device and platform management

bool selectGLDeviceAndPlatform(cl_device_id *device, cl_platform_id *platform)
{
  assert(device != nullptr);
  assert(platform != nullptr);

  /* first try to get the necessary extension */
  clGetGLContextInfoKHR_fn clGetGLContextInfoKHR = (clGetGLContextInfoKHR_fn) clGetExtensionFunctionAddress("clGetGLContextInfoKHR");
  if (clGetGLContextInfoKHR == nullptr)
  {
    ERROR("clGetGLContextInfoKHR extension function not supproted.");
    return false;
  }

#if defined(FLUIDSIM_OS_MAC)
  cl_context_properties props[] = {
    CL_CONTEXT_PROPERTY_USE_CGL_SHAREGROUP_APPLE,
    (cl_context_properties) CGLGetShareGroup(CGLGetCurrentContext()),
    0
  };

  cl_int err = clGetGLContextInfoKHR(props,                                  // the OpenGL context
                                     CL_CURRENT_DEVICE_FOR_GL_CONTEXT_KHR,   // get the id of the device currently executing OpenGL
                                     sizeof(*device),
                                     device,
                                     nullptr);
  if (err != CL_SUCCESS)
  {
    ERROR("Failed to retrieve the OpenCL id of the device executing OpenGL: " << errorToStr(err));
    return false;
  }

  /* get the platform associated with the device */
  err = clGetDeviceInfo(*device, CL_DEVICE_PLATFORM, sizeof(*platform), platform, nullptr);
  if (err != CL_SUCCESS)
  {
    ERROR("Failed to retirieve platform id for the device executing OpenGL: " << errorToStr(err));
    return false;
  }
#else
  cl_context_properties props[] = {
#if defined(FLUIDSIM_OS_UNIX)
    CL_GL_CONTEXT_KHR, (cl_context_properties) glXGetCurrentContext(), 
    CL_GLX_DISPLAY_KHR, (cl_context_properties) glXGetCurrentDisplay(), 
    CL_CONTEXT_PLATFORM, (cl_context_properties) nullptr,
#elif defined(FLUIDSIM_OS_WIN)
    CL_GL_CONTEXT_KHR, (cl_context_properties) wglGetCurrentContext(),
    CL_WGL_HDC_KHR, (cl_context_properties) wglGetCurrentDC(),
    CL_CONTEXT_PLATFORM, (cl_context_properties) nullptr,
#else
# error "Unsupported OS platform"
#endif
    0
  };

  std::vector<cl::Platform> platform_list;

  /* enumerate all available platforms */
  cl_int err = cl::Platform::get(&platform_list);
  if (err != CL_SUCCESS)
  {
    ERROR("Failed to retrieve a list of available platforms: " << errorToStr(err));
    return false;
  }

  /* got through all the available platforms and find the one
     that contains the OpenGL device */
  for (cl::Platform p : platform_list)
  {
    WARN("platform: " << p.getInfo<CL_PLATFORM_NAME>());
    props[5] = (cl_context_properties) p();
    cl_int err = clGetGLContextInfoKHR(props,                                 // the OpenGL context
                                       CL_CURRENT_DEVICE_FOR_GL_CONTEXT_KHR,  // get the id of the device currently executing OpenGL
                                       sizeof(*device),
                                       device,
                                       nullptr);
    if (err == CL_SUCCESS)
    {
      *platform = (cl_platform_id) props[5];
      return true;
    }
    else
    {
      WARN("clGetGLContextInfoKHR: " << errorToStr(err));
    }
  }
#endif

  return false;
}


bool selectPlatformAndDevice(cl_device_id *device, cl_platform_id *platform,
                             cl_device_type dev_type)
{
  assert(device != nullptr);
  assert(platform != nullptr);

  std::vector<cl::Platform> platform_list;
  std::vector<cl::Device> device_list;

  /* enumerate all available platforms */
  cl_int err = cl::Platform::get(&platform_list);
  if (err != CL_SUCCESS)
  {
    ERROR("Failed to retrieve a list of available platforms: " << errorToStr(err));
    return false;
  }

  /* go through each of the enumerated platforms and enumerate its devices */
  for (cl::Platform p : platform_list)
  {
    p.getDevices(dev_type, &device_list);

    /* select the one that is the most suitable according to given preferences */
    for (cl::Device d : device_list)
    {
      cl_device_type type = d.getInfo<CL_DEVICE_TYPE>(&err);
      if ((err == CL_SUCCESS) && (type & dev_type))
      {
        *device = d();
        *platform = p();
        return true;
      }
    }
  }

  // no suitable device found
  return false;
}


///////////////////////////////////////////////////////////////////////////////
// Kernel and program management


cl_program buildProgram(const cl_context ctx,
                        const char * const program_files[],
                        const unsigned int num,
                        const char *built_options)
{
  // use alloca since MSVC lacks variable length arrays
  // NOTE: alloca() allocates memory on stackand this memory is automatically
  // freed whenthe function is exited
  const char **programs = (const char **) alloca(num * sizeof(const char *));

  for (unsigned int i = 0; i < num; ++i)
  {
    programs[i] = utils::loadFile(utils::AssetsPath(program_files[i]));
    if (programs[i] == nullptr)
    {
      std::cerr << "Failed to load program file: "
                << utils::AssetsPath(program_files[i]) << std::endl;
      /* if loading failed delete all sofar loaded programs */
      for (unsigned int j = 0; j < i; ++j)
      {
        delete [] programs[j];
      }
      return nullptr;
    }
  }
  
  /* construct the program object */
  cl_int err = CL_SUCCESS;
  cl_program program = clCreateProgramWithSource(ctx, num, programs, nullptr, &err);

  /* free all the programs once the program is created */
  for (unsigned int i = 0; i < num; ++i)
  {
    delete [] programs[i];
  }

  /* check if program creation succeeded */
  if (err != CL_SUCCESS)
  {
    std::cerr << "Failed to create the program object: " << errorToStr(err) << std::endl;
    return nullptr;
  }

  /* compile the program for all devices in the context */
  err = clBuildProgram(program, 0, nullptr, built_options, nullptr, nullptr);

  /* print the build information */
  {
    clRetainContext(ctx);       // to prevent cl::Context from releasing it
    clRetainProgram(program);   // to prevent cl::Program from releasing it
    std::vector<cl::Device> devices = cl::Context(ctx).getInfo<CL_CONTEXT_DEVICES>();
    std::cerr << "Program build log: " << std::endl;
    std::cerr << cl::Program(program).getBuildInfo<CL_PROGRAM_BUILD_LOG>(devices.front()) << std::endl;
  }

  /* check on compilation status */
  if (err != CL_SUCCESS)
  {
    ERROR("Failed to compile OpenCL program: " << ocl::errorToStr(err));
    clReleaseProgram(program);
    return nullptr;
  }

  return program;
}


///////////////////////////////////////////////////////////////////////////////
// OpenGL interoperability

bool GLBuffer::bufferData(const GLvoid *data, GLsizeiptr size, AccessType at, GLenum usage)
{
  assert(m_ctx != nullptr);

  glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
  glBufferData(GL_ARRAY_BUFFER, size, data, usage);

  {
    GLenum err = glGetError();
    if (err != GL_NO_ERROR)
    {
      ERROR("Failed to buffer data: " << ogl::errorToStr(err));
      return false;
    }
  }

  glBindBuffer(GL_ARRAY_BUFFER, 0);

  cl_int err = CL_SUCCESS;
  cl_mem mem = clCreateFromGLBuffer(m_ctx, at, m_vbo, &err);
  if (err != CL_SUCCESS)
  {
    ERROR("Failed to create OpenCL buffer: " << ocl::errorToStr(err));
    return false;
  }

  clReleaseMemObject(m_mem);

  m_mem = mem;

  return true;
}

}