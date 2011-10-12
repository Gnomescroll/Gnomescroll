#include "inventory.hpp"

/*
int get_item_sheet_texture()
int get_item_slot_texture()
int get_inventory_background_texture()
*/

/*
    width = 1280
    height = 800
*/

int draw_inventory(float x, float y) {
	x = 50.0;
	y = 500.0;
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

    {
        const int x_size = 512;
        const int y_size = 512;

        glTexCoord2f(0.0,0.0);
        glVertex3f(x, y, z);  // Top left

        glTexCoord2f(1.0,0.0);
        glVertex3f(x+x_size, y, z);  // Top right

        glTexCoord2f(1.0,1.0);
        glVertex3i(x+x_size, y-y_size, z);  // Bottom right

        glTexCoord2f(0.0,1.0);
        glVertex3i(x, y-y_size, z);  // Bottom left
    }
    glEnd();
    //z = -0.1;
    glBindTexture(GL_TEXTURE_2D, get_item_slot_texture());
    glBegin(GL_QUADS);

/*
            glTexCoord2f(0.0,1.0);
            glVertex3f(0, 0, z);  // Top left

            glTexCoord2f(1.0,1.0);
            glVertex3f(0+64, 0, z);  // Top right

            glTexCoord2f(1.0,0.0);
            glVertex3i(0+64, 0+64, z);  // Bottom right

            glTexCoord2f(0.0,0.0);
            glVertex3i(0, 0+64, z);  // Bottom left
*/

/*
    {
        const int x_size = 64;
        const int y_size = 64;
        
            glTexCoord2f(0.0,1.0);
            glVertex3f(0, 0, z);  // Top left

            glTexCoord2f(1.0,1.0);
            glVertex3f(0+64, 0, z);  // Top right

            glTexCoord2f(1.0,0.0);
            glVertex3i(0+64, 0+64, z);  // Bottom right

            glTexCoord2f(0.0,0.0);
            glVertex3i(0, 0+64, z);  // Bottom left
    }
*/
    //x+= 0.001
    int i, j;

    {
	    float _x,_y;

        const int x_size = 64;
        const int y_size = 64;

	    const int x_off = 10;
	    const int y_off = 51;
	    const int x_inc = 50;
	    const int y_inc = 50;

	    for(i=0;i<9; i++) {
	    for(j=0;j<6; j++) {
	    	
	    	_x = x + x_off + i*x_inc;
	    	_y = y - y_off - j*y_inc;
        /*
	        glTexCoord2f(0.0,1.0);
	        glVertex3f(_x, _y, z);  // Top left

	        glTexCoord2f(1.0,1.0);
	        glVertex3f(_x+x_size, _y, z);  // Top right

	        glTexCoord2f(1.0,0.0);
	        glVertex3i(_x+x_size, _y-y_size, z);  // Bottom right

	        glTexCoord2f(0.0,0.0);
	        glVertex3i(_x, _y-y_size, z);  // Bottom left
        */
            glTexCoord2f(0.0,0.0);
            glVertex3f(_x, _y, z);  // Top left

            glTexCoord2f(1.0,0.0);
            glVertex3f(_x+x_size, _y, z);  // Top right

            glTexCoord2f(1.0,1.0);
            glVertex3i(_x+x_size, _y-y_size, z);  // Bottom right

            glTexCoord2f(0.0,1.0);
            glVertex3i(_x, _y-y_size, z);  // Bottom left

	    }}
	}

    glEnd();
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
}