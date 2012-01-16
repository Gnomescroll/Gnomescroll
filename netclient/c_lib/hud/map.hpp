#pragma once

namespace HudMap {

void init();
void update();
void draw();

// namespaced for cython
void update_map();
void draw_map();

}
