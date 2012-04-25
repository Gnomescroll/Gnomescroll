#pragma once

#include <c_lib/t_hud/texture.hpp>

namespace t_hud
{

extern InventoryUI* agent_toolbelt;

void init_agent_toolbelt_ui();



    ////glEnable(GL_TEXTURE_2D);
///*
    //TSTART
//*/

    //glColor4ub(255, 255, 255, 255);
    //glEnable(GL_TEXTURE_2D);
    //glBindTexture( GL_TEXTURE_2D, ItemSheetTexture );
    ////glEnable(GL_BLEND);
    ////glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    ////glColor3ub(255, 255, 255);

    //glBegin(GL_QUADS);

    //for(int i=0; i<xdim; i++)
    //for(int j=0; j<ydim; j++)
    //{
        //const float x = toolbelt_hud_x_off + border + i*(inc1+slot_size);
        //const float y = toolbelt_hud_y_off + border + j*(inc1+slot_size);

        //const int tex_id = rand()%40;

        //const float tx_min = (1.0/8.0)*(tex_id % 8);
        //const float ty_min = (1.0/8.0)*(tex_id / 8);
        //const float tx_max = tx_min + 1.0/8.0;
        //const float ty_max = ty_min + 1.0/8.0;

        //glTexCoord2f( tx_min, ty_min );
        //glVertex3f(x,y+w, z);

        //glTexCoord2f( tx_max, ty_min );
        //glVertex3f(x+w, y+w ,z);
            
        //glTexCoord2f( tx_max, ty_max );
        //glVertex3f(x+w, y, z);

        //glTexCoord2f( tx_min, ty_max );
        //glVertex3f(x, y, z);
    //}

    //glEnd();

    //glDisable(GL_TEXTURE_2D);

    //if(selected_slot_inventory == toolbelt_id)
    //{   
        //const float x = toolbelt_hud_x_off + border + selected_slot_x*(inc1+slot_size);
        //const float y = toolbelt_hud_y_off + border + (ydim-1-selected_slot_y)*(inc1+slot_size);

        //const float b = 2 + inc2;

        //glColor4ub(0, 0, 128+64, 255);
        //glLineWidth(2.0);

        //glBegin(GL_LINES);

        //glVertex3f(x-b, y+w+b, z);
        //glVertex3f(x+w+b, y+w+b, z);

        //glVertex3f(x+w+b, y+w+b, z);
        //glVertex3f(x+w+b, y-b, z);

        //glVertex3f(x+w+b, y-b, z);
        //glVertex3f(x-b, y-b, z);

        //glVertex3f(x-b, y-b, z);
        //glVertex3f(x-b, y+w+b, z);

        //glEnd();
        ////extern int selected_slot_x;
        ////extern int selected_slot_y;)

        //glLineWidth(1.0);


    //}

}
