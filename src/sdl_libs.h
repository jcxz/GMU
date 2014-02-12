#ifndef SDL_LIBS_H
#define SDL_LIBS_H

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX               // disable min, max macros from Windows headers

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <stdexcept>
#include <string>


namespace sdl {

struct Exception : public std::runtime_error
{
  Exception(void) throw()
    : std::runtime_error(std::string("SDL : ") + SDL_GetError())
  {
  }
  
  Exception(const std::string & text) throw()
    : std::runtime_error(std::string("SDL : ") + text + " : " + SDL_GetError())
  {
  }
};

}

#endif