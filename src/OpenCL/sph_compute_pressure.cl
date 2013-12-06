__kernel void sph_compute_pressure(__global float4* pos,
                                   __global float* density,
                                   __global float* pressure,
                                   float simscale,
                                   float smoothradius,
                                   float radius2,
                                   float mass,
                                   float polykern,
                                   float restdensity,
                                   float intstiffness,
                                   unsigned int numparticles)
{
  unsigned int i = get_global_id(0);
  
  float dx, dy, dz, sum, dsq, c;
  float d, mR, mR2;
  d = simscale;
  mR = smoothradius;
  mR2 = mR * mR;
  
  sum = 0.0f;
  
  for (int j = 0; j < numparticles; j++)
  {
    if (i != j)
    {
      dx = (pos[i].x - pos[j].x) * d;
      dy = (pos[i].y - pos[j].y) * d;
      dz = (pos[i].z - pos[j].z) * d;
      
      dsq = (dx * dx + dy * dy + dz * dz);
      
      if (mR2 > dsq)
      {
        c = radius2 - dsq;
        sum += c * c * c;
      }
    }
  }
  
  // polykern konstanta
  density[i] = sum * mass * polykern;
  
  //                          kludova hustota vody pri 20 C     plynova konstanta (ci sa to bude chovat skor ako plyn)
  pressure[i] = (density[i] - restdensity) * intstiffness;   // vzorec
  density[i] = 1.0f / density[i];
}