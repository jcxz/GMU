#include "TextRenderer.h"
#include "Window.h"
#include "debug.h"
#include "utils.h"

#include <stdexcept>
#include <string>



const char *TextRenderer::m_vert_shader =
  "#version 330\n"
  ""
  "uniform float x;"
  "uniform float y;"
  "uniform float w;"
  "uniform float h;"
  ""
  "layout(location = 0) in vec2 pos;"
  "layout(location = 3) in vec2 tex_coordi;"
  ""
  "out vec2 tex_coord;"
  ""
  "void main(void)"
  "{"
  "  gl_Position = vec4(pos.x * w + x, pos.y * h + y, -0.2f, 1.0f);"
  "  tex_coord = tex_coordi;"
  "}"
;

const char *TextRenderer::m_frag_shader =
  "#version 330\n"
  ""
  "uniform sampler2D tex;"
  ""
  "in vec2 tex_coord;"
  ""
  "void main(void)"
  "{"
  "  gl_FragColor = texture(tex, tex_coord);"
  "}"
;


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
        SDL_Color bg_col = { 0x00, 0x00, 0x00, 0xFF };   // fully transparent black
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
                          FontSizeType type)
{
  /* get window dimensions */
  int wnd_w = 0;
  int wnd_h = 0;

  if (m_target_wnd == nullptr)
  {
    WARN("Text not rendered, because target window is NULL.");
    return;
  }

  m_target_wnd->getSize(&wnd_w, &wnd_h);

  /* render the text in software */
  SDL_Surface *surf = renderToSurface(text, col, type);
  if (surf == nullptr)
  {
    WARN("Failed to render text to SDL_Surface.");
    return;
  }

  /* convert the text to OpenGL texture */
  if (!m_texture.load(surf, false))
  {
    WARN("Failed to convert text from SDL_Surface to OpenGL texture.");
    SDL_FreeSurface(surf);
    return;
  }

  SDL_FreeSurface(surf);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  /* render the text texture */
  m_shader.use();

  GLuint prog = m_shader.getID();

  // set uniform variables
  // the position and dimensions of the window have to be converted to normalized
  // device coordinates
  glUniform1f(glGetUniformLocation(prog, "x"), (float(x + surf->w) / float(wnd_w)) - 1.0f);
  glUniform1f(glGetUniformLocation(prog, "y"), 1.0f - (float(y + surf->h) / float(wnd_h)));
  glUniform1f(glGetUniformLocation(prog, "w"), (float(surf->w) / float(wnd_w)));
  glUniform1f(glGetUniformLocation(prog, "h"), (float(surf->h) / float(wnd_h)));

  glUniform1i(glGetUniformLocation(prog, "tex"), 0);

  //glActiveTexture(GL_TEXTURE0);
  m_texture.bind();

  glBindVertexArray(m_square.vao);

  glDrawArrays(m_square.mode, 0, m_square.count);

  glBindVertexArray(0);

  glBindTexture(GL_TEXTURE_2D, 0);
  
  glUseProgram(0);

  glDisable(GL_BLEND);

  return;
}