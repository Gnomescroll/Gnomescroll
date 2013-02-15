#pragma once

namespace HudReticle
{

class Reticle
{
    public:
        bool inited;
        float x0, y0, x1, y1;
        struct Texture tex_data;

    void init(const char* filename);
    void align_center(int window_width, int window_height);
    void draw();
    Reticle();
};

void init();

extern Reticle reticle;
extern Reticle scope_reticle;

}   // HudReticle
