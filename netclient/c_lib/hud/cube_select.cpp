#include "cube_select.hpp"


struct cube_select_element {
	int cube_id;
	int tex_id;
};

struct cube_select_element cube_select_array[255];
int cube_select_x = 0;
int cube_select_y = 0;

int init_cube_select() {
	int i;
	for(i=0;i<255;i++){
		cube_select_array[i].cube_id = -1;
		cube_select_array[i].tex_id = -1;
	}
	return 0;
}

int get_selected_cube_id(){ //get the cube selected by hud
	return cube_select_array[cube_select_x+16*cube_select_y].cube_id;
}

void cube_select_set_hud(int pos, int cube_id, int tex_id) { //set the cubes in each position on hud
	cube_select_array[pos].cube_id = cube_id;
	cube_select_array[pos].tex_id = tex_id;	
}

void draw_cube_selector(float x, float y) {

}

void hud_control_input(int pos) {
	cube_select_x = pos  % 16;
	cube_select_y = pos / 16;
}
