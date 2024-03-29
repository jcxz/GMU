/*
 * Copyright (C) 2014 Matus Fedorko <xfedor01@stud.fit.vutbr.cz>
 *
 * This software is provided 'as-is', without any express or implied
 * warranty.  In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:

 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 */

#include "TestSystem.h"
#include "global.h"
#include "debug.h"
#include "utils.h"

#include <ctime>





//const char *FluidSystem::m_part_pos_kernel_file = "/src/OpenCL/gen_rand_particles.cl";

const char *TestSystem::m_test_kernel_files[] = {
  "/src/OpenCL/gen_rand_particles.cl",
  "/src/OpenCL/polar_spiral.cl"
};

const unsigned int TestSystem::m_test_kernel_files_size =  sizeof(m_test_kernel_files) /
                                                           sizeof(*m_test_kernel_files);



bool TestSystem::init(void)
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

  m_polar_spiral_kernel = cl::Kernel(m_test_prog, "polar_spiral", &err);
  if (err != CL_SUCCESS)
  {
    ERROR("Failed to create polar spiral generator kernel: " << ocl::errorToStr(err));
    return false;
  }

  return true;
}


bool TestSystem::reset(unsigned int part_num)
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
            .arg(m_particle_col_buf.getCLID()))
            //.arg((cl_ulong) (time(nullptr))))
  {
    return false;
  }

  cl_float3 position = { 0.0f, 0.0f, 0.0f };
  if (!ocl::KernelArgs(m_polar_spiral_kernel, "m_polar_spiral_kernel")
            .arg(m_particle_pos_buf.getCLID())
            .arg(m_particle_col_buf.getCLID())
            .arg(position))
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


void TestSystem::update(float time_step)
{
  cl_int err = CL_SUCCESS;

  if (m_spiral)
    err = m_polar_spiral_kernel.setArg(3, cl_ulong(time(nullptr) + m_time));
  else
    err = m_test_kernel.setArg(2, cl_ulong(time(nullptr) + m_time));

  if (err != CL_SUCCESS)
  {
    WARN("TestSystem: Failed to set seed argument: " << ocl::errorToStr(err));
    return;
  }

  cl_command_queue queue = m_cl_queue();
  cl_mem buffers[] = { m_particle_pos_buf.getCLID(), m_particle_col_buf.getCLID() };

  ocl::GLSyncHandler sync(queue, FLUIDSIM_COUNT(buffers), buffers);
  if (!sync) return;

  if (m_spiral)
  {
    err = clEnqueueNDRangeKernel(queue, m_polar_spiral_kernel(), 1,
                                 nullptr, &m_num_particles, nullptr,
                                 0, nullptr, m_stats.event("polar_spiral"));
  }
  else
  {
    err = clEnqueueNDRangeKernel(queue, m_test_kernel(), 1,
                                 nullptr, &m_num_particles, nullptr,
                                 0, nullptr, m_stats.event("gen_part_positions"));
  }

  if (err != CL_SUCCESS)
  {
    WARN("Failed to enqueue test simulation kernel: " << ocl::errorToStr(err));
  }

  m_time += time_step;   // 3.0f;

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