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

/* load the entire file and return it as a c-like string
   Note that caller is responsible for deallocating the string */
const char *loadFile(const char * const filename, size_t *file_size = nullptr);

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

#define FLUIDSIM_COUNT(array) (sizeof((array)) / sizeof(*(array)))

}

#endif