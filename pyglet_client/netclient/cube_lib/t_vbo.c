#include "./t_vbo.h"

GLuint texture = 0;
SDL_Surface *surface;

int draw_mode_enabled = 0;

///advice
/*
It turns out that if you disable depth testing (glDisable(GL_DEPTH_TEST)),
GL also disables writes to the depth buffer. The correct solution is to tell GL to ignore the depth test
results with glDepthFunc (glDepthFunc(GL_ALWAYS)). Be careful because in this state, if you render a far away
object last, the depth buffer will contain the values of that far object.
 ---
glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
RenderScene();
SwapBuffers(hdc);  //For Windows
 *
 */

struct Vertex* quad_cache;

int _init_draw_terrain() {
    quad_cache = _get_quad_cache();

    if(texture == 0) { //load texture if texture is not set
    surface=IMG_Load("texture/textures_02.png");
    if(!surface) {printf("IMG_Load: %s \n", IMG_GetError());return 0;}
    glEnable(GL_TEXTURE_2D);
    glGenTextures( 1, &texture );
    glBindTexture( GL_TEXTURE_2D, texture );

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);

    glTexImage2D(GL_TEXTURE_2D, 0, 4, surface->w, surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels );
    glDisable(GL_TEXTURE_2D);
    }
}

int create_vbo(struct VBO* q_VBO, struct Vertex* v_list, int v_num) {
    GLuint VBO_id;
    VBO_id = 0;
    if (v_num == 0) { return 0; }
    glEnable(GL_TEXTURE_2D);
    q_VBO->v_list = malloc(v_num*sizeof(struct Vertex)); ///dont forget to free this!!!
    q_VBO->v_num = v_num;
    memcpy(q_VBO->v_list, v_list, v_num*sizeof(struct Vertex));
    glGenBuffers(1, &VBO_id);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_id);
    glBufferData(GL_ARRAY_BUFFER, v_num*sizeof(struct Vertex), q_VBO->v_list, GL_STATIC_DRAW); // size, pointer to array, usecase
    q_VBO->VBO_id = VBO_id;
    glDisable(GL_TEXTURE_2D);
    printf("inside: VBO_id= %i \n", VBO_id);
    return VBO_id;
}

int delete_vbo(struct VBO* vbo) {
    #ifdef _WIN32
    glDeleteBuffers(1, &vbo->VBO_id);
    #else
    glDeleteBuffers(1, &vbo->VBO_id);
    #endif
    ///free the system memory copy of the vertex buffer
    free(vbo->v_list);
    vbo->VBO_id = 0;
    vbo->v_num = 0;
    return 0;
}

int start_vbo_draw() {

draw_mode_enabled = 1;

glEnable(GL_TEXTURE_2D);
glEnable (GL_DEPTH_TEST);
glEnable(GL_CULL_FACE);  ///testing

glBindTexture( GL_TEXTURE_2D, texture );

glEnableClientState(GL_VERTEX_ARRAY);
glEnableClientState(GL_TEXTURE_COORD_ARRAY);
glEnableClientState(GL_COLOR_ARRAY);

return 0;
}

int end_vbo_draw() {
draw_mode_enabled = 0;
glDisableClientState(GL_VERTEX_ARRAY);
glDisableClientState(GL_COLOR_ARRAY);
glDisableClientState(GL_TEXTURE_COORD_ARRAY);

glDisable(GL_TEXTURE_2D);
glDisable (GL_DEPTH_TEST);
glDisable(GL_CULL_FACE);

return 0;
}

//assums vbo is type quad
int draw_quad_vbo(struct VBO* q_VBO) {

//printf("wtf\n");
//printf('vnum= %f \n', (float) (q_VBO->v_num));


if(draw_mode_enabled == 0) {
    glBindBuffer(GL_ARRAY_BUFFER, q_VBO->VBO_id);
    start_vbo_draw();

    glVertexPointer(3, GL_FLOAT, sizeof(struct Vertex), 0);
    glTexCoordPointer(2, GL_FLOAT, sizeof(struct Vertex), 12);
    glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(struct Vertex), 20);

    glDrawArrays(GL_QUADS,0, q_VBO->v_num);
    return 0;
} else {
    glBindBuffer(GL_ARRAY_BUFFER, q_VBO->VBO_id);

    glVertexPointer(3, GL_FLOAT, sizeof(struct Vertex), 0);
    glTexCoordPointer(2, GL_FLOAT, sizeof(struct Vertex), 12);
    glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(struct Vertex), 20);

    glDrawArrays(GL_QUADS,0, q_VBO->v_num);
    return 0;
}

}

int print_vertex(struct Vertex* v) {
printf("x,y,z= %f, %f, %f\n", v->x,v->y,v->z);
printf("tx,ty= %f, %f\n", v->tx, v->ty);
printf("r,g,b,a= %i, %i, %i, %i\n", v->r,v->g,v->b,v->a);
printf("\n");
return 0;
}

int _test3(int x, int y, int z) {
    return  _get(x,y,z);
}


/// start VBO

//buffers for VBO stuff
struct Vertex cs[(128*8*8)*4*6]; //chunk scratch
unsigned int cs_n; //number of vertices in chunk scratch

void inline add_quad(float x,float y,float z,int side, int tile_id) {
    int i;
    //struct Vertex* v;
    memcpy(&cs[cs_n], &quad_cache[tile_id*6*4+4*side], 4*sizeof(struct Vertex)); //id*6*4+4*side+vert_num
    for(i=0; i<=4;i++) {
        cs[cs_n+i].x += x;
        cs[cs_n+i].y += y;
        cs[cs_n+i].z += z;
    //v = &cs[cs_n+i];
    }
    cs_n += 4;
}

/*
/// ADDRESS
def draw_chunks():
    cdef MapChunk mc
    ll = terrain_map.get_raw_chunk_list()
    #print "Draw"
    #cube_lib.terrain_map.set(5,5,0,1)
    #for z in range (-16, 16):
    #    if cube_lib.terrain_map.get(5,5,z) != 0:
    #        print "non zero!"

    for l in ll:
        mc = <MapChunk>l
        if mc.VBO.VBO_id != 0:
            _draw_vbo(&mc.VBO)
            #print "VBO_id, v_num= %i, %i" % (mc.VBO.VBO_id, mc.VBO.v_num)
    _end_vbo_draw()
/// ADDRESS
*/

int update_column_VBO(struct vm_column* column) {
    printf("update_column_VBO \n");
    int tile_id, side_num;
    int _x, _y, _z;

    struct vm_chunk* chunk;
    int i;
    column->vbo_needs_update = 0;
    column->vbo_loaded = 1;
    cs_n = 0; //clear chunk scratch
    if(column->vbo.VBO_id != 0) {
        delete_vbo(&column->vbo);
    }
    for(i = 0; i < vm_column_max; i++) {
        if(column->chunk[i] == NULL) { continue; }
        //printf("chunk= %i\n", i);
        chunk = column->chunk[i];
        chunk->vbo_needs_update = 0;
        //printf("1,2,3 = %i, %i, %i \n", 8*chunk->x_off, 8*chunk->y_off, 8*chunk->z_off);
        for(_x = 8*chunk->x_off; _x < 8*chunk->x_off +8 ; _x++) {
            for(_y = 8*chunk->y_off; _y < 8*chunk->y_off +8 ; _y++) {
                for(_z = 8*chunk->z_off; _z < 8*chunk->z_off +8 ; _z++) {
                    tile_id = _get(_x,_y,_z);
                    //printf("test %i, %i, %i tile= %i\n", _x,_y,_z,tile_id );
                    if(_isActive(tile_id) == 0) {continue;} else {
                        //printf("add %i, %i, %i tile_id = %i \n", _x,_y,_z,tile_id);
                    }
                    for(side_num=0; side_num<6; side_num++) {
                        //#if not _is_occluded(x_+mc.index[0],y_+mc.index[1],z_+mc.index[2],side_num): #ints
                        add_quad(_x,_y,_z,side_num,tile_id);
                    }
        }
            }
                }
    }

    printf("v_num for chunk scratch = %i \n", cs_n);
    //printf("active cubes= %i \n", active_cube_num);
    create_vbo(&column->vbo, cs, cs_n);
    printf("VBO_id= %i \n", column->vbo.VBO_id);
 }


int _update_chunks() {
    //printf("_update_chunks \n");
    struct vm_map* m;
    int i,j;
    m = _get_map();
    for(i=0; i<vm_map_dim; i++) {
    for(j=0; j<vm_map_dim;j++) {
        if(m->column[j*vm_map_dim+i].vbo_needs_update) {
            update_column_VBO(&m->column[j*vm_map_dim+i]);
        }
    }}
}

int _draw_terrain() {
    struct vm_map* m;
    int i,j;
    start_vbo_draw();
    m = _get_map();
    //printf("Start Map Draw\n");
    for(i=0; i<vm_map_dim; i++) {
        for(j=0; j<vm_map_dim;j++) {
            if( m->column[j*vm_map_dim+i].vbo.v_num > 0) {
                //printf("Drawing chunk: %i, %i \n", i, j);
                draw_quad_vbo(&m->column[j*vm_map_dim+i].vbo);
            }
        }
    }
    end_vbo_draw();
}

int s_array[18] = {
            0,0,1,
            0,0,-1,
            0,1,0,
            0,-1,0,
            -1,0,0,
            1,0,0
            };

/*
int inline _is_occluded(int x,int y,int z, int side_num) {
    int i;
    i = s_array[3*side_num];
    x += s_array[i+0];
    y += s_array[i+1];
    z += s_array[i+2];
    return isOccluded(_get(x,y,z));
}
*/
