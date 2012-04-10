#pragma once

#if DC_CLIENT
	#include <c_lib/camera/camera.hpp>
	#include <c_lib/ray_trace/hitscan.hpp>
#endif

namespace t_mech
{


struct MECH
{
	int x,y,z; //position
	int type;  //type
	int direction; //direction

	bool active;
};


struct MECH mech_array[1024];
int mech_num = 0;

void init() {}
void teardown() {}

//return index or -1

#if DC_CLIENT

/*
struct HitscanBlock {
    int hit;
    int x,y,z;
    float distance;
    int side[3];
    int tile;
};
*/

void add_mech()
{
    Vec3 pos = vec3_init(current_camera->x, current_camera->y, current_camera->z);
    Vec3 vec = current_camera->forward_vector();


	Hitscan::HitscanBlock* h = Hitscan::ray_intersect_block(pos.x,pos.y,pos.z, vec.x,vec.y,vec.z);

	if(!h->hit) return;
	printf("Place: %i %i %i distance: %f \n", h->x,h->y,h->z, h->distance);

	struct MECH m;

	m.x = h->x;
	m.y = h->y;
	m.z = h->z;

	m.type = 0;

	m.direction = 0;

	mech_array[mech_num] = m;
	mech_num++;
}

void rotate_mech()
{
    Vec3 pos = vec3_init(current_camera->x, current_camera->y, current_camera->z);
    Vec3 vec = current_camera->forward_vector();

    Hitscan::HitscanBlock* h;
	h = Hitscan::ray_intersect_block(pos.x,pos.y,pos.z, vec.x,vec.y,vec.z);

	if(!h->hit) return;
	printf("Place: %i %i %i distance: %f \n", h->x,h->y,h->z, h->distance);

	for(int i=0; i<mech_num; i++)
	{
		if(mech_array[i].x == h->x && mech_array[i].y == h->y && mech_array[i].z == h->z)
		{
			mech_array[i].direction = (mech_array[i].direction +1) % 4;
			return;
		}

	}
}

int resolve_position()
{

	//playerAgent_state* p;
	//p = &ClientState::playerAgent_state;

    Vec3 pos = vec3_init(current_camera->x, current_camera->y, current_camera->z);
    Vec3 vec = current_camera->forward_vector();

/*
struct HitscanBlock {
    int hit;
    int x,y,z;
    float distance;
    int side[3];
    int tile;
};

HitscanBlock* ray_intersect_block(float x, float y, float z, float vx, float vy, float vz);
*/
    Hitscan::HitscanBlock* h;
	h = Hitscan::ray_intersect_block(pos.x,pos.y,pos.z, vec.x,vec.y,vec.z);


	if(!h->hit) return -1;

	for(int i=0; i<mech_num; i++)
	{
		if(mech_array[i].x == h->x && mech_array[i].y == h->y && mech_array[i].z == h->z)
		{
			return i;
		}

	}

	return -1;
}
#endif

void add_mech(int x, int y, int z, int direction, int type)
{
	if(mech_num == 1024) return;

	struct MECH m;

	m.x = x;
	m.y = y;
	m.z = z;
	m.direction = direction;
	m.type = type;

	mech_array[mech_num] = m;
	mech_num++;
}

void remove_mech(int x, int y, int z)
{
	for(int i=0; i< mech_num; i++)
	{
		if(mech_array[i].x ==x && mech_array[i].y == y && mech_array[i].z == z)
		{
			if(mech_num >= 1) mech_array[i] = mech_array[mech_num-1];
			mech_num--;
			break;
		}
	}
}

}