/*
 * Copyright (C) 2008. Rama Hoetzlein, http://www.rchoetzlein.com
 *               2014 Matus Fedorko <xfedor01@stud.fit.vutbr.cz>
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

/**
 * The code has been written according to http://www.rchoetzlein.com/eng/graphics/fluids.htm
 * and http://joeyfladderak.com/portfolio-items/sph-fluid-simulation/
 */

__kernel void sph_compute_pressure(__global float4* pos,
                                   __global float* density,
                                   __global float* pressure,
                                   float simscale,
                                   //float smoothradius,
                                   float radius2,
                                   //float mass,
                                   //float polykern,
                                   float mass_polykern,
                                   float restdensity,
                                   float intstiffness,
                                   uint numparticles)
{
  uint i = get_global_id(0);
  
  // The calculation is split in two loops to avoid
  // computing with self and to avoid an unnecessary
  // condition insede the for loop

  float sum = 0.0f;

  for (int j = 0; j < i; ++j)
  {
    float4 d = (pos[i] - pos[j]) * simscale;

    // note for myself:
    // the dot product of float4 is defined as x*x + y*y + z*z + w*w,
    // but since we set w to 1.0 in reset kernel and by subtracting
    // pos[i] - pos[j] we get 0,
    // this should not be a problem, in case
    // this is not true, then this has to be changed
    // to dsq = dot(d.xyz, d.xyz) or something similar
    float sqr = dot(d, d);

    if (radius2 > sqr)
    {
      float c = radius2 - sqr;
      sum += c * c * c;
    }
  }

  for (int j = i + 1; j < numparticles; ++j)
  {
    float4 d = (pos[i] - pos[j]) * simscale;
    float sqr = dot(d, d);

    if (radius2 > sqr)
    {
      float c = radius2 - sqr;
      sum += c * c * c;
    }
  }

  // polykern konstanta
  float ro = sum * mass_polykern; //mass * polykern;
  
  //                          kludova hustota vody pri 20 C     plynova konstanta (ci sa to bude chovat skor ako plyn)
  pressure[i] = (ro - restdensity) * intstiffness;   // vzorec
  density[i] = 1.0f / ro;
}