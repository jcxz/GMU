#include "FluidSystem.h"
#include "global.h"
#include "debug.h"
#include "utils.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>



const char *FluidSystem::m_vert_shader =
  "#version 330\n"
  ""
  "uniform mat4 mvp;"
  ""
  "layout(location = 0) in vec3 pos;"           // model's vertex position
  "layout(location = 1) in vec3 normal;"        // model's normal
  "layout(location = 4) in vec3 particle_pos;"  // particle's position
  ""
  "void main(void)"
  "{"
  "  gl_Position = mvp * vec4(pos + particle_pos, 1.0f);"
  //"  gl_Position = 0.005 * vec4(pos + particle_pos, 1.0f);"
  //"  gl_Position = vec4((pos + particle_pos) * 0.02, 1.0f);"
  "}"
;

const char *FluidSystem::m_frag_shader =
  "#version 330\n"
  ""
  "void main(void)"
  "{"
  "  gl_FragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);"
  "}"
;


const char *FluidSystem::m_part_pos_kernel_src = "/src/OpenCL/rand_part_positions.cl";



bool FluidSystem::initCL(void)
{
  INFO("Initializing OpenCL subsystem");

  /* select appropriate device and platform */
  cl_platform_id platform = nullptr;
  cl_device_id device = nullptr;

  if ((!ocl::selectGLDeviceAndPlatform(&device, &platform)) &&
      (!ocl::selectPlatformAndDevice(&device, &platform)))
  {
    ERROR("Failed to select an appropriate device or platform");
    return false;
  }

  std::vector<cl::Device> device_list(1, device);

  /* setup context */
  cl_context_properties props[] = {
#if defined(FLUIDSIM_OS_MAC)
    CL_CONTEXT_PROPERTY_USE_CGL_SHAREGROUP_APPLE,
    (cl_context_properties) CGLGetShareGroup(CGLGetCurrentContext()),
#elif defined(FLUIDSIM_OS_UNIX)
    CL_GL_CONTEXT_KHR, (cl_context_properties) glXGetCurrentContext(), 
    CL_GLX_DISPLAY_KHR, (cl_context_properties) glXGetCurrentDisplay(), 
    CL_CONTEXT_PLATFORM, (cl_context_properties) platform,
#elif defined(FLUIDSIM_OS_WIN)
    CL_GL_CONTEXT_KHR, (cl_context_properties) wglGetCurrentContext(),
    CL_WGL_HDC_KHR, (cl_context_properties) wglGetCurrentDC(),
    CL_CONTEXT_PLATFORM, (cl_context_properties) platform,
#else
# error "Unsupported OS platform"
#endif
    0
  };

  cl_int err = CL_SUCCESS;
  m_cl_ctx = cl::Context(device_list, props, nullptr, nullptr, &err);
  if (err != CL_SUCCESS)
  {
    ERROR("Failed to create OpenCL context: " << ocl::errorToStr(err));
    return false;
  }

  /* create command queue */
  m_cl_queue = cl::CommandQueue(m_cl_ctx, device, 0, &err);
  if (err != CL_SUCCESS)
  {
    ERROR("Failed to create OpenCL command queue: " << ocl::errorToStr(err));
    return false;
  }

  /* load kernel sources */
  std::string prog_source;
  if (!utils::loadFile(utils::AssetsPath(m_part_pos_kernel_src), &prog_source))
  {
    ERROR("Failed to load rand_part_positons.cl kernel");
    return false;
  }

  cl::Program::Sources sources(1, std::make_pair(prog_source.c_str(), prog_source.size()));

  /* create program  */
  m_cl_prog = cl::Program(m_cl_ctx, sources, &err);
  if (err != CL_SUCCESS)
  {
    ERROR("Failed to create OpenCL program: " << ocl::errorToStr(err));
    return false;
  }

  /* compile the program */
  err = m_cl_prog.build(device_list, "");
  
  std::cerr << m_cl_prog.getBuildInfo<CL_PROGRAM_BUILD_LOG>(device) << std::endl;

  if (err != CL_SUCCESS)
  {
    ERROR("Failed to compile OpenCL program: " << ocl::errorToStr(err));
    return false;
  }

  /* create kernels */
  m_part_pos_kernel = cl::Kernel(m_cl_prog, "gen_part_positions", &err);
  if (err != CL_SUCCESS)
  {
    ERROR("Failed to create particle generation kernel: " << ocl::errorToStr(err));
    return false;
  }

  /* pass the context pointer to the OpeGL shared buffer */
  m_particle_buf.setCLContext(m_cl_ctx());

  INFO("Successfully initialized OpenCL context and compiled kernels");

  return true;
}


bool FluidSystem::initGL(void)
{
  INFO("Initializing OpenGL subsystem");

  /* compile shaders */
  if (!m_shader.build(m_vert_shader, m_frag_shader))
  {
    ERROR("Failed to compile shaders");
    return false;
  }

  /* load models */
  if (!geom::genSphere(m_particle_geom))
  {
    ERROR("Failed to generate sphere model");
    return false;
  }

  return true;
}


bool FluidSystem::reset(unsigned int part_num)
{
  INFO("Initializing data");

  /* allocate the buffer to hold particle positions */
  if (!m_particle_buf.bufferData(nullptr, part_num * 3 * sizeof(float), ocl::GLBuffer::WRITE_ONLY))
  {
    ERROR("Failed to initialize GLBuffer");
    return false;
  }

  /* set the buffer as an argument for the kernel */
  cl_mem mem = m_particle_buf.getCLID();
  cl_int err = m_part_pos_kernel.setArg(0, sizeof(mem), &mem);
  if (err != CL_SUCCESS)
  {
    ERROR("Failed to set OpenCL kernel argument");
    return false;
  }

  m_num_particles = part_num;

  return true;
}


void FluidSystem::update(void)
{
  cl_mem mem = m_particle_buf.getCLID();
  cl_command_queue queue = m_cl_queue();

  /* wait for OpenGL to finish rendering */
  glFinish();

  /* acquire access to the shared vertex buffer object */
  cl_int err = clEnqueueAcquireGLObjects(queue, 1, &mem, 0, nullptr, nullptr);
  if (err != CL_SUCCESS)
  {
    ERROR("Failed to acquire an exclusive access to the OpenGL's vertex buffer object");
    return;
  }

  /* execute the kernel */
  err = clEnqueueNDRangeKernel(queue, m_part_pos_kernel(), 1,
                               nullptr, &m_num_particles, nullptr,
                               0, nullptr, nullptr);
  if (err != CL_SUCCESS)
  {
    WARN("Failed to release an exclusive access to the OpenGL's vertex buffer object");
  }

  /* unlock the vertex buffer object, so that OpenGL can continue using it */
  err = clEnqueueReleaseGLObjects(queue, 1, &mem, 0, nullptr, nullptr);
  if (err != CL_SUCCESS)
  {
    WARN("Failed to release an exclusive access to the OpenGL's vertex buffer object");
  }

  /* wait for OpenCL to finish processing */
  clFinish(queue);

  return;
}


void FluidSystem::render(const glm::mat4 & m)
{
  glEnable(GL_DEPTH_TEST);

  glm::mat4 mvp = glm::rotate(
                     glm::rotate(
                          glm::translate(
                               glm::perspective(45.0f, float(800) / float(600), 0.1f, 100.0f),
                               glm::vec3(0.0f, 0.0f, -50.0f)
                          ),
                        45.0f,
                        glm::vec3(1.0f, 0.0f, 0.0f)
                     ),
                     45.0f,
                     glm::vec3(0.0f, 1.0f, 0.0f)
                  );

  m_shader.use();

  glUniformMatrix4fv(glGetUniformLocation(m_shader.getID(), "mvp"), 1, GL_FALSE, glm::value_ptr(mvp));

  glBindVertexArray(m_particle_geom.vao);

  glBindBuffer(GL_ARRAY_BUFFER, m_particle_buf.getGLID());
  glEnableVertexAttribArray(4);
  glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 0, (void *) 0);
  glVertexAttribDivisor(4, 1);

  glDrawArraysInstanced(m_particle_geom.mode, 0, m_particle_geom.count, m_num_particles);

  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glBindVertexArray(0);

  glUseProgram(0);

  return;
}