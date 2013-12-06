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
      , m_cur_ps(m_fluid_system.get())
      , m_wnd_w(0)
      , m_wnd_h(0)
      , m_x_angle(0.0f)
      , m_y_angle(0.0f)
      , m_z_dist(-80.0f)
      , m_display_info(false)
      , m_display_help(false)
    {
      /* prepare simulator */
      if (!m_cur_ps->reset(2025)) //20025))
      //if (!m_cur_ps->reset(10000))
      {
        throw std::runtime_error("MainWindow: failed to prepare fluid simulator");
      }

      /* load fonts */
      //if (!m_text_renderer.loadFonts(utils::AssetsPath("/res/SourceSansPro-Regular.ttf")))
      if (!m_text_renderer.loadFonts(utils::AssetsPath("/res/Cousine-Bold-Latin.ttf"),
                                     SMALL_FONT_HEIGHT, NORMAL_FONT_HEIGHT, LARGE_FONT_HEIGHT))
      {
        throw std::runtime_error("MainWindow: failed to load font");
      }

      //m_text_renderer.setColor(0x80, 0x00, 0x10);
      m_text_renderer.setColor(0x80, 0x80, 0x80);
    }

  protected:
    virtual void onRedraw(void);
    virtual void onResize(int32_t width, int32_t height);
    virtual void onKeyDown(SDL_Keycode key, uint16_t mod);
    virtual void onMouseMove(uint32_t buttons, int32_t x, int32_t y, int32_t xrel, int32_t yrel);

  private:
    // helper functions to display information
    // @param height is the starting height
    // @return the end height where further text can be placed
    int displayInfo(int height);
    int displayHelp(int height);

  private:
    static const int SMALL_FONT_HEIGHT = 10;
    static const int NORMAL_FONT_HEIGHT = 12;
    static const int LARGE_FONT_HEIGHT = 20;

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
    bool m_display_info;
    bool m_display_help;
};

#endif