#include "test_TextRendererWindow.h"



void TextRendererWindow::onRedraw(void)
{
  m_text_renderer.renderSmall(10, 10, "Hello, World!");
  m_text_renderer.render(10, 110, "Hello, World!");
  m_text_renderer.renderLarge(10, 210, "Hello, World!");

  return;
}