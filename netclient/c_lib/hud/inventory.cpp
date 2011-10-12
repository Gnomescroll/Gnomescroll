#include "inventory.hpp"

/*
int get_item_sheet_texture()
int get_item_slot_texture()
int get_inventory_background_texture()
*/

int draw_inventory(float x, float y) {
	x = 250.0;
	y = 250.0;
	float z = -0.5;

    glColor4ub(255,255,255,255);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, get_inventory_background_texture());

   
    //glBegin(GL_QUADS);

    	
        glVertex3f(x, y, z);  // Top left

        glVertex3f(x+200.0, y, z);  // Top right

        glVertex3f(x+200.0, y+250.0, z);  // Bottom right

        glVertex3f(x, y+250.0, z);  // Bottom left

    glEnd();
    glDisable(GL_TEXTURE_2D);
}