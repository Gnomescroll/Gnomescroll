#include "t_vbo_draw.hpp"

#include <c_lib/t_map/glsl/shader.hpp>
#include <c_lib/t_map/glsl/texture.hpp>

#include <c_lib/camera/camera.hpp>
#include <c_lib/camera/fulstrum_test.hpp>

namespace t_map
{


/*
    Do distance check
*/
bool chunk_render_check( float x, float y)
{
    const float dist = 128.0;
    const float dist2 = dist*dist;

    float dx = current_camera->x - x;
    float dy = current_camera->y - y;

    return (dx*dx + dy*dy > dist2) ? false : true;
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
        //if( chunk_render_check( col->xpos, col->ypos) && xy_point_fulstrum_test(col->xpos, col->ypos) )
        //if( chunk_render_check( col->xpos, col->ypos) && xy_circle_fulstrum_test(col->xpos, col->ypos, 11.4) )
        if( chunk_render_check( col->xpos, col->ypos) && xy_circle_fulstrum_test(col->xpos, col->ypos, 32.0) )
        // probably an error

        //xy_circle_fulstrum_test(col->xpos, col->ypos, 8.0)
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



//float _normal_array[3*6];
float _chunk_position[3] = {0};


const float _normal_array[3*6] = 
{ 
    0.0,0.0,1.0,
    0.0,0.0,-1.0,
    1.0,0.0,0.0,
    -1.0,0.0,0.0,
    0.0,1.0,0.0,
    0.0,-1.0,0.0,
};

void Vbo_map::draw_map() 
{

    prep_draw();

    glUseProgramObjectARB(map_shader[0]);



    glColor3b(255,255,255);

    //glEnable(GL_TEXTURE_2D);
    glEnable (GL_DEPTH_TEST);
    
    glShadeModel(GL_SMOOTH);

    glEnable (GL_DEPTH_TEST);   //needed?

    glBindTexture( GL_TEXTURE_2D_ARRAY, terrain_map_glsl );


    //glEnableClientState(GL_VERTEX_ARRAY);
    //glEnableClientState(GL_COLOR_ARRAY);

    glEnableVertexAttribArray(map_Vertex);
    glEnableVertexAttribArray(map_TexCoord);
    glEnableVertexAttribArray(map_RGB);
    //glEnableVertexAttribArray(map_Normal);

    glEnableVertexAttribArray(map_LightMatrix);

    //printf("mapshader= %i  texture= %i \n", map_shader[0], terrain_map_glsl );
    
    //printf("%i = \n", map_TexCoord);

    struct Map_vbo* vbo;

    glEnable(GL_CULL_FACE);

    glUniform3fv(map_NormalArray , 6, (GLfloat*) _normal_array );

    for(int i=0;i<draw_vbo_n;i++)
    {
        vbo = draw_vbo_array[i];

        if(vbo->_v_num[0] == 0)
        {
            printf("t_vbo_draw.cpp:117 no blocks\n");
            continue; 
        } 
        glBindBuffer(GL_ARRAY_BUFFER, vbo->vbo_id);
        

        //glUniformMatrix3fv(InRotationMatrix, 1, false, (GLfloat*) vv->world_matrix._f );
        //glVertexPointer(3, GL_UNSIGNED_BYTE, sizeof(struct Vertex), (GLvoid*)0);

        //_chunk_position[0] = vbo->xoff;
        //_chunk_position[1] = vbo->xoff;

        //glUniform3fv(map_ChunkPosition, 1, (GLfloat*) _chunk_position );

        //printf("%i \n", map_ChunkPosition);

        glUniform3f(map_ChunkPosition, vbo->xoff, vbo->yoff, 0.0f);
        //glUniform3fv(map_NormalArray , 6, (GLfloat*) _normal_array );

        //printf("xoff,yoff= %f %f \n", vbo->xoff, vbo->yoff);



        glVertexAttribPointer(map_Vertex, 3, GL_UNSIGNED_BYTE, GL_FALSE, sizeof(struct Vertex), (GLvoid*)0);         
        glVertexAttribPointer(map_TexCoord, 3, GL_UNSIGNED_BYTE, GL_FALSE, sizeof(struct Vertex), (GLvoid*)4);

        glVertexAttribPointer(map_RGB, 3, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(struct Vertex), (GLvoid*)8);
        //glVertexAttribPointer(map_Normal, 1, GL_UNSIGNED_BYTE, GL_FALSE, sizeof(struct Vertex), (GLvoid*)9);

        glVertexAttribPointer(map_LightMatrix, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(struct Vertex), (GLvoid*)12);
        //glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(struct Vertex), (GLvoid*)24);
        //glVertexAttribPointer(map_TexCoord, 3, GL_FLOAT, GL_FALSE, sizeof(struct Vertex), (GLvoid*)12);


        glDrawArrays(GL_QUADS,0, vbo->_v_num[0]);
    }

    glDisableVertexAttribArray(map_Vertex);
    glDisableVertexAttribArray(map_TexCoord);
    glDisableVertexAttribArray(map_RGB);
    //glDisableVertexAttribArray(map_Normal);

    glDisableVertexAttribArray(map_LightMatrix);

    //glDisableClientState(GL_VERTEX_ARRAY);
    //glDisableClientState(GL_COLOR_ARRAY);

    glUseProgramObjectARB(0);

    //glDisableClientState(GL_TEXTURE_COORD_ARRAY);

    //glDisable(GL_TEXTURE_2D);

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


}



}