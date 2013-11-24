//#define ULONG_MAX 4294967295 

float rand(unsigned long *seed)
{
  *seed = *seed * 69069L + 1; // modulo 2^32 je implicitne
  return (*seed / ((float) ULONG_MAX + 1)); // konverzia do <0, 1)
}


__constant float3 part_positions[] = {
  (float3) (-10.0f, -10.0f, -1.0f),
  (float3) ( 10.0f, -10.0f, -1.0f),
  (float3) ( 10.0f,  10.0f, -1.0f),
  (float3) (-10.0f,  10.0f, -1.0f),
  (float3) (  0.0f,   0.0f, -1.0f),
};


__kernel void gen_part_positions(__global float3 *positions)
{
  unsigned long gid = get_global_id(0);
  //unsigned long seed = gid;
  
  //positions[gid] = (float3) (rand(&seed), rand(&seed), -(rand(&seed)));
  //positions[gid] = (float3) (0.0f, 0.0f, -1.0f);
  
  //positions[gid] = (float3) (part_positions[gid + 0], part_positions[gid + 1], part_positions[gid + 2]);
  positions[gid] = part_positions[gid]; 
}