#ifndef FLUIDSYSTEM_H
#define FLUIDSYSTEM_H

#include "geom.h"

#include <glm/glm.hpp>


struct Particle
{
  glm::vec3 pos;   /// position of the particle
};


class FluidSystem
{
  public:
    
    // initialize shaders and OpenCL
    bool init(void);

    // set the number of particles
    void setParticleCount(unsigned int part_num);

    // reset the particle system
    void reset(void);

    // recalculate the particle system
    void update(void);

    // render the particel system
    void render(void);

  private:
    // intializes OpenGL (loads models and compiles shaders)
    bool initGL(void);

    // initializes OpenCL context
    bool initCL(void);

  private:
    // OpenCL kernels
    // OpenGL shaders

    // vertex buffers with sphere geometry (or the geometry of objects that will represent particles)
    geom::Model m_particle_geom;

    // point sprite textures

    // memory objects with particle data
};

#endif