#include "billboard_text.hpp"
#pragma once

#ifdef DC_CLIENT
static float billboard_text_proj_mtrx[16];
#endif

BillboardText::BillboardText(int id) {
    create_particle2(&particle, id, BILLBOARD_TEXT_TYPE, 0.0f,0.0f,0.0f,0.0f,0.0f,0.0f, 0, BILLBOARD_TEXT_TTL);
    text_len = 3;
    text = "25";
}

BillboardText::BillboardText(int id, float x, float y, float z, float vx, float vy, float vz) {
    create_particle2(&particle, id, BILLBOARD_TEXT_TYPE, x,y,z,vx,vy,vz, 0, BILLBOARD_TEXT_TTL);
    text_len = 3;
    text = "25";
}

void BillboardText::tick() {
    move_simple_rk4(&particle, BILLBOARD_TEXT_DAMP);
    particle.ttl++;
}

void BillboardText::draw() {

#ifdef DC_CLIENT

    //float up[3] = {
        //billboard_text_proj_mtrx[0]*BILLBOARD_TEXT_TEXTURE_SCALE,
        //billboard_text_proj_mtrx[4]*BILLBOARD_TEXT_TEXTURE_SCALE,
        //billboard_text_proj_mtrx[8]*BILLBOARD_TEXT_TEXTURE_SCALE
    //};
    float up[3] = {
        0.0f,
        0.0f,
        //1.0f
        1.0f * BILLBOARD_TEXT_TEXTURE_SCALE
    };
    float right[3] = {
        billboard_text_proj_mtrx[1]*BILLBOARD_TEXT_TEXTURE_SCALE,
        billboard_text_proj_mtrx[5]*BILLBOARD_TEXT_TEXTURE_SCALE,
        billboard_text_proj_mtrx[9]*BILLBOARD_TEXT_TEXTURE_SCALE
    };

    int i;
    char c;
    struct Glyph glyph;
    float tx_min, tx_max, ty_min, ty_max;
    float x,y,z;
    float cursor = 0.0f;
    for (i=0; i<text_len; i++) {
        c = text[i];
        glyph = glyphs[c];
        
        tx_max = glyph.x;
        tx_min = glyph.x + glyph.tw;
        ty_min = glyph.y;
        ty_max = glyph.y + glyph.th;

        cursor += 1.8f; // use glyph.xadvance; once we figure how to scale properly

        x=particle.state.p.x; y=particle.state.p.y; z=particle.state.p.z;
        x -= right[0] * cursor;
        y -= right[1] * cursor;
        // also incorporate yoff/xoff

        glTexCoord2f(tx_min,ty_max );
        glVertex3f(x+(-right[0]-up[0]), y+(-right[1]-up[1]), z+(-right[2]-up[2]));  // Bottom left

        glTexCoord2f(tx_min,ty_min );
        glVertex3f(x+(up[0]-right[0]), y+(up[1]-right[1]), z+(up[2]-right[2]));  // Top left

        glTexCoord2f(tx_max,ty_min);
        glVertex3f(x+(up[0]+right[0]), y+(up[1]+right[1]), z+(up[2]+right[2]));  // Top right

        glTexCoord2f(tx_max,ty_max );
        glVertex3f(x+(right[0]-up[0]), y+(right[1]-up[1]), z+(right[2]-up[2]));  // Bottom right
    }

#endif    
}


/* BillboardText list */

void BillboardText_list::tick() {
    int i;
    for (i=0; i<n_max; i++) {
        if (a[i] == NULL) continue;
        a[i]->tick();
        if (a[i]->particle.ttl >= a[i]->particle.ttl_max) {
            destroy(a[i]->particle.id);
        }
    }
}

void BillboardText_list::draw() {
#ifdef DC_CLIENT

    if(num == 0) { return; }
    glGetFloatv(GL_MODELVIEW_MATRIX, billboard_text_proj_mtrx);

    glEnable(GL_TEXTURE_2D);
    glEnable (GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);

    glBindTexture( GL_TEXTURE_2D, fontTextureId );
    glEnable(GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE);

    glBegin( GL_QUADS );
set_text_color(255,10,10,255);  // red
    int i;
    for(i=0; i<n_max; i++) {
        if (a[i] == NULL) continue;
        a[i]->draw();
    }

    glEnd();
    glDepthMask(GL_TRUE);
    glDisable(GL_TEXTURE_2D);
    glDisable (GL_DEPTH_TEST);
    glDisable(GL_BLEND);

#endif
}

