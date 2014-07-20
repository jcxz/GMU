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

#ifndef WINDOW_H
#define WINDOW_H

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX               // disable min, max macros from Windows headers

#include "debug.h"

#include <SDL.h>
#include <stdexcept>
#include <iostream>


class Window
{
  public:
    Window(const char *window_title,
           unsigned width, unsigned height,
           unsigned color = 24, unsigned depth = 16, unsigned stencil = 0,
           bool enable_gl_debug = true)
      : m_sdl_wnd(nullptr),
        m_gl_ctx(nullptr),
        m_sdl_ren(nullptr)
    {
      /* initialize the window */
      if (!init(window_title,
                width, height,
                color, depth, stencil,
                enable_gl_debug))
      {
        throw std::runtime_error("Failed to construct Window");
      }

      /* let the derived class handle window resizing */
      SDL_Event event;
      event.type = SDL_WINDOWEVENT;
      event.window.type = SDL_WINDOWEVENT;
      event.window.timestamp = 0;
      event.window.windowID = SDL_GetWindowID(m_sdl_wnd);
      event.window.event = SDL_WINDOWEVENT_RESIZED;
      getSize(&event.window.data1, &event.window.data2);
      if (SDL_PushEvent(&event) < 0)
      {
        WARN("Window: Failed to send resize event");
      }
    }

    virtual ~Window(void);

    uint32_t getID(void) const
    {
      return SDL_GetWindowID(m_sdl_wnd);
    }

    void setSize(int w, int h)
    {
      SDL_SetWindowSize(m_sdl_wnd, w, h);
    }

    void getSize(int *width, int *height)
    {
      SDL_GetWindowSize(m_sdl_wnd, width, height);
    }

    bool isVisible(void) const
    {
      uint32_t flags = SDL_GetWindowFlags(m_sdl_wnd);
      return (flags & SDL_WINDOW_SHOWN) && (!(flags & SDL_WINDOW_MINIMIZED));
    }
    
    SDL_Window *getSDLWindow(void) const
    {
      return m_sdl_wnd;
    }

    // flips the rendering buffer and displayes rendered content
    void refresh(void)
    {
      SDL_GL_SwapWindow(m_sdl_wnd);
      //SDL_RenderPresent(m_sdl_ren);
    }

  protected:
    SDL_GLContext getGLContext(void) const
    {
      return m_gl_ctx;
    }

    //void onInit(void);

    // triggered when widnow nees to update its contents
    virtual void onRedraw(void);

    // triggered when window is resized
    virtual void onResize(int32_t width, int32_t height);

    // when a key is pressed
    virtual void onKeyDown(SDL_Keycode key, uint16_t mod);
    // when a key is released
    virtual void onKeyUp(SDL_Keycode key, uint16_t mod);

    // when mouse is moved
    virtual void onMouseMove(uint32_t buttons, int32_t x, int32_t y, int32_t xrel, int32_t yrel);
    // when mouse button is pressed
    virtual void onMouseDown(uint8_t button, int32_t x, int32_t y);
    // when mouse button is released
    virtual void onMouseUp(uint8_t button, int32_t x, int32_t y);
    // when mouse wheel is scrolled
    virtual void onMouseWheel(int32_t scroll_x, int32_t scroll_y);
    
    // triggered when window is about to be closed
    virtual void onClose(void);

  private:
    // Window is non-copyable
    Window(const Window & );
    Window & operator=(const Window & );

    friend class Application;   // give the application class access to protected functions

  private:
    bool init(const char *window_title,
              unsigned width, unsigned height,
              unsigned color, unsigned depth, unsigned stencil,
              bool enable_gl_debug = false);

  private:
    SDL_Window *m_sdl_wnd;    /// SDL window structure
    SDL_GLContext m_gl_ctx;   /// OpenGL context
    SDL_Renderer *m_sdl_ren;  /// SDL rendering context
};

#endif