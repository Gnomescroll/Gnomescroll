#include "monsters.hpp"

#include <c_lib/state/client_state.hpp>
#include <c_lib/common/random.h>
#include <c_lib/t_map/t_map.hpp>

namespace Monsters {

VoxBody slime_vox_dat;

/* Vox (make this a template or generic base class */

//void Slime_vox::init_parts() {
    //#ifdef DC_CLIENT
    //// create each vox part from slime_vox_dat conf
    //int i,j;
    //unsigned char r,g,b,a;
    //int x,y,z;
    //int ix,iy,iz;
    //VoxPart *vp;
    //float size = slime_vox_dat.vox_size;
    //for (i=0; i<SLIME_PART_NUM; i++) {
        //vp = slime_vox_dat.vox_part[i];
        //x = vp->dimension.x;
        //y = vp->dimension.y;
        //z = vp->dimension.z;

        //this->vv[i].init(x,y,z,size);
        //this->vv[i].set_unit_axis();

        //for (j=0; j<vp->colors.n; j++) {
            //ix = vp->colors.index[j][0];
            //iy = vp->colors.index[j][1];
            //iz = vp->colors.index[j][2];
            //r = vp->colors.rgba[j][0];
            //g = vp->colors.rgba[j][1];
            //b = vp->colors.rgba[j][2];
            //a = vp->colors.rgba[j][3];

            //this->vv[i].set_color(ix, iy, iz, r,g,b,a);
        //}

        //ClientState::voxel_render_list.register_voxel_volume(&(this->vv[i]));
    //}
    //ClientState::voxel_render_list.update_vertex_buffer_object();
    //#endif
//}

//void Slime_vox::set_draw(bool draw) {
    //int i;
    //for (i=0; i<SLIME_PART_NUM; i++) {
        //this->vv[i].draw = draw;
    //}
//}

//void Slime_vox::update() {

    //float x,y,z,theta;
    //x = this->a->x;
    //y = this->a->y;
    //z = this->a->z;
    //theta = this->a->theta;
    
    //struct Vector forward;
    //this->forward(&forward, theta);

    //Vector up = {0.0f,0.0f,1.0f};

    //VoxPart* vp;
    //float ax,ay,az;
    //int i;
    //for (i=0; i<SLIME_PART_NUM; i++) {
        //vp = slime_vox_dat.vox_part[i];
        //ax = vp->anchor.x;
        //ay = vp->anchor.y;
        //az = vp->anchor.z;
        //this->vv[i].set_center(x+ax,y+ay,z+az); // add vox config offsets

        //this->vv[i].set_axis(&forward, &up);
    //}
//}


//void Slime_vox::right(Vector* f, float theta) {
    //f->x = cos(theta * PI + PI/2); 
    //f->y = sin(theta * PI + PI/2);
    //f->z = 0.0f;
    //normalize_vector(f);
//}

//void Slime_vox::forward(Vector* f, float theta) {
    //f->x = cos(theta * PI);
    //f->y = sin(theta * PI);
    //f->z = 0.0f;
    //normalize_vector(f);
//}

//void Slime_vox::look(Vector* f, float theta, float phi) {
    //f->x = cos(theta * PI) * cos(phi * PI);
    //f->y = sin(theta * PI) * cos(phi * PI);
    //f->z = sin(phi);
    //normalize_vector(f);
//}

//Slime_vox::Slime_vox(Slime* a)
//:
//a(a)
//{
    //this->init_parts();
//}


//Slime_vox::~Slime_vox() {
    //#ifdef DC_CLIENT
    //int i;
    //for (i=0; i<SLIME_PART_NUM; i++) {
        //ClientState::voxel_render_list.unregister_voxel_volume(&(this->vv[i]));
    //}
    //#endif
//}

Slime::Slime()
:
theta(0.0f), phi(0.0f)
{
    this->vox = new Slime_vox(this, &slime_vox_dat);
}
Slime::Slime(int id)
:
id(id),
theta(0.0f), phi(0.0f)
{
    this->vox = new Slime_vox(this, &slime_vox_dat);
}
Slime::Slime(float x, float y, float z, float vx, float vy, float vz)
:
x(x), y(y), z(z), vx(vx), vy(vy), vz(vz),
theta(0.0f), phi(0.0f)
{
    this->vox = new Slime_vox(this, &slime_vox_dat);
}
Slime::Slime(int id, float x, float y, float z, float vx, float vy, float vz)
:
id(id), x(x), y(y), z(z), vx(vx), vy(vy), vz(vz),
theta(0.0f), phi(0.0f)
{
    this->vox = new Slime_vox(this, &slime_vox_dat);
}


#ifdef DC_CLIENT
void test() {
    //int n = 30;
    int n = 1;
    int i;
    int x,y,z;
    for (i=0; i<n; i++) {
        x = randrange(0,127);
        y = randrange(0,127);
        z = _get_highest_open_block(x,y);
        Slime* s = ClientState::slime_list.create(x,y,z+1, 0,0,0);
        s->vox->set_draw(true);
        s->vox->update(&slime_vox_dat);
    }
}
#endif


}
