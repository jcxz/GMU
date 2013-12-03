#include "FluidSystem.h"
#include "global.h"
#include "debug.h"
#include "utils.h"

#include <glm/gtc/type_ptr.hpp>
#include <ctime>




const char *FluidSystem::m_sph_kernel_files[] = {
  "/src/OpenCL/sph_reset.cl",
  "/src/OpenCL/sph_compute_pressure.cl",
  "/src/OpenCL/sph_compute_force.cl",
  "/src/OpenCL/sph_compute_step.cl"
};

const unsigned int FluidSystem::m_sph_kernel_files_size = sizeof(m_sph_kernel_files) /
                                                          sizeof(*m_sph_kernel_files);





bool FluidSystem::init(void)
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


bool FluidSystem::reset(unsigned int part_num)
{
  std::cerr << __FUNCTION__ << std::endl;

  m_num_particles = part_num;

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

#if 1
  cl_mem buffers[] = { m_particle_pos_buf.getCLID() };
  ocl::GLSyncHandler sync(queue, FLUIDSIM_COUNT(buffers), buffers);
  if (!sync) return false;
#else
  ocl::GLSyncHandler sync(queue, m_particle_pos_buf.getCLID());
  if (!sync) return false;
#endif

  err = clEnqueueNDRangeKernel(queue, m_sph_reset_kernel(), 1,
                               nullptr, &m_num_particles, nullptr,
                               0, nullptr, nullptr);
  if (err != CL_SUCCESS)
  {
    WARN("Failed to enqueue SPH reset kernel");
  }

  return true;
}


void FluidSystem::update(void)
{
  //std::cerr << __FUNCTION__ << std::endl;
  return;
}