#include "inventory.hpp"

#include <compat_gl.h>

#ifdef DC_CLIENT

static int item_sheet_texture;
static int item_slot_texture;
static int inventory_background_texture;

int init_inventory() {
    int a,b,c;
    a = create_texture_from_file((char*) "./media/texture/hud/item_sheet_01.png", &item_sheet_texture);
    b = create_texture_from_file((char*)"./media/texture/hud/item_slot_41x41.png", &item_slot_texture);
    c = create_texture_from_file((char*)"./media/texture/hud/inventory_background_461x352.png", &inventory_background_texture);
    printf("%d %d %d -HUD Texture Loader Results\n", a,b,c);
    return a||b||c;
}

int draw_inventory(float x, float y) {
    float z = -0.5;

    glColor3ub(255,255,255);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, inventory_background_texture);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   
    //draw background
    glBegin(GL_QUADS);
    {
        const int x_size = 512;
        const int y_size = 512;

        glTexCoord2f(0.0,0.0);
        glVertex3f(x, y, z);  // Top left

        glTexCoord2f(1.0,0.0);
        glVertex3f(x+x_size, y, z);  // Top right

        glTexCoord2f(1.0,1.0);
        glVertex3i(x+x_size, y-y_size, z);  // Bottom right

        glTexCoord2f(0.0,1.0);
        glVertex3i(x, y-y_size, z);  // Bottom left
    }
    glEnd();

    glBindTexture(GL_TEXTURE_2D, item_slot_texture);
    glBegin(GL_QUADS);

    {
        int i, j;
        float _x,_y;

        const int x_size = 64;
        const int y_size = 64;

        const int x_off = 10;
        const int y_off = 51;
        const int x_inc = 50;
        const int y_inc = 50;

        for(i=0;i<9; i++) {
        for(j=0;j<6; j++) {
            
            _x = x + x_off + i*x_inc;
            _y = y - y_off - j*y_inc;

            glTexCoord2f(0.0,0.0);
            glVertex3f(_x, _y, z);  // Top left

            glTexCoord2f(1.0,0.0);
            glVertex3f(_x+x_size, _y, z);  // Top right

            glTexCoord2f(1.0,1.0);
            glVertex3i(_x+x_size, _y-y_size, z);  // Bottom right

            glTexCoord2f(0.0,1.0);
            glVertex3i(_x, _y-y_size, z);  // Bottom left

        }}
    }
    glEnd();

    glBindTexture(GL_TEXTURE_2D, item_sheet_texture);

    glBegin(GL_QUADS);
    {
        int i, j;
        float _x,_y;
        int _ti, _tj;
        float tx, ty;
        int index;

        const int x_size = 32;
        const int y_size = 32;

        const int x_off = 14;
        const int y_off = 55;
        const int x_inc = 50;
        const int y_inc = 50;

        const float tx_inc = 0.0625;
        const float ty_inc = 0.0625;

        for(i=0;i<9; i++) {
        for(j=0;j<6; j++) {
            
            _x = x + x_off + i*x_inc;
            _y = y - y_off - j*y_inc;


            if(i<=j) index = 0;
            if(i>j) index = 1;

            _ti = index % 16;
            _tj = index / 16;
            
            tx = _ti*tx_inc;
            ty = _tj*ty_inc;
            
            glTexCoord2f(tx, ty);
            glVertex3f(_x, _y, z);  // Top left

            glTexCoord2f(tx+tx_inc, ty);
            glVertex3f(_x+x_size, _y, z);  // Top right

            glTexCoord2f(tx+tx_inc, ty+ty_inc);
            glVertex3i(_x+x_size, _y-y_size, z);  // Bottom right

            glTexCoord2f(tx, ty+tx_inc);
            glVertex3i(_x, _y-y_size, z);  // Bottom left

        }}
    }
    glEnd();

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);

    return 0;
}

#endif
