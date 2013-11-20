#include "utils.h"

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

}