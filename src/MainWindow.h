#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "FluidSystem.h"
#include "TestSystem.h"
#include "Window.h"
#include "ogl_lib.h"
#include "TextRenderer.h"
#include "utils.h"

#include <stdexcept>
#include <memory>


class MainWindow : public Window
{
  public:
    MainWindow(const char *window_title, unsigned width, unsigned height)
      : Window(window_title, width, height)
      , m_text_renderer(this)
      , m_fluid_system(new FluidSystem)
      , m_test_system(new TestSystem)
      //, m_cur_ps(m_test_system.get())
      , m_cur_ps(m_fluid_system.get())
      , m_wnd_w(0)
      , m_wnd_h(0)
      , m_x_angle(0.0f)
      , m_y_angle(0.0f)
      //, m_z_dist(-1.0f)
      , m_z_dist(-80.0f)
    {
      /* prepare simulator */
      if (!m_cur_ps->reset(2025)) //20025))
      {
        throw std::runtime_error("MainWindow: failed to prepare fluid simulator");
      }

      /* load fonts */
      //if (!m_text_renderer.loadFonts(utils::AssetsPath("/res/SourceSansPro-Regular.ttf")))
      if (!m_text_renderer.loadFonts(utils::AssetsPath("/res/Cousine-Regular-Latin.ttf")))
      {
        throw std::runtime_error("MainWindow: failed to load font");
      }

      m_text_renderer.setColor(0x80, 0x00, 0x10);
    }

  protected:
    virtual void onRedraw(void);
    virtual void onResize(int32_t width, int32_t height);
    virtual void onKeyDown(SDL_Keycode key, uint16_t mod);
    virtual void onMouseMove(uint32_t buttons, int32_t x, int32_t y, int32_t xrel, int32_t yrel);

  private:
    TextRenderer m_text_renderer;
    std::unique_ptr<FluidSystem> m_fluid_system;
    std::unique_ptr<TestSystem> m_test_system;
    ParticleSystem *m_cur_ps;
    int m_wnd_w;
    int m_wnd_h;
    float m_x_angle;
    float m_y_angle;
    float m_z_dist;
};

#endif