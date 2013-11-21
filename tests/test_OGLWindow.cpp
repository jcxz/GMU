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
#if 1
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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
  glUniform1f(glGetUniformLocation(m_shader_prog.getID(), "window_height"), m_wnd_h);

  assert(m_cur_model != nullptr);
  // bind the model to be drawn
  glBindVertexArray(m_cur_model->vao);

  glDrawArrays(m_cur_model->mode, 0, m_cur_model->count);

  glBindVertexArray(0);

  glUseProgram(0);
#endif

  m_text_renderer.renderSmall(10, 10, "Hello, World!");
  m_text_renderer.render(10, 110, "Hello, World!");
  m_text_renderer.renderLarge(10, 210, "Hello, World!");

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
    else
    {
      std::cerr << "m_triangle" << std::endl;
      m_cur_model = &m_triangle;
    }
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