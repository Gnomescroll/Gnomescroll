#include "inventory.hpp"

#include <c_lib/SDL/texture_loader.hpp>
#include <c_lib/SDL/draw_functions.hpp>

namespace HudInventory
{
    
void InventoryRender::draw()
{
    if (!this->inited) return;
    if (!this->inventory_background_texture) return;
    
    glColor3ub(255,255,255);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, this->inventory_background_texture);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    draw_bound_texture(this->x, this->y, this->width, this->height, this->z);

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
}

void InventoryRender::set_position(float x, float y)
{
    this->x = x;
    this->y = y;
}

void InventoryRender::init()
{
    if (this->inited) return;

    char* bg_path = (char*)"./media/texture/hud/inventory2.png";
    int ret = create_texture_from_file(bg_path, &this->inventory_background_texture);
    if (ret)
    {
        printf("ERROR: Failed to load InventoryRender texture %s\n", bg_path);
        return;
    }
    this->inited = true;
}

InventoryRender::InventoryRender()
:
inited(false),
visible(false),
x(0),y(0), z(-0.5f),
inventory_background_texture(0),
width(256.0f),height(128.0f) // todo
{}

GLuint icon_mask_texture = 0;

InventoryRender* agent_inventory = NULL;
InventoryRender* agent_toolbelt = NULL;
InventoryRender* nanite_inventory = NULL;
InventoryRender* craft_bench_inventory = NULL;


void draw_icon_mask(float x, float y, float w, float h, float depth)
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, icon_mask_texture);

    draw_bound_texture(x,y,w,h,depth);
    
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
}

// TODO -- this reads the agent's InventoryRender
// find a better place / way to do this
void draw_selected_icon_mask()
{
    //using ClientState::playerAgent_state;
    //if (playerAgent_state.you == NULL) return;
    //if (playerAgent_state.you->status.InventoryRender == NULL) return;
    //if (!playerAgent_state.you->status.InventoryRender->selected()) return;
    //Draw::SpriteData data;
    //playerAgent_state.you->status.InventoryRender->get_selected_icon_render_data(&data);
    //draw_icon_mask(data.x, data.y, data.w, data.h, data.z);
}


void init()
{
    char* icon_mask_path = (char*)"./media/texture/hud/icon_mask.png";
    int ret = create_texture_from_file(icon_mask_path, &icon_mask_texture);
    if (ret) printf("ERROR: Failed to load InventoryRender icon mask %s\n", icon_mask_path);
    
    agent_inventory = new InventoryRender;
    agent_inventory->init();
    agent_inventory->set_position(_xresf/2 - 128.0f, _yresf/2 - 64.0f);
    
    agent_toolbelt = new InventoryRender;
    agent_toolbelt->init();
    agent_toolbelt->set_position(_xresf/2 - 128.0f, _yresf/2 - 64.0f);
    
}

void teardown()
{
    delete agent_inventory;
    delete agent_toolbelt;
}

InventoryRender* get_inventory_hud_element(HudElementType type)
{
    switch (type)
    {
        case HUD_ELEMENT_AGENT_INVENTORY:
            return agent_inventory;
            break;
        case HUD_ELEMENT_AGENT_TOOLBELT:
            return agent_toolbelt;
            break;
        case HUD_ELEMENT_NANITE_INVENTORY:
            return nanite_inventory;
            break;
        case HUD_ELEMENT_CRAFTING_BENCH:
            return craft_bench_inventory;
            break;
            
        default:
            printf("WARNING: get_inventory_hud_element -- invalid type %d\n", type);
            assert(false);
    }
}

void get_screen_inventory_row_col(InventoryRender* inventory, int x, int y, int* row, int* col)
{
    // check if point in an inventory icon's rect
    // shift coordinates to 0,0 inventory relative
    x -= inventory->x;
    y -= inventory->y;

    // divide point by slot width/height
    //  TODO -- get icon height/width from inventory object
    const int icon_width = 32;
    const int icon_height = 32;
    *col = x/icon_width;
    *row = y/icon_height;

    // invert rows, since we draw top->bottom but coordinates are bottom->top
    // TODO -- get rows,cols from inventory objects
    //const int cols = 8;
    //const int rows = 4;
    //row = rows - row; // invert
}

}
