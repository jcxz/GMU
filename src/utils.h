#ifndef UTILS_H
#define UTILS_H

#include "Application.h"

#include <string>



// forward declarations
namespace ogl { class Texture; }


// namespace interface
namespace utils {

/* Load whole file and return it as std::string */
bool loadFile(const char * const filename, std::string *source_code);

bool genCircleTexture(ogl::Texture & tex,
                      unsigned int w, unsigned int h,
                      unsigned char r, unsigned char g,
                      unsigned char b, unsigned char a = 0xFF);

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

}

#endif