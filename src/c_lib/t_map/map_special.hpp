#pragma once


class CONTROL_POINT
{
	public:
	
	int x;
	int y;
	int z;
};

class CONTROL_POINT_LIST
{
	public:

	int cpi; //control point index
	int cpm; //control point max
	class CONTROL_POINT* cpa; //control point array;

	CONTROL_POINT_LIST()
	{
		cpi = 0;
		cpm = 8;
		cpa = (CONTROL_POINT*) malloc(8*sizeof(class CONTROL_POINT));
	}

	~CONTROL_POINT_LIST() {}

	void add_control_point(int x, int y, int z)
	{

		cpa[cpi].x = x;
		cpa[cpi].y = y;
		cpa[cpi].z = z;

		cpi++;

		if(cpi == cpm)
		{
			cpm *= 2;
			cpa = (CONTROL_POINT*) realloc(cpa, cpm*sizeof(class CONTROL_POINT));
		}
	}

	bool control_point_in_range_check(int x, int y, int z)
	{
		for(int i=0; i<cpi; i++)
		{
			if( abs(x-cpa[i].x) <= 6 && abs(y-cpa[i].y) <= 6 && abs(z-cpa[i].z) <= 6 )	
				return true;
		}

		return false;
	}
};