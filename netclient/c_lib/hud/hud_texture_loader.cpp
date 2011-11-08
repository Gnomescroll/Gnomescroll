#include "hud_texture_loader.hpp"

static int item_sheet_texture;
static int item_slot_texture;
static int inventory_background_texture;

int init_hud_texture_loader() {
    int a,b,c;
    a = init_texture_from_file("./media/texture/hud/item_sheet_01.png", &item_sheet_texture);
    b = init_texture_from_file("./media/texture/hud/item_slot_41x41.png", &item_slot_texture);
    c = init_texture_from_file("./media/texture/hud/inventory_background_461x352.png", &inventory_background_texture);
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
