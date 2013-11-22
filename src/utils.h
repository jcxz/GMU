#ifndef UTILS_H
#define UTILS_H

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

}

#endif