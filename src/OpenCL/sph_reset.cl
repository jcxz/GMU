
float random(ulong seed, float min, float max)
{
  seed = seed * (seed * (seed * (seed * (seed + 1u) + 1u) + 1u) +1u);
  float t = (float) (seed) / (float) ((ulong) (-1));
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
}