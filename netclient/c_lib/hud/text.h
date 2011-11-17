#pragma once

#include <compat_gl.h>
#include <stdio.h>

int load_font(char* fontfile);

void start_text_draw(int r, int g, int b, int a);
void end_text_draw();
void blit_glyph(
    float tex_x_min, float tex_x_max,
    float tex_y_min, float tex_y_max,
    float screen_x_min, float screen_x_max,
    float screen_y_min, float screen_y_max,
    float depth
);
