#ifndef UTILS_H
#define UTILS_H

#include <string>

namespace utils {

/* Load whole file and return it as std::string */
bool loadFile(const char * const filename, std::string *source_code);

}

#endif