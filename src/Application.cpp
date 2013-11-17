#include "Application.h"
#include "Window.h"

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <iostream>
#include <cassert>



Application & Application::instance(void)
{
  static Application instance;
  return instance;
}


void Application::registerWindow(Window *wnd)
{
  assert(wnd != nullptr);
  m_windows[wnd->getID()] = wnd;

  return;
}


void Application::unregisterWindow(Window *wnd)
{
  assert(wnd != nullptr);
  m_windows.erase(wnd->getID());
  return;
}


bool Application::init(void)
{
  std::cerr << __FUNCSIG__ << std::endl;

  /* Initialize event handling and video subsystem */
  if (SDL_Init(SDL_INIT_VIDEO) < 0)
  {
    std::cerr << "Failed to initialize SDL: " << SDL_GetError() << std::endl;
    return false;
  }

  atexit(SDL_Quit);

  /* initialize image loading subsystem */
  {
    int flags = IMG_INIT_JPG | IMG_INIT_PNG;
    if (IMG_Init(flags) != flags)
    {
      std::cerr << "Failed to intialize SDL_image: " << IMG_GetError() << std::endl;
      return false;
    }
  }

  atexit(IMG_Quit);

  /* initialize font rendering subsystem */
  if (TTF_Init() < 0)
  {
    std::cerr << "Failed to initialize SDL_ttf: " << TTF_GetError() << std::endl;
    return false;
  }

  atexit(TTF_Quit);

  return true;
}


void Application::quit(void)
{
  std::cerr << __FUNCSIG__ << std::endl;

  tWindowContainer::iterator it = m_windows.begin();
  tWindowContainer::iterator end_it = m_windows.end();

  while (it != end_it)
  {
    delete it->second;
    ++it;
  }

  m_windows.clear();

  return;
}


void Application::run(void)
{
  std::cerr << __FUNCSIG__ << std::endl;

  SDL_Event event;
  bool running = true;

  while (running)
  {
    while (SDL_PollEvent(&event))
    {
      switch (event.type)
      {
        case SDL_WINDOWEVENT:
          {
            switch (event.window.event)
            {
#if 0
              case SDL_WINDOWEVENT_HIDDEN:
              case SDL_WINDOWEVENT_MINIMIZED:  // Stop redraw when minimized
                std::cerr << "Window " << event.window.windowID << " DEActivated" << std::endl;
                m_windows[event.window.windowID].second = false;
                break;
              
              case SDL_WINDOWEVENT_SHOWN:
              case SDL_WINDOWEVENT_MAXIMIZED:
              case SDL_WINDOWEVENT_RESTORED:
                std::cerr << "Application Activated" << std::endl;
                m_windows[event.window.windowID].second = true;
                break;
#endif         
              case SDL_WINDOWEVENT_RESIZED:
                m_windows[event.window.windowID]->onResize(event.window.data1, event.window.data2);
                break;

              case SDL_WINDOWEVENT_CLOSE:
                m_windows[event.window.windowID]->onClose();
                // TODO: handle deleting the window
                break;
                
              case SDL_WINDOWEVENT_ENTER:
              case SDL_WINDOWEVENT_FOCUS_GAINED:
                m_focused_window = m_windows[event.window.windowID];
                break;
            }
          }
          break;

        case SDL_KEYDOWN:
          m_focused_window->onKeyDown(event.key.keysym.sym, event.key.keysym.mod);
          break;

        case SDL_KEYUP:
          m_focused_window->onKeyUp(event.key.keysym.sym, event.key.keysym.mod);
          break;

        case SDL_MOUSEMOTION:
          m_focused_window->onMouseMove(event.motion.x, event.motion.y, event.motion.xrel, event.motion.yrel, event.motion.state);
          break;

        case SDL_MOUSEBUTTONDOWN:
          m_focused_window->onMouseDown(event.button.button, event.button.x, event.button.y);
          break;

        case SDL_MOUSEBUTTONUP:
          m_focused_window->onMouseUp(event.button.button, event.button.x, event.button.y);
          break;

        case SDL_MOUSEWHEEL:
          m_focused_window->onMouseWheel(event.wheel.x, event.wheel.y);
          break;

        case SDL_QUIT:
          running = false;
          return;  // End main loop

        default:  // Do nothing
          break;
      }
    }
    
    // redraw every application window
    for (auto & it : m_windows)
    {
      Window *wnd = it.second;
      if (wnd->isVisible())
      { // draw the window only if it is active
        std::cerr << "Redrawing window " << wnd->getID() << std::endl;
        SDL_GL_MakeCurrent(wnd->getSDLWindow(), wnd->getGLContext());
        wnd->onRedraw();
        SDL_GL_SwapWindow(wnd->getSDLWindow());
      }
    }
  }

  return;
}