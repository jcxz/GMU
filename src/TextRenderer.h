/*
 * Copyright (C) 2014 Matus Fedorko <xfedor01@stud.fit.vutbr.cz>
 *
 * This software is provided 'as-is', without any express or implied
 * warranty.  In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:

 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 */

/**
 * 2D painting into Windows (text rendering, ...)
 */

#ifndef TEXTRENDERER_H
#define TEXTRENDERER_H

#include "geom.h"
#include "ogl_lib.h"

#include <SDL.h>
#include <SDL_ttf.h>


class Window;

class TextRenderer
{
  public:
    enum Quality {
      QUALITY_HIGH,
      QUALITY_NORMAL,
      QUALITY_LOW
    };

  public:
    enum FontSizeType {
      FONT_SIZE_TYPE_SMALL,
      FONT_SIZE_TYPE_NORMAL,
      FONT_SIZE_TYPE_LARGE
    };

  private:
    static const int SMALL_FONT_DEF_SIZE = 12;
    static const int NORMAL_FONT_DEF_SIZE = 24;
    static const int LARGE_FONT_DEF_SIZE = 48;

    static const char *m_vert_shader;
    static const char *m_frag_shader;

  public:
    TextRenderer::TextRenderer(Window *target = nullptr)
      :  m_small_font(nullptr),
         m_normal_font(nullptr),
         m_large_font(nullptr),
         m_small_fontfile(nullptr),
         m_normal_fontfile(nullptr),
         m_large_fontfile(nullptr),
         m_target_wnd(target),
         m_quality(QUALITY_HIGH),
         m_square(),
         m_texture(),
         m_shader()
    {
      m_font_col.r = 0xFF;
      m_font_col.g = 0x00;
      m_font_col.b = 0x00;
      m_font_col.a = 0xFF;
#if 1
      if (!m_shader.build(m_vert_shader, m_frag_shader))
      {
        throw std::runtime_error("Failed to construct TextRenderer: failed to compile shaders");
      }
#else
      //if (!m_shader.buildFiles("../../../src/OpenGL/TextRenderer.vert", "../../../src/OpenGL/TextRenderer.frag"))
      if (!m_shader.buildFiles("TextRenderer.vert", "TextRenderer.frag"))
      {
        throw std::runtime_error("Failed to construct TextRenderer: failed to compile shaders");
      }
#endif
      geom::gen2DRectangle(m_square);
    }
    
    TextRenderer::~TextRenderer(void)
    {
      TTF_CloseFont(m_small_font);
      TTF_CloseFont(m_normal_font);
      TTF_CloseFont(m_large_font);
    }

    void setTargetWindow(Window *wnd)
    {
      m_target_wnd = wnd;
    }

    void setQuality(Quality quality)
    {
      m_quality = quality;
    }

    void setStyle(FontSizeType type, int style)
    {
      TTF_SetFontStyle(fontByType(type), style);
    }

    void setColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a = 0xFF)
    {
      m_font_col.r = r;
      m_font_col.g = g;
      m_font_col.b = b;
      m_font_col.a = a;
    }

    void setColor(SDL_Color col)
    {
      m_font_col = col;
    }

    // Warning: this function needs to reload the Font file,
    // which may be expensive
    bool changeFontSize(FontSizeType type, int size)
    {
      return loadFont(fontFileByType(type), type, size);
    }

    bool loadFont(const char *fontfile, FontSizeType type, int size);

    bool loadFonts(const char *fontfile,
                   int small_font_size = SMALL_FONT_DEF_SIZE,
                   int normal_font_size = NORMAL_FONT_DEF_SIZE,
                   int large_font_size = LARGE_FONT_DEF_SIZE);

    SDL_Surface *renderToSurface(const char *text,
                                 const SDL_Color *col = nullptr,
                                 FontSizeType type = FONT_SIZE_TYPE_NORMAL);

    void render(int x, int y,
                const char *text,
                const SDL_Color *col = nullptr,
                FontSizeType type = FONT_SIZE_TYPE_NORMAL);

    void renderSmall(int x, int y,
                     const char *text,
                     const SDL_Color *col = nullptr)
    {
      return render(x, y, text, col, FONT_SIZE_TYPE_SMALL);
    }
    
    void renderLarge(int x, int y,
                     const char *text,
                     const SDL_Color *col = nullptr)
    {
      return render(x, y, text, col, FONT_SIZE_TYPE_LARGE);
    }

  private:
    inline TTF_Font *fontByType(FontSizeType type)
    {
      if (type == FONT_SIZE_TYPE_LARGE) return m_large_font;
      if (type == FONT_SIZE_TYPE_SMALL) return m_small_font;
      return m_normal_font;
    }
    
    inline const char *fontFileByType(FontSizeType type)
    {
      if (type == FONT_SIZE_TYPE_LARGE) return m_large_fontfile;
      if (type == FONT_SIZE_TYPE_SMALL) return m_small_fontfile;
      return m_normal_fontfile;
    }

  private:
    TextRenderer(const TextRenderer & );
    TextRenderer & operator=(const TextRenderer & );

  private:
    TTF_Font *m_small_font;         /// small font that will be rendered
    TTF_Font *m_normal_font;        /// normal font that will be rendered
    TTF_Font *m_large_font;         /// large font that will be rendered
    const char *m_small_fontfile;   /// the path to the file from which the font has been loaded
    const char *m_normal_fontfile;  /// the path to the file from which the font has been loaded
    const char *m_large_fontfile;   /// the path to the file from which the font has been loaded
    Window *m_target_wnd;           /// the target window
    Quality m_quality;              /// font rendering quality
    SDL_Color m_font_col;           /// font color (used for all font sizes)
    geom::Model m_square;           /// square geometry to place the texture
    ogl::Texture m_texture;         /// texture to hold the rendered text
    ogl::ShaderProgram m_shader;    /// shader program
};

#endif