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
 * This header file servers as a wrapper file around OpenGL's header file,
 * plus it provides some usefull utility functions
 */

#ifndef OGL_LIB_H
#define OGL_LIB_H

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX               // disable min, max macros from Windows headers

#include <GL/glew.h>

#include <stdexcept>
#include <string>
#include <cassert>


// Forward declartions
typedef struct SDL_Surface SDL_Surface;


// Interface definition
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

/**
 * Function to pretty format the message from ARB debug callback
 *
 * @param source The source that produced the message.
 * @param type The type of message.
 * @param id message id
 * @param severity severity of the message
 * @param msg the message from debug callback
 * @param out_str the output string
 * @param out_str_size the maximum size of the output string
 */
void formatDebugOutputARB(GLenum source, GLenum type,
                          GLuint id, GLenum severity,
                          const char *msg,
                          char *out_str, size_t out_str_size);

void formatDebugOutputAMD(GLuint id, GLenum category, GLenum severity,
                          const char *msg,
                          char *out_str, size_t out_str_size);

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

#if 0
GLuint compileShader(GLenum type, const char *source);
GLuint compileShaderFile(GLenum type, const char *source_file);

GLuint linkProgram(unsigned int count, ...);
/* other shader types could be added as optional argments */
GLuint buildProgram(const char *vert_shader_source, const char *frag_shader_source);
/* other shader types could be added as optional argments */
GLuint buildProgramFiles(const char *vert_shader_file, const char *frag_shader_file);
#endif

class ShaderProgram
{
  public:
    ShaderProgram(void)
      : m_program(glCreateProgram())
    {
      if (m_program == 0) throw Exception();
    }

    ShaderProgram(ShaderProgram && other)
    {
      m_program = other.m_program;
      other.m_program = 0;
    }

    ~ShaderProgram(void)
    {
      assert(glIsProgram(m_program));
      glDeleteProgram(m_program);
    }

    ShaderProgram & operator=(ShaderProgram && other)
    {
      // Note: this algorithm handles self assignment too
      GLuint tmp = other.m_program;
      other.m_program = m_program;
      m_program = tmp;
      return *this;
    }

    GLuint getID(void) const
    {
      assert(glIsProgram(m_program));
      return m_program;
    }

    void use(void)
    {
      assert(glIsProgram(m_program));
      glUseProgram(m_program);
    }

    void detachShader(GLuint shader_id)
    {
      assert(glIsProgram(m_program));
      glDetachShader(m_program, shader_id);
    }
    
    // attaches a given shader to program
    bool attachShader(GLuint shader_id)
    {
      assert(glIsProgram(m_program));
      glAttachShader(m_program, shader_id);
    }

    /**
     * compiles the shader and attaches it to the program
     *
     * @return shader id on success, 0 on failure
     */
    bool attachShader(GLenum type, const char *source, GLuint *shader_id = nullptr);

    /**
     * compiles the given shader file and attaches it to the program
     *
     * @return shader id on success, 0 on failure
     */
    bool attachShaderFile(GLenum type, const char *source_file, GLuint *shader_id = nullptr);

    // relinks the program
    bool link(void);

    // compiles vertex and fragment shader from given sources and links them
    // with program
    bool build(const char *vert_shader_source, const char *frag_shader_source)
    {
      return attachShader(GL_VERTEX_SHADER, vert_shader_source)   &&
             attachShader(GL_FRAGMENT_SHADER, frag_shader_source) &&
             link();
    }

    // compiles given vertex and fragment shader files and links them with program
    bool buildFiles(const char *vert_shader_file, const char *frag_shader_file)
    {
      return attachShaderFile(GL_VERTEX_SHADER, vert_shader_file)   &&
             attachShaderFile(GL_FRAGMENT_SHADER, frag_shader_file) &&
             link();
    }

  private:
    ShaderProgram(const ShaderProgram & );
    ShaderProgram & operator=(const ShaderProgram & );

  private:
    GLuint m_program;
};

///////////////////////////////////////////////////////////////////////////////
// Texture management

/**
 * Classic 2D texture
 */
class Texture
{
  public:
    Texture(void)
      : m_id(0)
    {
      glGenTextures(1, &m_id);
      GLenum err = glGetError();
      if (err != GL_NO_ERROR) throw Exception("Failed to construct Texture", err);
    }

    Texture(Texture && other)
    {
      m_id = other.m_id;
      other.m_id = 0;
    }

    ~Texture(void)
    {
      //assert(glIsTexture(m_id));
      glDeleteTextures(1, &m_id);
    }

    Texture & operator=(Texture && other)
    {
      // just swap the two values, this handles selfassignment as well
      GLuint tmp = other.m_id;
      other.m_id = m_id;
      m_id = tmp;
      return *this;
    }

    GLuint getID(void)
    {
      //assert(glIsTexture(m_id));
      return m_id;
    }

    void bind(void)
    {
      //assert(glIsTexture(m_id));
      glBindTexture(GL_TEXTURE_2D, m_id);
    }

    bool load(GLsizei w, GLsizei h,
              const GLvoid *pixels, 
              GLint src_format = GL_RGBA,
              GLenum src_type = GL_UNSIGNED_BYTE,
              GLenum dest_format = GL_RGBA,
              bool mipmapped = true);
    bool load(SDL_Surface *surf, bool mipmapped = true);
    bool load(const char *tex_file, bool mipmapped = true);

  private:
    // No copying
    Texture(const Texture & );
    Texture & operator=(Texture & );

  private:
    GLuint m_id;   /// texture id
};

///////////////////////////////////////////////////////////////////////////////
// Various utility functions

/** returns a string name for a given OpenGL primtive kind */
const char *primitiveToStr(GLenum primitive);

}

#endif