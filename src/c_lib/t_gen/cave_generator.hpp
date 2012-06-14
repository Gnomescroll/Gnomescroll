#pragma once

#include <t_gen/noise_map2.hpp>


#include <t_gen/twister.hpp>

#include <t_map/t_map.hpp>


namespace t_gen 
{

/*
__attribute((always_inline, optimize("-O3")))
float point_line_distance2(float px, float py, float pz, float ox, float oy, float oz, float tx, float ty, float tz)
{
    tx -= px;
    ty -= py;
    tz -= pz;

    float t =  tx*ox + ty*oy + tz*oz; // <tx|ox>

    //d = t/(ox*ox+oy*oy+oz*oz); //distance to collision

    float x = t*ox - tx;
    float y = t*oy - ty;
    float z = t*oz - tz;
    return x*x+y*y+z*z; // minimum distance squared between target and line

}
*/

/*
float minimum_distance(vec2 v, vec2 w, vec2 p) {
  // Return minimum distance between line segment vw and point p
  const float l2 = length_squared(v, w);  // i.e. |w-v|^2 -  avoid a sqrt
  if (l2 == 0.0) return distance(p, v);   // v == w case
  // Consider the line extending the segment, parameterized as v + t (w - v).
  // We find projection of point p onto the line. 
  // It falls where t = [(p-v) . (w-v)] / |w-v|^2
  const float t = dot(p - v, w - v) / l2;
  if (t < 0.0) return distance(p, v);       // Beyond the 'v' end of the segment
  else if (t > 1.0) return distance(p, w);  // Beyond the 'w' end of the segment
  const vec2 projection = v + t * (w - v);  // Projection falls on the segment
  return distance(p, projection);
}
*/

__attribute((always_inline, optimize("-O3")))
float point_line_distance2(float vx, float vy, float vz, float wx, float wy, float wz, float px, float py, float pz)
{

	px -= vx;
	py -= vy;
	pz -= vz;

	float t = px*wx + py*wy + pz*wz;

	float x = t*wx - px;
	float y = t*wy - py;
	float z = t*wz - pz;

	return x*x + y*y + z*z;

}

__attribute__((optimize("-O3")))
void generate_node(float xs, float ys, float zs, float theta, float phi, float cave_size)
{
	const static float length = 2.0;

/*
	float dx = length*sin(phi)*cos(theta)
	float dy = length*sin(phi)*sin(theta);
	float dz = length*cos(phi);
*/
	float dx = sin(phi)*cos(theta);
	float dy = sin(phi)*sin(theta);
	float dz = cos(phi);

	float xm = abs(dx) + cave_size;
	float ym = abs(dy) + cave_size;
	float zm = abs(dz) + cave_size;

	int xmin = xs - xm;
	int xmax = xs + xm;
	
	int ymin = ys - ym;
	int ymax = ys + ym;

	int zmin = zs - zm;
	int zmax = zs + zm;


	//can speed up by 8
	for(int i=xmin; i<=xmax; i++)
	for(int j=ymin; j<=ymax; j++)
	for(int k=zmin; k<=zmax; k++)
	{
		if(k < 0 || k > 127) continue;

		float x = ((float)i) + 0.5;
		float y = ((float)j) + 0.5;
		float z = ((float)k) + 0.5;

		float d = point_line_distance2(xs,ys,zs, dx,dy,dz, x,y,z);
		if(d < cave_size*cave_size) t_map::set(i,j,k, 0);
	}


	xs += length*dx;
	ys += length*dy;
	zs += length*dz;

	static const float theta_adj = 0.08;
	static const float phi_adj = 0.05;

	theta += theta_adj*3.1419*2*(2*genrand_real1() - 1.0);
	phi += phi_adj*3.1419*2*(2*genrand_real1() - 1.0);

	if( genrand_real1() < 0.999 )
	{
		generate_node(xs,ys,zs, theta,phi, cave_size);
	}

}


void start_cave_generator()
{
	const int nodes = 50;
	const float cave_size = 2.0;

	init_genrand(rand());

	for(int i=0; i<nodes; i++)
	{
		float x = genrand_real1()*512;
		float y = genrand_real1()*512;
		float z = genrand_real1()*128;

		int tries= 0;
		while( t_map::get(x,y,z) == 0 )
		{
			x = genrand_real1()*512;
			y = genrand_real1()*512;
			z = genrand_real1()*128;
			if(tries++ > 10000) return;
		}
	
		float phi = genrand_real1()*2*3.14159;
		float theta = genrand_real1()*2*3.14159;

		generate_node(x,y,z, theta, phi, cave_size);

	}


}


}