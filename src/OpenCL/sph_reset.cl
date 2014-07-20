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

#pragma OPENCL EXTENSION cl_amd_printf : enable


float random(ulong seed, float min, float max)
{
  seed = seed * (seed * (seed * (seed * (seed + 1u) + 1u) + 1u) +1u);
  //printf("sph_reset: random: seed == %u\n", seed);
  float t = (float) (seed) / (float) ((ulong) (-1));
  //printf("sph_reset: random: t == %f\n", t);
  //printf("sph_reset: random: min == %f, max == %f\n", t, min, max);
  return mix(min, max, t);
}


/**
 * A kernel to initialize particle buffers with reasonable default values
 */
__kernel void sph_reset(__global float4 *position,
                        __global float4 *velocity,
                        __global float4 *prev_velocity,
                        __global float *pressure,
                        __global float *density,
                        __global float4 *force,
                        float4 volume_min,
                        float4 volume_max,
                        ulong seed)
{
  ulong gid = get_global_id(0);

  position[gid] = (float4) (random(seed + gid + 1ul, volume_min.x, volume_max.x), 
                            random(seed + gid + 2ul, volume_min.y, volume_max.y),
                            random(seed + gid + 3ul, volume_min.z, volume_max.z),
                            1.0f);
  velocity[gid] = (float4)(0.0f);
  prev_velocity[gid] = (float4)(0.0f);
  pressure[gid] = 0;
  density[gid] = 0;
  force[gid] = (float4)(0.0f);

  //printf("sph_reset: seed == %u\n", seed);
  //printf("sph_reset: position[%u] == [%v4f]\n", gid, position[gid]);
  //printf("sph_reset: position[%u] == [%v4f]\n", gid, velocity[gid]);
  //printf("sph_reset: position[%u] == [%v4f]\n", gid, prev_velocity[gid]);
  //printf("sph_reset: position[%u] == %f\n", gid, pressure[gid]);
  //printf("sph_reset: position[%u] == %f\n", gid, density[gid]);
  //printf("sph_reset: position[%u] == [%v4f]\n", gid, force[gid]);
}