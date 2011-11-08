#pragma once

#include <compat_gl.h>
#include <stdio.h>

#define FONTFILE "media/fonts/font.png"

int init_text();

int draw_text(
    char* text,
    float x, float y,
    float height, float width,
    float depth,
    int r, int g, int b, int a
);
