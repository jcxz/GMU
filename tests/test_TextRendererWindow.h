#ifndef TEST_TEXTRENDERERWINDOW_H
#define TEST_TEXTRENDERERWINDOW_H

#include "Window.h"
#include "TextRenderer.h"
#include "utils.h"


class TextRendererWindow : public Window
{
  public:
    TextRendererWindow(const char *window_title, unsigned width, unsigned height)
      : Window(window_title, width, height),
        m_text_renderer(this)
    {
      //if (!m_text_renderer.loadFonts("../../../res/SourceSansPro-Regular.ttf"))
      if (!m_text_renderer.loadFonts(utils::AssetsPath("/res/SourceSansPro-Regular.ttf")))
      {
        throw std::runtime_error("TextRendererWindow: failed to load font");
      }

      m_text_renderer.setColor(0x80, 0x00, 0x10);

      glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    }

  protected:
    virtual void onRedraw(void);
    virtual void onResize(int32_t width, int32_t height);

  private:
    TextRenderer m_text_renderer;
};

#endif