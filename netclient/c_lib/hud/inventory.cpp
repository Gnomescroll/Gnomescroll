#include "inventory.hpp"

#include <c_lib/SDL/texture_loader.hpp>
#include <c_lib/SDL/draw_functions.hpp>

namespace HudInventory
{

//static const char* agent_inventory_texture_path = (char*)"./media/texture/hud/inventory2.png";
//static const char* agent_toolbelt_texture_path = (char*)"./media/texture/hud/inventory2.png";
//static const char* nanite_inventory_texture_path = (char*)"./media/texture/hud/inventory2.png";
//static const char* craft_bench_inventory_texture_path = (char*)"./media/texture/hud/inventory2.png";

//static const char* get_inventory_texture_path(HudElementType type)
//{
    //switch (type)
    //{
        //case HUD_ELEMENT_AGENT_INVENTORY:
            //return agent_inventory_texture_path;
        //case HUD_ELEMENT_AGENT_TOOLBELT:
            //return agent_toolbelt_texture_path;
        //case HUD_ELEMENT_NANITE_INVENTORY:
            //return nanite_inventory_texture_path;
        //case HUD_ELEMENT_CRAFTING_BENCH:
            //return craft_bench_inventory_texture_path;
        //default:
            //printf("ERROR: -- get_inventory_texture_path -- unknown HudElementType %d\n", type);
            //assert(false);
            //return (char*)"";
    //}
    //return (char*)"";
//}
    
void InventoryRender::draw()
{
    if (!this->visible) return;
    if (!this->background_texture) return;

    glColor3ub(255,255,255);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, this->background_texture);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    draw_bound_texture(this->x, this->y, this->w, this->h, this->z);

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
}

void InventoryRender::set_position(float x, float y)
{
    this->x = x;
    this->y = y;
}

int InventoryRender::get_slot_at(int x, int y)
{
    // check if point in an inventory icon's rect
    // shift coordinates to 0,0 this relative
    x -= this->x + ((int)this->border);
    y -= this->y + ((int)this->border);

    // divide point by slot dim
    int step = this->slot_size + this->icon_border + this->icon_spacing;
    int col = x/step;
    int row = y/step;

    // calculate slot position
    int slot = (row * this->xdim) + col;
    this->active_slot = slot;

    return slot;
}

void InventoryRender::init()
{
    //const char* path = get_inventory_texture_path(this->type);
    //int ret = create_texture_from_file((char*)path, &this->background_texture);
    //if (ret)
    //{
        //printf("ERROR: Failed to load InventoryRender texture %s\n", path);
        //assert(ret == 0);
        //return;
    //}
}

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
    //if (!playerAgent_state.you->status.Inventorythis->selected()) return;
    //Draw::SpriteData data;
    //playerAgent_state.you->status.Inventorythis->get_selected_icon_render_data(&data);
    //draw_icon_mask(data.x, data.y, data.w, data.h, data.z);
}


void init()
{
    char* icon_mask_path = (char*)"./media/texture/hud/icon_mask.png";
    int ret = create_texture_from_file(icon_mask_path, &icon_mask_texture);
    if (ret) printf("ERROR: Failed to load InventoryRender icon mask %s\n", icon_mask_path);
    
    agent_inventory = new InventoryRender;
    //agent_inventory->type = HUD_ELEMENT_AGENT_INVENTORY;
    agent_inventory->w = 256.0f;
    agent_inventory->h = 128.0f;
    agent_inventory->init();
    agent_inventory->set_position(_xresf/2 - 128.0f, _yresf/2 - 64.0f);
    
    agent_toolbelt = new InventoryRender;
    //agent_toolbelt->type = HUD_ELEMENT_AGENT_TOOLBELT;
    agent_toolbelt->init();
    agent_toolbelt->set_position(_xresf/2 - 128.0f, _yresf/2 - 64.0f);
    agent_toolbelt->visible = true; // always visible
    
    // todo -- nannites, craft bench
}

void teardown()
{
    delete agent_inventory;
    delete agent_toolbelt;

    // todo -- nannites, craft bench
}

//InventoryRender* get_inventory_hud_element(HudElementType type)
//{
    //switch (type)
    //{
        //case HUD_ELEMENT_AGENT_INVENTORY:
            //return agent_inventory;
        //case HUD_ELEMENT_AGENT_TOOLBELT:
            //return agent_toolbelt;
        ////case HUD_ELEMENT_NANITE_INVENTORY:
            ////return nanite_inventory;
        ////case HUD_ELEMENT_CRAFTING_BENCH:
            ////return craft_bench_inventory;
            
        //default:
            //printf("WARNING: get_inventory_hud_element -- invalid type %d\n", type);
            //assert(type == HUD_ELEMENT_AGENT_INVENTORY || type == HUD_ELEMENT_AGENT_TOOLBELT);
            //break;
    //}
    //return NULL;
//}


}
