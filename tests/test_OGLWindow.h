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

#ifndef TEST_OGLWINDOW_H
#define TEST_OGLWINDOW_H

#include "Window.h"
#include "TextRenderer.h"
#include "ogl_lib.h"
#include "geom.h"


class OGLWindow : public Window
{
  private:
    static const char * const vert_shader;
    static const char * const frag_shader;

  public:
    OGLWindow(const char *window_title, unsigned width, unsigned height)
      : Window(window_title, width, height),
        m_shader_prog(),
        m_sphere(),
        m_prism(),
        m_triangle(),
        m_square(),
        m_cur_model(&m_sphere),
        m_wnd_w(0),
        m_wnd_h(0),
        m_x_angle(0.0f),
        m_y_angle(0.0f),
        m_z_dist(-1.0f),
        m_text_renderer(this),
        m_text_quality(TextRenderer::QUALITY_HIGH)
    {
#if 1
      if (!m_shader_prog.build(vert_shader, frag_shader))
      {
        throw std::runtime_error("OGLWindow: failed to build shader program");
      }

      if (!geom::genSphere(m_sphere))
      {
        throw std::runtime_error("OGLWindow: failed to generate sphere model");
      }

      if (!geom::genPrism(m_prism))
      {
        throw std::runtime_error("OGLWindow: failed to generate prism model");
      }

      if (!geom::gen2DTriangle(m_triangle))
      {
        throw std::runtime_error("OGLWindow: failed to generate triangle model");
      }

      if (!geom::gen2DRectangle(m_square, 4.0f))
      {
        throw std::runtime_error("OGLWindow: failed to generate square model");
      }
#endif
      if (!m_text_renderer.loadFonts("../../../res/SourceSansPro-Regular.ttf"))
      //if (!m_text_renderer.loadFonts("../../../../res/SourceSansPro-Regular.ttf"))
      //if (!m_text_renderer.loadFonts("SourceSansPro-Regular.ttf"))
      {
        throw std::runtime_error("OGLWindow: failed to load font");
      }

      m_text_renderer.setColor(0x80, 0x00, 0x10);
      //m_text_renderer.setColor(0xFF, 0x00, 0x00);
      //m_text_renderer.setColor(0x00, 0x00, 0xFF);

      getSize(&m_wnd_w, &m_wnd_h);
    }

  protected:
    virtual void onRedraw(void);
    virtual void onResize(int32_t width, int32_t height);
    virtual void onKeyDown(SDL_Keycode key, uint16_t mod);
    virtual void onMouseMove(uint32_t buttons, int32_t x, int32_t y, int32_t xrel, int32_t yrel);

  private:
    ogl::ShaderProgram m_shader_prog;
    geom::Model m_sphere;
    geom::Model m_prism;
    geom::Model m_triangle;
    geom::Model m_square;
    geom::Model *m_cur_model;
    int m_wnd_w;
    int m_wnd_h;
    float m_x_angle;
    float m_y_angle;
    float m_z_dist;
    TextRenderer m_text_renderer;
    TextRenderer::Quality m_text_quality;
};

#endif