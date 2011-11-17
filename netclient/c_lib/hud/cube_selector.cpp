#include "cube_selector.hpp"

static struct cube_select_element cube_select_array[255];
static int cube_select_x = 0;
static int cube_select_y = 0;
static int cube_selector_cube_texture = 0;

void init_cube_selector() {
    int i;
    for(i=0;i<255;i++){
        cube_select_array[i].cube_id = 255;
        cube_select_array[i].tex_id = 254;
    }
    cube_select_x = 0;
    cube_select_y = 0;

    i = create_texture_from_file("./media/texture/hud_cube_selector.png", &cube_selector_cube_texture);
    if (i) {
        printf("init_cube_selector_textures failed with code %d\n", i);
    }
}

int get_active_cube_id(){ //get the cube selected by hud
    return cube_select_array[cube_select_x + 8 * cube_select_y].cube_id;
}

int get_active_cube_pos(){ //get the position of cube selected by hud
    return cube_select_x + (8 * cube_select_y);
}

void set_active_cube_pos(int pos) {
    cube_select_x = pos % 8;
    cube_select_y = pos / 8;

    printf("hud: block_id=%i, hud_tex=%i, hud_pos=  %i,  (%i,%i)\n", cube_select_array[pos].cube_id, cube_select_array[pos].tex_id, pos, cube_select_x, cube_select_y);
}

void set_active_cube_id(int id) {
    int i;
    for (i=0; i<255; i++) {
        if (cube_select_array[i].cube_id == id) {
            set_active_cube_pos(i);
            break;
        }
    }
}

void set_cube_selector_property(int pos, int cube_id, int tex_id) { //set the cubes in each position on hud
    if(tex_id == -1 || pos == -1) return;
    cube_select_array[pos].cube_id = cube_id;
    cube_select_array[pos].tex_id = tex_id;
    printf("cube_set: pos=%i, cube_id=%i, tex_id=%i \n", pos, cube_id, tex_id);
}

void draw_cube_selector(float x, float y, float size, int mode) {
printf("%0.2f %0.2f\n", x,y);
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

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, cube_selector_cube_texture);

    glBegin(GL_QUADS);

    for(i=0;i<8;i++) {
    for(j=0;j<8;j++) {
        x0 = x + i*(_ssize+sborder) + sborder/2;
        x1 = x0 + _ssize - sborder;
        y0 = y - j*(_ssize+sborder) + sborder/2;
        y1 = y0 + _ssize - sborder;

        tex_id = cube_select_array[i+8*j].tex_id;
        ti = tex_id % 16;
        tj = tex_id / 16;

        tx_min = ti*0.0625 + txmargin;
        ty_min = tj*0.0625 + txmargin;
        tx_max = ti*0.0625 + 0.0625 - txmargin;
        ty_max = tj*0.0625 + 0.0625 - txmargin;

        glTexCoord2f(tx_min,ty_max);
        glVertex3f(x0, y0, z_);  // Top left

        glTexCoord2f(tx_max,ty_max);
        glVertex3f(x1,y0, z_);  // Top right

        glTexCoord2f(tx_max,ty_min );
        glVertex3f(x1,y1, z_);  // Bottom right

        glTexCoord2f(tx_min,ty_min );
        glVertex3f(x0,y1, z_);  // Bottom left
    }}
    glEnd();
    glDisable(GL_TEXTURE_2D);

    // draw selected cube outline
    const float s_margin = 1.0; //selector margin
    i = cube_select_x;
    j = cube_select_y;
    //printf("i= %i, j= %i\n",i,j);

    x0 = x + i*(_ssize+sborder) + sborder/2 - s_margin;
    x1 = x + i*(_ssize+sborder) + _ssize - sborder/2 + s_margin;
    y0 = y - j*(_ssize+sborder) + sborder/2 - s_margin;
    y1 = y - j*(_ssize+sborder) + _ssize - sborder/2 + s_margin;
    
    x0 = x + i*(_ssize+sborder) + sborder/2;
    x1 = x0 + _ssize - sborder;
    y0 = y - j*(_ssize+sborder) + sborder/2;
    y1 = y0 + _ssize - sborder;
    
    glLineWidth(1.0);
    glColor4ub(0,0,255,0);  // blue
    glBegin(GL_LINE_STRIP);

        glVertex3f(x0, y0, z_);  // Top left

        glVertex3f(x1,y0, z_);  // Top right

        glVertex3f(x1,y1, z_);  // Bottom right

        glVertex3f(x0,y1, z_);  // Bottom left
        glVertex3f(x0, y0, z_);

    glEnd();
    glLineWidth(1.0);
    glColor4ub(255,255,255,255);
}
