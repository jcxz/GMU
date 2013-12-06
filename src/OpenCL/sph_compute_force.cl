/**
 * Kod napisany podla http://www.rchoetzlein.com/eng/graphics/fluids.htm
 * a http://joeyfladderak.com/portfolio-items/sph-fluid-simulation/
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
 