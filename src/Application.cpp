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


void Application::closeWindow(Window *wnd)
{
  assert(wnd != nullptr);
  wnd->onClose();
  m_windows.erase(wnd->getID());
  delete wnd;
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
      SDL_Quit();
      return false;
    }
  }

  atexit(IMG_Quit);

  /* initialize font rendering subsystem */
  if (TTF_Init() < 0)
  {
    std::cerr << "Failed to initialize SDL_ttf: " << TTF_GetError() << std::endl;
    IMG_Quit();
    SDL_Quit();
    return false;
  }

  atexit(TTF_Quit);

  return true;
}


void Application::quit(void)
{
  std::clog << __FUNCSIG__ << std::endl;

  tWindowContainer::iterator it = m_windows.begin();
  tWindowContainer::iterator end_it = m_windows.end();

  while (it != end_it)
  {
    delete it->second;
    ++it;
  }

  m_windows.clear();

  // registered Windows might be doing cleanup of some
  // SDL objects in their destructors
  // and because Application is a global object
  // it may happen that Window destructor code will be executed
  // only after atexit calls these functions
  
  //TTF_Quit();
  //IMG_Quit();
  //SDL_Quit();

  return;
}


void Application::run(void)
{
  std::cerr << __FUNCSIG__ << std::endl;

  SDL_Event event;
  bool running = true;

  static const uint32_t FPS = 25;
  int32_t frame_time = 1000 / FPS;   // the time that is reserved for each frame (in miliseconds)

  while (running)
  {
    int32_t frame_start = SDL_GetTicks();

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
                closeWindow(m_windows[event.window.windowID]);
                break;

#if 0
              case SDL_WINDOWEVENT_ENTER:
              case SDL_WINDOWEVENT_FOCUS_GAINED:
                m_focused_window = m_windows[event.window.windowID];
                break;
#endif
            }
          }
          break;

        case SDL_KEYDOWN:
          {
            tWindowContainer::iterator it = m_windows.find(event.key.windowID);
            if (it != m_windows.end())
            {
              it->second->onKeyDown(event.key.keysym.sym, event.key.keysym.mod);
            }
          }
          //m_focused_window->onKeyDown(event.key.keysym.sym, event.key.keysym.mod);
          break;

        case SDL_KEYUP:
          {
            tWindowContainer::iterator it = m_windows.find(event.key.windowID);
            if (it != m_windows.end())
            {
              it->second->onKeyUp(event.key.keysym.sym, event.key.keysym.mod);
            }
          }
          //m_focused_window->onKeyUp(event.key.keysym.sym, event.key.keysym.mod);
          break;

        case SDL_MOUSEMOTION:
          {
            tWindowContainer::iterator it = m_windows.find(event.motion.windowID);
            if (it != m_windows.end())
            {
              it->second->onMouseMove(event.motion.state, event.motion.x, event.motion.y, event.motion.xrel, event.motion.yrel);
            }
          }
          //m_focused_window->onMouseMove(event.motion.x, event.motion.y, event.motion.xrel, event.motion.yrel, event.motion.state);
          break;

        case SDL_MOUSEBUTTONDOWN:
          {
            tWindowContainer::iterator it = m_windows.find(event.button.windowID);
            if (it != m_windows.end())
            {
              it->second->onMouseDown(event.button.button, event.button.x, event.button.y);
            }
          }
          //m_focused_window->onMouseDown(event.button.button, event.button.x, event.button.y);
          break;

        case SDL_MOUSEBUTTONUP:
          {
            tWindowContainer::iterator it = m_windows.find(event.button.windowID);
            if (it != m_windows.end())
            {
              it->second->onMouseUp(event.button.button, event.button.x, event.button.y);
            }
          }
          //m_focused_window->onMouseUp(event.button.button, event.button.x, event.button.y);
          break;

        case SDL_MOUSEWHEEL:
          {
            tWindowContainer::iterator it = m_windows.find(event.wheel.windowID);
            if (it != m_windows.end())
            {
              it->second->onMouseWheel(event.wheel.x, event.wheel.y);
            }
          }
          //m_focused_window->onMouseWheel(event.wheel.x, event.wheel.y);
          break;

        case SDL_QUIT:
          std::clog << "SDL_QUIT" << std::endl;
          quit();
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
        //std::cerr << "Redrawing window " << wnd->getID() << std::endl;
        SDL_GL_MakeCurrent(wnd->getSDLWindow(), wnd->getGLContext());
        wnd->onRedraw();
        wnd->refresh();
        //SDL_GL_SwapWindow(wnd->getSDLWindow());
      }
    }

    // manage remaining time
    int32_t frame_delay = SDL_GetTicks() - frame_start;
    int32_t sleep_time = frame_time - frame_delay;
    //std::cerr << "sleep_time  : " << sleep_time  << std::endl;
    //std::cerr << "frame_start : " << frame_start << std::endl;
    //std::cerr << "frame_delay : " << frame_delay << std::endl;
    if (sleep_time > 0)
    {
      SDL_Delay(sleep_time);
    }
  }

  return;
}