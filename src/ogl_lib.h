/**
 * This header file servers as a wrapper file around OpenGL's header file,
 * plus it provides some usefull utility functions
 */

#ifndef OGL_LIB_H
#define OGL_LIB_H

#include <GL/glew.h>

#include <stdexcept>
#include <string>
#include <cassert>


namespace ogl {

///////////////////////////////////////////////////////////////////////////////
// Error handling

/** A function to convert OpenGL error enum to a string */
const char *errorToStr(GLenum err);

/** A function to directly return an OpenGL error string */
inline const char *errorString(void)
{
  return errorToStr(glGetError());
}

/** An OpenGL exception type */
struct Exception : std::runtime_error
{
  explicit Exception(GLenum err = glGetError()) throw()
    : std::runtime_error(errorToStr(err))
  {
  }

  explicit Exception(const std::string & msg, GLenum err = glGetError()) throw()
    : std::runtime_error(msg + ": " + errorToStr(err))
  {
  }
};

///////////////////////////////////////////////////////////////////////////////
// Shader management

std::string getInfoLog(GLuint id, PFNGLGETSHADERIVPROC getLen, PFNGLGETSHADERINFOLOGPROC getLog);

// Info logs contain errors and warnings from shader compilation and linking
inline std::string getShaderInfoLog(GLuint shader)
{
  assert(glIsShader(shader));
  return getInfoLog(shader, glGetShaderiv, glGetShaderInfoLog);
}

inline std::string getProgramInfoLog(GLuint program)
{
  assert(glIsProgram(program));
  return getInfoLog(program, glGetProgramiv, glGetProgramInfoLog);
}

GLuint compileShader(GLenum type, const char *source);
GLuint compileShaderFile(GLenum type, const char *source_file);

GLuint linkProgram(unsigned int count, ...);
/* other shader types could be added as optional argments */
GLuint buildProgram(const char *vert_shader_source, const char *frag_shader_source);
/* other shader types could be added as optional argments */
GLuint buildProgramFiles(const char *vert_shader_file, const char *frag_shader_file);

///////////////////////////////////////////////////////////////////////////////
// Texture management
GLuint loadTexture(GLsizei w, GLsizei h,
                   GLint src_format,
                   const GLvoid *pixels, 
                   GLenum src_type = GL_UNSIGNED_BYTE,
                   GLenum dest_format = GL_RGBA,
                   bool mipmapped = true);
GLuint loadTexture(const char *tex_file, bool mipmapped = true);

}

#endif