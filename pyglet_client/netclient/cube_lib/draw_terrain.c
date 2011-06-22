#include "./draw_terrain.h"

GLuint texture = 0;
SDL_Surface *surface;

//GLuint VBOid = 0;

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
    glGenTextures( 1, &texture );
    glBindTexture( GL_TEXTURE_2D, texture );

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, 4, surface->w, surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels );
    }
}

/*
    int _create_vbo(struct Quad_VBO* q_VBO, struct Quad* quad_list, int v_num);
    int _delete_vbo(struct Quad_VBO* q_VBO);
    int _draw_vbo(struct Quad_VBO* q_VBO);
*/

//int _create_vbo(struct Quad* quad_list, int v_num) {
int _create_vbo(struct Quad_VBO* q_VBO, struct Quad* quad_list, int v_num) {

    if (v_num == 0) { return 0; }
    q_VBO->quad_array = malloc(v_num*sizeof(struct Quad)); ///dont forget to free this!!!
    q_VBO->v_num = v_num;
    memcpy(q_VBO->quad_array, quad_list, v_num*sizeof(struct Quad));

    GLuint VBO_id;
    glGenBuffers(1, &VBO_id);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_id);

    glBufferData(GL_ARRAY_BUFFER, v_num*sizeof(struct Quad), q_VBO->quad_array, GL_STATIC_DRAW); // size, pointer to array, usecase

    q_VBO->VBO_id = VBO_id;
    return VBO_id;
}


//int _delete_vbo(unsigned int VBO_id)
int _delete_vbo(struct Quad_VBO* q_VBO) {
    glDeleteBuffers(q_VBO->VBO_id);
    ///free the system memory copy of the vertex buffer
    free(q_VBO->quad_array);
    q_VBO->v_num = 0;
    return 0;
}


int draw_mode_enabled = 0;

int _start_vbo_draw() {
draw_mode_enabled = 1;

glEnable(GL_TEXTURE_2D);
glEnable (GL_DEPTH_TEST);
//glEnable(GL_CULL_FACE);

//glBindTexture( GL_TEXTURE_2D, texture ); //needed?

glClientActiveTexture(texture);

glEnableClientState(GL_VERTEX_ARRAY);
glClientActiveTexture(texture); //bind texture
glEnableClientState(GL_TEXTURE_COORD_ARRAY);
glEnableClientState(GL_COLOR_ARRAY);
/*
glVertexPointer(3, GL_FLOAT, sizeof(struct Vertex), 0);
glTexCoordPointer(2, GL_FLOAT, sizeof(struct Vertex), 12);
glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(struct Vertex), 20);
*/
return 0;
}

int _end_vbo_draw() {
draw_mode_enabled = 0;
glDisableClientState(GL_VERTEX_ARRAY);
glDisableClientState(GL_COLOR_ARRAY);
glDisableClientState(GL_TEXTURE_COORD_ARRAY);
return 0;
}



//int _draw_vbo(unsigned int VBO_id, int v_num) {
int _draw_vbo(struct Quad_VBO* q_VBO) {

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

    glDrawArrays(GL_QUADS,0, q_VBO->v_num*4);
}

/*
glEnable(GL_TEXTURE_2D);
glEnable (GL_DEPTH_TEST);
//glEnable(GL_CULL_FACE);

//glBindTexture( GL_TEXTURE_2D, texture ); //needed?

//glBindBuffer(GL_ARRAY_BUFFER, q_VBO->VBO_id);

glEnableClientState (GL_VERTEX_ARRAY);
glVertexPointer(3, GL_FLOAT, sizeof(struct Vertex), 0);

glClientActiveTexture(texture); //bind texture
glEnableClientState(GL_TEXTURE_COORD_ARRAY);
glTexCoordPointer(2, GL_FLOAT, sizeof(struct Vertex), 12);

glEnableClientState(GL_COLOR_ARRAY);
glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(struct Vertex), 20);
*/
return 0;
}

int print_vertex(struct Vertex* v) {
printf("x,y,z= %f, %f, %f\n", v->x,v->y,v->z);
printf("tx,ty= %f, %f\n", v->tx, v->ty);
printf("r,g,b,a= %i, %i, %i, %i\n", v->r,v->g,v->b,v->a);
printf("\n");
return 0;
}

/*
int _bind_VBO(struct Quad* quad_list, int v_num) {

if(0) {
    printf("v_num= %i \n", v_num);
    vertex_num = 0;
    int i,j;
    for(i=0; i<1; i++){
        for(j=0; j<4; j++){
        print_vertex(&quad_list[i].vertex[j]);
        }
    }
}

glEnable(GL_TEXTURE_2D);
glEnable (GL_DEPTH_TEST);
glEnable(GL_CULL_FACE);
//glBindTexture( GL_TEXTURE_2D, texture ); /// ??? needed ???
glBindBuffer(GL_ARRAY_BUFFER, VBOid);

glEnableClientState(GL_VERTEX_ARRAY);
glVertexPointer(3, GL_FLOAT, sizeof(struct Vertex), 0);

glClientActiveTexture(texture); //bind texture
glEnableClientState(GL_TEXTURE_COORD_ARRAY);
glTexCoordPointer(2, GL_FLOAT, sizeof(struct Vertex), 12);

glEnableClientState(GL_COLOR_ARRAY);
glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(struct Vertex), 20);


//glVertexPointer(3, GL_FLOAT, sizeof(struct Vertex), 0);
//glTexCoordPointer(2, GL_FLOAT, sizeof(struct Vertex), 12); //12 bytes in
//glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(struct Vertex), 20);

glDrawArrays(GL_QUADS,0, v_num*4);

glDisableClientState(GL_VERTEX_ARRAY);
glDisableClientState(GL_COLOR_ARRAY);
glDisableClientState(GL_TEXTURE_COORD_ARRAY);

//printf("SDL_error: %s\n", SDL_GetError());
//    printf("finished\n");
///test
    return 0;
}
*/
