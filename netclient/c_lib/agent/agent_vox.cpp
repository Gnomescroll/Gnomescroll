#include "agent_vox.hpp"

VoxBody agent_vox_dat;

Agent_vox::Agent_vox(Agent_state* a)
:
a(a)
{
    this->vv.init(5,5,10, 0.1f);
    this->vv.set_unit_axis();
    ClientState::voxel_render_list.register_voxel_volume(&vv);
    ClientState::voxel_render_list.update_vertex_buffer_object();
}

void Agent_vox::update() {

    float x,y,z,theta;
    //,phi;
    x = this->a->s.x;
    y = this->a->s.y;
    z = this->a->s.z;
    theta = this->a->s.theta;
    //phi = this->a->s.phi;

    this->vv.set_center(x,y,z);
    //this->vv.set_rotated_unit_axis(cos(theta*PI), sin(theta*PI), 0.0f);
    
    //struct Vector right;
    //struct Vector look;
    struct Vector forward;

    //look = Vector_init(
        //cos(theta * PI) * cos(phi * PI),
        //sin(theta * PI) * cos(phi * PI),
        //sin(phi)
    //);
    //normalize_vector(&look);

    //right = Vector_init(
        //cos(theta * PI + PI/2), 
        //sin(theta * PI + PI/2),
        //0.0f
    //);
    //normalize_vector(&right);

    forward = Vector_init(
        cos(theta * PI),
        sin(theta * PI),
        0.0f
    );
    normalize_vector(&forward);

    Vector up = {0.0f,0.0f,1.0f};
    this->vv.set_axis(&forward, &up);
}


