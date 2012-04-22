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

		xinc = 4;
		yinc = 4;
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
	    glEnable(GL_TEXTURE_2D);
	    glBindTexture( GL_TEXTURE_2D, ItemGridSlotTexture );
	    glEnable(GL_BLEND);
	    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	    glColor3ub(255, 255, 255);

    	glBegin(GL_QUADS);



    	glEnd();

	    glDisable(GL_BLEND);

	}

};

}