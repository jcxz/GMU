/**
 * This file contains the implementation of debugging functionality
 */

#include "global.h"
#include "debug.h"

#include <iostream>
#include <iomanip>
#include <string>



/* a private namespace with helper functions */
namespace {

/**
 * This method will print one line of a given width binary data
 *
 * @param *data the binary data to be printed
 * @param len the length of binary data
 * @param offset an offset to the next line of bianry data to be printed.
 *               This value must be aligned to line_width bytes.
 * @param line_width the length of the line to be printed
 */
void hexdumpLine(std::ostream & os,
                 const uint8_t *data,
                 uint32_t len,
                 uint32_t offset,
                 uint32_t line_width)
{
  FLUIDSIM_ASSERT(data != NULL);

  /* calculate some auxiliary offsets */
  uint32_t sep_offset = offset + 7;           /// an offset of octet separator
  uint32_t end_offset = offset + line_width;  /// end of line offset

  /* print offset count */
  os << std::setw(5) << offset << "   ";

  std::ios_base::fmtflags old_flags = os.flags(std::ios_base::hex |
                                               std::ios_base::uppercase);

  /* print bytes in hexadecimal form */
  for (uint32_t i = offset; i < end_offset; ++i)
  {
    /* pad hexadecimal part with spaces if necessary */
    if (i >= len)
    {
      uint32_t num_spaces = (end_offset - i) * 3 + ((end_offset - sep_offset + 8) / 8);
      for (uint32_t j = 0; j < num_spaces; ++j)
      {
        os.put(' ');
      }

      break;
    }

    if (i == sep_offset)
    {
      os << std::setw(2) << ((uint32_t) data[i]) << "  ";
      sep_offset += 8;
    }
    else
    {
      os << std::setw(2) << ((uint32_t) data[i]) << ' ';
    }
  }

  /* print the hex dump and ascii dump separator */
  os.write("  ", 2);

  /* print the bytes in ascii form */
  for (uint32_t i = offset; i < end_offset; ++i)
  {
    if (i >= len) break;

    if (isprint(data[i]))
    {
      os.put(data[i]);
    }
    else
    {
      os.put('.');
    }
  }

  /* print the end of line and restore formatting settings */
  os << std::endl;
  os.flags(old_flags);

  return;
}

} // End of private namespace



namespace debug {

/**
 */
std::ostream & hexdump(std::ostream & os,
                       const void *data,
                       uint32_t len,
                       uint32_t line_width)
{
  /* make sure the input parameters are valid */
  if (data == NULL)
  {
    os << "NULL" << std::endl;
    return os;
  }

  /* check if there are data to print */
  if ((len <= 0) || (line_width <= 0))
  {
    return os;
  }

  char old_fill = os.fill('0');   /// set padding character

  /* print data */
  for (uint32_t i = 0; i < len; i += line_width)
  {
    hexdumpLine(os, (const uint8_t *) data, len, i, line_width);
  }

  os.fill(old_fill);

  return os;
}


void formatDebugOutputARB(GLenum source, GLenum type,
                          GLuint id, GLenum severity,
                          const char *msg,
                          char *out_str, size_t out_str_size)
{ 
#ifdef FLUIDSIM_CC_MSVC
#  define snprintf _snprintf
#endif

  char source_str[32];
  const char *source_fmt = "UNDEFINED(0x%04X)";

  switch (source)
  {
    case GL_DEBUG_SOURCE_API_ARB:             source_fmt = "API";             break;
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM_ARB:   source_fmt = "WINDOW_SYSTEM";   break;
    case GL_DEBUG_SOURCE_SHADER_COMPILER_ARB: source_fmt = "SHADER_COMPILER"; break;
    case GL_DEBUG_SOURCE_THIRD_PARTY_ARB:     source_fmt = "THIRD_PARTY";     break;
    case GL_DEBUG_SOURCE_APPLICATION_ARB:     source_fmt = "APPLICATION";     break;
    case GL_DEBUG_SOURCE_OTHER_ARB:           source_fmt = "OTHER";           break;
  }

  snprintf(source_str, 32, source_fmt, source);
 
  char type_str[32];
  const char *type_fmt = "UNDEFINED(0x%04X)";

  switch (type)
  {
    case GL_DEBUG_TYPE_ERROR_ARB:               type_fmt = "ERROR";               break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR_ARB: type_fmt = "DEPRECATED_BEHAVIOR"; break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR_ARB:  type_fmt = "UNDEFINED_BEHAVIOR";  break;
    case GL_DEBUG_TYPE_PORTABILITY_ARB:         type_fmt = "PORTABILITY";         break;
    case GL_DEBUG_TYPE_PERFORMANCE_ARB:         type_fmt = "PERFORMANCE";         break;
    case GL_DEBUG_TYPE_OTHER_ARB:               type_fmt = "OTHER";               break;
  }
  
  snprintf(type_str, 32, type_fmt, type);
  
  char severity_str[32];
  const char *severity_fmt = "UNDEFINED";
  
  switch (severity)
  {
    case GL_DEBUG_SEVERITY_HIGH_ARB:   severity_fmt = "HIGH";   break;
    case GL_DEBUG_SEVERITY_MEDIUM_ARB: severity_fmt = "MEDIUM"; break;
    case GL_DEBUG_SEVERITY_LOW_ARB:    severity_fmt = "LOW";    break;
  }

  snprintf(severity_str, 32, severity_fmt, severity);
 
  snprintf(out_str, out_str_size, "OpenGL: %s [source=%s type=%s severity=%s id=%d]",
           msg, source_str, type_str, severity_str, id);

#ifdef FLUIDSIM_CC_MSVC
#  undef snprintf
#endif

  return;
}


void formatDebugOutputAMD(GLuint id, GLenum category, GLenum severity,
                          const char *msg,
                          char *out_str, size_t out_str_size)
{
#ifdef FLUIDSIM_CC_MSVC
#  define snprintf _snprintf
#endif

  char category_str[32];
  const char *category_fmt = "UNDEFINED(0x%04X)";
  
  switch (category)
  {
    case GL_DEBUG_CATEGORY_API_ERROR_AMD:          category_fmt = "API_ERROR";          break;
    case GL_DEBUG_CATEGORY_WINDOW_SYSTEM_AMD:      category_fmt = "WINDOW_SYSTEM";      break;
    case GL_DEBUG_CATEGORY_DEPRECATION_AMD:        category_fmt = "DEPRECATION";        break;
    case GL_DEBUG_CATEGORY_UNDEFINED_BEHAVIOR_AMD: category_fmt = "UNDEFINED_BEHAVIOR"; break;
    case GL_DEBUG_CATEGORY_PERFORMANCE_AMD:        category_fmt = "PERFORMANCE";        break;
    case GL_DEBUG_CATEGORY_SHADER_COMPILER_AMD:    category_fmt = "SHADER_COMPILER";    break;
    case GL_DEBUG_CATEGORY_APPLICATION_AMD:        category_fmt = "APPLICATION";        break;
    case GL_DEBUG_CATEGORY_OTHER_AMD:              category_fmt = "OTHER";              break;
  }
  
  snprintf(category_str, 32, category_fmt, category);

  char severity_str[32];
  const char *severity_fmt = "UNDEFINED";
  
  switch (severity)
  {
    case GL_DEBUG_SEVERITY_HIGH_AMD:   severity_fmt = "HIGH";   break;
    case GL_DEBUG_SEVERITY_MEDIUM_AMD: severity_fmt = "MEDIUM"; break;
    case GL_DEBUG_SEVERITY_LOW_AMD:    severity_fmt = "LOW";    break;
  }
  
  snprintf(severity_str, 32, severity_fmt, severity);

  snprintf(out_str, out_str_size, "OpenGL: %s [category=%s severity=%s id=%d]",
           msg, category_str, severity_str, id);

#ifdef FLUIDSIM_CC_MSVC
#  undef snprintf
#endif 

  return;
}

} // End of namespace Debug
