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

uniform vec3 light_pos;    // camera space position of the light
uniform vec3 light_col_a;
uniform vec3 light_col_d;
uniform vec3 light_col_s;

uniform vec3 particle_col;  // particle's color

in vec3 o_normal;        // normal to the point on model's surface
in vec3 o_surf_pos;      // the point for which we are calculating the lighting model


void main(void)
{
  //  gl_FragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);

  /* normalize the normal */
  vec3 N = normalize(o_normal);

  /* calculate the light direction and normalize it */
  vec3 L = normalize(light_pos - o_surf_pos);

  /* calculate the cosine between N and L vectors */
  float cos_NL = dot(N, L);
  if (cos_NL < 0.0f) cos_NL = 0.0f;

  /* write the resulting color */
  gl_FragColor = vec4(light_col_a * particle_col + light_col_d * particle_col * cos_NL, 1.0f);
}