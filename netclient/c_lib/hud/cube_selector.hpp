#pragma once

namespace HudCubeSelector
{

struct cube_select_element
{
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
        void set_block_selector(int pos, int cube_id, int tex_id) GNOMESCROLL_API;
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

extern CubeSelector cube_selector;

void init();
}

extern "C"
{
void set_cube_hud(int pos, int cube_id, int tex_id) GNOMESCROLL_API;;
}
