#pragma once

#include <c_lib/t_hud/texture.hpp>

namespace t_hud
{

int toolbelt_hud_x_off = 300;
int toolbelt_hud_y_off = 100;

float toolbelt_hud_border = 16;
float toolbelt_hud_inc1 = 8;
float toolbelt_hud_inc2 = 2; //outer border
float toolbelt_hud_slot_size = 32;

int toolbelt_hud_xdim = 9;
int toolbelt_hud_ydim = 1;

bool toolbelt_hud_mouse_to_slot(int x, int y, int* xslot, int* yslot)
{

	y = _yres - y;

	const float xoff = toolbelt_hud_x_off;
	const float yoff = toolbelt_hud_y_off;

	const float border = toolbelt_hud_border;
	const float inc1 = toolbelt_hud_inc1;	//border around slots
	//const float inc2 = inventory_hud_inc2; //outer border
	const float slot_size = toolbelt_hud_slot_size;

	const int xdim = toolbelt_hud_xdim; // 6;
	const int ydim = toolbelt_hud_ydim; // 3;

	float width = 2*border + xdim*slot_size +(xdim-1)*inc1;
	float height = 2*border + ydim*slot_size + (ydim-1)*inc1;
 
	if(x < xoff || x > xoff + width) return false;
	if(y < yoff || y > yoff + height) return false;
	if(x < xoff + border || x > xoff + width - border ) return false;
	if(y < yoff + border || y > yoff + height - border ) return false;

	*xslot = (x - (xoff + border))  / (inc1 + slot_size);
	*yslot = ydim - ((y - (yoff + border)) / (inc1 + slot_size));

	printf("toolbelt: slot %i, %i \n", *xslot, *yslot);

	return true;
}

void draw_toolbelt_hud()
{
	float z = -0.5;
	float w = 32;

	float border = toolbelt_hud_border;
	float inc1 = toolbelt_hud_inc1;
	float inc2 = toolbelt_hud_inc2; //outer border
	float slot_size = toolbelt_hud_slot_size;

	int xdim = toolbelt_hud_xdim;
	int ydim = toolbelt_hud_ydim;

	glDisable(GL_DEPTH_TEST); // move this somewhere
	glDisable(GL_TEXTURE_2D);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
/*
	glColor4ub(50, 50, 50, 64);

	{
		float x = toolbelt_hud_x_off;
		float y = toolbelt_hud_y_off;
		float w = 2*border + xdim*slot_size+ (xdim-1)*inc;
		float h = 2*border + ydim*slot_size+ (ydim-1)*inc;

	    glBegin(GL_QUADS);

		glVertex3f(x, y+h, z);
		glVertex3f(x+w, y+h ,z);
		glVertex3f(x+w, y, z);
		glVertex3f(x, y, z);

		glEnd();
	}
*/
	glBegin(GL_QUADS);

	glColor4ub(50, 50, 50, 128);

	for(int i=0; i<xdim; i++)
	for(int j=0; j<ydim; j++)
	{
		float x = toolbelt_hud_x_off + border + i*(inc1+slot_size);
		float y = toolbelt_hud_y_off + border + j*(inc1+slot_size);

		glVertex3f(x-inc2,y+w+inc2, z);
		glVertex3f(x+w+inc2, y+w+inc2 ,z);
		glVertex3f(x+w+inc2, y-inc2, z);
		glVertex3f(x-inc2, y-inc2, z);
	}

	glColor4ub(50, 50, 50, 64);

	for(int i=0; i<xdim; i++)
	for(int j=0; j<ydim; j++)
	{
	
		float x = toolbelt_hud_x_off + border + i*(inc1+slot_size);
		float y = toolbelt_hud_y_off + border + j*(inc1+slot_size);

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