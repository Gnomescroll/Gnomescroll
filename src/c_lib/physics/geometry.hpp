#pragma once


bool line_plane_intersection2(
	float lx, float ly, float lz,		//line starting point
	float lvx, float lvy, float lyz,	//line direction
	float px, float py, float pz,		//point on plane
	float nx, float ny, float nz,		//normal
	float* a 							//coefficient
	)
{
	float num = (px-lx)*nx + (py-ly)*ny + (pz-lz)*nz;


	float denum = lvx*nx + lvy*ny + lvz*nz; 



	static const float epsilon 0.001f;

	_num = absf(num);
	_denum = abs(denum);


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


bool line_plane_intersection2(
	float lx, float ly, float lz,		//line starting point
	float lvx, float lvy, float lyz,	//line direction
	float bx, float by, float bz,		//center

	struct Vec3 n, 		//normal
	struct Vec3 v1 		//direction 1 on surface
	struct Vec3 v2		//direction 2 on surface

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
	ret = line_plane_intersection1(lx,ly,lz, lvx,lvy,lvz, px,py,pz, n.x,n.y,n.z, a);
	if( ret)
	{
		float x = lx + a*lvx - px;
		float y = ly + a*lvy - py;
		float z = lz + a*lvz - pz;

		float t1 = x*v1.x + y*v1.y + z*v1.z;
		if( t1 < bdx && t1 > -bdx )
		{
			float t2 = x*v2.x + y*v2.y + z*v2.z;
			if( t2 < bdy && t2 > -bdy )
			{
				return true;	//a is set already
			}
		}
	}


	return false;
}

void line_box_test(
	float lx, float ly, float lz,
	float lvx, float lvy, float lyz,
	float bx, float by, float bz,		//center
	float bdx, float bdy, float bdz,	//size
	struct Vec3 f,
	struct Vec3 r,
	struct Vec3 u
	)
{
	//there are 6 planes, do intersection on each of them
	bool ret;
	float a;

	//top
	ret = line_plane_intersection2((lx,ly,lz, lvx,lvy,lvz, bx,by,bz, f,r,u, bdx,bdy,bdz, &a);
	if( ret)
	{

	}

	//bottom


}