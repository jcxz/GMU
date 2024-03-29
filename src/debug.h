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

/**
 * Debugging functionality
 */

#ifndef DEBUG_H
#define DEBUG_H

#include "global.h"

/** turn on memory leak debugging */
#ifdef DEBUG_MEM_LEAKS
#  ifdef FLUIDSIM_CC_MSVC
#    define _CRTDBG_MAP_ALLOC
#    include <cstdlib>
#    include <crtdbg.h>
#    define new new (_NORMAL_BLOCK , __FILE__ , __LINE__)
#    define ENABLE_LEAK_DEBUG() _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF)
#  endif
#else
#  define ENABLE_LEAK_DEBUG()
#endif

#include <cstdint>
#include <iostream>

/** A macro for assert statements */
#include <cassert>
#define FLUIDSIM_ASSERT(x) assert(x)

/** A debugging macro */
#ifdef FLUIDSIM_DEBUG
#  define DBG(x) std::cerr << x << std::endl
#  define DBGHEX(data, len) debug::hexdump(std::cerr, data, len)
#else
#  define DBG(x) ((void) (0))
#  define DBGHEX(data, len)
#endif

/** A macro to print diagnostic messages */
#ifndef FLUIDSIM_NO_WARNINGS
#  define WARN(x) std::cerr << x << std::endl
#else
#  define WARN(x)
#endif

/** Error messages */
#define ERROR(x) std::cerr << x << std::endl

/** Info messages */
#define INFO(x) std::cerr << x << std::endl


namespace debug {

/**
 * This method will print binary data in human readable format.
 * First a line offset is printed, then a hex dump of binary data and
 * finally an ascii dump of those data.
 *
 * @param os an output stream
 * @param *data the data to be printed
 * @param len the length of the data
 * @param line_width width of the binary dump (e.g. how many bytes
 *                   will be printed on a single line)
 *
 * @return a reference to the output passed in as the first argument.
 *         This is to allow chaining these function calls.
 */
std::ostream & hexdump(std::ostream & os,
                       const void *data,
                       uint32_t len,
                       uint32_t line_width = 16);

} // End of namespace Debug

#endif
