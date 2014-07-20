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

#include "Window.h"
#include "debug.h"
#include "ogl_lib.h"

#include <GL/glew.h>
#include <iostream>
#include <cassert>



///////////////////////////////////////////////////////////////////////////////
// Helper functions

/**
 * ARB Debug message callback
 *
 * @param source The source that produced the message.
 * @param type The type of message.
 * @param id message id
 * @param severity severity of the message
 * @param length of the message
 * @param message the debugging message itself
 * @param userParam the error stream to be used
 */
static void __stdcall DebugCallbackARB(GLenum source, GLenum type, GLuint id,
                                       GLenum severity,
                                       GLsizei length, const GLchar *message,
                                       GLvoid *userParam)
{
  (void) length;
  
  FILE *out_file = (FILE *) userParam;
  char final_msg[256];
  
  ogl::formatDebugOutputARB(source, type, id, severity, message, final_msg, 256);
  fprintf(out_file, "%s\n", final_msg);
  //printf("DebugCallbackARB\n");
  
  return;
}


static void __stdcall DebugCallbackAMD(GLuint id, GLenum category, GLenum severity,
                                       GLsizei length, const GLchar *message,
                                       GLvoid *userParam)
{
  (void) length;

  FILE *out_file = (FILE *) userParam;
  char final_msg[256];

  ogl::formatDebugOutputAMD(id, category, severity, message, final_msg, 256);
  fprintf(out_file, "DebugCallbackAMD:\n%s\n", final_msg);    
  //printf("DebugCallbackAMD\n");

  return;
}



///////////////////////////////////////////////////////////////////////////////
// Window implementation

Window::~Window(void)
{
  std::clog << "Window: " << getID() << ": " << __FUNCTION__ << std::endl;
  //std::clog << "SDL_WasInit(): " << SDL_WasInit(0) << std::endl;

  SDL_DestroyRenderer(m_sdl_ren);
  SDL_GL_DeleteContext(m_gl_ctx);
  SDL_DestroyWindow(m_sdl_wnd);
}


void Window::onRedraw(void)
{
  std::cerr << "Window: " << getID() << ": " << __FUNCSIG__ << std::endl;
  return;
}


void Window::onResize(int32_t , int32_t )
{
  std::cerr << "Window: " << getID() << ": " << __FUNCTION__ << std::endl;
  return;
}


void Window::onKeyDown(SDL_Keycode , uint16_t )
{
  std::cerr << "Window: " << getID() << ": " << __FUNCTION__ << std::endl;
  return;
}


void Window::onKeyUp(SDL_Keycode , uint16_t )
{
  std::cerr << "Window: " << getID() << ": " << __FUNCTION__ << std::endl;
  return;
}


void Window::onMouseMove(uint32_t , int32_t , int32_t , int32_t , int32_t )
{
  std::cerr << "Window: " << getID() << ": " << __FUNCTION__ << std::endl;
  return;
}


void Window::onMouseDown(uint8_t , int32_t , int32_t )
{
  std::cerr << "Window: " << getID() << ": " << __FUNCTION__ << std::endl;
  return;
}


void Window::onMouseUp(uint8_t , int32_t , int32_t )
{
  std::cerr << "Window: " << getID() << ": " << __FUNCTION__ << std::endl;
  return;
}


void Window::onMouseWheel(int32_t , int32_t )
{
  std::cerr << "Window: " << getID() << ": " << __FUNCTION__ << std::endl;
  return;
}

    
void Window::onClose(void)
{
  std::cerr << "Window: " << getID() << ": " << __FUNCTION__ << std::endl;
  return;
}


bool Window::init(const char *window_title,
                  unsigned width, unsigned height,
                  unsigned color, unsigned depth, unsigned stencil,
                  bool enable_gl_debug)
{
  std::cerr << "Window: " << getID() << ": " << __FUNCTION__ << std::endl;

  // Set OpenGL attributes
  if (SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, color) < 0) return false;
  if (SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, depth) < 0) return false;
  if (SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, stencil) < 0) return false;
  if (SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1) < 0) return false;

  // set context type
  if (SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3) < 0) return false;
  if (SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3) < 0) return false;

  if (SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE) < 0) return false;
  //if (SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY) < 0) throw SDL_Exception();

  // request debugging context
  if ((enable_gl_debug) && (SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG) < 0)) return false;

  // Create window
  m_sdl_wnd = SDL_CreateWindow(window_title,
                               SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                               width, height,
                               SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
  if (m_sdl_wnd == nullptr)
  {
    std::cerr << "Failed to create window: " << SDL_GetError() << std::endl;
    return false;
  }

  // Create OpenGl context
  m_gl_ctx = SDL_GL_CreateContext(m_sdl_wnd);
  if (m_gl_ctx == nullptr)
  {
    SDL_DestroyWindow(m_sdl_wnd);
    m_sdl_wnd = nullptr;
    std::cerr << "Failed to create OpenGL context: " << SDL_GetError() << std::endl;
    return false;
  }

  SDL_GL_SetSwapInterval(1);  // synchronise swapping with vertical retrace

  // Init extensions
  // not sure, whether glewInit() shall be called for each created context or not,
  // but it has to be called after a valid OpenGl context has been created, so I rather leave it here
  GLenum error = glewInit();
  if (error != GLEW_OK)
  {
    SDL_GL_DeleteContext(m_gl_ctx);
    m_gl_ctx = nullptr;
    SDL_DestroyWindow(m_sdl_wnd);
    m_sdl_wnd = nullptr;
    std::cerr << "Failed to initialize GLEW: " << ((const char *) glewGetErrorString(error)) << std::endl;
    return false;
  }

  // Erase all potential OpenGL erros cause by SDL
  glGetError();

  if (enable_gl_debug)
  {
    if (glewIsSupported("GL_ARB_debug_output"))
    {
      std::cerr << "Using GL_ARB_debug_output" << std::endl;
      glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB);
      //glEnable(GL_DEBUG_OUTPUT);
      //glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
      glDebugMessageCallbackARB(DebugCallbackARB, stderr);
    }
    else if (glewIsSupported("GL_AMD_debug_output"))
    {
      std::cerr << "Using GL_AMD_debug_output" << std::endl;
      glDebugMessageCallbackAMD(DebugCallbackAMD, stderr); // print debug output to stderr
    }
    else
    {
      std::cerr << "No suitable debugging extension found" << std::endl;
    }
  }

  /* Initialize an SDL renderer */
  m_sdl_ren = SDL_CreateRenderer(m_sdl_wnd,                   // the window that the rendering context is going to be using
                                 -1,                          // the redering driver (-1 to let SDL pick the most suitable one)
                                 SDL_RENDERER_ACCELERATED |   // use HW accelerated rendering
                                 SDL_RENDERER_PRESENTVSYNC);  // turn on vsync
  if (m_sdl_ren == nullptr)
  {
    SDL_GL_DeleteContext(m_gl_ctx);
    m_gl_ctx = nullptr;
    SDL_DestroyWindow(m_sdl_wnd);
    m_sdl_wnd = nullptr;
    std::cerr << "Failed to create SDL_Renderer: " << SDL_GetError() << std::endl;
    return false;
  }

  std::cerr << "Successfully created Window with OpenGL " << (enable_gl_debug ? "debugging context" : "context") << std::endl;

  //std::cout << "OpenGL initialization: " << ogl::errorString() << std::endl;

  // Call init code
  //onInit();

  //onWindowResized(width, height);

  return true;
}