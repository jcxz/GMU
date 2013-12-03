__kernel void sph_compute_step(__global float4* position, 
                               __global float4* velocity, 
                               __global float4* prevvelocity, 
                               float slope,
                               float leftwave,
                               float rightwave,
                               float deltatime,
                               float limit,
                               __global float4* forces,
                               float extstiffness,
                               float extdamping,
                               float radius,
                               float4 volumemin,
                               float4 volumemax,
                               float simscale,
                               float mass,
                               float time)
{
    unsigned int threadIdxx = get_local_id(0);
    unsigned int threadIdxy = get_local_id(1);
    unsigned int blockIdxx = get_group_id(0);
    unsigned int blockIdxy = get_group_id(1);
    unsigned int gridDimx = get_num_groups(0);
    unsigned int blockDimx = get_local_size(0);
    unsigned int blockDimy = get_local_size(1);
 
    unsigned int i = mul24(blockIdxx, blockDimx) + threadIdxx;
    //unsigned int i = get_global_id(0);
 
    float4 norm;
    float4 accel;
    float4 vnext;
    float4 min, max;
    float adj;
    float SL, SL2, ss, rad;
    float stiff, damp, speed, diff; 
    SL = limit;
    SL2 = SL*SL;
 
    float4 pos = position[i];
    float4 vel = velocity[i];
    float4 prevvel = prevvelocity[i];
 
    stiff = extstiffness;
    damp = extdamping;
    rad = radius;
    min = volumemin;
    max = volumemax;
    ss = simscale;
 
    accel = forces[i];
    accel *= mass;
 
    speed = accel.x * accel.x + accel.y * accel.y + accel.z * accel.z;
    if(speed > SL2)
    {
        accel *= SL / sqrt(speed);
    }
 
    // Z Axis wall
    diff = 2.0f * rad - (pos.y - min.y - (pos.x - min.x) * slope)*ss;
    if(diff > 0.0001f)
    {
        norm.x = -slope; norm.z = 0.0f; norm.y = 1.0f - slope;// = float4(-0, 0, 1.0 - 0, 0);
        adj = stiff * diff - damp * dot(norm, prevvel);
        accel += adj * norm;
    }
 
    diff = 2.0f * rad - ( max.y - pos.y )*ss;
    if (diff > 0.0001f) {
        norm.x = 0.0f; norm.z = -1.0f; norm.y = 0.0f;//float4( 0, 0, -1, 0 );
        adj = stiff * diff - damp * dot(norm, prevvel);
        accel.x += adj * norm.x; 
        accel.y += adj * norm.y; 
        accel.z += adj * norm.z;
    }
 
    // X-axis walls
    diff = 2.0f * rad - (pos.x - min.x + (sin(time*10.0f)-1.0f+(pos.y*0.025f)*0.25f) * leftwave )*ss;   
    if (diff > 0.0001f ) {
        norm.x = 1.0f; norm.y = 0.0f; norm.z = 0.0f;//float4( 1.0, 0, 0, 0 );
        adj = (leftwave + 1.0f) * stiff * diff - damp * dot(norm, prevvel);
        accel.x += adj * norm.x; accel.y += adj * norm.y; accel.z += adj * norm.z;                  
    }
 
    diff = 2.0f * rad - ( max.x - pos.x + (sin(time*10.0f)-1.0f) * rightwave )*ss;  
    if (diff > 0.0001f) {
        norm.x = -1.0f; norm.y = 0.0f; norm.z = 0.0f;//float4( -1, 0, 0, 0 );
        adj = (rightwave+1.0f) * stiff * diff - damp * dot(norm, prevvel );
        accel.x += adj * norm.x; accel.y += adj * norm.y; accel.z += adj * norm.z;
    }
 
    // Y-axis walls
    diff = 2.0f * rad - ( pos.z - min.z )*ss;           
    if (diff > 0.0001f) {
        norm.x = 0.0f; norm.z = 1.0f; norm.y = 0.0f;//float4( 0, 1, 0, 0 );
        adj = stiff * diff - damp * dot(norm, prevvel );
        accel.x += adj * norm.x; accel.y += adj * norm.y; accel.z += adj * norm.z;
    }
    diff = 2.0f * rad - ( max.z - pos.z )*ss;
    if (diff > 0.0001f) {
        norm.x = 0.0f; norm.z = -1.0f; norm.y = 0.0f;//float4( 0, -1, 0, 0 );
        adj = stiff * diff - damp * dot(norm, prevvel);
        accel.x += adj * norm.x; accel.y += adj * norm.y; accel.z += adj * norm.z;
    }
 
    accel.y += -9.8f;//float4(0, -9.8, 0, 0);
 
    // Leapfrog Integration ----------------------------
    vnext = accel;                          
    vnext *= deltatime;
    vnext += vel;                       // v(t+1/2) = v(t-1/2) + a(t) dt
    prevvel = vel;
    prevvel += vnext;
    prevvel *= 0.5f;                    // v(t+1) = [v(t-1/2) + v(t+1/2)] * 0.5     used to compute forces later
    vel = vnext;
    vnext *= deltatime/ss;
    pos += vnext;                       // p(t+1) = p(t) + v(t+1/2) dt
 
    velocity[i] = vel;
    prevvelocity[i] = prevvel;
    position[i] = pos;
}