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
      : ParticleSystem(),
        m_test_prog(),
        m_test_kernel()
    {
      std::cerr << __FUNCTION__ << std::endl;

      // initialize OpenCL context, compile kernels
      if (!init())
      {
        throw std::runtime_error("Failed to construct TestSystem: OpenCL initialization failed");
      }
    }

    // reset the particle system
    // initializes buffers and shared data
    virtual bool reset(unsigned int part_num);

    // recalculate the particle system
    virtual void update(void);

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
    cl::Kernel m_test_kernel;       // testing kernel
};

#endif