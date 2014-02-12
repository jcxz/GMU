#include "utils_fs.h"

#include <fstream>
#include <iterator>
#include <iostream>



namespace utils {

namespace fs {

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

} // End of fs namespace

} // End of utils namespace