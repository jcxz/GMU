#include "ogl_lib.h"
#include "utils.h"
#include "debug.h"
#include "sdl_libs.h"

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
  if (!utils::loadFile(source_file, &source))
  {
    return false;
  }

  return attachShaderFile(type, source.c_str(), shader_id);
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
  assert(glIsTexture(m_id));

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
    ERROR("Failed to stora texture data on GPU: " << errorToStr(err));
    return false;
  }

  return true;
}


bool Texture::load(const char *tex_file, bool mipmapped)
{
  SDL_Surface *surf = IMG_Load(tex_file);
  if (surf == nullptr)
  {
    ERROR("Failed to load texture from file: " << tex_file << ": " << IMG_GetError());
    return false;
  }

  // TODO: convert the SDL_PIXELFORMAT enum to OpenGL enum,
  // so that the conversion is not necessary in most cases
  if (surf->format->format != SDL_PIXELFORMAT_RGBA8888)
  {
    SDL_Surface *tmp = SDL_ConvertSurfaceFormat(surf, SDL_PIXELFORMAT_RGBA8888, 0);
    if (tmp == nullptr)
    {
      ERROR("Failed to convert loaded texture to 32-bit RGBA format: " << IMG_GetError());
      SDL_FreeSurface(tmp);
      return false;
    }
    SDL_FreeSurface(surf);
    surf = tmp;
  }

  bool res = load(surf->w, surf->h, surf->pixels, GL_RGBA, GL_UNSIGNED_BYTE, GL_RGBA, mipmapped);

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