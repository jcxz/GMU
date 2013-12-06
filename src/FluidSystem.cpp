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




namespace {

cl_float4 calcGravitationVector(int rx, int ry)
{
  double dir_s, dir_c, mult_x, mult_y, mult_z;
  int corr_z = 1;
  
  double sinx = sin((double) rx * (3.141592f / 180.0f));
  if (sinx < 0)
  {
    dir_s = sinx;
    dir_c = 1+sinx;
  }
  else
  {
    dir_s = sinx;
    dir_c = 1-sinx;
  }

  if (abs(rx) % 360 > 90 && abs(rx) % 360 < 270)
  //if (fmod(abs(rx), 360) > 90 && fmod(abs(rx), 360) < 270)
  {
      corr_z = -1;
  }
  
  mult_y = cos((double) ry * (3.141592f / 180.0f));
  mult_x = sin((double) ry * (3.141592f / 180.0f)) * dir_s;
  mult_z = sin((double) ry * (3.141592f / 180.0f)) * dir_c;
  
  cl_float4 F;
  
  F.s[0] = mult_x * -9.81f;
  F.s[1] = mult_y * -9.81f;
  F.s[2] = corr_z * mult_z * 9.81f;
  F.s[3] = 0.0f;

  return F; /* vektor gravitace */
}

}



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
  
#define SIM_SCALE ((cl_float) (0.004f))
#define SMOOTH_RADIUS ((cl_float) (0.01f))
#define RADIUS2 ((cl_float) ((SMOOTH_RADIUS) * (SMOOTH_RADIUS)))

// m = Ro * (V / n)
// Ro   ... hustota tekutiny
// V    ... objem
// n    ... pocet castic
#define MASS ((cl_float) (0.00020543f))
#define POLYKERN ((cl_float) (315.0f / (64.0f * 3.141592 * pow(SMOOTH_RADIUS, 9))))
#define RESTDENSITY ((cl_float) (600.0f))
#define INTSTIFFNESS ((cl_float) (1.0f))
#define MASS_POLYKERN ((cl_float) ((MASS) * (POLYKERN)))

#define VISCOSITY ((cl_float) (0.2f))
#define LAPKERN ((cl_float) (45.0f / (3.141592 * pow(SMOOTH_RADIUS, 6))))
#define VTERM ((cl_float) ((LAPKERN) * (VISCOSITY)))
#define SPIKEYKERN ((cl_float) (-45.0f / (3.141592 * pow(SMOOTH_RADIUS, 6))))
#define SPIKEYKERN_HALF ((cl_float) ((SPIKEYKERN) * (-0.5f)))

#define SLOPE ((cl_float) (0.0f))        // ???
#define LEFTWAVE ((cl_float) (0.0f))     // ???
#define RIGHTWAVE ((cl_float) (0.0f))   // ???
#define DELTATIME ((cl_float) (.003f))   // ???
#define LIMIT ((cl_float) (200.0f))
#define EXTSTIFFNESS ((cl_float) (10000.0f))
#define EXTDAMPING ((cl_float) (256.0f))
#define RADIUS ((cl_float) (0.004f))

  /* compute pressure kernel's arguments */
  if (!ocl::KernelArgs(m_sph_compute_pressure_kernel, "m_sph_compute_pressure_kernel")
            .arg(m_particle_pos_buf.getCLID())
            .arg(m_density_buf)
            .arg(m_pressure_buf)
            .arg(SIM_SCALE)
            //.arg(SMOOTH_RADIUS)
            .arg(RADIUS2)
            //.arg(MASS)
            //.arg(POLYKERN)
            .arg(MASS_POLYKERN)
            .arg(RESTDENSITY)
            .arg(INTSTIFFNESS)
            .arg((unsigned int) (m_num_particles)))
  {
    return false;
  }

  /* compute force kernel's arguments */
  if (!ocl::KernelArgs(m_sph_compute_force_kernel, "m_sph_compute_force_kernel")
            .arg(m_particle_pos_buf.getCLID())
            .arg(m_density_buf)
            .arg(m_pressure_buf)
            .arg(m_force_buf)
            .arg(m_velocity_buf)
            .arg(SIM_SCALE)
            .arg(SMOOTH_RADIUS)
            .arg(RADIUS2)
            //.arg(VISCOSITY)
            //.arg(LAPKERN)
            .arg(VTERM)
            .arg(SPIKEYKERN_HALF)
            .arg((unsigned int) (m_num_particles)))
  {
    return false;
  }

  /* compute step kernel's arguments */
  if (!ocl::KernelArgs(m_sph_compute_step_kernel, "m_sph_compute_step_kernel")
            .arg(m_particle_pos_buf.getCLID())
            .arg(m_force_buf)
            .arg(m_velocity_buf)
            .arg(m_prev_velocity_buf)
            .arg(SLOPE)
            .arg(LEFTWAVE)
            .arg(RIGHTWAVE)
            .arg(DELTATIME)
            .arg(LIMIT)
            .arg(EXTSTIFFNESS)
            .arg(EXTDAMPING)
            .arg(RADIUS)
            .arg(m_volume_min)
            .arg(m_volume_max)
            .arg(SIM_SCALE)
            .arg(MASS))
  {
    return false;
  }

  /* reset kernel's arguments */
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


void FluidSystem::update(float time_step)
{
  // check if the simulation is not paused
  if (m_pause) return;

  /* set kernel arguments that change every frame */
  cl_int err = m_sph_compute_step_kernel.setArg(16, (cl_float) (m_time));
  if (err != CL_SUCCESS)
  {
    WARN("FluidSystem: Failed to set time argument: " << ocl::errorToStr(err));
    return;
  }

  err = m_sph_compute_step_kernel.setArg(17, (cl_uint) (m_effects));
  if (err != CL_SUCCESS)
  {
    WARN("FluidSystem: Failed to set flags argument: " << ocl::errorToStr(err));
    return;
  }

  //err = m_sph_compute_step_kernel.setArg(18, calcGravitationVector(m_rx, m_ry));
  //if (err != CL_SUCCESS)
  //{
  //  WARN("FluidSystem: Failed to set gravitation argument: " << ocl::errorToStr(err));
  //  return;
  //}

  /* synchronise with OpenGL */
  cl_command_queue queue = m_cl_queue();
  cl_mem buffers[] = { m_particle_pos_buf.getCLID() };

  ocl::GLSyncHandler sync(queue, FLUIDSIM_COUNT(buffers), buffers);
  if (!sync) return;

  /* compute pressure */
  err = clEnqueueNDRangeKernel(queue, m_sph_compute_pressure_kernel(), 1,
                               nullptr, &m_num_particles, nullptr,
                               0, nullptr, nullptr);
  if (err != CL_SUCCESS)
  {
    WARN("Failed to enqueue test simulation kernel: " << ocl::errorToStr(err));
  }

  /* compute force */
  err = clEnqueueNDRangeKernel(queue, m_sph_compute_force_kernel(), 1,
                               nullptr, &m_num_particles, nullptr,
                               0, nullptr, nullptr);
  if (err != CL_SUCCESS)
  {
    WARN("Failed to enqueue test simulation kernel: " << ocl::errorToStr(err));
  }

  /* integrate */
  err = clEnqueueNDRangeKernel(queue, m_sph_compute_step_kernel(), 1,
                               nullptr, &m_num_particles, nullptr,
                               0, nullptr, nullptr);
  if (err != CL_SUCCESS)
  {
    WARN("Failed to enqueue test simulation kernel: " << ocl::errorToStr(err));
  }

  /* advance simulation time */
  m_time += time_step;   // 3.0f;

  // kill the wave after 50 frames
  if (m_effects & EFFECT_WAVE)
  {
    if ((m_time - m_wave_start) > (time_step * 50))
    {
      m_effects &= ~(EFFECT_WAVE);
    }
  }

  return;
}