#pragma once

namespace t_hud

{

extern SDL_Surface* ItemSheetSurface;
extern unsigned int ItemSheetTexture;

extern SDL_Surface* ItemGridSlotSurface;
extern unsigned int ItemGridSlotTexture;

void init_item_grid();
void tear_down_item_grid();


class ItemGrid
{
	public:

	int xdim;
	int ydim;

	int xoff; //offset from upper left
	int yoff;

	int xinc; //pixel spacing between slots
	int yinc;

	int slot_size; // slot size
	float _slot_size;
	ItemGrid()
	{

		xdim = 16;
		ydim = 8;

		slot_size = 32;
		_slot_size = slot_size;

		xoff = 0;
		yoff = 0;

		xinc = 2;
		yinc = 2;
	}

	void draw_slot(float x, float y)
	{

		const float z = -0.5;
		const float w = _slot_size;

		const float tx_min = 0.0;
		const float ty_min = 0.0;
		const float tx_max = 0.25;
		const float ty_max = 0.25;

		glTexCoord2f( tx_min, ty_max );
		glVertex3f(x,y+w, z);

		glTexCoord2f( tx_max, ty_max );
		glVertex3f(x+w, y+w ,z);
	        
		glTexCoord2f( tx_max, ty_min );
		glVertex3f(x+w, y, z);

		glTexCoord2f( tx_min, ty_min );
		glVertex3f(x, y, z);

	}


	void draw(int _x, int _y)
	{

		glDisable(GL_TEXTURE_2D);
	    glColor3ub(128, 128, 128);
    	glBegin(GL_QUADS);

    	const float z = -0.6;
    	const float w = (xinc* (xdim+1)) + xdim*_slot_size;
    	const float h = (yinc* (ydim+1)) + ydim*_slot_size;

		glVertex3f(_x, _y, z);
		glVertex3f(_x+w, _y, z);
		glVertex3f(_x+w, _y-h, z);
		glVertex3f(_x, _y-h, z);

    	glEnd();


	    glEnable(GL_TEXTURE_2D);
	    glBindTexture( GL_TEXTURE_2D, ItemGridSlotTexture );
	    glEnable(GL_BLEND);
	    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	    glColor3ub(255, 255, 255);

    	glBegin(GL_QUADS);


    	for(int i=0; i<xdim; i++)
    	{

	    	for(int j=0; j<ydim; j++)
    		{
    			if(i == 0 && j == 0) continue;

    			draw_slot(_x + i*_slot_size+(i+1)*xinc, _y - ( (ydim-j)*_slot_size+ ((ydim-j)+1)*yinc ) );
    			//draw_slot(_x + i*_slot_size+(i+1)*xinc, _y - (ydim-(j+1)*_slot_size+ ((ydim-(j+1)+1)*yinc) ));
    		
				draw_slot(_x + i*_slot_size+(i+1)*xinc, _y - ( j*_slot_size+ (j+1)*yinc ) );

    		}

    	}

    	glEnd();

	    glDisable(GL_BLEND);

	}

};

}