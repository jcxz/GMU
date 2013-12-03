#ifndef FLUIDSYSTEM_H
#define FLUIDSYSTEM_H

#include "geom.h"
#include "ocl_lib.h"

#include <glm/glm.hpp>
#include <stdexcept>



class FluidSystem
{
  public:
    enum SimulationMode {
      SIM_MODE_TEST,     // dummy test mode
      SIM_MODE_SPH,       // the actual fluid simulation mode
      SIM_MODE_IDLE       // not doing anything
    };

  public:
    FluidSystem(void)
      : m_cl_ctx(),
        m_cl_queue(),
        m_sph_prog(),
        m_test_prog(),
        m_sph_reset_kernel(),
        m_sph_compute_step_kernel(),
        m_sph_compute_force_kernel(),
        m_sph_compute_pressure_kernel(),
        m_test_kernel(),
        m_shader(),
        m_particle_geom(),
        m_particle_pos_buf(),
        m_particle_col_buf(),
        m_velocity_buf(),
        m_pressure_buf(),
        m_density_buf(),
        m_force_buf(),
        m_prev_velocity_buf(),
        m_num_particles(0),
        m_mode(SIM_MODE_TEST)
    {
      // initialize OpenCL context, compile kernels
      if (!initCL())
      {
        throw std::runtime_error("Failed to construct FluidSystem: OpenCL initialization failed");
      }

      // initialize OpenGL data structures, load models and compile shaders
      // and initialize shader data
      if (!initGL())
      {
        throw std::runtime_error("Failed to construct FluidSystem: OpenGL initialization failed");
      }
    }

    void setSimulationMode(SimulationMode mode)
    {
      m_mode = mode;
    }

    void toggleMode(void)
    {
      m_mode = SimulationMode((m_mode + 1) % int(SIM_MODE_IDLE));
    }

    // reset the particle system
    // initializes buffers and shared data
    bool reset(unsigned int part_num)
    {
      if (m_mode == SIM_MODE_TEST) return resetTestSimulation(part_num);
      if (m_mode == SIM_MODE_SPH) return resetSPHSimulation(part_num);
      return true;
    }

    // recalculate the particle system
    void update(void)
    {
      if (m_mode == SIM_MODE_TEST) return updateTestSimulation();
      if (m_mode == SIM_MODE_SPH) return updateSPHSimulation();
      return;
    }

    // render the particle system
    // @param mv model-view matrix 
    // @param proj projection matrix
    void render(const glm::mat4 & mv = glm::mat4(), const glm::mat4 & proj = glm::mat4());

    const char *modeName(void) const
    {
      if (m_mode == SIM_MODE_TEST) return "SIM_MODE_TEST";
      if (m_mode == SIM_MODE_SPH) return "SIM_MODE_SPH";
      if (m_mode == SIM_MODE_IDLE) return "SIM_MODE_IDLE";
      return "Unknown Simulation mode";
    }

  private:
    // intializes the OpenCL program and kernels for the testing mode
    bool initTestSimulation(void);
    // initializes the OpenCL program and kernel for SPH simulation
    bool initSPHSimulation(void);
    // initializes OpenCL context
    bool initCL(void);
    // intializes OpenGL (loads models and compiles shaders)
    bool initGL(void);

    // resets the the state of dummy simulation mode
    bool resetTestSimulation(unsigned int part_num);
    // resets the the state of SPH simulation mode
    bool resetSPHSimulation(unsigned int part_num);

    // updates the the state of dummy simulation mode
    void updateTestSimulation(void);
    // updates the the state of SPH simulation mode
    void updateSPHSimulation(void);

  private:
    static const char *m_vert_shader;
    static const char *m_frag_shader;
    static const char *m_vert_shader_file;
    static const char *m_frag_shader_file;

    static const char *m_test_kernel_files[];
    static const unsigned int m_test_kernel_files_size;

    static const char *m_sph_kernel_files[];
    static const unsigned int m_sph_kernel_files_size;

  private:
    // OpenCL context data
    cl::Context m_cl_ctx;          // OpenCL context
    cl::CommandQueue m_cl_queue;   // OpenCL command queue

    // OpenCL programs
    cl::Program m_sph_prog;     // OpenCL program
    cl::Program m_test_prog;    // debugging program

    // OpenCL kernels
    cl::Kernel m_sph_reset_kernel;             // a reset kernel for the SPH simulation
    cl::Kernel m_sph_compute_step_kernel;      // a kernel to compute a single SPH step
    cl::Kernel m_sph_compute_force_kernel;     // kernel for computing forces
    cl::Kernel m_sph_compute_pressure_kernel;  // kernel for computing the pressure inside of the fluid

    cl::Kernel m_test_kernel;       // testing kernel

    // OpenGL shaders
    ogl::ShaderProgram m_shader;

    // vertex buffers with sphere geometry (or the geometry of objects that will represent particles)
    geom::Model m_particle_geom;

    // memory objects with particle data
    ocl::GLBuffer m_particle_pos_buf;  // a buffer with particle positions (shared with OpenGL)
    ocl::GLBuffer m_particle_col_buf;  // a buffer with particle colors (shared with OpenGL)

    // buffers for SPH simulation
    cl::Buffer m_velocity_buf;
    cl::Buffer m_pressure_buf;
    cl::Buffer m_density_buf;
    cl::Buffer m_force_buf;
    cl::Buffer m_prev_velocity_buf;

    // point sprite textures

    // helper variables
    size_t m_num_particles;

    // simulation mode
    SimulationMode m_mode;
};

#endif