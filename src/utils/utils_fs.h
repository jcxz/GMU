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

/** Utility functions concerning filesystem handling */

#ifndef UTILS_FS_H
#define UTILS_FS_H

#include "Application.h"

#include <string>


namespace utils {

namespace fs {

/* Load whole file and return it as std::string */
bool loadFile(const char * const filename, std::string *source_code);

/* load the entire file and return it as a c-like string
   Note that caller is responsible for deallocating the string */
const char *loadFile(const char * const filename, size_t *file_size = nullptr);

/** A helper class to assemble an Assets path */
struct AssetsPath
{
  std::string p;

  explicit AssetsPath(const char *path)
    : p(Application::instance().getAssetsRootDir())
  {
    // handle the double slash problem
    if (p.back() != '/') p.push_back('/');
    p += ((*path == '/') ? (path + 1) : path);
  }

  operator const char*(void)
  {
    return p.c_str();
  }
};

} // End of fs namespace

} // End of utils namespace


#endif