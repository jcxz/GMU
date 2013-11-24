#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "Window.h"
#include "ogl_lib.h"
#include "TextRenderer.h"
#include "FluidSystem.h"
#include "utils.h"

#include <stdexcept>


class MainWindow : public Window
{
  public:
    MainWindow(const char *window_title, unsigned width, unsigned height)
      : Window(window_title, width, height),
        m_text_renderer(this),
        m_fluid_system(),
        m_wnd_w(0),
        m_wnd_h(0),
        m_x_angle(0.0f),
        m_y_angle(0.0f),
        m_z_dist(-1.0f)
    {
      /* load fonts */
      if (!m_text_renderer.loadFonts(utils::AssetsPath("/res/SourceSansPro-Regular.ttf")))
      {
        throw std::runtime_error("MainWindow: failed to load font");
      }

      m_text_renderer.setColor(0x80, 0x00, 0x10);

      /* prepare fluid simulator */
      if (!m_fluid_system.reset(5))
      {
        throw std::runtime_error("MainWindow: failed to prepare fluid simulator");
      }
    }

  protected:
    virtual void onRedraw(void);
    virtual void onResize(int32_t width, int32_t height);
    virtual void onMouseMove(uint32_t buttons, int32_t x, int32_t y, int32_t xrel, int32_t yrel);

  private:
    TextRenderer m_text_renderer;
    FluidSystem m_fluid_system;
    int m_wnd_w;
    int m_wnd_h;
    float m_x_angle;
    float m_y_angle;
    float m_z_dist;
};

#endif