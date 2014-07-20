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

/*
inline ulong srand(ulong seed, ulong gid)
{
  return seed * ((gid << 1) + 1) * 69069L + 1;
}

inline float random(ulong *seed, float min, float max)
{
  *seed = *seed * 69069L + 1;               // modulo 2^32 je implicitne
  return ((float) (*seed)) / (((float) ULONG_MAX) + 1); // konverzia do <0, 1)
}
*/


__kernel void polar_spiral(__global float4 *positions,
                           __global float4 *colors,
                           float3 position,
                           ulong seed)
{
  uint gid = get_global_id(0);
  float gidf = gid * 0.1;
  
  //seed = srand(seed, gid);
  srand(&seed, gid << 1);
  
  positions[gid] = (float4) (native_sin(gidf) * (gidf * 1.5),
                             native_cos(gidf) * gidf,
                             -2.0f * gidf,
                             1.0f);
  
  colors[gid] = (float4) (random(&seed, 0.1f, 1.0f),
                          random(&seed, 0.1f, 1.0f),
                          random(&seed, 0.1f, 1.0f),
                          1.0f);
} 