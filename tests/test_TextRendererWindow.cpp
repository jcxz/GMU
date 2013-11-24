#include "test_TextRendererWindow.h"



void TextRendererWindow::onRedraw(void)
{
  glClear(GL_COLOR_BUFFER_BIT);

  m_text_renderer.renderSmall(10, 10, "Hello, World!");
  m_text_renderer.render(10, 110, "Hello, World!");
  m_text_renderer.renderLarge(10, 210, "Hello, World!");

  return;
}


void TextRendererWindow::onResize(int32_t width, int32_t height)
{
  glViewport(0, 0, width, height);
  return;
}