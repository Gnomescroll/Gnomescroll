#pragma once

namespace HudCubeSelector
{
    
//void init_cube_selector();
//int get_active_cube_id(); //get the cube selected by hud
//int get_active_cube_pos(); //get the cube selected by hud
//void set_cube_selector_property(int pos, int cube_id, int tex_id); //set the cubes in each position on hud
//void draw_cube_selector(float x, float y, float size, int mode);
//void set_active_cube_id(int id);    // set based on cube id
//void set_active_cube_pos(int pos);  // set based on cube selector position

struct cube_select_element {
    int cube_id;
    int tex_id;
};

class CubeSelector
{
    private:
        void horizontal(bool left);
        void vertical(bool up);
        void update_block_applier();
    public:
        bool inited;
        float x,y;
        float size;
        int mode;

        int n_x, n_y;
        int pos;
        int pos_x,pos_y;

        //int texture;

        struct cube_select_element cubes[256];

        void init();
        void set_position(float x, float y);
        void load_cube_property(int pos, int cube_id, int tex_id) GNOMESCROLL_API;
        void set_active_pos(int pos);
        void set_active_id(int id);
        int get_active_pos();
        int get_active_id();
        void draw();

        void up();
        void down();
        void left();
        void right();

        CubeSelector();
};

extern CubeSelector cube_selector; // GNOMESCROLL_API;

void init();
}

extern "C"
{
void set_cube_hud(int pos, int cube_id, int tex_id) GNOMESCROLL_API;;
}