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