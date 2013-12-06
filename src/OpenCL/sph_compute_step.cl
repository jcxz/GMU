/**
 * Kod napisany podla http://www.rchoetzlein.com/eng/graphics/fluids.htm
 * a http://joeyfladderak.com/portfolio-items/sph-fluid-simulation/
 */

#define DRAIN_MASK    (1 << 0)
#define WAVE_MASK     (1 << 1)
#define FOUNTAIN_MASK (1 << 2)



__kernel void sph_compute_step(__global float4* position,
                               __global float4* forces,
                               __global float4* velocity,
                               __global float4* prevvelocity,
                               float slope,
                               float leftwave,
                               float rightwave,
                               float deltatime,
                               float limit,
                               float extstiffness,
                               float extdamping,
                               float radius,
                               float4 volumemin,
                               float4 volumemax,
                               float simscale,
                               float mass,
                               float time,
                               uint flags)
                               //float4 gravitation)
{
  unsigned int i = get_global_id(0);
  
  float4 norm = (float4) (0.0f, 0.0f, 0.0f, 0.0f);
  float adj;
  float diff; 
  
  float4 pos = position[i];
  float4 vel = velocity[i];
  float4 prevvel = prevvelocity[i];
  float4 accel = forces[i] * mass;
  
  float speed = accel.x * accel.x + accel.y * accel.y + accel.z * accel.z;
  if (speed > (limit * limit))
  {
    accel *= limit / sqrt(speed);
  }
  
  // Z Axis wall
  diff = 2.0f * radius - (pos.y - volumemin.y - (pos.x - volumemin.x) * slope) * simscale;
  if (diff > 0.0001f)
  {
    norm.x = -slope;
    norm.z = 0.0f;
    norm.y = 1.0f - slope;  // = float4(-0, 0, 1.0 - 0, 0);
    adj = extstiffness * diff - extdamping * dot(norm, prevvel);
    accel += adj * norm;
  }
  
  diff = 2.0f * radius - (volumemax.y - pos.y) * simscale;
  if (diff > 0.0001f)
  {
    norm.x = 0.0f;
    norm.z = -1.0f;
    norm.y = 0.0f;  //float4( 0, 0, -1, 0 );
    adj = extstiffness * diff - extdamping * dot(norm, prevvel);
    accel += adj * norm;
  }
  
  // X-axis walls
  diff = 2.0f * radius - (pos.x - volumemin.x + (sin(time * 10.0f) - 1.0f + (pos.y * 0.025f) * 0.25f) * leftwave) * simscale;   
  if (diff > 0.0001f)
  {
    norm.x = 1.0f;
    norm.y = 0.0f;
    norm.z = 0.0f;  //float4( 1.0, 0, 0, 0 );
    adj = (leftwave + 1.0f) * extstiffness * diff - extdamping * dot(norm, prevvel);
    accel += adj * norm;                
  }
  
  diff = 2.0f * radius - (volumemax.x - pos.x + (sin(time * 10.0f) - 1.0f) * rightwave) * simscale;  
  if (diff > 0.0001f)
  {
    norm.x = -1.0f;
    norm.y = 0.0f;
    norm.z = 0.0f;  //float4( -1, 0, 0, 0 );
    adj = (rightwave+1.0f) * extstiffness * diff - extdamping * dot(norm, prevvel );
    accel += adj * norm;
  }
  
  // Y-axis walls
  diff = 2.0f * radius - (pos.z - volumemin.z) * simscale;
  if (diff > 0.0001f)
  {
    norm.x = 0.0f;
    norm.z = 1.0f;
    norm.y = 0.0f;  //float4( 0, 1, 0, 0 );
    adj = extstiffness * diff - extdamping * dot(norm, prevvel );
    accel += adj * norm;
  }
  
  diff = 2.0f * radius - (volumemax.z - pos.z) * simscale;
  if (diff > 0.0001f)
  {
    norm.x = 0.0f;
    norm.z = -1.0f;
    norm.y = 0.0f;  //float4( 0, -1, 0, 0 );
    adj = extstiffness * diff - extdamping * dot(norm, prevvel);
    accel += adj * norm;
  }

  /* drain */
#if 1
  if (flags & DRAIN_MASK)
  {
    float dx = (0 - pos.x) * simscale;            // dist in cm
    float dy = (volumemin.y - pos.y) * simscale;
    float dz = (0 - pos.z) * simscale;
    float dsq = (dx * dx + dy * dy + dz * dz);
    if (0.0001f > dsq)
    {
      pos.x = volumemin.x * 0.5f;
      pos.y = volumemax.y * 0.5f;
      pos.z = volumemin.z;
      accel.z += 50;
      accel.x += 1;
      accel.y += 20;
    }
  }
#else
  if (flags & DRAIN_MASK)
  {
    diff = 2 * radius - (pos.z - volumemin.z-15 ) * simscale;
    if (diff < 2*radius && diff > 0.0001f && (fabs(pos.x)>3 || fabs(pos.y)>3) )
    {
      norm = (float4) (0, 0, 1, 0);
      //adj = stiff * diff - damp * dot(norm, prevvelocity[i]);
      adj = extstiffness * diff - extdamping * dot(norm, prevvelocity[i]);
      accel.x += adj * norm.x; accel.y += adj * norm.y; accel.z += adj * norm.z;
    }
  }
#endif
  
  /* wave */
  if (flags & WAVE_MASK)
  {
    if (pos.x < 0) accel.x += 20;
  }
  
  /* fountain */
  if (flags & FOUNTAIN_MASK)
  {
    float dx = (0 - pos.x) * simscale;            // dist in cm
    float dy = (volumemin.y - pos.y) * simscale;
    float dz = (0 - pos.z) * simscale;
    float dsq = (dx * dx + dy * dy + dz * dz);
    if (0.0005f > dsq)
    {
      accel.y += 140;
    }
  } 

  accel.y += -9.8f;  //float4(0, -9.8, 0, 0);
  //accel += gravitation;
  
  // Leapfrog Integration ----------------------------
  float4 vnext = accel * deltatime + vel;  // v(t+1/2) = v(t-1/2) + a(t) dt
  prevvel = (vel + vnext) * 0.5f;          // v(t+1) = [v(t-1/2) + v(t+1/2)] * 0.5     used to compute forces later
  vel = vnext;
  vnext *= deltatime / simscale;
  pos += vnext;                       // p(t+1) = p(t) + v(t+1/2) dt
  
  velocity[i] = vel;
  prevvelocity[i] = prevvel;
  position[i] = pos;
}