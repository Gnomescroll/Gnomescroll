#include "./t_vbo.h"

//#include "./t_inline.c"

GLuint texture = 0;

SDL_Surface *surface;

int draw_mode_enabled = 0;

unsigned int _garbage_collection_counter = 0;
#define garbage_collection_frequency 10

int T_MAP_BLEND_MODE = 0;

void _toggle_terrain_map_blend_mode() {
    T_MAP_BLEND_MODE = (T_MAP_BLEND_MODE+1)%7;
    printf("toggle_terrain_map_blend_mode: blend mode= %i\n", T_MAP_BLEND_MODE);
}

int T_MAP_Z_BUFFER = 0;
void _toggle_z_buffer() {
    T_MAP_Z_BUFFER = (T_MAP_Z_BUFFER +1) %2;
    if(T_MAP_Z_BUFFER==0) printf("z_buffer off\n");
    if(T_MAP_Z_BUFFER==1) printf("z_buffer on\n");

}


//fulstrum culling globals
struct _Camera {
float x,y,z;
float vx,vy,vz;
float ux,uy,uz; //up direction
float ratio;
float viewangle;
};

int fulstrum_culling;
int view_distance = 40;
struct _Camera c;


/*
glDepthMask — enable or disable writing into the depth buffer

void glDepthMask(   GLboolean   flag);

Specifies whether the depth buffer is enabled for writing. If flag is GL_FALSE, depth buffer writing is disabled. Otherwise, it is enabled. Initially, depth buffer writing is enabled.
*/


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

/*
 OpenGL has a function that can prevent pixels with a specified set of alpha values from writing the colour or Z buffers. For example:


     glAlphaFunc ( GL_GREATER, 0.1 ) ;
     glEnable ( GL_ALPHA_TEST ) ;
*/

//struct Vertex* quad_cache;
struct Vertex quad_cache[max_cubes*6*4];

struct Vertex* _get_quad_cache() {
    return (struct Vertex*) &quad_cache;
}

/*
#north/south is +/- x
#west/east is +/- y
l = [
        1,1,1 , 0,1,1 , 0,0,1 , 1,0,1 , #top
        0,1,0 , 1,1,0 , 1,0,0 , 0,0,0 , #bottom
        1,0,1 , 1,0,0 , 1,1,0 , 1,1,1 , #north
        0,1,1 , 0,1,0 , 0,0,0 , 0,0,1 , #south
        1,1,1 , 1,1,0 , 0,1,0,  0,1,1 , #west
        0,0,1 , 0,0,0 , 1,0,0 , 1,0,1 , #east
]
*/

const int _lighting = 0;


GLfloat mat_specular[] = {1.0, 1.0, 1.0, 1.0};
GLfloat mat_diffuse[] = {1.0, 0.0, 0.0};
GLfloat mat_shininess[] = { 50.0 };
GLfloat light_position[] = {256.0, 256.0, 128.0, 0.0};
GLfloat light_color[] = {1.0, 1.0, 1.0, 0.0};
GLfloat diffuse_color[] =  {1.0, 1.0, 1.0, 0.0};
//GLfloat lmodel_ambient[] = {0.8, 0.8, 0.8, 0.0};
GLfloat lmodel_ambient[] = {0.8, 0.8, 0.8, 0.8};

int _init_quad_cache_normals() {
    //int i,j;
    int cube_id, side, i;

    int n[3];
    for(cube_id=0;cube_id<max_cubes;cube_id++) {
        for(side=0;side<6;side++) {

            if(side == 0) {
                n[0]= 0;
                n[1]= 0;
                n[2]= 1;
            }
            if(side == 1) {
                n[0]= 0;
                n[1]= 0;
                n[2]= -1;
            }
            if(side == 2) {
                n[0]= 1;
                n[1]= 0;
                n[2]= 0;
            }

            if(side == 3) {
                n[0]= -1;
                n[1]= 0;
                n[2]= 0;
            }
            if(side == 4) {
                n[0]= 0;
                n[1]= 1;
                n[2]= 0;
            }
            if(side == 5) {
                n[0]= 0;
                n[1]= -1;
                n[2]= 0;
            }
        /*
            if(side == 1) n = {0,0,-1};
            if(side == 2) n = {1,0,0};
            if(side == 3) n = {-1,0,0};
            if(side == 4) n = {0,1,0};
            if(side == 5) n = {0,-1,0};
        */       
            for(i=0;i<4;i++) {
                quad_cache[cube_id*6*4+ 6*side+ i].normal[0] = n[0];
                quad_cache[cube_id*6*4+ 6*side+ i].normal[1] = n[1];
                quad_cache[cube_id*6*4+ 6*side+ i].normal[2] = n[2];
                quad_cache[cube_id*6*4+ 6*side+ i].normal[3] = 0;
            }
        }
    }

}

int _init_draw_terrain() {
    //quad_cache = _get_quad_cache();
    _init_quad_cache_normals();

    //glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
    //glEnable(GL_POLYGON_SMOOTH);
    printf("Terrain map: vertex size is %i bytes \n", sizeof(struct Vertex));

    if(texture == 0) { //load texture if texture is not set
    //surface=IMG_Load("media/texture/textures_03.png");  //should this be freed?
    surface=IMG_Load("media/texture/blocks_01.png");
    if(!surface) {printf("IMG_Load: %s \n", IMG_GetError());return 0;}

    //SDL_Surface *_surface = IMG_Load("media/texture/textures_03.png");
    //if(!_surface) {printf("IMG_Load: %s \n", IMG_GetError());return 0;}

    //SDL_FreeSurface(_surface);

    glEnable(GL_TEXTURE_2D);
    glGenTextures( 1, &texture );
    glBindTexture( GL_TEXTURE_2D, texture );

/*
 * Replace with tiles and use mipmapping for filter
*/
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);

    //Uint32 GL_PIXEL_TYPE = GL_BGR;
    int texture_format;
    if (surface->format->Rmask == 0x000000ff) texture_format = GL_RGBA;
    else texture_format = GL_BGRA;

    glTexImage2D(GL_TEXTURE_2D, 0, 4, surface->w, surface->h, 0, texture_format, GL_UNSIGNED_BYTE, surface->pixels );
    //glTexImage2D(GL_TEXTURE_2D, 0, 4, surface->w, surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels );
    //glTexImage2D(GL_TEXTURE_2D, 0, 4, surface->w, surface->h, 0, GL_BGRA, GL_UNSIGNED_BYTE, surface->pixels );
    glDisable(GL_TEXTURE_2D);
    }
    return 0;
}

int create_vbo(struct VBO* q_VBO, struct Vertex* v_list, int v_num) {
    GLuint VBO_id;
    VBO_id = 0;
    if (v_num == 0) { return 0; }
    glEnable(GL_TEXTURE_2D);
    q_VBO->v_list = (struct Vertex* )malloc(v_num*sizeof(struct Vertex)); ///dont forget to free this!!!
    q_VBO->v_num = v_num;
    memcpy(q_VBO->v_list, v_list, v_num*sizeof(struct Vertex));
    glGenBuffers(1, &VBO_id);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_id);
    glBufferData(GL_ARRAY_BUFFER, v_num*sizeof(struct Vertex), q_VBO->v_list, GL_STATIC_DRAW); // size, pointer to array, usecase
    q_VBO->VBO_id = VBO_id;
    glDisable(GL_TEXTURE_2D);
    //printf("inside: VBO_id= %i \n", VBO_id);
    return VBO_id;
}

int delete_vbo(struct VBO* vbo) {

    glDeleteBuffers(1, (GLuint*)&vbo->VBO_id);
    ///free the system memory copy of the vertex buffer
    free(vbo->v_list);
    vbo->VBO_id = 0;
    vbo->v_num = 0;
    return 0;
}

int start_vbo_draw() {

//glColor4bu(255,255,255,255);
glColor3b(255,255,255);

draw_mode_enabled = 1;

glEnable(GL_TEXTURE_2D);
glEnable (GL_DEPTH_TEST);

glShadeModel(GL_SMOOTH);

if(_lighting) {
    
    glMaterialfv(GL_FRONT,GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse);

    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse_color);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_color);
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHTING);
    glEnable(GL_COLOR_MATERIAL);
}

if(T_MAP_Z_BUFFER == 0) {
    glEnable (GL_DEPTH_TEST);
} else {
    glDisable (GL_DEPTH_TEST);
}

if(T_MAP_BLEND_MODE != 2) { //make a toggle for cull face
    glEnable(GL_CULL_FACE);  ///testing
}

if(T_MAP_BLEND_MODE != 0) {
    glEnable(GL_BLEND);
    if(T_MAP_BLEND_MODE ==1 ) glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    if(T_MAP_BLEND_MODE ==2 ) glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    if(T_MAP_BLEND_MODE ==3 ) glBlendFunc(GL_ONE, GL_ONE);
    if(T_MAP_BLEND_MODE ==4 ) glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    if(T_MAP_BLEND_MODE ==5 ) glBlendFunc(GL_ONE_MINUS_SRC_COLOR, GL_ONE_MINUS_DST_COLOR);
    if(T_MAP_BLEND_MODE ==6 ) glBlendFunc(GL_ONE, GL_ALPHA);
}

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

if(_lighting) {

    glDisable(GL_LIGHT0);
    glDisable(GL_LIGHTING);
    glDisable(GL_COLOR_MATERIAL);
}

glShadeModel(GL_FLAT);

glDisable(GL_TEXTURE_2D);
glDisable (GL_DEPTH_TEST);
glDisable(GL_CULL_FACE);

if(T_MAP_BLEND_MODE != 0) glDisable(GL_BLEND);
return 0;
}

//assums vbo is type quad

int draw_quad_vbo(struct VBO* q_VBO) {

//printf("wtf\n");
//printf('vnum= %f \n', (float) (q_VBO->v_num));


if(draw_mode_enabled == 0) {
    glBindBuffer(GL_ARRAY_BUFFER, q_VBO->VBO_id);
    start_vbo_draw();

    glVertexPointer(3, GL_FLOAT, sizeof(struct Vertex), (GLvoid*)0);
    glTexCoordPointer(2, GL_FLOAT, sizeof(struct Vertex), (GLvoid*)12);
    glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(struct Vertex), (GLvoid*)20);
    glNormalPointer(GL_BYTE, sizeof(struct Vertex), (GLvoid*)24);

    glDrawArrays(GL_QUADS,0, q_VBO->v_num);
    //return 0;
} else {
    glBindBuffer(GL_ARRAY_BUFFER, q_VBO->VBO_id);

    glVertexPointer(3, GL_FLOAT, sizeof(struct Vertex), (GLvoid*)0);
    glTexCoordPointer(2, GL_FLOAT, sizeof(struct Vertex), (GLvoid*)12);
    glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(struct Vertex), (GLvoid*)20);
    glNormalPointer(GL_BYTE, sizeof(struct Vertex), (GLvoid*)24);

    glDrawArrays(GL_QUADS,0, q_VBO->v_num);
    //return 0;
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

/// start VBO

//buffers for VBO stuff
struct Vertex cs[(128*8*8)*4*6]; //chunk scratch
unsigned int cs_n; //number of vertices in chunk scratch

//chunk scratch for transparency

static int vertex_count;
int transparent_cache[8192];    //8192, 16384/2
int transparent_cache_n;


/*
l = [
        1,1,1 , 0,1,1 , 0,0,1 , 1,0,1 , #top
        0,1,0 , 1,1,0 , 1,0,0 , 0,0,0 , #bottom
        1,0,1 , 1,0,0 , 1,1,0 , 1,1,1 , #north
        0,1,1 , 0,1,0 , 0,0,0 , 0,0,1 , #south
        1,1,1 , 1,1,0 , 0,1,0,  0,1,1 , #west
        0,0,1 , 0,0,0 , 1,0,0 , 1,0,1 , #east
]
*/
//int CI[9];


static const int CI[6*8*3] = {1, 1, 1, 0, 1, 1, -1, 1, 1, -1, 0, 1, -1, -1, 1, 0, -1, 1, 1, -1, 1, 1, 0, 1,
-1, 1, -1, 0, 1, -1, 1, 1, -1, 1, 0, -1, 1, -1, -1, 0, -1, -1, -1, -1, -1, -1, 0, -1,
1, -1, 1, 1, -1, 0, 1, -1, -1, 1, 0, -1, 1, 1, -1, 1, 1, 0, 1, 1, 1, 1, 0, 1,
-1, 1, 1, -1, 1, 0, -1, 1, -1, -1, 0, -1, -1, -1, -1, -1, -1, 0, -1, -1, 1, -1, 0, 1,
1, 1, 1, 1, 1, 0, 1, 1, -1, 0, 1, -1, -1, 1, -1, -1, 1, 0, -1, 1, 1, 0, 1, 1,
-1, -1, 1, -1, -1, 0, -1, -1, -1, 0, -1, -1, 1, -1, -1, 1, -1, 0, 1, -1, 1, 0, -1, 1 };


/*
static const int CI0[8*3] = { 1,1,1, 0,1,1, -1,1,1, -1,0,1, -1,-1,1, 0,-1,1, 1,-1,1, 1,0,1};
static const int CI1[8*3] = { -1,1,-1, 0,1,-1, 1,1,-1, 1,0,-1, 1,-1,-1, 0,-1,-1, -1,-1,-1, -1,0,-1,};
static const int CI2[8*3] = { 0,-1,2, 0, -1, 1, 0, -1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 1, 2, 0, 0, 2,};
static const int CI3[8*3] = { 0,1,2, 0, 1, 1, 0, 1, 0, 0, 0, 0, 0, -1, 0, 0, -1, 1, 0, -1, 2, 0, 0, 2,};
static const int CI4[8*3] = { 1,0,2, 1, 0, 1, 1, 0, 0, 0, 0, 0, -1, 0, 0, -1, 0, 1, -1, 0, 2, 0, 0, 2,};
static const int CI5[8*3] = { -1,0,2, -1, 0, 1, -1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 1, 1, 0, 2, 0, 0, 2};
*/

/*
will be 1 if is adjacent to any side
will be 2 only if both sides are occluded
 */

inline int calcAdj(int side_1, int side_2, int corner)
{
    int occ = (side_1 | side_2 | corner) + (side_1 & side_2);
    if( occ == 0) return 255;
    if(occ == 1) return 177;
    //if(occ == 2) return 177;  
    if(occ == 2) return 100;    
}


void __inline add_quad(int x, int y, int z, int side, int tile_id) {
    int i;
    //struct Vertex* v;
    memcpy(&cs[cs_n], &quad_cache[tile_id*6*4+4*side], 4*sizeof(struct Vertex)); //id*6*4+4*side+vert_num

    int index;
    int CX[8];
    for(i=0; i<8; i++) {
        index = side*8*3+i*3;
        CX[i] = isActive(_get(x+CI[index+0],y+CI[index+1],z+CI[index+2]));
    }

    float _x = x;
    float _y = y;
    float _z = z;
    for(i=0; i<=4;i++) {
        cs[cs_n+i].x += _x;
        cs[cs_n+i].y += _y;
        cs[cs_n+i].z += _z;
    }

    int occ = (x+y+z);
    if(occ > 255) occ = 255;

    occ = calcAdj(CX[7], CX[1], CX[0]);
    cs[cs_n+0].r = occ;
    cs[cs_n+0].g = occ;
    cs[cs_n+0].b = occ;

    occ = calcAdj(CX[1], CX[3], CX[2]);
    cs[cs_n+1].r = occ;
    cs[cs_n+1].g = occ;
    cs[cs_n+1].b = occ;

    occ = calcAdj(CX[3], CX[5], CX[4]);
    cs[cs_n+2].r = occ;
    cs[cs_n+2].g = occ;
    cs[cs_n+2].b = occ;

    occ = calcAdj(CX[5], CX[7], CX[6]);
    cs[cs_n+3].r = occ;
    cs[cs_n+3].g = occ;
    cs[cs_n+3].b = occ;

    cs_n += 4;
}

int inline _is_occluded(int x,int y,int z, int side_num) {
    const static int s_array[18] = {
            0,0,1,  //top
            0,0,-1, //bottom
            1,0,0,  //north
            -1,0,0, //south
            0,1,0,  //west
            0,-1,0, //east
            };

    int i;
    i = 3*side_num;
    x += s_array[i+0];
    y += s_array[i+1];
    z += s_array[i+2];
    //printf("%i, %i, %i \n", s_array[i+0], s_array[i+1], s_array[i+2]);
    //return isOccluded(_get(x,y,z));
    return isActive(_get(x,y,z));
}

const static int s_array[18] = {
        0,0,1,  //top
        0,0,-1, //bottom
        1,0,0,  //north
        -1,0,0, //south
        0,1,0,  //west
        0,-1,0, //east
        };

int inline _is_occluded_transparent(int x,int y,int z, int side_num, int _tile_id) {
    int i;
    i = 3*side_num;
    x += s_array[i+0];
    y += s_array[i+1];
    z += s_array[i+2];
    //printf("%i, %i, %i \n", s_array[i+0], s_array[i+1], s_array[i+2]);
    //return isOccluded(_get(x,y,z));

    int tile_id = _get(x,y,z);
    if(tile_id == _tile_id) return 1;
    return isActive(tile_id);
}

int update_column_VBO(struct vm_column* column) {
    int tile_id, side_num;
    int _x, _y, _z;

    struct vm_chunk* chunk;
    int i;
    int transparency;
    cs_n = 0; //clear chunk scratch
    vertex_count = 0; //clear counter
    if(column->vbo.VBO_id != 0) {
        delete_vbo(&column->vbo);
    }
    for(i = 0; i < vm_column_max; i++) {
        if(column->chunk[i] == NULL) { continue; }
        //printf("chunk= %i\n", i);
        chunk = column->chunk[i];
        //chunk->vbo_needs_update = 0;
        //printf("1,2,3 = %i, %i, %i \n", 8*chunk->x_off, 8*chunk->y_off, 8*chunk->z_off);
        for(_x = 8*chunk->x_off; _x < 8*chunk->x_off +8 ; _x++) {
            for(_y = 8*chunk->y_off; _y < 8*chunk->y_off +8 ; _y++) {
                for(_z = 8*chunk->z_off; _z < 8*chunk->z_off +8 ; _z++) {
                    tile_id = _get(_x,_y,_z);
                    //printf("test %i, %i, %i tile= %i\n", _x,_y,_z,tile_id );
                    if(isActive(tile_id) == 0) {continue;} else {
                        if(isOccludes(tile_id) == 1) {
                            for(side_num=0; side_num<6; side_num++) {
                                if(! _is_occluded(_x,_y,_z,side_num)) {
                                    add_quad(_x,_y,_z,side_num,tile_id);
                                }
                            }
                        } 
                        else
                        {
                            //active block that does not occlude
                            transparency = isTransparent(tile_id);
                            for(side_num=0; side_num<6; side_num++) 
                            {
                                if(! _is_occluded_transparent(_x,_y,_z,side_num, tile_id)) 
                                {
                                    add_quad(_x,_y,_z,side_num,tile_id);
                                }
                            }
                        }
                    }    
    }
            }
                }
    }
    if(cs_n == 0 ) {
        column->vbo.VBO_id = 0;
        column->vbo.v_num = 0;
        //column->vbo_loaded = 0;
        //column->vbo_needs_update = 0;
        //column->empty = 1;
        set_flag(column, VBO_loaded, 0);
        set_flag(column, VBO_needs_update, 0);
        set_flag(column, VBO_has_blocks, 0);
        return 0;
    } else {
        set_flag(column, VBO_loaded, 1);
        set_flag(column, VBO_needs_update, 0);
        set_flag(column, VBO_has_blocks, 1);

        create_vbo(&column->vbo, cs, cs_n);
        //column->vbo_needs_update = 0;
        //column->vbo_loaded = 1;
    }
    //printf("vbo_id= %i v_num= %i \n", column->vbo.VBO_id,column->vbo.v_num);

     return 0;
}


int _update_chunks() {
    //printf("_update_chunks \n");
    struct vm_map* m;
    struct vm_column* c;
    int i,j;
    m = _get_map();
    //garbage collection

    _garbage_collection_counter++;
    if(_garbage_collection_counter >= garbage_collection_frequency) {
        _garbage_collection_counter = 0;
        for(i=0; i<vm_map_dim; i++) {
        for(j=0; j<vm_map_dim;j++) {
            c = &m->column[j*vm_map_dim+i];
            //if VBO is farther than 10 viewing units, delete it
            if (flag_is_true(c, VBO_loaded) && !chunk_render_check(c->x_off, c->y_off, 15)) {
                //printf("unloaded VBO: %i, %i \n", c->x_off, c->y_off);
                delete_vbo(&c->vbo);
                set_flag(c, VBO_loaded, 0);
            }
        }}
    }

    //VBO construction loop

    for(i=0; i<vm_map_dim; i++) {
    for(j=0; j<vm_map_dim;j++) {
        c = &m->column[j*vm_map_dim+i];
        //update or create VBO for chunks within 10 units of viewing distance
        if(chunk_render_check(c->x_off, c->y_off, 10)) {
            //if(c->vbo_needs_update == 1 || (c->vbo_loaded==0 && c->vbo_needs_update)) {

        if(flag_is_true(c, VBO_has_blocks) && ( flag_is_true(c, VBO_needs_update) || flag_is_false(c, VBO_loaded))) {
                if(c->vbo.VBO_id == 0) {
                    //printf("create VBO: %i, %i \n", c->x_off, c->y_off);
                } else {
                    //printf("update VBO: %i, %i \n", c->x_off, c->y_off);
                }
                update_column_VBO(c);
                return 0;
            }
        }


    }}
        return 0;

}

GLuint filter = 0; ;                                // Which Filter To Use
GLuint fogMode[]= { GL_EXP, GL_EXP2, GL_LINEAR };   // Storage For Three Types Of Fog
GLuint fogfilter= 2;                    // Which Fog To Use
GLfloat fogColor[4]= {0.5f, 0.5f, 0.5f, 1.0f};      // Fog Color

int _draw_fog() {

glClearColor(0.5f,0.5f,0.5f,1.0f);          // We'll Clear To The Color Of The Fog ( Modified )

glFogi(GL_FOG_MODE, fogMode[fogfilter]);        // Fog Mode
glFogfv(GL_FOG_COLOR, fogColor);            // Set Fog Color
glFogf(GL_FOG_DENSITY, 0.35f);              // How Dense Will The Fog Be
glHint(GL_FOG_HINT, GL_DONT_CARE);          // Fog Hint Value
glFogf(GL_FOG_START, 1.0f);             // Fog Start Depth
glFogf(GL_FOG_END, 5.0f);               // Fog End Depth
glEnable(GL_FOG);                   // Enables GL_FOG

    return 0;

}
/*
int _t_ = 0;
int _c_ = 0;
*/

int _draw_terrain() {
/*
    _c_++;
    if(_c_ % 120 == 0) {
        _t_ ++;
        if(_t_ %2 == 0) {
            printf("multisample enabled\n");
            glEnable(GL_MULTISAMPLE);
        } else {
            printf("multisample disabled\n");
            glDisable(GL_MULTISAMPLE);
        }
    }
*/
    //int s,f;
    //s= SDL_GetTicks();
    struct vm_map* m;
    struct vm_column* col;
    int i,j;
    int c_drawn, c_pruned;
    c_drawn=0; c_pruned=0;
    start_vbo_draw();
    m = _get_map();
    //printf("Start Map Draw\n");
    for(i=0; i<vm_map_dim; i++) {
    for(j=0; j<vm_map_dim; j++) {
        col = &m->column[j*vm_map_dim+i];
        if(flag_is_true(col, VBO_loaded) && chunk_render_check(col->x_off, col->y_off, 0)) {
            c_drawn++;
            set_flag(col,VBO_drawn,1);
            draw_quad_vbo(&col->vbo);
        } else {
            set_flag(col,VBO_drawn,0);
            c_pruned++;
        }
    }}
    end_vbo_draw();
    //printf("drawn chunks= %i, pruned chunks= %i \n", c_drawn, c_pruned);
    //_draw_fog();
    //f = SDL_GetTicks();
    //printf("Terrain rendering time= %i \n", f-s);
    return 0;

}

int __inline chunk_render_check(int x_off, int y_off, int tolerance) {
float x,y;
x = 8*x_off + 4;
y = 8*y_off + 4;

//printf("c.x= %f, c.y= %f, x_off= %i, y_off= %i \n", c.x,c.y,x_off, y_off);
if((c.x-x)*(c.x-x) + (c.y-y)*(c.y-y) < (tolerance+view_distance)*(tolerance+view_distance))
    {
        return 1;
    }
//else
    //{
        return 0;
    //}
}

int _set_camera(float x, float y, float z, float vx, float vy, float vz, float ux, float uy, float uz, float ratio, float viewangle) {
c.x = x; c.y = y; c.z = z;
c.ux=ux; c.uy=uy; c.uz=uz;
c.vx=vx; c.vy=vy; c.vz=vz;
c.ratio = ratio;
c.viewangle = viewangle;
return 0;
}

int _set_view_distance(int vd) {
    view_distance = vd;
    return 0;
}

int _set_fulstrum_culling(int value) {
    fulstrum_culling = value;
    return 0;
}


int crb[3]; //the request buffer

int* _chunk_request() {
    struct vm_map* m;
    struct vm_chunk* ch;
    struct vm_column* col;
    int x; int y;
    struct vm_chunk* ch_lowest = NULL;
    int score, score_lowest;
    int i,j,k;

    score_lowest = -1;

    m = _get_map();
    for(i=0; i<vm_map_dim; i++) {
    for(j=0; j<vm_map_dim;j++) {
        col = &m->column[j*vm_map_dim+i];
        for(k = 0; k < vm_column_max; k++) {
            ch = col->chunk[k];
            if(ch == NULL) { continue; }
            if(ch->server_version == ch->local_version || ch->requested == 1) { continue; }
            x = 8*col->x_off + 4;
            y = 8*col->y_off + 4;
            score = (c.x-x)*(c.x-x) + (c.y-y)*(c.y-y);
            //printf("score= %i, x,y,z= %i, %i, %i \n", score, ch->x_off, ch->y_off, ch->z_off);
            if(score_lowest == -1){
                score_lowest = score;
                ch_lowest = ch;
            }
            if(score < score_lowest) {
                score_lowest = score;
                ch_lowest = ch;
            }
        }
    }}

    if(score_lowest == -1) {
        return NULL;
    } else {
        ch = ch_lowest;
        //printf("score= %i, x,y,z= %i, %i, %i \n", score_lowest, ch->x_off, ch->y_off, ch->z_off);
        ch_lowest->requested = 1;
        crb[0] = ch_lowest->x_off;
        crb[1] = ch_lowest->y_off;
        crb[2] = ch_lowest->z_off;
        return crb;
    }
    return NULL;
}

void _refresh_map_vbo() {
    printf("Refreshing map VBOs\n");
    struct vm_map* m;
    struct vm_column* c;
    int i,j;
    m = _get_map();

    //VBO construction loop

    for(i=0; i<vm_map_dim; i++) {
    for(j=0; j<vm_map_dim;j++) {
        c = &m->column[j*vm_map_dim+i];
        if(flag_is_true(c, VBO_has_blocks)) {
                //c->vbo.VBO_id == 0
                set_flag(c, VBO_needs_update, 1);
                set_flag(c, VBO_loaded, 0);
                //flag_is_false(c, VBO_loaded)
            }

    }}
}
