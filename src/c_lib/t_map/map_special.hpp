#pragma once

namespace t_map
{

class control_node
{
	public:
	
	int x;
	int y;
	int z;
};

class CONTROL_NODE_LIST
{
	public:

	int cpi; //control point index
	int cpm; //control point max
	class control_node* cpa; //control point array;

	bool needs_update; //for drawing

	CONTROL_NODE_LIST()
	{
		cpi = 0;
		cpm = 8;
		cpa = (control_node*) malloc(8*sizeof(class control_node));
		needs_update = true;
	}

	~CONTROL_NODE_LIST() {}

	void add_control_node(int x, int y, int z)
	{
		//needs_update = true;

		cpa[cpi].x = x;
		cpa[cpi].y = y;
		cpa[cpi].z = z;

		cpi++;

		if(cpi == cpm)
		{
			cpm *= 2;
			control_node* new_cpa = (control_node*) realloc(cpa, cpm*sizeof(class control_node));
            if (new_cpa == NULL)
            {
                free(cpa);
                cpa = NULL;
                cpm = 0;
            }
            else cpa = new_cpa;
		}
	}

	void remove_control_node(int x, int y, int z)
	{
		//needs_update = true;

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


	bool control_node_in_range_check(int x, int y, int z)
	{
		for(int i=0; i<cpi; i++)
		{
			if( abs(x-cpa[i].x) <= 6 && abs(y-cpa[i].y) <= 6 && abs(z-cpa[i].z) <= 6 )	
				return true;
		}

		return false;
	}

#if DC_SERVER

	void send_control_nodes_to_client(int client_id)
	{
		for(int i=0; i<cpi; i++)
		{
			control_node_create_StoC p;
			p.x = cpa[i].x;
			p.y = cpa[i].y;
			p.z = cpa[i].z;
			p.sendToClient(client_id);
		}
	}

	void server_add_control_node(int x, int y, int z)
	{
		this->add_control_node(x,y,z);

		control_node_create_StoC p;
		p.x = x;
		p.y = y;
		p.z = z;
		p.broadcast();
	}

	void server_remove_control_node(int x, int y, int z)
	{
		remove_control_node(x,y,z);

		control_node_delete_StoC p;
		p.x = x;
		p.y = y;
		p.z = z;
		p.broadcast();
	}

#endif

};

#if DC_CLIENT

struct CONTROL_NODE_RENDER
{
	short x,y,z;
	unsigned char face;	//block side and edge
	unsigned char r,g,b;	//color
};

struct CONTROL_NODE_RENDER* cnra; // control_node_render_list array
int cnri = 0; // index
int cnrm = 32; // max

class CONTROL_NODE_LIST* cnl; //control node list

void control_node_render_init(class CONTROL_NODE_LIST* _cnl)
{
	cnra = (struct CONTROL_NODE_RENDER*) malloc(cnrm*sizeof(struct CONTROL_NODE_RENDER));

	cnl = _cnl; //
}

void control_node_render_teardown()
{
	free(cnra);
}

void _insert_control_node_render_element(short x, short y, short z, unsigned char face)
{
	struct CONTROL_NODE_RENDER cnr;
	cnr.x = x;
	cnr.y = y;
	cnr.z = z;
	cnr.face = face;

	cnra[cnri] = cnr; //insert
	cnri++;

	if(cnri == cnrm)
	{
		cnrm *= 2;
		cnra = (struct CONTROL_NODE_RENDER*) realloc(cnra, cnrm*sizeof(struct CONTROL_NODE_RENDER));
	}


}

//int cpi; //control point index
//int cpm; //control point max
//class control_node* cpa; //control point array;

void control_node_render_update()
{
	//static int counter = 0; //refresh, deals with loading before terrain map
	//counter ++ ;

	if(cnl->needs_update == false && cnri != 0) return;
	cnl->needs_update = false;
	cnri = 0; //reset index

	const int size = 4;	//size of control grid to left or right of block

	for(int _i=0; _i<cnl->cpi; _i++)
	{
		//top

		int _x = cnl->cpa[_i].x;
		int _y = cnl->cpa[_i].y;
		int _z = cnl->cpa[_i].z;
/*
		//left 
		for(int i=-size; i<size; i++)
		for(int j=-size; j<size; j++)
		{
			int x = _x + i;
			int y = _y + j;
			int z = _z + size;

			if(!isSolid(x,y,z) && isSolid(x,y,z+1) )
			{

			}
		}
*/
		//west
		for(int i=-size; i<size; i++)
		for(int j=-size; j<size; j++)
		{
			int x = _x - size;
			int y = _y + i;
			int z = _z + j;

			if(!isSolid(x,y,z) && isSolid(x,y,z-1) )
			{
				int face = 0;	//orientation
				_insert_control_node_render_element(x,y,z, face);

				printf("CN: %i %i %i \n", x,y,z);
			}
		}

		//top
/*
		for(int i=-size; i<size; i++)
		for(int j=-size; j<size; j++)
		{
			int x = _x + i;
			int y = _y + j;
			int z = _z + size;

			if(!isSolid(x,y,z) && isSolid(x,y,z+1) )
			{

			}
		}
*/
		//bottom
	/*
		for(int i=-size; i<size; i++)
		for(int j=-size; j<size; j++)
		{


		}
	*/
	}
	
}

/*
static const float v_index[72] = 
{
    1,1,1 , 0,1,1 , 0,0,1 , 1,0,1 , //top
    0,1,0 , 1,1,0 , 1,0,0 , 0,0,0 , //bottom
    1,0,1 , 1,0,0 , 1,1,0 , 1,1,1 , //north
    0,1,1 , 0,1,0 , 0,0,0 , 0,0,1 , //south
    1,1,1 , 1,1,0 , 0,1,0,  0,1,1 , //west
    0,0,1 , 0,0,0 , 1,0,0 , 1,0,1 , //east
};
*/

void control_node_render_draw()
{
	static const float v_index[72] = 
	{
	    1,1,1 , 0,1,1 , 0,0,1 , 1,0,1 , //top
	    0,1,0 , 1,1,0 , 1,0,0 , 0,0,0 , //bottom
	    1,0,1 , 1,0,0 , 1,1,0 , 1,1,1 , //west
	    0,1,1 , 0,1,0 , 0,0,0 , 0,0,1 , //east
	    1,1,1 , 1,1,0 , 0,1,0,  0,1,1 , //north
	    0,0,1 , 0,0,0 , 1,0,0 , 1,0,1 , //south
	};

	glPointSize(6.0);

	glColor4ub(127,0,0,128);

	glBegin(GL_QUADS);
	//glBegin(GL_POINTS);
	for(int i=0; i<cnri; i++)
	{
		float x = (float) cnra[i].x;
		float y = (float) cnra[i].y;
		float z = (float) cnra[i].z;
		int face = cnra[i].face;
		
		//west
		//x -= 0.66;
		//y += 0.33;

		//clockwise


		int s = 2;

		glVertex3f(x+v_index[12*s+3*0+0], y+v_index[12*s+3*0+1], z+v_index[12*s+3*0+2]);
		glVertex3f(x+v_index[12*s+3*1+0], y+v_index[12*s+3*1+1], z+v_index[12*s+3*1+2]);
		glVertex3f(x+v_index[12*s+3*2+0], y+v_index[12*s+3*2+1], z+v_index[12*s+3*2+2]);
		glVertex3f(x+v_index[12*s+3*3+0], y+v_index[12*s+3*3+1], z+v_index[12*s+3*3+2]);

	/*
		glVertex3f(x, y, z+1.0);
		glVertex3f(x, y+1.0, z+1.0);
		glVertex3f(x, y+1.0, z);
		glVertex3f(x, y, z);
	*/
	}

	glEnd();




	glBegin(GL_POINTS);
	for(int i=0; i<cnri; i++)
	{
		float x = (float) cnra[i].x;
		float y = (float) cnra[i].y;
		float z = (float) cnra[i].z;
		int face = cnra[i].face;
		
		z += 1.5;
		glColor4ub(0,127,0,128);
		glVertex3f(x, y, z);
		glColor4ub(0,127,127,128);
		glVertex3f(x+0.5, y, z);
		glColor4ub(127,0,0,128);
		glVertex3f(x, y+0.5, z);


	}

	glEnd();

	glColor4ub(255,255,255,255);
	glPointSize(1.0);
}

#endif


}

