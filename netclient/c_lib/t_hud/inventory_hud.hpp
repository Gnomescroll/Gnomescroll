#pragma once

namespace t_hud
{


int inventory_hud_x_off = 300;
int inventory_hud_y_off = 300;



void draw_inventory_hud()
{
	float z = -0.5;
	float w = 32;
	float border = 16;
	float inc = 8;
	float slot_size = 32;

	int xdim = 6;
	int ydim = 3;

	glDisable(GL_DEPTH_TEST); // move this somewhere
	glDisable(GL_TEXTURE_2D);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4ub(50, 50, 50, 50);

	{
		float x = inventory_hud_x_off;
		float y = inventory_hud_y_off;
		float w = 2*border + xdim*slot_size+ (xdim-1)*inc;
		float h = 2*border + ydim*slot_size+ (ydim-1)*inc;

	    glBegin(GL_QUADS);

		glVertex3f(x, y+h, z);
		glVertex3f(x+w, y+h ,z);
		glVertex3f(x+w, y, z);
		glVertex3f(x, y, z);

		glEnd();
	}


	glColor4ub(255, 50, 50, 50);

	glBegin(GL_QUADS);

	for(int i=0; i<xdim; i++)
	for(int j=0; j<ydim; j++)
	{
	
		float x = inventory_hud_x_off + border + i*(inc+slot_size);
		float y = inventory_hud_y_off + border + j*(inc+slot_size);


		glVertex3f(x,y+w, z);
		glVertex3f(x+w, y+w ,z);
		glVertex3f(x+w, y, z);
		glVertex3f(x, y, z);
	}

	glEnd();

	glEnable(GL_TEXTURE_2D);

	glEnable(GL_DEPTH_TEST); // move this somewhere
	glDisable(GL_BLEND);

	glColor4ub(255, 255, 255, 255);

}

	
}