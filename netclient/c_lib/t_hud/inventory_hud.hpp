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
	float inc2 = 2; //outer border
	float slot_size = 32;

	int xdim = 6;
	int ydim = 3;

	glDisable(GL_DEPTH_TEST); // move this somewhere
	glDisable(GL_TEXTURE_2D);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

/*
	glColor4ub(50, 50, 50, 64);

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
*/
	glBegin(GL_QUADS);

	glColor4ub(50, 50, 50, 128);

	for(int i=0; i<xdim; i++)
	for(int j=0; j<ydim; j++)
	{
		float x = inventory_hud_x_off + border + i*(inc+slot_size);
		float y = inventory_hud_y_off + border + j*(inc+slot_size);

		glVertex3f(x-inc2,y+w+inc2, z);
		glVertex3f(x+w+inc2, y+w+inc2 ,z);
		glVertex3f(x+w+inc2, y-inc2, z);
		glVertex3f(x-inc2, y-inc2, z);
	}

	glColor4ub(50, 50, 50, 64);

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

	glColor4ub(255, 255, 255, 255);



    glEnable(GL_TEXTURE_2D);
    glBindTexture( GL_TEXTURE_2D, ItemSheetTexture );
    //glEnable(GL_BLEND);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //glColor3ub(255, 255, 255);

	glBegin(GL_QUADS);

	//const float z = -0.5;
	//const float w = slot_size;

	for(int i=0; i<xdim; i++)
    for(int j=0; j<ydim; j++)
	{
		if(i == 0 && j == 0) continue;

		const float x = inventory_hud_x_off + border + i*(inc+slot_size);
		const float y = inventory_hud_y_off + border + j*(inc+slot_size);

		const int tex_id = rand()%40;

		const float tx_min = (1.0/8.0)*(tex_id % 8);
		const float ty_min = (1.0/8.0)*(tex_id / 8);
		const float tx_max = tx_min + 1.0/8.0;
		const float ty_max = ty_min + 1.0/8.0;

/*
		glTexCoord2f( tx_min, ty_max );
		glVertex3f(x,y+w, z);

		glTexCoord2f( tx_max, ty_max );
		glVertex3f(x+w, y+w ,z);
	        
		glTexCoord2f( tx_max, ty_min );
		glVertex3f(x+w, y, z);

		glTexCoord2f( tx_min, ty_min );
		glVertex3f(x, y, z);
*/
		glTexCoord2f( tx_min, ty_min );
		glVertex3f(x,y+w, z);

		glTexCoord2f( tx_max, ty_min );
		glVertex3f(x+w, y+w ,z);
	        
		glTexCoord2f( tx_max, ty_max );
		glVertex3f(x+w, y, z);

		glTexCoord2f( tx_min, ty_max );
		glVertex3f(x, y, z);

	}

	glEnd();

	glEnable(GL_DEPTH_TEST); // move this somewhere
	glDisable(GL_BLEND);

}

	
}