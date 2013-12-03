#include "MainWindow.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>



void MainWindow::onRedraw(void)
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glm::mat4 mv = glm::rotate(
                      glm::rotate(
                           glm::translate(
                                glm::mat4(),
                                glm::vec3(0.0f, 0.0f, m_z_dist)
                           ),
                           m_x_angle,
                           glm::vec3(1.0f, 0.0f, 0.0f)
                      ),
                      m_y_angle,
                      glm::vec3(0.0f, 1.0f, 0.0f)
                 );

  glm::mat4 proj = glm::perspective(45.0f, float(m_wnd_w) / float(m_wnd_h), 0.1f, 1000.0f);

  /* update the fluid system */
  m_fluid_system.update();

  m_fluid_system.render(mv, proj);

  /* display messages */
  m_text_renderer.renderSmall(10, 10, "Hello, World!");
  m_text_renderer.render(10, 110, "Hello, World!");
  m_text_renderer.renderLarge(10, 210, "Hello, World!");

  return;
}


void MainWindow::onResize(int32_t width, int32_t height)
{
  m_wnd_h = height;
  m_wnd_w = width;
  glViewport(0, 0, width, height);
  return;
}


void MainWindow::onKeyDown(SDL_Keycode key, uint16_t mod)
{
  if (mod & KMOD_CTRL)
  {
    if (key == SDLK_s)
    {
      m_fluid_system.toggleMode();
      std::cerr << m_fluid_system.modeName() << std::endl;
    }
  }

  return;
}


void MainWindow::onMouseMove(uint32_t buttons, int32_t x, int32_t y, int32_t xrel, int32_t yrel)
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
  
    std::cout << "m_x_angle: " << m_x_angle << ", m_y_angle: " << m_y_angle << ", z_dist: " << m_z_dist << std::endl;
  }

  return;
}