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

#include "test_OGLWindow.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>



const char * const OGLWindow::vert_shader =
  "#version 330\n"
  ""
  "uniform mat4 mvp;"
  ""
  "layout(location = 0) in vec3 pos;"
  "layout(location = 1) in vec3 normal;"
  ""
  "void main(void)"
  "{"
  "  gl_Position = mvp * vec4(pos, 1.0f);"
  "}"
;


const char * const OGLWindow::frag_shader =
  "#version 330\n"
  ""
  "uniform float window_height;"
  ""
  "void main(void)"
  "{"
  "  float t = 1.0f - gl_FragCoord.y / window_height;"
  "  gl_FragColor = mix(vec4(1.0f, 1.0f, 1.0f, 1.0f),"
  "                     vec4(0.0f, 0.0f, 0.0f, 1.0f),"
  "                     t);"
  "}"
;



void OGLWindow::onRedraw(void)
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
#if 1
  glm::mat4 mvp = glm::rotate(
                     glm::rotate(
                          glm::translate(
                               glm::perspective(45.0f, float(m_wnd_w) / float(m_wnd_h), 0.1f, 100.0f),
                               glm::vec3(0.0f, 0.0f, m_z_dist)
                          ),
                        m_x_angle,
                        glm::vec3(1.0f, 0.0f, 0.0f)
                     ),
                     m_y_angle,
                     glm::vec3(0.0f, 1.0f, 0.0f)
                  );

  m_shader_prog.use();

  glUniformMatrix4fv(glGetUniformLocation(m_shader_prog.getID(), "mvp"), 1, GL_FALSE, glm::value_ptr(mvp));
  glUniform1f(glGetUniformLocation(m_shader_prog.getID(), "window_height"), (GLfloat) m_wnd_h);

  assert(m_cur_model != nullptr);
  // bind the model to be drawn
  glBindVertexArray(m_cur_model->vao);

  glDrawArrays(m_cur_model->mode, 0, m_cur_model->count);

  glBindVertexArray(0);

  glUseProgram(0);
#endif

  m_text_renderer.setQuality(m_text_quality);

  {
    SDL_Color col = { 0xFF, 0xFF, 0xFF, 0xFF };
    m_text_renderer.renderSmall(10, 10, "Hello, World!", &col);
  }
  
  {
    SDL_Color col = { 0xFF, 0x00, 0x00, 0xFF };
    m_text_renderer.render(10, 110, "Hello, World!", &col);
  }

  {
    SDL_Color col = { 0x00, 0xFF, 0x00, 0xFF };
    m_text_renderer.renderLarge(10, 210, "Hello, World!", &col);
  }

  return;
}


void OGLWindow::onResize(int32_t width, int32_t height)
{
  m_wnd_h = height;
  m_wnd_w = width;
  glViewport(0, 0, m_wnd_w, m_wnd_h);

  return;
}


void OGLWindow::onKeyDown(SDL_Keycode key, uint16_t mod)
{
  (void) mod;

  if (key == SDLK_m)
  {
    if (m_cur_model == &m_sphere)
    {
      std::cerr << "m_prism" << std::endl;
      m_cur_model = &m_prism;
    }
    else if (m_cur_model == &m_triangle)
    {
      std::cerr << "m_sphere" << std::endl;
      m_cur_model = &m_sphere;
    }
    else if (m_cur_model == &m_square)
    {
      std::cerr << "m_trianle" << std::endl;
      m_cur_model = &m_triangle;
    }
    else
    {
      std::cerr << "m_square" << std::endl;
      m_cur_model = &m_square;
    }
  }
  else if (key == SDLK_q)
  {
    if (m_text_quality == TextRenderer::QUALITY_HIGH)
    {
      m_text_quality = TextRenderer::QUALITY_NORMAL;
      std::cerr << "TextRenderer::QUALITY_NORMAL" << std::endl;
    }
    else if (m_text_quality == TextRenderer::QUALITY_NORMAL)
    {
      m_text_quality = TextRenderer::QUALITY_LOW;
      std::cerr << "TextRenderer::QUALITY_LOW" << std::endl;
    }
    else
    {
      m_text_quality = TextRenderer::QUALITY_HIGH;
      std::cerr << "TextRenderer::QUALITY_HIGH" << std::endl;
    }
  }
  else if (key == SDLK_w)
  {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  }
  else if (key == SDLK_n)
  {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  }

  return;
}


void OGLWindow::onMouseMove(uint32_t buttons, int32_t x, int32_t y, int32_t xrel, int32_t yrel)
{
  if (buttons)
  {
    if (buttons & SDL_BUTTON_LMASK)
    {
      m_x_angle += yrel;
      m_y_angle += xrel;
    }
    else if (buttons & SDL_BUTTON_RMASK)
    {
      m_z_dist += yrel;
    }
  
    //std::cout << "m_x_angle: " << m_x_angle << ", m_y_angle: " << m_y_angle << std::endl;
  }

  return;
}