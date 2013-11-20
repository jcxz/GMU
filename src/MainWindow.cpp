#include "MainWindow.h"



void MainWindow::onRedraw(void)
{
  SDL_Color col = { 0xFF, 0x00, 0x00, 0xFF };

#if 0
  SDL_Surface *text_surf = nullptr;

  text_surf = m_text_renderer.renderText("Hello, World!", &col, TextRenderer::FONT_SIZE_TYPE_SMALL);
  displaySurface(10, 10, text_surf);

  text_surf = m_text_renderer.renderText("Hello, World!", &col, TextRenderer::FONT_SIZE_TYPE_NORMAL);
  displaySurface(10, 110, text_surf);

  //text_surf = m_text_renderer.renderText("Hello, World!", &col, TextRenderer::FONT_SIZE_TYPE_LARGE);
  //displaySurface(10, 210, text_surf);

  m_text_renderer.renderText(this, 10, 210, "Hello, World!", &col, TextRenderer::FONT_SIZE_TYPE_LARGE);
#endif

  m_text_renderer.renderSmall(10, 10, "Hello, World!");//, &col);
  m_text_renderer.render(10, 110, "Hello, World!");//, &col);
  m_text_renderer.renderLarge(10, 210, "Hello, World!", &col);

  return;
}