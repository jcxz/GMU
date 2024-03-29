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