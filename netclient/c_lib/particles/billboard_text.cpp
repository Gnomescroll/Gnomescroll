#include "billboard_text.hpp"
#pragma once

#ifdef DC_CLIENT
static float billboard_text_proj_mtrx[16];
#endif

BillboardText::BillboardText(int id) {
    create_particle2(&particle, id, BILLBOARD_TEXT_TYPE, 0.0f,0.0f,0.0f,0.0f,0.0f,0.0f, 0, BILLBOARD_TEXT_TTL);
    text_len = 0;
}

BillboardText::BillboardText(int id, float x, float y, float z, float vx, float vy, float vz) {
    create_particle2(&particle, id, BILLBOARD_TEXT_TYPE, x,y,z,vx,vy,vz, 0, BILLBOARD_TEXT_TTL);
    text_len = 0;
}

void BillboardText::tick() {
    move_simple_rk4(&particle, BILLBOARD_TEXT_DAMP);
    particle.ttl++;
}

/*
    Note: billboarded text is client side only, so whole file can be #ifdef DC_CLIENT

*/

#include <c_lib/camera/camera.hpp>
void BillboardText::draw() {

#ifdef DC_CLIENT
    if(!text_len || text == NULL || current_camera == NULL) {
        printf("%d %s %p\n", text_len, text, current_camera);
        return;
    }

    float up[3] = {
        0.0f,
        0.0f,
        1.0f
    };

    float norm;

    float look[3];
    look[0] = current_camera->x - particle.state.p.x;
    look[1] = current_camera->y - particle.state.p.y;
    look[2] = current_camera->z - particle.state.p.z;
    norm = sqrt(look[0]*look[0] + look[1]*look[1] + look[2]*look[2]);
    look[0] /= -norm;
    look[1] /= -norm;
    look[2] /= -norm;

    float right[3];
    right[0] = up[1]*look[2] - look[2]*up[1];
    right[1] = up[2]*look[0] - up[0]*look[2];
    right[2] = up[0]*look[1] - up[1]*look[0];
    norm = sqrt(right[0]*right[0] + right[1]*right[1] + right[2]*right[2]);
    right[0] /= norm;
    right[1] /= norm;
    right[2] /= norm;

    int i;
    char c;
    struct Glyph glyph;
    float tx_min, tx_max, ty_min, ty_max;
    float x,y,z;
    float cursor = 0.0f;

    up[0] *= BILLBOARD_TEXT_TEXTURE_SCALE;
    up[1] *= BILLBOARD_TEXT_TEXTURE_SCALE;
    up[2] *= BILLBOARD_TEXT_TEXTURE_SCALE;
    right[0] *= BILLBOARD_TEXT_TEXTURE_SCALE;
    right[1] *= BILLBOARD_TEXT_TEXTURE_SCALE;
    right[2] *= BILLBOARD_TEXT_TEXTURE_SCALE;

    const float magic_cursor_ratio = 1.8f / 9.0f;
    //text_len = 1;
    //printf("text_len = %i \n", text_len);

    for (i=0; i<text_len; i++) {
        c = text[i];
        glyph = glyphs[c];
        //glyph = glyphs[54];
        tx_max = glyph.x;
        tx_min = glyph.x + glyph.tw;
        ty_min = glyph.y;
        ty_max = glyph.y + glyph.th;


        cursor += magic_cursor_ratio * glyph.xadvance; // use glyph.xadvance; once we figure how to scale properly

        x=particle.state.p.x; y=particle.state.p.y; z=particle.state.p.z;
        x -= right[0] * cursor;
        y -= right[1] * cursor;
        z -= glyph.yoff * magic_cursor_ratio;  // magic fraction
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
printf("drew a billbaord\n");
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
    //set_text_color(255,10,10,255);  // red
    glColor3ub((unsigned char)255,(unsigned char)0,(unsigned char)0);

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

