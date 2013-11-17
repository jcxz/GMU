#include "Window.h"

#include <GL/glew.h>
#include <iostream>



///////////////////////////////////////////////////////////////////////////////
// Helper functions

static void __stdcall DebugCallbackARB(GLenum source, GLenum type, GLuint id,
                                       GLenum severity,
                                       GLsizei length, const GLchar *message,
                                       GLvoid *userParam)
{
  (void) length;
  
  //FILE *outFile = (FILE *) userParam;
  //char finalMessage[256];
  //FormatDebugOutputARB(finalMessage, 256, source, type, id, severity, message);
  //fprintf(outFile, "DebugCallbackARB:\n%s\n", finalMessage);
  printf("DebugCallbackARB\n");
  
  return;
}


static void __stdcall DebugCallbackAMD(GLuint id,
                                       GLenum category, GLenum severity,
                                       GLsizei length, const GLchar *message,
                                       GLvoid *userParam)
{
  (void) length;

  //FILE *outFile = (FILE *) userParam;
  //char finalMsg[256];
  //FormatDebugOutputAMD(finalMsg, 256, category, id, severity, message);
  //fprintf(outFile, "DebugCallbackAMD:\n%s\n", finalMsg);    
  printf("DebugCallbackAMD\n");

  return;
}



///////////////////////////////////////////////////////////////////////////////
// Window implementation

Window::~Window(void)
{
  std::cerr << "Window: " << getID() << ": " << __FUNCTION__ << std::endl;

  SDL_GL_DeleteContext(m_gl_ctx);
  SDL_DestroyWindow(m_sdl_wnd);
}


void Window::onRedraw(void)
{
  //std::cerr << "Window: " << getID() << ": " << __FUNCSIG__ << std::endl;
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


void Window::onMouseMove(uint8_t , int32_t , int32_t , int32_t , int32_t )
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

  //std::cout << "OpenGL initialization: " << ogl::errorString() << std::endl;

  // Call init code
  //onInit();

  //onWindowResized(width, height);

  return true;
}