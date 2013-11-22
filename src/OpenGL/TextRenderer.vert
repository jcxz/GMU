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
