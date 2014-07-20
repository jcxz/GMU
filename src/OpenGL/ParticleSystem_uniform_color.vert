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

#version 330

uniform mat4 mv;           // model-view matrix for vertices
uniform mat3 mv_normal;    // model-view matrix for normals
uniform mat4 proj;         // projection matrix

layout(location = 0) in vec3 pos;           // model's vertex position
layout(location = 1) in vec3 normal;        // model's normal
layout(location = 4) in vec3 particle_pos;  // particle's position

out vec3 o_normal;
out vec3 o_surf_pos;


void main(void)
{
  /* transform the normal top camera space */
  o_normal = mv_normal * normal;

  /* transform the vertex to camera space and get the incidence position on the odel surface */
  vec4 tmp = mv * vec4(pos + particle_pos, 1.0f);
  o_surf_pos = tmp.xyz;

  /* calculate the clip-space position of the vertex */
  gl_Position = proj * tmp;
}