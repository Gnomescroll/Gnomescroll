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
	class CONTROL_POINT cpa; //control point array;

	CONTROL_POINT_LIST()
	{
		cpi = 0;
		cpm = 8;
		cpa = (CONTROL_POINT*) malloc(8*sizeof(class CONTROL_POINT));
	}

	void add_control_point(int x, int y, int z)
	{

		cpa[cpi].x = x;
		cpa[cpi].y = y;
		cpa[cpi].z = z;

		cpi++;

		if(cpi == cpm)
		{
			cpm *= 2;
			cpa = realloc(cpa, cpm*sizeof(class CONTROL_POINT));
		}
	}

}