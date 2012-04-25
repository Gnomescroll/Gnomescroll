#include "inventory_ui.hpp"

#include <c_lib/physics/common.hpp>

#include <c_lib/t_hud/constants.hpp>

#include <c_lib/t_hud/inventory_hud.hpp>
#include <c_lib/t_hud/toolbelt_hud.hpp>

namespace t_hud
{

int InventoryUI::get_slot_at(int px, int py)
{
    px -= this->x + this->border;
    py -= this->y - this->border;

    float width  = xdim*slot_size + (xdim-1)*icon_spacing;
    float height = ydim*slot_size + (ydim-1)*icon_spacing;

    if (px < 0 || px > width)  return NULL_SLOT;
    if (py < 0 || py > height) return NULL_SLOT;

    int xslot = px / (icon_spacing + slot_size);
    int yslot = py / (icon_spacing + slot_size);

    int slot = yslot * this->xdim + xslot;
    
    //printf("inventory: slot %d, %d\n", xslot, yslot);

    return slot;
}

void InventoryUI::draw()
{
    const float w = this->slot_size;
    const float inc1 = this->icon_spacing;
    const float inc2 = this->icon_border; //outer border

    glDisable(GL_DEPTH_TEST); // move this somewhere
    glDisable(GL_TEXTURE_2D);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

//#if 0
    //glColor4ub(0, 0, 50, 64);

    //{
        //float x = inventory_hud_x_off;
        //float y = inventory_hud_y_off;
        //float w = 2*border + xdim*slot_size+ (xdim-1)*inc1;
        //float h = 2*border + ydim*slot_size+ (ydim-1)*inc1;

        //glBegin(GL_QUADS);

        //glVertex3f(x, y+h, z);
        //glVertex3f(x+w, y+h ,z);
        //glVertex3f(x+w, y, z);
        //glVertex3f(x, y, z);

        //glEnd();

    //}
//#endif

    glBegin(GL_QUADS);

    glColor4ub(50, 50, 50, 128);

    for(int i=0; i<xdim; i++)
    for(int j=0; j<ydim; j++)
    {
        float x = this->x + border + i*(inc1+slot_size);
        float y = _yresf - (this->y + border + j*(inc1+slot_size));

        glVertex3f(x-inc2,y+w+inc2, z);
        glVertex3f(x+w+inc2, y+w+inc2 ,z);
        glVertex3f(x+w+inc2, y-inc2, z);
        glVertex3f(x-inc2, y-inc2, z);
    }

    glColor4ub(50, 50, 50, 64);

    for(int i=0; i<xdim; i++)
    for(int j=0; j<ydim; j++)
    {
    
        float x = this->x + border + i*(inc1+slot_size);
        float y = _yresf - (this->y + border + j*(inc1+slot_size));

        glVertex3f(x,y+w, z);
        glVertex3f(x+w, y+w ,z);
        glVertex3f(x+w, y, z);
        glVertex3f(x, y, z);
    }

    glEnd();

    glEnable(GL_TEXTURE_2D);

    glColor4ub(255, 255, 255, 255);



    glEnable(GL_TEXTURE_2D);
    glBindTexture( GL_TEXTURE_2D, ItemSheetTexture );
    //glEnable(GL_BLEND);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //glColor3ub(255, 255, 255);

    InventorySlot* contents = Items::get_inventory_contents(this->inventory_id);
    if (contents == NULL) return;

    glBegin(GL_QUADS);

    for(int i=0; i<xdim; i++)
    for(int j=0; j<ydim; j++)
    {
        //if(i == 0 && j == 0) continue;

        int slot = j * this->xdim + i;
        if (contents[slot].item_id == EMPTY_SLOT) continue;
        int tex_id = contents[slot].sprite_index;

        const float x = this->x + border + i*(inc1+slot_size);
        const float y = _yresf - (this->y + border + j*(inc1+slot_size));

        const float tx_min = (1.0/8.0)*(tex_id % 8);
        const float ty_min = (1.0/8.0)*(tex_id / 8);
        const float tx_max = tx_min + 1.0/8.0;
        const float ty_max = ty_min + 1.0/8.0;

        glTexCoord2f( tx_min, ty_min );
        glVertex3f(x,y+w, z);

        glTexCoord2f( tx_max, ty_min );
        glVertex3f(x+w, y+w ,z);
            
        glTexCoord2f( tx_max, ty_max );
        glVertex3f(x+w, y, z);

        glTexCoord2f( tx_min, ty_max );
        glVertex3f(x, y, z);

    }

    glEnd();

    glEnable(GL_DEPTH_TEST); // move this somewhere
    glDisable(GL_BLEND);

    glDisable(GL_TEXTURE_2D);

    // draw border highlight
    if(this->selected_slot == inventory_id)
    {   
        int slotx = this->selected_slot % this->xdim;
        int sloty = this->selected_slot / this->xdim;
        const float x = this->x + border + slotx*(inc1+slot_size);
        const float y = _yresf - (this->y + border + (ydim-1-sloty)*(inc1+slot_size));

        const float b = 2 + inc2;

        glColor4ub(0, 0, 128+64, 255);
        glLineWidth(2.0);

        glBegin(GL_LINES);

        glVertex3f(x-b, y+w+b, z);
        glVertex3f(x+w+b, y+w+b, z);

        glVertex3f(x+w+b, y+w+b, z);
        glVertex3f(x+w+b, y-b, z);

        glVertex3f(x+w+b, y-b, z);
        glVertex3f(x-b, y-b, z);

        glVertex3f(x-b, y-b, z);
        glVertex3f(x-b, y+w+b, z);

        glEnd();
        //extern int selected_slot_x;
        //extern int selected_slot_y;)

        glLineWidth(1.0);


    }


    glColor4ub(255, 255, 255, 255);

}

InventoryUI* nanite_inventory = NULL;
InventoryUI* craft_bench_inventory = NULL;

void init_nanite_inventory_ui()
{
    assert(nanite_inventory == NULL);
    
}

void init_craft_bench_inventory_ui()
{
    assert(craft_bench_inventory == NULL);

}

void teardown_inventory_ui()
{
    if (agent_inventory != NULL) delete agent_inventory;
    if (agent_toolbelt != NULL) delete agent_toolbelt;
    if (nanite_inventory != NULL) delete nanite_inventory;
    if (craft_bench_inventory != NULL) delete craft_bench_inventory;
}

InventoryUI* get_inventory_hud_element(HudElementType type)
{
    switch (type)
    {
        case HUD_ELEMENT_AGENT_INVENTORY:
            return agent_inventory;
        case HUD_ELEMENT_AGENT_TOOLBELT:
            return agent_toolbelt;
        case HUD_ELEMENT_NANITE_INVENTORY:
            return nanite_inventory;
        case HUD_ELEMENT_CRAFTING_BENCH:
            return craft_bench_inventory;
            
        default:
            printf("WARNING: get_inventory_hud_element -- invalid type %d\n", type);
            assert(false);
            break;
    }
    return NULL;
}

} // t_hud
