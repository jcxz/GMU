#ifndef FLUIDSYSTEM_H
#define FLUIDSYSTEM_H

#include "ParticleSystem.h"



class FluidSystem : public ParticleSystem
{
  public:
    enum Effects {
      EFFECT_DRAIN    = (1 << 0),
      EFFECT_WAVE     = (1 << 1),
      EFFECT_FOUNTAIN = (1 << 2),
      EFFECT_NONE     = (0 << 0)
    };

  public:
    FluidSystem(void)
      : ParticleSystem()
      , m_sph_prog()
      , m_sph_reset_kernel()
      , m_sph_compute_step_kernel()
      , m_sph_compute_force_kernel()
      , m_sph_compute_pressure_kernel()
      , m_velocity_buf()
      , m_pressure_buf()
      , m_density_buf()
      , m_force_buf()
      , m_prev_velocity_buf()
      , m_effects(EFFECT_NONE)
      , m_wave_start(0.0f)
      , m_rx(0)
      , m_ry(0)
    {
#if 0
      std::cerr << "this: " << (void *) this << std::endl;
      
      std::cerr << "&m_cl_ctx                      : " << &m_cl_ctx << std::endl;
      std::cerr << "&m_cl_queue                    : " << &m_cl_queue << std::endl;
      std::cerr << "&m_sph_prog                    : " << &m_sph_prog << std::endl;
      std::cerr << "&m_test_prog                   : " << &m_test_prog << std::endl;
      std::cerr << "&m_sph_reset_kernel            : " << &m_sph_reset_kernel << std::endl;
      std::cerr << "&m_sph_compute_step_kernel     : " << &m_sph_compute_step_kernel << std::endl;
      std::cerr << "&m_sph_compute_force_kernel    : " << &m_sph_compute_force_kernel << std::endl;
      std::cerr << "&m_sph_compute_pressure_kernel : " << &m_sph_compute_pressure_kernel << std::endl;
      std::cerr << "&m_test_kernel                 : " << &m_test_kernel << std::endl;
      std::cerr << "&m_shader                      : " << &m_shader << std::endl;
      std::cerr << "&m_particle_geom               : " << &m_particle_geom << std::endl;
      std::cerr << "&m_particle_pos_buf            : " << &m_particle_pos_buf << std::endl;
      std::cerr << "&m_particle_col_buf            : " << &m_particle_col_buf << std::endl;
      std::cerr << "&m_velocity_buf                : " << &m_velocity_buf << std::endl;
      std::cerr << "&m_pressure_buf                : " << &m_pressure_buf << std::endl;
      std::cerr << "&m_density_buf                 : " << &m_density_buf << std::endl;
      std::cerr << "&m_force_buf                   : " << &m_force_buf << std::endl;
      std::cerr << "&m_prev_velocity_buf           : " << &m_prev_velocity_buf << std::endl;
      std::cerr << "&m_num_particles               : " << &m_num_particles << std::endl;
      std::cerr << "&m_volume_min                  : " << &m_volume_min << std::endl;
      std::cerr << "&m_volume_max                  : " << &m_volume_max << std::endl;
      std::cerr << "&m_mode                        : " << &m_mode << std::endl;
#endif
      // initialize OpenCL context, compile kernels
      if (!init())
      {
        throw std::runtime_error("Failed to construct FluidSystem: OpenCL initialization failed");
      }

      m_use_uniform_color = true;
    }

    void activateDrain(void) { deactivateFountain(); m_effects |= EFFECT_DRAIN; }
    void deactivateDrain(void) { m_effects &= ~(EFFECT_DRAIN); }
    bool toggleDrain(void) { deactivateFountain(); return (m_effects ^= EFFECT_DRAIN) != 0; }

    void emitWave(void) { m_effects |= EFFECT_WAVE; m_wave_start = m_time; }

    void activateFountain(void) { deactivateDrain(); m_effects |= EFFECT_FOUNTAIN; }
    void deactivateFountain(void) { m_effects &= ~(EFFECT_FOUNTAIN); }
    bool toggleFountain(void) { deactivateDrain(); return (m_effects ^= EFFECT_FOUNTAIN) != 0; }

    void setRotation(float rx, float ry) { m_rx = rx; m_ry = ry; }

    // reset the particle system
    // initializes buffers and shared data
    virtual bool reset(unsigned int part_num);

    // recalculate the particle system
    virtual void update(float time_step = 1.0f);

    // render the particle system
    // @param mv model-view matrix 
    // @param proj projection matrix
    void render(const glm::mat4 & mv = glm::mat4(), const glm::mat4 & proj = glm::mat4());

  private:
    // initializes the OpenCL program and kernel for SPH simulation
    bool init(void);

  private:
    static const char *m_sph_kernel_files[];
    static const unsigned int m_sph_kernel_files_size;

  private:
    // OpenCL programs
    cl::Program m_sph_prog;     // OpenCL program

    // OpenCL kernels
    cl::Kernel m_sph_reset_kernel;             // a reset kernel for the SPH simulation
    cl::Kernel m_sph_compute_step_kernel;      // a kernel to compute a single SPH step
    cl::Kernel m_sph_compute_force_kernel;     // kernel for computing forces
    cl::Kernel m_sph_compute_pressure_kernel;  // kernel for computing the pressure inside of the fluid

    // buffers for SPH simulation
    cl::Buffer m_velocity_buf;
    cl::Buffer m_pressure_buf;
    cl::Buffer m_density_buf;
    cl::Buffer m_force_buf;
    cl::Buffer m_prev_velocity_buf;

    // simulation settings
    unsigned int m_effects;
    float m_wave_start;
    int m_rx;
    int m_ry;
};

#endif