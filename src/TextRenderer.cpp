#include "TextRenderer.h"
#include "Window.h"
#include "debug.h"

#include <stdexcept>
#include <string>



bool TextRenderer::loadFont(const char *fontfile, FontSizeType type, int size)
{
  TTF_Font *font = TTF_OpenFont(fontfile, size);
  if (font == nullptr)
  {
    std::cerr << "Failed to open font from file " << fontfile << ": " << TTF_GetError() << std::endl;
    return false;
  }

  if (type == FONT_SIZE_TYPE_LARGE)
  {
    TTF_CloseFont(m_large_font);  // free the old font
    m_large_font = font;
    m_large_fontfile = fontfile;
    return true;
  }

  if (type == FONT_SIZE_TYPE_SMALL)
  {
    TTF_CloseFont(m_small_font);  // free the old font
    m_small_font = font;
    m_small_fontfile = fontfile;
    return true;
  }

  TTF_CloseFont(m_normal_font);  // free the old font
  m_normal_font = font;
  m_normal_fontfile = fontfile;

  return true;
}


bool TextRenderer::loadFonts(const char *fontfile,
                             int small_font_size,
                             int normal_font_size,
                             int large_font_size)
{
  return loadFont(fontfile, FONT_SIZE_TYPE_SMALL, small_font_size)   &&
         loadFont(fontfile, FONT_SIZE_TYPE_NORMAL, normal_font_size) &&
         loadFont(fontfile, FONT_SIZE_TYPE_LARGE, large_font_size);
}


SDL_Surface *TextRenderer::renderToSurface(const char *text, const SDL_Color *col, FontSizeType type)
{
  SDL_Surface *surf = nullptr;  // free the temporary result surface

  TTF_Font *font = fontByType(type);
  assert(font != nullptr);

  /* render the string */
  switch (m_quality)
  {
    case QUALITY_HIGH:
      surf = TTF_RenderText_Blended(font, text, (col ? *col : m_font_col));
      break;

    case QUALITY_NORMAL:
      {
        SDL_Color bg_col = { 0x00, 0x00, 0x00, 0x00 };   // fully transparent black
        surf = TTF_RenderText_Shaded(font, text, (col ? *col : m_font_col), bg_col);
      }
      break;

    case QUALITY_LOW:
      surf = TTF_RenderText_Solid(font, text, (col ? *col : m_font_col));
      break;
  }

  if (surf == nullptr)
  {
    std::cerr << "Failed to render font: " << TTF_GetError() << std::endl;
    return nullptr;
  }

  return surf;
}


void TextRenderer::render(int x, int y,
                          const char *text,
                          const SDL_Color *col,
                          FontSizeType type,
                          Window *wnd)
{
  SDL_Surface *text_surf = renderToSurface(text, col, type);
  if (text_surf == nullptr)
  {
    return;
  }

  if (wnd != nullptr)
  {
    wnd->displaySurface(x, y, text_surf);
  }
  else if (m_target_wnd != nullptr)
  {
    m_target_wnd->displaySurface(x, y, text_surf);
  }

  SDL_FreeSurface(text_surf);

  return;
}