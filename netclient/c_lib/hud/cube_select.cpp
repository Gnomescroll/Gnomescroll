#include "cube_select.hpp"


struct cube_select_element {
	int cube_id;
	int tex_id;
};

struct cube_select_element cube_select_array[255];
int cube_select_x = 0;
int cube_select_y = 0;

void _init_cube_select() {
	int i;
	for(i=0;i<255;i++){
		cube_select_array[i].cube_id = -1;
		cube_select_array[i].tex_id = 254;
	}
	return;
}

int _get_selected_cube_id(){ //get the cube selected by hud
	return cube_select_array[cube_select_x+16*cube_select_y].cube_id;
}

void _cube_select_set_hud(int pos, int cube_id, int tex_id) { //set the cubes in each position on hud
	if(tex_id == -1 || pos == -1) return;
	cube_select_array[pos].cube_id = cube_id;
	cube_select_array[pos].tex_id = tex_id;	
}

void _draw_cube_selector(float x, float y, float size, int mode) {

    int i,j;
    float x0,y0,x1,y1;

    int ti, tj;
    int tex_id;
    float tx_min,ty_min,tx_max,ty_max;

    const float _ssize = 16;
    const float sborder = 1;
    const float z_ = -0.5;

    const float txmargin = 0.001;

    glColor3ub(255,255,255);
	//glColor3b(0, 0, 0);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, get_hud_block_selector_texture());

	//printf("bind= %i\n", get_hud_block_selector_texture());

    glBegin(GL_QUADS);

    for(i=0;i<8;i++) {
    for(j=0;j<8;j++) {
    	x0 = x + i*(_ssize+sborder) + sborder/2;
    	x1 = x0 + _ssize - sborder;
    	y0 = y - j*(_ssize+sborder) + sborder/2;
    	y1 = y0 + _ssize - sborder;

    	tex_id = cube_select_array[i+16*j].tex_id;
    	ti = tex_id % 16;
    	tj = tex_id / 16;

    	tx_min = ti*0.0625 + txmargin;
    	ty_min = tj*0.0625 + txmargin;
    	tx_max = ti*0.0625 + 0.0625 - txmargin;
    	ty_max = tj*0.0625 + 0.0625 - txmargin;

/*
    	if(i==2 && j==0) {
    		//printf("tex_id= %i \n", tex_id);
   			printf("tx_min, ty_min= %f, %f \n", tx_min, ty_min);
    	}
*/


        glTexCoord2f(tx_min,ty_max);
        glVertex3f(x0, y0, z_);  // Top left

        glTexCoord2f(tx_max,ty_max);
        glVertex3f(x1,y0, z_);  // Top right

        glTexCoord2f(tx_max,ty_min );
        glVertex3f(x1,y1, z_);  // Bottom right

        glTexCoord2f(tx_min,ty_min );
        glVertex3f(x0,y1, z_);  // Bottom left
        
        //       
/*
        glTexCoord2f(tx_min,ty_max );
        glVertex3f(x0,y1, z_);  // Bottom left

        glTexCoord2f(tx_min,ty_min );
        glVertex3f(x0, y0, z_);  // Top left

        glTexCoord2f(tx_max,ty_min);
        glVertex3f(x1,y0, z_);  // Top right

        glTexCoord2f(tx_max,ty_max );
        glVertex3f(x1,y1, z_);  // Bottom right
*/
    }}

    glEnd();
    glDisable(GL_TEXTURE_2D);

 
	//need to implement draw
	return;
}

void _hud_control_input(int pos) {
	cube_select_x = pos  % 16;
	cube_select_y = pos / 16;
}
