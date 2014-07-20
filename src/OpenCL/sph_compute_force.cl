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
 
__kernel void sph_compute_force(__global float4* pos,
                                __global float* density,
                                __global float* pressure,
                                __global float4* forces,
                                __global float4* vel,
                                float simscale,
                                float smoothradius,
                                float radius2,
                                //float viscosity,
                                //float lapkern,
                                float vterm,
                                float spikykern_half,
                                unsigned int numparticles)
{
  unsigned int i = get_global_id(0);

  float4 force = (float4) (0.0f, 0.0f, 0.0f, 0.0f);

  //float vterm = lapkern * viscosity;

  for (int j = 0; j < i; ++j)
  {
    float4 d = (pos[i] - pos[j]) * simscale;
    float sqr = dot(d, d);

    if (radius2 > sqr)
    {
      float r = sqrt(sqr);
      float c = (smoothradius - r);
      //float pterm = -0.5f * c * spikeykern * (pressure[i] + pressure[j]) / r;
      float pterm = c * spikykern_half * (pressure[i] + pressure[j]) / r;
      float dterm = c * density[i] * density[j];
    
      force += (pterm * d + vterm * (vel[j] - vel[i])) * dterm;
    }
  }

  for (int j = i + 1; j < numparticles; ++j)
  {
    float4 d = (pos[i] - pos[j]) * simscale;
    float sqr = dot(d, d);
    
    if (radius2 > sqr)
    {
      float r = sqrt(sqr);
      float c = (smoothradius - r);
      //float pterm = -0.5f * c * spikeykern * (pressure[i] + pressure[j]) / r;
      float pterm = c * spikykern_half * (pressure[i] + pressure[j]) / r;
      float dterm = c * density[i] * density[j];

      force += (pterm * d + vterm * (vel[j] - vel[i])) * dterm;
    }
  }
  
  forces[i] = force;
}
 