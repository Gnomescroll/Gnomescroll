#include "toolbelt_hud.hpp"

namespace t_hud
{

InventoryUI* agent_toolbelt = NULL;

void init_agent_toolbelt_ui()
{
    assert(agent_toolbelt == NULL);
    agent_toolbelt = new InventoryUI;

    agent_toolbelt->x = 500.0f;
    agent_toolbelt->y = 500.0f;

    // these are received from network
    agent_toolbelt->xdim = 9;
    agent_toolbelt->ydim = 1;
}


void draw_toolbelt()
{
    InventoryUI* render = agent_toolbelt;
    
    const float w = render->slot_size;
    const float inc1 = render->icon_spacing;
    const float inc2 = render->icon_border; //outer border

    glDisable(GL_DEPTH_TEST); // move render somewhere
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

    int xdim = render->xdim;
    int ydim = render->ydim;
    float slot_size = render->slot_size;
    float z = render->z;
    float border = render->border;

    for(int i=0; i<xdim; i++)
    for(int j=0; j<ydim; j++)
    {
        float x = render->x + border + i*(inc1+slot_size);
        float y = _yresf - (render->y + border + j*(inc1+slot_size));

        glVertex3f(x-inc2,y+w+inc2, z);
        glVertex3f(x+w+inc2, y+w+inc2 ,z);
        glVertex3f(x+w+inc2, y-inc2, z);
        glVertex3f(x-inc2, y-inc2, z);
    }

    glColor4ub(50, 50, 50, 64);

    for(int i=0; i<xdim; i++)
    for(int j=0; j<ydim; j++)
    {
    
        float x = render->x + border + i*(inc1+slot_size);
        float y = _yresf - (render->y + border + j*(inc1+slot_size));

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

    InventorySlot* contents = Items::get_inventory_contents(render->inventory_id);
    if (contents == NULL) return;

    glBegin(GL_QUADS);

    for(int i=0; i<xdim; i++)
    for(int j=0; j<ydim; j++)
    {
        //if(i == 0 && j == 0) continue;

        int slot = j * render->xdim + i;
        if (contents[slot].item_id == EMPTY_SLOT) continue;
        int tex_id = contents[slot].sprite_index;

        const float x = render->x + border + i*(inc1+slot_size);
        const float y = _yresf - (render->y + border + j*(inc1+slot_size));

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

    glEnable(GL_DEPTH_TEST); // move render somewhere
    glDisable(GL_BLEND);

    glDisable(GL_TEXTURE_2D);

    // draw border highlight
    if(render->selected_slot != NULL_SLOT)
    {   
        int slotx = render->selected_slot % render->xdim;
        int sloty = render->ydim - (render->selected_slot / render->xdim);
        const float x = render->x + border + slotx*(inc1+slot_size);
        const float y = _yresf - (render->y + border + (ydim-sloty)*(inc1+slot_size));

        const float b = 2 + inc2;

        glColor4ub(0, 0, 128+64, 255);
        glLineWidth(2.0);

        glBegin(GL_LINES);

        // top left, clockwise
        
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



}
