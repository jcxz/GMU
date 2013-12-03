__kernel void sph_compute_pressure(__global float* density,
                                   __global float* pressure,
                                   float simscale,
                                   float smoothradius,
                                   __global float4* pos,
                                   float radius2,
                                   float mass,
                                   float polykern,
                                   float restdensity,
                                   float intstiffness,
                                   unsigned int numparticles)
{
    /*unsigned int threadIdxx = get_local_id(0);
    unsigned int threadIdxy = get_local_id(1);
    unsigned int blockIdxx = get_group_id(0);
    unsigned int blockIdxy = get_group_id(1);
    unsigned int gridDimx = get_num_groups(0);
    unsigned int blockDimx = get_local_size(0);
    unsigned int blockDimy = get_local_size(1);
 
    unsigned int i = mul24(blockIdxx, blockDimx) + threadIdxx;*/
    unsigned int i = get_global_id(0);
 
    float dx, dy, dz, sum, dsq, c;
    float d, mR, mR2;
    d = simscale;
    mR = smoothradius;
    mR2 = mR*mR;
 
    sum = 0.0f;
 
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
                c = radius2 - dsq;
                sum += c*c*c;
            }
        }
    }
 
    density[i] = sum * mass * polykern;
    pressure[i] = (density[i] - restdensity) * intstiffness;
    density[i] = 1.0f / density[i];
}