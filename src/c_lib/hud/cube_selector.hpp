/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#pragma once

#include <t_map/common/types.hpp>

namespace HudCubeSelector
{

class CubeSelector
{
    private:
        int prev_blink;
        bool blink_status_visible;
    void horizontal(bool left);
    void vertical(bool up);

    public:

        float x,y;
        int n_x, n_y;
        int pos;
        int pos_x,pos_y;

        struct CubeSelectElement
        {
            CubeType cube_type;
            int tex_id;
        };

        struct CubeSelectElement* cubes;

    void set_position(float x, float y);
    void set_block_selector(int pos, CubeType cube_type, int tex_id);
    void set_block_selector(CubeType cube_type, int tex_id);
    void set_active_pos(int pos);
    void set_active_id(int id);
    int get_active_pos();
    CubeType get_active_id();
    void draw();

    void up();
    void down();
    void left();
    void right();
    void reset_blink();

    bool set_block_type(CubeType cube_type);

    CubeSelector();
    ~CubeSelector()
    {
        free(this->cubes);
    }
};

extern CubeSelector cube_selector;

void init();
void set_cube_hud(int hudx, int hudy, CubeType cube_type, int tex_id);
void set_cube_hud(CubeType cube_type, int tex_id);

}   // HudCubeSelector
