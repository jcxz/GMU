#pragma OPENCL EXTENSION cl_amd_printf : enable

#if 0
float random(unsigned long *seed, float min, float max)
{
  *seed = *seed * 69069L + 1;               // modulo 2^32 je implicitne
  return (*seed / ((float) ULONG_MAX + 1)) * (max - min) + min; // konverzia do <0, 1)
}
#elif 1
void srand(ulong *seed, ulong gid)
{
  *seed = (*seed) * (gid + 1) * 69069L + 1;
}

float random(ulong *seed, float min, float max)
{
  *seed = *seed * 69069L + 1;               // modulo 2^32 je implicitne
  return (((float) (*seed)) / (((float) ULONG_MAX) + 1))
         * (max - min) + min; // konverzia do <0, 1)
}
#else
ulong srand(ulong *seed, ulong gid)
{
  *seed = (*seed) * (gid + 1) * 69069L + 1;
}

float random(ulong *seed, float min, float max)
{
  printf("KERNEL: *seed = %u\n", *seed);
  *seed = *seed * 69069L + 1;               // modulo 2^32 je implicitne
  float res = (((float) (*seed)) / (((float) ULONG_MAX) + 1)); // konverzia do <0, 1)
  printf("KERNEL: *seed = %u, res = %10f\n", *seed, res);
  return res;
}
#endif

#if 0
__constant float3 part_positions[] = {
  (float3) (-10.0f, -10.0f, -1.0f),
  (float3) ( 10.0f, -10.0f, -1.0f),
  (float3) ( 10.0f,  10.0f, -1.0f),
  (float3) (-10.0f,  10.0f, -1.0f),
  (float3) (  0.0f,   0.0f, -1.0f),
};
#endif


__kernel void gen_part_positions(__global float3 *positions, ulong seed)
{
  ulong gid = get_global_id(0);
  //unsigned long seed = (gid + 1) << 16;
  
  // discard the first random number (because it is a kind of crappy)
  srand(&seed, gid);
  
  positions[gid] = (float3) (random(&seed, -100, 100),
                             random(&seed, -100, 100),
                             random(&seed, -30,  0));
  
#if 0
  printf("KERNEL: positions[%lu] = [%f, %f, %f]\n",
         gid,
         positions[gid].s0,
         positions[gid].s1,
         positions[gid].s2);
#endif                                                      
  
  //positions[gid] = (float3) (rand(&seed), rand(&seed), -(rand(&seed)));
  //positions[gid] = (float3) (0.0f, 0.0f, -1.0f);
  
  //positions[gid] = (float3) (part_positions[gid + 0], part_positions[gid + 1], part_positions[gid + 2]);
  //positions[gid] = part_positions[gid]; 
}