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

#include "utils_graphics.h"
#include "ogl_lib.h"
#include "debug.h"



namespace utils {

namespace graphics {

bool genCircleTexture(ogl::Texture & tex,
                      unsigned int w, unsigned int h,
                      unsigned char r, unsigned char g,
                      unsigned char b, unsigned char a)
{
  /* allocate temporary memory for texture generation */
  unsigned char *pixels = (unsigned char *) malloc(w * h * 4 * sizeof(char));
  if (pixels == nullptr)
  {
    ERROR("Failed to allocate auxiliary memory for circle texture");
    return false;
  }

  /* calculate the square of the circle radius */
  int w_half = w / 2 - 1;
  int h_half = h / 2 - 1;
  unsigned int r2 = (w_half < h_half) ? w_half : h_half;

  r2 = r2 * r2;

  /* generate the texture */
  unsigned char *p_pixels = pixels;
  for (unsigned int y = 0; y < h; ++y)
  {
    for (unsigned int x = 0; x < w; ++x)
    {
      if (((x - w_half) * (x - w_half) + (y - h_half) * (y - h_half)) <= r2)
      {
        p_pixels[0] = r;
        p_pixels[1] = g;
        p_pixels[2] = b;
        p_pixels[3] = a;  // full opacity
      }
      else
      {
        p_pixels[0] = 0;
        p_pixels[1] = 0;
        p_pixels[2] = 0;
        p_pixels[3] = 0;
      }

      p_pixels += 4;
    }
  }
  
  /* load the texture to GPU */
  bool res = tex.load(w, h, pixels);

  free(pixels);

  return res;
}

} // End of graphics namespace

} // End of utils namespace