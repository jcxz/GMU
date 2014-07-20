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

#include "ogl_lib.h"
#include "sdl_libs.h"
#include "utils.h"
#include "debug.h"

#include <iostream>
#include <cstdarg>




namespace ogl {

///////////////////////////////////////////////////////////////////////////////
// Error handling

const char *errorToStr(GLenum err)
{
  #define CASE(e) case e: return #e

  switch (err)
  {
    CASE(GL_NO_ERROR);
    CASE(GL_INVALID_ENUM);
    CASE(GL_INVALID_VALUE);
    CASE(GL_INVALID_OPERATION);
    CASE(GL_INVALID_FRAMEBUFFER_OPERATION);
    CASE(GL_OUT_OF_MEMORY);
  }

  #undef CASE
  
  return "UNKNOWN_GL_ERROR";
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

///////////////////////////////////////////////////////////////////////////////
// Shader management

std::string getInfoLog(GLuint id, PFNGLGETSHADERIVPROC getLen, PFNGLGETSHADERINFOLOGPROC getLog)
{
  assert(getLen != NULL);
  assert(getLog != NULL);

  /* query the info log length */
  GLint length = -1;
  getLen(id, GL_INFO_LOG_LENGTH, &length);
  assert(length >= 0);

  /* get the actual info log */
  std::string log(length, '\0');
  getLog(id, length, NULL, &log[0]);

  assert(glGetError() == GL_NO_ERROR);

  return log;
}

#if 0
GLuint compileShader(GLenum type, const char *source)
{
  /* create shader object */
  GLuint shader = glCreateShader(type);
  if (shader == 0)
  {
    std::cerr << "Failed to create shader object: "
              << errorToStr(glGetError())
              << std::endl;
    return 0;
  }

  /* copy the sources to the shader object */
  glShaderSource(shader, 1, &source, NULL);

  /* compile the sources */
  glCompileShader(shader);

  /* query compilation status */
  GLint status = GL_FALSE;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &status);

  /* print compilation log */
  std::cerr << "Compilation status: " << (status == GL_TRUE ? "succeeded" : "failed") << std::endl;
  std::cerr << getShaderInfoLog(shader) << std::endl;

  /* check compilation status */
  if (status == GL_FALSE)
  {
    glDeleteShader(shader);
    return 0;
  }

  return shader;
}


GLuint compileShaderFile(GLenum type, const char *source_file)
{
  std::string source;
  if (!utils::loadFile(source_file, &source))
  {
    return 0;
  }

  return compileShader(type, source.c_str());
}


GLuint linkProgram(unsigned int count, ...)
{
  /* create program object */
  GLuint program = glCreateProgram();
  if (program == 0)
  {
    std::cerr << "Failed to create shader program object: "
              << errorToStr(glGetError())
              << std::endl;
    return 0;
  }

  /* attach shaders */
  va_list args;
  va_start(args, count);
  for (unsigned int i = 0; i < count; ++i)
  {
    glAttachShader(program, va_arg(args, GLuint));
  }
  va_end(args);

  /* link program */
  glLinkProgram(program);

  /* query link status */
  GLint status = GL_FALSE;
  glGetProgramiv(program, GL_LINK_STATUS, &status);

  /* print linking log */
  std::cerr << "Linking status: " << (status == GL_TRUE ? "succeeded" : "failed") << std::endl;
  std::cerr << getProgramInfoLog(program) << std::endl;

  /* check linking status */
  if (status == GL_FALSE)
  {
    glDeleteProgram(program);
    return 0;
  }

  return program;
}


GLuint buildProgram(const char *vert_shader_source, const char *frag_shader_source)
{
  /* compile the vertex shader */
  GLuint vert_shader = compileShader(GL_VERTEX_SHADER, vert_shader_source);
  if (vert_shader == 0)
  {
    return 0;
  }

  /* compile the fragment shader */
  GLuint frag_shader = compileShader(GL_FRAGMENT_SHADER, frag_shader_source);
  if (frag_shader == 0)
  {
    glDeleteShader(vert_shader);
    return 0;
  }

  /* link the shaders together */
  GLuint program = linkProgram(2, vert_shader, frag_shader);

  /* delete shader objects, so that when user destroys the program object,
     the shaders will be freed too */
  glDeleteShader(frag_shader);
  glDeleteShader(vert_shader);

  return program;
}


GLuint buildProgramFiles(const char *vert_shader_file, const char *frag_shader_file)
{
  /* compile the vertex shader */
  GLuint vert_shader = compileShaderFile(GL_VERTEX_SHADER, vert_shader_file);
  if (vert_shader == 0)
  {
    return 0;
  }

  /* compile the fragment shader */
  GLuint frag_shader = compileShaderFile(GL_FRAGMENT_SHADER, frag_shader_file);
  if (frag_shader == 0)
  {
    glDeleteShader(vert_shader);
    return 0;
  }

  /* link the shaders together */
  GLuint program = linkProgram(2, vert_shader, frag_shader);

  /* delete shader objects, so that when user destroys the program object,
     the shaders will be freed too */
  glDeleteShader(frag_shader);
  glDeleteShader(vert_shader);

  return program;
}
#endif


bool ShaderProgram::attachShader(GLenum type, const char *source, GLuint *shader_id)
{
  assert(glIsProgram(m_program));

  /* create shader object */
  GLuint shader = glCreateShader(type);
  if (shader == 0)
  {
    ERROR("Failed to create shader object: " << errorToStr(glGetError()));
    return false;
  }

  /* copy the sources to the shader object */
  glShaderSource(shader, 1, &source, NULL);

  /* compile the sources */
  glCompileShader(shader);

  /* query compilation status */
  GLint status = GL_FALSE;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &status);

  /* print compilation log */
  std::cerr << "Compilation status: " << (status == GL_TRUE ? "succeeded" : "failed") << std::endl;
  std::cerr << getShaderInfoLog(shader) << std::endl;

  /* check compilation status */
  if (status == GL_FALSE)
  {
    glDeleteShader(shader);
    return false;
  }

  /* attach the shader to program */
  glAttachShader(m_program, shader);

  if (shader_id == nullptr)
  {
    glDeleteShader(shader);
  }
  else
  {
    *shader_id = shader;
  }

  return true;
}


bool ShaderProgram::attachShaderFile(GLenum type, const char *source_file, GLuint *shader_id)
{
  std::string source;
  if (!utils::fs::loadFile(source_file, &source))
  {
    return false;
  }

  return attachShader(type, source.c_str(), shader_id);
}


bool ShaderProgram::link(void)
{
  assert(glIsProgram(m_program));

  /* link program */
  glLinkProgram(m_program);

  /* query link status */
  GLint status = GL_FALSE;
  glGetProgramiv(m_program, GL_LINK_STATUS, &status);

  /* print linking log */
  std::cerr << "Linking status: " << (status == GL_TRUE ? "succeeded" : "failed") << std::endl;
  std::cerr << getProgramInfoLog(m_program) << std::endl;

  /* check linking status */
  return status != GL_FALSE;
}

///////////////////////////////////////////////////////////////////////////////
// Texture management

bool Texture::load(GLsizei w, GLsizei h,
                   const GLvoid *pixels, 
                   GLint src_format,
                   GLenum src_type,
                   GLenum dest_format,
                   bool mipmapped)
{
  //assert(glIsTexture(m_id));
  assert(pixels != nullptr);

  glBindTexture(GL_TEXTURE_2D, m_id);

  glTexImage2D(GL_TEXTURE_2D, 0, dest_format, w, h, 0, src_format, src_type, pixels);

  if (mipmapped)
  {
    glGenerateMipmap(GL_TEXTURE_2D);
  }
  else
  {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  }

  GLenum err = glGetError();

  glBindTexture(GL_TEXTURE_2D, 0);

  if (err != GL_NO_ERROR)
  {
    ERROR("Failed to store texture data on GPU: " << errorToStr(err));
    return false;
  }

  return true;
}


bool Texture::load(SDL_Surface *surf, bool mipmapped)
{
  assert(surf != nullptr);

  /* convert the surface to RGBA format */
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
  uint32_t rmask = 0xff000000;
  uint32_t gmask = 0x00ff0000;
  uint32_t bmask = 0x0000ff00;
  uint32_t amask = 0x000000ff;
#else
  uint32_t rmask = 0x000000ff;
  uint32_t gmask = 0x0000ff00;
  uint32_t bmask = 0x00ff0000;
  uint32_t amask = 0xff000000;
#endif

  SDL_Surface *tmp = SDL_CreateRGBSurface(0, surf->w, surf->h, 32, rmask, gmask, bmask, amask);
  if (tmp == nullptr)
  {
    ERROR("Failed to create temporaty surface to perform pixel format conversion.");
    return false;
  }
  
  SDL_BlitSurface(surf, NULL, tmp, NULL);

  /* copy the surface to GPU */
  bool res = load(tmp->w, tmp->h, tmp->pixels, GL_RGBA, GL_UNSIGNED_BYTE, GL_RGBA, mipmapped);

  SDL_FreeSurface(tmp);

  return res;
}


bool Texture::load(const char *tex_file, bool mipmapped)
{
  assert(tex_file != nullptr);

  SDL_Surface *surf = IMG_Load(tex_file);
  if (surf == nullptr)
  {
    ERROR("Failed to load texture from file: " << tex_file << ": " << IMG_GetError());
    return false;
  }

  bool res = load(surf, mipmapped);

  SDL_FreeSurface(surf);

  return res;
}

///////////////////////////////////////////////////////////////////////////////
// Various utility functions

const char *primitiveToStr(GLenum primitive)
{
  #define CASE(p) case p: return #p

  switch (primitive)
  {
    CASE(GL_POINTS);
    CASE(GL_LINE_STRIP);
    CASE(GL_LINE_LOOP);
    CASE(GL_LINES);
    CASE(GL_LINE_STRIP_ADJACENCY);
    CASE(GL_LINES_ADJACENCY);
    CASE(GL_TRIANGLE_STRIP);
    CASE(GL_TRIANGLE_FAN);
    CASE(GL_TRIANGLES);
    CASE(GL_TRIANGLE_STRIP_ADJACENCY);
    CASE(GL_TRIANGLES_ADJACENCY);
    CASE(GL_PATCHES);
  }

  #undef CASE

  return "Unknown primitive type";
}

}