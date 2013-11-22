#version 330

uniform sampler2D tex;

in vec2 tex_coord;

void main(void)
{
  gl_FragColor = texture(tex, tex_coord);
}