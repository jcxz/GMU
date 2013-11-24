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

} // End of namespace Debug