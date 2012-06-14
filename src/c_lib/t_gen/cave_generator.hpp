#pragma once

#include <t_gen/noise_map2.hpp>


#include <t_gen/twister.hpp>

#include <t_map/t_map.hpp>


namespace t_gen 
{

// v is starting point of line
// vz is unit vector for direction of line
// px is the point
// returns square of distance between line and point
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