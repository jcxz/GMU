#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "Window.h"
#include "TextRenderer.h"
#include "FluidSystem.h"
#include "ogl_lib.h"

#include <stdexcept>


class MainWindow : public Window
{
  public:
    MainWindow(const char *window_title, unsigned width, unsigned height)
      : Window(window_title, width, height),
        m_text_renderer(this),
        m_fluid_system()
    {
      if (!m_text_renderer.loadFonts("../../../res/SourceSansPro-Regular.ttf"))
      {
        throw std::runtime_error("MainWindow: failed to load font");
      }

      m_text_renderer.setColor(0x80, 0x00, 0x10);

      if (!m_shader_prog.build("", ""))
      {
        throw std::runtime_error("MainWindow: failed to build shader program");
      }
    }

  protected:
    virtual void onRedraw(void);

  private:
    TextRenderer m_text_renderer;
    FluidSystem m_fluid_system;
    ogl::ShaderProgram m_shader_prog;
};

#endif