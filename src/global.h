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
 * Globally applicable definitions and useful macros
 */

#ifndef GLOBAL_H
#define GLOBAL_H

/** OS detection */
#if defined(__FreeBSD__) || defined(__NetBSD__) || \
    defined(__OpenBSD__) || defined(__bsdi__)   || \
    defined(__DragonFly__)
# define FLUIDSIM_OS_BSD
#elif defined(_WIN32) || defined(__WIN32__) || defined(__TOS_WIN__) || defined(__WINDOWS__)
# define FLUIDSIM_OS_WIN
#elif defined(__APPLE__) || defined(__MACH__)
# define FLUIDSIM_OS_MAC
#elif defined(__linux__) || defined(linux) || defined(__linux)
# define FLUIDSIM_OS_LINUX
#else
# error Unsupported operating system
#endif

/** Compiler detection */
#if defined(_MSC_VER)
# define FLUIDSIM_CC_MSVC
# if (_MSC_VER >= 1600 && _MSC_VER < 1700)
#  define FLUIDSIM_CC_MSVC2010
# elif (_MSC_VER >= 1500 && _MSC_VER < 1600)
#  define FLUIDSIM_CC_MSVC2008
# endif
#elif defined(__GNUC__)
# define FLUIDSIM_CC_GCC
#else
# error Unsupported compiler
#endif

/**
 * Structure packing macro
 * @see http://stackoverflow.com/questions/1537964/visual-c-equivalent-of-gccs-attribute-packed
 */
#if defined(FLUIDSIM_CC_MSVC)
# define FLUIDSIM_PACKED_STRUCT(decl) __pragma(pack(push, 1)) decl __pragma(pack(pop))
#elif defined(FLUIDSIM_CC_GCC)
# define FLUIDSIM_PACKED_STRUCT(decl) decl __attribute__((packed))
#else
# define FLUIDSIM_PACKED_STRUCT(decl) decl
#endif

/** Define portable line, function, and file macros */
# define FLUIDSIM_FILE __FILE__
# define FLUIDSIM_LINE __LINE__
# define FLUIDSIM_FUNC_PLAIN __FUNCTION__
#if defined(FLUIDSIM_CC_MSVC)
# define FLUIDSIM_FUNC __FUNCSIG__
#elif defined(FLUIDSIM_CC_GCC)
# define FLUIDSIM_FUNC __PRETTY_FUNCTION__
#else
# define FLUIDSIM_FUNC __FUNCTION__
#endif

/** Silence noisy but otherwise useless MSVC warnings */
#if defined(FLUIDSIM_CC_MSVC)
# define _CRT_SECURE_NO_WARNINGS
#endif

/** A macro to designate unused parameters */
#define FLUIDSIM_UNUSED(x) ((void) (x))

#endif
