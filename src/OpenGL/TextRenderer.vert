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

uniform float x;
uniform float y;
uniform float w;
uniform float h;

layout(location = 0) in vec2 pos;
layout(location = 3) in vec2 tex_coordi;

out vec2 tex_coord;

void main(void)
{
  gl_Position = vec4(pos.x * w + x, pos.y * h + y, -0.2f, 1.0f);
  tex_coord = tex_coordi;
}
