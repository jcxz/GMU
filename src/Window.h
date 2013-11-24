#ifndef WINDOW_H
#define WINDOW_H

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
      if (!init(window_title,
                width, height,
                color, depth, stencil,
                enable_gl_debug))
      {
        throw std::runtime_error("Failed to construct Window");
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