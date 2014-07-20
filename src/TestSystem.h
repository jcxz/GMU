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

#ifndef TESTSYSTEM_H
#define TESTSYSTEM_H

#include "ParticleSystem.h"



/**
 * A dummy debugging particle system
 */
class TestSystem : public ParticleSystem
{
  public:
    TestSystem(void)
      : ParticleSystem()
      , m_test_prog()
      , m_test_kernel()
      , m_polar_spiral_kernel()
      , m_spiral(true)
    {
      std::cerr << __FUNCTION__ << std::endl;

      // initialize OpenCL context, compile kernels
      if (!init())
      {
        throw std::runtime_error("Failed to construct TestSystem: OpenCL initialization failed");
      }
    }

    bool spiral(void) const { return m_spiral; }
    void setSpiral(bool spiral = true) { m_spiral = spiral; }
    bool toggleSpiral(void) { return m_spiral = !m_spiral; }

    // reset the particle system
    // initializes buffers and shared data
    virtual bool reset(unsigned int part_num);

    // recalculate the particle system
    virtual void update(float time_step = 1.0f);

  private:
    // initializes OpenCL context
    bool init(void);

  private:
    static const char *m_test_kernel_files[];
    static const unsigned int m_test_kernel_files_size;

  private:
    // OpenCL programs
    cl::Program m_test_prog;    // debugging program

    // OpenCL kernels
    cl::Kernel m_test_kernel;          // testing kernel
    cl::Kernel m_polar_spiral_kernel;  // a kernel to generate polar spiral

    bool m_spiral;  // whether to generate archimedean spiral or just random positions
};

#endif