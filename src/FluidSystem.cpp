#include "FluidSystem.h"
#include "global.h"
#include "debug.h"
#include "utils.h"

#include <glm/gtc/type_ptr.hpp>
#include <ctime>



const char *FluidSystem::m_vert_shader =
  "#version 330\n"
  ""
  "uniform mat4 mvp;"
  ""
  "layout(location = 0) in vec3 pos;"           // model's vertex position
  "layout(location = 1) in vec3 normal;"        // model's normal
  "layout(location = 4) in vec3 particle_pos;"  // particle's position
  "layout(location = 5) in vec4 particle_col;"  // particle's color
  ""
  ""
  "out vec3 o_normal;"
  "out vec4 o_particle_col;"
  ""
  "void main(void)"
  "{"
  "  gl_Position = mvp * vec4(pos + particle_pos, 1.0f);"
  "  o_normal = normal;"
  "  o_particle_col = particle_col;"
  //"  gl_Position = 0.005 * vec4(pos + particle_pos, 1.0f);"
  //"  gl_Position = vec4((pos + particle_pos) * 0.02, 1.0f);"
  "}"
;

const char *FluidSystem::m_frag_shader =
  "#version 330\n"
  ""
  "uniform vec3 light_pos;"
  "uniform vec4 light_col_a;"
  "uniform vec4 light_col_k;"
  "uniform vec4 light_col_s;"
  ""
  "in vec3 o_normal;"
  "in vec4 o_particle_col;"
  ""
  "void main(void)"
  "{"
  //"  gl_FragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);"
  "  float dp = dot(o_normal, light_pos);"
  "  if (dp < 0.0f) dp = 0.0f;"
  "  gl_FragColor = light_col_a * o_particle_col + light_col_k * o_particle_col * dp;"
  "}"
;


const char *FluidSystem::m_vert_shader_file = "src/OpenGL/FluidSystem.vert";
const char *FluidSystem::m_frag_shader_file = "src/OpenGL/FluidSystem.frag";

//const char *FluidSystem::m_part_pos_kernel_file = "/src/OpenCL/gen_rand_particles.cl";

const char *FluidSystem::m_test_kernel_files[] = {
  "/src/OpenCL/gen_rand_particles.cl"
};

const unsigned int FluidSystem::m_test_kernel_files_size =  sizeof(m_test_kernel_files) /
                                                            sizeof(*m_test_kernel_files);


const char *FluidSystem::m_sph_kernel_files[] = {
  "/src/OpenCL/sph_compute_pressure.cl",
  "/src/OpenCL/sph_compute_force.cl",
  "/src/OpenCL/sph_compute_step.cl",
  "/src/OpenCL/sph_reset.cl"
};

const unsigned int FluidSystem::m_sph_kernel_files_size = sizeof(m_sph_kernel_files) /
                                                          sizeof(*m_sph_kernel_files);





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

#if 0
  /* load kernel sources */
  std::string prog_source;
  if (!utils::loadFile(utils::AssetsPath(m_part_pos_kernel_file), &prog_source))
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
#endif

  if ((!initTestSimulation()) || (!initSPHSimulation()))
  {
    return false;
  }

  /* pass the context pointer to OpenGL shared buffers */
  m_particle_pos_buf.setCLContext(m_cl_ctx());
  m_particle_col_buf.setCLContext(m_cl_ctx());

  INFO("Successfully initialized OpenCL context and compiled kernels");

  return true;
}


bool FluidSystem::initGL(void)
{
  INFO("Initializing OpenGL subsystem");

  /* compile shaders */
  //if (!m_shader.build(m_vert_shader, m_frag_shader))
  if (!m_shader.buildFiles(utils::AssetsPath(m_vert_shader_file),
                           utils::AssetsPath(m_frag_shader_file)))
  {
    ERROR("Failed to compile shaders");
    return false;
  }

  /* set lights */
  GLuint prog = m_shader.getID();
  
  glUseProgram(prog);
  //glUniform3f(glGetUniformLocation(prog, "light_pos"), 0.0f, 0.0f, -1.0f);     // shader assumes the position is normalized
  //glUniform3f(glGetUniformLocation(prog, "light_pos"), -1.0f, 1.0f, -1.0f);
  glUniform3f(glGetUniformLocation(prog, "light_pos"), -10.0f, 10.0f, 15.0f);
  glUniform3f(glGetUniformLocation(prog, "light_col_a"), 0.8f, 0.8f, 0.8f);
  glUniform3f(glGetUniformLocation(prog, "light_col_d"), 1.0f, 1.0f, 1.0f);
  glUniform3f(glGetUniformLocation(prog, "light_col_s"), 1.0f, 1.0f, 1.0f);
  glUseProgram(0);

  /* load models */
  if (!geom::genSphere(m_particle_geom))
  //if (!geom::genPrism(m_particle_geom))
  {
    ERROR("Failed to generate sphere model");
    return false;
  }

  return true;
}


bool FluidSystem::initTestSimulation(void)
{
  /* create a program */
  m_test_prog = ocl::buildProgram(m_cl_ctx(), m_test_kernel_files, m_test_kernel_files_size);
  if (m_test_prog() == nullptr)
  {
    ERROR("Failed to create Test OpenCL program");
    return false;
  }

  /* create kernels */
  cl_int err = CL_SUCCESS;
  m_test_kernel = cl::Kernel(m_test_prog, "gen_part_positions", &err);
  if (err != CL_SUCCESS)
  {
    ERROR("Failed to create particle generation kernel for Test simulation: "
          << ocl::errorToStr(err));
    return false;
  }

  return true;
}


bool FluidSystem::resetTestSimulation(unsigned int part_num)
{
  INFO("Initializing Test Simulation data");

  /* allocate the buffers to hold particle positions and colors */
  if (!m_particle_pos_buf.bufferData(nullptr, part_num * sizeof(cl_float4), ocl::GLBuffer::WRITE_ONLY))
  {
    ERROR("Failed to initialize position GLBuffer");
    return false;
  }

  if (!m_particle_col_buf.bufferData(nullptr, part_num * sizeof(cl_float4), ocl::GLBuffer::WRITE_ONLY))
  {
    ERROR("Failed to initialize color GLBuffer");
    return false;
  }

  /* initialize kernel arguments */
  if (!ocl::KernelArgs(m_test_kernel, "m_test_kernel")
            .arg(m_particle_pos_buf.getCLID())
            .arg(m_particle_col_buf.getCLID())
            .arg((cl_ulong) (time(nullptr))))
  {
    return false;
  }

#if 0
  cl_int err = CL_SUCCESS;

  /* set the position buffer as an argument to the kernel */
  cl_mem pos_buf = m_particle_pos_buf.getCLID();
  err = m_test_kernel.setArg(0, sizeof(pos_buf), &pos_buf);
  if (err != CL_SUCCESS)
  {
    ERROR("Failed to set particle position buffer as an OpenCL kernel argument: " << ocl::errorToStr(err));
    return false;
  }
  
  /* set the color buffer as an argument to the kernel */
  cl_mem col_buf = m_particle_col_buf.getCLID();
  err = m_test_kernel.setArg(1, sizeof(col_buf), &col_buf);
  if (err != CL_SUCCESS)
  {
    ERROR("Failed to set particle color buffer as an OpenCL kernel argument: " << ocl::errorToStr(err));
    return false;
  }

  /* set the random number generator seed */
  cl_ulong seed = time(nullptr);
  err = m_test_kernel.setArg(2, sizeof(seed), &seed);
  if (err != CL_SUCCESS)
  {
    ERROR("Failed to set seed value as an OpenCL kernel argument: " << ocl::errorToStr(err));
    return false;
  }
#endif
  m_num_particles = part_num;

  return true;
}


void FluidSystem::updateTestSimulation(void)
{
  cl_command_queue queue = m_cl_queue();
  cl_mem buffers[] = { m_particle_pos_buf.getCLID(), m_particle_col_buf.getCLID() };

  ocl::GLSyncHandler sync(queue, FLUIDSIM_COUNT(buffers), buffers);
  if (!sync) return;

  cl_int err = clEnqueueNDRangeKernel(queue, m_test_kernel(), 1,
                                      nullptr, &m_num_particles, nullptr,
                                      0, nullptr, nullptr);
  if (err != CL_SUCCESS)
  {
    WARN("Failed to enqueue test simulation kernel");
  }

#if 0
  cl_command_queue queue = m_cl_queue();

  /* wait for OpenGL to finish rendering */
  glFinish();

  cl_int err = CL_SUCCESS;
  cl_mem buffers[] = { m_particle_pos_buf.getCLID(), m_particle_col_buf.getCLID() };

  /* acquire access to the shared vertex buffer object */
  err = clEnqueueAcquireGLObjects(queue, FLUIDSIM_COUNT(buffers), buffers, 0, nullptr, nullptr);
  if (err != CL_SUCCESS)
  {
    ERROR("Failed to acquire an exclusive access to the OpenGL's vertex buffer object with particle positions");
    return;
  }

  /* execute the kernel */
  err = clEnqueueNDRangeKernel(queue, m_test_kernel(), 1,
                               nullptr, &m_num_particles, nullptr,
                               0, nullptr, nullptr);
  if (err != CL_SUCCESS)
  {
    WARN("Failed to release an exclusive access to the OpenGL's vertex buffer object");
  }

  /* unlock the vertex buffer object, so that OpenGL can continue using it */
  err = clEnqueueReleaseGLObjects(queue, FLUIDSIM_COUNT(buffers), buffers, 0, nullptr, nullptr);
  if (err != CL_SUCCESS)
  {
    WARN("Failed to release an exclusive access to the OpenGL's vertex buffer object with colors");
  }

  /* wait for OpenCL to finish processing */
  clFinish(queue);
#endif

  return;
}


bool FluidSystem::initSPHSimulation(void)
{
  /* create a program */
  m_sph_prog = ocl::buildProgram(m_cl_ctx(), m_sph_kernel_files, m_sph_kernel_files_size);
  if (m_sph_prog() == nullptr)
  {
    ERROR("Failed to create SPH OpenCL program");
    return false;
  }

  /* create kernels */
  cl_int err = CL_SUCCESS;
  m_sph_reset_kernel = cl::Kernel(m_sph_prog, "sph_reset", &err);
  if (err != CL_SUCCESS)
  {
    ERROR("Failed to create reset kernel for SPH simulation: " << ocl::errorToStr(err));
    return false;
  }

  m_sph_compute_force_kernel = cl::Kernel(m_sph_prog, "sph_compute_force", &err);
  if (err != CL_SUCCESS)
  {
    ERROR("Failed to create compute_force kernel for SPH simulation: " << ocl::errorToStr(err));
    return false;
  }

  m_sph_compute_pressure_kernel = cl::Kernel(m_sph_prog, "sph_compute_pressure", &err);
  if (err != CL_SUCCESS)
  {
    ERROR("Failed to create compute_pressure kernel for SPH simulation: " << ocl::errorToStr(err));
    return false;
  }

  m_sph_compute_step_kernel = cl::Kernel(m_sph_prog, "sph_compute_step", &err);
  if (err != CL_SUCCESS)
  {
    ERROR("Failed to create compute_step kernel for SPH simulation: " << ocl::errorToStr(err));
    return false;
  }

  return true;
}


bool FluidSystem::resetSPHSimulation(unsigned int part_num)
{
  /* initialize OpenGL shared buffer with particle positions */
  if (!m_particle_pos_buf.bufferData(nullptr, part_num * sizeof(cl_float4), ocl::GLBuffer::WRITE_ONLY))
  {
    ERROR("SPH: Failed to initialize position GLBuffer");
    return false;
  }

  cl_int err = CL_SUCCESS;

#define ALLOC_BUF(buf, data_type, err_msg) \
  { \
    buf = cl::Buffer(m_cl_ctx, CL_MEM_READ_WRITE /* | CL_MEM_HOST_NO_ACCESS */, \
                     part_num * sizeof(data_type), nullptr, &err); \
    if (err != CL_SUCCESS) \
    { \
      std::cerr << err_msg << ocl::errorToStr(err) << std::endl; \
      return false; \
    } \
  }

  /* allocate buffers on GPU */
  ALLOC_BUF(m_velocity_buf, cl_float4, "SPH: Failed to allocate velocity buffer: ");
  ALLOC_BUF(m_prev_velocity_buf, cl_float4, "SPH: Failed to allocate prev velocity buffer: ");
  ALLOC_BUF(m_pressure_buf, cl_float, "SPH: Failed to allocate pressure buffer: ");
  ALLOC_BUF(m_density_buf, cl_float, "SPH: Failed to allocate density buffer: ");
  ALLOC_BUF(m_force_buf, cl_float4, "SPH: Failed to allocate force buffer: ");

#undef ALLOC_BUF

  /* set kernel parameters that do not change once the simulation is prepared */
  
  /* reset kernel arguments */
  if (!ocl::KernelArgs(m_sph_reset_kernel, "m_sph_reset_kernel")
            .arg(m_particle_pos_buf.getCLID())
            .arg(m_velocity_buf)
            .arg(m_prev_velocity_buf)
            .arg(m_pressure_buf)
            .arg(m_density_buf)
            .arg(m_force_buf)
            .arg(m_volume_min)
            .arg(m_volume_max)
            .arg((cl_ulong) (time(nullptr))))
  {
    return false;
  }

  /* run the reset kernel to initialize particle data */
  cl_command_queue queue = m_cl_queue();

  ocl::GLSyncHandler sync(queue, m_particle_pos_buf.getCLID());
  if (!sync) return false;

  err = clEnqueueNDRangeKernel(queue, m_sph_reset_kernel(), 1,
                               nullptr, &m_num_particles, nullptr,
                               0, nullptr, nullptr);
  if (err != CL_SUCCESS)
  {
    WARN("Failed to enqueue SPH reset kernel");
  }

  return true;
}


void FluidSystem::updateSPHSimulation(void)
{
  std::cerr << __FUNCTION__ << std::endl;
  return;
}


void FluidSystem::render(const glm::mat4 & mv, const glm::mat4 & proj)
{
  glEnable(GL_DEPTH_TEST);

  m_shader.use();

  glUniformMatrix4fv(glGetUniformLocation(m_shader.getID(), "proj"), 1, GL_FALSE, glm::value_ptr(proj));
  glUniformMatrix4fv(glGetUniformLocation(m_shader.getID(), "mv"), 1, GL_FALSE, glm::value_ptr(mv));

  /* calculate the normal matrix (assume only rotations, i.e only rthogonal matrices) */
  glm::mat3 mv_normal = glm::mat3(mv);   // otherwise add glm::transpose(glm::inverse(mv));
  glUniformMatrix3fv(glGetUniformLocation(m_shader.getID(), "mv_normal"), 1, GL_FALSE, glm::value_ptr(mv_normal));

  glBindVertexArray(m_particle_geom.vao);

  glBindBuffer(GL_ARRAY_BUFFER, m_particle_pos_buf.getGLID());
  glEnableVertexAttribArray(4);
  glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(cl_float4), (void *) (0));
  glVertexAttribDivisor(4, 1);

  glBindBuffer(GL_ARRAY_BUFFER, m_particle_col_buf.getGLID());
  glEnableVertexAttribArray(5);
  glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(cl_float4), (void *) (0));
  glVertexAttribDivisor(5, 1);

  glDrawArraysInstanced(m_particle_geom.mode, 0, m_particle_geom.count, m_num_particles);

  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glBindVertexArray(0);

  glUseProgram(0);

  return;
}