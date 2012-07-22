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

unsigned int control_node_texture;

class SHADER* control_node_shader;

void init_control_node_texture()
{
	SDL_Surface* s = create_surface_from_file("./media/sprites/territory_00.png");

	glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &control_node_texture);
	glBindTexture(GL_TEXTURE_2D, control_node_texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	GLenum texture_format;
	if (s->format->Rmask == 0x000000ff)
		texture_format = GL_RGBA;
	else
		texture_format = GL_BGRA;

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, s->w, s->h, 0, texture_format, GL_UNSIGNED_BYTE, s->pixels); //2nd parameter is level
	
	glDisable(GL_TEXTURE_2D);
}

//uniforms
unsigned int control_node_CameraPosition;
//attributes
unsigned int control_node_Vertex;
unsigned int control_node_TexCoord;
unsigned int control_node_Brightness;

void init_control_node_shader(class SHADER* shader)
{
    shader->set_debug(true);

    shader->load_shader( "control_node_shader",
        "./media/shaders/effect/control_node.vsh",
        "./media/shaders/effect/control_node.fsh" );

    control_node_CameraPosition = 	shader->get_uniform("CameraPosition");
    
    control_node_Vertex =			shader->get_attribute("InVertex");
    control_node_TexCoord = 		shader->get_attribute("InTexCoord");
    control_node_Brightness	=		shader->get_attribute("InBrightness");
}


void control_node_render_init(class CONTROL_NODE_LIST* _cnl)
{
	cnra = (struct CONTROL_NODE_RENDER*) malloc(cnrm*sizeof(struct CONTROL_NODE_RENDER));
	cnl = _cnl; //

	init_control_node_texture();

	control_node_shader = new SHADER;
	init_control_node_shader(control_node_shader);
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

void control_node_render_update()
{
	static int counter = 0; //refresh, deals with loading before terrain map
	counter++;

	//if(cnl->needs_update == false && cnri != 0 && counter%30!=0) return;
if(cnl->needs_update == false && cnri != 0 ) return;
	cnl->needs_update = false;
	cnri = 0; //reset index

	const int size = 6;	//size of control grid to left or right of block

	for(int _i=0; _i<cnl->cpi; _i++)
	{
		int _x = cnl->cpa[_i].x;
		int _y = cnl->cpa[_i].y;
		int _z = cnl->cpa[_i].z;

		//top
		for(int i=-size; i<=size; i++)
		for(int j=-size; j<=size; j++)
		{
			int x = _x + i;
			int y = _y + j;
			int z = _z + size;

			if(!isSolid(x,y,z) && !isSolid(x,y,z+1) )
			{
				int face = 0;	//orientation
				_insert_control_node_render_element(x,y,z, face);
			}
		}

		//bottom
		for(int i=-size; i<=size; i++)
		for(int j=-size; j<=size; j++)
		{
			int x = _x + i;
			int y = _y + j;
			int z = _z - size;

			if(!isSolid(x,y,z) && !isSolid(x,y,z-1) )
			{
				int face = 1;	//orientation
				_insert_control_node_render_element(x,y,z, face);
			}
		}

		//north
		for(int i=-size; i<=size; i++)
		for(int j=-size; j<=size; j++)
		{
			int x = _x + size;
			int y = _y + i;
			int z = _z + j;

			if(!isSolid(x,y,z) && !isSolid(x,y,z-1) )
			{
				int face = 2;	//orientation
				_insert_control_node_render_element(x,y,z, face);
			}
		}

		//south
		for(int i=-size; i<=size; i++)
		for(int j=-size; j<=size; j++)
		{
			int x = _x - size;
			int y = _y + i;
			int z = _z + j;

			if(!isSolid(x,y,z) && !isSolid(x,y,z-1) )
			{
				int face = 3;	//orientation
				_insert_control_node_render_element(x,y,z, face);
			}
		}

		//west
		for(int i=-size; i<=size; i++)
		for(int j=-size; j<=size; j++)
		{
			int x = _x + i;
			int y = _y + size;
			int z = _z + j;

			if(!isSolid(x,y,z) && !isSolid(x,y,z-1) )
			{
				int face = 4;	//orientation
				_insert_control_node_render_element(x,y,z, face);
			}
		}

		//east
		for(int i=-size; i<=size; i++)
		for(int j=-size; j<=size; j++)
		{
			int x = _x + i;
			int y = _y - size;
			int z = _z + j;

			if(!isSolid(x,y,z) && !isSolid(x,y,z-1) )
			{
				int face = 5;	//orientation
				_insert_control_node_render_element(x,y,z, face);
			}
		}

	}
	
}

void control_node_render_draw()
{
	static const float v_index[72] = 
	{
    	1,1,1 , 0,1,1 , 0,0,1 , 1,0,1 , //top
    	0,1,0 , 1,1,0 , 1,0,0 , 0,0,0 , //bottom
    	1,0,1 , 1,0,0 , 1,1,0 , 1,1,1 , //north
    	0,1,1 , 0,1,0 , 0,0,0 , 0,0,1 , //south
    	1,1,1 , 1,1,0 , 0,1,0,  0,1,1 , //west
    	0,0,1 , 0,0,0 , 1,0,0 , 1,0,1 , //east
	};

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, control_node_texture);

	glColor4ub(127,0,0,128);

	glEnable(GL_BLEND);
	glDepthMask(GL_FALSE);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE);

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

		//x += 0.33;


		int tex_id = 0;

		const float txmargin = 0.0f;
		float tx_min, ty_min, tx_max, ty_max;

        int ti = tex_id % 4;
        int tj = tex_id / 4;

        tx_min = ti*0.25f + txmargin;
        ty_min = tj*0.25f + txmargin;
        tx_max = ti*0.25f + 0.25f - txmargin;
        ty_max = tj*0.25f + 0.25f - txmargin;

		int s = face;

		glTexCoord2f(tx_min,ty_max);
		glVertex3f(x+v_index[12*s+3*0+0], y+v_index[12*s+3*0+1], z+v_index[12*s+3*0+2]);

        glTexCoord2f(tx_max,ty_max);
		glVertex3f(x+v_index[12*s+3*1+0], y+v_index[12*s+3*1+1], z+v_index[12*s+3*1+2]);

        glTexCoord2f(tx_max,ty_min );
		glVertex3f(x+v_index[12*s+3*2+0], y+v_index[12*s+3*2+1], z+v_index[12*s+3*2+2]);

        glTexCoord2f(tx_min,ty_min );
		glVertex3f(x+v_index[12*s+3*3+0], y+v_index[12*s+3*3+1], z+v_index[12*s+3*3+2]);


	/*
		glVertex3f(x, y, z+1.0);
		glVertex3f(x, y+1.0, z+1.0);
		glVertex3f(x, y+1.0, z);
		glVertex3f(x, y, z);
	*/
	}

	glEnd();
	glDisable(GL_BLEND);
	glDepthMask(GL_TRUE);

	glDisable(GL_TEXTURE_2D);


/*
	glPointSize(6.0);

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
	glPointSize(1.0);
*/
	glColor4ub(255,255,255,255);

}

#endif


}

