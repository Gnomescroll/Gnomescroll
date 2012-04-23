#include "hud.hpp"

void hud_handle_left_mouse_click(int x, int y)
{
    using ClientState::playerAgent_state;
    if (playerAgent_state.you == NULL) return;

    int row,col;
    if (Hud::get_screen_inventory_row_col(x,y, &row, &col))
        playerAgent_state.you->status.inventory->select_slot(row, col);
}

void hud_handle_right_mouse_click(int x, int y)
{
    using ClientState::playerAgent_state;
    if (playerAgent_state.you == NULL) return;

    int row,col;
    if (Hud::get_screen_inventory_row_col(x,y, &row, &col))
        playerAgent_state.you->status.inventory->select_and_remove_action(row, col);
}

