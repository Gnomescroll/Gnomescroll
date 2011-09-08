/*
 *  Client only
 */
#ifdef DC_CLIENT

//GLint particle_sheet_id;

struct p_tree_node {
    int depth;
    int s[3];
    int e[3];
};

struct p_tree_node p_array[2048];
int pt_i = 1;

struct p_tree_node* new_branch(struct p_tree_node* n, int dx,int dy,int dz) {
    if(pt_i >= 2048) return NULL;

    p_array[pt_i].depth = n->depth+1;
    p_array[pt_i].s[0] = n->e[0];
    p_array[pt_i].s[1] = n->e[1];
    p_array[pt_i].s[2] = n->e[1];
    p_array[pt_i].e[0] = n->e[0]+dx;
    p_array[pt_i].e[1] = n->e[1]+dy;
    p_array[pt_i].e[2] = n->e[1]+dz;

    pt_i++;
}

void p_tree_recursive(struct p_tree_node* n) {
    pt_i++;
    if(pt_i >= 2048) return;
    if(n->depth > 3) return;

    int x,y,z

    p = struct p_tree_node* n;
    int i, r;
    r = (rand % 3)+1;
    for(i=0;i<r;i++) {
        n = new_branch(n, x,y,z);
        p_tree_recursive(p);


    }
}

void circuit_tree_generate(int type, int seed) {
    printf("Generated Circuit Tree\n");
    printf("RANDMAX= %i \n", RAND_MAX);
    srand(seed); //seed
    //rand()
    pt_i = 1;

    p_array.depth = 1;
    p_array[0].s = {10,10,8};
    p_array[0].s = {10,10,10};

    p_tree_recursive(&p_array[0]);
}

void circuit_tree_draw() {
    printf("Drawing Circuit Tree\n");
}

/*
void neutron_draw() {
    //printf("particle sheet id= %i \n", get_particle_texture() );
    if(neutron_count == 0) { return; }
    glGetFloatv(GL_MODELVIEW_MATRIX, a);

    struct neutron* g = NULL;
    int i;

    float size = 0.3;
    float up[3] = {a[0]*size, a[4]*size, a[8]*size};
    float right[3] = {a[1]*size, a[5]*size, a[9]*size};
    int id = 5;

    float tx_min, tx_max, ty_min, ty_max;

    //should not change state unless there is something to draw
    glEnable(GL_TEXTURE_2D);
    glEnable (GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);

    glBindTexture( GL_TEXTURE_2D, get_particle_texture() );
    //printf("particle sheet= %i \n", particle_sheet_id);
    glEnable(GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE);

    glBegin( GL_QUADS );
    float x,y,z;

    int _c = 0;
    for(i=0; i<1024; i++) {
    if(neutron_list[i] != NULL) {
        //printf("draw neutron: %i \n", i);
        _c++;
        g = neutron_list[i];
        //draw setup
        id = 48+3*g->type+ (g->energy-1);
        tx_min = (float)(id%16)* (1.0/16.0);
        tx_max = tx_min + (1.0/16.0);
        ty_min = (float)(id/16)* (1.0/16.0);
        ty_max = ty_min + (1.0/16.0);

        x=g->x; y=g->y; z=g->z;

        glTexCoord2f(tx_min,ty_max );
        glVertex3f(x+(-right[0]-up[0]), y+(-right[1]-up[1]), z+(-right[2]-up[2]));  // Bottom left

        glTexCoord2f(tx_min,ty_min );
        glVertex3f(x+(up[0]-right[0]), y+(up[1]-right[1]), z+(up[2]-right[2]));  // Top left

        glTexCoord2f(tx_max,ty_min);
        glVertex3f(x+(up[0]+right[0]), y+(up[1]+right[1]), z+(up[2]+right[2]));  // Top right

        glTexCoord2f(tx_max,ty_max );
        glVertex3f(x+(right[0]-up[0]), y+(right[1]-up[1]), z+(right[2]-up[2]));  // Bottom right
        }
    }
    //printf("drew %i gernades\n", _c);
    glEnd();

    glDepthMask(GL_TRUE);
    glDisable(GL_TEXTURE_2D);
    glDisable (GL_DEPTH_TEST);
    glDisable(GL_BLEND);
}
*/
#endif
