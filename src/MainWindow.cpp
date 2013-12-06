#include "MainWindow.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>




int MainWindow::displayInfo(int height)
{
  if (!m_display_info) return height;

  if (m_cur_ps == m_fluid_system.get())
  {
    m_text_renderer.render(10, height, "Fluid System simulator");
  }
  else if (m_cur_ps == m_test_system.get())
  {
    m_text_renderer.render(10, height, "Test System simulator");
  }
  else
  {
    m_text_renderer.render(10, height, "Unknown simulator");
  }

  return height + 50;
}


int MainWindow::displayHelp(int height)
{
  if (!m_display_help) return height;

  static const char *help_strings[] = {
    "Press D to toggle drain effect On/Off",
    "Press F to toggle fountain effect On/Off",
    "Press W to emit wave",
    "Press H to toggle On/Off this help message",
    "Press I to toggle On/Off status information display",
    "Press B to show/hide bounding volume box",
    "Press R to restart simulation",
    "Press SPACE BAR to pause/restart simulation"
  };

  for (unsigned int i = 0; i < FLUIDSIM_COUNT(help_strings); ++i)
  {
    m_text_renderer.render(10, height, help_strings[i]);
    height += 30;
  }

  return height + 20;
}


void MainWindow::onRedraw(void)
{
  //glClearColor(0.5f, 0.5f, 0.5f, 0.5f);

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
  m_cur_ps->update();

  m_cur_ps->render(mv, proj);

  /* display messages */
  m_text_renderer.renderSmall(10, 10, "Press 'H' to display help");
  m_text_renderer.renderSmall(10, 40, "Press 'I' to display status information");

  displayInfo(displayHelp(110));

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
  switch (key)
  {
    case SDLK_d:     m_fluid_system->toggleDrain();    break;
    case SDLK_f:     m_fluid_system->toggleFountain(); break;
    case SDLK_w:     m_fluid_system->emitWave();       break;
    case SDLK_h:     m_display_help = !m_display_help; break;
    case SDLK_i:     m_display_info = !m_display_info; break;
    case SDLK_SPACE: m_cur_ps->togglePause();          break;

    case SDLK_b:
      std::cerr << "Bounding volume: " << (m_cur_ps->toggleDrawBoundingVolume() ? "on" : "off") << std::endl;
      break;

    case SDLK_r:
      if (!m_cur_ps->reset(2025)) //20025))
      {
        std::cerr << "MainWindow: failed to reset fluid simulator" << std::endl;
      }
      break;
  }

  if (mod & KMOD_CTRL)
  {
    if (key == SDLK_s)
    {
      if (m_cur_ps == m_fluid_system.get())
      {
        std::cerr << "MainWindow: Switching to Fluid simulator" << std::endl;
        m_cur_ps = m_test_system.get();
      }
      else if (m_cur_ps == m_test_system.get())
      {
        std::cerr << "MainWindow: Switching to Test simulator" << std::endl;
        m_cur_ps = m_fluid_system.get();
      }
      else
      {
        std::cerr << "MainWindow: Unknown particle system" << std::endl;
        return;
      }

      if (!m_cur_ps->reset(2025)) //20025))
      {
        std::cerr << "MainWindow: failed to reset fluid simulator" << std::endl;
      }
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
      m_fluid_system->setRotation(m_x_angle, m_y_angle);
    }
    else if (buttons & SDL_BUTTON_RMASK)
    {
      m_z_dist += yrel;
    }
  
    std::cout << "m_x_angle: " << m_x_angle << ", m_y_angle: " << m_y_angle << ", z_dist: " << m_z_dist << std::endl;
  }

  return;
}