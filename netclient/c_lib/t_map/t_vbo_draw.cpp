#include "t_vbo_draw.hpp"

#include "shader.hpp"
#include "texture.hpp"

namespace t_map
{


/*
    Do distance check
*/
bool chunk_render_check( float x, float y)
{
    return true;
}

static const int MAX_DRAWN_VBO = 1024;  //this should not be hardcoded; will piss someone off

static int draw_vbo_n;
static struct Map_vbo* draw_vbo_array[MAX_DRAWN_VBO];


void Vbo_map::prep_draw()
{
    struct Map_vbo* col;

    int c_drawn, c_pruned;
    c_drawn=0; c_pruned=0;
    //start_vbo_draw();
    draw_vbo_n = 0;
    //printf("Start Map Draw\n");
    for(int i=0; i<map->xchunk_dim; i++) {
    for(int j=0; j<map->ychunk_dim; j++) {
        col = vbo_array[j*xchunk_dim + i ];

        if( col == NULL ) continue;
        if( chunk_render_check( col->xpos, col->ypos) ) 
        {
            c_drawn++;
            /*
                Que up map VBOs to be drawn
                !!! May want to sort VBOs in front to back order
            */

            /*
                Fulstrum culling
            */
            draw_vbo_array[draw_vbo_n] = col;
            draw_vbo_n++;
        }
        else
        {
            c_pruned++;
            //set_flag(col,VBO_drawn,0);
        }
    }}
    //sort VBOs by distance to player

    //printf("drawn: %i pruned: %i \n",  c_drawn, c_pruned);
}


void Vbo_map::draw_map() 
{

    prep_draw();

    glUseProgramObjectARB(map_shader[0]);

    glEnableVertexAttribArray(map_TexCoord);
    glEnableVertexAttribArray(map_LightMatrix);

    glColor3b(255,255,255);

    glEnable(GL_TEXTURE_2D);
    glEnable (GL_DEPTH_TEST);
    
    glShadeModel(GL_SMOOTH);

    glEnable (GL_DEPTH_TEST);   //needed?

    glBindTexture( GL_TEXTURE_2D_ARRAY, terrain_map_glsl );

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);

    struct Map_vbo* vbo;

    glEnable(GL_CULL_FACE);

    for(int i=0;i<draw_vbo_n;i++)
    {
        vbo = draw_vbo_array[i];
        if(vbo->_v_num[0] == 0)
        {
            printf("no blocks\n");
            continue; 
        } 
        glBindBuffer(GL_ARRAY_BUFFER, vbo->vbo_id);
        
        glVertexPointer(3, GL_FLOAT, sizeof(struct Vertex), (GLvoid*)0);
        glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(struct Vertex), (GLvoid*)24);

        glVertexAttribPointer(map_TexCoord, 3, GL_FLOAT, GL_FALSE, sizeof(struct Vertex), (GLvoid*)12);
        glVertexAttribPointer(map_LightMatrix, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(struct Vertex), (GLvoid*)32);

        glDrawArrays(GL_QUADS,0, vbo->_v_num[0]);
    }

    glDisableVertexAttribArray(map_TexCoord);
    glDisableVertexAttribArray(map_LightMatrix);

    glUseProgramObjectARB(0);


    //transparency, backface culling
/*
    glAlphaFunc ( GL_GREATER, 0.1 ) ;

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
*/

/*
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
*/
    //glDisable(GL_BLEND);
        
    //end draw
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);

    glDisable(GL_TEXTURE_2D);

}



}