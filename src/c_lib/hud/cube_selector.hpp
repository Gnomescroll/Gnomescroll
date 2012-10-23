#pragma once

#include <t_map/common/types.hpp>

namespace HudCubeSelector
{

const int N_CUBES = 256;

struct cube_select_element
{
    CubeID cube_id;
    int tex_id;
};

class CubeSelector
{
    private:
        void horizontal(bool left);
        void vertical(bool up);

    public:
        float x,y;
        float size;
        int mode;

        int n_x, n_y;
        int pos;
        int pos_x,pos_y;

        //int texture;

        struct cube_select_element* cubes;

        void init();
        void set_position(float x, float y);
        void set_block_selector(int pos, CubeID cube_id, int tex_id);
        void set_active_pos(int pos);
        void set_active_id(int id);
        int get_active_pos();
        CubeID get_active_id();
        void draw();

        void up();
        void down();
        void left();
        void right();

        bool set_block_type(CubeID cube_id);

        CubeSelector();
};

extern CubeSelector cube_selector;

void init();


}   // HudCubeSelector

void set_cube_hud(int hudx, int hudy, CubeID cube_id, int tex_id);
