/** utility functions concerning graphical output */

#ifndef UTILS_GRAPHICS_H
#define UTILS_GRAPHICS_H

// forward declarations
namespace ogl { class Texture; }


namespace utils {

namespace graphics {

bool genCircleTexture(ogl::Texture & tex,
                      unsigned int w, unsigned int h,
                      unsigned char r, unsigned char g,
                      unsigned char b, unsigned char a = 0xFF);

} // End of graphics namespace

} // End of utils namespace

#endif