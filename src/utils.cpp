#include "utils.h"
#include "ogl_lib.h"
#include "debug.h"

#include <fstream>
#include <iterator>
#include <iostream>


namespace utils {

bool loadFile(const char * const filename, std::string *source_code)
{
  std::ifstream stream(filename);
  if (!stream)
  {
    std::cerr << "Failed to open file \'" << filename << "\'";
    return false;
  }

  source_code->assign(std::istream_iterator<char>(stream >> std::noskipws),
                      std::istream_iterator<char>());    // an empty constructor creates an end-of stream iterator

  return true;
}


const char *loadFile(const char * const filename, size_t *file_size)
{
  /* open the file for reading */
  std::ifstream stream(filename, std::ios::in | std::ios::binary);
  if (!stream)
  {
    std::cerr << "Failed to open file \'" << filename << "\'" << std::endl;
    return nullptr;
  }

  /* find out the file's size */
  stream.seekg(0, std::ios::end);
  std::size_t size = (size_t) stream.tellg();
  stream.seekg(0, std::ios::beg);

  /* allocate memory and read file contents */
  char *str = new char [size + 1];

  stream.read(str, size);

  str[size] = 0;  // null termination

  if (!stream)
  {
    std::cerr << "Failed to read the contents of file \'" << filename << "\': " << std::endl;
    delete [] str;
    return nullptr;
  }

  if (file_size != nullptr)
  {
    *file_size = size;
  }

  return str;
}


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

}