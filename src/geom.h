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

#ifndef GEOM_H
#define GEOM_H

#include "ogl_lib.h"

#include <ostream>


namespace geom {

// class encapsulating information on mesh geometry
struct Model
{
  GLenum mode;    /// what type of primitives to render
  GLsizei count;  /// number of vertices
  GLuint vbo;     /// vertex buffer object handle
  GLuint vao;     /// vertex array object handle

  Model(void)
    : mode(GL_TRIANGLES),
      count(0),
      vbo(0),
      vao(0)
  {
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
  }

  ~Model(void)
  {
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
  }

  friend std::ostream & operator<<(std::ostream & os, const Model & geom)
  {
    return os << "Model(" << ogl::primitiveToStr(geom.mode) << ", "
              << geom.count << ", "
              << geom.vbo << ", "
              << geom.vao << ")";
  }
};


/**
 * Generates sphere vertices and loads them to GPU
 *
 * @param mesh mesh object where the information about loaded geometry will be stored
 * @param r radius of the sphere
 *
 * @return true when the geometry has been successfully loaded to GPU, false otherwise
 */
bool genSphere(Model & model, float r = 1.0f);


/**
 * Generates prism vertices and loads them to GPU
 *
 * @param mesh mesh object where the information about loaded geometry will be stored
 * @param a first side of the prism
 * @param a third side of the prism
 * @param a second side of the prism
 *
 * @return true when the geometry has been successfully loaded to GPU, false otherwise
 */
bool genPrism(Model & model, float a = 2.0f, float b = 2.0f, float c = 2.0f);


/**
 */
bool gen2DTriangle(Model & model);

/**
 * Generates square vertices (position and texture coordinates) and loads them to GPU
 */
bool gen2DRectangle(Model & model, float w = 2.0f, float h = 2.0f);

}

#endif