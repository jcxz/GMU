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

#include "Geom.h"
#include "debug.h"

#include <cmath>
#include <cassert>



///////////////////////////////////////////////////////////////////////////////
// Convention for attribute positions in shaders:
//
// 0 ... position data
// 1 ... normal data
// 2 ... color data
// 3 ... texture coordinates



// Helper functions
namespace {

void *allocVBO(GLuint vbo, unsigned int vertex_count, unsigned int vertex_size)
{
  //assert(glIsBuffer(vbo));

  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, vertex_count * vertex_size, nullptr, GL_STATIC_DRAW);

  return glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
}

}


namespace geom {

bool genSphere(Model & model, float r)
{
  /* helper constants */
  static const float tilt_start = -90.0f;
  static const float tilt_end = 90.0f;
  static const float tilt_step = 10.0f; //90; //10.0f;

  static const float ang_start = 0.0f;
  static const float ang_end = 360.0f;
  static const float ang_step = 10.0f; //30.0f;

  static const int VERTEX_SIZE = 2 * 3 * sizeof(float);

  assert(glIsVertexArray(model.vao));  // vertex array object should be already generated
                                       // at this point (it gets generated in the constructor of GLMesh class)

  /* bind the vertex array object, which will be used
     to tell OpenGL about the format of generated data */
  glBindVertexArray(model.vao);

  /* set up mesh parameters */
  model.mode = GL_TRIANGLE_STRIP;
  model.count = (GLsizei) (((tilt_end - tilt_start) / tilt_step + 1) * ((ang_end - ang_start) / ang_step + 1) * 2); // * 2 because there are two vertices generated each iteration //19 * 13 * 2;

  DBG("((tilt_end - tilt_start + 1) / tilt_step) : " << ((tilt_end - tilt_start) / tilt_step + 1));
  DBG("((ang_end - ang_start + 1) / ang_step)    : " << ((ang_end - ang_start) / ang_step + 1));
  DBG("model.count                               : " << model.count);

#if 0
  /* create a vertex buffer */
  glGenBuffers(1, &g.vbo);
  glBindBuffer(GL_ARRAY_BUFFER, g.vbo);
  glBufferData(GL_ARRAY_BUFFER, g.count * 2 * 3 * sizeof(float), nullptr, GL_STATIC_DRAW);

  /* map the created array to a client pointer */
  float *data = (float *) glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
  if (data == nullptr)
  {
    glDeleteBuffers(1, &g.vbo);
    throw GL_Exception();
  }
#endif

  /* create buffers on GPU */
  float *data = (float *) allocVBO(model.vbo, model.count, VERTEX_SIZE);
  if (data == nullptr)
  {
    ERROR("Failed to allocate buffer for sphere model: " << ogl::errorString());
    return false;
  }

  #define DEGtoRAD (3.141592 / 180.0)

  /* generate sphere vertices */
  for (float tilt = tilt_start; tilt <= tilt_end; tilt += tilt_step)
  {
    for (float ang = ang_start; ang <= ang_end; ang += ang_step)
    {
	  float x = float(sin(ang * DEGtoRAD) * cos(tilt * DEGtoRAD));
	  float y = float(cos(ang * DEGtoRAD) * cos(tilt * DEGtoRAD));
	  float z = float(sin(tilt * DEGtoRAD));
      float x1 = float(sin(ang * DEGtoRAD) * cos((tilt + tilt_step) * DEGtoRAD));
      float y1 = float(cos(ang * DEGtoRAD) * cos((tilt + tilt_step) * DEGtoRAD));
      float z1 = float(sin((tilt + tilt_step) * DEGtoRAD));

      // normal 1
      *data++ = x;      *data++ = y;      *data++ = z;
      // vertex 1
      *data++ = x * r;  *data++ = y * r;  *data++ = z * r;

      // normal 2
      *data++ = x1;     *data++ = y1;     *data++ = z1;
      // vertex 2
      *data++ = x1 * r; *data++ = y1 * r; *data++ = z1 * r;
	}
  }
  
  #undef DEGtoRAD

  /* unmap the vertex buffer */
  glUnmapBuffer(GL_ARRAY_BUFFER);

  /* tell OpenGL about the format of vertices */
  glEnableVertexAttribArray(0);  // position 0 will allways be vertex position in our shaders
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, VERTEX_SIZE, (void *) (0));

  glEnableVertexAttribArray(1);  // position 1 will allways be vertex normal in our shaders
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, VERTEX_SIZE, (void *) (sizeof(float) * 3));

  /* unbind the vertex and buffer objects */
  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  return true;
}


bool genPrism(Model & model, float a, float b, float c)
{
  static const float vertices[][3] = {
    { -1.0f,  1.0f,  1.0f },  // 0
    {  1.0f,  1.0f,  1.0f },  // 1
    { -1.0f,  1.0f, -1.0f },  // 3

    {  1.0f,  1.0f,  1.0f },  // 1
    {  1.0f,  1.0f, -1.0f },  // 2
    { -1.0f,  1.0f, -1.0f },  // 3

    {  1.0f,  1.0f,  1.0f },  // 1
    {  1.0f, -1.0f,  1.0f },  // 5
    {  1.0f,  1.0f, -1.0f },  // 2

    {  1.0f, -1.0f,  1.0f },  // 5
    {  1.0f, -1.0f, -1.0f },  // 6
    {  1.0f,  1.0f, -1.0f },  // 2

    {  1.0f, -1.0f,  1.0f },  // 5
    { -1.0f, -1.0f,  1.0f },  // 4
    {  1.0f, -1.0f, -1.0f },  // 6

    { -1.0f, -1.0f,  1.0f },  // 4
    { -1.0f, -1.0f, -1.0f },  // 7
    {  1.0f, -1.0f, -1.0f },  // 6

    { -1.0f, -1.0f,  1.0f },  // 4
    { -1.0f,  1.0f,  1.0f },  // 0
    { -1.0f, -1.0f, -1.0f },  // 7

    { -1.0f,  1.0f,  1.0f },  // 0
    { -1.0f,  1.0f, -1.0f },  // 3
    { -1.0f, -1.0f, -1.0f },  // 7

    { -1.0f,  1.0f, -1.0f },  // 3
    {  1.0f,  1.0f, -1.0f },  // 2
    { -1.0f, -1.0f, -1.0f },  // 7

    {  1.0f,  1.0f, -1.0f },  // 2
    {  1.0f, -1.0f, -1.0f },  // 6
    { -1.0f, -1.0f, -1.0f },  // 7

    { -1.0f, -1.0f,  1.0f },  // 4
    {  1.0f, -1.0f,  1.0f },  // 5
    { -1.0f,  1.0f,  1.0f },  // 0

    {  1.0f, -1.0f,  1.0f },  // 5
    {  1.0f,  1.0f,  1.0f },  // 1
    { -1.0f,  1.0f,  1.0f },  // 0
  };

  model.mode = GL_TRIANGLES;
  model.count = sizeof(vertices) / sizeof(*vertices);

  assert(glIsVertexArray(model.vao));  // vertex array object should be already generated
                                       // at this point (it gets generated in the constructor of GLMesh class)

  /* bind the vertex array object, which will be used
     to tell OpenGL about the format of generated data */
  glBindVertexArray(model.vao);

#if 0
  /* create a vertex buffer */
  glGenBuffers(1, &mesh.vbo);
  glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
#endif

  /* create buffers on GPU */
  float *data = (float *) allocVBO(model.vbo, model.count, sizeof(*vertices));
  if (data == nullptr)
  {
    ERROR("Failed to allocate buffer for sphere mesh: " << ogl::errorString());
    return false;
  }

  /* generate the prism */
  float a_side = a / 2.0f;  // the original coordinates go from -1 to 1, a defines the length of a side
  float b_side = b / 2.0f;
  float c_side = c / 2.0f;

  for (int i = 0; i < model.count; ++i)
  {
    *data++ = vertices[i][0] * a_side;
    *data++ = vertices[i][1] * b_side;
    *data++ = vertices[i][2] * c_side;
  }

  /* unmap the vertex buffer */
  glUnmapBuffer(GL_ARRAY_BUFFER);

  /* tell OpenGL about the format of vertices */
  glEnableVertexAttribArray(0);  // position 0 will allways be vertex position in our shaders
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(*vertices), (void *) (0));

  /* unbind the vertex buffer */
  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  return true;
}


bool gen2DTriangle(Model & model)
{
  float vertices[][2] = {
    { -1.0f, -1.0f },
    {  1.0f, -1.0f },
    {  1.0f,  1.0f },
  };

  /* bind the vertex array object, which will be used
     to tell OpenGL about the format of generated data */
  glBindVertexArray(model.vao);

  model.mode = GL_TRIANGLES;
  model.count = sizeof(vertices) / sizeof(*vertices);

  /* create a vertex buffer */
  glBindBuffer(GL_ARRAY_BUFFER, model.vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  /* tell OpenGL about the format of vertices */
  glEnableVertexAttribArray(0);  // position 0 will allways be vertex position in our shaders
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(*vertices), (void *) (0));

  /* unbind the vertex buffer */
  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  return true;
}


bool gen2DRectangle(Model & model, float w, float h)
{
  float w2 = w / 2.0f;
  float h2 = h / 2.0f;
  float vertices[][2][2] = {
    //  position  ,   texture coordinates
    { { -w2, -h2 }, { 0.0f, 1.0f } },
    { {  w2, -h2 }, { 1.0f, 1.0f } },
    { {  w2,  h2 }, { 1.0f, 0.0f } },
    { { -w2,  h2 }, { 0.0f, 0.0f } }
  };

  /* bind the vertex array object, which will be used
     to tell OpenGL about the format of generated data */
  glBindVertexArray(model.vao);

  model.mode = GL_TRIANGLE_FAN;
  model.count = sizeof(vertices) / sizeof(*vertices);

  /* create a vertex buffer */
  glBindBuffer(GL_ARRAY_BUFFER, model.vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  /* tell OpenGL about the format of vertices */
  glEnableVertexAttribArray(0);  // position 0 will allways be vertex position in our shaders
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(*vertices), (void *) (0));

  glEnableVertexAttribArray(3);  // position 3 will allways be the texture coordinates in our shaders
  glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(*vertices), (void *) (2 * sizeof(float)));

  /* unbind the vertex buffer */
  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  return true;
}

}