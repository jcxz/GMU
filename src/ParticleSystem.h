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
      , m_num_particles(0)
      , m_time(0.0f)
      , m_volume_min()
      , m_volume_max()
      , m_use_uniform_color(false)
      , m_draw_bounding_volume(true)
      , m_pause(false)
      , m_stats()
    {    
      // initialize bounding volume
      m_volume_min.s[0] = -15.0f; m_volume_min.s[1] = -15.0f; m_volume_min.s[2] = -15.0f; m_volume_min.s[3] = 1.0f;
      m_volume_max.s[0] =  15.0f; m_volume_max.s[1] =  15.0f; m_volume_max.s[2] =  15.0f; m_volume_max.s[3] = 1.0f;

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
    }

    virtual ~ParticleSystem(void)
    {
      std::cerr << "Performance statistics:\n" <<  m_stats << std::endl;
    }

    bool toggleDrawBoundingVolume(void)
    {
      return m_draw_bounding_volume = !m_draw_bounding_volume;
    }

    bool togglePause(void) { return m_pause = !m_pause; }

    // reset the particle system
    // initializes buffers and shared data
    virtual bool reset(unsigned int part_num) = 0;

    // recalculate the particle system
    virtual void update(float time_step = 1.0f) = 0;

    // render the particle system
    // @param mv model-view matrix 
    // @param proj projection matrix
    void render(const glm::mat4 & mv = glm::mat4(), const glm::mat4 & proj = glm::mat4());

  private:
    // initializes OpenCL context
    bool initCL(void);
    // intializes OpenGL (loads models and compiles shaders)
    bool initGL(void);

    void drawParticles(void);
    void drawBoundingVolume(void);

  private:
    static const char *m_vert_shader;
    static const char *m_frag_shader;

    static const char *m_vert_shader_particle_colors_file;
    static const char *m_frag_shader_particle_colors_file;

    static const char *m_vert_shader_uniform_color_file;
    static const char *m_frag_shader_uniform_color_file;

    static const char *m_vert_shader_bounding_volume_file;
    static const char *m_frag_shader_bounding_volume_file;

  private:
    // OpenGL shaders
    ogl::ShaderProgram m_shader_particle_colors;
    ogl::ShaderProgram m_shader_uniform_color;
    ogl::ShaderProgram m_shader_bounding_volume;

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

    // helper variables for simulation
    size_t m_num_particles;      // number of particles in simulation
    cl_float m_time;             // simulation time
    cl_float4 m_volume_min;      // bounding volume minimum corner
    cl_float4 m_volume_max;      // bounding volume maximum corner

    // rendering options
    bool m_use_uniform_color;     // whether to use the same color for all particles or per particle color
    bool m_draw_bounding_volume;  // whether to display bounding volume or not
    bool m_pause;                 // whether to pause simulation

    // statistics
    ocl::PerfStats m_stats;
};

#endif