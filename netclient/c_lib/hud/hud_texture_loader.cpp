#include "hud_texture_loader.hpp"

static int item_sheet_texture;
static int item_slot_texture;
static int inventory_background_texture;

int init_hud_texture_loader() {
    int a,b,c;
    a =_init_texture("./media/texture/hud/item_sheet_01.png", &item_sheet_texture);
    b = _init_texture("./media/texture/hud/item_slot_41x41.png", &item_slot_texture);
    c = _init_texture("./media/texture/hud/inventory_background_461x352.png", &inventory_background_texture);
    printf("%d %d %d -HUD Texture Loader Results\n", a,b,c);
    return 0;
}

int get_item_sheet_texture() {
    return item_sheet_texture;
}

int get_item_slot_texture() {
    return item_slot_texture;
}

int get_inventory_background_texture() {
    return inventory_background_texture;
}
