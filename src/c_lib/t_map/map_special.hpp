#pragma once

namespace t_map
{

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

	void remove_control_point(int x, int y, int z)
	{
		for(int i=0; i<cpi; i++)
		{
			if(x==cpa[i].x && y==cpa[i].y && z==cpa[i].z)
			{
				cpa[i] = cpa[cpi-1];
				cpi--;
				GS_ASSERT(cpi >= 0);
			}
		}

		printf("Error: tried to remove control point that does not exist!\n");
		GS_ABORT(); //should never reach this point
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

#if DC_SERVER

	void send_control_points_to_client(int client_id)
	{
		for(int i=0; i<cpi; i++)
		{
			control_point_create_StoC p;
			p.x = cpa[i].x;
			p.y = cpa[i].y;
			p.z = cpa[i].z;
			p.sendToClient(client_id);
		}
	}

	void server_add_control_point(int x, int y, int z)
	{
		this->add_control_point(x,y,z);

		control_point_create_StoC p;
		p.x = x;
		p.y = y;
		p.z = z;
		p.broadcast();
	}

	void server_remove_control_point(int x, int y, int z)
	{
		remove_control_point(x,y,z);

		control_point_delete_StoC p;
		p.x = x;
		p.y = y;
		p.z = z;
		p.broadcast();
	}

#endif

};

}