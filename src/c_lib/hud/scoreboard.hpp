#pragma once

#include <hud/text.hpp>
#include <common/color.hpp>

namespace Hud
{

class Scoreboard
{

    public:
        static const int N_STATS = 2;
        bool inited;

        float start_x, start_y;
        float col_width;
        class Color color;

        struct {
            float x,y;
        } line_pos[PLAYERS_MAX];

        class HudText::Text* tags[N_STATS];
        class HudText::Text* ids[PLAYERS_MAX];
        class HudText::Text* names[PLAYERS_MAX];

    void update();
    void init();
    void draw_text();
    void draw_badges();

    Scoreboard();
    ~Scoreboard();
};

} // Scoreboard
