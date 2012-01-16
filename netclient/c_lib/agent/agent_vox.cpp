#include "agent_vox.hpp"

VoxBody agent_vox_dat;

void Agent_vox::update(VoxBody* vox_dat) {

    float x,y,z,theta,phi;
    x = this->a->s.x;
    y = this->a->s.y;
    z = this->a->s.z;
    theta = this->a->s.theta;
    phi = this->a->s.phi;

    struct Vector forward;
    this->forward(&forward, theta);

    Vector up = {0.0f,0.0f,1.0f};

    VoxPart* vp;
    float ax,ay,az;
    int i;
    for (i=0; i<this->n_parts; i++) {
        vp = vox_dat->vox_part[i];
        ax = vp->anchor.x;
        ay = vp->anchor.y;
        az = vp->anchor.z;
        this->vv[i].set_center(x+ax,y+ay,z+az); // add vox config offsets

        if (vp->biaxial) {
            //this->vv[i].set_rotated_unit_axis(0.0f, phi, theta);
            this->vv[i].set_rotated_unit_axis(theta, phi, 0.0f);
        } else {
            this->vv[i].set_axis(&forward, &up);
        }
    }
}
