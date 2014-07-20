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

#include "ParticleSystem.h"
#include "global.h"
#include "debug.h"
#include "utils.h"

#include <glm/gtc/type_ptr.hpp>
#include <ctime>



const char *ParticleSystem::m_vert_shader =
  "#version 330\n"
  ""
  "uniform mat4 mvp;"
  ""
  "layout(location = 0) in vec3 pos;"           // model's vertex position
  "layout(location = 1) in vec3 normal;"        // model's normal
  "layout(location = 4) in vec3 particle_pos;"  // particle's position
  "layout(location = 5) in vec4 particle_col;"  // particle's color
  ""
  ""
  "out vec3 o_normal;"
  "out vec4 o_particle_col;"
  ""
  "void main(void)"
  "{"
  "  gl_Position = mvp * vec4(pos + particle_pos, 1.0f);"
  "  o_normal = normal;"
  "  o_particle_col = particle_col;"
  //"  gl_Position = 0.005 * vec4(pos + particle_pos, 1.0f);"
  //"  gl_Position = vec4((pos + particle_pos) * 0.02, 1.0f);"
  "}"
;

const char *ParticleSystem::m_frag_shader =
  "#version 330\n"
  ""
  "uniform vec3 light_pos;"
  "uniform vec4 light_col_a;"
  "uniform vec4 light_col_k;"
  "uniform vec4 light_col_s;"
  ""
  "in vec3 o_normal;"
  "in vec4 o_particle_col;"
  ""
  "void main(void)"
  "{"
  //"  gl_FragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);"
  "  float dp = dot(o_normal, light_pos);"
  "  if (dp < 0.0f) dp = 0.0f;"
  "  gl_FragColor = light_col_a * o_particle_col + light_col_k * o_particle_col * dp;"
  "}"
;


const char *ParticleSystem::m_vert_shader_particle_colors_file = "src/OpenGL/ParticleSystem_particle_colors.vert";
const char *ParticleSystem::m_frag_shader_particle_colors_file = "src/OpenGL/ParticleSystem_particle_colors.frag";

const char *ParticleSystem::m_vert_shader_uniform_color_file = "src/OpenGL/ParticleSystem_uniform_color.vert";
const char *ParticleSystem::m_frag_shader_uniform_color_file = "src/OpenGL/ParticleSystem_uniform_color.frag";

const char *ParticleSystem::m_vert_shader_bounding_volume_file = "src/OpenGL/ParticleSystem_bounding_volume.vert";
const char *ParticleSystem::m_frag_shader_bounding_volume_file = "src/OpenGL/ParticleSystem_bounding_volume.frag";



bool ParticleSystem::initCL(void)
{
  INFO("Initializing OpenCL subsystem");

#if 0
  std::cerr << "this                : " << (void *) this << std::endl;
      
  std::cerr << "&m_shader           : " << (void *) &m_shader << std::endl;
  std::cerr << "&m_particle_geom    : " << (void *) &m_particle_geom << std::endl;
  std::cerr << "&m_cl_ctx           : " << (void *) &m_cl_ctx << std::endl;
  std::cerr << "&m_cl_queue         : " << (void *) &m_cl_queue << std::endl;
  std::cerr << "&m_particle_pos_buf : " << (void *) &m_particle_pos_buf << std::endl;
  std::cerr << "&m_particle_col_buf : " << (void *) &m_particle_col_buf << std::endl;
  std::cerr << "&m_num_particles    : " << (void *) &m_num_particles << std::endl;
  //std::cerr << "&m_volume_min       : " << (void *) &m_volume_min << std::endl;
  //std::cerr << "&m_volume_max       : " << (void *) &m_volume_max << std::endl;
  //
  std::cerr << "m_cl_ctx()          : " << m_cl_ctx() << std::endl;
  std::cerr << "m_cl_queue()        : " << m_cl_queue() << std::endl;
#endif

  /* select appropriate device and platform */
  cl_platform_id platform = nullptr;
  cl_device_id device = nullptr;

  if ((!ocl::selectGLDeviceAndPlatform(&device, &platform)) &&
      (!ocl::selectPlatformAndDevice(&device, &platform)))
  {
    ERROR("Failed to select an appropriate device or platform");
    return false;
  }

  //std::cerr << "m_cl_ctx: " << m_cl_ctx() << std::endl;
  //std::cerr << "m_cl_queue: " << m_cl_queue() << std::endl;

  std::vector<cl::Device> device_list(1, device);

  /* setup context */
  cl_context_properties props[] = {
#if defined(FLUIDSIM_OS_MAC)
    CL_CONTEXT_PROPERTY_USE_CGL_SHAREGROUP_APPLE,
    (cl_context_properties) CGLGetShareGroup(CGLGetCurrentContext()),
#elif defined(FLUIDSIM_OS_UNIX)
    CL_GL_CONTEXT_KHR, (cl_context_properties) glXGetCurrentContext(), 
    CL_GLX_DISPLAY_KHR, (cl_context_properties) glXGetCurrentDisplay(), 
    CL_CONTEXT_PLATFORM, (cl_context_properties) platform,
#elif defined(FLUIDSIM_OS_WIN)
    CL_GL_CONTEXT_KHR, (cl_context_properties) wglGetCurrentContext(),
    CL_WGL_HDC_KHR, (cl_context_properties) wglGetCurrentDC(),
    CL_CONTEXT_PLATFORM, (cl_context_properties) platform,
#else
# error "Unsupported OS platform"
#endif
    0
  };

  cl_int err = CL_SUCCESS;
  m_cl_ctx = cl::Context(device_list, props, nullptr, nullptr, &err);
  if (err != CL_SUCCESS)
  {
    ERROR("Failed to create OpenCL context: " << ocl::errorToStr(err));
    return false;
  }

  /* create command queue */
  m_cl_queue = cl::CommandQueue(m_cl_ctx, device, CL_QUEUE_PROFILING_ENABLE, &err);
  if (err != CL_SUCCESS)
  {
    ERROR("Failed to create OpenCL command queue: " << ocl::errorToStr(err));
    return false;
  }

  /* pass the context pointer to OpenGL shared buffers */
  m_particle_pos_buf.setCLContext(m_cl_ctx());
  m_particle_col_buf.setCLContext(m_cl_ctx());

  INFO("Successfully initialized OpenCL context and command queue");

  return true;
}


bool ParticleSystem::initGL(void)
{
  INFO("Initializing OpenGL subsystem");

  /* compile shaders */
  //if (!m_shader.build(m_vert_shader, m_frag_shader))
  if (!m_shader_particle_colors.buildFiles(utils::fs::AssetsPath(m_vert_shader_particle_colors_file),
                                           utils::fs::AssetsPath(m_frag_shader_particle_colors_file)))
  {
    ERROR("Failed to compile shaders for particle colors program");
    return false;
  }

  if (!m_shader_uniform_color.buildFiles(utils::fs::AssetsPath(m_vert_shader_uniform_color_file),
                                         utils::fs::AssetsPath(m_frag_shader_uniform_color_file)))
  {
    ERROR("Failed to compile shaders for uniform color program");
    return false;
  }

  if (!m_shader_bounding_volume.buildFiles(utils::fs::AssetsPath(m_vert_shader_bounding_volume_file),
                                           utils::fs::AssetsPath(m_frag_shader_bounding_volume_file)))
  {
    ERROR("Failed to compile shaders for bounding volume program");
    return false;
  }

  /* per particle colors program */
  GLuint pc_prog = m_shader_particle_colors.getID();
  
  glUseProgram(pc_prog);
  //glUniform3f(glGetUniformLocation(prog, "light_pos"), 0.0f, 0.0f, -1.0f);     // shader assumes the position is normalized
  //glUniform3f(glGetUniformLocation(prog, "light_pos"), -1.0f, 1.0f, -1.0f);
  glUniform3f(glGetUniformLocation(pc_prog, "light_pos"), -10.0f, 10.0f, 15.0f);
  glUniform3f(glGetUniformLocation(pc_prog, "light_col_a"), 0.8f, 0.8f, 0.8f);
  glUniform3f(glGetUniformLocation(pc_prog, "light_col_d"), 1.0f, 1.0f, 1.0f);
  glUniform3f(glGetUniformLocation(pc_prog, "light_col_s"), 1.0f, 1.0f, 1.0f);

  /* uniform color program */
  GLuint uc_prog = m_shader_uniform_color.getID();
  
  glUseProgram(uc_prog);
  glUniform3f(glGetUniformLocation(uc_prog, "light_pos"), -10.0f, 10.0f, 15.0f);
  glUniform3f(glGetUniformLocation(uc_prog, "light_col_a"), 0.8f, 0.8f, 0.8f);
  glUniform3f(glGetUniformLocation(uc_prog, "light_col_d"), 1.0f, 1.0f, 1.0f);
  glUniform3f(glGetUniformLocation(uc_prog, "light_col_s"), 1.0f, 1.0f, 1.0f);
  glUniform3f(glGetUniformLocation(uc_prog, "particle_col"), 0.5f, 0.5f, 1.0f);

  /* bounding volume program */
  GLuint bv_prog = m_shader_bounding_volume.getID();

  glUseProgram(bv_prog);

  glUniform3f(glGetUniformLocation(bv_prog, "dimensions"),
              (m_volume_max.s[0] - m_volume_min.s[0]) * 0.5f,
              (m_volume_max.s[1] - m_volume_min.s[1]) * 0.5f,
              (m_volume_max.s[2] - m_volume_min.s[2]) * 0.5f);
  glUniform3f(glGetUniformLocation(bv_prog, "col"), 1.0f, 1.0f, 1.0f);

  glUseProgram(0);

  /* load models */
  if (!geom::genSphere(m_particle_geom))
  //if (!geom::genPrism(m_particle_geom))
  {
    ERROR("Failed to generate sphere model");
    return false;
  }

  return true;
}


void ParticleSystem::render(const glm::mat4 & mv, const glm::mat4 & proj)
{
  glEnable(GL_DEPTH_TEST);

  /* render particle system */
  glBindVertexArray(m_particle_geom.vao);

  GLuint shader_id = 0;

  if (m_use_uniform_color)
  {
    m_shader_uniform_color.use();
    shader_id = m_shader_uniform_color.getID();
  }
  else
  {
    m_shader_particle_colors.use();
    shader_id = m_shader_particle_colors.getID();
    
    glBindBuffer(GL_ARRAY_BUFFER, m_particle_col_buf.getGLID());
    glEnableVertexAttribArray(5);
    glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(cl_float4), (void *) (0));
    glVertexAttribDivisor(5, 1);
  }

  glUniformMatrix4fv(glGetUniformLocation(shader_id, "proj"), 1, GL_FALSE, glm::value_ptr(proj));
  glUniformMatrix4fv(glGetUniformLocation(shader_id, "mv"), 1, GL_FALSE, glm::value_ptr(mv));

  /* calculate the normal matrix (assume only rotations, i.e only rthogonal matrices) */
  glm::mat3 mv_normal = glm::mat3(mv);   // otherwise add glm::transpose(glm::inverse(mv));
  glUniformMatrix3fv(glGetUniformLocation(shader_id, "mv_normal"), 1, GL_FALSE, glm::value_ptr(mv_normal));

  glBindBuffer(GL_ARRAY_BUFFER, m_particle_pos_buf.getGLID());
  glEnableVertexAttribArray(4);
  glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(cl_float4), (void *) (0));
  glVertexAttribDivisor(4, 1);

  glDrawArraysInstanced(m_particle_geom.mode, 0, m_particle_geom.count, m_num_particles);

  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glBindVertexArray(0);

  /* render bounding volume */
  if (m_draw_bounding_volume)
  {
    shader_id = m_shader_bounding_volume.getID();
    glUseProgram(shader_id);

    glUniformMatrix4fv(glGetUniformLocation(shader_id, "proj"), 1, GL_FALSE, glm::value_ptr(proj));
    glUniformMatrix4fv(glGetUniformLocation(shader_id, "mv"), 1, GL_FALSE, glm::value_ptr(mv));

    glDrawArrays(GL_LINES, 0, 24);
  }

  glUseProgram(0);

  return;
}