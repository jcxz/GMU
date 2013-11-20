#include "ogl_lib.h"
#include "utils.h"

#include <iostream>
#include <cstdarg>



namespace ogl {

///////////////////////////////////////////////////////////////////////////////
// Error handling

const char *errorToStr(GLenum err)
{
  #define ERROR(e) case e : return #e

  switch (err)
  {
    ERROR(GL_NO_ERROR);
    ERROR(GL_INVALID_ENUM);
    ERROR(GL_INVALID_VALUE);
    ERROR(GL_INVALID_OPERATION);
    ERROR(GL_INVALID_FRAMEBUFFER_OPERATION);
    ERROR(GL_OUT_OF_MEMORY);
  }

  #undef ERROR
  
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

///////////////////////////////////////////////////////////////////////////////
// Texture management

}