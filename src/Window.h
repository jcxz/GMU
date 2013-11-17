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
           bool enable_gl_debug = false)
      : m_sdl_wnd(nullptr),
        m_gl_ctx(nullptr)
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

    bool isVisible(void) const
    {
      uint32_t flags = SDL_GetWindowFlags(m_sdl_wnd);
      return (flags & SDL_WINDOW_SHOWN) && (!(flags & SDL_WINDOW_MINIMIZED));
    }
  
  protected:
    SDL_Window *getSDLWindow(void) const
    {
      return m_sdl_wnd;
    }

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
    virtual void onMouseMove(uint8_t buttons, int32_t x, int32_t y, int32_t xrel, int32_t yrel);
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

    friend class Application;  // give the application class access to protected functions

  private:
    bool init(const char *window_title,
              unsigned width, unsigned height,
              unsigned color, unsigned depth, unsigned stencil,
              bool enable_gl_debug = false);

  private:
    SDL_Window *m_sdl_wnd;   /// SDL window structure
    SDL_GLContext m_gl_ctx;  /// OpenGL context
};

#endif