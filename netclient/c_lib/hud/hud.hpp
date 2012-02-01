#pragma once

void set_hud_draw_settings(
    bool zoom,
    bool cube_selector,
    bool inventory,
    bool chat_cursor
);
void set_chat_cursor(char* text, float x, float y);

void draw_hud_textures();
void draw_hud_text();

