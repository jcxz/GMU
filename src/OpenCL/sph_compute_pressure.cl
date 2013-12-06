/**
 * Kod napisany podla http://www.rchoetzlein.com/eng/graphics/fluids.htm
 * a http://joeyfladderak.com/portfolio-items/sph-fluid-simulation/
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

    // not for myself:
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