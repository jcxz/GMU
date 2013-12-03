// calculations largely based on R.C. Hoetzlein's implementation
// http://www.rchoetzlein.com/eng/graphics/fluids.htm
 
__kernel void sph_compute_force(__global float4* forces,
                                float simscale,
                                float smoothradius,
                                float viscosity,
                                float lapkern,
                                __global float4* pos,
                                __global float* pressure,
                                __global float* density,
                                __global float4* vel,
                                unsigned int numparticles,
                                float spikeykern)
{
  //unsigned int threadIdxx = get_local_id(0);
  //unsigned int threadIdxy = get_local_id(1);
  //unsigned int blockIdxx = get_group_id(0);
  //unsigned int blockIdxy = get_group_id(1);
  //unsigned int gridDimx = get_num_groups(0);
  //unsigned int blockDimx = get_local_size(0);
  //unsigned int blockDimy = get_local_size(1);
  
  //unsigned int i = mul24(blockIdxx, blockDimx) + threadIdxx;
  
  unsigned int i = get_global_id(0);
  
  forces[i] = (float4)(0.0f, 0.0f, 0.0f, 0.0f);
  
  float4 force = (float4)(0.0f, 0.0f, 0.0f, 0.0f);
  float pterm, vterm, dterm;
  float c, r, d, dsq;
  float dx, dy, dz;
  float mR, mR2, visc;
  
  d = simscale;
  mR = smoothradius;
  mR2 = (mR*mR);
  visc = viscosity;
  vterm = lapkern * visc;
  
  for(int j = 0; j < numparticles; j++)
  {
    if(i != j)
    {
      dx = (pos[i].x - pos[j].x) * d;
      dy = (pos[i].y - pos[j].y) * d;
      dz = (pos[i].z - pos[j].z) * d;
      
      dsq = (dx*dx + dy*dy + dz*dz);
      
      if(mR2 > dsq)
      {
        r = sqrt(dsq);
        c = (mR - r);
        pterm = -0.5f * c * spikeykern * (pressure[i] + pressure[j]) / r;
        dterm = c * density[i] * density[j];
        
        force.x += (pterm * dx + vterm * (vel[j].x - vel[i].x)) * dterm;
        force.y += (pterm * dy + vterm * (vel[j].y - vel[i].y)) * dterm;
        force.z += (pterm * dz + vterm * (vel[j].z - vel[i].z)) * dterm;
      }
    }
  }
  
  forces[i] = force;
}
 