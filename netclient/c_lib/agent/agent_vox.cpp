#include "agent_vox.hpp"

VoxBody agent_vox_dat;

void Agent_vox::init() {
    if (agent_vox_dat.vox_size == 0.0f) {
        printf("Agent_vox::init() error. agent_vox_dat is not initialized");
        return;
    }

    VoxPart* part;

    Vox* v;
    int i;

    for (i=0; i<AGENT_PART_NUM; i++) {
        part = agent_vox_dat.vox_part[i];

        // create and assign vox part
        v = new Vox(i, part->dimension.x, part->dimension.y, part->dimension.z, agent_vox_dat.vox_size);
        vox_part[i] = v;
        
        // apply dats
        v->set_base_anchor_point(part->anchor.length, part->anchor.x, part->anchor.y, part->anchor.z);
        v->set_direction(
            part->rotation.fx, part->rotation.fy, part->rotation.fz,
            part->rotation.nx, part->rotation.ny, part->rotation.nz
        );

        // set color
        v->set_color(255,10,10,255);
    }
}

void Agent_vox::init_vox_part(int part, int xdim, int ydim, int zdim, float vosize) {
    if(vox_part[part] != NULL) {
        delete vox_part[part];
        vox_part[part] = NULL;
    }

    Vox* v = new Vox(part, xdim, ydim, zdim, vosize);
    vox_part[part] = v;
}

void Agent_vox::set_limb_anchor_point(int part, float ax, float ay, float az) {
    Vox* v = vox_part[part];
    if(v == NULL) {
        printf("set_limb_anchor_point: Vox is Null!\n");
        return;
    }
    v->set_anchor_point(ax, ay, az);
}

void Agent_vox::set_limb_base_anchor_point(int part, float len, float ax, float ay, float az) {
    Vox* v = vox_part[part];
    if(v == NULL) {
        printf("set_limb_base_anchor_point: Vox is Null!\n");
        return;
    }
    v->set_base_anchor_point(len, ax, ay, az);
}

void Vox::set_anchor_point(float ax, float ay, float az) {
    anchor.x = ax;
    anchor.y = ay;
    anchor.z = az;
}

void Vox::set_base_anchor_point(float len, float ax, float ay, float az) {
    length = len;
    a.x = ax;
    a.y = ay;
    a.z = az;
}

void Agent_vox::set_limb_direction(int part, float fx,float fy,float fz, float nx,float ny, float nz) {
    Vox* v = vox_part[part];
    if(v == NULL) {
        printf("set_agent_box_anchor_point: Vox is Null!\n");
        return;
    }
    v->set_direction(fx,fy,fz, nx,ny,nz);
}

void Vox::set_direction(float fx, float fy, float fz, float nx, float ny, float nz) {
    f = Vector_init(fx,fy,fz);
    normalize_vector(&f);
    n = Vector_init(nx,ny,nz);
    normalize_vector(&n);

    struct Vector vx,vy,vz;
    vx = f;
    vy = n;

    vz = vector_cross(vx,vy);
    vy = vector_cross(vx, vz);

    f = vx;
    n = vy;
    u = vz;
}

void Agent_vox::set_vox_volume(int part, int x, int y, int z, int r, int g, int b, int a) {
    Vox* v = vox_part[part];
    if (v == NULL) return;
    v->set_volume(x,y,z, r,g,b,a);
}

void Vox::set_volume(int x, int y, int z, int r, int g, int b, int a) {
    if(x + y*xdim + z*xdim*ydim >= (int)num_vox) {
        printf("Warning! %i, %i, %i, %i ; %i, %i, %i; %i \n", xdim, ydim, zdim, num_vox, x,y,z, x + y*xdim + z*xdim*ydim);
        //return;
    }
    if(x<0 || y <0 || z < 0) {
        printf("WTF!!! Warning!\n");
        return;
    }
    if(x >= xdim || y >= ydim || z >= zdim) {
        printf("horrible error!\n");
        return;
    }

    union Voxel* vo = &vox[x + y*xdim + z*xdim*ydim];
    if(vo == NULL) {
        printf("set_agent_vox_volume: null pointer \n");
        return;
    }
    vo->r =r;
    vo->g =g;
    vo->b =b;
    vo->a =a;
}

void Vox::set_color(int r, int g, int b, int a) {
    int n,j;
    n = xdim*ydim*zdim;
    union Voxel* vo;
    for (j=0; j<n; j++) {
        vo = &vox[j];
        vo->r = r;
        vo->g = g;
        vo->b = b;
        vo->a = a;
    }
}

void Vox::set_color(int x, int y, int z, int r, int g, int b, int a) {
    union Voxel* vo = &vox[x + y*xdim + z*xdim*ydim];
    if (vo==NULL) {
        printf("set_agent_vox_volume: null ptr\n");
        return;
    }
    vo->r = r;
    vo->g = g;
    vo->b = b;
    vo->a = a;
}

/*
 *  Client only
 */

#ifdef DC_CLIENT

void Agent_vox::draw(float x, float y, float z, float theta, float phi) {
    struct Vector right;
    struct Vector look;
    struct Vector forward;

    look = Vector_init(
        cos(theta * PI) * cos(phi * PI),
        sin(theta * PI) * cos(phi * PI),
        sin(phi)
    );
    normalize_vector(&look);

    right = Vector_init(
        cos(theta * PI + PI/2), 
        sin(theta * PI + PI/2),
        0.0f
    );
    normalize_vector(&right);

    forward = Vector_init(
        cos(theta * PI),
        sin(theta * PI),
        0.0f
    );
    normalize_vector(&forward);

    int i;
    for (i=0; i<AGENT_PART_NUM; i++) {
        if (vox_part[i] != NULL) {

            vox_part[i]->rotate_anchor(theta);

            switch (i) {
                case AGENT_PART_HEAD:
                    vox_part[i]->draw_head(look, right, x,y,z);
                    break;
                default:
                    vox_part[i]->draw(forward, right, x, y, z);
                    break;
            }
        }
    }
}

void Vox::rotate_anchor(float theta) {
    set_anchor_point(
        a.x * cos(theta * PI) - a.y * sin(theta * PI),
        a.x * sin(theta * PI) + a.y * cos(theta * PI),
        a.z
    );
}


void Vox::draw_head(struct Vector look, struct Vector right, float x, float y, float z) {

    int i,j,k;
    int i1;
    int index;

    float vos = vox_size;

    struct Vector vx,vy,vz;

    vx = look; //instead of look direction
    vz = vector_cross(vx, right);
    vy = vector_cross(vx, vz);

    //save 3 multiplications per voxel by premultiplying in vox
    vx.x *= vos; vx.y *= vos; vx.z *= vos;
    vy.x *= vos; vy.y *= vos; vy.z *= vos;
    vz.x *= vos; vz.y *= vos; vz.z *= vos;

    for(i=0; i<8; i++) {
        v_buffer[3*i+0] = v_set[3*i+0]*vx.x + v_set[3*i+1]*vy.x + v_set[3*i+2]*vz.x;
        v_buffer[3*i+1] = v_set[3*i+0]*vx.y + v_set[3*i+1]*vy.y + v_set[3*i+2]*vz.y;
        v_buffer[3*i+2] = v_set[3*i+0]*vx.z + v_set[3*i+1]*vy.z + v_set[3*i+2]*vz.z;
    }
    for(i=0; i<6; i++) {
        for(j=0; j<4; j++) {
            s_buffer[12*i+3*j+0] = v_buffer[3*q_set[4*i+j] + 0];
            s_buffer[12*i+3*j+1] = v_buffer[3*q_set[4*i+j] + 1];
            s_buffer[12*i+3*j+2] = v_buffer[3*q_set[4*i+j] + 2];
        }
    }

    struct Vector c;
    float cx,cy,cz;
    c.x = anchor.x + length*look.x;
    c.y = anchor.y + length*look.y;
    c.z = anchor.z + length*look.z;

    c.x += x;
    c.y += y;
    c.z += z;

    cx = c.x - (xdim*vx.x + ydim*vy.x + zdim*vz.x)/2;
    cy = c.y - (xdim*vx.y + ydim*vy.y + zdim*vz.y)/2;
    cz = c.z - (xdim*vx.z + ydim*vy.z + zdim*vz.z)/2;

    float x0, y0, z0;

    for(i= 0; i < xdim; i++) {
    for(j= 0; j < ydim; j++) {
    for(k= 0; k < zdim; k++) {
    index = i + j*xdim + k*xdim*ydim; //malloc problem?

    if(vox[index].a == 0) continue;
    glColor3ub((unsigned char) vox[index].r,(unsigned char)vox[index].g,(unsigned char)vox[index].b);

    x0 = cx + (i*vx.x + j*vy.x + k*vz.x);
    y0 = cy + (i*vx.y + j*vy.y + k*vz.y);
    z0 = cz + (i*vx.z + j*vy.z + k*vz.z);

    for(i1=0; i1<6; i1++) {
        glVertex3f(x0 + s_buffer[12*i1+3*0+0], y0+ s_buffer[12*i1+3*0+1], z0+ s_buffer[12*i1+3*0+2]);
        glVertex3f(x0 + s_buffer[12*i1+3*1+0], y0+ s_buffer[12*i1+3*1+1], z0+ s_buffer[12*i1+3*1+2]);
        glVertex3f(x0 + s_buffer[12*i1+3*2+0], y0+ s_buffer[12*i1+3*2+1], z0+ s_buffer[12*i1+3*2+2]);
        glVertex3f(x0 + s_buffer[12*i1+3*3+0], y0+ s_buffer[12*i1+3*3+1], z0+ s_buffer[12*i1+3*3+2]);
    }

    }}}
}

void Vox::draw(struct Vector forward, struct Vector right, float x, float y, float z) {

    int i,j,k;
    int i1;
    int index;

    float vos = vox_size;

    struct Vector vx,vy,vz;

    vx = forward;
    //vx = f;
    vz = vector_cross(vx, right);
    vy = vector_cross(vx, vz);

    //save 3 multiplications per voxel by premultiplying in vox
    vx.x *= vos; vx.y *= vos; vx.z *= vos;
    vy.x *= vos; vy.y *= vos; vy.z *= vos;
    vz.x *= vos; vz.y *= vos; vz.z *= vos;

    for(i=0; i<8; i++) {
        v_buffer[3*i+0] = v_set[3*i+0]*vx.x + v_set[3*i+1]*vy.x + v_set[3*i+2]*vz.x;
        v_buffer[3*i+1] = v_set[3*i+0]*vx.y + v_set[3*i+1]*vy.y + v_set[3*i+2]*vz.y;
        v_buffer[3*i+2] = v_set[3*i+0]*vx.z + v_set[3*i+1]*vy.z + v_set[3*i+2]*vz.z;
    }
    for(i=0; i<6; i++) {
        for(j=0; j<4; j++) {
            s_buffer[12*i+3*j+0] = v_buffer[3*q_set[4*i+j] + 0];
            s_buffer[12*i+3*j+1] = v_buffer[3*q_set[4*i+j] + 1];
            s_buffer[12*i+3*j+2] = v_buffer[3*q_set[4*i+j] + 2];
        }
    }

    struct Vector c;
    float cx,cy,cz;
    c.x = anchor.x + length*f.x;
    c.y = anchor.y + length*f.y;
    c.z = anchor.z + length*f.z;

    c.x += x;
    c.y += y;
    c.z += z;

    cx = c.x - (xdim*vx.x + ydim*vy.x + zdim*vz.x)/2;
    cy = c.y - (xdim*vx.y + ydim*vy.y + zdim*vz.y)/2;
    cz = c.z - (xdim*vx.z + ydim*vy.z + zdim*vz.z)/2;

    float x0, y0, z0;

    for(i= 0; i < xdim; i++) {
    for(j= 0; j < ydim; j++) {
    for(k= 0; k < zdim; k++) {
    index = i + j*xdim + k*xdim*ydim; //malloc problem?

    if(vox[index].a == 0) continue;
    glColor3ub((unsigned char) vox[index].r,(unsigned char)vox[index].g,(unsigned char)vox[index].b);

    x0 = cx + (i*vx.x + j*vy.x + k*vz.x);
    y0 = cy + (i*vx.y + j*vy.y + k*vz.y);
    z0 = cz + (i*vx.z + j*vy.z + k*vz.z);

    for(i1=0; i1<6; i1++) {
        glVertex3f(x0 + s_buffer[12*i1+3*0+0], y0+ s_buffer[12*i1+3*0+1], z0+ s_buffer[12*i1+3*0+2]);
        glVertex3f(x0 + s_buffer[12*i1+3*1+0], y0+ s_buffer[12*i1+3*1+1], z0+ s_buffer[12*i1+3*1+2]);
        glVertex3f(x0 + s_buffer[12*i1+3*2+0], y0+ s_buffer[12*i1+3*2+1], z0+ s_buffer[12*i1+3*2+2]);
        glVertex3f(x0 + s_buffer[12*i1+3*3+0], y0+ s_buffer[12*i1+3*3+1], z0+ s_buffer[12*i1+3*3+2]);
    }

    }}}
}

#endif


