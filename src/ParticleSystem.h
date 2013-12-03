#ifndef PARTICLESYSTEM_H
#define PARTICLESYSTEM_H

#include "geom.h"
#include "ocl_lib.h"

#include <glm/glm.hpp>
#include <stdexcept>



/**
 * Common base class for all particle systems
 */
class ParticleSystem
{
  public:
    ParticleSystem(void)
      : m_shader_particle_colors()
      , m_shader_uniform_color()
      , m_particle_geom()
      , m_cl_ctx()
      , m_cl_queue()
      , m_particle_pos_buf()
      , m_particle_col_buf()
      , m_num_particles()
      , m_volume_min()
      , m_volume_max()
      , m_use_uniform_color(false)
    {
#if 0
      std::cerr << __FUNCTION__ << std::endl;

      std::cerr << "this                : " << (void *) this << std::endl;
      
      std::cerr << "&m_shader           : " << (void *) &m_shader << std::endl;
      std::cerr << "&m_particle_geom    : " << (void *) &m_particle_geom << std::endl;
      std::cerr << "&m_cl_ctx           : " << (void *) &m_cl_ctx << std::endl;
      std::cerr << "&m_cl_queue         : " << (void *) &m_cl_queue << std::endl;
      std::cerr << "&m_particle_pos_buf : " << (void *) &m_particle_pos_buf << std::endl;
      std::cerr << "&m_particle_col_buf : " << (void *) &m_particle_col_buf << std::endl;
      std::cerr << "&m_num_particles    : " << (void *) &m_num_particles << std::endl;
      std::cerr << "&m_volume_min       : " << (void *) &m_volume_min << std::endl;
      std::cerr << "&m_volume_max       : " << (void *) &m_volume_max << std::endl;

      std::cerr << "m_cl_ctx()          : " << m_cl_ctx() << std::endl;
      std::cerr << "m_cl_queue()        : " << m_cl_queue() << std::endl;
#endif
      // initialize OpenCL context, compile kernels
      if (!initCL())
      {
        throw std::runtime_error("Failed to construct ParticleSystem: OpenCL initialization failed");
      }

      // initialize OpenGL data structures, load models and compile shaders
      // and initialize shader data
      if (!initGL())
      {
        throw std::runtime_error("Failed to construct ParticleSystem: OpenGL initialization failed");
      }

      // initialize bounding volume
      m_volume_min.s[0] = -15.0f; m_volume_min.s[1] = -15.0f; m_volume_min.s[2] = -15.0f; m_volume_min.s[3] = 1.0f;
      m_volume_max.s[0] =  15.0f; m_volume_max.s[1] =  15.0f; m_volume_max.s[2] =  15.0f; m_volume_max.s[3] = 1.0f;
    }

    virtual ~ParticleSystem(void) { }

    // reset the particle system
    // initializes buffers and shared data
    virtual bool reset(unsigned int part_num) = 0;

    // recalculate the particle system
    virtual void update(void) = 0;

    // render the particle system
    // @param mv model-view matrix 
    // @param proj projection matrix
    void render(const glm::mat4 & mv = glm::mat4(), const glm::mat4 & proj = glm::mat4());

  private:
    // initializes OpenCL context
    bool initCL(void);
    // intializes OpenGL (loads models and compiles shaders)
    bool initGL(void);

  private:
    static const char *m_vert_shader;
    static const char *m_frag_shader;

    static const char *m_vert_shader_particle_colors_file;
    static const char *m_frag_shader_particle_colors_file;

    static const char *m_vert_shader_uniform_color_file;
    static const char *m_frag_shader_uniform_color_file;

  private:
    // OpenGL shaders
    ogl::ShaderProgram m_shader_particle_colors;
    ogl::ShaderProgram m_shader_uniform_color;

    // vertex buffers with sphere geometry (or the geometry of objects that will represent particles)
    geom::Model m_particle_geom;
    
    // point sprite textures

  protected:
    // OpenCL context data
    cl::Context m_cl_ctx;          // OpenCL context
    cl::CommandQueue m_cl_queue;   // OpenCL command queue

    // memory objects with particle data
    ocl::GLBuffer m_particle_pos_buf;  // a buffer with particle positions (shared with OpenGL)
    ocl::GLBuffer m_particle_col_buf;  // a buffer with particle colors (shared with OpenGL)

    // helper variables
    size_t m_num_particles;      // number of particles in simulation
    cl_float4 m_volume_min;      // bounding volume minimum corner
    cl_float4 m_volume_max;      // bounding volume maximum corner

    // rendering settings
    bool m_use_uniform_color;    // whether to use the same color for all particles or per particle color
};

#endif