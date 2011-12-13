#pragma once

namespace HudEquipment {

void init();

void draw();

//cython
void draw_equipment(int slot);
void set_slot_icon(int slot, int icon_id);
}
