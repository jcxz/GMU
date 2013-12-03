#version 330

uniform mat4 mv;           // model-view matrix for vertices
uniform mat3 mv_normal;    // model-view matrix for normals
uniform mat4 proj;         // projection matrix

layout(location = 0) in vec3 pos;           // model's vertex position
layout(location = 1) in vec3 normal;        // model's normal
layout(location = 4) in vec3 particle_pos;  // particle's position
layout(location = 5) in vec3 particle_col;  // particle's color

out vec3 o_normal;
out vec3 o_surf_pos;
out vec3 o_particle_col;


void main(void)
{
  /* transform the normal top camera space */
  o_normal = mv_normal * normal;

  /* transform the vertex to camera space and get the incidence position on the odel surface */
  vec4 tmp = mv * vec4(pos + particle_pos, 1.0f);
  o_surf_pos = tmp.xyz;

  /* calculate the clip-space position of the vertex */
  gl_Position = proj * tmp;

  /* calculate the particle color */
  o_particle_col = particle_col;
  
  // gl_Position = 0.005 * vec4(pos + particle_pos, 1.0f);
  // gl_Position = vec4((pos + particle_pos) * 0.02, 1.0f);
}