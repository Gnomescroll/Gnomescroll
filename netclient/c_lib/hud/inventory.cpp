#include "inventory.hpp"

#include <c_lib/SDL/texture_loader.hpp>
#include <c_lib/SDL/draw_functions.hpp>

namespace HudInventory
{
    
//void Inventory::draw()
//{
    //if (!this->inited) return;
    //const float z = -0.5;

    //glColor3ub(255,255,255);

    //glEnable(GL_TEXTURE_2D);
    //glBindTexture(GL_TEXTURE_2D, inventory_background_texture);
    
    //glEnable(GL_BLEND);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   
    ////draw background
    //glBegin(GL_QUADS);
    //{
        //const int x_size = 512;
        //const int y_size = 512;

        //glTexCoord2f(0.0,0.0);
        //glVertex3f(x, y, z);  // Top left

        //glTexCoord2f(1.0,0.0);
        //glVertex3f(x+x_size, y, z);  // Top right

        //glTexCoord2f(1.0,1.0);
        //glVertex3i(x+x_size, y-y_size, z);  // Bottom right

        //glTexCoord2f(0.0,1.0);
        //glVertex3i(x, y-y_size, z);  // Bottom left
    //}
    //glEnd();

    //glBindTexture(GL_TEXTURE_2D, item_slot_texture);
    //glBegin(GL_QUADS);

    //{
        //int i, j;
        //float x,y;

        //const int x_size = 64;
        //const int y_size = 64;

        //const int x_off = 10;
        //const int y_off = 51;
        //const int x_inc = 50;
        //const int y_inc = 50;

        //for(i=0;i<9; i++)
        //for(j=0;j<6; j++)
        //{
            //x = this->x + x_off + i*x_inc;
            //y = this->y - y_off - j*y_inc;

            //glTexCoord2f(0.0,0.0);
            //glVertex3f(x, y, z);  // Top left

            //glTexCoord2f(1.0,0.0);
            //glVertex3f(x+x_size, y, z);  // Top right

            //glTexCoord2f(1.0,1.0);
            //glVertex3i(x+x_size, y-y_size, z);  // Bottom right

            //glTexCoord2f(0.0,1.0);
            //glVertex3i(x, y-y_size, z);  // Bottom left
        //}
    //}
    //glEnd();

    //glBindTexture(GL_TEXTURE_2D, item_sheet_texture);

    //glBegin(GL_QUADS);
    //{
        //int i, j;
        //float x,y;
        //int ti, tj;
        //float tx, ty;
        //int index;

        //const int x_size = 32;
        //const int y_size = 32;

        //const int x_off = 14;
        //const int y_off = 55;
        //const int x_inc = 50;
        //const int y_inc = 50;

        //const float tx_inc = 0.0625;
        //const float ty_inc = 0.0625;

        //for(i=0;i<9; i++) {
        //for(j=0;j<6; j++) {
            
            //x = this->x + x_off + i*x_inc;
            //y = this->y - y_off - j*y_inc;


            //if(i<=j) index = 0;
            //if(i>j) index = 1;

            //ti = index % 16;
            //tj = index / 16;
            
            //tx = ti*tx_inc;
            //ty = tj*ty_inc;
            
            //glTexCoord2f(tx, ty);
            //glVertex3f(x, y, z);  // Top left

            //glTexCoord2f(tx+tx_inc, ty);
            //glVertex3f(x+x_size, y, z);  // Top right

            //glTexCoord2f(tx+tx_inc, ty+ty_inc);
            //glVertex3i(x+x_size, y-y_size, z);  // Bottom right

            //glTexCoord2f(tx, ty+tx_inc);
            //glVertex3i(x, y-y_size, z);  // Bottom left

        //}}
    //}
    //glEnd();

    //glDisable(GL_TEXTURE_2D);
    //glDisable(GL_BLEND);
//}

void Inventory::draw()
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

void Inventory::set_position(float x, float y)
{
    this->x = x;
    this->y = y;
}

void Inventory::init()
{
    if (this->inited) return;

    //int a,b,c;
    ////a = create_texture_from_file((char*) "./media/texture/hud/item_sheet_01.png", &this->item_sheet_texture);
    ////b = create_texture_from_file((char*)"./media/texture/hud/item_slot_41x41.png", &this->item_slot_texture);
    ////c = create_texture_from_file((char*)"./media/texture/hud/inventory_background_461x352.png", &this->inventory_background_texture);
    //if(a!=0 || b!=0 || c!=0) printf("%d %d %d -Inventory Texture Loader Error (any nonzero is failure)\n", a,b,c);
    //if (a||b||c) return;

    char* bg_path = (char*)"./media/texture/hud/inventory2.png";
    int ret = create_texture_from_file(bg_path, &this->inventory_background_texture);
    if (ret)
    {
        printf("ERROR: Failed to load inventory texture %s\n", bg_path);
        return;
    }
    
    this->inited = true;
}

Inventory::Inventory()
:
inited(false),
x(0),y(0), z(-0.5f),
//item_sheet_texture(0),
//item_slot_texture(0),
inventory_background_texture(0),
width(256.0f),height(128.0f) // todo
{}

GLuint icon_mask_texture = 0;
Inventory* inventory = NULL;

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

// TODO -- this reads the agent's inventory
// find a better place / way to do this
void draw_selected_icon_mask()
{
    //using ClientState::playerAgent_state;
    //if (playerAgent_state.you == NULL) return;
    //if (playerAgent_state.you->status.inventory == NULL) return;
    //if (!playerAgent_state.you->status.inventory->selected()) return;
    //Draw::SpriteData data;
    //playerAgent_state.you->status.inventory->get_selected_icon_render_data(&data);
    //draw_icon_mask(data.x, data.y, data.w, data.h, data.z);
}


void init()
{
    char* icon_mask_path = (char*)"./media/texture/hud/icon_mask.png";
    int ret = create_texture_from_file(icon_mask_path, &icon_mask_texture);
    if (ret) printf("ERROR: Failed to load inventory icon mask %s\n", icon_mask_path);
    
    inventory = new Inventory;
    inventory->init();
    inventory->set_position(_xresf/2 - 128.0f, _yresf/2 - 64.0f);
}

void teardown()
{
    delete inventory;
}

}
