#include "inventory.hpp"

/*
int get_item_sheet_texture()
int get_item_slot_texture()
int get_inventory_background_texture()
*/

int draw_inventory(float x, float y) {
	//x = 250.0;
	//y = 250.0;
	float z = -0.5;

	//printf("i=%i \n",get_inventory_background_texture());

    glColor3ub(255,255,255);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, get_inventory_background_texture());
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   
   	//draw background
    glBegin(GL_QUADS);
/*
        glTexCoord2f(0.0,1.0);	
        glVertex3f(x, y, z);  // Top left

        glTexCoord2f(1.0,1.0);
        glVertex3f(x+621.0, y, z);  // Top right

        glTexCoord2f(1.0,0.0);
        glVertex3f(x+621.0, y+352.0, z);  // Bottom right

        glTexCoord2f(0.0,0.0);
        glVertex3f(x, y+352.0, z);  // Bottom left
*/
        //stuff

        glTexCoord2f(0.0,1.0);
        glVertex3f(x, y, z);  // Top left

        glTexCoord2f(1.0,1.0);
        glVertex3f(x+512, y, z);  // Top right

        glTexCoord2f(1.0,0.0);
        glVertex3i(x+512, y+512, z);  // Bottom right

        glTexCoord2f(0.0,0.0);
        glVertex3i(x, y+512, z);  // Bottom left

    glEnd();
    


    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
}