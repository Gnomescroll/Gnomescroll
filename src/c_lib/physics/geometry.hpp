#pragma once

#include <physics/vec3.hpp>

bool line_plane_intersection1(
	float lx, float ly, float lz,		//line starting point
	float lvx, float lvy, float lvz,	//line direction
	float px, float py, float pz,		//point on plane
	float nx, float ny, float nz,		//normal
	float* a 							//coefficient
	)
{
	float num = (px-lx)*nx + (py-ly)*ny + (pz-lz)*nz;
	float denum = lvx*nx + lvy*ny + lvz*nz; 

	static const float epsilon = 0.001f;

	float _num = fabs(num);
	float _denum = fabs(denum);

	if(_num > epsilon && _denum > epsilon )
	{
		//intersection
		*a = num / denum;
		return true;
	}

	//If the line starts outside the plane and is parallel to the plane, there is no intersection. 
	//In this case, the above denominator will be zero and the numerator will be non-zero.

	if(_denum < epsilon && _num > epsilon)
	{
		//line outside of plane and parrallel
		return false;
	}

	//If the line starts inside the plane and is parallel to the plane, the line intersects the plane everywhere. 
	//In this case, both the numerator and denominator above will be zero.

	if(_denum < epsilon && _num < epsilon)
	{
		//intersects everywhere
		*a = 0.0f;
		return true;
	}

	GS_ASSERT(false);
	return false;
}

float _plx, _ply, _plz;
float _pvx, _pvy, _pvz;

float _ppx, _ppy, _ppz;

float _x0, _y0, _z0;
float _x1, _y1, _z1;

float _ix, _iy, _iz;

bool line_plane_intersection2(
	float lx, float ly, float lz,		//line starting point
	float lvx, float lvy, float lvz,	//line direction
	float bx, float by, float bz,		//center

	struct Vec3 n, 		//normal
	struct Vec3 v1, 		//direction 1 on surface
	struct Vec3 v2,		//direction 2 on surface

	float sn, float sv1, float sv2, 	//size for directions
	float* a 							//coefficient
	)
{
	bool ret;
	float px,py,pz;
	//top
	px = bx+sn*n.x;
	py = by+sn*n.y;
	pz = bz+sn*n.z;


	ret = line_plane_intersection1(lx,ly,lz, 
		lvx,lvy,lvz, 
		px,py,pz, 
		n.x,n.y,n.z, 
		a);

	if( ret)
	{
		float x = lx + (*a)*lvx - px;
		float y = ly + (*a)*lvy - py;
		float z = lz + (*a)*lvz - pz;

		float t1 = x*v1.x + y*v1.y + z*v1.z;
		if( t1 < sv1 && t1 > -sv1 )
		{
			float t2 = x*v2.x + y*v2.y + z*v2.z;
			if( t2 < sv2 && t2 > -sv2 )
			{

					_ppx = px;
					_ppy = py;
					_ppz = pz;

					_plx = lx;
					_ply = ly;
					_plz = lz;

					_pvx = lvx;
					_pvy = lvy;
					_pvz = lvz;

					_x0 = sv1*v1.x;
					_y0 = sv1*v1.y;
					_z0 = sv1*v1.z;

					_x1 = sv2*v2.x;
					_y1 = sv2*v2.y;
					_z1 = sv2*v2.z;

					_ix = x + px;
					_iy = y + py;
					_iz = z + pz;

				printf("x,y,z= %0.2f %0.2f %0.2f  px,py,pz= %0.2f %0.2f %0.2f \n", x,y,z, px,py,pz);
				printf("t1: %0.2f t2: %0.2f sv1,sv2= %0.2f %0.2f \n", t1,t2, sv1,sv2);
				return true;	//a is set already
			}
		}
	}


	return false;
}

void visualize_line()
{
/*
float _plx, _ply, _plz;
float _pvx, _pvy, _pvz;

float _ppx, _ppy, _ppz;

float _x0, _y0, _z0;
float _x1, _y1, _z1;

float _ix, _iy, _iz;
*/
    glDisable(GL_TEXTURE_2D);
	glColor4ub(255,0,0,255);
	glLineWidth(1.0f);

	const float len = 4.0f;

	glBegin(GL_LINES);

	glVertex3f(_plx, _ply, _plz);
	glVertex3f(_plx+len*_pvx, _ply+len*_pvy, _plz+len*_pvz);

	glEnd();
	glLineWidth(1.0f);


	glColor4ub(0,255,0,255);
	glPointSize(4.0f);
	glBegin(GL_POINTS);

	//glVertex3f(_plx, _ply, _plz);
	//glVertex3f(_plx+len*_pvx, _ply+len*_pvy, _plz+len*_pvz);
	glVertex3f(_ppx, _ppy, _ppz);

	glEnd();


	glColor4ub(0,0,255,255);
	glPointSize(6.0f);
	glBegin(GL_POINTS);

	//glVertex3f(_plx, _ply, _plz);
	//glVertex3f(_plx+len*_pvx, _ply+len*_pvy, _plz+len*_pvz);
	glVertex3f(_ppx+_x0, _ppy+_y0, _ppz+_z0);
	glVertex3f(_ppx-_x0, _ppy-_y0, _ppz-_z0);
	glVertex3f(_ppx+_x1, _ppy+_y1, _ppz+_z1);
	glVertex3f(_ppx-_x1, _ppy-_y1, _ppz-_z1);

	glEnd();

	glColor4ub(0,255,255,255);
	glPointSize(8.0f);
	glBegin(GL_POINTS);

	glVertex3f(_ix, _iy, _iz);

	glEnd();

	glColor4ub(255,255,255,255);
	glPointSize(1.0f);
}


bool line_box_test(
	float lx, float ly, float lz,
	float lvx, float lvy, float lvz,
	float bx, float by, float bz,		//center
	float bdx, float bdy, float bdz,	//size
	struct Vec3 f,
	struct Vec3 r,
	struct Vec3 u,

	float* a
	)
{
	//there are 6 planes, do intersection on each of them
	bool ret;
	//float a;

	//top
	ret = line_plane_intersection2(lx,ly,lz, lvx,lvy,lvz, bx,by,bz, u,f,r, bdz,bdx,bdy, a);
	if( ret)
	{
		return true;
	}

	return false;
	//bottom


}