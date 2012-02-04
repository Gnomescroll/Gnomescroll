#include "t_vbo_draw.hpp"

namespace t_map
{

void prep_draw()
{
    struct vm_map* m;
    struct vm_column* col;
    int i,j;
    int c_drawn, c_pruned;
    c_drawn=0; c_pruned=0;
    //start_vbo_draw();
    draw_vbo_n = 0;
    m = _get_map();
    //printf("Start Map Draw\n");
    for(i=0; i<vm_map_dim; i++) {
    for(j=0; j<vm_map_dim; j++) {
        col = &m->column[j*vm_map_dim+i];
        if(flag_is_true(col, VBO_loaded) && chunk_render_check(col->x_off, col->y_off, 0)) {
            c_drawn++;
            set_flag(col,VBO_drawn,1);
            /*
                Que up map VBOs to be drawn
                !!! May want to sort VBOs in front to back order
            */

            /*
                Fulstrum culling
            */
            draw_vbo_array[draw_vbo_n] = &col->vbo;
            draw_vbo_n++;
        } else {
            set_flag(col,VBO_drawn,0);
            c_pruned++;
        }
    }}
    return 0;
}

void draw_map() 
{
    prep_draw();

    const int index = 0;
    
    glUseProgramObjectARB(shader_prog4);

    glEnableVertexAttribArray(texCoord0Loc_4);
    glEnableVertexAttribArray(LightMatrix0Loc_4);

    glColor3b(255,255,255);

    glEnable(GL_TEXTURE_2D);
    glEnable (GL_DEPTH_TEST);
    
    glShadeModel(GL_SMOOTH);
    //glShadeModel(GL_FLAT);

    glEnable (GL_DEPTH_TEST);
    glAlphaFunc ( GL_GREATER, 0.1 ) ;

    glBindTexture( GL_TEXTURE_2D_ARRAY, terrain_map_glsl_1 );

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);

    int i;
    struct VBO* vbo;

    glEnable(GL_CULL_FACE);
    for(i=0;i<draw_vbo_n;i++) {
        vbo = draw_vbo_array[i];
        if(vbo->_v_num[0] == 0) continue; 
        glBindBuffer(GL_ARRAY_BUFFER, vbo->VBO_id);
        
        glVertexPointer(3, GL_FLOAT, sizeof(struct Vertex), (GLvoid*)0);
        glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(struct Vertex), (GLvoid*)24);

        glVertexAttribPointer(texCoord0Loc_4, 3, GL_FLOAT, GL_FALSE, sizeof(struct Vertex), (GLvoid*)12);
        glVertexAttribPointer(LightMatrix0Loc_4, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(struct Vertex), (GLvoid*)32);

        glDrawArrays(GL_QUADS,0, vbo->_v_num[0]);
    }

    glDisableVertexAttribArray(texCoord0Loc_4);
    glDisableVertexAttribArray(LightMatrix0Loc_4);

    glUseProgramObjectARB(0);
    glActiveTexture(GL_TEXTURE0);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable (GL_ALPHA_TEST);

    for(i=0;i<draw_vbo_n;i++) {
        vbo = draw_vbo_array[i];
        if(vbo->_v_num[1] == 0) continue; 
        glBindBuffer(GL_ARRAY_BUFFER, vbo->VBO_id);
        glVertexPointer(3, GL_FLOAT, sizeof(struct Vertex), (GLvoid*)0);
        glTexCoordPointer(2, GL_FLOAT, sizeof(struct Vertex), (GLvoid*)12);
        glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(struct Vertex), (GLvoid*)24);
        glDrawArrays(GL_QUADS, vbo->_v_offset[1], vbo->_v_num[1]);
    }

    glDisable(GL_CULL_FACE);
    for(i=0;i<draw_vbo_n;i++) {
        vbo = draw_vbo_array[i];
        if(vbo->_v_num[2] == 0) continue; 
        glBindBuffer(GL_ARRAY_BUFFER, vbo->VBO_id);
        glVertexPointer(3, GL_FLOAT, sizeof(struct Vertex), (GLvoid*)0);
        glTexCoordPointer(2, GL_FLOAT, sizeof(struct Vertex), (GLvoid*)12);
        glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(struct Vertex), (GLvoid*)24);
        glDrawArrays(GL_QUADS, vbo->_v_offset[2], vbo->_v_num[2]);
    }
    
    glDisable(GL_ALPHA_TEST);   

    glDepthMask(false);
    for(i=0;i<draw_vbo_n;i++) {
        vbo = draw_vbo_array[i];
        if(vbo->_v_num[3] == 0) continue; 
        glBindBuffer(GL_ARRAY_BUFFER, vbo->VBO_id);
        glVertexPointer(3, GL_FLOAT, sizeof(struct Vertex), (GLvoid*)0);
        glTexCoordPointer(2, GL_FLOAT, sizeof(struct Vertex), (GLvoid*)12);
        glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(struct Vertex), (GLvoid*)24);
        glDrawArrays(GL_QUADS, vbo->_v_offset[3], vbo->_v_num[3]);
    }
    glDepthMask(true); 
    glDisable(GL_BLEND);
        
    //end draw
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);

    glDisable(GL_TEXTURE_2D);

}



}