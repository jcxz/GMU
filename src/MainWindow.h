#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "Window.h"
#include "TextRenderer.h"


class MainWindow : public Window
{
  public:
    MainWindow(const char *window_title, unsigned width, unsigned height)
      : Window(window_title, width, height),
        m_text_renderer(this)
    {
      m_text_renderer.loadFonts("../../../res/SourceSansPro-Regular.ttf");
      m_text_renderer.setColor(0x80, 0x00, 0x10);
    }

  protected:
    virtual void onRedraw(void);

  private:
    TextRenderer m_text_renderer;
};

#endif