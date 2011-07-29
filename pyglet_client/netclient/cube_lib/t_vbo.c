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

int _init_draw_terrain() {
    if(texture == 0) { //load texture if texture is not set
    surface=IMG_Load("texture/textures_01.png");
    if(!surface) {printf("IMG_Load: %s \n", IMG_GetError());return 0;}
    glEnable(GL_TEXTURE_2D);
    glGenTextures( 1, &texture );
    glBindTexture( GL_TEXTURE_2D, texture );

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, 4, surface->w, surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels );
    glDisable(GL_TEXTURE_2D);
    }
}

int _create_vbo(struct VBO* q_VBO, struct Vertex* v_list, int v_num) {
    GLuint VBO_id;
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
    return VBO_id;
}

int _delete_vbo(struct VBO* q_VBO) {
    #ifdef _WIN32
    glDeleteBuffers(1, &q_VBO->VBO_id);
    #else
    glDeleteBuffers(q_VBO->VBO_id);
    #endif
    ///free the system memory copy of the vertex buffer
    free(q_VBO->v_list);
    vbo->VBO_id = 0;
    q_VBO->v_num = 0;
    return 0;
}

int _start_vbo_draw() {
draw_mode_enabled = 1;

glEnable(GL_TEXTURE_2D);
glEnable (GL_DEPTH_TEST);
//glEnable(GL_CULL_FACE);

glBindTexture( GL_TEXTURE_2D, texture ); //needed?

glEnableClientState(GL_VERTEX_ARRAY);
glEnableClientState(GL_TEXTURE_COORD_ARRAY);
glEnableClientState(GL_COLOR_ARRAY);

return 0;
}

int _end_vbo_draw() {
draw_mode_enabled = 0;
glDisableClientState(GL_VERTEX_ARRAY);
glDisableClientState(GL_COLOR_ARRAY);
glDisableClientState(GL_TEXTURE_COORD_ARRAY);

glDisable(GL_TEXTURE_2D);
glDisable (GL_DEPTH_TEST);
//glDisable(GL_CULL_FACE);

return 0;
}

//assums vbo is type quad
int _draw_quad_vbo(struct VBO* q_VBO) {

glBindBuffer(GL_ARRAY_BUFFER, q_VBO->VBO_id);

if(draw_mode_enabled == 0) {
    glBindBuffer(GL_ARRAY_BUFFER, q_VBO->VBO_id);
    _start_vbo_draw();

    glVertexPointer(3, GL_FLOAT, sizeof(struct Vertex), 0);
    glTexCoordPointer(2, GL_FLOAT, sizeof(struct Vertex), 12);
    glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(struct Vertex), 20);

    glDrawArrays(GL_QUADS,0, q_VBO->v_num*4);
    return 0;
} else {
    glBindBuffer(GL_ARRAY_BUFFER, q_VBO->VBO_id);

    glVertexPointer(3, GL_FLOAT, sizeof(struct Vertex), 0);
    glTexCoordPointer(2, GL_FLOAT, sizeof(struct Vertex), 12);
    glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(struct Vertex), 20);

    glDrawArrays(GL_QUADS,0, q_VBO->v_num);
}

return 0;
}

int print_vertex(struct Vertex* v) {
printf("x,y,z= %f, %f, %f\n", v->x,v->y,v->z);
printf("tx,ty= %f, %f\n", v->tx, v->ty);
printf("r,g,b,a= %i, %i, %i, %i\n", v->r,v->g,v->b,v->a);
printf("\n");
return 0;
}
///test

int _test3(int x, int y, int z) {
    return  _get(x,y,z);
}
///test

/// start VBO

//buffers for VBO stuff
struct Vertex cs[(128*8*8)*4*6]; //chunk scratch
unsigned int n cs_n; //number of vertices in chunk scratch

float v_index[72] = {
         0,1,1 , 0,0,1 , 1,0,1 , 1,1,1 , #top
         1,0,0 , 0,0,0 , 0,1,0 , 1,1,0 , #bottom
         0,1,1 , 1,1,1 , 1,1,0 , 0,1,0 , #north
         0,0,1 , 0,0,0 , 1,0,0 , 1,0,1 , #south
         0,1,1 , 0,1,0 , 0,0,0 , 0,0,1 , #west
         1,0,1 , 1,0,0 , 1,1,0 , 1,1,1 , #east
    }

void inline set_side(float x, float y, float z, int side_num, int tile_id, Quad* quad):
    global quad_cache
    cdef int i
    #cdef Vertex* vertex
    #cdef Quad* quad2 = &quad_cache[6*tile_id + side_num]
    memcpy(quad, &quad_cache[6*tile_id + side_num], sizeof(Quad))
    for i in range(0,4):
        #print "(tx= %f,ty= %f)" %(quad_cache[6*tile_id + side_num].vertex[i].tx,quad_cache[6*tile_id + side_num].vertex[i].ty) + " tx,ty= %f, %f" %(quad.vertex[i].tx, quad.vertex[i].ty)
        quad.vertex[i].x += x
        quad.vertex[i].y += y
        quad.vertex[i].z += z
        #time.sleep(1.)


void inline add_quad(float x,float y,float z,int side,int tile) {
    cdef Quad* quad = &chunk_scratch.quad[chunk_scratch.v_num]
    chunk_scratch.v_num += 1 #quads have 4 vertex
    set_side(x,y,z, side, tile, quad)
}

void inline clear_chunk_scratch(){
    cs_n = 0;
}

void update_chunks() {
    return; //do something
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

void update_column_VBO(struct vm_column* column) {
    int tile_id, x, y, z, side_num;
    float x_off, y_off, z_off;
    int x_, y_, z_;
    int active_cube_num;

    vm_chunk* chunk;
    VBO* vbo;
    int i;
    vbo = column->VBO;
    column->vbo_needs_update = 0;
    column->vbo_loaded = 1;
    clear_chunk_scratch();
    if(vbo->VBO_id != 0) {
        delete_VBO(vbo);
    }
    for(i = 0; i < vm_column_max; i++) {
        if(column->chunk[i] == NULL) { continue; }
        chunk = column->chunk[i];
        chunk->vbo_needs_update = 0;

    printf("1,2,3 = %f, %f, %f \n", 8*chunk->x_off, 8*chunk->y_off, 8*chunk->z_off);
    clear_chunk_scratch()

    active_cube_num = 0
    //#print "ysa= %i" % (x_chunk_size)
    for x_ in range(0, x_chunk_size):
        for y_ in range(0, y_chunk_size):
            for z_ in range(0, z_chunk_size):
                tile_id = cube_lib.terrain_map.get(x_+mc.index[0],y_+mc.index[1],z_+mc.index[2])
                #if tile_id != 0:
                #    print "tile, active= %i, %i" %(tile_id, isActive(tile_id))
                if isActive(tile_id) != 0: #non-active tiles are not draw
                    active_cube_num += 1 #comment out
                    for side_num in [0,1,2,3,4,5]: #[1]:#
                        #if not _is_occluded(x_+mc.index[0],y_+mc.index[1],z_+mc.index[2],side_num): #ints
                        if True:
                            add_quad(x_+x_off,y_+y_off,z_+z_off,side_num,tile_id) #floats

    printf("v_num for chunk scratch = %i \n", chunk_scratch.v_num);
    printf("active cubes= %i \n", active_cube_num);
    _create_vbo(&mc.VBO, chunk_scratch.quad, chunk_scratch.v_num)
    printf("VBO_id= %i \n", vbo.VBO_id);
 }
}

void delete_VBO(VBO* vbo) {

    _delete_vbo(vbo);
    }

int s_array[18] = {
            0,0,1,
            0,0,-1,
            0,1,0,
            0,-1,0,
            -1,0,0,
            1,0,0
            };

int inline _is_occluded(int x,int y,int z, int side_num) {
    int i;
    i = s_array[3*side_num];
    x += s_array[i+0];
    y += s_array[i+1];
    z += s_array[i+2];
    return isOcclude(_get(x,y,z));
}
