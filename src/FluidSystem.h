#ifndef FLUIDSYSTEM_H
#define FLUIDSYSTEM_H

#include "geom.h"
#include "ocl_lib.h"

#include <glm/glm.hpp>
#include <stdexcept>



struct Particle
{
  glm::vec3 pos;   /// position of the particle
};


class FluidSystem
{
  public:
    FluidSystem(void)
      : m_cl_ctx(),
        m_cl_queue(),
        m_cl_prog(),
        m_part_pos_kernel(),
        m_shader(),
        m_particle_geom(),
        m_particle_buf(),
        m_num_particles(0)
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

    // reset the particle system
    // initializes buffers and shared data
    bool reset(unsigned int part_num);

    // recalculate the particle system
    void update(void);

    // render the particle system
    // @param mvp model-view-projection matrix 
    void render(const glm::mat4 & mvp = glm::mat4());

  private:
    // initializes OpenCL context
    bool initCL(void);

    // intializes OpenGL (loads models and compiles shaders)
    bool initGL(void);

  private:
    static const char *m_vert_shader;
    static const char *m_frag_shader;
    static const char *m_part_pos_kernel_src;

  private:
    // OpenCL context data
    cl::Context m_cl_ctx;          // OpenCL context
    cl::CommandQueue m_cl_queue;   // OpenCL command queue
    cl::Program m_cl_prog;         // OpenCL program

    // OpenCL kernels
    cl::Kernel m_part_pos_kernel;

    // OpenGL shaders
    ogl::ShaderProgram m_shader;

    // vertex buffers with sphere geometry (or the geometry of objects that will represent particles)
    geom::Model m_particle_geom;

    // memory objects with particle data
    ocl::GLBuffer m_particle_buf;

    // point sprite textures

    // helper variables
    size_t m_num_particles;
};

#endif